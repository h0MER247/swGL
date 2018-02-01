#pragma once

namespace SWGL {

    // Forward declarations
    class Vector;

    //
    // Matrix math helper
    //
    class Matrix {

    public:
        Matrix() = default;
        Matrix(float m00, float m01, float m02, float m03,
               float m10, float m11, float m12, float m13,
               float m20, float m21, float m22, float m23,
               float m30, float m31, float m32, float m33);
        ~Matrix() = default;

    public:
        float &operator()(int i, int j);
        const float &operator()(int i, int j) const;

    public:
        void writeTo(float *addr) const;

    public:
        Matrix &operator=(const Matrix &rhs);

    public:
        Matrix &operator*=(const Matrix &rhs);
        friend Matrix operator*(const Matrix &lhs, const Matrix &rhs);
        friend Vector operator*(const Vector &lhs, const Matrix &rhs);

    public:
        bool isIdentity() const;
        Matrix getTranspose() const;
        Matrix getTransposedInverse() const;

    public:
        static Matrix buildIdentity();
        static Matrix buildOrtho(float left, float right, float bottom, float top, float zNear, float zFar);
        static Matrix buildFrustum(float left, float right, float bottom, float top, float zNear, float zFar);
        static Matrix buildRotation(float angle, float x, float y, float z);
        static Matrix buildScale(float x, float y, float z);
        static Matrix buildTranslation(float x, float y, float z);
        static Matrix buildFrom(const float *addr);
        static Matrix buildFrom(const double *addr);

    private:
        float m_data[4][4];
    };
}
