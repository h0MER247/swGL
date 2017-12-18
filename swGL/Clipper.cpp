#include <vector>
#include <intrin.h>
#include "OpenGL.h"
#include "Vertex.h"
#include "Triangle.h"
#include "Matrix.h"
#include "Clipper.h"

namespace SWGL {

    Clipper::Clipper()
    
        : m_clipPlaneEqs{
        
            Vector(0.0f, 0.0f, -1.0f, 1.0f), // Near
            Vector(0.0f, 0.0f, 1.0f, 1.0f),  // Far
            Vector(0.0f, -1.0f, 0.0f, 1.0f), // Top
            Vector(0.0f, 1.0f, 0.0f, 1.0f),  // Bottom
            Vector(-1.0f, 0.0f, 0.0f, 1.0f), // Right
            Vector(1.0f, 0.0f, 0.0f, 1.0f)   // Left
          },
          m_userClipPlanesOrMask(Clipcode::None),
          m_transInvProjMatrix(Matrix::getIdentity()) {

    }



    void Clipper::setClipPlaneEquation(int index, Vector planeEq) {

        auto &userClipPlane = m_userClipPlanes[index];

        userClipPlane.equation = planeEq;
        if (userClipPlane.isEnabled) {
        
            updateUserClippingPlane(index);
        }
    }

    void Clipper::setClipPlaneEnable(int index, bool isEnabled) {

        if (isEnabled) {

            m_userClipPlanes[index].isEnabled = true;
            m_userClipPlanesOrMask |= 1 << (6 + index);

            updateUserClippingPlane(index);
        }
        else {

            m_userClipPlanes[index].isEnabled = false;
            m_userClipPlanesOrMask &= ~(1 << (6 + index));
        }
    }

    void Clipper::updateUserClippingPlanes(Matrix &projMatrix) {

        m_transInvProjMatrix = projMatrix.getTransposedInverse();

        for (int i = 0; i < SWGL_MAX_CLIP_PLANES; i++) {

            if (m_userClipPlanes[i].isEnabled) {

                updateUserClippingPlane(i);
            }
        }
    }

    Vector Clipper::getClipPlaneEquation(int index) {
    
        return m_userClipPlanes[index].equation;
    }

    bool Clipper::isClipPlaneEnabled(int index) {

        return m_userClipPlanes[index].isEnabled;
    }

    bool Clipper::isAnyUserClippingPlaneEnabled() {

        return m_userClipPlanesOrMask != 0;
    }

    void Clipper::updateUserClippingPlane(int index) {

        m_clipPlaneEqs[6 + index] = m_userClipPlanes[index].equation * m_transInvProjMatrix;
    }



    void Clipper::clipTriangles(TriangleList &triangles) {

        std::vector<Triangle> outputList;

        for (auto &t : triangles) {

            // Generate the clipcodes for the triangle (Sutherland-Hodgman style)
            // This is used to determine clipping against the view frustum
            int clipOr = m_userClipPlanesOrMask;
            int clipAnd = Clipcode::All;

            for (int i = 0; i < 3; i++) {

                Vertex &v = t.v[i];

                int clipCode = Clipcode::None;
                if (v.proj.x() < -v.proj.w()) clipCode |= Clipcode::Left;
                if (v.proj.x() > v.proj.w()) clipCode |= Clipcode::Right;
                if (v.proj.y() < -v.proj.w()) clipCode |= Clipcode::Bottom;
                if (v.proj.y() > v.proj.w()) clipCode |= Clipcode::Top;
                if (v.proj.z() < -v.proj.w()) clipCode |= Clipcode::Far;
                if (v.proj.z() > v.proj.w()) clipCode |= Clipcode::Near;

                // TODO: Calculate clipcode for user defined clipping planes as the
                //       "clipOr = m_userClipPlanesOrMask" is only a hack to get things
                //       working.

                clipOr |= clipCode;
                clipAnd &= clipCode;
            }

            // Only process triangles that are not completely outside the view frustum
            if (clipAnd == Clipcode::None) {

                if (clipOr == Clipcode::None) {

                    // The triangle is completely inside the view frustum
                    outputList.emplace_back(t);
                }
                else {
                    
                    // The triangle is partially outside the view frustum and gets clipped
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

            for (int i = 0, n = vInList.size() - 1; i <= n; i++) {

                Vertex &current = vInList[i];
                Vertex &next = vInList[i == n ? 0 : i + 1];

                // Calculate dot products with the plane equation to determine
                // the distances to the plane
                float d1 = Vector::dot(planeEq, current.proj);
                float d2 = Vector::dot(planeEq, next.proj);

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

                        intersection.proj = Vector::lerp(current.proj, next.proj, t);
                        intersection.color = Vector::lerp(current.color, next.color, t);
                        for (int j = 0; j < SWGL_MAX_TEXTURE_UNITS; j++) {

                            intersection.texCoord[j] = Vector::lerp(current.texCoord[j], next.texCoord[j], t);
                        }
                    }
                    else {

                        float t = d2 / (d2 - d1);

                        intersection.proj = Vector::lerp(next.proj, current.proj, t);
                        intersection.color = Vector::lerp(next.color, current.color, t);
                        for (int j = 0; j < SWGL_MAX_TEXTURE_UNITS; j++) {

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
