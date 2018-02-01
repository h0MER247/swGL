#include "MatrixStack.h"

namespace SWGL {

    MatrixStack::MatrixStack()

        : m_activeTexture(0U) {

        for (auto &stack : m_stack) {

            for (auto &matrix : stack.matrix) {

                matrix = Matrix::buildIdentity();
            }
            stack.currentMatrix = &stack.matrix[0];
        }

        setMatrixMode(GL_MODELVIEW);
    }



    void MatrixStack::setMatrixMode(GLenum matrixMode) {

        m_matrixMode = matrixMode;

        switch (matrixMode) {

        case GL_MODELVIEW:
            m_currentStack = &m_stack[STACK_MODELVIEW];
            break;

        case GL_PROJECTION:
            m_currentStack = &m_stack[STACK_PROJECTION];
            break;

        case GL_TEXTURE:
            m_currentStack = &m_stack[STACK_TEXTURE_0 + m_activeTexture];
            break;

        case GL_COLOR:
            m_currentStack = &m_stack[STACK_COLOR];
            break;
        }
    }

    GLenum MatrixStack::getMatrixMode() {

        return m_matrixMode;
    }



    void MatrixStack::setCurrentMatrix(Matrix matrix) {

        *m_currentStack->currentMatrix = matrix;
    }

    void MatrixStack::setActiveTexture(unsigned int activeTexture) {

        m_activeTexture = activeTexture;

        if (m_matrixMode == GL_TEXTURE) {

            m_currentStack = &m_stack[STACK_TEXTURE_0 + m_activeTexture];
        }
    }
    


    bool MatrixStack::push() {

        if (m_currentStack->currentMatrix < m_currentStack->matrix + SWGL_MAX_MATRIXSTACK_DEPTH) {

            auto src = m_currentStack->currentMatrix;
            auto dst = m_currentStack->currentMatrix + 1;

            *dst = *src;

            m_currentStack->currentMatrix++;
            return true;
        }

        return false;
    }

    bool MatrixStack::pop() {

        if (m_currentStack->currentMatrix > m_currentStack->matrix) {

            m_currentStack->currentMatrix--;
            return true;
        }

        return false;
    }




    const Matrix &MatrixStack::getCurrentMatrix() const {

        return *m_currentStack->currentMatrix;
    }

    const Matrix &MatrixStack::getModelViewMatrix() const {

        return *m_stack[STACK_MODELVIEW].currentMatrix;
    }

    const Matrix &MatrixStack::getProjectionMatrix() const {

        return *m_stack[STACK_PROJECTION].currentMatrix;
    }

    const Matrix &MatrixStack::getColorMatrix() const {

        return *m_stack[STACK_COLOR].currentMatrix;
    }

    const Matrix &MatrixStack::getTextureMatrix() const {

        return *m_stack[STACK_TEXTURE_0 + m_activeTexture].currentMatrix;
    }

    const Matrix &MatrixStack::getTextureMatrix(unsigned int idx) const {

        return *m_stack[STACK_TEXTURE_0 + idx].currentMatrix;
    }
}
