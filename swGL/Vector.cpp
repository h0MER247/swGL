#include <cmath>
#include "Vector.h"

namespace SWGL {

    Vector::Vector(float x, float y, float z, float w) {

        m_data[0] = w;
        m_data[1] = z;
        m_data[2] = y;
        m_data[3] = x;
    }



    // Access to a certain vector element
    float &Vector::x() { return m_data[3]; }
    float &Vector::y() { return m_data[2]; }
    float &Vector::z() { return m_data[1]; }
    float &Vector::w() { return m_data[0]; }
    const float &Vector::x() const { return m_data[3]; }
    const float &Vector::y() const { return m_data[2]; }
    const float &Vector::z() const { return m_data[1]; }
    const float &Vector::w() const { return m_data[0]; }

    float &Vector::r() { return m_data[3]; }
    float &Vector::g() { return m_data[2]; }
    float &Vector::b() { return m_data[1]; }
    float &Vector::a() { return m_data[0]; }
    const float &Vector::r() const { return m_data[3]; }
    const float &Vector::g() const { return m_data[2]; }
    const float &Vector::b() const { return m_data[1]; }
    const float &Vector::a() const { return m_data[0]; }

    float &Vector::operator[](int idx) { return m_data[idx]; }
    const float &Vector::operator[](int idx) const { return m_data[idx]; }



    // Scalar multiplication
    Vector &Vector::operator*=(const float &rhs) {

        m_data[0] *= rhs;
        m_data[1] *= rhs;
        m_data[2] *= rhs;
        m_data[3] *= rhs;

        return *this;
    }

    // Scalar multiplication
    Vector operator*(const Vector &lhs, const float &rhs) {

        return Vector(

            lhs.x() * rhs,
            lhs.y() * rhs,
            lhs.z() * rhs,
            lhs.w() * rhs
        );
    }

    // Vector subtraction
    Vector operator-(const Vector &lhs, const Vector &rhs) {

        return Vector(

            lhs.x() - rhs.x(),
            lhs.y() - rhs.y(),
            lhs.z() - rhs.z(),
            lhs.w() - rhs.w()
        );
    }

    // Vector addition
    Vector operator+(const Vector &lhs, const Vector &rhs) {

        return Vector(

            lhs.x() + rhs.x(),
            lhs.y() + rhs.y(),
            lhs.z() + rhs.z(),
            lhs.w() + rhs.w()
        );
    }



    // Linear interpolation between two vectors
    Vector Vector::lerp(const Vector &vA, const Vector &vB, const float t) {

        return Vector(

            vA.x() + t * (vB.x() - vA.x()),
            vA.y() + t * (vB.y() - vA.y()),
            vA.z() + t * (vB.z() - vA.z()),
            vA.w() + t * (vB.w() - vA.w())
        );
    }

    // Dot product of two vectors
    float Vector::dot(const Vector &vA, const Vector &vB) {

        return vA.x() * vB.x() +
               vA.y() * vB.y() +
               vA.z() * vB.z() +
               vA.w() * vB.w();
    }

    float Vector::dot3(const Vector &vA, const Vector &vB) {

        return vA.x() * vB.x() +
               vA.y() * vB.y() +
               vA.z() * vB.z();
    }

    // Length of a vector with three components
    float Vector::length(const Vector &v) {

        return std::sqrt(

            v.x() * v.x() +
            v.y() * v.y() +
            v.z() * v.z() +
            v.w() * v.w()
        );
    }

    float Vector::length3(const Vector &v) {

        return std::sqrt(

            v.x() * v.x() +
            v.y() * v.y() +
            v.z() * v.z()
        );
    }

    // Normalizes a vector with three components,
    Vector Vector::normalize(const Vector &v) {

        float rcpLength = 1.0f / Vector::length(v);

        return Vector(

            v.x() * rcpLength,
            v.y() * rcpLength,
            v.z() * rcpLength,
            v.w() * rcpLength
        );
    }

    Vector Vector::normalize3(const Vector &v) {

        float rcpLength = 1.0f / Vector::length3(v);

        return Vector(

            v.x() * rcpLength,
            v.y() * rcpLength,
            v.z() * rcpLength,
            0.0f
        );
    }
}
