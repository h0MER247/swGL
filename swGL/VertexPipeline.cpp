#include <cmath>
#include "Context.h"
#include "MatrixStack.h"
#include "VertexPipeline.h"

namespace SWGL {

    VertexPipeline::VertexPipeline()
    
        : m_isInsideGLBegin(false),
          m_texCoordGen(m_vertexState, m_matrixStack),
          m_vertexDataArray(m_vertexState) {

        setNormal(Vector(0.0f, 0.0f, 1.0f, 0.0f));
        setPrimaryColor(Vector(1.0f, 1.0f, 1.0f, 1.0f));
        for (auto i = 0U; i < SWGL_MAX_TEXTURE_UNITS; i++) {

            setTexCoord(i, Vector(0.0f, 0.0f, 0.0f, 1.0f));
        }
    }



    void VertexPipeline::begin(GLenum primitiveType) {

        m_primitiveType = primitiveType;
        m_isInsideGLBegin = true;

        // Update matrices
        bool wasProjMatUpdated = m_matrixStack.wasMatrixStackUpdated(MatrixStack::STACK_PROJECTION);
        bool wasModelViewMatUpdated = m_matrixStack.wasMatrixStackUpdated(MatrixStack::STACK_MODELVIEW);

        if (wasProjMatUpdated || wasModelViewMatUpdated) {

            m_mvpMatrix = m_matrixStack.getProjectionMatrix() *
                          m_matrixStack.getModelViewMatrix();
        }

        if (m_clipper.isAnyUserPlaneEnabled() && wasProjMatUpdated) {

            m_clipper.updateUserPlanes(
            
                m_matrixStack.getProjectionMatrix()
            );
        }
    }

    void VertexPipeline::end() {

        bool flipFlop = true;

        switch (m_primitiveType) {

        case GL_POINTS:
            LOG("Unimplemented primitive type: GL_POINTS");
            break;

        case GL_LINE_LOOP:
            if (m_vertices.size() < 2) { break; }
            for (auto i = 0U, n = m_vertices.size() - 1; i <= n; i++) {

                Vertex &v1 = m_vertices[i];
                Vertex &v2 = i == n ? m_vertices[0] : m_vertices[i + 1U];
                
                addLine(v1, v2);
            }
            break;

        case GL_LINE_STRIP:
            if (m_vertices.size() < 2) { break; }
            for (auto i = 0U, n = m_vertices.size() - 1; i < n; i++) {

                Vertex &v1 = m_vertices[i];
                Vertex &v2 = m_vertices[i + 1];

                addLine(v1, v2);
            }
            break;

        case GL_LINES:
            if (m_vertices.size() < 2) { break; }
            for (auto i = 0U, n = m_vertices.size(); i < n; i += 2) {

                Vertex &v1 = m_vertices[i];
                Vertex &v2 = m_vertices[i + 1];

                addLine(v1, v2);
            }
            break;

        case GL_TRIANGLES:
            if (m_vertices.size() < 3) { break; }
            for (auto i = 0U, n = m_vertices.size() - 2; i < n; i += 3) {

                Vertex &v1 = m_vertices[i];
                Vertex &v2 = m_vertices[i + 1];
                Vertex &v3 = m_vertices[i + 2];

                addTriangle(v1, v2, v3);
            }
            break;

        case GL_TRIANGLE_STRIP:
            if (m_vertices.size() < 3) { break; }
            for (auto i = 0U, n = m_vertices.size() - 2; i < n; i++) {

                Vertex &v1 = m_vertices[i];
                Vertex &v2 = m_vertices[i + 1];
                Vertex &v3 = m_vertices[i + 2];

                if (flipFlop)
                    addTriangle(v1, v2, v3);
                else
                    addTriangle(v1, v3, v2);

                flipFlop ^= true;
            }
            break;

        case GL_TRIANGLE_FAN:
        case GL_POLYGON:
            if (m_vertices.size() < 3) { break; }
            for (auto i = 1U, n = m_vertices.size() - 1U; i < n; i++) {

                Vertex &v1 = m_vertices[0];
                Vertex &v2 = m_vertices[i];
                Vertex &v3 = m_vertices[i + 1];

                addTriangle(v1, v2, v3);
            }
            break;

        case GL_QUADS:
            if (m_vertices.size() < 4) { break; }
            for (auto i = 0U, n = m_vertices.size() - 3; i < n; i += 4) {

                Vertex &v1 = m_vertices[i];
                Vertex &v2 = m_vertices[i + 1];
                Vertex &v3 = m_vertices[i + 2];
                Vertex &v4 = m_vertices[i + 3];

                addTriangle(v1, v2, v3);
                addTriangle(v3, v4, v1);
            }
            break;

        case GL_QUAD_STRIP:
            if (m_vertices.size() < 4) { break; }
            for (auto i = 0U, n = m_vertices.size() - 3; i < n; i += 2) {

                Vertex &v1 = m_vertices[i];
                Vertex &v2 = m_vertices[i + 1];
                Vertex &v3 = m_vertices[i + 2];
                Vertex &v4 = m_vertices[i + 3];

                addTriangle(v1, v2, v4);
                addTriangle(v4, v2, v3);
            }
            break;
        }

        if (!m_triangles.empty()) {
        
            drawTriangles();
            m_triangles.clear();
        }
        m_vertices.clear();

        m_isInsideGLBegin = false;
    }

