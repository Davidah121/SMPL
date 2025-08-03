#pragma once
#include <immintrin.h>
#include "SIMD_Template.h"
#include "Float16.h"


namespace smpl
{
	#ifdef __SSE4_2__

	#ifndef SIMD_128_FP32_BLEND_MASK
	#define SIMD_128_FP32_BLEND_MASK(x, y, imm) _mm_blendv_ps(x, y, imm)
	#endif

	#ifndef SIMD_128_FP64_BLEND_MASK
	#define SIMD_128_FP64_BLEND_MASK(x, y, imm) _mm_blendv_pd(x, y, imm)
	#endif
	template<>
	class SIMD_SSE<float>
	{
	public:
		static const int SIZE = 4;
		static const int SIZE_FP16 = 8;
		static unsigned long long getSIMDBound(unsigned long long s) {return (s>>2)<<2;}
		static unsigned long long getSIMDBoundFP16(unsigned long long s) {return (s>>3)<<3;}

		SIMD_SSE(){}
		SIMD_SSE(float byte){values = _mm_set1_ps(byte);}
		SIMD_SSE(__m128 byte){values = byte;}
		
		SIMD_SSE(const SIMD_SSE<float>& other) {values = other.values;}
		void operator=(const SIMD_SSE<float>& other) {values = other.values;}
		
		SIMD_SSE(const SIMD_SSE<float>&& other) noexcept {values = other.values;}
		void operator=(const SIMD_SSE<float>&& other) noexcept {values = other.values;}
		
		~SIMD_SSE(){}

		//load / store
		static std::pair<SIMD_SSE<float>, SIMD_SSE<float>> loadFP16(Float16* pointer)
		{
			__m128i asShorts = _mm_loadu_si128((__m128i*)pointer); //can read past the allocated area but that data will be dumped. should be acceptable but may cause issues with address sanitizers.
			__m128 v1 = _mm_cvtph_ps(asShorts);
			__m128 v2 = _mm_cvtph_ps(_mm_srli_si128(asShorts, 4));
			
			return {v1, v2};
		}
		static void storeAsFP16(Float16* pointer, const std::pair<SIMD_SSE<float>, SIMD_SSE<float>>& floatPair)
		{
			__m128i asShorts1 = _mm_cvtps_ph(floatPair.first.values, _MM_FROUND_TO_NEAREST_INT);
			__m128i asShorts2 = _mm_cvtps_ph(floatPair.second.values, _MM_FROUND_TO_NEAREST_INT);
			__m128i result = _mm_add_epi16(_mm_srli_si128(asShorts1, 8), asShorts2);

			_mm_storeu_si128((__m128i*)pointer, result);
		}

		static SIMD_SSE<float>load(float* pointer){return _mm_loadu_ps(pointer);}
		void store(float* pointer){_mm_storeu_ps(pointer, values);}
		
		//arithmetic
		SIMD_SSE<float> operator-() const {return SEML::negate(values);}
		SIMD_SSE<float> operator+(const SIMD_SSE<float>& other) const {return _mm_add_ps(values, other.values);}
		SIMD_SSE<float> operator-(const SIMD_SSE<float>& other) const {return _mm_sub_ps(values, other.values);}

		SIMD_SSE<float> operator*(const SIMD_SSE<float>& other) const {return _mm_mul_ps(values, other.values);}
		SIMD_SSE<float> operator/(const SIMD_SSE<float>& other) const {return _mm_div_ps(values, other.values);}
		
		void operator+=(const SIMD_SSE<float>& other) {values = operator+(other).values;}
		void operator-=(const SIMD_SSE<float>& other) {values = operator-(other).values;}
		void operator*=(const SIMD_SSE<float>& other) {values = operator*(other).values;}
		void operator/=(const SIMD_SSE<float>& other) {values = operator/(other).values;}
		
		//memory access and modification
		
		//bitwise
		
		SIMD_SSE<float> operator&(const float v) const {return _mm_and_ps(values, _mm_set1_ps(v));}
		SIMD_SSE<float> operator&(const SIMD_SSE<float>& other) const {return _mm_and_ps(values, other.values);}
		
		void operator&=(const float v) {values = operator&(v).values;}
		void operator&=(const SIMD_SSE<float>& other) {values = operator&(other).values;}
		
		SIMD_SSE<float> bitwiseAndNot(const float v) const {return _mm_andnot_ps(values, _mm_set1_ps(v));}
		SIMD_SSE<float> bitwiseAndNot(const SIMD_SSE<float>& other) const {return _mm_andnot_ps(values, other.values);}
		
