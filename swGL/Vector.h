#pragma once

#include <type_traits>
#include <limits>
#include "OpenGL.h"
#include "Defines.h"

namespace SWGL {

    //
    //
    //
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
        Vector &operator*=(const float &rhs);
        friend Vector operator*(const Vector &lhs, const float &rhs);
        friend Vector operator-(const Vector &lhs, const Vector &rhs);
        friend Vector operator+(const Vector &lhs, const Vector &rhs);

    public:
        static Vector lerp(const Vector &vA, const Vector &vB, const float t);
        
        static float dot(const Vector &vA, const Vector &vB);
        static float length(const Vector &v);
        static Vector normalize(const Vector &v);
        
        static float dot3(const Vector &vA, const Vector &vB);
        static float length3(const Vector &v);
        static Vector normalize3(const Vector &v);

    public:
        template<typename T>
        static INLINED std::enable_if_t<std::is_integral<T>::value && std::is_signed<T>::value, float> normalizeInteger(T value) {

            static constexpr auto max = static_cast<float>(std::numeric_limits<T>::max());
            static constexpr auto min = static_cast<float>(std::numeric_limits<T>::min());

            return (static_cast<float>(value) * 2.0f + 1.0f) / (max - min);
        }

        template<typename T>
        static INLINED std::enable_if_t<std::is_integral<T>::value && std::is_unsigned<T>::value, float> normalizeInteger(T value) {

            static constexpr auto max = static_cast<float>(std::numeric_limits<T>::max());

            return static_cast<float>(value) / max;
        }

    private:
        float m_data[4];
    };
}
