#include <vector>
#include <intrin.h>
#include "OpenGL.h"
#include "Vertex.h"
#include "Triangle.h"
#include "Matrix.h"
#include "Clipper.h"

namespace SWGL {
    
    Clipper::Clipper()
    
        : m_transInvProjMatrix(Matrix::getIdentity()),
          m_isAnyUserPlaneEnabled(false),
          m_clipPlaneEqs{
        
            Vector( 0.0f,  0.0f, -1.0f, 1.0f), // Near
            Vector( 0.0f,  0.0f,  1.0f, 1.0f), // Far
            Vector( 0.0f, -1.0f,  0.0f, 1.0f), // Top
            Vector( 0.0f,  1.0f,  0.0f, 1.0f), // Bottom
            Vector(-1.0f,  0.0f,  0.0f, 1.0f), // Right
            Vector( 1.0f,  0.0f,  0.0f, 1.0f)  // Left
          } {

    
    }



    void Clipper::setUserPlaneEquation(int index, Vector planeEq) {

        auto &plane = m_userClipPlanes[index];

        plane.equation = planeEq;
        if (plane.isEnabled) {
        
            updateUserPlane(index);
        }
    }

    void Clipper::setUserPlaneEnable(int index, bool isEnabled) {

        m_userClipPlanes[index].isEnabled = isEnabled;
        m_isAnyUserPlaneEnabled = isEnabled;
        
        if (isEnabled) {

            updateUserPlane(index);
        }
        else {

            for (auto i = 0U; i < SWGL_MAX_CLIP_PLANES; i++) {

                if (m_userClipPlanes[i].isEnabled) {

                    m_isAnyUserPlaneEnabled = true;
                    break;
                }
            }
        }
    }

    void Clipper::updateUserPlanes(Matrix &projMatrix) {

        m_transInvProjMatrix = projMatrix.getTransposedInverse();

        for (auto i = 0U; i < SWGL_MAX_CLIP_PLANES; i++) {

            if (m_userClipPlanes[i].isEnabled) {

                updateUserPlane(i);
            }
        }
    }

    Vector Clipper::getUserPlaneEquation(int index) {
    
        return m_userClipPlanes[index].equation;
    }

    bool Clipper::isUserPlaneEnabled(int index) {

        return m_userClipPlanes[index].isEnabled;
    }

    bool Clipper::isAnyUserPlaneEnabled() {

        return m_isAnyUserPlaneEnabled;
    }

    void Clipper::updateUserPlane(int index) {

        m_clipPlaneEqs[OFFSET_USER_PLANES + index] = m_userClipPlanes[index].equation * m_transInvProjMatrix;
    }



    void Clipper::clipTriangles(TriangleList &triangles) {

        std::vector<Triangle> outputList;

        for (auto &t : triangles) {

            // Generate the Sutherland-Hodgman style clipcodes to determine if the
            // triangle must be clipped against a plane of the view frustum.
            int clipOr = Clipcode::None;
            int clipAnd = Clipcode::All;

            for (int i = 0; i < 3; i++) {

                Vertex &v = t.v[i];

                int clipCode = Clipcode::None;
                if (v.projected.x() < -v.projected.w()) clipCode |= Clipcode::Left;
                if (v.projected.x() >  v.projected.w()) clipCode |= Clipcode::Right;
                if (v.projected.y() < -v.projected.w()) clipCode |= Clipcode::Bottom;
                if (v.projected.y() >  v.projected.w()) clipCode |= Clipcode::Top;
                if (v.projected.z() < -v.projected.w()) clipCode |= Clipcode::Far;
                if (v.projected.z() >  v.projected.w()) clipCode |= Clipcode::Near;

                clipOr |= clipCode;
                clipAnd &= clipCode;
            }

            // Determine if the triangle must be clipped against a user defined plane
            if (m_isAnyUserPlaneEnabled) {

                for (auto i = 0U; i < SWGL_MAX_CLIP_PLANES; i++) {

                    if (isUserPlaneEnabled(i)) {

                        for (auto j = 0U; j < 3U; j++) {

                            if (Vector::dot(m_clipPlaneEqs[6 + i], t.v[j].projected) < 0.0f) {

                                clipOr |= Clipcode::User << i;
                            }
                        }
                    }
                }
            }

            // Only process triangles that are not completely outside the view frustum
            if (clipAnd == Clipcode::None) {

                if (clipOr == Clipcode::None) {

                    // The triangle is completely inside the view frustum and user defined
                    // clipping planes
                    outputList.emplace_back(t);
                }
                else {
                    
                    // The triangle is partially outside the view frustum or one of
                    // the user defined clipping planes and must be clipped
                    clipTriangle(t, clipOr, outputList);
                }
            }
        }

        triangles = std::move(outputList);
    }



