#pragma once

#include "CommandBase.h"

namespace SWGL {

    //
    // Clears the color buffer with a specific color
    //
    class CommandClearColor : public CommandBase {

    public:
        CommandClearColor(unsigned int value, int minX, int minY, int maxX, int maxY)

            : m_value(value),
              m_minX(minX),
              m_minY(minY),
              m_maxX(maxX),
              m_maxY(maxY) {

        }
        ~CommandClearColor() = default;

    public:
        bool execute(DrawThread *thread) override;

    private:
        unsigned int m_value;
        int m_minX, m_minY;
        int m_maxX, m_maxY;
    };
}
