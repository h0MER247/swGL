#include <cmath>
#include "Context.h"
#include "MatrixStack.h"
#include "VertexPipeline.h"

namespace SWGL {

    VertexPipeline::VertexPipeline()
    
        : m_isInsideGLBegin(false),
          m_texCoordGen(m_vertexState) {

        setActiveTexture(0U);
        setColor(Vector(1.0f, 1.0f, 1.0f, 1.0f));
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
        case GL_LINES:
        case GL_LINE_LOOP:
        case GL_LINE_STRIP:
            LOG("Unimplemented primitive type: %04x", m_primitiveType);
            break;

        case GL_TRIANGLES:
            if (m_vertices.size() < 3) break;
            for (size_t i = 0, n = m_vertices.size() - 2; i < n; i += 3) {

                Vertex &v1 = m_vertices[i + 0];
                Vertex &v2 = m_vertices[i + 1];
                Vertex &v3 = m_vertices[i + 2];

                addTriangle(v1, v2, v3);
            }
            break;

        case GL_TRIANGLE_STRIP:
            if (m_vertices.size() < 3) break;
            for (size_t i = 0, n = m_vertices.size() - 2; i < n; i++) {

                Vertex &v1 = m_vertices[i + 0];
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
            if (m_vertices.size() < 3) break;
            for (size_t i = 1, n = m_vertices.size() - 1; i < n; i++) {

                Vertex &v1 = m_vertices[0];
                Vertex &v2 = m_vertices[i];
                Vertex &v3 = m_vertices[i + 1];

                addTriangle(v1, v2, v3);
            }
            break;

        case GL_QUADS:
            if (m_vertices.size() < 4) break;
            for (size_t i = 0, n = m_vertices.size() - 3; i < n; i += 4) {

                Vertex &v1 = m_vertices[i + 0];
                Vertex &v2 = m_vertices[i + 1];
                Vertex &v3 = m_vertices[i + 2];
                Vertex &v4 = m_vertices[i + 3];

                addTriangle(v1, v2, v3);
                addTriangle(v3, v4, v1);
            }
            break;

        case GL_QUAD_STRIP:
            if (m_vertices.size() < 4) break;
            for (size_t i = 0, n = m_vertices.size() - 3; i < n; i += 2) {

                Vertex &v1 = m_vertices[i + 0];
                Vertex &v2 = m_vertices[i + 1];
                Vertex &v3 = m_vertices[i + 2];
                Vertex &v4 = m_vertices[i + 3];

                addTriangle(v1, v2, v4);
                addTriangle(v4, v2, v3);
            }
            break;
        }

        if (m_triangles.size() > 0) {

            drawTriangles();
            m_triangles.clear();
        }
        m_vertices.clear();

        m_isInsideGLBegin = false;
    }

    void VertexPipeline::addTriangle(Vertex &v1, Vertex &v2, Vertex &v3) {

        if (Context::getCurrentContext()->getCulling().isTriangleVisible(v1, v2, v3)) {
        
            m_triangles.emplace_back(Triangle(v1, v2, v3));
        }
    }



    void VertexPipeline::setColor(const Vector &color) {

        m_vertexState.color = color;
    }

    void VertexPipeline::setTexCoord(size_t index, const Vector &texCoord) {

        m_vertexState.texCoord[index] = texCoord;
    }

    void VertexPipeline::setPosition(const Vector &position) {

        m_vertexState.obj = position;
        m_vertexState.proj = position * m_mvpMatrix;
    }

    void VertexPipeline::addVertex() {

        if (m_texCoordGen.isEnabled()) {

            m_texCoordGen.generate();
        }

        m_vertices.emplace_back(m_vertexState);
    }

    void VertexPipeline::setActiveTexture(size_t unit) {

        m_activeTexture = unit;
    }

    void VertexPipeline::setArrayElement(int idx) {

        if (m_vertexDataArray.getColor().isEnabled()) {

            setColor(m_vertexDataArray.getColor().read(idx));
        }

        for (int i = 0; i < SWGL_MAX_TEXTURE_UNITS; i++) {

            if (m_vertexDataArray.getTexCoord(i).isEnabled()) {

                setTexCoord(i, m_vertexDataArray.getTexCoord(i).read(idx));
            }
        }

        if (m_vertexDataArray.getPosition().isEnabled()) {

            setPosition(m_vertexDataArray.getPosition().read(idx));
            addVertex();
        }
    }



    void VertexPipeline::lockArrayElements(int firstIndex, int count) {

        m_vertexDataArray.lock(firstIndex, count);
    }

    void VertexPipeline::unlockArrayElements() {

        m_vertexDataArray.unlock();
    }

    void VertexPipeline::drawIndexedArrayElements(GLenum mode, int count, GLenum type, const GLvoid *indices) {

        begin(mode); {

            m_vertexDataArray.prefetch(m_mvpMatrix, m_vertexState);

            switch (type) {

            case GL_UNSIGNED_BYTE:
                for (int i = 0; i < count; i++) {

                    unsigned char index = reinterpret_cast<const unsigned char *>(indices)[i];

                    if (m_vertexDataArray.getPrefetchedVertex(index, m_vertexState)) {
                    
                        addVertex();
                    }
                    else {

                        setArrayElement(index);
                    }
                }
                break;

            case GL_UNSIGNED_SHORT:
                for (int i = 0; i < count; i++) {

                    unsigned short index = reinterpret_cast<const unsigned short *>(indices)[i];

                    if (m_vertexDataArray.getPrefetchedVertex(index, m_vertexState)) {
                    
                        addVertex();
                    }
                    else {

                        setArrayElement(index);
                    }
                }
                break;

            case GL_UNSIGNED_INT:
                for (int i = 0; i < count; i++) {

                    unsigned int index = reinterpret_cast<const unsigned int *>(indices)[i];

                    if (m_vertexDataArray.getPrefetchedVertex(index, m_vertexState)) {

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

    void VertexPipeline::drawArrayElements(GLenum mode, GLint first, GLsizei count) {

        begin(mode); {

            m_vertexDataArray.prefetch(m_mvpMatrix, m_vertexState);

            for (int i = first, n = first + count; i < n; i++) {

                if (m_vertexDataArray.getPrefetchedVertex(i, m_vertexState)) {

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

        m_clipper.clipTriangles(m_triangles);

        if (!m_triangles.empty()) {

            auto &viewport = Context::getCurrentContext()->getViewport();
            auto &culling = Context::getCurrentContext()->getCulling();

            for (auto &t : m_triangles) {

                for (int i = 0; i < 3; i++) {

                    auto &proj = t.v[i].proj;
                    auto &raster = t.v[i].raster;

                    // Perspective division
                    auto rhw = 1.0f / proj.w();

                    raster.w() = rhw;
                    raster.z() = proj.z() * rhw;
                    raster.y() = proj.y() * rhw;
                    raster.x() = proj.x() * rhw;

                    t.v[i].color *= rhw;
                    for (int j = 0; j < SWGL_MAX_TEXTURE_UNITS; j++) {

                        t.v[i].texCoord[j] *= rhw;
                    }

                    // Viewport transformation
                    viewport.transform(raster);
                }
            }

            Context::getCurrentContext()->getRenderer().drawTriangles(std::move(m_triangles));
        }
    }
}
