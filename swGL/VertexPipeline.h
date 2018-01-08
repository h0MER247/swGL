#pragma once

#include <vector>
#include <array>
#include "OpenGL.h"
#include "Vertex.h"
#include "Triangle.h"
#include "Clipper.h"
#include "Matrix.h"
#include "ContextTypes.h"
#include "MatrixStack.h"
#include "VertexDataArray.h"
#include "TexCoordGen.h"

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
        void setColor(const Vector &color);
        void setTexCoord(size_t index, const Vector &texCoord);
        void setPosition(const Vector &position);
        void addVertex();
        void setActiveTexture(size_t unit);
        void setArrayElement(int idx);

    public:
        void setTexGenEnable(int texCoordIdx, bool isEnabled);
        void setTexGenMode(int texCoordIdx, GLenum mode);
        void setTexGenObjectPlane(int texCoordIdx, Vector planeEq);
        void setTexGenEyePlane(int texCoordIdx, Vector planeEq);

    public:
        void lockArrayElements(int firstIndex, int count);
        void unlockArrayElements();
        void drawIndexedArrayElements(GLenum mode, int count, GLenum type, const GLvoid *indices);
        void drawArrayElements(GLenum mode, GLint first, GLsizei count);

    public:
        VertexPointer &getPositionArrayPointer() { return m_vertexDataArray.getPosition(); }
        ColorPointer &getColorArrayPointer() { return m_vertexDataArray.getColor(); }
        TexCoordPointer &getTexCoordArrayPointer() { return m_vertexDataArray.getTexCoord(m_activeTexture); }

    public:
        Clipper &getClipper() { return m_clipper; }
        MatrixStack &getMatrixStack() { return m_matrixStack; }

    private:
        void addTriangle(Vertex &v1, Vertex &v2, Vertex &v3);
        void drawTriangles();

    private:
        bool m_isInsideGLBegin;
        Vertex m_vertexState;
        GLenum m_primitiveType;
        size_t m_activeTexture;
        Matrix m_mvpMatrix;

        TexCoordGen m_texCoordGen;

        VertexList m_vertices;
        TriangleList m_triangles;

        VertexDataArray m_vertexDataArray;

        Clipper m_clipper;
        MatrixStack m_matrixStack;
    };
}
