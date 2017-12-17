#include "DrawThread.h"
#include "CommandClearDepth.h"

namespace SWGL {

    //
    // Clears the depth buffer with a specific depth value
    //
    bool CommandClearDepth::execute(DrawThread *thread) {

        thread->getDrawBuffer().clearDepth(m_value);

        return true;
    }
}
