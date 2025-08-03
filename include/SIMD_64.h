#pragma once
#include <immintrin.h>
#include "SIMD_Template.h"

namespace smpl
{
	#ifdef __SSE4_2__
	template<>
	class SIMD_SSE<int64_t>
	{
	public:
		static const int SIZE = 2;
		static uint64_t getSIMDBound(uint64_t s) {return (s>>1)<<1;}

		SIMD_SSE(){}
		SIMD_SSE(int64_t byte){values = _mm_set1_epi64x(byte);}
		SIMD_SSE(__m128i byte){values = byte;}
		
		SIMD_SSE(const SIMD_SSE<int64_t>& other) {values = other.values;}
		void operator=(const SIMD_SSE<int64_t>& other) {values = other.values;}
		
		SIMD_SSE(const SIMD_SSE<int64_t>&& other) noexcept {values = other.values;}
		void operator=(const SIMD_SSE<int64_t>&& other) noexcept {values = other.values;}
		
		~SIMD_SSE(){}

		//load / store
		static SIMD_SSE<int64_t>load(int64_t* pointer){return _mm_loadu_si128((__m128i*)pointer);}
		void store(int64_t* pointer){_mm_storeu_si128((__m128i*)pointer, values);}
		
		//arithmetic
		SIMD_SSE<int64_t> operator-() const {return _mm_sub_epi64(_mm_set1_epi64x(0), values);}
		SIMD_SSE<int64_t> operator+(const SIMD_SSE<int64_t>& other) const {return _mm_add_epi64(values, other.values);}
		SIMD_SSE<int64_t> operator-(const SIMD_SSE<int64_t>& other) const {return _mm_sub_epi64(values, other.values);}
		SIMD_SSE<int64_t> operator*(const SIMD_SSE<int64_t>& other) const 
		{
			//latency added together = 21 cycles
			__m128i A = _mm_slli_epi64(_mm_mul_epi32(_mm_srli_epi64(values, 32), other.values), 32); //(A1*B2)<<32
			__m128i B = _mm_slli_epi64(_mm_mul_epi32(_mm_srli_epi64(other.values, 32), values), 32); //(A2*B1)<<32
			__m128i C = _mm_mul_epu32(values, other.values); //A2*B2 as 64 bit.
			
			return _mm_add_epi64(A, _mm_add_epi64(B, C));
		}

		SIMD_SSE<int64_t> operator/(const SIMD_SSE<int64_t>& other) const
		{
			//Just cast to float -> divide -> cast back
			__m128d A1 = SEML::fastInt64ToDouble(values);
			__m128d B1 = SEML::fastInt64ToDouble(other.values);
			__m128d res1 = _mm_div_pd(A1, B1);
			return SEML::fastDoubleToInt64(res1);
		}
		
		void operator+=(const SIMD_SSE<int64_t>& other) {values = operator+(other).values;}
		void operator-=(const SIMD_SSE<int64_t>& other) {values = operator-(other).values;}
		void operator*=(const SIMD_SSE<int64_t>& other) {values = operator*(other).values;}
		void operator/=(const SIMD_SSE<int64_t>& other) {values = operator/(other).values;}
		
		//memory access and modification
		
		//bitwise
		SIMD_SSE<int64_t> operator>>(const int64_t shift) const {return _mm_srli_epi64(values, shift);}
		SIMD_SSE<int64_t> operator<<(const int64_t shift) const {return _mm_slli_epi64(values, shift);}
		
		void operator>>=(const int64_t shift) {values = operator>>(shift).values;}
		void operator<<=(const int64_t shift) {values = operator<<(shift).values;}
		
		SIMD_SSE<int64_t> operator&(const int64_t v) const {return _mm_and_si128(values, _mm_set1_epi64x(v));}
		SIMD_SSE<int64_t> operator&(const SIMD_SSE<int64_t>& other) const {return _mm_and_si128(values, other.values);}
		
		void operator&=(const int64_t v) {values = operator&(v).values;}
		void operator&=(const SIMD_SSE<int64_t>& other) {values = operator&(other).values;}
		
		SIMD_SSE<int64_t> bitwiseAndNot(const int64_t v) const {return _mm_andnot_si128(values, _mm_set1_epi64x(v));}
		SIMD_SSE<int64_t> bitwiseAndNot(const SIMD_SSE<int64_t>& other) const {return _mm_andnot_si128(values, other.values);}
		
