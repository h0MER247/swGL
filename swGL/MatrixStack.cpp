#include "Log.h"
#include "MatrixStack.h"

namespace SWGL {

    MatrixStack::MatrixStack() {
        
        for(int i = 0; i < 4; i++)
            m_stack[i].currentMatrix = &m_stack[i].matrix[0];

        setMatrixMode(GL_MODELVIEW);
    }



    void MatrixStack::setMatrixMode(GLenum matrixMode) {

        m_matrixMode = matrixMode;

        switch(matrixMode) {

            case GL_MODELVIEW: m_currentStack = &m_stack[STACK_MODELVIEW]; break;
            case GL_PROJECTION: m_currentStack = &m_stack[STACK_PROJECTION]; break;
            case GL_TEXTURE: m_currentStack = &m_stack[STACK_TEXTURE]; break;
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
