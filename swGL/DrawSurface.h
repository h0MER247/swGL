#pragma once

#include <Windows.h>
#include <array>
#include "DrawBuffer.h"
#if !SWGL_USE_HARDWARE_GAMMA
#include "SWGammaRamp.h"
#endif

namespace SWGL {

    //
    // A drawing surface, composed of several DrawBuffer's
    //
    class DrawSurface {

    public:
        DrawSurface();
        ~DrawSurface() = default;

    public:
        void setHDC(HDC hdc);
        HDC getHDC() { return m_hdc; }
        
    public:
        int getClientWidth() { return m_clientWidth; }
        int getClientHeight() { return m_clientHeight; }

    public:
        DrawBufferPtr getBuffer(int threadIdx) { return m_buffer[threadIdx]; }
        int getBufferWidth() { return m_bufferWidth; }
        int getBufferHeight() { return m_bufferHeight; }
        int getNumBuffersInX() { return m_numBuffersInX; }
        int getNumBuffersInY() { return m_numBuffersInY; }

    public:
        void swap();

    #if !SWGL_USE_HARDWARE_GAMMA
    public:
        static SWGammaRamp &getGammaRamp() {
        
            return m_gammaRamp;
        }

    private:
        static SWGammaRamp m_gammaRamp;
    #endif

    private:
        HDC m_hdc;
        int m_clientWidth;
        int m_clientHeight;
        BITMAPINFO m_bmi;
        
    private:
        void unswizzleColorBuffer();
        ColorBuffer m_unswizzledColor;

    private:
        int m_numBuffersInX;
        int m_numBuffersInY;
        int m_bufferWidth;
        int m_bufferHeight;
        std::array<DrawBufferPtr, SWGL_NUM_DRAW_THREADS> m_buffer;
    };
}
