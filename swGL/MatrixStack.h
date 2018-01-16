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
        bool canPush();
        void push();
        bool canPop();
        void pop();

    public:
        void updateCurrentMatrixStack();
        bool wasMatrixStackUpdated(unsigned int stack);

    public:
        void setActiveTexture(unsigned int activeTexture);

    public:
        Matrix & getCurrentMatrix() const;
        Matrix &getModelViewMatrix() const;
        Matrix &getProjectionMatrix() const;
        Matrix &getColorMatrix() const;
        Matrix &getActiveTextureMatrix() const;
        Matrix &getTextureMatrix(unsigned int idx) const;

    private:
        unsigned int m_activeTexture;
        GLenum m_matrixMode;

    private:
        struct Stack {

            Matrix matrix[SWGL_MAX_MATRIXSTACK_DEPTH];
            Matrix *currentMatrix;
            bool wasUpdated;
        };
        Stack m_stack[3 + SWGL_MAX_TEXTURE_UNITS];
        Stack *m_currentStack;

    public:
        static constexpr unsigned int STACK_MODELVIEW = 0U;
        static constexpr unsigned int STACK_PROJECTION = 1U;
        static constexpr unsigned int STACK_COLOR = 2U;
        static constexpr unsigned int STACK_TEXTURE_0 = 3U;
    };
}