		void bitwiseAndNot(const int64_t v) {values = _mm_andnot_si128(values, _mm_set1_epi64x(v));}
		void bitwiseAndNot(const SIMD_SSE<int64_t>& other) {values = _mm_andnot_si128(values, other.values);}
		
		//comparison
		SIMD_SSE<int64_t> operator>(const uint64_t byte) const {return _mm_cmpgt_epi64(values, _mm_set1_epi64x(byte));}
		SIMD_SSE<int64_t> operator>(const SIMD_SSE<int64_t>& other) const {return _mm_cmpgt_epi64(values, other.values);}
		
		SIMD_SSE<int64_t> operator<(const uint64_t byte) const {return _mm_cmpgt_epi64(_mm_set1_epi64x(byte), values);}
		SIMD_SSE<int64_t> operator<(const SIMD_SSE<int64_t>& other) const {return _mm_cmpgt_epi64(other.values, values);}
		
		SIMD_SSE<int64_t> operator==(const uint64_t byte) const {return _mm_cmpeq_epi64(values, _mm_set1_epi64x(byte));}
		SIMD_SSE<int64_t> operator==(const SIMD_SSE<int64_t>& other) const {return _mm_cmpeq_epi64(values, other.values);}
		
		SIMD_SSE<int64_t> operator!=(const uint64_t byte) const 
		{
			__m128i temp = _mm_cmpeq_epi64(values, _mm_set1_epi64x(byte));
			return _mm_andnot_si128(temp, temp); //does not bitwise not
		}
		SIMD_SSE<int64_t> operator!=(const SIMD_SSE<int64_t>& other) const
		{
			__m128i temp = _mm_cmpeq_epi64(values, other.values);
			return _mm_andnot_si128(temp, temp); //does not bitwise not
		}
		
		//special case functions
		SIMD_SSE<int64_t> horizontalAdd(const SIMD_SSE<int64_t>& other) const 
		{
			return _mm_set_epi64x(sum(), other.sum()); //only 2 elements so the sum is the same as hadd
		}
		int64_t sum() const
		{
			//sum of all items into the largest datatype NEEDED to avoid overflow.
			//overflow is possible as 64 bit is largest size possible.
			long long temp[2];
			_mm_storeu_si128((__m128i*)temp, values);
			return temp[0] + temp[1];
		}

		__m128i values;
	};

	template<>
	class SIMD_SSE<uint64_t> : public SIMD_SSE<int64_t>
	{
	public:
		SIMD_SSE(){}
		SIMD_SSE(uint64_t byte){values = _mm_set1_epi64x(byte);}
		SIMD_SSE(__m128i byte){values = byte;}
		
		SIMD_SSE(const SIMD_SSE<uint64_t>& other) {values = other.values;}
		void operator=(const SIMD_SSE<uint64_t>& other) {values = other.values;}
		
		SIMD_SSE(const SIMD_SSE<uint64_t>&& other) noexcept {values = other.values;}
		void operator=(const SIMD_SSE<uint64_t>&& other) noexcept {values = other.values;}
		
		~SIMD_SSE(){}

		SIMD_SSE<uint64_t> operator*(const SIMD_SSE<uint64_t>& other) const
		{
			//break into 4 values.
				//A = A1<<32 + A2
				//B = B1<<32 + B2
				//A*B = (A1*B1)<<64 + (A1*B2)<<32 + (A2*B1)<<32 + A2*B2
				//drop upper multiplication.
			//latency added together = 21 cycles
			__m128i A = _mm_slli_epi64(_mm_mul_epu32(_mm_srli_epi64(values, 32), other.values), 32); //(A1*B2)<<32
			__m128i B = _mm_slli_epi64(_mm_mul_epu32(_mm_srli_epi64(other.values, 32), values), 32); //(A2*B1)<<32
			__m128i C = _mm_mul_epu32(values, other.values); //A2*B2 as 64 bit.
			
			return _mm_add_epi64(A, _mm_add_epi64(B, C));
		}

		SIMD_SSE<uint64_t> operator/(const SIMD_SSE<uint64_t>& other) const
		{
			__m128d A1 = SEML::uint64ToDouble(values);
			__m128d B1 = SEML::uint64ToDouble(other.values);
			__m128d res1 = _mm_div_pd(A1, B1);
			return SEML::fastDoubleToUInt64(res1);
		}
		
