#pragma once

#include "CommandBase.h"

namespace SWGL {

    //
    // Clears the depth buffer with a specific depth value
    //
    class CommandClearDepth : public CommandBase {

    public:
        CommandClearDepth(float value)
        
            : m_value(value) {
            
        }
        ~CommandClearDepth() = default;

    public:
        bool execute(DrawThread *thread) override;

    private:
        float m_value;
    };
}