		void bitwiseAndNot(const float v) {values = _mm_andnot_ps(values, _mm_set1_ps(v));}
		void bitwiseAndNot(const SIMD_SSE<float>& other) {values = _mm_andnot_ps(values, other.values);}
		
		//comparison
		SIMD_SSE<float> operator>(const float byte) const {return _mm_cmpgt_ps(values, _mm_set1_ps(byte));}
		SIMD_SSE<float> operator>(const SIMD_SSE<float>& other) const {return _mm_cmpgt_ps(values, other.values);}
		
		SIMD_SSE<float> operator<(const float byte) const {return _mm_cmplt_ps(values, _mm_set1_ps(byte));}
		SIMD_SSE<float> operator<(const SIMD_SSE<float>& other) const {return _mm_cmplt_ps(values, other.values);}
		
		SIMD_SSE<float> operator==(const float byte) const {return _mm_cmpeq_ps(values, _mm_set1_ps(byte));}
		SIMD_SSE<float> operator==(const SIMD_SSE<float>& other) const {return _mm_cmpeq_ps(values, other.values);}
		
		SIMD_SSE<float> operator!=(const float byte) const {return _mm_cmpneq_ps(values, _mm_set1_ps(byte));}
		SIMD_SSE<float> operator!=(const SIMD_SSE<float>& other) const {return _mm_cmpneq_ps(values, other.values);}
		
		//special case functions
		SIMD_SSE<float> horizontalAdd(const SIMD_SSE<float>& other) const 
		{
			return _mm_hadd_ps(values, other.values);
		}
		float sum() const
		{
			//https://stackoverflow.com/questions/6996764/fastest-way-to-do-horizontal-sse-vector-sum-or-other-reduction
			//faster than 2 hadd_ps. has a throughput faster by 2 than using hadd_ps
			__m128 shuf = _mm_movehdup_ps(values);
			__m128 sums = _mm_add_ps(values, shuf);
			shuf = _mm_movehl_ps(shuf, sums);
			sums = _mm_add_ps(sums, shuf);
			return _mm_cvtss_f32(sums);
		}

		__m128 values;
	};

	template<>
	class SIMD_SSE<double>
	{
	public:
		static const int SIZE = 2;
		static unsigned long long getSIMDBound(unsigned long long s) {return (s>>1)<<1;}

		SIMD_SSE(){}
		SIMD_SSE(double byte){values = _mm_set1_pd(byte);}
		SIMD_SSE(__m128d byte){values = byte;}
		
		SIMD_SSE(const SIMD_SSE<double>& other) {values = other.values;}
		void operator=(const SIMD_SSE<double>& other) {values = other.values;}
		
		SIMD_SSE(const SIMD_SSE<double>&& other) noexcept {values = other.values;}
		void operator=(const SIMD_SSE<double>&& other) noexcept {values = other.values;}
		
		~SIMD_SSE(){}

		//load / store
		static SIMD_SSE<double>load(double* pointer){return _mm_loadu_pd(pointer);}
		void store(double* pointer){_mm_storeu_pd(pointer, values);}
		
		//arithmetic
		SIMD_SSE<double> operator-() const {return SEML::negate(values);}
		SIMD_SSE<double> operator+(const SIMD_SSE<double>& other) const {return _mm_add_pd(values, other.values);}
		SIMD_SSE<double> operator-(const SIMD_SSE<double>& other) const {return _mm_sub_pd(values, other.values);}

		SIMD_SSE<double> operator*(const SIMD_SSE<double>& other) const {return _mm_mul_pd(values, other.values);}
		SIMD_SSE<double> operator/(const SIMD_SSE<double>& other) const {return _mm_div_pd(values, other.values);}
		
		void operator+=(const SIMD_SSE<double>& other) {values = operator+(other).values;}
		void operator-=(const SIMD_SSE<double>& other) {values = operator-(other).values;}
		void operator*=(const SIMD_SSE<double>& other) {values = operator*(other).values;}
		void operator/=(const SIMD_SSE<double>& other) {values = operator/(other).values;}
		
		//memory access and modification
		
		//bitwise
		
		SIMD_SSE<double> operator&(const double v) const {return _mm_and_pd(values, _mm_set1_pd(v));}
		SIMD_SSE<double> operator&(const SIMD_SSE<double>& other) const {return _mm_and_pd(values, other.values);}
		
		void operator&=(const double v) {values = operator&(v).values;}
		void operator&=(const SIMD_SSE<double>& other) {values = operator&(other).values;}
		