    void Clipper::clipTriangle(Triangle &t, int clipcode, TriangleList &out) {

        std::vector<Vertex> vInList{t.v[0], t.v[1], t.v[2]};
        std::vector<Vertex> vOutList;

        unsigned long planeIdx;
        while (_BitScanForward(&planeIdx, clipcode)) {

            clipcode ^= 1 << planeIdx;

            //
            // Clip all vertices against the current plane
            //
            const Vector &planeEq = m_clipPlaneEqs[planeIdx];

            for (auto i = 0U, n = vInList.size() - 1; i <= n; i++) {

                Vertex &current = vInList[i];
                Vertex &next = vInList[i == n ? 0 : i + 1];

                // Calculate dot products with the plane equation to determine
                // the distances to the plane
                float d1 = Vector::dot(planeEq, current.projected);
                float d2 = Vector::dot(planeEq, next.projected);

                bool isCurrentInside = d1 >= 0.0f;
                bool isNextInside = d2 >= 0.0f;

                // If the current vertex is inside the clipping plane then add it to
                // the output list.
                if (isCurrentInside) {

                    vOutList.emplace_back(current);
                }

                // If either the current or the next vertex is outside the clipping plane
                // then calculate the intersection with the plane and create a new vertex
                // at the intersection point, which is then added to the output list.
                if (isCurrentInside ^ isNextInside) {

                    // Always clip from the inside out to avoid t-junctions
                    Vertex intersection;

                    if (isCurrentInside) {

                        float t = d1 / (d1 - d2);

                        intersection.projected = Vector::lerp(current.projected, next.projected, t);
                        intersection.colorPrimary = Vector::lerp(current.colorPrimary, next.colorPrimary, t);
                        intersection.colorSecondary = Vector::lerp(current.colorSecondary, next.colorSecondary, t);
                        for (auto j = 0U; j < SWGL_MAX_TEXTURE_UNITS; j++) {

                            intersection.texCoord[j] = Vector::lerp(current.texCoord[j], next.texCoord[j], t);
                        }
                    }
                    else {

                        float t = d2 / (d2 - d1);

                        intersection.projected = Vector::lerp(next.projected, current.projected, t);
                        intersection.colorPrimary = Vector::lerp(next.colorPrimary, current.colorPrimary, t);
                        intersection.colorSecondary = Vector::lerp(next.colorSecondary, current.colorSecondary, t);
                        for (auto j = 0U; j < SWGL_MAX_TEXTURE_UNITS; j++) {

                            intersection.texCoord[j] = Vector::lerp(next.texCoord[j], current.texCoord[j], t);
                        }
                    }

                    vOutList.emplace_back(intersection);
                }
            }

            // Leave this method if there aren't enough vertices for a triangle
            if (vOutList.size() < 3) {
                
                return;
            }

            vInList = std::move(vOutList);
            vOutList.clear();
        }

        // Triangulate the vertices into a triangle fan
        for (int i = 1, n = vInList.size() - 1; i < n; i++) {

            out.emplace_back(

                Triangle(vInList[0], vInList[i], vInList[i + 1])
            );
        }
    }
}
