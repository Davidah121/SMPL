#pragma once
#include "SIMD.h"
#include "SEML.h"
#include "SEML_256.h"

namespace smpl
{
    class SIMDConverter
    {
    public:
        #ifdef __SSE4_2__
        //cast / convert operations
        template<int N>
        static SIMD_128_FP32 convertToFP32(SIMD_128_FP16 v)
        {
            return SIMD_128_FP32(_mm_cvtph_ps(_mm_srli_si128(v.values, N*4)));
        }

        static SIMD_128_FP16 convertToFP16(SIMD_128_FP32 v)
        {
            return SIMD_128_FP16(_mm_cvtps_ph(v.values, ROUNDING_RULE));
        }

        static SIMD_128_FP32 convertToFP32(SIMD_128_32 v)
        {
            return SIMD_128_FP32(SEML::int32ToFloat(v.values));
        }
        
        static SIMD_128_32 convertToInt32(SIMD_128_FP32 v)
        {
            return SIMD_128_32(SEML::floatToInt32(v.values));
        }
        #endif


        #ifdef __AVX2__
        template<int N>
        static SIMD_256_FP32 convertToFP32(SIMD_256_FP16 v)
        {
            return SIMD_256_FP32(_mm256_cvtph_ps(_mm256_extracti128_si256(v.values, N)));
        }
        
        static SIMD_256_FP32 convertToFP32(SIMD_256_32 v)
        {
            return SIMD_256_FP32(SEML::int32ToFloat(v.values));
        }
        
        static SIMD_256_32 convertToInt32(SIMD_256_FP32 v)
        {
            return SIMD_256_32(SEML::floatToInt32(v.values));
        }
        #endif

    };
}