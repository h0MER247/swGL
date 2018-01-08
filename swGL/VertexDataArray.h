#pragma once

#include <algorithm>
#include <functional>
#include "Vector.h"
#include "Vertex.h"
#include "Matrix.h"
#include "OpenGL.h"

namespace SWGL {

    //
    // Reads data from client memory into Vector's
    //
    template<bool normalize>
    class VectorReaderBase {

    public:
        VectorReaderBase() = default;
        virtual ~VectorReaderBase() { }

    public:
        void setSource(const void *addr, GLenum type, size_t stride, size_t numArgs) {

            switch (type) {

            case GL_BYTE:           initSource<GLbyte>(stride, numArgs, normalize); break;
            case GL_UNSIGNED_BYTE:  initSource<GLubyte>(stride, numArgs, normalize); break;
            case GL_SHORT:          initSource<GLshort>(stride, numArgs, normalize); break;
            case GL_UNSIGNED_SHORT: initSource<GLushort>(stride, numArgs, normalize); break;
            case GL_INT:            initSource<GLint>(stride, numArgs, normalize); break;
            case GL_UNSIGNED_INT:   initSource<GLuint>(stride, numArgs, normalize); break;
            case GL_FLOAT:          initSource<GLfloat>(stride, numArgs, normalize); break;
            case GL_DOUBLE:         initSource<GLdouble>(stride, numArgs, normalize); break;
            }

            m_addr = reinterpret_cast<const char *>(addr);
            m_numArgs = numArgs;
            m_hasStateChanged = true;
        }

        void setEnable(bool isEnabled) {

            m_isEnabled = isEnabled;
            m_hasStateChanged = true;
        }

        bool isEnabled() {

            return m_isEnabled;
        }

        bool hasStateChanged() {

            return m_hasStateChanged;
        }

        void updateState() {

            m_hasStateChanged = false;
        }

    public:
        virtual Vector read(size_t index) = 0;

    private:
        template<typename T>
        inline void initSource(size_t stride, size_t numArgs, bool doNormalize) {

            m_stride = (stride == 0U) ? sizeof(T) * numArgs : stride;

            if (doNormalize) {

                m_reader = std::bind(&VectorReaderBase::readValueAndNormalize<T>, this, std::placeholders::_1, std::placeholders::_2);
            }
            else {

                m_reader = std::bind(&VectorReaderBase::readValue<T>, this, std::placeholders::_1, std::placeholders::_2);
            }
        }

    private:
        template<typename T>
        float readValue(const char *addr, size_t arg) {

            return static_cast<float>(reinterpret_cast<const T *>(addr)[arg]);
        }

        template<typename T>
        float readValueAndNormalize(const char *addr, size_t arg) {

            return toNormalizedFloat(reinterpret_cast<const T *>(addr)[arg]);
        }

        template<typename T>
        inline float toNormalizedFloat(T t) { return Vector::normalizeInteger(t); }
        inline float toNormalizedFloat(float t) { return t; }
        inline float toNormalizedFloat(double t) { return static_cast<float>(t); }

    protected:
        std::function<float(const char *, size_t)> m_reader;
        bool m_isEnabled;
        const char *m_addr;
        size_t m_numArgs;
        size_t m_stride;
        bool m_normalize;

    private:
        bool m_hasStateChanged;
    };



    //
    // Reads color values from client memory
    //
    class ColorPointer : public VectorReaderBase<true> {

    public:
        ColorPointer() = default;
        ~ColorPointer() = default;

    public:
        Vector read(size_t index) override {

            auto p = m_addr + (m_stride * index);

            if (m_numArgs == 3) {

                return Vector(m_reader(p, 0U), m_reader(p, 1U), m_reader(p, 2U), 1.0f);
            }
            else {

                return Vector(m_reader(p, 0U), m_reader(p, 1U), m_reader(p, 2U), m_reader(p, 3U));
            }
        }
    };



    //
    // Reads vertex positions from client memory
    //
    class VertexPointer : public VectorReaderBase<false> {

    public:
        VertexPointer() = default;
        ~VertexPointer() = default;

    public:
        Vector read(size_t index) override {

            auto p = m_addr + (m_stride * index);

            if (m_numArgs == 2) {

                return Vector(m_reader(p, 0U), m_reader(p, 1U), 0.0f, 1.0f);
            }
            else if (m_numArgs == 3) {

                return Vector(m_reader(p, 0U), m_reader(p, 1U), m_reader(p, 2U), 1.0f);
            }
            else {

                return Vector(m_reader(p, 0U), m_reader(p, 1U), m_reader(p, 2U), m_reader(p, 3U));
            }
        }
    };



