#include <memory>
#include "Defines.h"
#include "Log.h"
#include "DrawSurface.h"

namespace SWGL {

#if !SWGL_USE_HARDWARE_GAMMA
    SWGammaRamp DrawSurface::m_gammaRamp;
#endif

    DrawSurface::DrawSurface()
    
        : m_width(0),
          m_height(0) {

        for (int i = 0; i < SWGL_NUM_DRAW_THREADS; i++) {

            m_buffer[i] = std::make_shared<DrawBuffer>();
        }
    }



    void DrawSurface::setHDC(HDC hdc) {

        m_hdc = hdc;

        RECT r;
        GetClientRect(WindowFromDC(m_hdc), &r);

        auto width = r.right - r.left;
        auto height = r.bottom - r.top;

        // Resize the drawing surface if needed
        if(width != m_width || height != m_height) {

            m_width = width;
            m_height = height;

            // Make sure that width and height are multiples of two. This is needed as 
            // the drawing surfaces uses a quad pixel layout.
            width = (width + 1) & ~1;
            height = (height + 1) & ~1;

            // Init storage in which the unswizzled color buffer gets written into
            m_unswizzledColor.resize(width * height);

            // Setup bitmap info structure which is needed for SetDIBitsToDevice()
            memset(&m_bmi, 0, sizeof(BITMAPINFO));
            m_bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
            m_bmi.bmiHeader.biWidth = width;
            m_bmi.bmiHeader.biHeight = height;
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
            m_bufferWidth = width / m_numBuffersInX;
            m_bufferHeight = height / m_numBuffersInY;

            for (int y = 0, idx = 0; y < m_numBuffersInY; y++) {

                for (int x = 0; x < m_numBuffersInX; x++) {

                    auto minX = x * m_bufferWidth;
                    auto minY = y * m_bufferHeight;
                    auto maxX = (x == m_numBuffersInX - 1) ? width : (x + 1) * m_bufferWidth;
                    auto maxY = (y == m_numBuffersInY - 1) ? height : (y + 1) * m_bufferHeight;

                    m_buffer[idx++]->resize(minX, minY, maxX, maxY);
                }
            }
        }
    }



    void DrawSurface::swap() {

        // Unswizzle color buffer
        auto width = m_bmi.bmiHeader.biWidth;
        auto height = m_bmi.bmiHeader.biHeight;
        auto dst = m_unswizzledColor.data();

        for (auto &buffer : m_buffer) {

            buffer->unswizzleColor(dst, width);
        }

#if !SWGL_USE_HARDWARE_GAMMA
        // Software emulated Gamma correction
        m_gammaRamp.correct(

            m_unswizzledColor.data(), m_unswizzledColor.size()
        );
#endif

        // Blit pixels to device
        SetDIBitsToDevice(

            m_hdc,
            0, 0,
            width, height,
            0, 0,
            0, height,
            reinterpret_cast<void *>(dst),
            &m_bmi,
            DIB_RGB_COLORS
        );
    }
}
