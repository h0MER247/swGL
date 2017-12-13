#pragma once

#include "CommandBase.h"

namespace SWGL {

    //
    // Clears the stencil buffer with a specific stencil value
    //
    class CommandClearStencil : public CommandBase {

    public:
        CommandClearStencil(unsigned char value)

            : m_value(value) {

        }
        ~CommandClearStencil() = default;

    public:
        bool execute(DrawThread *thread) override;

    private:
        unsigned char m_value;
    };
}