    //
    // Reads texture coordinates from client memory
    //
    class TexCoordPointer : public VectorReaderBase<false> {

    public:
        TexCoordPointer() = default;
        ~TexCoordPointer() = default;

    public:
        Vector read(size_t index) override {

            auto p = m_addr + (m_stride * index);

            if (m_numArgs == 1) {

                return Vector(m_reader(p, 0U), 0.0f, 0.0f, 1.0f);
            }
            else if (m_numArgs == 2) {

                return Vector(m_reader(p, 0U), m_reader(p, 1U), 0.0f, 1.0f);
            }
            else if (m_numArgs == 3) {

                return Vector(m_reader(p, 0U), m_reader(p, 1U), m_reader(p, 2U), 1.0f);
            }
            else {

                return Vector(m_reader(p, 0U), m_reader(p, 1U), m_reader(p, 2U), m_reader(p, 3U));
            }
        }
    };



    //
    // Implementation of a vertex data array that is used in glArrayElement() / glDrawElements(), etc.
    //
    class VertexDataArray {

    public:
        VertexDataArray() {

            m_position.setEnable(false);
            m_color.setEnable(false);
            for (int i = 0; i < SWGL_MAX_TEXTURE_UNITS; i++) {

                m_texCoord[i].setEnable(false);
            }

            m_prefetchedVertices.resize(1024);

            unlock();
        }
        ~VertexDataArray() = default;

    public:
        VertexPointer &getPosition() {
        
            return m_position;
        }

        ColorPointer &getColor() {
        
            return m_color;
        }

        TexCoordPointer &getTexCoord(int unit) {
        
            return m_texCoord[unit];
        }

    // This takes advantage of locked vertex arrays by caching vertices and only updating
    // state that has changed. Not the best implementation but it gives some extra fps.
    public:
        void lock(int firstIdx, int count) {

            m_isLocked = true;

            m_firstIdx = firstIdx;
            m_lastIdx = firstIdx + std::min(count, 1024) - 1;
        }

        void unlock() {

            m_isLocked = false;

            m_firstIdx = -1;
            m_lastIdx = -1;
        }

        bool getPrefetchedVertex(int index, Vertex &vertexOut) {

            if (m_isLocked && index >= m_firstIdx && index <= m_lastIdx) {

                vertexOut = m_prefetchedVertices[index - m_firstIdx];
                return true;
            }

            return false;
        }

        void prefetch(Matrix &mvpMatrix, Vertex &vertexState) {

            if(m_isLocked) {

                for (int i = m_firstIdx, j = 0; i <= m_lastIdx; i++, j++) {

                    auto &v = m_prefetchedVertices[j];

                    // Update color
                    if (!m_color.isEnabled()) {

                        v.color = vertexState.color;
                    }
                    else if (m_color.hasStateChanged()) {

                        v.color = m_color.read(i);
                    }

                    // Update texture coordiante
                    for (int k = 0; k < SWGL_MAX_TEXTURE_UNITS; k++) {

                        if (!m_texCoord[k].isEnabled()) {

                            v.texCoord[k] = vertexState.texCoord[k];
                        }
                        else if (m_texCoord[k].hasStateChanged()) {

                            v.texCoord[k] = m_texCoord[k].read(i);
                        }
                    }

                    // Update position
                    if (!m_position.isEnabled()) {

                        v.obj = vertexState.obj;
                        v.proj = vertexState.proj;
                    }
                    else if (m_position.hasStateChanged()) {

                        v.obj = m_position.read(i);
                        v.proj = v.obj * mvpMatrix;
                    }
                }

                m_color.updateState();
                for (int i = 0; i < SWGL_MAX_TEXTURE_UNITS; i++) {

                    m_texCoord[i].updateState();
                }
                m_position.updateState();
            }
        }

    private:
        VertexPointer m_position;
        ColorPointer m_color;
        TexCoordPointer m_texCoord[SWGL_MAX_TEXTURE_UNITS];

    private:
        bool m_isLocked;
        int m_firstIdx;
        int m_lastIdx;
        VertexList m_prefetchedVertices;
    };
}
