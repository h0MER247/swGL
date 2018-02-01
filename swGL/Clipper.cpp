#include <vector>
#include <intrin.h>
#include "Vertex.h"
#include "Triangle.h"
#include "Clipper.h"

// TODO: Try to implement guard band clipping to (mostly) avoid the costly analytical clipping
namespace SWGL {
    
    Clipper::Clipper()
    
        : m_clipPlaneEqs{
        
            Vector( 0.0f,  0.0f, -1.0f, 1.0f), // Near
            Vector( 0.0f,  0.0f,  1.0f, 1.0f), // Far
            Vector( 0.0f, -1.0f,  0.0f, 1.0f), // Top
            Vector( 0.0f,  1.0f,  0.0f, 1.0f), // Bottom
            Vector(-1.0f,  0.0f,  0.0f, 1.0f), // Right
            Vector( 1.0f,  0.0f,  0.0f, 1.0f)  // Left
          },
          m_userPlaneEnableMask(0U) {

        for (auto i = 0U; i < SWGL_MAX_CLIP_PLANES; i++) {

            m_clipPlaneEqs[OFFSET_USER_PLANES + i] = Vector(0.0f, 0.0f, 0.0f, 0.0f);
        }
    }



    bool Clipper::clipTriangles(TriangleList &triangles) {

        std::vector<Triangle> outputList;

        for (auto &t : triangles) {

            // Generate the Sutherland-Hodgman style clipcodes to determine if the
            // triangle must be clipped against a plane of the view frustum.
            unsigned int clipOr = Clipcode::None;
            unsigned int clipAnd = Clipcode::All;

            for (auto &v : t.v) {

                unsigned int clipCode = Clipcode::None;
                if (v.posProj.x() < -v.posProj.w()) { clipCode |= Clipcode::Left; }
                if (v.posProj.x() >  v.posProj.w()) { clipCode |= Clipcode::Right; }
                if (v.posProj.y() < -v.posProj.w()) { clipCode |= Clipcode::Bottom; }
                if (v.posProj.y() >  v.posProj.w()) { clipCode |= Clipcode::Top; }
                if (v.posProj.z() < -v.posProj.w()) { clipCode |= Clipcode::Far; }
                if (v.posProj.z() >  v.posProj.w()) { clipCode |= Clipcode::Near; }

                clipOr |= clipCode;
                clipAnd &= clipCode;
            }

            // Determine if the triangle must be clipped against a user defined plane
            if (isAnyUserPlaneEnabled()) {

                for (auto i = 0U; i < SWGL_MAX_CLIP_PLANES; i++) {

                    if (isUserPlaneEnabled(i)) {

                        auto &planeEq = m_clipPlaneEqs[OFFSET_USER_PLANES + i];
                        auto d1 = Vector::dot(planeEq, t.v[0].posEye) < 0.0f;
                        auto d2 = Vector::dot(planeEq, t.v[1].posEye) < 0.0f;
                        auto d3 = Vector::dot(planeEq, t.v[2].posEye) < 0.0f;

                        if (d1 | d2 | d3) {

                            clipOr |= Clipcode::User << i;
                        }
                    }
                }
            }

            // Only process triangles that are not completely outside the view frustum
            if (clipAnd == Clipcode::None) {

                if (clipOr == Clipcode::None) {

                    // The triangle is completely inside the view frustum and the user
                    // defined clipping planes
                    outputList.emplace_back(t);
                }
                else {
                    
                    // The triangle is partially outside the view frustum or one of
                    // the user defined clipping planes and must be clipped
                    clipTriangle(t, clipOr, outputList);
                }
            }
        }

        if (!outputList.empty()) {

            triangles = std::move(outputList);
            return true;
        }

        return false;
    }

    void Clipper::clipTriangle(Triangle &t, unsigned int clipcode, TriangleList &out) {

        std::vector<Vertex> vInList{t.v[0], t.v[1], t.v[2]};
        std::vector<Vertex> vOutList;

        unsigned long planeIdx;
        while (_BitScanForward(&planeIdx, clipcode)) {

            clipcode ^= 1U << planeIdx;

            //
            // Clip all vertices against the current plane
            //
            auto isUserPlane = planeIdx >= OFFSET_USER_PLANES;
            auto &planeEq = m_clipPlaneEqs[planeIdx];

            for (auto i = 0U, n = vInList.size() - 1; i <= n; i++) {

                Vertex &current = vInList[i];
                Vertex &next = vInList[i == n ? 0 : i + 1];

                // Calculate the distance of "current" and "next" to the plane
                float d1 = Vector::dot(planeEq, isUserPlane ? current.posEye : current.posProj);
                float d2 = Vector::dot(planeEq, isUserPlane ? next.posEye : next.posProj);

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

                        intersection.posProj = Vector::lerp(current.posProj, next.posProj, t);
                        intersection.posEye = Vector::lerp(current.posEye, next.posEye, t);
                        intersection.colorPrimary = Vector::lerp(current.colorPrimary, next.colorPrimary, t);
                        //intersection.colorSecondary = Vector::lerp(current.colorSecondary, next.colorSecondary, t);
                        for (auto j = 0U; j < SWGL_MAX_TEXTURE_UNITS; j++) {

                            intersection.texCoord[j] = Vector::lerp(current.texCoord[j], next.texCoord[j], t);
                        }
                    }
                    else {

                        float t = d2 / (d2 - d1);

                        intersection.posProj = Vector::lerp(next.posProj, current.posProj, t);
                        intersection.posEye = Vector::lerp(next.posEye, current.posEye, t);
                        intersection.colorPrimary = Vector::lerp(next.colorPrimary, current.colorPrimary, t);
                        //intersection.colorSecondary = Vector::lerp(next.colorSecondary, current.colorSecondary, t);
                        for (auto j = 0U; j < SWGL_MAX_TEXTURE_UNITS; j++) {

                            intersection.texCoord[j] = Vector::lerp(next.texCoord[j], current.texCoord[j], t);
                        }
                    }

                    vOutList.emplace_back(intersection);
                }
            }

            // Leave this method if there aren't enough vertices for a triangle
            if (vOutList.size() < 3U) { return; }

            std::swap(vInList, vOutList);
            vOutList.clear();
        }

        // Triangulate the vertices into a triangle fan
        for (auto i = 1U, n = vInList.size() - 1; i < n; i++) {

            out.emplace_back(

                Triangle(vInList[0], vInList[i], vInList[i + 1])
            );
        }
    }



    void Clipper::setUserPlaneEquation(unsigned int index, Vector planeEq) {

        m_clipPlaneEqs[OFFSET_USER_PLANES + index] = planeEq;
    }

    void Clipper::setUserPlaneEnable(unsigned int index, bool isEnabled) {

        if (isEnabled) {

            m_userPlaneEnableMask |= 1U << index;
        }
        else {

            m_userPlaneEnableMask &= ~(1U << index);
        }
    }

    Vector Clipper::getUserPlaneEquation(unsigned int index) {

        return m_clipPlaneEqs[OFFSET_USER_PLANES + index];
    }

    bool Clipper::isUserPlaneEnabled(unsigned int index) {

        return (m_userPlaneEnableMask & (1U << index)) != 0U;
    }

    bool Clipper::isAnyUserPlaneEnabled() {

        return m_userPlaneEnableMask != 0U;
    }
}
