#pragma once

#include <Windows.h>
#include "OpenGL.h"

// Wiggle API
// -------------------------------------------------------
SWGLAPI int STDCALL glDrv_wglChoosePixelFormat(HDC hdc, CONST PIXELFORMATDESCRIPTOR *ppfd);
SWGLAPI BOOL STDCALL glDrv_wglCopyContext(HGLRC hglrc, HGLRC hglrc2, UINT i);
SWGLAPI HGLRC STDCALL glDrv_wglCreateContext(HDC hdc);
SWGLAPI HGLRC STDCALL glDrv_wglCreateLayerContext(HDC hdc, int iLayerPlane);
SWGLAPI BOOL STDCALL glDrv_wglDeleteContext(HGLRC hglrc);
SWGLAPI BOOL STDCALL glDrv_wglDescribeLayerPlane(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nBytes, LPLAYERPLANEDESCRIPTOR plpd);
SWGLAPI int STDCALL glDrv_wglDescribePixelFormat(HDC hdc, int iPixelFormat, UINT nBytes, LPPIXELFORMATDESCRIPTOR ppfd);
SWGLAPI HGLRC STDCALL glDrv_wglGetCurrentContext();
SWGLAPI HDC STDCALL glDrv_wglGetCurrentDC();
SWGLAPI int STDCALL glDrv_wglGetLayerPaletteEntries(HDC hdc, int iLayerPlane, int iStart, int cEntries, COLORREF *pcr);
SWGLAPI int STDCALL glDrv_wglGetPixelFormat(HDC hdc);
SWGLAPI PROC STDCALL glDrv_wglGetProcAddress(LPCSTR s);
SWGLAPI BOOL STDCALL glDrv_wglMakeCurrent(HDC hdc, HGLRC hglrc);
SWGLAPI BOOL STDCALL glDrv_wglRealizeLayerPalette(HDC hdc, int iLayerPlane, BOOL b);
SWGLAPI int STDCALL glDrv_wglSetLayerPaletteEntries(HDC hdc, int iLayerPlane, int iStart, int cEntries, CONST COLORREF *pcr);
SWGLAPI BOOL STDCALL glDrv_wglSetPixelFormat(HDC hdc, int format, CONST PIXELFORMATDESCRIPTOR *ppfd);
SWGLAPI BOOL STDCALL glDrv_wglShareLists(HGLRC hglrc, HGLRC hglrc2);
SWGLAPI BOOL STDCALL glDrv_wglSwapBuffers(HDC hdc);
SWGLAPI BOOL STDCALL glDrv_wglSwapLayerBuffers(HDC hdc, UINT planes);
SWGLAPI BOOL STDCALL glDrv_wglUseFontBitmapsA(HDC hdc, DWORD dw1, DWORD dw2, DWORD dw3);
SWGLAPI BOOL STDCALL glDrv_wglUseFontBitmapsW(HDC hdc, DWORD dw1, DWORD dw2, DWORD dw3);
SWGLAPI BOOL STDCALL glDrv_wglUseFontOutlinesA(HDC hdc, DWORD dw1, DWORD dw2, DWORD dw3, FLOAT f1, FLOAT f2, int i, LPGLYPHMETRICSFLOAT pgmf);
SWGLAPI BOOL STDCALL glDrv_wglUseFontOutlinesW(HDC hdc, DWORD dw1, DWORD dw2, DWORD dw3, FLOAT f1, FLOAT f2, int i, LPGLYPHMETRICSFLOAT pgmf);
// -------------------------------------------------------

// Some extensions
// -------------------------------------------------------
SWGLAPI BOOL STDCALL glDrv_wglSetDeviceGammaRamp(HDC hdc, LPVOID lpRamp);
SWGLAPI BOOL STDCALL glDrv_wglGetDeviceGammaRamp(HDC hdc, LPVOID lpRamp);
SWGLAPI const char * STDCALL glDrv_wglGetExtensionsString(HDC hdc);
// -------------------------------------------------------
