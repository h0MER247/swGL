#pragma once

#include "OpenGL.h"
#include "Vector.h"

namespace SWGL {

    //
    //
    //
    struct TexCoordGenState {

        int enableMask = 0;

        struct {

            GLenum mode;
            Vector objectPlane;
            Vector eyePlane;

        } data[4] = {

            { GL_EYE_LINEAR, Vector(1.0f, 0.0f, 0.0f, 0.0f), Vector(1.0f, 0.0f, 0.0f, 0.0f) },
            { GL_EYE_LINEAR, Vector(0.0f, 1.0f, 0.0f, 0.0f), Vector(0.0f, 1.0f, 0.0f, 0.0f) },
            { GL_EYE_LINEAR, Vector(0.0f, 0.0f, 0.0f, 0.0f), Vector(0.0f, 0.0f, 0.0f, 0.0f) },
            { GL_EYE_LINEAR, Vector(0.0f, 0.0f, 0.0f, 0.0f), Vector(0.0f, 0.0f, 0.0f, 0.0f) }
        };
    };

    //
    //
    //
    class TexCoordGen {

    public:
        TexCoordGen(Vertex &vertexState);
        ~TexCoordGen() = default;

    public:
        void setEnable(int activeTexture, int texCoordIdx, bool isEnabled);
        void setMode(int activeTexture, int texCoordIdx, GLenum mode);
        void setObjectPlane(int activeTexture, int texCoordIdx, Vector plane);
        void setEyePlane(int activeTexture, int texCoordIdx, Vector plane);

    private:
        float generateTexCoord(int texCoordIdx, TexCoordGenState &state);

    public:
        bool isEnabled();
        void generate();

    private:
        int m_enableMask;
        Vertex &m_vertexState;
        TexCoordGenState m_state[SWGL_MAX_TEXTURE_UNITS];
    };
}
