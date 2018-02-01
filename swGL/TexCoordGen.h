#pragma once

#include "OpenGL.h"
#include "Vector.h"

namespace SWGL {

    //
    //
    //
    class TexCoordGen {

    public:
        TexCoordGen();
        ~TexCoordGen() = default;

    public:
        void setEnable(unsigned int texCoordIdx, bool isEnabled);
        void setMode(unsigned int texCoordIdx, GLenum mode);
        void setObjectPlane(unsigned int texCoordIdx, Vector plane);
        void setEyePlane(unsigned int texCoordIdx, Vector plane);
        void setActiveTexture(unsigned int activeTexture);

    public:
        bool isEnabled(unsigned int texUnit);
        void generate(Vertex &v, unsigned int texUnit);

    private:
        struct TexCoordGenState {

            unsigned int enableMask = 0U;

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
        TexCoordGenState m_state[SWGL_MAX_TEXTURE_UNITS];

    private:
        unsigned int m_activeTexture;
    };
}
