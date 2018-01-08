#pragma once

#include <intrin.h>
#include "Defines.h"

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

        INLINED QFloat absolute(QFloat value) {

            return _mm_andnot_ps(_mm_set1_ps(-0.0f), value);
        }

        INLINED QFloat clamp(QFloat value, QFloat min, QFloat max) {

            return _mm_max_ps(_mm_min_ps(value, max), min);
        }

        INLINED QInt clamp(QInt value, QInt min, QInt max) {

            return _mm_max_epi32(_mm_min_epi32(value, max), min);
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

                base[_mm_extract_epi32(index, 3)],
                base[_mm_extract_epi32(index, 2)],
                base[_mm_extract_epi32(index, 1)],
                base[_mm_extract_epi32(index, 0)]
            );
        #endif
        }

        INLINED QFloat floor(QFloat value) {

            return _mm_round_ps(value, _MM_FROUND_TO_NEG_INF | _MM_FROUND_NO_EXC);
        }

        INLINED QFloat ceil(QFloat value) {

            return _mm_round_ps(value, _MM_FROUND_TO_POS_INF | _MM_FROUND_NO_EXC);
        }
    }
}
