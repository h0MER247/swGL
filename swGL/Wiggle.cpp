#include <Windows.h>
#include "OpenGL.h"
#include "Defines.h"
#include "Log.h"
#include "Context.h"
#include "DrawSurface.h"

SWGLAPI int STDCALL glDrv_wglChoosePixelFormat(HDC hdc, CONST PIXELFORMATDESCRIPTOR *ppfd) {

    LOG("HDC: %p, Flags: %08x, Stencil Bits: %d, Accum Bits: %d, Color Bits: %d, Alpha Bits: %d, Depth Bits: %d", hdc, ppfd->dwFlags, ppfd->cStencilBits, ppfd->cAccumBits, ppfd->cColorBits, ppfd->cAlphaBits, ppfd->cDepthBits);

    return 1;
}

SWGLAPI BOOL STDCALL glDrv_wglCopyContext(HGLRC hglrc, HGLRC hglrc2, UINT i) {

    LOG("Unimplemented");

    return FALSE;
}

SWGLAPI HGLRC STDCALL glDrv_wglCreateContext(HDC hdc) {

    LOG("HDC: %p", hdc);

    auto contextID = SWGL::Context::createContext(hdc);

    LOG("New context has id %d", contextID);

    return reinterpret_cast<HGLRC>(contextID);
}

SWGLAPI HGLRC STDCALL glDrv_wglCreateLayerContext(HDC hdc, int iLayerPlane) {

    LOG("Unimplemented");

    return reinterpret_cast<HGLRC>(0);
}

SWGLAPI BOOL STDCALL glDrv_wglDeleteContext(HGLRC hglrc) {

    LOG("Deleting context with id %p", hglrc);

    auto contextID = reinterpret_cast<SWGL::ContextID>(hglrc);

    if (SWGL::Context::deleteContext(contextID))
        return TRUE;

    return FALSE;
}

SWGLAPI BOOL STDCALL glDrv_wglDescribeLayerPlane(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nBytes, LPLAYERPLANEDESCRIPTOR plpd) {

    LOG("Unimplemented");
    return FALSE;
}

SWGLAPI int STDCALL glDrv_wglDescribePixelFormat(HDC hdc, int iPixelFormat, UINT nBytes, LPPIXELFORMATDESCRIPTOR ppfd) {

    LOG("HDC: %p, Pixelformat: %d, Bytes: %d, Descriptor: %p", hdc, iPixelFormat, nBytes, ppfd);

    if (ppfd != nullptr) {

        memset(ppfd, 0, sizeof(PIXELFORMATDESCRIPTOR));

        ppfd->nSize = sizeof(PIXELFORMATDESCRIPTOR);
        ppfd->nVersion = 1;
        ppfd->dwFlags = PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL;
        ppfd->iPixelType = PFD_TYPE_RGBA;
        ppfd->cColorBits = 32;
        ppfd->cAlphaBits = 8; ppfd->cAlphaShift = 24;
        ppfd->cRedBits = 8; ppfd->cRedShift = 16;
        ppfd->cGreenBits = 8; ppfd->cGreenShift = 8;
        ppfd->cBlueBits = 8; ppfd->cBlueShift = 0;
        ppfd->cDepthBits = 24;
    }

    return 1;
}

SWGLAPI HGLRC STDCALL glDrv_wglGetCurrentContext() {

    LOG("");

    auto &context = SWGL::Context::getCurrentContext();
    if (context != nullptr) {

        return reinterpret_cast<HGLRC>(context->getContextID());
    }
    else {

        return reinterpret_cast<HGLRC>(0);
    }
}

SWGLAPI HDC STDCALL glDrv_wglGetCurrentDC() {

    LOG("");

    auto &context = SWGL::Context::getCurrentContext();
    if (context != nullptr) {

        return context->getRenderer().getDrawSurface().getHDC();
    }
    else {

        return reinterpret_cast<HDC>(0);
    }
}

SWGLAPI int STDCALL glDrv_wglGetLayerPaletteEntries(HDC hdc, int iLayerPlane, int iStart, int cEntries, COLORREF *pcr) {

    LOG("Unimplemented");
    return 0;
}

SWGLAPI int STDCALL glDrv_wglGetPixelFormat(HDC hdc) {

    LOG("HDC: %p", hdc);

    return 1;
}

SWGLAPI PROC STDCALL glDrv_wglGetProcAddress(LPCSTR s) {

    LOG("Proc: %s", s);

    auto &context = SWGL::Context::getCurrentContext();
    if (context != nullptr) {

        return reinterpret_cast<PROC>(SWGL::Context::getCurrentContext()->getLibProcedure(s));
    }
    else {

        return reinterpret_cast<PROC>(0);
    }
}