		SIMD_SSE<double> bitwiseAndNot(const double v) const {return _mm_andnot_pd(values, _mm_set1_pd(v));}
		SIMD_SSE<double> bitwiseAndNot(const SIMD_SSE<double>& other) const {return _mm_andnot_pd(values, other.values);}
		
		void bitwiseAndNot(const double v) {values = _mm_andnot_pd(values, _mm_set1_pd(v));}
		void bitwiseAndNot(const SIMD_SSE<double>& other) {values = _mm_andnot_pd(values, other.values);}
		
		//comparison
		SIMD_SSE<double> operator>(const double byte) const {return _mm_cmpgt_pd(values, _mm_set1_pd(byte));}
		SIMD_SSE<double> operator>(const SIMD_SSE<double>& other) const {return _mm_cmpgt_pd(values, other.values);}
		
		SIMD_SSE<double> operator<(const double byte) const {return _mm_cmplt_pd(values, _mm_set1_pd(byte));}
		SIMD_SSE<double> operator<(const SIMD_SSE<double>& other) const {return _mm_cmplt_pd(values, other.values);}
		
		SIMD_SSE<double> operator==(const double byte) const {return _mm_cmpeq_pd(values, _mm_set1_pd(byte));}
		SIMD_SSE<double> operator==(const SIMD_SSE<double>& other) const {return _mm_cmpeq_pd(values, other.values);}
		
		SIMD_SSE<double> operator!=(const double byte) const {return _mm_cmpneq_pd(values, _mm_set1_pd(byte));}
		SIMD_SSE<double> operator!=(const SIMD_SSE<double>& other) const {return _mm_cmpneq_pd(values, other.values);}
		
		//special case functions
		SIMD_SSE<double> horizontalAdd(const SIMD_SSE<double>& other) const 
		{
			return _mm_hadd_pd(values, other.values);
		}
		double sum() const
		{
			__m128d temp = _mm_hadd_pd(values, values); //contains A1+B1, A1+B1
			return _mm_cvtsd_f64(temp);
		}

		__m128d values;
	};

	#endif


	#ifdef __AVX2__

	#ifndef SIMD_256_FP32_BLEND_MASK
	#define SIMD_256_FP32_BLEND_MASK(x, y, imm) _mm256_blendv_ps(x, y, imm)
	#endif

	#ifndef SIMD_256_FP64_BLEND_MASK
	#define SIMD_256_FP64_BLEND_MASK(x, y, imm) _mm256_blendv_pd(x, y, imm)
	#endif
	template<>
	class SIMD_AVX<float>
	{
	public:
		static const int SIZE = 8;
		static const int SIZE_FP16 = 16;
		static unsigned long long getSIMDBound(unsigned long long s) {return (s>>3)<<3;}
		static unsigned long long getSIMDBoundFP16(unsigned long long s) {return (s>>4)<<4;}

		SIMD_AVX(){}
		SIMD_AVX(float byte){values = _mm256_set1_ps(byte);}
		SIMD_AVX(__m256 byte){values = byte;}
		
		SIMD_AVX(const SIMD_AVX<float>& other) {values = other.values;}
		void operator=(const SIMD_AVX<float>& other) {values = other.values;}
		
		SIMD_AVX(const SIMD_AVX<float>&& other) noexcept {values = other.values;}
		void operator=(const SIMD_AVX<float>&& other) noexcept {values = other.values;}
		
		~SIMD_AVX(){}

		//load / store
		static SIMD_AVX<float> loadFP16(Float16* pointer)
		{
			__m256i asShorts = _mm256_loadu_si256((__m256i*)pointer); //can read past the allocated area but that data will be dumped. should be acceptable but may cause issues with address sanitizers.
			return _mm256_cvtph_ps(_mm256_extracti128_si256(asShorts, 0));
		}
		static void storeAsFP16(Float16* pointer, const SIMD_AVX<float>& v1, const SIMD_AVX<float>& v2)
		{
			__m128i asShorts1 = _mm256_cvtps_ph(v1.values, _MM_FROUND_TO_NEAREST_INT);
			__m128i asShorts2 = _mm256_cvtps_ph(v2.values, _MM_FROUND_TO_NEAREST_INT);
			__m256i result = _mm256_set_m128i(asShorts1, asShorts2);

			_mm256_storeu_si256((__m256i*)pointer, result);
		}
		
		static SIMD_AVX<float>load(float* pointer){return _mm256_loadu_ps(pointer);}
		void store(float* pointer){_mm256_storeu_ps(pointer, values);}
		
