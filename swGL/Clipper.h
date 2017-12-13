#pragma once

namespace SWGL {

    //
    // Performs the clipping of triangles
    //
    class Clipper {

    public:
        Clipper() = default;
        ~Clipper() = default;

    public:
        void clipTriangles(TriangleList &triangles);

    private:
        void clipTriangle(Triangle &t, int clipcode, TriangleList &out);

    private:
        enum Clipcode : int {

            None = 0,
            Near = 1,
            Far = 2,
            Top = 4,
            Bottom = 8,
            Right = 16,
            Left = 32,
            All = 63
        };
        const Vector m_clipPlaneEqs[6] = {

            Vector( 0.0f,  0.0f, -1.0f, 1.0f), // Near
            Vector( 0.0f,  0.0f,  1.0f, 1.0f), // Far
            Vector( 0.0f, -1.0f,  0.0f, 1.0f), // Top
            Vector( 0.0f,  1.0f,  0.0f, 1.0f), // Bottom
            Vector(-1.0f,  0.0f,  0.0f, 1.0f), // Right
            Vector( 1.0f,  0.0f,  0.0f, 1.0f)  // Left
        };
    };
}
