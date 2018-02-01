#include "Defines.h"
#include "Log.h"
#include "Vertex.h"
#include "MatrixStack.h"
#include "TexCoordGen.h"

namespace SWGL {
    
    TexCoordGen::TexCoordGen()

        : m_activeTexture(0U) {

    }



    void TexCoordGen::setEnable(unsigned int texCoordIdx, bool isEnabled) {

        if (isEnabled) {

            m_state[m_activeTexture].enableMask |= 1U << (3U - texCoordIdx);
        }
        else {

            m_state[m_activeTexture].enableMask &= ~(1U << (3U - texCoordIdx));
        }
    }

    void TexCoordGen::setMode(unsigned int texCoordIdx, GLenum mode) {

        m_state[m_activeTexture].data[3U - texCoordIdx].mode = mode;
    }

    void TexCoordGen::setObjectPlane(unsigned int texCoordIdx, Vector plane) {

        m_state[m_activeTexture].data[3U - texCoordIdx].objectPlane = plane;
    }

    void TexCoordGen::setEyePlane(unsigned int texCoordIdx, Vector plane) {

        m_state[m_activeTexture].data[3U - texCoordIdx].eyePlane = plane;
    }

    void TexCoordGen::setActiveTexture(unsigned int activeTexture) {

        m_activeTexture = activeTexture;
    }



    bool TexCoordGen::isEnabled(unsigned int texUnit) {

        return m_state[texUnit].enableMask != 0U;
    }

    void TexCoordGen::generate(Vertex &v, unsigned int texUnit) {

        auto &state = m_state[texUnit];

        for (auto i = 0U; i < 4U; i++) {

            if ((state.enableMask & (1U << i)) != 0U) {

                auto &data = state.data[i];
                auto &texCoord = v.texCoord[texUnit];

                switch (data.mode) {

                case GL_EYE_LINEAR:
                    texCoord[i] = Vector::dot(v.posEye, data.eyePlane);
                    break;

                case GL_OBJECT_LINEAR:
                    texCoord[i] = Vector::dot(v.posObj, data.objectPlane);
                    break;

                case GL_SPHERE_MAP:
                    LOG("Unimplemented texture generation mode: GL_SPHERE_MAP");
                    texCoord[i] = 0.0f;
                    break;

                case GL_NORMAL_MAP:
                    texCoord[i] = v.normal[i];
                    break;

                case GL_REFLECTION_MAP:
                    texCoord[i] = v.posEye[i] - (v.normal[i] * 2.0f * Vector::dot3(v.posEye, v.normal));
                    break;
                }
            }
        }
    }
}
