#pragma once

#include "Defines.h"
#include "OpenGL.h"
#include "Matrix.h"

namespace SWGL {

    //
    // Implements OpenGL's matrix stack
    //
    class MatrixStack {

    public:
        MatrixStack();
        ~MatrixStack() = default;

    public:
        void setMatrixMode(GLenum matrixMode);
        GLenum getMatrixMode();

    public:
        void setCurrentMatrix(Matrix matrix);
        void setActiveTexture(unsigned int activeTexture);

    public:
        bool push();
        bool pop();

    public:
        const Matrix &getCurrentMatrix() const;
        const Matrix &getModelViewMatrix() const;
        const Matrix &getProjectionMatrix() const;
        const Matrix &getColorMatrix() const;
        const Matrix &getTextureMatrix() const;
        const Matrix &getTextureMatrix(unsigned int idx) const;

    private:
        unsigned int m_activeTexture;
        GLenum m_matrixMode;

    private:
        struct Stack {

            Matrix matrix[SWGL_MAX_MATRIXSTACK_DEPTH];
            Matrix *currentMatrix;
        };
        Stack m_stack[3 + SWGL_MAX_TEXTURE_UNITS];
        Stack *m_currentStack;

    private:
        static constexpr unsigned int STACK_MODELVIEW = 0U;
        static constexpr unsigned int STACK_PROJECTION = 1U;
        static constexpr unsigned int STACK_COLOR = 2U;
        static constexpr unsigned int STACK_TEXTURE_0 = 3U;
    };
}
