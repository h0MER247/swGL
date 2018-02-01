#pragma once

namespace SWGL {

    //
    // Performs the clipping of triangles
    //
    class Clipper {

    public:
        Clipper();
        ~Clipper() = default;

    public:
        bool clipTriangles(TriangleList &triangles);

    private:
        void clipTriangle(Triangle &t, unsigned int clipcode, TriangleList &out);

    public:
        void setUserPlaneEquation(unsigned int index, Vector planeEq);
        void setUserPlaneEnable(unsigned int index, bool isEnabled);
        Vector getUserPlaneEquation(unsigned int index);
        bool isUserPlaneEnabled(unsigned int index);
        bool isAnyUserPlaneEnabled();

    private:
        enum Clipcode : unsigned int {

            // Frustum
            None = 0U,
            Near = 1U,
            Far = 2U,
            Top = 4U,
            Bottom = 8U,
            Right = 16U,
            Left = 32U,
            All = 63U,

            // User defined
            User = 64U
        };

        static constexpr unsigned int OFFSET_USER_PLANES = 6U;

    private:
        Vector m_clipPlaneEqs[6 + SWGL_MAX_CLIP_PLANES];
        unsigned int m_userPlaneEnableMask;
    };
}
