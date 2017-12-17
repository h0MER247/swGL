#include "DrawThread.h"
#include "CommandClearColor.h"

namespace SWGL {

    //
    // Clears the color buffer with a specific color
    //
    bool CommandClearColor::execute(DrawThread *thread) {

        thread->getDrawBuffer().clearColor(m_value);

        return true;
    }
}
