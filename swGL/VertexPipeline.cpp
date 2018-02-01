#include <cmath>
#include "Context.h"
#include "VertexPipeline.h"

namespace SWGL {

    VertexPipeline::VertexPipeline()
    
        : m_isInsideGLBegin(false),
          m_vertexDataArray(m_matrixStack, m_texCoordGen) {

        setNormal(Vector(0.0f, 0.0f, 1.0f, 0.0f));
        setPrimaryColor(Vector(1.0f, 1.0f, 1.0f, 1.0f));
        for (auto i = 0U; i < SWGL_MAX_TEXTURE_UNITS; i++) {

            setTexCoord(i, Vector(0.0f, 0.0f, 0.0f, 1.0f));
        }
    }



    void VertexPipeline::begin(GLenum primitiveType) {

        m_primitiveType = primitiveType;
        m_isInsideGLBegin = true;
    }

    void VertexPipeline::end() {

        bool flipFlop = true;

        switch (m_primitiveType) {

        case GL_POINTS:
            LOG("Unimplemented primitive type: GL_POINTS");
            break;

        case GL_LINE_LOOP:
            if (m_vertices.size() < 2U) { break; }
            for (auto i = 0U, n = m_vertices.size() - 1; i <= n; i++) {

                Vertex &v1 = m_vertices[i];
                Vertex &v2 = i == n ? m_vertices[0] : m_vertices[i + 1];
                
                addLine(v1, v2);
            }
            break;

        case GL_LINE_STRIP:
            if (m_vertices.size() < 2U) { break; }
            for (auto i = 0U, n = m_vertices.size() - 1; i < n; i++) {

                Vertex &v1 = m_vertices[i];
                Vertex &v2 = m_vertices[i + 1];

                addLine(v1, v2);
            }
            break;

        case GL_LINES:
            if (m_vertices.size() < 2U) { break; }
            for (auto i = 0U, n = m_vertices.size(); i < n; i += 2) {

                Vertex &v1 = m_vertices[i];
                Vertex &v2 = m_vertices[i + 1];

                addLine(v1, v2);
            }
            break;

        case GL_TRIANGLES:
            if (m_vertices.size() < 3U) { break; }
            for (auto i = 0U, n = m_vertices.size() - 2; i < n; i += 3) {

                Vertex &v1 = m_vertices[i];
                Vertex &v2 = m_vertices[i + 1];
                Vertex &v3 = m_vertices[i + 2];

                addTriangle(v1, v2, v3);
            }
            break;

        case GL_TRIANGLE_STRIP:
            if (m_vertices.size() < 3U) { break; }
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
            if (m_vertices.size() < 3U) { break; }
            for (auto i = 1U, n = m_vertices.size() - 1; i < n; i++) {

                Vertex &v1 = m_vertices[0];
                Vertex &v2 = m_vertices[i];
                Vertex &v3 = m_vertices[i + 1];

                addTriangle(v1, v2, v3);
            }
            break;

        case GL_QUADS:
            if (m_vertices.size() < 4U) { break; }
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
            if (m_vertices.size() < 4U) { break; }
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
        float nx = -((v2.posProj.y() / v2.posProj.w()) - (v1.posProj.y() / v1.posProj.w()));
        float ny =  ((v2.posProj.x() / v2.posProj.w()) - (v1.posProj.x() / v1.posProj.w()));
        float rcpLength = 1.0f / std::sqrt(nx * nx + ny * ny);
        nx *= rcpLength;
        ny *= rcpLength;

        // Scale normal according to the width of the line
        float halfLineWidth = 1.0f * 0.5f;
        nx *= halfLineWidth;
        ny *= halfLineWidth;

        // Now create the vertices that define two triangles which are used to draw the line
        Vertex v[] { v1, v1, v2, v2 };

        v[0].posProj.x() += ( nx * v1.posProj.w()) * rcpViewportScaleX;
        v[0].posProj.y() += ( ny * v1.posProj.w()) * rcpViewportScaleY;
        v[1].posProj.x() += (-nx * v1.posProj.w()) * rcpViewportScaleX;
        v[1].posProj.y() += (-ny * v1.posProj.w()) * rcpViewportScaleY;

        v[2].posProj.x() += ( nx * v2.posProj.w()) * rcpViewportScaleX;
        v[2].posProj.y() += ( ny * v2.posProj.w()) * rcpViewportScaleY;
        v[3].posProj.x() += (-nx * v2.posProj.w()) * rcpViewportScaleX;
        v[3].posProj.y() += (-ny * v2.posProj.w()) * rcpViewportScaleY;

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

        m_vertexState.posObj = position;
        m_vertexState.posEye = m_vertexState.posObj * m_matrixStack.getModelViewMatrix();
        m_vertexState.posProj = m_vertexState.posEye * m_matrixStack.getProjectionMatrix();
    }

    void VertexPipeline::addVertex() {

        // TODO: Only execute this if needed
        for (auto i = 0U; i < SWGL_MAX_TEXTURE_UNITS; i++) {

            if (m_texCoordGen.isEnabled(i)) {

                m_texCoordGen.generate(m_vertexState, i);
            }

            m_vertexState.texCoord[i] = m_vertexState.texCoord[i] * m_matrixStack.getTextureMatrix(i);
        }

        m_vertices.emplace_back(m_vertexState);
    }



    void VertexPipeline::setArrayElement(unsigned int idx) {

        // Color
        if (m_vertexDataArray.isVertexColorEnabled()) {

            setPrimaryColor(m_vertexDataArray.getVertexColor(idx));
        }

        // Normal
        if (m_vertexDataArray.isVertexNormalEnabled()) {

            setNormal(m_vertexDataArray.getVertexNormal(idx));
        }

        // Texture coordinates
        for (auto texUnit = 0U; texUnit < SWGL_MAX_TEXTURE_UNITS; texUnit++) {

            if (m_vertexDataArray.isVertexTexCoordEnabled(texUnit)) {

                setTexCoord(texUnit, m_vertexDataArray.getVertexTexCoord(idx, texUnit));
            }
        }

        // Position
        if (m_vertexDataArray.isVertexPositionEnabled()) {

            setPosition(m_vertexDataArray.getVertexPosition(idx));
            addVertex();
        }
    }



    void VertexPipeline::drawIndexedArrayElements(GLenum mode, unsigned int count, GLenum type, const GLvoid *indices) {

        if (m_vertexDataArray.isVertexPositionEnabled()) {

            begin(mode); {

                // TODO: Figure something else out as this isn't a good solution
                IndexSupplier supplier;
                switch (type) {

                case GL_UNSIGNED_BYTE:
                    supplier = [&idx = indices] (unsigned int i) {

                        return static_cast<unsigned int>(reinterpret_cast<const unsigned char *>(idx)[i]);
                    };
                    break;

                case GL_UNSIGNED_SHORT:
                    supplier = [&idx = indices](unsigned int i) {

                        return static_cast<unsigned int>(reinterpret_cast<const unsigned short *>(idx)[i]);
                    };
                    break;

                case GL_UNSIGNED_INT:
                    supplier = [&idx = indices](unsigned int i) {

                        return reinterpret_cast<const unsigned int *>(idx)[i];
                    };
                    break;
                }
                    
                m_vertexDataArray.fetchVertices(

                    m_vertexState,
                    m_vertices,
                    0U,
                    count,
                    supplier
                );
            }
            end();
        }
    }

    void VertexPipeline::drawArrayElements(GLenum mode, unsigned int first, unsigned int count) {

        if (m_vertexDataArray.isVertexPositionEnabled()) {

            begin(mode); {

                m_vertexDataArray.fetchVertices(

                    m_vertexState,
                    m_vertices,
                    first,
                    count,
                    [] (unsigned int i) { return i; }
                );
            }
            end();
        }
    }



    void VertexPipeline::drawTriangles() {

        if (m_lighting.isEnabled()) {

            m_lighting.calculateLighting(m_triangles);
        }

        if (m_clipper.clipTriangles(m_triangles)) {

            for (auto &t : m_triangles) {

                for (auto &v : t.v) {

                    auto &proj = v.posProj;
                    auto &raster = v.posObj;

                    // Perspective division
                    auto rhw = 1.0f / proj.w();

                    raster.w() = rhw;
                    raster.z() = proj.z() * rhw;
                    raster.y() = proj.y() * rhw;
                    raster.x() = proj.x() * rhw;

                    v.colorPrimary *= rhw;
                    v.colorSecondary *= rhw;
                    for (auto &texCoord : v.texCoord) {

                        texCoord *= rhw;
                    }

                    // Viewport transformation
                    m_viewport.transform(raster);
                }
            }

            Context::getCurrentContext()->getRenderer().drawTriangles(m_triangles);
        }
    }
}
