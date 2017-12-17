#include "DrawThread.h"
#include "CommandClearStencil.h"

namespace SWGL {

    //
    // Clears the stencil buffer with a specific stencil value
    //
    bool CommandClearStencil::execute(DrawThread *thread) {

        thread->getDrawBuffer().clearStencil(m_value);

        return true;
    }
}
