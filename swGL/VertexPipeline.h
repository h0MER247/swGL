﻿#pragma once

#include <vector>
#include <array>
#include "OpenGL.h"
#include "Vertex.h"
#include "Triangle.h"
#include "Clipper.h"
#include "Matrix.h"
#include "ContextTypes.h"
#include "Log.h"
#include "VertexDataArray.h"

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

    public:
        void setColor(const Vector &color);
        void setTexCoord(size_t index, const Vector &texCoord);
        void setPosition(const Vector &position);
        void addVertex();
        void setActiveTexture(size_t unit);
        void setArrayElement(int idx);

    public:
        void lockArrayElements(int firstIndex, int count);
        void unlockArrayElements();
        void drawArrayElements(GLenum mode, int count, GLenum type, const GLvoid *indices);

    public:
        VertexPointer &getPositionArrayPointer() { return m_vertexDataArray.getPosition(); }
        ColorPointer &getColorArrayPointer() { return m_vertexDataArray.getColor(); }
        TexCoordPointer &getTexCoordArrayPointer() { return m_vertexDataArray.getTexCoord(m_activeTexture); }

    private:
        void addTriangle(Vertex &v1, Vertex &v2, Vertex &v3);
        void drawTriangles();

    private:
        Vertex m_vertexState;
        GLenum m_primitiveType;
        size_t m_activeTexture;
        Matrix m_mvpMatrix;

        VertexList m_vertices;
        TriangleList m_triangles;

        VertexDataArray m_vertexDataArray;
        Clipper m_clipper;
    };
}
