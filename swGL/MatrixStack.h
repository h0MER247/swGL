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
        Matrix &getCurrentMatrix() { return *m_currentStack->currentMatrix; }
        Matrix &getModelViewMatrix() { return *m_stack[STACK_MODELVIEW].currentMatrix; }
        Matrix &getProjectionMatrix() { return *m_stack[STACK_PROJECTION].currentMatrix; }
        Matrix &getColorMatrix() { return *m_stack[STACK_COLOR].currentMatrix; }

        // TODO: Each texture unit has its own texture matrix stack... That 
        //       here may have worked for OpenGL 1.1, but it doesn't for 1.3.
        Matrix &getTextureMatrix() { return *m_stack[STACK_TEXTURE].currentMatrix; }

    private:
        GLenum m_matrixMode;

    private:
        struct Stack {

            Matrix matrix[SWGL_MAX_MATRIXSTACK_DEPTH];
            Matrix *currentMatrix;
        };
        Stack m_stack[4];
        Stack *m_currentStack;

    private:
        static constexpr size_t STACK_MODELVIEW = 0U;
        static constexpr size_t STACK_PROJECTION = 1U;
        static constexpr size_t STACK_TEXTURE = 2U;
        static constexpr size_t STACK_COLOR = 3U;
    };
}
