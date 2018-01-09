#include "Defines.h"
#include "Log.h"
#include "Vertex.h"
#include "TexCoordGen.h"

namespace SWGL {

    TexCoordGen::TexCoordGen(Vertex &vertexState)

        : m_vertexState(vertexState),
          m_enableMask(0) {

    }



    void TexCoordGen::setEnable(int activeTexture, int texCoordIdx, bool isEnabled) {

        if (isEnabled) {

            m_enableMask |= 1 << activeTexture;
            m_state[activeTexture].enableMask |= 1 << texCoordIdx;
        }
        else {

            m_enableMask &= ~(1 << activeTexture);
            m_state[activeTexture].enableMask &= ~(1 << texCoordIdx);
        }
    }

    void TexCoordGen::setMode(int activeTexture, int texCoordIdx, GLenum mode) {

        m_state[activeTexture].data[texCoordIdx].mode = mode;
    }

    void TexCoordGen::setObjectPlane(int activeTexture, int texCoordIdx, Vector plane) {

        m_state[activeTexture].data[texCoordIdx].objectPlane = plane;
    }

    void TexCoordGen::setEyePlane(int activeTexture, int texCoordIdx, Vector plane) {

        m_state[activeTexture].data[texCoordIdx].eyePlane = plane;
    }



    float TexCoordGen::generateTexCoord(int texCoordIdx, TexCoordGenState &state) {

        const auto &data = state.data[texCoordIdx];

        switch (data.mode) {

        case GL_EYE_LINEAR:
            LOG("Unimplemented GL_EYE_LINEAR");
            return 0.0f;

        case GL_OBJECT_LINEAR:
            return Vector::dot(m_vertexState.obj, data.objectPlane);

        case GL_SPHERE_MAP:
            LOG("Unimplemented GL_SPHERE_MAP");
            return 0.0f;

        case GL_NORMAL_MAP:
            LOG("Unimplemented GL_NORMAL_MAP");
            return 0.0f;

        case GL_REFLECTION_MAP:
            LOG("Unimplemented GL_REFLECTION_MAP");
            return 0.0f;
        }

        LOG("Invalid mode");
        return 0.0f;
    }



    bool TexCoordGen::isEnabled() {

        return m_enableMask != 0;
    }

    void TexCoordGen::generate() {

        for (size_t i = 0U; i < SWGL_MAX_TEXTURE_UNITS; i++) {

            auto &state = m_state[i];
            if (state.enableMask != 0) {

                auto &texCoord = m_vertexState.texCoord[i];

                if ((state.enableMask & 1) != 0) { texCoord.x() = generateTexCoord(0, state); }
                if ((state.enableMask & 2) != 0) { texCoord.y() = generateTexCoord(1, state); }
                if ((state.enableMask & 4) != 0) { texCoord.z() = generateTexCoord(2, state); }
                if ((state.enableMask & 8) != 0) { texCoord.w() = generateTexCoord(3, state); }
            }
        }
    }
}
