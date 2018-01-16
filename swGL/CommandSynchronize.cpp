#include "DrawThread.h"
#include "CountDownLatch.h"
#include "CommandSynchronize.h"

namespace SWGL {

    //
    // Used to synchronize all draw threads with a countdown latch
    //
    bool CommandSynchronize::execute(DrawThread *thread) {

        m_latch.countDown();

        return true;
    }
}
