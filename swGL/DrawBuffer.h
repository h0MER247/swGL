#pragma once

#include <vector>
#include <memory>
#include "AlignedAllocator.h"

namespace SWGL {

    // Forward declarations
    class DrawBuffer;

    // Type aliases
    template<typename T>
    using BufferType = std::vector<T, AlignedAllocator<T, 16>>;

    using ColorBuffer = BufferType<unsigned int>;
    using DepthBuffer = BufferType<unsigned int>;
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
            m_size = m_width * m_height;

            m_color.resize(m_size);
            m_depth.resize(m_size);
        }

    public:
        int getMinX() { return m_minX; }
        int getMaxX() { return m_maxX; }
        int getMinY() { return m_minY; }
        int getMaxY() { return m_maxY; }

        int getWidth() { return m_width; }
        int getHeight() { return m_height; }

    public:
        unsigned int *getColor() { return m_color.data(); }
        unsigned int *getDepth() { return m_depth.data(); }

    public:
        void unswizzleColor(unsigned int *dst, int dstWidth) {

            unswizzle(m_color.data(), dst, dstWidth);
        }

        void clearColor(unsigned int value, int minX, int minY, int maxX, int maxY) {

            clear(m_color.data(), value, minX, minY, maxX, maxY);
        }

        void clearDepth(unsigned int value, int minX, int minY, int maxX, int maxY) {

            clear(m_depth.data(), value, minX, minY, maxX, maxY);
        }

    private:
        template<typename T>
        void unswizzle(T *src, T *dst, int dstWidth) {

            // Calculate offsets
            auto startOffset = m_minX + (m_minY * dstWidth);
            auto rowOffset = (dstWidth << 1) - m_width;

            // Unswizzle the data
            auto dstRow1 = dst + startOffset;
            auto dstRow2 = dst + startOffset + dstWidth;

            // TODO: Optimize this later.
            for (int y = 0; y < m_height; y += 2) {

                for (int x = 0; x < m_width; x += 2, src += 4) {

                    *dstRow1++ = src[0]; // (x0,y0)
                    *dstRow1++ = src[1]; // (x1,y0)
                    *dstRow2++ = src[2]; // (x0,y1)
                    *dstRow2++ = src[3]; // (x1,y1)
                }

                dstRow1 += rowOffset;
                dstRow2 += rowOffset;
            }
        }

        template<typename T>
        void clear(T *dst, T value, int minX, int minY, int maxX, int maxY) {

            minX = std::max(minX, m_minX) - m_minX;
            minY = std::max(minY, m_minY) - m_minY;
            maxX = std::min(maxX, m_maxX) - m_minX;
            maxY = std::min(maxY, m_maxY) - m_minY;

            if (minX == 0 && minY == 0 && maxX == m_width && maxY == m_height) {

                std::fill(dst, dst + m_size, value);
            }
            else {

                // TODO: Optimize this later.
                for (int y = minY; y < maxY; y++) {

                    T *p = &dst[((y & 1) << 1) + ((y & ~1) * m_width)];

                    for (int x = minX; x < maxX; x++) {

                        p[((x & ~1) << 1) + (x & 1)] = value;
                    }
                }
            }
        }

    private:
        int m_minY, m_maxY;
        int m_minX, m_maxX;
        int m_width, m_height;
        int m_size;

    private:
        ColorBuffer m_color;
        DepthBuffer m_depth;
    };
}
