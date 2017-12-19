#pragma once

#include "Matrix.h"

namespace SWGL {

    //
    // Performs the clipping of triangles
    //
    class Clipper {

    public:
        Clipper();
        ~Clipper() = default;

    public:
        void setUserPlaneEquation(int index, Vector planeEq);
        void setUserPlaneEnable(int index, bool isEnabled);
        void updateUserPlanes(Matrix &projMatrix);

        Vector getUserPlaneEquation(int index);
        bool isUserPlaneEnabled(int index);
        bool isAnyUserPlaneEnabled();

    private:
        void updateUserPlane(int index);

    public:
        void clipTriangles(TriangleList &triangles);

    private:
        void clipTriangle(Triangle &t, int clipcode, TriangleList &out);

    private:
        enum Clipcode : int {

            // Frustum
            None = 0,
            Near = 1,
            Far = 2,
            Top = 4,
            Bottom = 8,
            Right = 16,
            Left = 32,
            All = 63,

            // User defined
            User = 64
        };

        struct UserClipPlane {

            UserClipPlane()

                : isEnabled(false),
                  equation(Vector(0.0f, 0.0f, 0.0f, 0.0f)) {

            }

            bool isEnabled;
            Vector equation;
        };

        static constexpr size_t OFFSET_USER_PLANES = 6;

    private:
        Matrix m_transInvProjMatrix;
        bool m_isAnyUserPlaneEnabled;
        UserClipPlane m_userClipPlanes[SWGL_MAX_CLIP_PLANES];
        Vector m_clipPlaneEqs[6 + SWGL_MAX_CLIP_PLANES];
    };
}