SWGLAPI BOOL STDCALL glDrv_wglMakeCurrent(HDC hdc, HGLRC hglrc) {

    LOG("Make context %p current, hdc = %p", hglrc, hdc);

    if (hglrc != nullptr) {

        auto context = SWGL::Context::getContext(
            reinterpret_cast<SWGL::ContextID>(hglrc)
        );

        if (context != nullptr) {

            SWGL::Context::setCurrentContext(context, hdc);
            return TRUE;
        }
    }
    else {

        SWGL::Context::setCurrentContext(nullptr, nullptr);
        return TRUE;
    }

    return FALSE;
}

SWGLAPI BOOL STDCALL glDrv_wglRealizeLayerPalette(HDC hdc, int iLayerPlane, BOOL b) {

    LOG("Unimplemented");
    return FALSE;
}

SWGLAPI int STDCALL glDrv_wglSetLayerPaletteEntries(HDC hdc, int iLayerPlane, int iStart, int cEntries, CONST COLORREF *pcr) {

    LOG("Unimplemented");
    return 0;
}

SWGLAPI BOOL STDCALL glDrv_wglSetPixelFormat(HDC hdc, int format, CONST PIXELFORMATDESCRIPTOR *ppfd) {

    LOG("HDC: %p, Format: %d, Descriptor: %p", hdc, format, ppfd);

    return format == 1;
}

SWGLAPI BOOL STDCALL glDrv_wglShareLists(HGLRC hglrc, HGLRC hglrc2) {

    LOG("Unimplemented");
    return FALSE;
}

SWGLAPI BOOL STDCALL glDrv_wglSwapBuffers(HDC hdc) {

    LOG("HDC: %p", hdc);

    auto &context = SWGL::Context::getCurrentContext();
    if (context != nullptr) {

        context->getRenderer().swapBuffers();
        return TRUE;
    }

    return FALSE;
}

SWGLAPI BOOL STDCALL glDrv_wglSwapLayerBuffers(HDC hdc, UINT planes) {

    LOG("HDC: %p, Planes: %08x", hdc, planes);

    auto &context = SWGL::Context::getCurrentContext();
    if (context != nullptr) {

        if ((planes & WGL_SWAP_MAIN_PLANE) != 0U) {

            context->getRenderer().swapBuffers();
        }

        return TRUE;
    }

    return FALSE;
}

SWGLAPI BOOL STDCALL glDrv_wglUseFontBitmapsA(HDC hdc, DWORD dw1, DWORD dw2, DWORD dw3) {

    LOG("Unimplemented");
    return FALSE;
}

SWGLAPI BOOL STDCALL glDrv_wglUseFontBitmapsW(HDC hdc, DWORD dw1, DWORD dw2, DWORD dw3) {

    LOG("Unimplemented");
    return FALSE;
}

SWGLAPI BOOL STDCALL glDrv_wglUseFontOutlinesA(HDC hdc, DWORD dw1, DWORD dw2, DWORD dw3, FLOAT f1, FLOAT f2, int i, LPGLYPHMETRICSFLOAT pgmf) {

    LOG("Unimplemented");
    return FALSE;
}

SWGLAPI BOOL STDCALL glDrv_wglUseFontOutlinesW(HDC hdc, DWORD dw1, DWORD dw2, DWORD dw3, FLOAT f1, FLOAT f2, int i, LPGLYPHMETRICSFLOAT pgmf) {

    LOG("Unimplemented");
    return FALSE;
}

SWGLAPI BOOL STDCALL glDrv_wglSetDeviceGammaRamp(HDC hdc, LPVOID lpRamp) {

    LOG("HDC: %p, Ramp Address: %p", hdc, lpRamp);

#if SWGL_USE_HARDWARE_GAMMA
    return ::SetDeviceGammaRamp(hdc, lpRamp);
#else
    SWGL::DrawSurface::getGammaRamp().set(reinterpret_cast<unsigned short *>(lpRamp));
    return true;
#endif
}

SWGLAPI BOOL STDCALL glDrv_wglGetDeviceGammaRamp(HDC hdc, LPVOID lpRamp) {

    LOG("HDC: %p, Ramp Address: %p", hdc, lpRamp);

#if SWGL_USE_HARDWARE_GAMMA
    return ::GetDeviceGammaRamp(hdc, lpRamp);
#else
    SWGL::DrawSurface::getGammaRamp().get(reinterpret_cast<unsigned short *>(lpRamp));
    return true;
#endif
}

SWGLAPI const char * STDCALL glDrv_wglGetExtensionsString(HDC hdc) {

    LOG("HDC: %p", hdc);

    auto &context = SWGL::Context::getCurrentContext();
    if (context != nullptr) {

        return context->getLibExtensions();
    }

    return nullptr;
}