		//arithmetic
		SIMD_AVX<float> operator-() const {return SEML::negate(values);}
		SIMD_AVX<float> operator+(const SIMD_AVX<float>& other) const {return _mm256_add_ps(values, other.values);}
		SIMD_AVX<float> operator-(const SIMD_AVX<float>& other) const {return _mm256_sub_ps(values, other.values);}

		SIMD_AVX<float> operator*(const SIMD_AVX<float>& other) const {return _mm256_mul_ps(values, other.values);}
		SIMD_AVX<float> operator/(const SIMD_AVX<float>& other) const {return _mm256_div_ps(values, other.values);}
		
		void operator+=(const SIMD_AVX<float>& other) {values = operator+(other).values;}
		void operator-=(const SIMD_AVX<float>& other) {values = operator-(other).values;}
		void operator*=(const SIMD_AVX<float>& other) {values = operator*(other).values;}
		void operator/=(const SIMD_AVX<float>& other) {values = operator/(other).values;}
		
		//memory access and modification
		
		//bitwise
		
		SIMD_AVX<float> operator&(const float v) const {return _mm256_and_ps(values, _mm256_set1_ps(v));}
		SIMD_AVX<float> operator&(const SIMD_AVX<float>& other) const {return _mm256_and_ps(values, other.values);}
		
		void operator&=(const float v) {values = operator&(v).values;}
		void operator&=(const SIMD_AVX<float>& other) {values = operator&(other).values;}
		
		SIMD_AVX<float> bitwiseAndNot(const float v) const {return _mm256_andnot_ps(values, _mm256_set1_ps(v));}
		SIMD_AVX<float> bitwiseAndNot(const SIMD_AVX<float>& other) const {return _mm256_andnot_ps(values, other.values);}
		
		void bitwiseAndNot(const float v) {values = _mm256_andnot_ps(values, _mm256_set1_ps(v));}
		void bitwiseAndNot(const SIMD_AVX<float>& other) {values = _mm256_andnot_ps(values, other.values);}
		
		//comparison
		SIMD_AVX<float> operator>(const float byte) const {return _mm256_cmp_ps(values, _mm256_set1_ps(byte), _CMP_GT_OQ);}
		SIMD_AVX<float> operator>(const SIMD_AVX<float>& other) const {return _mm256_cmp_ps(values, other.values, _CMP_GT_OQ);}
		
		SIMD_AVX<float> operator<(const float byte) const {return _mm256_cmp_ps(values, _mm256_set1_ps(byte), _CMP_LT_OQ);}
		SIMD_AVX<float> operator<(const SIMD_AVX<float>& other) const {return _mm256_cmp_ps(values, other.values, _CMP_LT_OQ);}
		
		SIMD_AVX<float> operator==(const float byte) const {return _mm256_cmp_ps(values, _mm256_set1_ps(byte), _CMP_EQ_OQ);}
		SIMD_AVX<float> operator==(const SIMD_AVX<float>& other) const {return _mm256_cmp_ps(values, other.values, _CMP_EQ_OQ);}
		
		SIMD_AVX<float> operator!=(const float byte) const {return _mm256_cmp_ps(values, _mm256_set1_ps(byte), _CMP_NEQ_OQ);}
		SIMD_AVX<float> operator!=(const SIMD_AVX<float>& other) const {return _mm256_cmp_ps(values, other.values, _CMP_NEQ_OQ);}
		
		//special case functions
		SIMD_AVX<float> horizontalAdd(const SIMD_AVX<float>& other) const 
		{
			return _mm256_hadd_ps(values, other.values);
		}
		float sum() const
		{
			//https://stackoverflow.com/questions/6996764/fastest-way-to-do-horizontal-sse-vector-sum-or-other-reduction
			//faster than 2 hadd_ps. has a throughput faster by 2 than using hadd_ps
			
			//TODO: Test
			__m256 shuf = _mm256_movehdup_ps(values);
			__m256 sums = _mm256_add_ps(values, shuf);
			shuf = _mm256_shuffle_ps(sums, shuf, 0b11101110);
			sums = _mm256_add_ps(sums, shuf);
			shuf = _mm256_shuffle_ps(sums, shuf, 0b11101110);
			sums = _mm256_add_ps(sums, shuf);
			return _mm256_cvtss_f32(sums);
		}

		__m256 values;
	};

	template<>
	class SIMD_AVX<double>
	{
	public:
		static const int SIZE = 2;
		static unsigned long long getSIMDBound(unsigned long long s) {return (s>>1)<<1;}

