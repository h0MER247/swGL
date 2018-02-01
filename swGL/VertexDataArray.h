#pragma once

#include <algorithm>
#include <functional>
#include <memory>
#include "Vector.h"
#include "Vertex.h"
#include "TexCoordGen.h"
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

        bool isEnabled() const {

            return m_isEnabled;
        }

    public:
        Vector read(unsigned int index) const {

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
    using IndexSupplier = std::function<unsigned int(unsigned int)>;

    //
    // Implementation of a vertex data array that is used in glArrayElement() / glDrawElements(), etc.
    //
    class VertexDataArray {

    public:
        VertexDataArray(MatrixStack &matrixStack, TexCoordGen &texGen)

            : m_activeTexture(0U),
              m_matrixStack(matrixStack),
              m_texGen(texGen) {

            m_position = std::make_unique<VectorReader>(false, true);
            m_normal = std::make_unique<VectorReader>(true, false);
            m_color = std::make_unique<VectorReader>(true, true);

            for (auto i = 0U; i < SWGL_MAX_TEXTURE_UNITS; i++) {

                m_texCoord[i] = std::make_unique<VectorReader>(false, true);
            }
        }
        ~VertexDataArray() = default;

    public:
        void setActiveTexture(unsigned int activeTexture) {

            m_activeTexture = activeTexture;
        }

        void setSourcePointer(GLenum source, const void *addr, GLenum type, unsigned int stride, unsigned int numArgs) {

            switch (source) {

            case GL_VERTEX_ARRAY:
                m_position->setSource(addr, type, stride, numArgs);
                break;

            case GL_NORMAL_ARRAY:
                m_normal->setSource(addr, type, stride, numArgs);
                break;

            case GL_COLOR_ARRAY:
                m_color->setSource(addr, type, stride, numArgs);
                break;

            case GL_TEXTURE_COORD_ARRAY:
                m_texCoord[m_activeTexture]->setSource(addr, type, stride, numArgs);
                break;
            }
        }

        void setSourceEnable(GLenum source, bool isEnabled) {

            switch (source) {

            case GL_VERTEX_ARRAY:
                m_position->setEnable(isEnabled);
                break;

            case GL_NORMAL_ARRAY:
                m_normal->setEnable(isEnabled);
                break;

            case GL_COLOR_ARRAY:
                m_color->setEnable(isEnabled);
                break;

            case GL_TEXTURE_COORD_ARRAY:
                m_texCoord[m_activeTexture]->setEnable(isEnabled);
                break;
            }
        }

    public:
        inline bool isVertexColorEnabled() const {

            return m_color->isEnabled();
        }

        inline bool isVertexNormalEnabled() const {

            return m_normal->isEnabled();
        }

        inline bool isVertexPositionEnabled() const {

            return m_position->isEnabled();
        }

        inline bool isVertexTexCoordEnabled(unsigned int texUnit) const {

            return m_texCoord[texUnit]->isEnabled();
        }

    public:
        inline Vector getVertexColor(unsigned int index) {

            return m_color->read(index);
        }

        inline Vector getVertexNormal(unsigned int index) {

            return m_normal->read(index);
        }

        inline Vector getVertexPosition(unsigned int index) {

            return m_position->read(index);
        }

        inline Vector getVertexTexCoord(unsigned int index, unsigned int texUnit) {

            return m_texCoord[texUnit]->read(index);
        }

    public:
        void fetchVertices(Vertex currentState, VertexList &vertices, unsigned int first, unsigned int count, IndexSupplier indexSupplier) {

            // Reset vertex post transform cache
            for (auto &cacheEntry : m_postTransformCache) {

                cacheEntry.index = 0xffffffff;
            }

            //
            // Read vertices
            //
            for (auto i = first, n = first + count; i < n; i++) {

                // Get the index to the vertex
                auto idx = indexSupplier(i);


                // Look inside the post transform cache first
                auto &cacheEntry = m_postTransformCache[idx & 0x1f];
                if (cacheEntry.index == idx) {

                    vertices.emplace_back(cacheEntry.vertex);
                }
                else {

                    // Position
                    currentState.posObj = getVertexPosition(idx);
                    currentState.posEye = currentState.posObj * m_matrixStack.getModelViewMatrix();
                    currentState.posProj = currentState.posEye * m_matrixStack.getProjectionMatrix();

                    // Normal
                    if (isVertexNormalEnabled()) {

                        currentState.normal = getVertexNormal(idx) * m_matrixStack.getModelViewMatrix();
                    }

                    // Color
                    if (isVertexColorEnabled()) {

                        currentState.colorPrimary = getVertexColor(idx);
                    }

                    // Texture coordinates
                    for (auto texUnit = 0U; texUnit < SWGL_MAX_TEXTURE_UNITS; texUnit++) {

                        if (m_texGen.isEnabled(texUnit)) {

                            m_texGen.generate(currentState, texUnit);
                            currentState.texCoord[texUnit] = currentState.texCoord[texUnit] * m_matrixStack.getTextureMatrix(texUnit);
                        }
                        else if (isVertexTexCoordEnabled(texUnit)) {

                            currentState.texCoord[texUnit] = getVertexTexCoord(idx, texUnit);
                            currentState.texCoord[texUnit] = currentState.texCoord[texUnit] * m_matrixStack.getTextureMatrix(texUnit);
                        }
                    }

                    // Add vertex to the cache
                    cacheEntry.index = idx;
                    cacheEntry.vertex = currentState;

                    // Add vertex
                    vertices.emplace_back(currentState);
                }
            }
        }

    private:
        VectorReaderPtr m_position;
        VectorReaderPtr m_normal;
        VectorReaderPtr m_color;
        VectorReaderPtr m_texCoord[SWGL_MAX_TEXTURE_UNITS];

    private:
        unsigned int m_activeTexture;

    private:
        MatrixStack &m_matrixStack;
        TexCoordGen &m_texGen;

    private:
        struct CachedVertex {

            unsigned int index;
            Vertex vertex;
        };
        CachedVertex m_postTransformCache[32];
    };
}
