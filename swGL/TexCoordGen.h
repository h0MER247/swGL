#pragma once

#include "OpenGL.h"
#include "Vector.h"

namespace SWGL {

    //
    //
    //
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

    //
    //
    //
    class TexCoordGen {

    public:
        TexCoordGen(Vertex &vertexState, MatrixStack &matrixStack);
        ~TexCoordGen() = default;

    public:
        void setEnable(unsigned int texCoordIdx, bool isEnabled);
        void setMode(unsigned int texCoordIdx, GLenum mode);
        void setObjectPlane(unsigned int texCoordIdx, Vector plane);
        void setEyePlane(unsigned int texCoordIdx, Vector plane);
        void setActiveTexture(unsigned int activeTexture);

    private:
        float generateTexCoord(unsigned int texCoordIdx, TexCoordGenState &state);

    public:
        bool isEnabled();
        void generate();

    private:
        unsigned int m_activeTexture;
        unsigned int m_enableMask;
        Vertex &m_vertexState;
        MatrixStack &m_matrixStack;
        TexCoordGenState m_state[SWGL_MAX_TEXTURE_UNITS];
    };
}