		SIMD_SSE<uint64_t> operator>(const uint64_t byte) const
		{
			__m128i temp = _mm_set1_epi64x(byte);
			return _mm_cmpeq_epi64(values, _mm_max_epu64_emulated(values, temp));
		}
		SIMD_SSE<uint64_t> operator>(const SIMD_SSE<uint64_t>& other) const
		{
			return _mm_cmpeq_epi64(values, _mm_max_epu64_emulated(values, other.values));
		}
		
		SIMD_SSE<uint64_t> operator<(const uint64_t byte) const
		{
			__m128i temp = _mm_set1_epi64x(byte);
			return _mm_cmpneq_epi64_emulated(values, _mm_max_epu64_emulated(values, temp));
		}
		SIMD_SSE<uint64_t> operator<(const SIMD_SSE<uint64_t>& other) const
		{
			return _mm_cmpneq_epi64_emulated(values, _mm_max_epu64_emulated(values, other.values));
		}

		uint64_t sum() const
		{
			//sum of all items into the largest datatype NEEDED to avoid overflow.
			//overflow is possible as 64 bit is largest size possible.
			uint64_t temp[2];
			_mm_storeu_si128((__m128i*)temp, values);
			return temp[0] + temp[1];
		}

		//load / store
		static SIMD_SSE<uint64_t>load(uint64_t* pointer){return _mm_loadu_si128((__m128i*)pointer);}
		void store(uint64_t* pointer){_mm_storeu_si128((__m128i*)pointer, values);}

	private:
		//comparison
		__m128i _mm_cmpgt_epu64_emulated(__m128i v1, __m128i v2) const
		{
			return _mm_cmpgt_epi64(_mm_sub_epi64(v1, v2), _mm_set1_epi64x(0));
		}

		__m128i _mm_max_epu64_emulated(__m128i v1, __m128i v2) const
		{
			__m128i blendedValue = _mm_cmpgt_epu64_emulated(v1, v2);
			__m128d resultAsPD = _mm_blendv_pd(_mm_castsi128_pd(v1) ,_mm_castsi128_pd(v2), _mm_castsi128_pd(blendedValue));
			return _mm_castpd_si128(resultAsPD);
		}
		__m128i _mm_cmpneq_epi64_emulated(__m128i v1, __m128i v2) const
		{
			__m128i temp = _mm_cmpeq_epi64(v1, v2);
			return _mm_andnot_si128(temp, temp);
		}
	};

	#endif


	#ifdef __AVX2__
	template<>
	class SIMD_AVX<int64_t>
	{
	public:
		static const int SIZE = 4;
		static uint64_t getSIMDBound(uint64_t s) {return (s>>2)<<2;}

		SIMD_AVX(){}
		SIMD_AVX(int64_t byte){values = _mm256_set1_epi64x(byte);}
		SIMD_AVX(__m256i byte){values = byte;}
		
		SIMD_AVX(const SIMD_AVX<int64_t>& other) {values = other.values;}
		void operator=(const SIMD_AVX<int64_t>& other) {values = other.values;}
		
		SIMD_AVX(const SIMD_AVX<int64_t>&& other) noexcept {values = other.values;}
		void operator=(const SIMD_AVX<int64_t>&& other) noexcept {values = other.values;}
		
		~SIMD_AVX(){}

		//load / store
		static SIMD_AVX<int64_t>load(int64_t* pointer){return _mm256_loadu_si256((__m256i*)pointer);}
		void store(int64_t* pointer){_mm256_storeu_si256((__m256i*)pointer, values);}
		
		//arithmetic
		SIMD_AVX<int64_t> operator-() const {return _mm256_sub_epi64(_mm256_set1_epi64x(0), values);}
		SIMD_AVX<int64_t> operator+(const SIMD_AVX<int64_t>& other) const {return _mm256_add_epi64(values, other.values);}
		SIMD_AVX<int64_t> operator-(const SIMD_AVX<int64_t>& other) const {return _mm256_sub_epi64(values, other.values);}

		SIMD_AVX<int64_t> operator*(const SIMD_AVX<int64_t>& other) const 
		{
			//latency added together = 21 cycles
			__m256i A = _mm256_slli_epi64(_mm256_mul_epi32(_mm256_srli_epi64(values, 32), other.values), 32); //(A1*B2)<<32
			__m256i B = _mm256_slli_epi64(_mm256_mul_epi32(_mm256_srli_epi64(other.values, 32), values), 32); //(A2*B1)<<32
			__m256i C = _mm256_mul_epu32(values, other.values); //A2*B2 as 64 bit.
			
			return _mm256_add_epi64(A, _mm256_add_epi64(B, C));
		}
		SIMD_AVX<int64_t> operator/(const SIMD_AVX<int64_t>& other) const
		{
			//doesn't really exist here.
			//Just cast to double -> divide -> cast back
			__m256d A1 = SEML::fastUInt64ToDouble(values);
			__m256d B1 = SEML::fastUInt64ToDouble(other.values);
			__m256d res1 = _mm256_div_pd(A1, B1);
			return SEML::fastDoubleToInt64(res1);
		}
		
