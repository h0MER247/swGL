#pragma once

#include <vector>
#include <memory>
#include "AlignedAllocator.h"

namespace SWGL {

    // Forward declarations
    class DrawBuffer;

    // Type aliases
    using ColorBuffer = std::vector<unsigned int, AlignedAllocator<unsigned int, 16>>;
    using DepthBuffer = std::vector<float, AlignedAllocator<float, 16>>;
    using DrawBufferPtr = std::shared_ptr<DrawBuffer>;

    //
    // Holds the drawing buffers for one particular thread
    //
    class DrawBuffer {

    public:
        DrawBuffer() = default;
        ~DrawBuffer() = default;

    public:
        void resize(int minX, int minY, int maxX, int maxY) {

            m_minX = minX; m_minY = minY;
            m_maxX = maxX; m_maxY = maxY;

            m_width = maxX - minX;
            m_height = maxY - minY;

            auto size = m_width * m_height;
            m_color.resize(size);
            m_depth.resize(size);
        }

    public:
        int getMinX() { return m_minX; }
        int getMaxX() { return m_maxX; }
        int getMinY() { return m_minY; }
        int getMaxY() { return m_maxY; }

        int getWidth() { return m_width; }
        int getHeight() { return m_height; }

        unsigned int *getColor() { return m_color.data(); }
        float *getDepth() { return m_depth.data(); }

    public:
        void clearColor(unsigned int value) {

            std::fill(m_color.begin(), m_color.end(), value);
        }

        void clearDepth(float value) {

            std::fill(m_depth.begin(), m_depth.end(), value);
        }

    private:
        int m_minY, m_maxY;
        int m_minX, m_maxX;
        int m_width, m_height;

    private:
        ColorBuffer m_color;
        DepthBuffer m_depth;
    };
}
