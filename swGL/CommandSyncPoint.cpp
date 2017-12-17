#include "DrawThread.h"
#include "CountDownLatch.h"
#include "CommandSyncPoint.h"

namespace SWGL {

    //
    // Used to synchronize all draw threads with a countdown latch
    //
    bool CommandSyncPoint::execute(DrawThread *thread) {

        m_latch.countDown();

        return true;
    }
}
