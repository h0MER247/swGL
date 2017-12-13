#include <memory>
#include "Defines.h"
#include "DrawSurface.h"

namespace SWGL {

#if !SWGL_USE_HARDWARE_GAMMA
    SWGammaRamp DrawSurface::m_gammaRamp;
#endif

    DrawSurface::DrawSurface() {

        for (int i = 0; i < SWGL_NUM_DRAW_THREADS; i++) {

            m_buffer[i] = std::make_shared<DrawBuffer>();
        }
    }



    void DrawSurface::setHDC(HDC hdc) {

        m_hdc = hdc;

        // Get the dimenions of the client window
        RECT r;
        GetClientRect(WindowFromDC(m_hdc), &r);
        m_clientWidth = (r.right - r.left + 1) & ~1;
        m_clientHeight = (r.bottom - r.top + 1) & ~1;

        // Init storage in which the unswizzled color buffer gets written into
        m_unswizzledColor.resize(m_clientWidth * m_clientHeight);

        // Setup bitmap info structure which is needed for SetDIBitsToDevice()
        memset(&m_bmi, 0, sizeof(BITMAPINFO));
        m_bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
        m_bmi.bmiHeader.biWidth = m_clientWidth;
        m_bmi.bmiHeader.biHeight = m_clientHeight;
        m_bmi.bmiHeader.biPlanes = 1;
        m_bmi.bmiHeader.biBitCount = 32;
        m_bmi.bmiHeader.biCompression = BI_RGB;

        // Calculate how often we can subdivide the drawing buffer with respect
        // to the number of drawing threads
        int numDivX = 1, numDivY = 1;
        for (int n = SWGL_NUM_DRAW_THREADS; n > 1; ) {

            numDivX++; n >>= 1;
            if (n == 1) break;
            numDivY++; n >>= 1;
        }

        // Initialize the buffer for each drawing thread
        m_numBuffersInX = 1 << (numDivX - 1);
        m_numBuffersInY = 1 << (numDivY - 1);
        m_bufferWidth = m_clientWidth / m_numBuffersInX;
        m_bufferHeight = m_clientHeight / m_numBuffersInY;

        for (int y = 0, idx = 0; y < m_numBuffersInY; y++) {

            for (int x = 0; x < m_numBuffersInX; x++) {

                auto minX = x * m_bufferWidth;
                auto minY = y * m_bufferHeight;
                auto maxX = (x == m_numBuffersInX - 1) ? m_clientWidth : (x + 1) * m_bufferWidth;
                auto maxY = (y == m_numBuffersInY - 1) ? m_clientHeight : (y + 1) * m_bufferHeight;

                m_buffer[idx++]->resize(minX, minY, maxX, maxY);
            }
        }
    }



    void DrawSurface::swap() {

        unswizzleColorBuffer();

#if !SWGL_USE_HARDWARE_GAMMA
        m_gammaRamp.correct(

            m_unswizzledColor.data(), m_unswizzledColor.size()
        );
#endif

        SetDIBitsToDevice(

            m_hdc,
            0, 0,
            m_clientWidth, m_clientHeight,
            0, 0,
            0, m_clientHeight,
            reinterpret_cast<void *>(m_unswizzledColor.data()),
            &m_bmi,
            DIB_RGB_COLORS
        );
    }



    void DrawSurface::unswizzleColorBuffer() {

        for (auto &buffer : m_buffer) {

            auto height = buffer->getHeight();
            auto width = buffer->getWidth();
            auto minX = buffer->getMinX();
            auto minY = buffer->getMinY();
            auto data = buffer->getColor();

            // Calculate offsets
            size_t startOffset = minX + (minY * m_clientWidth);
            size_t rowOffset = (m_clientWidth << 1) - width;

            // Unswizzle the data (TODO: Maybe SIMDify this?!)
            auto dstRow1 = m_unswizzledColor.data() + startOffset;
            auto dstRow2 = m_unswizzledColor.data() + startOffset + m_clientWidth;

            for (int y = 0; y < height; y += 2) {

                for (int x = 0; x < width; x += 2, data += 4) {

                    *dstRow1++ = data[3];
                    *dstRow1++ = data[2];
                    *dstRow2++ = data[1];
                    *dstRow2++ = data[0];
                }

                dstRow1 += rowOffset;
                dstRow2 += rowOffset;
            }
        }
    }
}