    void VertexPipeline::addTriangle(Vertex &v1, Vertex &v2, Vertex &v3) {

        if (m_culling.isTriangleVisible(v1, v2, v3)) {
        
            m_triangles.emplace_back(Triangle(v1, v2, v3));
        }
    }

    void VertexPipeline::addLine(Vertex &v1, Vertex &v2) {

        // Get the reciprocal viewport scaling factor
        float rcpViewportScaleX = 2.0f / static_cast<float>(m_viewport.getWidth());
        float rcpViewportScaleY = 2.0f / static_cast<float>(m_viewport.getHeight());

        // Calculate the lines normal n = normalize(-dx, dy)
        float nx = -((v2.projected.y() / v2.projected.w()) - (v1.projected.y() / v1.projected.w()));
        float ny =  ((v2.projected.x() / v2.projected.w()) - (v1.projected.x() / v1.projected.w()));
        float rcpLength = 1.0f / std::sqrt(nx * nx + ny * ny);
        nx *= rcpLength;
        ny *= rcpLength;

        // Scale normal according to the width of the line
        float halfLineWidth = 1.0f * 0.5f;
        nx *= halfLineWidth;
        ny *= halfLineWidth;

        // Now create the vertices that define two triangles which are used to draw the line
        Vertex v[4] { v1, v1, v2, v2 };

        v[0].projected.x() += ( nx * v1.projected.w()) * rcpViewportScaleX;
        v[0].projected.y() += ( ny * v1.projected.w()) * rcpViewportScaleY;
        v[1].projected.x() += (-nx * v1.projected.w()) * rcpViewportScaleX;
        v[1].projected.y() += (-ny * v1.projected.w()) * rcpViewportScaleY;

        v[2].projected.x() += ( nx * v2.projected.w()) * rcpViewportScaleX;
        v[2].projected.y() += ( ny * v2.projected.w()) * rcpViewportScaleY;
        v[3].projected.x() += (-nx * v2.projected.w()) * rcpViewportScaleX;
        v[3].projected.y() += (-ny * v2.projected.w()) * rcpViewportScaleY;

        addTriangle(v[0], v[1], v[2]);
        addTriangle(v[2], v[1], v[3]);
    }



    void VertexPipeline::setPrimaryColor(const Vector &color) {

        m_vertexState.colorPrimary = color;
    }

    void VertexPipeline::setTexCoord(unsigned int index, const Vector &texCoord) {

        m_vertexState.texCoord[index] = texCoord;
    }

    void VertexPipeline::setNormal(const Vector &normal) {

        m_vertexState.normal = normal * m_matrixStack.getModelViewMatrix();
    }

    void VertexPipeline::setPosition(const Vector &position) {

        m_vertexState.position = position;
        m_vertexState.projected = position * m_mvpMatrix;
    }

    void VertexPipeline::addVertex() {

        if (m_texCoordGen.isEnabled()) {

            m_texCoordGen.generate();
        }

        // TODO: Only execute this if needed
        for (auto i = 0U; i < SWGL_MAX_TEXTURE_UNITS; i++) {
        
            m_vertexState.texCoord[i] = m_vertexState.texCoord[i] * m_matrixStack.getTextureMatrix(i);
        }

        m_vertices.emplace_back(m_vertexState);
    }

