#include <algorithm>
#include <cmath>
#include "DrawThread.h"
#include "ContextTypes.h"
#include "SIMD.h"
#include "OpenGL.h"
#include "Triangle.h"
#include "TextureManager.h"
#include "CommandDrawTriangle.h"

#define GRADIENT_VALUE(NAME) \
    qV ## NAME

#define GRADIENT_DX(NAME) \
    qDX ## NAME

#define GRADIENT_DY(NAME) \
    qDY ## NAME

#define DEFINE_GRADIENT(NAME) \
    QFloat GRADIENT_VALUE(NAME), GRADIENT_DX(NAME), GRADIENT_DY(NAME)

#define SETUP_GRADIENT_EQ(NAME, Q1, Q2, Q3) \
    setupGradientEquation(GRADIENT_VALUE(NAME), GRADIENT_DX(NAME), GRADIENT_DY(NAME), Q1, Q2, Q3, v1.posObj.x(), v1.posObj.y(), fdx21, fdy21, fdx31, fdy31, rcpArea)

#define GET_GRADIENT_VALUE_AFFINE(NAME) \
    _mm_add_ps(qV ## NAME, _mm_add_ps(_mm_mul_ps(xxxx, GRADIENT_DX(NAME)), _mm_mul_ps(yyyy, GRADIENT_DY(NAME))))

#define GET_GRADIENT_VALUE_PERSP(NAME) \
    _mm_mul_ps(w, GET_GRADIENT_VALUE_AFFINE(NAME))



//
// TODO: Find the cause of dropped fragments as there are cracks between adjacent triangles.
//       It is very obvious in Unreal Tournament's map CTF-LavaGiant.
//
namespace SWGL {

    static INLINED QInt getIntegerRGBA(ARGBColor &color) {

        const QFloat cMin = _mm_setzero_ps();
        const QFloat cMax = _mm_set1_ps(255.0f);

        // Scale floating point color values
        QFloat a = _mm_mul_ps(color.a, cMax);
        QFloat r = _mm_mul_ps(color.r, cMax);
        QFloat g = _mm_mul_ps(color.g, cMax);
        QFloat b = _mm_mul_ps(color.b, cMax);

        // Clamp the values between [0,255]
        a = SIMD::clamp(a, cMin, cMax);
        r = SIMD::clamp(r, cMin, cMax);
        g = SIMD::clamp(g, cMin, cMax);
        b = SIMD::clamp(b, cMin, cMax);

        // Build result
        QInt resA = _mm_slli_epi32(_mm_cvtps_epi32(a), 24);
        QInt resR = _mm_slli_epi32(_mm_cvtps_epi32(r), 16);
        QInt resG = _mm_slli_epi32(_mm_cvtps_epi32(g), 8);
        QInt resB = _mm_cvtps_epi32(b);

        return _mm_or_si128(

            _mm_or_si128(resA, resR),
            _mm_or_si128(resG, resB)
        );
    }

    static void setupGradientEquation(QFloat &qVAL, QFloat &qDQDX, QFloat &qDQDY, float q1, float q2, float q3, float x1, float y1, float dx21, float dy21, float dx31, float dy31, float rcpArea) {

        float dq21 = q2 - q1;
        float dq31 = q3 - q1;
        float dqdx = rcpArea * (dq21 * dy31 - dq31 * dy21);
        float dqdy = rcpArea * (dq31 * dx21 - dq21 * dx31);

        // Calculate the interpolant value at the origin point
        float value = q1 - (x1 * dqdx) - (y1 * dqdy);

        qDQDX = _mm_set1_ps(dqdx);
        qDQDY = _mm_set1_ps(dqdy);
        qVAL = _mm_set_ps(

            value + dqdy + dqdx,
            value + dqdy,
            value + dqdx,
            value
        );
    }

    static void setupEdgeEquation(QInt &eVAL, QInt &eDEDX, QInt &eDEDY, int x, int y, int dx, int dy, int minX, int minY, int width) {

        int dedx = -dy << 4;
        int dedy = dx << 4;
        int value = (dy * x) - (dx * y) + (dedx * minX) + (dedy * minY);

        if (dy < 0 || (dy == 0 && dx > 0)) {

            value++;
        }

        eDEDX = _mm_set1_epi32(dedx << 1);
        eDEDY = _mm_set1_epi32((dedy << 1) - (dedx * width));
        eVAL = _mm_set_epi32(

            value + dedy + dedx,
            value + dedy,
            value + dedx,
            value
        );
    }



    //
    // Draw triangle command
    //
    bool CommandDrawTriangle::execute(DrawThread *thread) {

        auto &drawBuffer = thread->getDrawBuffer();

        auto &scissor = m_state->scissor;
        auto &polygonOffset = m_state->polygonOffset;
        auto &depthTesting = m_state->depthTesting;
        auto &alphaTesting = m_state->alphaTesting;
        auto &blending = m_state->blending;
        auto &colorMask = m_state->colorMask;
        auto &writeDepthAfterAlphaTest = m_state->deferedDepthWrite;
        auto writeDepthAfterDepthTest = depthTesting.isWriteEnabled() && !writeDepthAfterAlphaTest;
        auto &textureState = m_state->textures;

        for (auto triangleIdx : m_indices) {

            auto &t = m_state->triangles[triangleIdx];
            auto &v1 = t.v[0];
            auto &v2 = t.v[1];
            auto &v3 = t.v[2];

            //
            // Calculate the triangles reciprocal area
            //
            float rcpArea = 1.0f / ((v2.posObj.x() - v1.posObj.x()) * (v3.posObj.y() - v1.posObj.y()) -
                                    (v2.posObj.y() - v1.posObj.y()) * (v3.posObj.x() - v1.posObj.x()));

            //
            // Calculate fixed point coordinates
            //
            int x1, y1, x2, y2, x3, y3;

            x1 = static_cast<int>(v1.posObj.x() * 16.0f);
            y1 = static_cast<int>(v1.posObj.y() * 16.0f);
            if (rcpArea < 0.0f) {

                x2 = static_cast<int>(v2.posObj.x() * 16.0f);
                y2 = static_cast<int>(v2.posObj.y() * 16.0f);
                x3 = static_cast<int>(v3.posObj.x() * 16.0f);
                y3 = static_cast<int>(v3.posObj.y() * 16.0f);
            }
            else {

                x2 = static_cast<int>(v3.posObj.x() * 16.0f);
                y2 = static_cast<int>(v3.posObj.y() * 16.0f);
                x3 = static_cast<int>(v2.posObj.x() * 16.0f);
                y3 = static_cast<int>(v2.posObj.y() * 16.0f);
            }

            //
            // Determine triangle bounding box with respect to our rendertarget
            //
            int minY = std::max((std::min({ y1, y2, y3 }) + 0x0f) >> 4, drawBuffer.getMinY());
            int maxY = std::min((std::max({ y1, y2, y3 }) + 0x0f) >> 4, drawBuffer.getMaxY());
            int minX = std::max((std::min({ x1, x2, x3 }) + 0x0f) >> 4, drawBuffer.getMinX());
            int maxX = std::min((std::max({ x1, x2, x3 }) + 0x0f) >> 4, drawBuffer.getMaxX());

            if (scissor.isEnabled()) {

                // TODO: I don't think that scissoring works correctly if the coordinates
                //       are uneven. An example would be minXY=(1, 1) and maxXY=(7, 7)
                scissor.cut(minX, minY, maxX, maxY);
            }

            // Make sure that we rasterize at the beginning of a quad (which is 2x2 pixel).
            // Then determine the width of the bounding box in full quads.
            minX &= ~1;
            minY &= ~1;

            int width = (1 + (maxX - minX)) & ~1;

            //
            // Determine the write position into the color and depth buffer
            //
            int startX = minX - drawBuffer.getMinX();
            int startY = minY - drawBuffer.getMinY();

            ptrdiff_t bufferOffset = (startX << 1) + (startY * drawBuffer.getWidth());
            ptrdiff_t bufferStride = (drawBuffer.getWidth() - width) << 1;

            auto colorBuffer = drawBuffer.getColor() + bufferOffset;
            auto depthBuffer = drawBuffer.getDepth() + bufferOffset;

            //
            // Determine the triangle edge equations
            //
            int dx12 = x1 - x2, dx23 = x2 - x3, dx31 = x3 - x1;
            int dy12 = y1 - y2, dy23 = y2 - y3, dy31 = y3 - y1;

            QInt edgeValue[3], edgeDX[3], edgeDY[3];
            setupEdgeEquation(edgeValue[0], edgeDX[0], edgeDY[0], x1, y1, dx12, dy12, minX, minY, width);
            setupEdgeEquation(edgeValue[1], edgeDX[1], edgeDY[1], x2, y2, dx23, dy23, minX, minY, width);
            setupEdgeEquation(edgeValue[2], edgeDX[2], edgeDY[2], x3, y3, dx31, dy31, minX, minY, width);

            //
            // Determine the gradient equations
            //
            float fdx21 = v2.posObj.x() - v1.posObj.x(), fdy21 = v2.posObj.y() - v1.posObj.y();
            float fdx31 = v3.posObj.x() - v1.posObj.x(), fdy31 = v3.posObj.y() - v1.posObj.y();

            DEFINE_GRADIENT(z);
            SETUP_GRADIENT_EQ(z, v1.posObj.z(), v2.posObj.z(), v3.posObj.z());

            DEFINE_GRADIENT(rcpW);
            SETUP_GRADIENT_EQ(rcpW, v1.posObj.w(), v2.posObj.w(), v3.posObj.w());

            DEFINE_GRADIENT(primaryA);
            SETUP_GRADIENT_EQ(primaryA, v1.colorPrimary.a(), v2.colorPrimary.a(), v3.colorPrimary.a());
            DEFINE_GRADIENT(primaryR);
            SETUP_GRADIENT_EQ(primaryR, v1.colorPrimary.r(), v2.colorPrimary.r(), v3.colorPrimary.r());
            DEFINE_GRADIENT(primaryG);
            SETUP_GRADIENT_EQ(primaryG, v1.colorPrimary.g(), v2.colorPrimary.g(), v3.colorPrimary.g());
            DEFINE_GRADIENT(primaryB);
            SETUP_GRADIENT_EQ(primaryB, v1.colorPrimary.b(), v2.colorPrimary.b(), v3.colorPrimary.b());

            DEFINE_GRADIENT(texS[SWGL_MAX_TEXTURE_UNITS]);
            DEFINE_GRADIENT(texT[SWGL_MAX_TEXTURE_UNITS]);
            DEFINE_GRADIENT(texR[SWGL_MAX_TEXTURE_UNITS]);
            DEFINE_GRADIENT(texQ[SWGL_MAX_TEXTURE_UNITS]);
            for (auto i = 0U; i < SWGL_MAX_TEXTURE_UNITS; i++) {

                SETUP_GRADIENT_EQ(texS[i], v1.texCoord[i].x(), v2.texCoord[i].x(), v3.texCoord[i].x());
                SETUP_GRADIENT_EQ(texT[i], v1.texCoord[i].y(), v2.texCoord[i].y(), v3.texCoord[i].y());
                SETUP_GRADIENT_EQ(texR[i], v1.texCoord[i].z(), v2.texCoord[i].z(), v3.texCoord[i].z());
                SETUP_GRADIENT_EQ(texQ[i], v1.texCoord[i].w(), v2.texCoord[i].w(), v3.texCoord[i].w());
            }

            //
            // Calculate polygon offset (see page 77, glspec13.pdf)
            //
            if (polygonOffset.isFillEnabled()) {

                QFloat m = _mm_max_ps(

                    SIMD::absolute(GRADIENT_DX(z)),
                    SIMD::absolute(GRADIENT_DY(z))
                );

                QFloat zOffset = SIMD::multiplyAdd(

                    m,
                    _mm_set1_ps(polygonOffset.getFactor()),
                    _mm_set1_ps(polygonOffset.getRTimesUnits())
                );

                GRADIENT_VALUE(z) = _mm_add_ps(GRADIENT_VALUE(z), zOffset);
            }


            //
            // Rasterize and shade the triangle
            //
            ARGBColor srcColor;
            ARGBColor texColor;
            ARGBColor primaryColor;
            TextureCoordinates texCoords;

            for (int y = minY; y < maxY; y += 2) {

                QFloat yyyy = _mm_set1_ps(static_cast<float>(y));

                for (int x = minX; x < maxX; x += 2) {

                    //
                    // Coverage test for a 2x2 pixel quad
                    //
                    QInt e0 = _mm_cmpgt_epi32(edgeValue[0], _mm_setzero_si128());
                    QInt e1 = _mm_cmpgt_epi32(edgeValue[1], _mm_setzero_si128());
                    QInt e2 = _mm_cmpgt_epi32(edgeValue[2], _mm_setzero_si128());
                    QInt fragmentMask = _mm_and_si128(_mm_and_si128(e0, e1), e2);

                    if (_mm_testz_si128(fragmentMask, fragmentMask) == 0) {

                        QFloat xxxx = _mm_set1_ps(static_cast<float>(x));


                        //
                        // (Early) Depth test
                        //
                        QInt depthBufferZ, currentZ;

                        if (depthTesting.isTestEnabled()) {

                            depthBufferZ = _mm_load_si128(reinterpret_cast<QInt *>(depthBuffer));
                            currentZ = _mm_cvtps_epi32(
                                _mm_mul_ps(
                                    _mm_set1_ps(16777215.0f),
                                    SIMD::clamp01(GET_GRADIENT_VALUE_AFFINE(z))
                                )
                            );

                            switch (depthTesting.getTestFunction()) {

                            case GL_NEVER: goto nextQuad;
                            case GL_LESS: fragmentMask = _mm_and_si128(_mm_cmplt_epi32(currentZ, depthBufferZ), fragmentMask); break;
                            case GL_EQUAL: fragmentMask = _mm_and_si128(_mm_cmpeq_epi32(currentZ, depthBufferZ), fragmentMask); break;
                            case GL_LEQUAL: fragmentMask = _mm_andnot_si128(_mm_cmpgt_epi32(currentZ, depthBufferZ), fragmentMask); break;
                            case GL_GREATER: fragmentMask = _mm_and_si128(_mm_cmpgt_epi32(currentZ, depthBufferZ), fragmentMask); break;
                            case GL_NOTEQUAL: fragmentMask = _mm_andnot_si128(_mm_cmpeq_epi32(currentZ, depthBufferZ), fragmentMask); break;
                            case GL_GEQUAL: fragmentMask = _mm_andnot_si128(_mm_cmplt_epi32(currentZ, depthBufferZ), fragmentMask); break;
                            }

                            // Check if any fragment survived the depth test
                            if (_mm_testz_si128(fragmentMask, fragmentMask) != 0) {

                                goto nextQuad;
                            }

                            // Write the new depth values to the depth buffer
                            if (writeDepthAfterDepthTest) {

                                _mm_store_si128(

                                    reinterpret_cast<QInt *>(depthBuffer),
                                    SIMD::blend(depthBufferZ, currentZ, fragmentMask)
                                );
                            }
                        }


                        //
                        // Calculate perspective w
                        //
                        QFloat w = _mm_div_ps(_mm_set1_ps(1.0f), GET_GRADIENT_VALUE_AFFINE(rcpW));


                        //
                        // Set the fragments initial color
                        //
                        primaryColor.a = GET_GRADIENT_VALUE_PERSP(primaryA);
                        primaryColor.r = GET_GRADIENT_VALUE_PERSP(primaryR);
                        primaryColor.g = GET_GRADIENT_VALUE_PERSP(primaryG);
                        primaryColor.b = GET_GRADIENT_VALUE_PERSP(primaryB);


                        //
                        // Texture sampling and blending for each active texture unit
                        //
                        srcColor = primaryColor;

                        for (auto texUnit = 0U; texUnit < SWGL_MAX_TEXTURE_UNITS; texUnit++) {

                            auto &texState = textureState[texUnit];
                            if (texState.texData == nullptr) {

                                continue;
                            }

                            // Get texture sample
                            QFloat rcpQ = _mm_div_ps(_mm_set1_ps(1.0f), GET_GRADIENT_VALUE_AFFINE(texQ[texUnit]));
                            texCoords.s = _mm_mul_ps(rcpQ, GET_GRADIENT_VALUE_AFFINE(texS[texUnit]));
                            texCoords.t = _mm_mul_ps(rcpQ, GET_GRADIENT_VALUE_AFFINE(texT[texUnit]));
                            texCoords.r = _mm_mul_ps(rcpQ, GET_GRADIENT_VALUE_AFFINE(texR[texUnit]));
                            texState.texData->sampleTexels(texState.texParams, texCoords, texColor);

                            // Execute the texturing function
                            if (texState.texEnv.mode != GL_COMBINE) {

                                switch (texState.texEnv.mode) {

                                case GL_REPLACE:
                                    switch (texState.texData->format) {

                                    case TextureBaseFormat::Alpha:
                                        srcColor.a = texColor.a;
                                        break;

                                    case TextureBaseFormat::RGB:
                                    case TextureBaseFormat::Luminance:
                                        srcColor.r = texColor.r;
                                        srcColor.g = texColor.g;
                                        srcColor.b = texColor.b;
                                        break;

                                    case TextureBaseFormat::LuminanceAlpha:
                                    case TextureBaseFormat::Intensity:
                                    case TextureBaseFormat::RGBA:
                                        srcColor.a = texColor.a;
                                        srcColor.r = texColor.r;
                                        srcColor.g = texColor.g;
                                        srcColor.b = texColor.b;
                                        break;
                                    }
                                    break;

                                case GL_MODULATE:
                                    switch (texState.texData->format) {

                                    case TextureBaseFormat::Alpha:
                                        srcColor.a = _mm_mul_ps(srcColor.a, texColor.a);
                                        break;

                                    case TextureBaseFormat::LuminanceAlpha:
                                    case TextureBaseFormat::Intensity:
                                    case TextureBaseFormat::RGBA:
                                        srcColor.a = _mm_mul_ps(srcColor.a, texColor.a);
                                    case TextureBaseFormat::Luminance:
                                    case TextureBaseFormat::RGB:
                                        srcColor.r = _mm_mul_ps(srcColor.r, texColor.r);
                                        srcColor.g = _mm_mul_ps(srcColor.g, texColor.g);
                                        srcColor.b = _mm_mul_ps(srcColor.b, texColor.b);
                                        break;
                                    }
                                    break;

                                case GL_DECAL:
                                    switch (texState.texData->format) {

                                    case TextureBaseFormat::Alpha:
                                    case TextureBaseFormat::Intensity:
                                    case TextureBaseFormat::Luminance:
                                    case TextureBaseFormat::LuminanceAlpha:
                                        // Undefined
                                        break;

                                    case TextureBaseFormat::RGB:
                                        srcColor.r = texColor.r;
                                        srcColor.g = texColor.g;
                                        srcColor.b = texColor.b;
                                        break;

                                    case TextureBaseFormat::RGBA:
                                        srcColor.r = SIMD::lerp(texColor.a, srcColor.r, texColor.r);
                                        srcColor.g = SIMD::lerp(texColor.a, srcColor.g, texColor.g);
                                        srcColor.b = SIMD::lerp(texColor.a, srcColor.b, texColor.b);
                                        break;
                                    }
                                    break;

                                case GL_ADD:
                                    switch (texState.texData->format) {

                                    case TextureBaseFormat::Alpha:
                                        srcColor.a = _mm_mul_ps(srcColor.a, texColor.a);
                                        break;

                                    case TextureBaseFormat::LuminanceAlpha:
                                    case TextureBaseFormat::RGBA:
                                        srcColor.a = _mm_mul_ps(srcColor.a, texColor.a);
                                    case TextureBaseFormat::Luminance:
                                    case TextureBaseFormat::RGB:
                                        srcColor.r = _mm_min_ps(_mm_set1_ps(1.0f), _mm_add_ps(srcColor.r, texColor.r));
                                        srcColor.g = _mm_min_ps(_mm_set1_ps(1.0f), _mm_add_ps(srcColor.g, texColor.g));
                                        srcColor.b = _mm_min_ps(_mm_set1_ps(1.0f), _mm_add_ps(srcColor.b, texColor.b));
                                        break;

                                    case TextureBaseFormat::Intensity:
                                        srcColor.a = _mm_min_ps(_mm_set1_ps(1.0f), _mm_add_ps(srcColor.a, texColor.a));
                                        srcColor.r = _mm_min_ps(_mm_set1_ps(1.0f), _mm_add_ps(srcColor.r, texColor.r));
                                        srcColor.g = _mm_min_ps(_mm_set1_ps(1.0f), _mm_add_ps(srcColor.g, texColor.g));
                                        srcColor.b = _mm_min_ps(_mm_set1_ps(1.0f), _mm_add_ps(srcColor.b, texColor.b));
                                        break;
                                    }
                                    break;

                                case GL_BLEND:
                                    switch (texState.texData->format) {

                                    case TextureBaseFormat::Alpha:
                                        srcColor.a = _mm_mul_ps(srcColor.a, texColor.a);
                                        break;

                                    case TextureBaseFormat::LuminanceAlpha:
                                    case TextureBaseFormat::RGBA:
                                        srcColor.a = _mm_mul_ps(srcColor.a, texColor.a);
                                    case TextureBaseFormat::Luminance:
                                    case TextureBaseFormat::RGB:
                                        srcColor.r = SIMD::lerp(texColor.r, srcColor.r, _mm_set1_ps(texState.texEnv.colorConstR));
                                        srcColor.g = SIMD::lerp(texColor.g, srcColor.g, _mm_set1_ps(texState.texEnv.colorConstG));
                                        srcColor.b = SIMD::lerp(texColor.b, srcColor.b, _mm_set1_ps(texState.texEnv.colorConstB));
                                        break;

                                    case TextureBaseFormat::Intensity:
                                        srcColor.a = SIMD::lerp(texColor.a, srcColor.a, _mm_set1_ps(texState.texEnv.colorConstA));
                                        srcColor.r = SIMD::lerp(texColor.r, srcColor.r, _mm_set1_ps(texState.texEnv.colorConstR));
                                        srcColor.g = SIMD::lerp(texColor.g, srcColor.g, _mm_set1_ps(texState.texEnv.colorConstG));
                                        srcColor.b = SIMD::lerp(texColor.b, srcColor.b, _mm_set1_ps(texState.texEnv.colorConstB));
                                        break;
                                    }
                                    break;
                                }
                            }
                            else {

                                ARGBColor args[3], result;

                                auto &modeRGB = texState.texEnv.combineModeRGB;
                                auto &modeAlpha = texState.texEnv.combineModeAlpha;

                                //
                                // Alpha
                                //
                                if (modeRGB != GL_DOT3_RGBA) {

                                    // Read argument(s) and apply the modifiers
                                    for (int argIdx = 0, n = texState.texEnv.numArgsAlpha; argIdx < n; argIdx++) {

                                        auto &arg = args[argIdx];
                                        auto &src = texState.texEnv.sourceAlpha[argIdx];
                                        auto &mod = texState.texEnv.operandAlpha[argIdx];

                                        switch (src) {

                                        case GL_TEXTURE:
                                            arg.a = texColor.a;
                                            break;

                                        case GL_CONSTANT:
                                            arg.a = _mm_set1_ps(texState.texEnv.colorConstA);
                                            break;

                                        case GL_PRIMARY_COLOR:
                                            arg.a = primaryColor.a;
                                            break;

                                        case GL_PREVIOUS:
                                            arg.a = srcColor.a;
                                            break;
                                        }

                                        if (mod == GL_ONE_MINUS_SRC_ALPHA) {

                                            arg.a = _mm_sub_ps(_mm_set1_ps(1.0f), arg.a);
                                        }
                                    }

                                    // Combine alpha
                                    switch (modeAlpha) {

                                    case GL_REPLACE:
                                        result.a = args[0].a;
                                        break;

                                    case GL_MODULATE:
                                        result.a = _mm_mul_ps(args[0].a, args[1].a);
                                        break;

                                    case GL_ADD:
                                        result.a = _mm_add_ps(args[0].a, args[1].a);
                                        break;

                                    case GL_ADD_SIGNED:
                                        result.a = _mm_sub_ps(_mm_add_ps(args[0].a, args[1].a), _mm_set1_ps(0.5f));
                                        break;

                                    case GL_SUBTRACT:
                                        result.a = _mm_sub_ps(args[0].a, args[1].a);
                                        break;

                                    case GL_INTERPOLATE:
                                        result.a = SIMD::lerp(args[2].a, args[1].a, args[0].a);
                                        break;
                                    }
                                }

                                //
                                // RGB
                                //
                                for (int argIdx = 0, n = texState.texEnv.numArgsRGB; argIdx < n; argIdx++) {

                                    auto &src = texState.texEnv.sourceRGB[argIdx];
                                    auto &mod = texState.texEnv.operandRGB[argIdx];
                                    auto &arg = args[argIdx];

                                    // Read argument(s) and apply the modifiers
                                    switch (src) {

                                    case GL_TEXTURE:
                                        arg = texColor;
                                        break;

                                    case GL_CONSTANT:
                                        arg.a = _mm_set1_ps(texState.texEnv.colorConstA);
                                        arg.r = _mm_set1_ps(texState.texEnv.colorConstR);
                                        arg.g = _mm_set1_ps(texState.texEnv.colorConstG);
                                        arg.b = _mm_set1_ps(texState.texEnv.colorConstB);
                                        break;

                                    case GL_PRIMARY_COLOR:
                                        arg = primaryColor;
                                        break;

                                    case GL_PREVIOUS:
                                        arg = srcColor;
                                        break;
                                    }

                                    switch (mod) {

                                    case GL_SRC_COLOR:
                                        break;

                                    case GL_ONE_MINUS_SRC_COLOR:
                                        arg.r = _mm_sub_ps(_mm_set1_ps(1.0f), arg.r);
                                        arg.g = _mm_sub_ps(_mm_set1_ps(1.0f), arg.g);
                                        arg.b = _mm_sub_ps(_mm_set1_ps(1.0f), arg.b);
                                        break;

                                    case GL_SRC_ALPHA:
                                        arg.r = arg.a;
                                        arg.g = arg.a;
                                        arg.b = arg.a;
                                        break;

                                    case GL_ONE_MINUS_SRC_ALPHA:
                                        arg.r = _mm_sub_ps(_mm_set1_ps(1.0f), arg.a);
                                        arg.g = _mm_sub_ps(_mm_set1_ps(1.0f), arg.a);
                                        arg.b = _mm_sub_ps(_mm_set1_ps(1.0f), arg.a);
                                        break;
                                    }
                                }

                                // Combine red, green and blue
                                switch (modeRGB) {

                                case GL_REPLACE:
                                    result.r = args[0].r;
                                    result.g = args[0].g;
                                    result.b = args[0].b;
                                    break;

                                case GL_MODULATE:
                                    result.r = _mm_mul_ps(args[0].r, args[1].r);
                                    result.g = _mm_mul_ps(args[0].g, args[1].g);
                                    result.b = _mm_mul_ps(args[0].b, args[1].b);
                                    break;

                                case GL_ADD:
                                    result.r = _mm_add_ps(args[0].r, args[1].r);
                                    result.g = _mm_add_ps(args[0].g, args[1].g);
                                    result.b = _mm_add_ps(args[0].b, args[1].b);
                                    break;

                                case GL_ADD_SIGNED:
                                    result.r = _mm_sub_ps(_mm_add_ps(args[0].r, args[1].r), _mm_set1_ps(0.5f));
                                    result.g = _mm_sub_ps(_mm_add_ps(args[0].g, args[1].g), _mm_set1_ps(0.5f));
                                    result.b = _mm_sub_ps(_mm_add_ps(args[0].b, args[1].b), _mm_set1_ps(0.5f));
                                    break;

                                case GL_SUBTRACT:
                                    result.r = _mm_sub_ps(args[0].r, args[1].r);
                                    result.g = _mm_sub_ps(args[0].g, args[1].g);
                                    result.b = _mm_sub_ps(args[0].b, args[1].b);
                                    break;

                                case GL_DOT3_RGB:
                                    result.r = SIMD::dot3(args[0].r, args[1].r, args[0].g, args[1].g, args[0].b, args[1].b);
                                    result.g = result.r;
                                    result.b = result.r;
                                    break;

                                case GL_DOT3_RGBA:
                                    result.a = SIMD::dot3(args[0].r, args[1].r, args[0].g, args[1].g, args[0].b, args[1].b);
                                    result.r = result.a;
                                    result.g = result.a;
                                    result.b = result.a;
                                    break;

                                case GL_INTERPOLATE:
                                    result.r = SIMD::lerp(args[2].r, args[1].r, args[0].r);
                                    result.g = SIMD::lerp(args[2].g, args[1].g, args[0].g);
                                    result.b = SIMD::lerp(args[2].b, args[1].b, args[0].b);
                                    break;
                                }

                                srcColor.a = _mm_mul_ps(result.a, _mm_set1_ps(texState.texEnv.colorScaleA));
                                srcColor.r = _mm_mul_ps(result.r, _mm_set1_ps(texState.texEnv.colorScaleRGB));
                                srcColor.g = _mm_mul_ps(result.g, _mm_set1_ps(texState.texEnv.colorScaleRGB));
                                srcColor.b = _mm_mul_ps(result.b, _mm_set1_ps(texState.texEnv.colorScaleRGB));
                            }

                            // Not quite sure about that
                            //srcColor.a = SIMD::clamp01(srcColor.a);
                            //srcColor.r = SIMD::clamp01(srcColor.r);
                            //srcColor.g = SIMD::clamp01(srcColor.g);
                            //srcColor.b = SIMD::clamp01(srcColor.b);
                        }


                        //
                        // Alpha testing
                        //
                        if (alphaTesting.isEnabled()) {

                            QFloat refVal = _mm_set1_ps(alphaTesting.getReferenceValue());

                            switch (alphaTesting.getTestFunction()) {

                            case GL_NEVER: fragmentMask = _mm_setzero_si128(); break;
                            case GL_LESS: fragmentMask = _mm_and_si128(fragmentMask, _mm_castps_si128(_mm_cmplt_ps(srcColor.a, refVal))); break;
                            case GL_EQUAL: fragmentMask = _mm_and_si128(fragmentMask, _mm_castps_si128(_mm_cmpeq_ps(srcColor.a, refVal))); break;
                            case GL_LEQUAL: fragmentMask = _mm_and_si128(fragmentMask, _mm_castps_si128(_mm_cmple_ps(srcColor.a, refVal))); break;
                            case GL_GREATER: fragmentMask = _mm_and_si128(fragmentMask, _mm_castps_si128(_mm_cmpgt_ps(srcColor.a, refVal))); break;
                            case GL_NOTEQUAL: fragmentMask = _mm_and_si128(fragmentMask, _mm_castps_si128(_mm_cmpneq_ps(srcColor.a, refVal))); break;
                            case GL_GEQUAL: fragmentMask = _mm_and_si128(fragmentMask, _mm_castps_si128(_mm_cmpge_ps(srcColor.a, refVal))); break;
                            case GL_ALWAYS: break;
                            }

                            // Check if any fragment survived the alpha test
                            if (_mm_testz_si128(fragmentMask, fragmentMask) != 0) {

                                goto nextQuad;
                            }

                            // The write to the depthbuffer can be defered after alpha testing is done. This makes
                            // it possible to do a early depthbuffer test while maintaining the "natural" flow of
                            // data as OpenGL specifies it.
                            if (writeDepthAfterAlphaTest) {

                                _mm_store_si128(

                                    reinterpret_cast<QInt *>(depthBuffer),
                                    SIMD::blend(depthBufferZ, currentZ, fragmentMask)
                                );
                            }
                        }


                        //
                        // Blending with the color buffer
                        //
                        QInt quadBackbuffer = _mm_load_si128(reinterpret_cast<QInt *>(colorBuffer));
                        QInt quadBlendingResult;

                        if (blending.isEnabled()) {

                            // Convert the backbuffer colors back to floats
                            const QFloat normalize = _mm_set1_ps(1.0f / 255.0f);
                            const QInt mask = _mm_set1_epi32(0xff);

                            ARGBColor dstColor;
                            dstColor.a = _mm_mul_ps(normalize, _mm_cvtepi32_ps(_mm_srli_epi32(quadBackbuffer, 24)));
                            dstColor.r = _mm_mul_ps(normalize, _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(quadBackbuffer, 16), mask)));
                            dstColor.g = _mm_mul_ps(normalize, _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(quadBackbuffer, 8), mask)));
                            dstColor.b = _mm_mul_ps(normalize, _mm_cvtepi32_ps(_mm_and_si128(quadBackbuffer, mask)));

                            // Determine the source and destination blending factors
                            ARGBColor srcFactor, dstFactor;

                            switch (blending.getSourceFactor()) {

                            case GL_ZERO:
                                srcFactor.a = _mm_setzero_ps();
                                srcFactor.r = _mm_setzero_ps();
                                srcFactor.g = _mm_setzero_ps();
                                srcFactor.b = _mm_setzero_ps();
                                break;

                            case GL_ONE:
                                srcFactor.a = _mm_set1_ps(1.0f);
                                srcFactor.r = _mm_set1_ps(1.0f);
                                srcFactor.g = _mm_set1_ps(1.0f);
                                srcFactor.b = _mm_set1_ps(1.0f);
                                break;

                            case GL_DST_COLOR:
                                srcFactor = dstColor;
                                break;

                            case GL_ONE_MINUS_DST_COLOR:
                                srcFactor.a = _mm_sub_ps(_mm_set1_ps(1.0f), dstColor.a);
                                srcFactor.r = _mm_sub_ps(_mm_set1_ps(1.0f), dstColor.r);
                                srcFactor.g = _mm_sub_ps(_mm_set1_ps(1.0f), dstColor.g);
                                srcFactor.b = _mm_sub_ps(_mm_set1_ps(1.0f), dstColor.b);
                                break;

                            case GL_SRC_ALPHA:
                                srcFactor.a = srcColor.a;
                                srcFactor.r = srcColor.a;
                                srcFactor.g = srcColor.a;
                                srcFactor.b = srcColor.a;
                                break;

                            case GL_ONE_MINUS_SRC_ALPHA:
                                srcFactor.a = _mm_sub_ps(_mm_set1_ps(1.0f), srcColor.a);
                                srcFactor.r = _mm_sub_ps(_mm_set1_ps(1.0f), srcColor.a);
                                srcFactor.g = _mm_sub_ps(_mm_set1_ps(1.0f), srcColor.a);
                                srcFactor.b = _mm_sub_ps(_mm_set1_ps(1.0f), srcColor.a);
                                break;

                            case GL_DST_ALPHA:
                                srcFactor.a = dstColor.a;
                                srcFactor.r = dstColor.a;
                                srcFactor.g = dstColor.a;
                                srcFactor.b = dstColor.a;
                                break;

                            case GL_ONE_MINUS_DST_ALPHA:
                                srcFactor.a = _mm_sub_ps(_mm_set1_ps(1.0f), dstColor.a);
                                srcFactor.r = _mm_sub_ps(_mm_set1_ps(1.0f), dstColor.a);
                                srcFactor.g = _mm_sub_ps(_mm_set1_ps(1.0f), dstColor.a);
                                srcFactor.b = _mm_sub_ps(_mm_set1_ps(1.0f), dstColor.a);
                                break;

                            case GL_SRC_ALPHA_SATURATE:
                                srcFactor.a = _mm_set1_ps(1.0f);
                                srcFactor.r = _mm_min_ps(srcColor.a, _mm_sub_ps(_mm_set1_ps(1.0f), dstColor.a));
                                srcFactor.g = _mm_min_ps(srcColor.a, _mm_sub_ps(_mm_set1_ps(1.0f), dstColor.a));
                                srcFactor.b = _mm_min_ps(srcColor.a, _mm_sub_ps(_mm_set1_ps(1.0f), dstColor.a));
                                break;
                            }

                            switch (blending.getDestinationFactor()) {

                            case GL_ZERO:
                                dstFactor.a = _mm_setzero_ps();
                                dstFactor.r = _mm_setzero_ps();
                                dstFactor.g = _mm_setzero_ps();
                                dstFactor.b = _mm_setzero_ps();
                                break;

                            case GL_ONE:
                                dstFactor.a = _mm_set1_ps(1.0f);
                                dstFactor.r = _mm_set1_ps(1.0f);
                                dstFactor.g = _mm_set1_ps(1.0f);
                                dstFactor.b = _mm_set1_ps(1.0f);
                                break;

                            case GL_SRC_COLOR:
                                dstFactor = srcColor;
                                break;

                            case GL_ONE_MINUS_SRC_COLOR:
                                dstFactor.a = _mm_sub_ps(_mm_set1_ps(1.0f), srcColor.a);
                                dstFactor.r = _mm_sub_ps(_mm_set1_ps(1.0f), srcColor.r);
                                dstFactor.g = _mm_sub_ps(_mm_set1_ps(1.0f), srcColor.g);
                                dstFactor.b = _mm_sub_ps(_mm_set1_ps(1.0f), srcColor.b);
                                break;

                            case GL_SRC_ALPHA:
                                dstFactor.a = srcColor.a;
                                dstFactor.r = srcColor.a;
                                dstFactor.g = srcColor.a;
                                dstFactor.b = srcColor.a;
                                break;

                            case GL_ONE_MINUS_SRC_ALPHA:
                                dstFactor.a = _mm_sub_ps(_mm_set1_ps(1.0f), srcColor.a);
                                dstFactor.r = _mm_sub_ps(_mm_set1_ps(1.0f), srcColor.a);
                                dstFactor.g = _mm_sub_ps(_mm_set1_ps(1.0f), srcColor.a);
                                dstFactor.b = _mm_sub_ps(_mm_set1_ps(1.0f), srcColor.a);
                                break;

                            case GL_DST_ALPHA:
                                dstFactor.a = dstColor.a;
                                dstFactor.r = dstColor.a;
                                dstFactor.g = dstColor.a;
                                dstFactor.b = dstColor.a;
                                break;

                            case GL_ONE_MINUS_DST_ALPHA:
                                dstFactor.a = _mm_sub_ps(_mm_set1_ps(1.0f), dstColor.a);
                                dstFactor.r = _mm_sub_ps(_mm_set1_ps(1.0f), dstColor.a);
                                dstFactor.g = _mm_sub_ps(_mm_set1_ps(1.0f), dstColor.a);
                                dstFactor.b = _mm_sub_ps(_mm_set1_ps(1.0f), dstColor.a);
                                break;
                            }

                            // Perform the blending
                            srcColor.a = _mm_add_ps(_mm_mul_ps(srcColor.a, srcFactor.a), _mm_mul_ps(dstColor.a, dstFactor.a));
                            srcColor.r = _mm_add_ps(_mm_mul_ps(srcColor.r, srcFactor.r), _mm_mul_ps(dstColor.r, dstFactor.r));
                            srcColor.g = _mm_add_ps(_mm_mul_ps(srcColor.g, srcFactor.g), _mm_mul_ps(dstColor.g, dstFactor.g));
                            srcColor.b = _mm_add_ps(_mm_mul_ps(srcColor.b, srcFactor.b), _mm_mul_ps(dstColor.b, dstFactor.b));
                        }

                        quadBlendingResult = getIntegerRGBA(srcColor);


                        //
                        // Color masking
                        //
                        quadBlendingResult = SIMD::mask(

                            quadBlendingResult,
                            quadBackbuffer,
                            _mm_set1_epi32(colorMask.getMask())
                        );


                        //
                        // Store final color in the color buffer
                        //
                        _mm_store_si128(

                            reinterpret_cast<QInt *>(colorBuffer),
                            SIMD::blend(quadBackbuffer, quadBlendingResult, fragmentMask)
                        );
                    }

                nextQuad:

                    // Update edge equation values with respect to the change in x
                    edgeValue[0] = _mm_add_epi32(edgeValue[0], edgeDX[0]);
                    edgeValue[1] = _mm_add_epi32(edgeValue[1], edgeDX[1]);
                    edgeValue[2] = _mm_add_epi32(edgeValue[2], edgeDX[2]);

                    // Update buffer address
                    colorBuffer += 4;
                    depthBuffer += 4;
                }

                // Update edge equation values with respect to the change in y
                edgeValue[0] = _mm_add_epi32(edgeValue[0], edgeDY[0]);
                edgeValue[1] = _mm_add_epi32(edgeValue[1], edgeDY[1]);
                edgeValue[2] = _mm_add_epi32(edgeValue[2], edgeDY[2]);

                // Update buffer address
                colorBuffer += bufferStride;
                depthBuffer += bufferStride;
            }
        }

        return true;
    }
}

#undef GET_GRADIENT_VALUE_PERSP
#undef GET_GRADIENT_VALUE_AFFINE
#undef SETUP_GRADIENT_EQ
#undef DEFINE_GRADIENT
#undef GRADIENT_DY
#undef GRADIENT_DX
#undef GRADIENT_VALUE
