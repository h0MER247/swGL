﻿#include <cmath>
#include <memory>
#include "Vector.h"
#include "Matrix.h"

namespace SWGL {

    Matrix::Matrix(float m00, float m01, float m02, float m03,
                   float m10, float m11, float m12, float m13,
                   float m20, float m21, float m22, float m23,
                   float m30, float m31, float m32, float m33) {

        Matrix &M = *this;

        M(0, 0) = m00; M(0, 1) = m01; M(0, 2) = m02; M(0, 3) = m03;
        M(1, 0) = m10; M(1, 1) = m11; M(1, 2) = m12; M(1, 3) = m13;
        M(2, 0) = m20; M(2, 1) = m21; M(2, 2) = m22; M(2, 3) = m23;
        M(3, 0) = m30; M(3, 1) = m31; M(3, 2) = m32; M(3, 3) = m33;
    }



    // Access to a certain matrix element
    float &Matrix::operator()(int i, int j) {

        return m_data[i][j];
    }

    const float &Matrix::operator()(int i, int j) const {

        return m_data[i][j];
    }



    // Return address of the first matrix element
    const float *Matrix::data() const {

        return &m_data[0][0];
    }



    // Assign matrix
    Matrix &Matrix::operator=(const Matrix &rhs) {

        memcpy(m_data, rhs.m_data, 16 * sizeof(float));
        return *this;
    }



    // Multiply this matrix with another matrix
    Matrix &Matrix::operator*=(const Matrix &rhs) {

        Matrix &M = *this;

        M = M * rhs;
        return M;
    }

    // Multiply a matrix with another matrix
    Matrix operator*(const Matrix &M, const Matrix &N) {

        return Matrix(

            M(0, 0) * N(0, 0) + M(0, 1) * N(1, 0) + M(0, 2) * N(2, 0) + M(0, 3) * N(3, 0),
            M(0, 0) * N(0, 1) + M(0, 1) * N(1, 1) + M(0, 2) * N(2, 1) + M(0, 3) * N(3, 1),
            M(0, 0) * N(0, 2) + M(0, 1) * N(1, 2) + M(0, 2) * N(2, 2) + M(0, 3) * N(3, 2),
            M(0, 0) * N(0, 3) + M(0, 1) * N(1, 3) + M(0, 2) * N(2, 3) + M(0, 3) * N(3, 3),

            M(1, 0) * N(0, 0) + M(1, 1) * N(1, 0) + M(1, 2) * N(2, 0) + M(1, 3) * N(3, 0),
            M(1, 0) * N(0, 1) + M(1, 1) * N(1, 1) + M(1, 2) * N(2, 1) + M(1, 3) * N(3, 1),
            M(1, 0) * N(0, 2) + M(1, 1) * N(1, 2) + M(1, 2) * N(2, 2) + M(1, 3) * N(3, 2),
            M(1, 0) * N(0, 3) + M(1, 1) * N(1, 3) + M(1, 2) * N(2, 3) + M(1, 3) * N(3, 3),

            M(2, 0) * N(0, 0) + M(2, 1) * N(1, 0) + M(2, 2) * N(2, 0) + M(2, 3) * N(3, 0),
            M(2, 0) * N(0, 1) + M(2, 1) * N(1, 1) + M(2, 2) * N(2, 1) + M(2, 3) * N(3, 1),
            M(2, 0) * N(0, 2) + M(2, 1) * N(1, 2) + M(2, 2) * N(2, 2) + M(2, 3) * N(3, 2),
            M(2, 0) * N(0, 3) + M(2, 1) * N(1, 3) + M(2, 2) * N(2, 3) + M(2, 3) * N(3, 3),

            M(3, 0) * N(0, 0) + M(3, 1) * N(1, 0) + M(3, 2) * N(2, 0) + M(3, 3) * N(3, 0),
            M(3, 0) * N(0, 1) + M(3, 1) * N(1, 1) + M(3, 2) * N(2, 1) + M(3, 3) * N(3, 1),
            M(3, 0) * N(0, 2) + M(3, 1) * N(1, 2) + M(3, 2) * N(2, 2) + M(3, 3) * N(3, 2),
            M(3, 0) * N(0, 3) + M(3, 1) * N(1, 3) + M(3, 2) * N(2, 3) + M(3, 3) * N(3, 3)
        );
    }

