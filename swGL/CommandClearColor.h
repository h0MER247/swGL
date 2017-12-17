#pragma once

#include "CommandBase.h"

namespace SWGL {

    //
    // Clears the color buffer with a specific color
    //
    class CommandClearColor : public CommandBase {

    public:
        CommandClearColor(unsigned int value)
        
            : m_value(value) {
            
        }
        ~CommandClearColor() = default;

    public:
        bool execute(DrawThread *thread) override;

    private:
        unsigned int m_value;
    };
}