		void operator+=(const SIMD_AVX<int64_t>& other) {values = operator+(other).values;}
		void operator-=(const SIMD_AVX<int64_t>& other) {values = operator-(other).values;}
		void operator*=(const SIMD_AVX<int64_t>& other) {values = operator*(other).values;}
		void operator/=(const SIMD_AVX<int64_t>& other) {values = operator/(other).values;}
		
		//memory access and modification
		
		//bitwise
		SIMD_AVX<int64_t> operator>>(const int64_t shift) const {return _mm256_srli_epi64(values, shift);}
		SIMD_AVX<int64_t> operator<<(const int64_t shift) const {return _mm256_slli_epi64(values, shift);}
		
		void operator>>=(const int64_t shift) {values = operator>>(shift).values;}
		void operator<<=(const int64_t shift) {values = operator<<(shift).values;}
		
		SIMD_AVX<int64_t> operator&(const int64_t v) const {return _mm256_and_si256(values, _mm256_set1_epi64x(v));}
		SIMD_AVX<int64_t> operator&(const SIMD_AVX<int64_t>& other) const {return _mm256_and_si256(values, other.values);}
		
		void operator&=(const int64_t v) {values = operator&(v).values;}
		void operator&=(const SIMD_AVX<int64_t>& other) {values = operator&(other).values;}
		
		SIMD_AVX<int64_t> bitwiseAndNot(const int64_t v) const {return _mm256_andnot_si256(values, _mm256_set1_epi64x(v));}
		SIMD_AVX<int64_t> bitwiseAndNot(const SIMD_AVX<int64_t>& other) const {return _mm256_andnot_si256(values, other.values);}
		
		void bitwiseAndNot(const int64_t v) {values = _mm256_andnot_si256(values, _mm256_set1_epi64x(v));}
		void bitwiseAndNot(const SIMD_AVX<int64_t>& other) {values = _mm256_andnot_si256(values, other.values);}
		
		//comparison
		SIMD_AVX<int64_t> operator>(const uint64_t byte) const {return _mm256_cmpgt_epi64(values, _mm256_set1_epi64x(byte));}
		SIMD_AVX<int64_t> operator>(const SIMD_AVX<int64_t>& other) const {return _mm256_cmpgt_epi64(values, other.values);}
		
		SIMD_AVX<int64_t> operator<(const uint64_t byte) const {return _mm256_cmpgt_epi64(_mm256_set1_epi64x(byte), values);}
		SIMD_AVX<int64_t> operator<(const SIMD_AVX<int64_t>& other) const {return _mm256_cmpgt_epi64(other.values, values);}
		
		SIMD_AVX<int64_t> operator==(const uint64_t byte) const {return _mm256_cmpeq_epi64(values, _mm256_set1_epi64x(byte));}
		SIMD_AVX<int64_t> operator==(const SIMD_AVX<int64_t>& other) const {return _mm256_cmpeq_epi64(values, other.values);}
		
		SIMD_AVX<int64_t> operator!=(const uint64_t byte) const 
		{
			__m256i temp = _mm256_cmpeq_epi64(values, _mm256_set1_epi64x(byte));
			return _mm256_andnot_si256(temp, temp); //does not bitwise not
		}
		SIMD_AVX<int64_t> operator!=(const SIMD_AVX<int64_t>& other) const
		{
			__m256i temp = _mm256_cmpeq_epi64(values, other.values);
			return _mm256_andnot_si256(temp, temp); //does not bitwise not
		}
		
