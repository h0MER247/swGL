#pragma once

#include <array>
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
        void setClipPlaneEquation(int index, Vector planeEq);
        void setClipPlaneEnable(int index, bool isEnabled);

        Vector getClipPlaneEquation(int index);
        bool isClipPlaneEnabled(int index);

        void updateUserClippingPlanes(Matrix &projMatrix);
        bool isAnyUserClippingPlaneEnabled();

    public:
        void clipTriangles(TriangleList &triangles);

    private:
        void clipTriangle(Triangle &t, int clipcode, TriangleList &out);
        void updateUserClippingPlane(int index);

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

    private:
        struct UserClippingPlane {

            UserClippingPlane()

                : equation(Vector(0.0f, 0.0f, 0.0f, 0.0f)),
                  isEnabled(false) {

            }

            Vector equation;
            bool isEnabled;
        };
        std::array<UserClippingPlane, SWGL_MAX_CLIP_PLANES> m_userClipPlanes;
        Matrix m_transInvProjMatrix;
        int m_userClipPlanesOrMask;

    private:
        std::array<Vector, 6 + SWGL_MAX_CLIP_PLANES> m_clipPlaneEqs;
    };
}
