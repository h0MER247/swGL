#pragma once

#include "OpenGL.h"
#include "Defines.h"

namespace SWGL {

    class Vector {

    public:
        Vector(float x, float y, float z, float w);
        Vector() = default;
        ~Vector() = default;

    public:
        float &x();
        float &y();
        float &z();
        float &w();
        const float &x() const;
        const float &y() const;
        const float &z() const;
        const float &w() const;

        float &r();
        float &g();
        float &b();
        float &a();
        const float &r() const;
        const float &g() const;
        const float &b() const;
        const float &a() const;

    public:
        float &operator[](int idx);
        const float &operator[](int idx) const;

    public:
        const float *data() const;

    public:
        Vector &operator*=(const float &rhs);

    public:
        static Vector lerp(const Vector &vA, const Vector &vB, const float t);
        static float dot(const Vector &vA, const Vector &vB);

    public:
        static INLINED float normalizeInteger(GLuint value) {
        
            // Normalizes [0,4294967295] to [0,1]
            return static_cast<float>(value) / 4294967295.0f;
        }
        static INLINED float normalizeInteger(GLushort value) {

            // Normalizes [0,65535] to [0,1]
            return static_cast<float>(value) / 65535.0f;
        }
        static INLINED float normalizeInteger(GLubyte value) {

            // Normalizes [0,255] to [0,1]
            return static_cast<float>(value) / 255.0f;
        }
        static INLINED float normalizeInteger(GLint value) {

            // Normalizes [-2147483648,2147483647] to [-1,1]
            return -1.0f + ((2.0f * (static_cast<float>(value) + 2147483648.0f)) / 4294967295.0f);
        }
        static INLINED float normalizeInteger(GLshort value) {

            // Normalizes [-32768,32767] to [-1,1]
            return -1.0f + ((2.0f * (static_cast<float>(value) + 32768.0f)) / 65535.0f);
        }
        static INLINED float normalizeInteger(GLbyte value) {

            // Normalizes [-128,127] to [-1,1]
            return -1.0f + ((2.0f * (static_cast<float>(value) + 128.0f)) / 255.0f);
        }

    private:
        float m_data[4];
    };
}
