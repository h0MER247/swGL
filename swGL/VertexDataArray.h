#pragma once

#include <algorithm>
#include <functional>
#include <memory>
#include "Vector.h"
#include "Vertex.h"
#include "Matrix.h"
#include "OpenGL.h"

namespace SWGL {

    //
    // Reads data from client memory into Vector's
    //
    class VectorReader {
        
    public:
        VectorReader(bool normalizeValues, bool setWTo1) :

            m_isEnabled(false),
            m_addr(nullptr),
            m_normalizeValues(normalizeValues),
            m_setWTo1(setWTo1) {

        }
        ~VectorReader() = default;

    public:
        void setSource(const void *addr, GLenum type, unsigned int stride, unsigned int numArgs) {

            switch (type) {

            case GL_BYTE:           initSource<GLbyte>(stride, numArgs); break;
            case GL_UNSIGNED_BYTE:  initSource<GLubyte>(stride, numArgs); break;
            case GL_SHORT:          initSource<GLshort>(stride, numArgs); break;
            case GL_UNSIGNED_SHORT: initSource<GLushort>(stride, numArgs); break;
            case GL_INT:            initSource<GLint>(stride, numArgs); break;
            case GL_UNSIGNED_INT:   initSource<GLuint>(stride, numArgs); break;
            case GL_FLOAT:          initSource<GLfloat>(stride, numArgs); break;
            case GL_DOUBLE:         initSource<GLdouble>(stride, numArgs); break;
            }

            m_addr = reinterpret_cast<const char *>(addr);
            m_numArgs = numArgs;
        }

        void setEnable(bool isEnabled) {

            m_isEnabled = isEnabled;
        }

        bool isEnabled() {

            return m_isEnabled;
        }

    public:
        Vector read(unsigned int index) {

            return m_reader(index);
        }

    private:
        template<typename T>
        inline void initSource(unsigned int stride, unsigned int numArgs) {

            m_stride = stride == 0U ? sizeof(T) * numArgs : stride;

            #define DEFINE_READER(ARG1, ARG2, ARG3, ARG4)                                   \
                m_reader = [&myAddr = m_addr, &myStride = m_stride] (unsigned int index) {  \
                                                                                            \
                    auto p = reinterpret_cast<const T *>(myAddr + myStride * index);        \
                    return Vector(ARG1, ARG2, ARG3, ARG4);                                  \
                }

            switch (numArgs) {

            case 1U:
                if (m_normalizeValues && m_setWTo1) {

                    DEFINE_READER(toNormalizedFloat(p[0]), 0.0f, 0.0f, 1.0f);
                }
                else if (m_normalizeValues && !m_setWTo1) {

                    DEFINE_READER(toNormalizedFloat(p[0]), 0.0f, 0.0f, 0.0f);
                }
                else if (!m_normalizeValues && m_setWTo1) {

                    DEFINE_READER(static_cast<float>(p[0]), 0.0f, 0.0f, 1.0f);
                }
                else if (!m_normalizeValues && !m_setWTo1) {

                    DEFINE_READER(static_cast<float>(p[0]), 0.0f, 0.0f, 0.0f);
                }
                break;

            case 2U:
                if (m_normalizeValues && m_setWTo1) {

                    DEFINE_READER(toNormalizedFloat(p[0]), toNormalizedFloat(p[1]), 0.0f, 1.0f);
                }
                else if (m_normalizeValues && !m_setWTo1) {

                    DEFINE_READER(toNormalizedFloat(p[0]), toNormalizedFloat(p[1]), 0.0f, 0.0f);
                }
                else if (!m_normalizeValues && m_setWTo1) {

                    DEFINE_READER(static_cast<float>(p[0]), static_cast<float>(p[1]), 0.0f, 1.0f);
                }
                else if (!m_normalizeValues && !m_setWTo1) {

                    DEFINE_READER(static_cast<float>(p[0]), static_cast<float>(p[1]), 0.0f, 0.0f);
                }
                break;

            case 3U:
                if (m_normalizeValues && m_setWTo1) {

                    DEFINE_READER(toNormalizedFloat(p[0]), toNormalizedFloat(p[1]), toNormalizedFloat(p[2]), 1.0f);
                }
                else if (m_normalizeValues && !m_setWTo1) {

                    DEFINE_READER(toNormalizedFloat(p[0]), toNormalizedFloat(p[1]), toNormalizedFloat(p[2]), 0.0f);
                }
                else if (!m_normalizeValues && m_setWTo1) {

                    DEFINE_READER(static_cast<float>(p[0]), static_cast<float>(p[1]), static_cast<float>(p[2]), 1.0f);
                }
                else if (!m_normalizeValues && !m_setWTo1) {

                    DEFINE_READER(static_cast<float>(p[0]), static_cast<float>(p[1]), static_cast<float>(p[2]), 0.0f);
                }
                break;

            case 4U:
                if (m_normalizeValues) {

                    DEFINE_READER(toNormalizedFloat(p[0]), toNormalizedFloat(p[1]), toNormalizedFloat(p[2]), toNormalizedFloat(p[3]));
                }
                else {

                    DEFINE_READER(static_cast<float>(p[0]), static_cast<float>(p[1]), static_cast<float>(p[2]), static_cast<float>(p[3]));
                }
                break;
            }

            #undef DEFINE_READER
        }

