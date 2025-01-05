#pragma once
#include "Opti.h"

/*
	Some notes;
		Must have operators overridden.
			+ - / * [] etc.
		Must be able to cast to other data types
			__m128i to __m128
			__m128i to __m128i (cast from 8 bit to 16 bit. Need 2 functions for low and high)
			etc.
		Must be able to shuffle and blend within a datatype
		Must be able to do bitwise operations
		Must be able to get raw datatype (to do other operations not implemented)
		Need a means to use __m128 or __m256 depending on preprocessor.
		Need a means to use these even if SSE or higher isn't available
*/


//SSE
#if (OPTI >= 1)
class SIMD_128_U8;
class SIMD_128_U16;
class SIMD_128_U32;
// class SIMD_128_U64;
class SIMD_128_8;
class SIMD_128_16;
class SIMD_128_32;
// class SIMD_128_64;
class SIMD_128_FP32;
class SIMD_128_FP64;
#endif

//AVX
// class SIMD_256_U8;

#if (OPTI>=1)
typedef SIMD_128_8 SIMD_8;
typedef SIMD_128_U8 SIMD_U8;
typedef SIMD_128_16 SIMD_16;
typedef SIMD_128_U16 SIMD_U16;
typedef SIMD_128_32 SIMD_32;
typedef SIMD_128_U32 SIMD_U32;
// typedef SIMD_128_64 SIMD_64;
// typedef SIMD_128_U64 SIMD_U64;
typedef SIMD_128_FP32 SIMD_FP32;
typedef SIMD_128_FP64 SIMD_FP64;

#define SIMD_FP32_BLEND(x, y, imm) SIMD_128_FP32_BLEND(x, y, imm)
#define SIMD_FP32_BLEND_MASK(x, y, mask) SIMD_128_FP32_BLEND_MASK(x, y, mask)

#else
typedef void SIMD_8;
typedef void SIMD_U8;
typedef void SIMD_16;
typedef void SIMD_U16;
typedef void SIMD_32;
typedef void SIMD_U32;
// typedef void SIMD_64;
// typedef void SIMD_U64;
typedef void SIMD_FP32;
typedef void SIMD_FP64;
#endif

#include "SIMD_8.h"
#include "SIMD_16.h"
#include "SIMD_32.h"
#include "SIMD_FP32.h"
#include "SIMD_FP64.h"