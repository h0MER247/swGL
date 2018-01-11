#pragma once

#include <stdint.h>
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
        void updateCurrentMatrixStack() {

            m_currentStack->wasUpdated = true;
        }

        bool wasMatrixStackUpdated(size_t stack) {

            bool wasUpdated = m_stack[stack].wasUpdated;
            m_stack[stack].wasUpdated = false;

            return wasUpdated;
        }

    public:
        void setActiveTexture(size_t activeTexture) {

            m_activeTexture = activeTexture;
        }

    public:
        Matrix &getCurrentMatrix() const { return *m_currentStack->currentMatrix; }
        Matrix &getModelViewMatrix() const { return *m_stack[STACK_MODELVIEW].currentMatrix; }
        Matrix &getProjectionMatrix() const { return *m_stack[STACK_PROJECTION].currentMatrix; }
        Matrix &getColorMatrix() const { return *m_stack[STACK_COLOR].currentMatrix; }
        Matrix &getTextureMatrix() const { return *m_stack[STACK_TEXTURE_0 + m_activeTexture].currentMatrix; }

    private:
        size_t m_activeTexture;
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
        static constexpr size_t STACK_MODELVIEW = 0U;
        static constexpr size_t STACK_PROJECTION = 1U;
        static constexpr size_t STACK_COLOR = 2U;
        static constexpr size_t STACK_TEXTURE_0 = 3U;
    };
}
