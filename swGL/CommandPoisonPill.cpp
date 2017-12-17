#include "DrawThread.h"
#include "CommandPoisonPill.h"

namespace SWGL {

    //
    // A command used to shutdown a thread
    //
    bool CommandPoisonPill::execute(DrawThread *thread) {

        return false;
    }
}
