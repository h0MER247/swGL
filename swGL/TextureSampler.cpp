#include <cmath>
#include <algorithm>
#include "SIMD.h"
#include "TextureManager.h"

namespace SWGL {

    static float getLambda(TextureObjectPtr &tex, TextureCoordinates &texCoords) {

        auto u = texCoords.u;
        auto v = texCoords.v;

        // Calculate the partial derivatives for each texture coordinate. Those tell us how much we
        // have to "move" inside texture space for one pixel in screen space.
        float u1 = SIMD::extract<1>(u), u2 = SIMD::extract<2>(u), u3 = SIMD::extract<3>(u);
        float v1 = SIMD::extract<1>(v), v2 = SIMD::extract<2>(v), v3 = SIMD::extract<3>(v);

        float w = static_cast<float>(tex->mips[0].width);
        float h = static_cast<float>(tex->mips[0].height);

        float dudx = (u2 - u3) * w, dvdx = (v2 - v3) * h;
        float dudy = (u1 - u3) * w, dvdy = (v1 - v3) * h;

        // We now make a rough estimation of how big our pixel is in texture space. The assumption here
        // is, that the pixel maps to a rectangular shape in texture space. The side lengths of the
        // rectangle (the pixel "footprint" in texture space) are determined by r1 = sqrt(dudx² + dvdx²)
        // and r2 = sqrt(dudy² + dvdy²). The side with the maximum length is then chosen to be the side
        // lengths of a square shaped pixel footprint. Its area can than be calculated rather easily by
        // A = max(r1, r2)² = max(r1², r2²).
        float squaredR1 = (dudx * dudx) + (dvdx * dvdx);
        float squaredR2 = (dudy * dudy) + (dvdy * dvdy);
        float A = std::max(squaredR1, squaredR2);

        // The value of "A" tells us, how many texels of detail level 0 should be mapped to one pixel. If
        // "A" is big, then many texels must be mapped to one pixel. This will result in the typical alias
        // artifacts. If "A" is small, then one texel must be mapped to many pixels, which in  turn results
        // in a "blocky" looking texture. One texel of detail level "L" is generally  obtained by blending
        // 2^(2*L) texel from detail level 0. If we assume that "A" is equal to 2^(2*L), we can calculate
        // the detail level "L" in which one texel maps to (roughly) one pixel:
        //
        //        2^(2*L) = A
        //   log(2^(2*L)) = log(A)
        // log(2) * (2*L) = log(A)
        //          2 * L = log(A) / log(2)
        //
        //           L = 0.5 * log_2(A)
        //             = 0.5 * log_2(max(dudx²+dvdx², dudy²+dvdy²))
        //
        // The value obtained here is a approximation of L = 0.5 * log_2(A):
        union {

            float f;
            int i;

        } value = { A };

        return (static_cast<float>(value.i) * 0.000000059604644775390625f) - 63.47134752f;
    }