    // Transform a vector with a matrix
    Vector operator*(const Vector &v, const Matrix &M) {

        return Vector(

            v.x() * M(0, 0) + v.y() * M(0, 1) + v.z() * M(0, 2) + v.w() * M(0, 3),
            v.x() * M(1, 0) + v.y() * M(1, 1) + v.z() * M(1, 2) + v.w() * M(1, 3),
            v.x() * M(2, 0) + v.y() * M(2, 1) + v.z() * M(2, 2) + v.w() * M(2, 3),
            v.x() * M(3, 0) + v.y() * M(3, 1) + v.z() * M(3, 2) + v.w() * M(3, 3)
        );
    }



    // Return the transpose of this matrix
    Matrix Matrix::getTranspose() {

        Matrix &M = *this;
        return Matrix(

            M(0, 0), M(1, 0), M(2, 0), M(3, 0),
            M(0, 1), M(1, 1), M(2, 1), M(3, 1),
            M(0, 2), M(1, 2), M(2, 2), M(3, 2),
            M(0, 3), M(1, 3), M(2, 3), M(3, 3)
        );
    }



    // Return identity matrix
    Matrix Matrix::getIdentity() {

        return Matrix(
        
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    // Return orthogonal projection matrix
    Matrix Matrix::getOrtho(float l, float r, float b, float t, float n, float f) {

        float tx = -((r + l) / (r - l));
        float ty = -((t + b) / (t - b));
        float tz = -((f + n) / (f - n));

        return Matrix(
        
            2.0f / (r - l), 0.0f,            0.0f,           tx,
            0.0f,           2.0f / (t - b),  0.0f,           ty,
            0.0f,           0.0f,           -2.0f / (f - n), tz,
            0.0f,           0.0f,            0.0f,           1.0f
        );
    }

    // Return perspective projection matrix
    Matrix Matrix::getFrustum(float l, float r, float b, float t, float n, float f) {

        float Q = 2.0f * n;
        float A = (r + l) / (r - l);
        float B = (t + b) / (t - b);
        float C = -(f + n) / (f - n);
        float D = -(Q * f) / (f - n);
        
        return Matrix(
        
            Q / (r - l), 0.0f,        A,     0.0f,
            0.0f,        Q / (t - b), B,     0.0f,
            0.0f,        0.0f,        C,     D,
            0.0f,        0.0f,        -1.0f, 0.0f
        );
    }

    // Return rotation matrix
    Matrix Matrix::getRotation(float angle, float x, float y, float z) {

        float invLen = 1.0f / std::sqrt(x * x + y * y + z * z);
        x *= invLen;
        y *= invLen;
        z *= invLen;

        angle = (angle * 3.14159265359f) / 180.0f;
        float c = std::cos(angle);
        float s = std::sin(angle);

        float xc = x * (1.0f - c), xs = x * s;
        float yc = y * (1.0f - c), ys = y * s;
        float zc = z * (1.0f - c), zs = z * s;

        return Matrix(
        
            x * xc + c,  x * yc - zs, x * zc + ys, 0.0f,
            y * xc + zs, y * yc + c,  y * zc - xs, 0.0f,
            x * zc - ys, y * zc + xs, z * zc + c,  0.0f,
            0.0f,        0.0f,        0.0f,        1.0f
        );
    }

    // Return scale matrix
    Matrix Matrix::getScale(float x, float y, float z) {

        return Matrix(
        
            x,    0.0f, 0.0f, 0.0f,
            0.0f, y,    0.0f, 0.0f,
            0.0f, 0.0f, z,    0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    // Return translation matrix
    Matrix Matrix::getTranslation(float x, float y, float z) {

        return Matrix(
       
            1.0f, 0.0f, 0.0f, x,
            0.0f, 1.0f, 0.0f, y,
            0.0f, 0.0f, 1.0f, z,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }
}
