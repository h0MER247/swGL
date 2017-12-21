#include "DrawThread.h"
#include "CommandClearColor.h"

namespace SWGL {

    bool CommandClearColor::execute(DrawThread *thread) {

        thread->getDrawBuffer().clearColor(m_value, m_minX, m_minY, m_maxX, m_maxY);

        return true;
    }
}
