#pragma once
#include "BuildOptions.h"
#include "SIMD_Template.h"
#include "SIMD_8.h"
#include "SIMD_16.h"
#include "SIMD_32.h"
#include "SIMD_64.h"
#include "SIMD_FP.h"

#define ROUNDING_RULE _MM_FROUND_TO_NEAREST_INT

#ifndef OPTI
	#define OPTI 0
#endif

namespace smpl
{
	#ifndef SIMD_PREFERENCE
		#if (OPTI==1)
			#ifdef __SSE4_2__
				#define SIMD_PREFERENCE 1
			#else
				#define SIMD_PREFERENCE 0
			#endif
		#elif (OPTI==2)
			#ifdef __AVX2__
				#define SIMD_PREFERENCE 2
			#elif defined(__SSE4_2__)
				#define SIMD_PREFERENCE 1
			#else
				#define SIMD_PREFERENCE 0
			#endif
		#else
			#define SIMD_PREFERENCE 0
		#endif
	#endif


	#if (SIMD_PREFERENCE==1)
	template<typename T>
	using 	SIMD_TEMPLATE=				SIMD_SSE<T>;
	typedef SIMD_SSE<char>				SIMD_8;
	typedef SIMD_SSE<unsigned char>		SIMD_U8;
	typedef SIMD_SSE<int16_t>			SIMD_16;
	typedef SIMD_SSE<uint16_t>			SIMD_U16;
	typedef SIMD_SSE<int32_t>			SIMD_32;
	typedef SIMD_SSE<uint32_t>			SIMD_U32;
	typedef SIMD_SSE<int64_t>			SIMD_64;
	typedef SIMD_SSE<uint64_t>			SIMD_U64;
	typedef SIMD_SSE<float>				SIMD_FP32;
	typedef SIMD_SSE<double>			SIMD_FP64;

	#define SIMD_FP32_BLEND(x, y, imm) SIMD_128_FP32_BLEND(x, y, imm)
	#define SIMD_FP32_BLEND_MASK(x, y, mask) SIMD_128_FP32_BLEND_MASK(x, y, mask)

	#elif (SIMD_PREFERENCE==2)
	template<typename T>
	using 	SIMD_TEMPLATE=				SIMD_AVX<T>;
	typedef SIMD_AVX<char>				SIMD_8;
	typedef SIMD_AVX<unsigned char>		SIMD_U8;
	typedef SIMD_AVX<int16_t>			SIMD_16;
	typedef SIMD_AVX<uint16_t>			SIMD_U16;
	typedef SIMD_AVX<int32_t>			SIMD_32;
	typedef SIMD_AVX<uint32_t>			SIMD_U32;
	typedef SIMD_AVX<int64_t>			SIMD_64;
	typedef SIMD_AVX<uint64_t>			SIMD_U64;
	typedef SIMD_AVX<float>				SIMD_FP32;
	typedef SIMD_AVX<double>			SIMD_FP64;

	#define SIMD_FP32_BLEND(x, y, imm) SIMD_256_FP32_BLEND(x, y, imm)
	#define SIMD_FP32_BLEND_MASK(x, y, mask) SIMD_256_FP32_BLEND_MASK(x, y, mask)

	#else
	template<typename T>
	using 	SIMD_TEMPLATE=				SIMD_RAW<T>;
	typedef SIMD_RAW<char>				SIMD_8;
	typedef SIMD_RAW<unsigned char>		SIMD_U8;
	typedef SIMD_RAW<int16_t>			SIMD_16;
	typedef SIMD_RAW<uint16_t>			SIMD_U16;
	typedef SIMD_RAW<int32_t>			SIMD_32;
	typedef SIMD_RAW<uint32_t>			SIMD_U32;
	typedef SIMD_RAW<int64_t>			SIMD_64;
	typedef SIMD_RAW<uint64_t>			SIMD_U64;
	typedef SIMD_RAW<float>				SIMD_FP32;
	typedef SIMD_RAW<double>			SIMD_FP64;
	#endif
}