#include <cmath>
#include <memory>
#include <algorithm>
#include "Vector.h"
#include "Matrix.h"

namespace SWGL {

    Matrix::Matrix(float m00, float m01, float m02, float m03,
                   float m10, float m11, float m12, float m13,
                   float m20, float m21, float m22, float m23,
                   float m30, float m31, float m32, float m33) {

        auto &M = *this;

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



    // Write matrix to a certain memory address
    void Matrix::writeTo(float *addr) const {

        auto src = &m_data[0][0];
        std::copy(src, src + 16, addr);
    }


    // Assign matrix
    Matrix &Matrix::operator=(const Matrix &rhs) {

        rhs.writeTo(&m_data[0][0]);
        return *this;
    }



    // Multiply this matrix with another matrix
    Matrix &Matrix::operator*=(const Matrix &rhs) {

        auto &M = *this;

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



    // Returns true if this matrix is the identity matrix
    bool Matrix::isIdentity() const {

        auto &M = *this;

        return M(0, 0) == 1.0f && M(0, 1) == 0.0f && M(0, 2) == 0.0f && M(0, 3) == 0.0f &&
               M(1, 0) == 0.0f && M(1, 1) == 1.0f && M(1, 2) == 0.0f && M(1, 3) == 0.0f &&
               M(2, 0) == 0.0f && M(2, 1) == 0.0f && M(2, 2) == 1.0f && M(2, 3) == 0.0f &&
               M(3, 0) == 0.0f && M(3, 1) == 0.0f && M(3, 2) == 0.0f && M(3, 3) == 1.0f;
    }

    // Return the transpose of this matrix
    Matrix Matrix::getTranspose() const {

        auto &M = *this;
        return Matrix(

            M(0, 0), M(1, 0), M(2, 0), M(3, 0),
            M(0, 1), M(1, 1), M(2, 1), M(3, 1),
            M(0, 2), M(1, 2), M(2, 2), M(3, 2),
            M(0, 3), M(1, 3), M(2, 3), M(3, 3)
        );
    }

    // Return the transposed inverse of this matrix
    Matrix Matrix::getTransposedInverse() const {

        auto &M = *this;

        // Calculate minors
        float A_ = M(2, 2) * M(3, 3) - M(3, 2) * M(2, 3);
        float B_ = M(1, 2) * M(3, 3) - M(3, 2) * M(1, 3);
        float C_ = M(1, 2) * M(2, 3) - M(2, 2) * M(1, 3);
        float D_ = M(2, 1) * M(3, 3) - M(3, 1) * M(2, 3);
        float E_ = M(1, 1) * M(3, 3) - M(3, 1) * M(1, 3);
        float F_ = M(1, 1) * M(2, 3) - M(2, 1) * M(1, 3);
        float G_ = M(2, 1) * M(3, 2) - M(3, 1) * M(2, 2);
        float H_ = M(1, 1) * M(3, 2) - M(3, 1) * M(1, 2);
        float I_ = M(1, 1) * M(2, 2) - M(2, 1) * M(1, 2);
        float J_ = M(0, 2) * M(3, 3) - M(3, 2) * M(0, 3);
        float K_ = M(0, 2) * M(2, 3) - M(2, 2) * M(0, 3);
        float L_ = M(0, 1) * M(3, 3) - M(3, 1) * M(0, 3);
        float M_ = M(0, 1) * M(2, 3) - M(2, 1) * M(0, 3);
        float N_ = M(0, 1) * M(3, 2) - M(3, 1) * M(0, 2);
        float O_ = M(0, 1) * M(2, 2) - M(2, 1) * M(0, 2);
        float P_ = M(0, 2) * M(1, 3) - M(1, 2) * M(0, 3);
        float Q_ = M(0, 1) * M(1, 3) - M(1, 1) * M(0, 3);
        float R_ = M(0, 1) * M(1, 2) - M(1, 1) * M(0, 2);

        // Calculate transposed inverse matrix
        Matrix I(

             M(1, 1) * A_ - M(2, 1) * B_ + M(3, 1) * C_,
            -M(1, 0) * A_ + M(2, 0) * B_ - M(3, 0) * C_,
             M(1, 0) * D_ - M(2, 0) * E_ + M(3, 0) * F_,
            -M(1, 0) * G_ + M(2, 0) * H_ - M(3, 0) * I_,

            -M(0, 1) * A_ + M(2, 1) * J_ - M(3, 1) * K_,
             M(0, 0) * A_ - M(2, 0) * J_ + M(3, 0) * K_,
            -M(0, 0) * D_ + M(2, 0) * L_ - M(3, 0) * M_,
             M(0, 0) * G_ - M(2, 0) * N_ + M(3, 0) * O_,

             M(0, 1) * B_ - M(1, 1) * J_ + M(3, 1) * P_,
            -M(0, 0) * B_ + M(1, 0) * J_ - M(3, 0) * P_,
             M(0, 0) * E_ - M(1, 0) * L_ + M(3, 0) * Q_,
            -M(0, 0) * H_ + M(1, 0) * N_ - M(3, 0) * R_,

            -M(0, 1) * C_ + M(1, 1) * K_ - M(2, 1) * P_,
             M(0, 0) * C_ - M(1, 0) * K_ + M(2, 0) * P_,
            -M(0, 0) * F_ + M(1, 0) * M_ - M(2, 0) * Q_,
             M(0, 0) * I_ - M(1, 0) * O_ + M(2, 0) * R_
        );

        float rcpDet = 1.0f / (M(0, 0) * I(0, 0) +
                               M(1, 0) * I(1, 0) +
                               M(2, 0) * I(2, 0) +
                               M(3, 0) * I(3, 0));

        I(0, 0) *= rcpDet; I(0, 1) *= rcpDet; I(0, 2) *= rcpDet; I(0, 3) *= rcpDet;
        I(1, 0) *= rcpDet; I(1, 1) *= rcpDet; I(1, 2) *= rcpDet; I(1, 3) *= rcpDet;
        I(2, 0) *= rcpDet; I(2, 1) *= rcpDet; I(2, 2) *= rcpDet; I(2, 3) *= rcpDet;
        I(3, 0) *= rcpDet; I(3, 1) *= rcpDet; I(3, 2) *= rcpDet; I(3, 3) *= rcpDet;

        return I;
    }



    // Return identity matrix
    Matrix Matrix::buildIdentity() {

        return Matrix(
        
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    // Return orthogonal projection matrix
    Matrix Matrix::buildOrtho(float l, float r, float b, float t, float n, float f) {

        float tx = -(r + l) / (r - l);
        float ty = -(t + b) / (t - b);
        float tz = -(f + n) / (f - n);

        return Matrix(
        
            2.0f / (r - l), 0.0f,            0.0f,           tx,
            0.0f,           2.0f / (t - b),  0.0f,           ty,
            0.0f,           0.0f,           -2.0f / (f - n), tz,
            0.0f,           0.0f,            0.0f,           1.0f
        );
    }

    // Return perspective projection matrix
    Matrix Matrix::buildFrustum(float l, float r, float b, float t, float n, float f) {

        float A = (r + l) / (r - l);
        float B = (t + b) / (t - b);
        float C = -(f + n) / (f - n);
        float D = (-2.0f * f * n) / (f - n);
        
        return Matrix(
        
            (2.0f * n) / (r - l), 0.0f,                 A,     0.0f,
            0.0f,                 (2.0f * n) / (t - b), B,     0.0f,
            0.0f,                 0.0f,                 C,     D,
            0.0f,                 0.0f,                 -1.0f, 0.0f
        );
    }

    // Return rotation matrix
    Matrix Matrix::buildRotation(float angle, float x, float y, float z) {

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
    Matrix Matrix::buildScale(float x, float y, float z) {

        return Matrix(
        
            x,    0.0f, 0.0f, 0.0f,
            0.0f, y,    0.0f, 0.0f,
            0.0f, 0.0f, z,    0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    // Return translation matrix
    Matrix Matrix::buildTranslation(float x, float y, float z) {

        return Matrix(
       
            1.0f, 0.0f, 0.0f, x,
            0.0f, 1.0f, 0.0f, y,
            0.0f, 0.0f, 1.0f, z,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    // Return custom matrix from a memory source
    Matrix Matrix::buildFrom(const float *addr) {

        return Matrix(

            addr[ 0], addr[ 1], addr[ 2], addr[ 3],
            addr[ 4], addr[ 5], addr[ 6], addr[ 7],
            addr[ 8], addr[ 9], addr[10], addr[11],
            addr[12], addr[13], addr[14], addr[15]
        );
    }

    Matrix Matrix::buildFrom(const double *addr) {

        return Matrix(

            static_cast<float>(addr[ 0]), static_cast<float>(addr[ 1]), static_cast<float>(addr[ 2]), static_cast<float>(addr[ 3]),
            static_cast<float>(addr[ 4]), static_cast<float>(addr[ 5]), static_cast<float>(addr[ 6]), static_cast<float>(addr[ 7]),
            static_cast<float>(addr[ 8]), static_cast<float>(addr[ 9]), static_cast<float>(addr[10]), static_cast<float>(addr[11]),
            static_cast<float>(addr[12]), static_cast<float>(addr[13]), static_cast<float>(addr[14]), static_cast<float>(addr[15])
        );
    }
}
