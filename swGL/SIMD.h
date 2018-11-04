#pragma once

#include <intrin.h>
#include "Defines.h"

#define _mm_setallones_si128() \
    _mm_cmpeq_epi32(_mm_setzero_si128(), _mm_setzero_si128())
#define _mm_cmple_epi32(a, b) \
    _mm_xor_si128(_mm_cmpgt_epi32(a, b), _mm_setallones_si128())
#define _mm_cmpge_epi32(a, b) \
    _mm_xor_si128(_mm_cmplt_epi32(a, b), _mm_setallones_si128())
#define _mm_cmpneq_epi32(a, b) \
    _mm_xor_si128(_mm_cmpeq_epi32(a, b), _mm_setallones_si128())

namespace SWGL {

    using QFloat = __m128;
    using QInt = __m128i;

    namespace SIMD {

        template<int idx>
        INLINED float extract(QFloat value) {

            return _mm_cvtss_f32(_mm_shuffle_ps((value), (value), _MM_SHUFFLE(0, 0, 0, idx)));
        }

        template<>
        INLINED float extract<0>(QFloat value) {

            return _mm_cvtss_f32(value);
        }

        template<int idx>
        INLINED int extract(QInt value) {

            //return _mm_cvtsi128_si32(_mm_shuffle_epi32(value, _MM_SHUFFLE(0, 0, 0, idx)));
            return _mm_extract_epi32(value, idx);
        }

        template<>
        INLINED int extract<0>(QInt value) {

            return _mm_cvtsi128_si32(value);
        }

        INLINED QFloat absolute(QFloat value) {

            return _mm_andnot_ps(_mm_castsi128_ps(_mm_set1_epi32(0x80000000)), value);
        }

        INLINED QFloat negate(QFloat value) {

            return _mm_xor_ps(_mm_castsi128_ps(_mm_set1_epi32(0x80000000)), value);
        }

        INLINED QFloat clamp(QFloat value, QFloat min, QFloat max) {

            return _mm_max_ps(_mm_min_ps(value, max), min);
        }

        INLINED QInt clamp(QInt value, QInt min, QInt max) {

            return _mm_max_epi32(_mm_min_epi32(value, max), min);
        }

        INLINED QFloat clamp01(QFloat value) {

            const QFloat zero = _mm_setzero_ps();
            const QFloat one = _mm_set1_ps(1.0f);

            return _mm_max_ps(_mm_min_ps(value, one), zero);
        }

        INLINED QFloat blend(QFloat valueA, QFloat valueB, QFloat mask) {

            return _mm_blendv_ps(valueA, valueB, mask);
        }

        INLINED QInt blend(QInt valueA, QInt valueB, QInt mask) {

            return _mm_castps_si128(

                _mm_blendv_ps(
            
                    _mm_castsi128_ps(valueA),
                    _mm_castsi128_ps(valueB),
                    _mm_castsi128_ps(mask)
                )
            );
        }

        INLINED QInt mask(QInt valueA, QInt valueB, QInt mask) {

            return _mm_or_si128(

                _mm_and_si128(mask, valueA),
                _mm_andnot_si128(mask, valueB)
            );
        }

        INLINED QInt multiplyAdd(QInt valueA, QInt valueB, QInt valueC) {

            return _mm_add_epi32(_mm_mullo_epi32(valueA, valueB), valueC);
        }

        INLINED QFloat multiplyAdd(QFloat valueA, QFloat valueB, QFloat valueC) {

            return _mm_add_ps(_mm_mul_ps(valueA, valueB), valueC);
        }

        INLINED QInt multiplySub(QInt valueA, QInt valueB, QInt valueC) {

            return _mm_sub_epi32(_mm_mullo_epi32(valueA, valueB), valueC);
        }

        INLINED QFloat multiplySub(QFloat valueA, QFloat valueB, QFloat valueC) {

            return _mm_sub_ps(_mm_mul_ps(valueA, valueB), valueC);
        }

        INLINED QFloat lerp(QFloat t, QFloat valueA, QFloat valueB) {

            return multiplyAdd(t, _mm_sub_ps(valueB, valueA), valueA);
        }

        INLINED QInt gather(const int *base, QInt index) {
        #if defined(__AVX2__) && SWGL_USE_AVX2_GATHER
            return _mm_i32gather_epi32(base, index, 1);
        #else
            return _mm_set_epi32(

                base[extract<3>(index)],
                base[extract<2>(index)],
                base[extract<1>(index)],
                base[extract<0>(index)]
            );
        #endif
        }

        INLINED QFloat floor(QFloat value) {

            return _mm_round_ps(value, _MM_FROUND_TO_NEG_INF | _MM_FROUND_NO_EXC);
        }

        INLINED QFloat ceil(QFloat value) {

            return _mm_round_ps(value, _MM_FROUND_TO_POS_INF | _MM_FROUND_NO_EXC);
        }

        INLINED QFloat dot3(QFloat x1, QFloat x2, QFloat y1, QFloat y2, QFloat z1, QFloat z2) {

            const QFloat half = _mm_set1_ps(0.5f);
            const QFloat four = _mm_set1_ps(4.0f);
            
            x1 = _mm_sub_ps(x1, half);
            x2 = _mm_sub_ps(x2, half);
            y1 = _mm_sub_ps(y1, half);
            y2 = _mm_sub_ps(y2, half);
            z1 = _mm_sub_ps(z1, half);
            z2 = _mm_sub_ps(z2, half);

            QFloat d1 = _mm_mul_ps(x1, x2);
            QFloat d2 = _mm_mul_ps(y1, y2);
            QFloat d3 = _mm_mul_ps(z1, z2);

            return _mm_mul_ps(four, _mm_add_ps(_mm_add_ps(d1, d2), d3));
        }
    }
}
