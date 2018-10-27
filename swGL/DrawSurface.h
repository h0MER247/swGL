#pragma once

#include <Windows.h>
#include <array>
#include "DrawBuffer.h"
#if !SWGL_USE_HARDWARE_GAMMA
#include "GammaRamp.h"
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
        int getWidth() const { return m_width; }
        int getHeight() const { return m_height; }

    public:
        DrawBufferPtr getBuffer(int threadIdx) { return m_buffer[threadIdx]; }
        int getBufferWidth() { return m_bufferWidth; }
        int getBufferHeight() { return m_bufferHeight; }
        int getNumBuffersInX() { return m_numBuffersInX; }
        int getNumBuffersInY() { return m_numBuffersInY; }

    public:
        void swap();

	private:
		void updateDimensions();

    #if !SWGL_USE_HARDWARE_GAMMA
    public:
        static GammaRamp &getGammaRamp() {
        
            return m_gammaRamp;
        }

    private:
        static GammaRamp m_gammaRamp;
    #endif

    private:
		HWND m_hWnd;
        HDC m_hdc;
        BITMAPINFO m_bmi;
        int m_width;
        int m_height;

    private:
        ColorBuffer m_unswizzledColor;

    private:
        int m_numBuffersInX;
        int m_numBuffersInY;
        int m_bufferWidth;
        int m_bufferHeight;
        std::array<DrawBufferPtr, SWGL_NUM_DRAW_THREADS> m_buffer;
    };
}
