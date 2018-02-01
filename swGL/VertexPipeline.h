#pragma once

#include <vector>
#include "OpenGL.h"
#include "Vertex.h"
#include "Triangle.h"
#include "Clipper.h"
#include "ContextTypes.h"
#include "MatrixStack.h"
#include "VertexDataArray.h"
#include "TexCoordGen.h"
#include "Lighting.h"

namespace SWGL {

    //
    // Implementation of swGL's vertex pipeline
    //
    class VertexPipeline {

    public:
        VertexPipeline();
        ~VertexPipeline() = default;

    public:
        void begin(GLenum primitiveType);
        void end();
        bool isInsideGLBegin() { return m_isInsideGLBegin; }

    public:
        void setPrimaryColor(const Vector &color);
        void setTexCoord(unsigned int index, const Vector &texCoord);
        void setNormal(const Vector &normal);
        void setPosition(const Vector &position);
        void addVertex();
        void setArrayElement(unsigned int idx);

    public:
        void drawIndexedArrayElements(GLenum mode, unsigned int count, GLenum type, const GLvoid *indices);
        void drawArrayElements(GLenum mode, unsigned int first, unsigned int count);

    public:
        TexCoordGen &getTexGen() { return m_texCoordGen; }
        Lighting &getLighting() { return m_lighting; }
        Culling &getCulling() { return m_culling; }
        Viewport &getViewport() { return m_viewport; }
        Clipper &getClipper() { return m_clipper; }
        MatrixStack &getMatrixStack() { return m_matrixStack; }
        VertexDataArray &getVertexDataArray() { return m_vertexDataArray; }

    private:
        void addTriangle(Vertex &v1, Vertex &v2, Vertex &v3);
        void addLine(Vertex &v1, Vertex &v2);
        void drawTriangles();

    private:
        bool m_isInsideGLBegin;
        Vertex m_vertexState;
        GLenum m_primitiveType;

    private:
        TexCoordGen m_texCoordGen;
        Lighting m_lighting;
        Culling m_culling;
        Viewport m_viewport;
        Clipper m_clipper;
        MatrixStack m_matrixStack;
        VertexDataArray m_vertexDataArray;

    private:
        VertexList m_vertices;
        TriangleList m_triangles;
    };
}
