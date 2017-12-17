#pragma once

#include <memory>

namespace SWGL {

    // Forward declarations
    class DrawThread;
    class CommandBase;

    // Type aliases
    using CommandPtr = std::unique_ptr<CommandBase>;

    //
    // Base class of a draw command
    //
    class CommandBase {

    public:
        CommandBase() = default;
        virtual ~CommandBase() {}

    public:
        virtual bool isFlushingQueue() {
        
            return false;
        };

        virtual int getWorkLoadEstimate() {
        
            return 1;
        }

        virtual bool execute(DrawThread *thread) = 0;
    };
}