    void VertexPipeline::setArrayElement(unsigned int idx) {

        if (m_vertexDataArray.getColor().isEnabled()) {

            setPrimaryColor(m_vertexDataArray.getColor().read(idx));
        }

        if (m_vertexDataArray.getNormal().isEnabled()) {

            setNormal(m_vertexDataArray.getNormal().read(idx));
        }

        for (auto i = 0U; i < SWGL_MAX_TEXTURE_UNITS; i++) {

            auto &texCoord = m_vertexDataArray.getTexCoord(i);
            if (texCoord.isEnabled()) {

                setTexCoord(i, texCoord.read(idx));
            }
        }

        if (m_vertexDataArray.getPosition().isEnabled()) {

            setPosition(m_vertexDataArray.getPosition().read(idx));
            addVertex();
        }
    }



    void VertexPipeline::lockArrayElements(unsigned int firstIndex, unsigned int count) {

        m_vertexDataArray.lock(firstIndex, count);
    }

    void VertexPipeline::unlockArrayElements() {

        m_vertexDataArray.unlock();
    }



    void VertexPipeline::drawIndexedArrayElements(GLenum mode, int count, GLenum type, const GLvoid *indices) {

        begin(mode); {

            m_vertexDataArray.prefetch(m_mvpMatrix, m_matrixStack.getModelViewMatrix());

            switch (type) {

            case GL_UNSIGNED_BYTE:
                for (int i = 0; i < count; i++) {

                    auto index = static_cast<unsigned int>(reinterpret_cast<const unsigned char *>(indices)[i]);

                    if (m_vertexDataArray.getPrefetchedVertex(index)) {
                    
                        addVertex();
                    }
                    else {

                        setArrayElement(index);
                    }
                }
                break;

            case GL_UNSIGNED_SHORT:
                for (int i = 0; i < count; i++) {

                    auto index = static_cast<unsigned int>(reinterpret_cast<const unsigned short *>(indices)[i]);

                    if (m_vertexDataArray.getPrefetchedVertex(index)) {
                    
                        addVertex();
                    }
                    else {

                        setArrayElement(index);
                    }
                }
                break;

            case GL_UNSIGNED_INT:
                for (int i = 0; i < count; i++) {

                    auto index = reinterpret_cast<const unsigned int *>(indices)[i];

                    if (m_vertexDataArray.getPrefetchedVertex(index)) {

                        addVertex();
                    }
                    else {

                        setArrayElement(index);
                    }
                }
                break;
            }
        }
        end();
    }

    void VertexPipeline::drawArrayElements(GLenum mode, int first, int count) {

        begin(mode); {

            m_vertexDataArray.prefetch(m_mvpMatrix, m_matrixStack.getModelViewMatrix());

            for (auto i = first, n = first + count; i < n; i++) {

                if (m_vertexDataArray.getPrefetchedVertex(i)) {

                    addVertex();
                }
                else {

                    setArrayElement(i);
                }
            }
        }
        end();
    }



    void VertexPipeline::drawTriangles() {

        if (m_lighting.isEnabled()) {

            m_lighting.calculateLighting(

                m_triangles,
                m_matrixStack.getModelViewMatrix()
            );
        }

        m_clipper.clipTriangles(m_triangles);

        if (!m_triangles.empty()) {

            for (auto &t : m_triangles) {

                for (int i = 0; i < 3; i++) {

                    auto &proj = t.v[i].projected;
                    auto &raster = t.v[i].position;

                    // Perspective division
                    auto rhw = 1.0f / proj.w();

                    raster.w() = rhw;
                    raster.z() = proj.z() * rhw;
                    raster.y() = proj.y() * rhw;
                    raster.x() = proj.x() * rhw;

                    t.v[i].colorPrimary *= rhw;
                    t.v[i].colorSecondary *= rhw;
                    for (auto j = 0U; j < SWGL_MAX_TEXTURE_UNITS; j++) {

                        t.v[i].texCoord[j] *= rhw;
                    }

                    // Viewport transformation
                    m_viewport.transform(raster);
                }
            }

            Context::getCurrentContext()->getRenderer().drawTriangles(m_triangles);
        }
    }
}
