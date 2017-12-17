#pragma once

#include "CommandBase.h"

namespace SWGL {

    //
    // Used to synchronize all draw threads with a countdown latch
    //
    class CommandSyncPoint : public CommandBase {

    public:
        CommandSyncPoint(CountDownLatch &latch)
        
            : m_latch(latch) {
                
        }
        ~CommandSyncPoint() = default;

    public:
        bool isFlushingQueue() override {

            return true;
        }

        bool execute(DrawThread *thread) override;

    private:
        CountDownLatch &m_latch;
    };
}
