#pragma once

#include "Defines.h"
#include "Vector.h"
#include "Vertex.h"

namespace SWGL {

    //
    //
    //
    struct TexCoordGenState {

        int enableMask;

        struct {

            GLenum mode;
            Vector objectPlane;
            Vector eyePlane;

        } data[4];
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
