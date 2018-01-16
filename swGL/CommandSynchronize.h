#pragma once

#include "CommandBase.h"

namespace SWGL {

    //
    // Used to synchronize all draw threads with a countdown latch
    //
    class CommandSynchronize : public CommandBase {

    public:
        CommandSynchronize(CountDownLatch &latch)
        
            : m_latch(latch) {
                
        }
        ~CommandSynchronize() = default;

    public:
        bool isFlushingQueue() override {

            return true;
        }

        bool execute(DrawThread *thread) override;

    private:
        CountDownLatch &m_latch;
    };
}
