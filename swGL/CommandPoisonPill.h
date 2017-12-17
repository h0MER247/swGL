#pragma once

#include "CommandBase.h"

namespace SWGL {

    //
    // A command used to shutdown a thread
    //
    class CommandPoisonPill : public CommandBase {

    public:
        CommandPoisonPill() = default;
        ~CommandPoisonPill() = default;

    public:
        bool isFlushingQueue() override {

            return true;
        }

        bool execute(DrawThread *thread) override;
    };
}