		SIMD_AVX(){}
		SIMD_AVX(double byte){values = _mm256_set1_pd(byte);}
		SIMD_AVX(__m256d byte){values = byte;}
		
		SIMD_AVX(const SIMD_AVX<double>& other) {values = other.values;}
		void operator=(const SIMD_AVX<double>& other) {values = other.values;}
		
		SIMD_AVX(const SIMD_AVX<double>&& other) noexcept {values = other.values;}
		void operator=(const SIMD_AVX<double>&& other) noexcept {values = other.values;}
		
		~SIMD_AVX(){}

		//load / store
		static SIMD_AVX<double>load(double* pointer){return _mm256_loadu_pd(pointer);}
		void store(double* pointer){_mm256_storeu_pd(pointer, values);}
		
		//arithmetic
		SIMD_AVX<double> operator-() const {return SEML::negate(values);}
		SIMD_AVX<double> operator+(const SIMD_AVX<double>& other) const {return _mm256_add_pd(values, other.values);}
		SIMD_AVX<double> operator-(const SIMD_AVX<double>& other) const {return _mm256_sub_pd(values, other.values);}

		SIMD_AVX<double> operator*(const SIMD_AVX<double>& other) const {return _mm256_mul_pd(values, other.values);}
		SIMD_AVX<double> operator/(const SIMD_AVX<double>& other) const {return _mm256_div_pd(values, other.values);}
		
		void operator+=(const SIMD_AVX<double>& other) {values = operator+(other).values;}
		void operator-=(const SIMD_AVX<double>& other) {values = operator-(other).values;}
		void operator*=(const SIMD_AVX<double>& other) {values = operator*(other).values;}
		void operator/=(const SIMD_AVX<double>& other) {values = operator/(other).values;}
		
		//memory access and modification
		
		//bitwise
		
		SIMD_AVX<double> operator&(const double v) const {return _mm256_and_pd(values, _mm256_set1_pd(v));}
		SIMD_AVX<double> operator&(const SIMD_AVX<double>& other) const {return _mm256_and_pd(values, other.values);}
		
		void operator&=(const double v) {values = operator&(v).values;}
		void operator&=(const SIMD_AVX<double>& other) {values = operator&(other).values;}
		
		SIMD_AVX<double> bitwiseAndNot(const double v) const {return _mm256_andnot_pd(values, _mm256_set1_pd(v));}
		SIMD_AVX<double> bitwiseAndNot(const SIMD_AVX<double>& other) const {return _mm256_andnot_pd(values, other.values);}
		
		void bitwiseAndNot(const double v) {values = _mm256_andnot_pd(values, _mm256_set1_pd(v));}
		void bitwiseAndNot(const SIMD_AVX<double>& other) {values = _mm256_andnot_pd(values, other.values);}
		
		//comparison
		SIMD_AVX<double> operator>(const double byte) const {return _mm256_cmp_pd(values, _mm256_set1_pd(byte), _CMP_GT_OQ);}
		SIMD_AVX<double> operator>(const SIMD_AVX<double>& other) const {return _mm256_cmp_pd(values, other.values, _CMP_GT_OQ);}
		
		SIMD_AVX<double> operator<(const double byte) const {return _mm256_cmp_pd(values, _mm256_set1_pd(byte), _CMP_LT_OQ);}
		SIMD_AVX<double> operator<(const SIMD_AVX<double>& other) const {return _mm256_cmp_pd(values, other.values, _CMP_LT_OQ);}
		
		SIMD_AVX<double> operator==(const double byte) const {return _mm256_cmp_pd(values, _mm256_set1_pd(byte), _CMP_EQ_OQ);}
		SIMD_AVX<double> operator==(const SIMD_AVX<double>& other) const {return _mm256_cmp_pd(values, other.values, _CMP_EQ_OQ);}
		
		SIMD_AVX<double> operator!=(const double byte) const {return _mm256_cmp_pd(values, _mm256_set1_pd(byte), _CMP_NEQ_OQ);}
		SIMD_AVX<double> operator!=(const SIMD_AVX<double>& other) const {return _mm256_cmp_pd(values, other.values, _CMP_NEQ_OQ);}
		
		//special case functions
		SIMD_AVX<double> horizontalAdd(const SIMD_AVX<double>& other) const 
		{
			return _mm256_hadd_pd(values, other.values);
		}
		double sum() const
		{
			__m256d temp = _mm256_hadd_pd(values, values); //contains A1+B1, A1+B1
			return _mm256_cvtsd_f64(temp);
		}

		__m256d values;
	};
	#endif
}