		//special case functions
		SIMD_AVX<int64_t> horizontalAdd(const SIMD_AVX<int64_t>& other) const 
		{
			//sum over A1, sum over A2, sum over B1, sum over B2
			//A, B, C, D | E, F, G, H
			//A+B, C+D, E+F, G+H
			//A+B, B+A, C+D, D+C

			__m256i blendedA = _mm256_castpd_si256(_mm256_shuffle_pd(_mm256_castsi256_pd(values), _mm256_castsi256_pd(values), 0b0101)); //B,A,D,C
			__m256i A = _mm256_add_epi64(values, blendedA); //A+B, B+A, C+D, D+C

			__m256i blendedB = _mm256_castpd_si256(_mm256_shuffle_pd(_mm256_castsi256_pd(other.values), _mm256_castsi256_pd(other.values), 0b0101));
			__m256i B = _mm256_add_epi64(other.values, blendedB);

			A = _mm256_permute4x64_epi64(A, 0b10001000); //A+B, C+D, A+B, C+D
			B = _mm256_permute4x64_epi64(B, 0b10001000); //E+F, G+H, E+F, G+H

			return _mm256_castpd_si256(_mm256_blend_pd(_mm256_castsi256_pd(A), _mm256_castsi256_pd(B), 0b1100));
		}
		int64_t sum() const
		{
			//sum of all items into the largest datatype NEEDED to avoid overflow.
			//overflow is possible as 64 bit is largest size possible.
			long long temp[4];
			__m256i blendedA = _mm256_castpd_si256(_mm256_shuffle_pd(_mm256_castsi256_pd(values), _mm256_castsi256_pd(values), 0b0101)); //B,A,D,C
			__m256i A = _mm256_add_epi64(values, blendedA); //A+B, B+A, C+D, D+C
			_mm256_storeu_si256((__m256i*)temp, values);

			return temp[0] + temp[2];
		}

		__m256i values;

	private:
	};

	template<>
	class SIMD_AVX<uint64_t> : public SIMD_AVX<int64_t>
	{
	public:
		SIMD_AVX(){}
		SIMD_AVX(uint64_t byte){values = _mm256_set1_epi64x(byte);}
		SIMD_AVX(__m256i byte){values = byte;}
		
		SIMD_AVX(const SIMD_AVX<uint64_t>& other) {values = other.values;}
		void operator=(const SIMD_AVX<uint64_t>& other) {values = other.values;}
		
		SIMD_AVX(const SIMD_AVX<uint64_t>&& other) noexcept {values = other.values;}
		void operator=(const SIMD_AVX<uint64_t>&& other) noexcept {values = other.values;}
		
		~SIMD_AVX(){}

		SIMD_AVX<uint64_t> operator*(const SIMD_AVX<uint64_t>& other) const
		{
			__m256i A = _mm256_slli_epi64(_mm256_mul_epu32(_mm256_srli_epi64(values, 32), other.values), 32); //(A1*B2)<<32
			__m256i B = _mm256_slli_epi64(_mm256_mul_epu32(_mm256_srli_epi64(other.values, 32), values), 32); //(A2*B1)<<32
			__m256i C = _mm256_mul_epu32(values, other.values); //A2*B2 as 64 bit.
			
			return _mm256_add_epi64(A, _mm256_add_epi64(B, C));
		}

		SIMD_AVX<uint64_t> operator/(const SIMD_AVX<uint64_t>& other) const
		{
			return 0; //TODO: FIX LATER
		}
		
		SIMD_AVX<uint64_t> operator>(const uint64_t byte) const
		{
			return _mm256_cmpgt_epi64(values, _mm256_set1_epi64x(byte));
		}
		SIMD_AVX<uint64_t> operator>(const SIMD_AVX<uint64_t>& other) const
		{
			return _mm256_cmpgt_epi64(values, other.values);
		}
		
		SIMD_AVX<uint64_t> operator<(const uint64_t byte) const
		{
			return _mm256_cmpgt_epi64(_mm256_set1_epi64x(byte), values);
		}
		SIMD_AVX<uint64_t> operator<(const SIMD_AVX<uint64_t>& other) const
		{
			return _mm256_cmpgt_epi64(other.values, values);
		}

		uint64_t sum() const
		{
			//sum of all items into the largest datatype NEEDED to avoid overflow.
			//overflow is possible as 64 bit is largest size possible.
			unsigned long long temp[4];
			__m256i blendedA = _mm256_castpd_si256(_mm256_shuffle_pd(_mm256_castsi256_pd(values), _mm256_castsi256_pd(values), 0b0101)); //B,A,D,C
			__m256i A = _mm256_add_epi64(values, blendedA); //A+B, B+A, C+D, D+C
			_mm256_storeu_si256((__m256i*)temp, values);
			
			return temp[0] + temp[2];
		}

		//load / store
		static SIMD_AVX<uint64_t>load(uint64_t* pointer){return _mm256_loadu_si256((__m256i*)pointer);}
		void store(uint64_t* pointer){_mm256_storeu_si256((__m256i*)pointer, values);}
	};
	#endif
}