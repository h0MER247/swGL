#include "Log.h"
#include "MatrixStack.h"

namespace SWGL {

    MatrixStack::MatrixStack()

        : m_activeTexture(0) {

        for (int i = 0; i < 3 + SWGL_MAX_TEXTURE_UNITS; i++) {

            auto &stack = m_stack[i];

            for (int j = 0; j < SWGL_MAX_MATRIXSTACK_DEPTH; j++) {

                stack.matrix[j] = Matrix::getIdentity();
            }
            stack.currentMatrix = &stack.matrix[0];
            stack.wasUpdated = false;
        }

        setMatrixMode(GL_MODELVIEW);
    }



    void MatrixStack::setMatrixMode(GLenum matrixMode) {

        m_matrixMode = matrixMode;

        switch (matrixMode) {

        case GL_MODELVIEW: m_currentStack = &m_stack[STACK_MODELVIEW]; break;
        case GL_PROJECTION: m_currentStack = &m_stack[STACK_PROJECTION]; break;
        case GL_TEXTURE: m_currentStack = &m_stack[STACK_TEXTURE_0 + m_activeTexture]; break;
        case GL_COLOR: m_currentStack = &m_stack[STACK_COLOR]; break;
        }
    }

    GLenum MatrixStack::getMatrixMode() {

        return m_matrixMode;
    }



    bool MatrixStack::canPush() {

        return m_currentStack->currentMatrix < m_currentStack->matrix + SWGL_MAX_MATRIXSTACK_DEPTH;
    }

    void MatrixStack::push() {

        Matrix *srcMatrix = m_currentStack->currentMatrix;
        Matrix *dstMatrix = m_currentStack->currentMatrix + 1;

        *dstMatrix = *srcMatrix;
        m_currentStack->currentMatrix++;
    }

    bool MatrixStack::canPop() {

        return m_currentStack->currentMatrix > m_currentStack->matrix;
    }

    void MatrixStack::pop() {

        m_currentStack->currentMatrix--;
    }
}