    void sampleTexelsLinear(TextureObjectPtr &tex, TextureParameter &texParams, int lod, TextureCoordinates &texCoords, ARGBColor &color) {

        auto &mip = tex->mips[lod];

        // Get the dimension of the texture
        QInt width = _mm_set1_epi32(mip.width);
        QInt height = _mm_set1_epi32(mip.height);
        QInt wrapWidth = _mm_set1_epi32(mip.width - 1);
        QInt wrapHeight = _mm_set1_epi32(mip.height - 1);

        // Scale u and v according to the textures dimensions
        QFloat scaledU = _mm_sub_ps(_mm_mul_ps(texCoords.u, _mm_cvtepi32_ps(width)), _mm_set1_ps(0.5f));
        QFloat scaledV = _mm_sub_ps(_mm_mul_ps(texCoords.v, _mm_cvtepi32_ps(height)), _mm_set1_ps(0.5f));
        QFloat flooredU = SIMD::floor(scaledU);
        QFloat flooredV = SIMD::floor(scaledV);

        QInt texelX0 = _mm_cvttps_epi32(flooredU);
        QInt texelY0 = _mm_cvttps_epi32(flooredV);
        QInt texelX1 = _mm_add_epi32(texelX0, _mm_set1_epi32(1));
        QInt texelY1 = _mm_add_epi32(texelY0, _mm_set1_epi32(1));

        // Get fractional part of u and v
        QFloat fracX0 = _mm_sub_ps(scaledU, flooredU);
        QFloat fracY0 = _mm_sub_ps(scaledV, flooredV);
        QFloat fracX1 = _mm_sub_ps(_mm_set1_ps(1.0f), fracX0);
        QFloat fracY1 = _mm_sub_ps(_mm_set1_ps(1.0f), fracY0);

        // Calculate the blending weights
        QFloat wx1y1 = _mm_mul_ps(fracX0, fracY0);
        QFloat wx0y1 = _mm_mul_ps(fracX1, fracY0);
        QFloat wx1y0 = _mm_mul_ps(fracX0, fracY1);
        QFloat wx0y0 = _mm_mul_ps(fracX1, fracY1);

        // Determine the texel x- and y-coordinates according to the selected wrapping mode
        if (texParams.wrappingModeS == GL_REPEAT) {

            texelX0 = _mm_and_si128(texelX0, wrapWidth);
            texelX1 = _mm_and_si128(texelX1, wrapWidth);
        }
        else {

            texelX0 = SIMD::clamp(texelX0, _mm_setzero_si128(), wrapWidth);
            texelX1 = SIMD::clamp(texelX1, _mm_setzero_si128(), wrapWidth);
        }
        if (texParams.wrappingModeT == GL_REPEAT) {

            texelY0 = _mm_and_si128(texelY0, wrapHeight);
            texelY1 = _mm_and_si128(texelY1, wrapHeight);
        }
        else {

            texelY0 = SIMD::clamp(texelY0, _mm_setzero_si128(), wrapHeight);
            texelY1 = SIMD::clamp(texelY1, _mm_setzero_si128(), wrapHeight);
        }

        // Gather texture samples
        QInt texelOffsetX1Y1 = SIMD::multiplyAdd(texelY1, width, texelX1);
        QInt texelOffsetX0Y1 = SIMD::multiplyAdd(texelY1, width, texelX0);
        QInt texelOffsetX1Y0 = SIMD::multiplyAdd(texelY0, width, texelX1);
        QInt texelOffsetX0Y0 = SIMD::multiplyAdd(texelY0, width, texelX0);

        auto data = reinterpret_cast<const int *>(mip.pixel.data());
        QInt sampleX1Y1 = SIMD::gather(data, texelOffsetX1Y1);
        QInt sampleX0Y1 = SIMD::gather(data, texelOffsetX0Y1);
        QInt sampleX1Y0 = SIMD::gather(data, texelOffsetX1Y0);
        QInt sampleX0Y0 = SIMD::gather(data, texelOffsetX0Y0);

        // Convert the rgba-channels to their floating point representation
        const QFloat normalize = _mm_set1_ps(1.0f / 255.0f);
        const QInt mask = _mm_set1_epi32(0xff);

        QFloat a_[4], r_[4], g_[4], b_[4];
        a_[0] = _mm_cvtepi32_ps(_mm_srli_epi32(sampleX0Y0, 24));
        a_[1] = _mm_cvtepi32_ps(_mm_srli_epi32(sampleX1Y0, 24));
        a_[2] = _mm_cvtepi32_ps(_mm_srli_epi32(sampleX0Y1, 24));
        a_[3] = _mm_cvtepi32_ps(_mm_srli_epi32(sampleX1Y1, 24));
        r_[0] = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(sampleX0Y0, 16), mask));
        r_[1] = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(sampleX1Y0, 16), mask));
        r_[2] = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(sampleX0Y1, 16), mask));
        r_[3] = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(sampleX1Y1, 16), mask));
        g_[0] = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(sampleX0Y0, 8), mask));
        g_[1] = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(sampleX1Y0, 8), mask));
        g_[2] = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(sampleX0Y1, 8), mask));
        g_[3] = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(sampleX1Y1, 8), mask));
        b_[0] = _mm_cvtepi32_ps(_mm_and_si128(sampleX0Y0, mask));
        b_[1] = _mm_cvtepi32_ps(_mm_and_si128(sampleX1Y0, mask));
        b_[2] = _mm_cvtepi32_ps(_mm_and_si128(sampleX0Y1, mask));
        b_[3] = _mm_cvtepi32_ps(_mm_and_si128(sampleX1Y1, mask));

        // Blend samples
        color.a = _mm_mul_ps(normalize, _mm_add_ps(_mm_add_ps(_mm_mul_ps(a_[0], wx0y0), _mm_mul_ps(a_[1], wx1y0)), _mm_add_ps(_mm_mul_ps(a_[2], wx0y1), _mm_mul_ps(a_[3], wx1y1))));
        color.r = _mm_mul_ps(normalize, _mm_add_ps(_mm_add_ps(_mm_mul_ps(r_[0], wx0y0), _mm_mul_ps(r_[1], wx1y0)), _mm_add_ps(_mm_mul_ps(r_[2], wx0y1), _mm_mul_ps(r_[3], wx1y1))));
        color.g = _mm_mul_ps(normalize, _mm_add_ps(_mm_add_ps(_mm_mul_ps(g_[0], wx0y0), _mm_mul_ps(g_[1], wx1y0)), _mm_add_ps(_mm_mul_ps(g_[2], wx0y1), _mm_mul_ps(g_[3], wx1y1))));
        color.b = _mm_mul_ps(normalize, _mm_add_ps(_mm_add_ps(_mm_mul_ps(b_[0], wx0y0), _mm_mul_ps(b_[1], wx1y0)), _mm_add_ps(_mm_mul_ps(b_[2], wx0y1), _mm_mul_ps(b_[3], wx1y1))));
    }

    void sampleTexelsNearest(TextureObjectPtr &tex, TextureParameter &texParams, int lod, TextureCoordinates &texCoords, ARGBColor &color) {

        auto &mip = tex->mips[lod];

        // Get the dimension of the texture
        QInt width = _mm_set1_epi32(mip.width);
        QInt height = _mm_set1_epi32(mip.height);
        QInt wrapWidth = _mm_set1_epi32(mip.width - 1);
        QInt wrapHeight = _mm_set1_epi32(mip.height - 1);

        // Scale u and v according to the texture dimension
        QInt scaledU = _mm_cvttps_epi32(SIMD::floor(_mm_mul_ps(texCoords.u, _mm_cvtepi32_ps(width))));
        QInt scaledV = _mm_cvttps_epi32(SIMD::floor(_mm_mul_ps(texCoords.v, _mm_cvtepi32_ps(height))));

        // Determine the texel x- and y-coordinates according to the selected wrapping mode
        QInt texelX, texelY;
        if (texParams.wrappingModeS == GL_REPEAT) {

            texelX = _mm_and_si128(scaledU, wrapWidth);
        }
        else {

            texelX = SIMD::clamp(scaledU, _mm_setzero_si128(), wrapWidth);
        }
        if (texParams.wrappingModeT == GL_REPEAT) {

            texelY = _mm_and_si128(scaledV, wrapHeight);
        }
        else {

            texelY = SIMD::clamp(scaledV, _mm_setzero_si128(), wrapHeight);
        }

        // Gather texture samples
        QInt texelOffset = SIMD::multiplyAdd(texelY, width, texelX);
        QInt samples = SIMD::gather(

            reinterpret_cast<const int *>(mip.pixel.data()),
            texelOffset
        );

        // Convert the rgba-channels to their floating point representation
        const QFloat normalize = _mm_set1_ps(1.0f / 255.0f);
        const QInt mask = _mm_set1_epi32(0xff);

        color.a = _mm_mul_ps(normalize, _mm_cvtepi32_ps(_mm_srli_epi32(samples, 24)));
        color.r = _mm_mul_ps(normalize, _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(samples, 16), mask)));
        color.g = _mm_mul_ps(normalize, _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(samples, 8), mask)));
        color.b = _mm_mul_ps(normalize, _mm_cvtepi32_ps(_mm_and_si128(samples, mask)));
    }

    void sampleTexels(TextureObjectPtr &tex, TextureParameter &texParams, TextureCoordinates &texCoords, ARGBColor &color) {

        if (texParams.isUsingMipMapping) {

            // TODO: The constant "c" depends on some GL state: If magnifyFilter is GL_LINEAR and minifyFilter is
            //       GL_NEAREST_MIPMAP_NEAREST or NEAREST_MIPMAP_LINEAR, then c is equal to 0.5f (otherwise 0.0f)
            constexpr float c = 0.5f;

            // To make things easier the lod is determined by the lod of the 2x2 quads upper left pixel. That lod
            // is then applied to all four pixels of the quad.
            float lambda = getLambda(tex, texCoords);

            //
            // Texture minification
            //
            if (lambda >= c) {

                if (texParams.isUsingTrilinearFilter) {

                    int lod = static_cast<int>(lambda);
                    if (lod >= tex->maxLOD) {

                        texParams.minifySampler(tex, texParams, tex->maxLOD, texCoords, color);
                    }
                    else {

                        QFloat t = _mm_set1_ps(lambda - std::floor(lambda));

                        ARGBColor color1;
                        ARGBColor color2;
                        texParams.minifySampler(tex, texParams, lod, texCoords, color1);
                        texParams.minifySampler(tex, texParams, lod + 1, texCoords, color2);

                        color.a = SIMD::lerp(t, color1.a, color2.a);
                        color.r = SIMD::lerp(t, color1.r, color2.r);
                        color.g = SIMD::lerp(t, color1.g, color2.g);
                        color.b = SIMD::lerp(t, color1.b, color2.b);
                    }
                }
                else {

                    int lod = static_cast<int>(std::ceil(lambda + 0.5f) - 1.0f);
                    if (lod > tex->maxLOD)
                        lod = tex->maxLOD;

                    texParams.minifySampler(tex, texParams, lod, texCoords, color);
                }

                return;
            }
            // Fall through (texture is magnified)
        }

        //
        // Disabled Mip Mapping / Texture magnification
        //
        texParams.magnifySampler(tex, texParams, 0, texCoords, color);
    }
}
