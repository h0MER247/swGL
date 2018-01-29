#include <cmath>
#include <algorithm>
#include "SIMD.h"
#include "TextureManager.h"

namespace SWGL {

    static float getLambda(TextureData *texData, TextureCoordinates &texCoords) {

        auto &u = texCoords.s;
        auto &v = texCoords.t;

        // Calculate the partial derivatives for each texture coordinate. Those tell us how much we
        // have to "move" inside texture space for one pixel in screen space.
        float u1 = SIMD::extract<1>(u), u2 = SIMD::extract<2>(u), u3 = SIMD::extract<3>(u);
        float v1 = SIMD::extract<1>(v), v2 = SIMD::extract<2>(v), v3 = SIMD::extract<3>(v);

        float w = static_cast<float>(texData->mips[0][0].width);
        float h = static_cast<float>(texData->mips[0][0].height);

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
        // artifacts. If "A" is small, then one texel must be mapped to many pixels, which in turn results
        // in a "blocky" looking texture. One texel of detail level "L" is generally obtained by blending
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



    void sampleTexelsLinear(TextureMipMap &texMipMap, TextureParameter &texParams, TextureCoordinates &texCoords, ARGBColor &colorOut) {

        // Get the dimension of the texture
        QInt width = _mm_set1_epi32(texMipMap.width);
        QInt height = _mm_set1_epi32(texMipMap.height);
        QInt wrapWidth = _mm_set1_epi32(texMipMap.width - 1);
        QInt wrapHeight = _mm_set1_epi32(texMipMap.height - 1);

        // Scale u and v according to the textures dimensions
        QFloat scaledU = _mm_sub_ps(_mm_mul_ps(texCoords.s, _mm_cvtepi32_ps(width)), _mm_set1_ps(0.5f));
        QFloat scaledV = _mm_sub_ps(_mm_mul_ps(texCoords.t, _mm_cvtepi32_ps(height)), _mm_set1_ps(0.5f));
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

        // Calculate the blending weights as Q1.8 fixed point values
        const QFloat shift = _mm_set1_ps(256.0f);
        QInt wx1y1 = _mm_cvtps_epi32(_mm_mul_ps(_mm_mul_ps(fracX0, fracY0), shift));
        QInt wx0y1 = _mm_cvtps_epi32(_mm_mul_ps(_mm_mul_ps(fracX1, fracY0), shift));
        QInt wx1y0 = _mm_cvtps_epi32(_mm_mul_ps(_mm_mul_ps(fracX0, fracY1), shift));
        QInt wx0y0 = _mm_cvtps_epi32(_mm_mul_ps(_mm_mul_ps(fracX1, fracY1), shift));

        // TODO: Implement the wrapping modes correctly!

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

        auto data = reinterpret_cast<const int *>(texMipMap.pixel.data());
        QInt sampleX1Y1 = SIMD::gather(data, texelOffsetX1Y1);
        QInt sampleX0Y1 = SIMD::gather(data, texelOffsetX0Y1);
        QInt sampleX1Y0 = SIMD::gather(data, texelOffsetX1Y0);
        QInt sampleX0Y0 = SIMD::gather(data, texelOffsetX0Y0);

        // Extract alpha/green and red/blue channels
        const QInt channelMask = _mm_set1_epi32(0x00ff00ff);

        QInt ag[4], rb[4];
        ag[0] = _mm_and_si128(_mm_srli_epi32(sampleX0Y0, 8), channelMask);
        ag[1] = _mm_and_si128(_mm_srli_epi32(sampleX1Y0, 8), channelMask);
        ag[2] = _mm_and_si128(_mm_srli_epi32(sampleX0Y1, 8), channelMask);
        ag[3] = _mm_and_si128(_mm_srli_epi32(sampleX1Y1, 8), channelMask);
        rb[0] = _mm_and_si128(sampleX0Y0, channelMask);
        rb[1] = _mm_and_si128(sampleX1Y0, channelMask);
        rb[2] = _mm_and_si128(sampleX0Y1, channelMask);
        rb[3] = _mm_and_si128(sampleX1Y1, channelMask);

        // Blend samples
        QInt blendAG = _mm_add_epi32(_mm_add_epi32(_mm_add_epi32(_mm_mullo_epi32(ag[0], wx0y0), _mm_mullo_epi32(ag[1], wx1y0)), _mm_mullo_epi32(ag[2], wx0y1)), _mm_mullo_epi32(ag[3], wx1y1));
        QInt blendRB = _mm_add_epi32(_mm_add_epi32(_mm_add_epi32(_mm_mullo_epi32(rb[0], wx0y0), _mm_mullo_epi32(rb[1], wx1y0)), _mm_mullo_epi32(rb[2], wx0y1)), _mm_mullo_epi32(rb[3], wx1y1));

        // Convert the rgba-channels to their floating point representation
        const QFloat normalize = _mm_set1_ps(1.0f / 255.0f);
        const QInt mask = _mm_set1_epi32(0xff);

        QFloat a = _mm_cvtepi32_ps(_mm_srli_epi32(blendAG, 24));
        QFloat g = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(blendAG, 8), mask));
        QFloat r = _mm_cvtepi32_ps(_mm_srli_epi32(blendRB, 24));
        QFloat b = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(blendRB, 8), mask));

        colorOut.a = _mm_mul_ps(a, normalize);
        colorOut.r = _mm_mul_ps(r, normalize);
        colorOut.g = _mm_mul_ps(g, normalize);
        colorOut.b = _mm_mul_ps(b, normalize);
    }

    void sampleTexelsNearest(TextureMipMap &texMipMap, TextureParameter &texParams, TextureCoordinates &texCoords, ARGBColor &colorOut) {

        // Get the dimension of the texture
        QInt width = _mm_set1_epi32(texMipMap.width);
        QInt height = _mm_set1_epi32(texMipMap.height);
        QInt wrapWidth = _mm_set1_epi32(texMipMap.width - 1);
        QInt wrapHeight = _mm_set1_epi32(texMipMap.height - 1);

        // Scale u and v according to the texture dimension
        QInt scaledU = _mm_cvttps_epi32(SIMD::floor(_mm_mul_ps(texCoords.s, _mm_cvtepi32_ps(width))));
        QInt scaledV = _mm_cvttps_epi32(SIMD::floor(_mm_mul_ps(texCoords.t, _mm_cvtepi32_ps(height))));

        // TODO: Implement the wrapping modes correctly!

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

            reinterpret_cast<const int *>(texMipMap.pixel.data()),
            texelOffset
        );

        // Convert the rgba-channels to their floating point representation
        const QFloat normalize = _mm_set1_ps(1.0f / 255.0f);
        const QInt mask = _mm_set1_epi32(0xff);

        QFloat a = _mm_cvtepi32_ps(_mm_srli_epi32(samples, 24));
        QFloat r = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(samples, 16), mask));
        QFloat g = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(samples, 8), mask));
        QFloat b = _mm_cvtepi32_ps(_mm_and_si128(samples, mask));

        colorOut.a = _mm_mul_ps(normalize, a);
        colorOut.r = _mm_mul_ps(normalize, r);
        colorOut.g = _mm_mul_ps(normalize, g);
        colorOut.b = _mm_mul_ps(normalize, b);
    }



    void TextureData2D::sampleTexels(TextureParameter &texParams, TextureCoordinates &texCoords, ARGBColor &colorOut) {

        if (texParams.isUsingMipMapping) {

            // TODO: The constant "c" depends on some GL state: If magnifyFilter is GL_LINEAR and minifyFilter is
            //       GL_NEAREST_MIPMAP_NEAREST or NEAREST_MIPMAP_LINEAR, then c is equal to 0.5f (otherwise 0.0f)
            static constexpr float c = 0.5f;

            // To make things easier the lod is determined by the lod of the 2x2 quads upper left pixel. That lod
            // is then applied to all four pixels of the quad.
            float lambda = getLambda(this, texCoords);

            //
            // Texture minification
            //
            if (lambda >= c) {

                if (texParams.isUsingTrilinearFilter) {

                    int lod = static_cast<int>(lambda);
                    if (lod >= maxLOD) {

                        texParams.minifySampler(mips[maxLOD][0], texParams, texCoords, colorOut);
                    }
                    else {

                        QFloat t = _mm_set1_ps(lambda - std::floor(lambda));

                        ARGBColor color1;
                        ARGBColor color2;
                        texParams.minifySampler(mips[lod][0], texParams, texCoords, color1);
                        texParams.minifySampler(mips[lod + 1][0], texParams, texCoords, color2);

                        colorOut.a = SIMD::lerp(t, color1.a, color2.a);
                        colorOut.r = SIMD::lerp(t, color1.r, color2.r);
                        colorOut.g = SIMD::lerp(t, color1.g, color2.g);
                        colorOut.b = SIMD::lerp(t, color1.b, color2.b);
                    }
                }
                else {

                    int lod = static_cast<int>(std::ceil(lambda + 0.5f) - 1.0f);
                    if (lod > maxLOD) {

                        lod = maxLOD;
                    }

                    texParams.minifySampler(mips[lod][0], texParams, texCoords, colorOut);
                }

                return;
            }
            // Fall through (texture is magnified)
        }

        //
        // Disabled Mip Mapping / Texture magnification
        //
        texParams.magnifySampler(mips[0][0], texParams, texCoords, colorOut);
    }



    void TextureData1D::sampleTexels(TextureParameter &texParams, TextureCoordinates &texCoords, ARGBColor &colorOut) {

        // Unimplemented
        colorOut.a = _mm_set1_ps(1.0f);
        colorOut.r = _mm_set1_ps(1.0f);
        colorOut.g = _mm_set1_ps(0.0f);
        colorOut.b = _mm_set1_ps(1.0f);
    }

    void TextureData3D::sampleTexels(TextureParameter &texParams, TextureCoordinates &texCoords, ARGBColor &colorOut) {

        // Unimplemented
        colorOut.a = _mm_set1_ps(1.0f);
        colorOut.r = _mm_set1_ps(1.0f);
        colorOut.g = _mm_set1_ps(0.0f);
        colorOut.b = _mm_set1_ps(1.0f);
    }



    //
    // TODO: This is just a test implementation to see if it works in general.
    //
    void TextureDataCubeMap::sampleTexels(TextureParameter &texParams, TextureCoordinates &texCoords, ARGBColor &colorOut) {

        // TODO: SIMDify this
        TextureMipMap *mip[4];
        QFloat u, v;

        for (int i = 0; i < 4; i++) {

            float rx = texCoords.s.m128_f32[i]; if (rx == 0.0f) rx = 0.00001f;
            float ry = texCoords.t.m128_f32[i]; if (ry == 0.0f) ry = 0.00001f;
            float rz = texCoords.r.m128_f32[i]; if (rz == 0.0f) rz = 0.00001f;

            float arx = std::abs(rx);
            float ary = std::abs(ry);
            float arz = std::abs(rz);
            float sc, tc, ma;

            if (arx >= ary && arx >= arz) {

                if (rx >= 0.0f) {

                    mip[i] = &mips[0][0]; sc = -rz; tc = -ry; ma = 0.5f / arx;
                }
                else {

                    mip[i] = &mips[0][1]; sc = +rz; tc = -ry; ma = 0.5f / arx;
                }
            }
            else if (ary >= arx && ary >= arz) {

                if (ry >= 0.0f) {

                    mip[i] = &mips[0][2]; sc = +rx; tc = +rz; ma = 0.5f / ary; 
                }
                else {

                    mip[i] = &mips[0][3]; sc = +rx; tc = -rz; ma = 0.5f / ary;
                }
            }
            else {

                if (rz >= 0.0f) {

                    mip[i] = &mips[0][4]; sc = +rx; tc = -ry; ma = 0.5f / arz; 
                }
                else {

                    mip[i] = &mips[0][5]; sc = -rx; tc = -ry; ma = 0.5f / arz;
                }
            }

            u.m128_f32[i] = 0.5f + (sc * ma);
            v.m128_f32[i] = 0.5f + (tc * ma);
        }

        // TODO: The type of filter should also be selectable!!! This here is a linear filter:

        // Get the dimension of the texture
        QInt dim = _mm_set_epi32(mip[3]->width, mip[2]->width, mip[1]->width, mip[0]->width);
        QInt wrapDim = _mm_sub_epi32(dim, _mm_set1_epi32(1));

        // Scale u and v according to the textures dimensions
        QFloat scaledU = _mm_sub_ps(_mm_mul_ps(u, _mm_cvtepi32_ps(dim)), _mm_set1_ps(0.5f));
        QFloat scaledV = _mm_sub_ps(_mm_mul_ps(v, _mm_cvtepi32_ps(dim)), _mm_set1_ps(0.5f));
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

        // Calculate the blending weights as Q1.8 fixed point values
        const QFloat shift = _mm_set1_ps(256.0f);
        QInt wx1y1 = _mm_cvtps_epi32(_mm_mul_ps(_mm_mul_ps(fracX0, fracY0), shift));
        QInt wx0y1 = _mm_cvtps_epi32(_mm_mul_ps(_mm_mul_ps(fracX1, fracY0), shift));
        QInt wx1y0 = _mm_cvtps_epi32(_mm_mul_ps(_mm_mul_ps(fracX0, fracY1), shift));
        QInt wx0y0 = _mm_cvtps_epi32(_mm_mul_ps(_mm_mul_ps(fracX1, fracY1), shift));

        // TODO: Implement the wrapping modes correctly!!!
        // Just clamp the texel coordinates for now...
        texelX0 = SIMD::clamp(texelX0, _mm_setzero_si128(), wrapDim);
        texelY0 = SIMD::clamp(texelY0, _mm_setzero_si128(), wrapDim);
        texelX1 = SIMD::clamp(texelX1, _mm_setzero_si128(), wrapDim);
        texelY1 = SIMD::clamp(texelY1, _mm_setzero_si128(), wrapDim);

        // Gather texture samples
        QInt texelOffsetX1Y1 = SIMD::multiplyAdd(texelY1, dim, texelX1);
        QInt texelOffsetX0Y1 = SIMD::multiplyAdd(texelY1, dim, texelX0);
        QInt texelOffsetX1Y0 = SIMD::multiplyAdd(texelY0, dim, texelX1);
        QInt texelOffsetX0Y0 = SIMD::multiplyAdd(texelY0, dim, texelX0);

        QInt sampleX1Y1 = _mm_set_epi32(

            mip[3]->pixel[SIMD::extract<3>(texelOffsetX1Y1)],
            mip[2]->pixel[SIMD::extract<2>(texelOffsetX1Y1)],
            mip[1]->pixel[SIMD::extract<1>(texelOffsetX1Y1)],
            mip[0]->pixel[SIMD::extract<0>(texelOffsetX1Y1)]
        );
        QInt sampleX0Y1 = _mm_set_epi32(

            mip[3]->pixel[SIMD::extract<3>(texelOffsetX0Y1)],
            mip[2]->pixel[SIMD::extract<2>(texelOffsetX0Y1)],
            mip[1]->pixel[SIMD::extract<1>(texelOffsetX0Y1)],
            mip[0]->pixel[SIMD::extract<0>(texelOffsetX0Y1)]
        );
        QInt sampleX1Y0 = _mm_set_epi32(

            mip[3]->pixel[SIMD::extract<3>(texelOffsetX1Y0)],
            mip[2]->pixel[SIMD::extract<2>(texelOffsetX1Y0)],
            mip[1]->pixel[SIMD::extract<1>(texelOffsetX1Y0)],
            mip[0]->pixel[SIMD::extract<0>(texelOffsetX1Y0)]
        );
        QInt sampleX0Y0 = _mm_set_epi32(

            mip[3]->pixel[SIMD::extract<3>(texelOffsetX0Y0)],
            mip[2]->pixel[SIMD::extract<2>(texelOffsetX0Y0)],
            mip[1]->pixel[SIMD::extract<1>(texelOffsetX0Y0)],
            mip[0]->pixel[SIMD::extract<0>(texelOffsetX0Y0)]
        );

        // Extract alpha/green and red/blue channels
        const QInt channelMask = _mm_set1_epi32(0x00ff00ff);

        QInt ag[4], rb[4];
        ag[0] = _mm_and_si128(_mm_srli_epi32(sampleX0Y0, 8), channelMask);
        ag[1] = _mm_and_si128(_mm_srli_epi32(sampleX1Y0, 8), channelMask);
        ag[2] = _mm_and_si128(_mm_srli_epi32(sampleX0Y1, 8), channelMask);
        ag[3] = _mm_and_si128(_mm_srli_epi32(sampleX1Y1, 8), channelMask);
        rb[0] = _mm_and_si128(sampleX0Y0, channelMask);
        rb[1] = _mm_and_si128(sampleX1Y0, channelMask);
        rb[2] = _mm_and_si128(sampleX0Y1, channelMask);
        rb[3] = _mm_and_si128(sampleX1Y1, channelMask);

        // Blend samples
        QInt blendAG = _mm_add_epi32(_mm_add_epi32(_mm_add_epi32(_mm_mullo_epi32(ag[0], wx0y0), _mm_mullo_epi32(ag[1], wx1y0)), _mm_mullo_epi32(ag[2], wx0y1)), _mm_mullo_epi32(ag[3], wx1y1));
        QInt blendRB = _mm_add_epi32(_mm_add_epi32(_mm_add_epi32(_mm_mullo_epi32(rb[0], wx0y0), _mm_mullo_epi32(rb[1], wx1y0)), _mm_mullo_epi32(rb[2], wx0y1)), _mm_mullo_epi32(rb[3], wx1y1));

        // Convert the rgba-channels to their floating point representation
        const QFloat normalize = _mm_set1_ps(1.0f / 255.0f);
        const QInt mask = _mm_set1_epi32(0xff);

        QFloat a = _mm_cvtepi32_ps(_mm_srli_epi32(blendAG, 24));
        QFloat g = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(blendAG, 8), mask));
        QFloat r = _mm_cvtepi32_ps(_mm_srli_epi32(blendRB, 24));
        QFloat b = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(blendRB, 8), mask));

        colorOut.a = _mm_mul_ps(a, normalize);
        colorOut.r = _mm_mul_ps(r, normalize);
        colorOut.g = _mm_mul_ps(g, normalize);
        colorOut.b = _mm_mul_ps(b, normalize);
    }
}
