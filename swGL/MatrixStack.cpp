#include "MatrixStack.h"

namespace SWGL {

    MatrixStack::MatrixStack()

        : m_activeTexture(0U) {

        for (auto &stack : m_stack) {

            for (auto &matrix : stack.matrix) {

                matrix = Matrix::getIdentity();
            }
            stack.currentMatrix = &stack.matrix[0];
            stack.wasUpdated = false;
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



    void MatrixStack::updateCurrentMatrixStack() {

        m_currentStack->wasUpdated = true;
    }

    bool MatrixStack::wasMatrixStackUpdated(unsigned int stack) {

        bool wasUpdated = m_stack[stack].wasUpdated;
        m_stack[stack].wasUpdated = false;

        return wasUpdated;
    }



    void MatrixStack::setActiveTexture(unsigned int activeTexture) {

        m_activeTexture = activeTexture;
    }



    Matrix &MatrixStack::getCurrentMatrix() const {

        return *m_currentStack->currentMatrix;
    }

    Matrix &MatrixStack::getModelViewMatrix() const {

        return *m_stack[STACK_MODELVIEW].currentMatrix;
    }

    Matrix &MatrixStack::getProjectionMatrix() const {

        return *m_stack[STACK_PROJECTION].currentMatrix;
    }

    Matrix &MatrixStack::getColorMatrix() const {

        return *m_stack[STACK_COLOR].currentMatrix;
    }

    Matrix &MatrixStack::getTextureMatrix() const {

        return *m_stack[STACK_TEXTURE_0 + m_activeTexture].currentMatrix;
    }

    Matrix &MatrixStack::getTextureMatrix(unsigned int idx) const {

        return *m_stack[STACK_TEXTURE_0 + idx].currentMatrix;
    }
}