    private:
        template<typename T>
        static constexpr float toNormalizedFloat(T t) { return Vector::normalizeInteger(t); }
        static constexpr float toNormalizedFloat(float t) { return t; }
        static constexpr float toNormalizedFloat(double t) { return static_cast<float>(t); }

    private:
        std::function<Vector(unsigned int)> m_reader;
        bool m_isEnabled;
        const char *m_addr;
        unsigned int m_numArgs;
        unsigned int m_stride;
        bool m_normalizeValues;
        bool m_setWTo1;
    };

    using VectorReaderPtr = std::unique_ptr<VectorReader>;



    //
    // Implementation of a vertex data array that is used in glArrayElement() / glDrawElements(), etc.
    //
    class VertexDataArray {

    public:
        VertexDataArray(Vertex &vertexState)

            : m_isLocked(false),
              m_activeTexture(0U),
              m_vertexState(vertexState) {

            m_position = std::make_unique<VectorReader>(false, true);
            m_normal = std::make_unique<VectorReader>(true, false);
            m_color = std::make_unique<VectorReader>(true, true);

            for (auto i = 0U; i < SWGL_MAX_TEXTURE_UNITS; i++) {

                m_texCoord[i] = std::make_unique<VectorReader>(false, true);
            }

            m_prefetchedVertices.resize(MAX_VERTICES);
        }
        ~VertexDataArray() = default;

    public:
        void setActiveTexture(unsigned int activeTexture) {

            m_activeTexture = activeTexture;
        }

    public:
        VectorReader &getPosition() {

            return *m_position;
        }

        VectorReader &getNormal() {

            return *m_normal;
        }

        VectorReader &getColor() {

            return *m_color;
        }

        VectorReader &getTexCoord() {

            return *m_texCoord[m_activeTexture];
        }

        VectorReader &getTexCoord(unsigned int index) {

            return *m_texCoord[index];
        }

    public:
        void lock(unsigned int firstIdx, unsigned int count) {

            m_isLocked = true;

            m_firstIdx = firstIdx;
            m_lastIdx = firstIdx + std::min(count, MAX_VERTICES) - 1U;
        }

        void unlock() {

            m_isLocked = false;
        }

        bool getPrefetchedVertex(unsigned int index) {

            if (m_isLocked && index >= m_firstIdx && index <= m_lastIdx) {

                m_vertexState = m_prefetchedVertices[index - m_firstIdx];
                return true;
            }

            return false;
        }

        void prefetch(Matrix &mvpMatrix, Matrix &mvMatrix) {

            if (m_isLocked) {

                for (auto i = m_firstIdx, j = 0U; i <= m_lastIdx; i++, j++) {

                    auto &v = m_prefetchedVertices[j];

                    // Update position
                    if (m_position->isEnabled()) {

                        v.position = m_position->read(i);
                        v.projected = v.position * mvpMatrix;
                    }
                    else {

                        v.position = m_vertexState.position;
                        v.projected = m_vertexState.projected;
                    }

                    // Update normal
                    if (m_normal->isEnabled()) {

                        v.normal = m_normal->read(i) * mvMatrix;
                    }
                    else {

                        v.normal = m_vertexState.normal;
                    }

                    // Update color
                    if (m_color->isEnabled()) {

                        v.colorPrimary = m_color->read(i);
                    }
                    else {

                        v.colorPrimary = m_vertexState.colorPrimary;
                    }

                    // Update texture coordinate(s)
                    for (auto texUnit = 0U; texUnit < SWGL_MAX_TEXTURE_UNITS; texUnit++) {

                        if (m_texCoord[texUnit]->isEnabled()) {

                            v.texCoord[texUnit] = m_texCoord[texUnit]->read(i);
                        }
                        else {

                            v.texCoord[texUnit] = m_vertexState.texCoord[texUnit];
                        }
                    }
                }
            }
        }

    private:
        VectorReaderPtr m_position;
        VectorReaderPtr m_normal;
        VectorReaderPtr m_color;
        VectorReaderPtr m_texCoord[SWGL_MAX_TEXTURE_UNITS];

    private:
        bool m_isLocked;
        unsigned int m_firstIdx;
        unsigned int m_lastIdx;
        unsigned int m_activeTexture;
        Vertex &m_vertexState;
        VertexList m_prefetchedVertices;

    private:
        static constexpr unsigned int MAX_VERTICES = 4096U;
    };
}
