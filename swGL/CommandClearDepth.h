#pragma once

#include "CommandBase.h"

namespace SWGL {

    //
    // Clears the depth buffer with a specific depth value
    //
    class CommandClearDepth : public CommandBase {

    public:
        CommandClearDepth(float value, int minX, int minY, int maxX, int maxY)

            : m_value(value),
              m_minX(minX),
              m_minY(minY),
              m_maxX(maxX),
              m_maxY(maxY) {

        }
        ~CommandClearDepth() = default;

    public:
        bool execute(DrawThread *thread) override;

    private:
        float m_value;
        int m_minX, m_minY;
        int m_maxX, m_maxY;
    };
}
