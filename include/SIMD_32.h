#pragma once
#include "StandardTypes.h"
#ifdef __WIN32
#include <intrin.h>
#else
#include <immintrin.h>
#endif
#include "SIMD_Template.h"

namespace smpl
{
	#ifdef __SSE4_2__
	template<>
	class SIMD_SSE<int>
	{
	public:
		static const int SIZE;
		static unsigned long long getSIMDBound(unsigned long long s) {return (s>>2)<<2;}

		SIMD_SSE(){}
		SIMD_SSE(int byte){values = _mm_set1_epi32(byte);}
		SIMD_SSE(__m128i byte){values = byte;}
		
		SIMD_SSE(const SIMD_SSE<int>& other) {values = other.values;}
		void operator=(const SIMD_SSE<int>& other) {values = other.values;}
		
		SIMD_SSE(const SIMD_SSE<int>&& other) noexcept {values = other.values;}
		void operator=(const SIMD_SSE<int>&& other) noexcept {values = other.values;}
		
		~SIMD_SSE(){}

		//load / store
		static SIMD_SSE<int> load(const int* pointer){return _mm_loadu_si128((__m128i*)pointer);}
		void store(int* pointer){_mm_storeu_si128((__m128i*)pointer, values);}
		
		//arithmetic
		SIMD_SSE<int> operator-() const {return _mm_sub_epi32(_mm_set1_epi32(0), values);}
		SIMD_SSE<int> operator+(const SIMD_SSE<int>& other) const {return _mm_add_epi32(values, other.values);}
		SIMD_SSE<int> operator-(const SIMD_SSE<int>& other) const {return _mm_sub_epi32(values, other.values);}
		SIMD_SSE<int> operator*(const SIMD_SSE<int>& other) const {return _mm_mullo_epi32(values, other.values);}

		SIMD_SSE<int> operator/(const SIMD_SSE<int>& other) const
		{
			__m128 A = _mm_cvtepi32_ps(values);
			__m128 B = _mm_cvtepi32_ps(other.values);
			__m128 result = _mm_div_ps(A,B);

			__m128i finalRes = SEML::floatToInt32(result);
			return finalRes;
		}
		
		void operator+=(const SIMD_SSE<int>& other) {values = operator+(other).values;}
		void operator-=(const SIMD_SSE<int>& other) {values = operator-(other).values;}
		void operator*=(const SIMD_SSE<int>& other) {values = operator*(other).values;}
		void operator/=(const SIMD_SSE<int>& other) {values = operator/(other).values;}
		
		//memory access and modification
		
		//bitwise
		SIMD_SSE<int> operator>>(const int shift) const {return _mm_srli_epi32(values, shift);}
		SIMD_SSE<int> operator<<(const int shift) const {return _mm_slli_epi32(values, shift);}
		
		void operator>>=(const int shift) {values = operator>>(shift).values;}
		void operator<<=(const int shift) {values = operator<<(shift).values;}
		
		SIMD_SSE<int> operator&(const int v) const {return _mm_and_si128(values, _mm_set1_epi32(v));}
		SIMD_SSE<int> operator&(const SIMD_SSE<int>& other) const {return _mm_and_si128(values, other.values);}
		
		void operator&=(const int v) {values = operator&(v).values;}
		void operator&=(const SIMD_SSE<int>& other) {values = operator&(other).values;}
		
		SIMD_SSE<int> bitwiseAndNot(const int v) const {return _mm_andnot_si128(values, _mm_set1_epi32(v));}
		SIMD_SSE<int> bitwiseAndNot(const SIMD_SSE<int>& other) const {return _mm_andnot_si128(values, other.values);}
		
		void bitwiseAndNot(const int v) {values = _mm_andnot_si128(values, _mm_set1_epi32(v));}
		void bitwiseAndNot(const SIMD_SSE<int>& other) {values = _mm_andnot_si128(values, other.values);}
		
		//comparison
		SIMD_SSE<int> operator>(const unsigned int byte) const {return _mm_cmpgt_epi32(values, _mm_set1_epi32(byte));}
		SIMD_SSE<int> operator>(const SIMD_SSE<int>& other) const {return _mm_cmpgt_epi32(values, other.values);}
		
		SIMD_SSE<int> operator<(const unsigned int byte) const {return _mm_cmplt_epi32(values, _mm_set1_epi32(byte));}
		SIMD_SSE<int> operator<(const SIMD_SSE<int>& other) const {return _mm_cmplt_epi32(values, other.values);}
		
		SIMD_SSE<int> operator==(const unsigned int byte) const {return _mm_cmpeq_epi32(values, _mm_set1_epi32(byte));}
		SIMD_SSE<int> operator==(const SIMD_SSE<int>& other) const {return _mm_cmpeq_epi32(values, other.values);}
		
		SIMD_SSE<int> operator!=(const unsigned int byte) const 
		{
			__m128i temp = _mm_cmpeq_epi32(values, _mm_set1_epi32(byte));
			return _mm_andnot_si128(temp, temp); //does not bitwise not
		}
		SIMD_SSE<int> operator!=(const SIMD_SSE<int>& other) const
		{
			__m128i temp = _mm_cmpeq_epi32(values, other.values);
			return _mm_andnot_si128(temp, temp); //does not bitwise not
		}
		SIMD_SSE<int> blend(const SIMD_SSE<int>& other, const SIMD_SSE<int>& blendFactor) const
		{
			return _mm_castps_si128(_mm_blendv_ps(_mm_castsi128_ps(values), _mm_castsi128_ps(other.values), _mm_castsi128_ps(blendFactor.values)));
		}
		
		//special case functions
		SIMD_SSE<int> horizontalAdd(const SIMD_SSE<int>& other) const {return _mm_hadd_epi32(values, other.values);}
		long long sum() const
		{
			//sum of all items into the largest datatype NEEDED to avoid overflow.
			//Ensures no overflow
			long long temp[2];
			__m128i low = _mm_cvtepi32_epi64(values); //(A1, A2)
			__m128i high = _mm_cvtepi32_epi64(_mm_srli_si128(values, 8)); //(A3, A4)

			//add 64 bit values
			__m128i result = _mm_add_epi64(low, high); //(A1+A3, A2+A4)
			_mm_storeu_si128((__m128i*)temp, result);
			return temp[0] + temp[1];
		}

		operator SIMD_SSE<unsigned int>() const;
		operator SIMD_SSE<float>() const;
		__m128i values;
	};
	inline const int SIMD_SSE<int>::SIZE = 4;

	template<>
	class SIMD_SSE<unsigned int> : public SIMD_SSE<int>
	{
	public:
		SIMD_SSE(){}
		SIMD_SSE(unsigned int byte){values = _mm_set1_epi32(byte);}
		SIMD_SSE(__m128i byte){values = byte;}
		
		SIMD_SSE(const SIMD_SSE<unsigned int>& other) {values = other.values;}
		void operator=(const SIMD_SSE<unsigned int>& other) {values = other.values;}
		
		SIMD_SSE(const SIMD_SSE<unsigned int>&& other) noexcept {values = other.values;}
		void operator=(const SIMD_SSE<unsigned int>&& other) noexcept {values = other.values;}
		
		~SIMD_SSE(){}

		SIMD_SSE<unsigned int> operator/(const SIMD_SSE<unsigned int>& other) const
		{
			//To utilize the entire range and do it in a small number of instructions, use 64 bit (yes its probably faster)
			__m128d A1 = SEML::fastInt64ToDouble(_mm_cvtepu32_epi64(values));
			__m128d A2 = SEML::fastInt64ToDouble(_mm_cvtepu32_epi64(_mm_srli_si128(values, 8)));
			__m128d B1 = SEML::fastInt64ToDouble(_mm_cvtepu32_epi64(other.values));
			__m128d B2 = SEML::fastInt64ToDouble(_mm_cvtepu32_epi64(_mm_srli_si128(other.values, 8)));
			
			__m128d res1 = _mm_div_pd(A1, B1);
			__m128d res2 = _mm_div_pd(A2, B2);
			
			__m128i low = _mm_cvtpd_epi32(res1);
			__m128i high = _mm_cvtpd_epi32(res2);

			//cheat because the proper shuffle function doesn't exist for integers
			__m128 result = _mm_shuffle_ps(_mm_castsi128_ps(low), _mm_castsi128_ps(high), 0b01000100);
			return _mm_castps_si128(result);
		}
		
		SIMD_SSE<unsigned int> operator>(const unsigned int byte) const
		{
			__m128i data = _mm_set1_epi32(byte);
			return _mm_cmpeq_epi32(_mm_max_epu32(values, data), values);
		}
		SIMD_SSE<unsigned int> operator>(const SIMD_SSE<unsigned int>& other) const
		{
			return _mm_cmpeq_epi32(_mm_max_epu32(values, other.values), values);
		}
		
		SIMD_SSE<unsigned int> operator<(const unsigned int byte) const
		{
			__m128i data = _mm_set1_epi32(byte);
			return _mm_cmpeq_epi32(_mm_min_epu32(values, data), values);
		}
		SIMD_SSE<unsigned int> operator<(const SIMD_SSE<unsigned int>& other) const
		{
			return _mm_cmpeq_epi32(_mm_min_epu32(values, other.values), values);
		}
		SIMD_SSE<unsigned int> blend(const SIMD_SSE<unsigned int>& other, const SIMD_SSE<unsigned int>& blendFactor) const
		{
			return _mm_castps_si128(_mm_blendv_ps(_mm_castsi128_ps(values), _mm_castsi128_ps(other.values), _mm_castsi128_ps(blendFactor.values)));
		}

		unsigned long long sum() const
		{
			//sum of all items into the largest datatype NEEDED to avoid overflow.
			//Ensures no overflow
			unsigned long long temp[2];
			__m128i low = _mm_cvtepu32_epi64(values); //(A1, A2)
			__m128i high = _mm_cvtepu32_epi64(_mm_srli_si128(values, 8)); //(A3, A4)

			//add 64 bit values
			__m128i result = _mm_add_epi64(low, high); //(A1+A3, A2+A4)
			_mm_storeu_si128((__m128i*)temp, result);
			return temp[0] + temp[1];
		}

		//load / store
		static SIMD_SSE<unsigned int>load(const unsigned int* pointer){return _mm_loadu_si128((__m128i*)pointer);}
		void store(unsigned int* pointer){_mm_storeu_si128((__m128i*)pointer, values);}
		
		operator SIMD_SSE<float>() const;
	};

	#endif


	#ifdef __AVX2__
	template<>
	class SIMD_AVX<int>
	{
	public:
		static const int SIZE;
		static unsigned long long getSIMDBound(unsigned long long s) {return (s>>3)<<3;}

		SIMD_AVX(){}
		SIMD_AVX(int byte){values = _mm256_set1_epi32(byte);}
		SIMD_AVX(__m256i byte){values = byte;}
		
		SIMD_AVX(const SIMD_AVX<int>& other) {values = other.values;}
		void operator=(const SIMD_AVX<int>& other) {values = other.values;}
		
		SIMD_AVX(const SIMD_AVX<int>&& other) noexcept {values = other.values;}
		void operator=(const SIMD_AVX<int>&& other) noexcept {values = other.values;}
		
		~SIMD_AVX(){}

		//load / store
		static SIMD_AVX<int>load(const int* pointer){return _mm256_loadu_si256((__m256i*)pointer);}
		void store(int* pointer){_mm256_storeu_si256((__m256i*)pointer, values);}
		
		//arithmetic
		SIMD_AVX<int> operator-() const {return _mm256_sub_epi32(_mm256_set1_epi32(0), values);}
		SIMD_AVX<int> operator+(const SIMD_AVX<int>& other) const {return _mm256_add_epi32(values, other.values);}
		SIMD_AVX<int> operator-(const SIMD_AVX<int>& other) const {return _mm256_sub_epi32(values, other.values);}

		SIMD_AVX<int> operator*(const SIMD_AVX<int>& other) const {return _mm256_mullo_epi32(values, other.values);}
		SIMD_AVX<int> operator/(const SIMD_AVX<int>& other) const
		{
			//Just cast to float -> divide -> cast back
			__m256 A1 = _mm256_cvtepi32_ps(values);
			__m256 B1 = _mm256_cvtepi32_ps(other.values);
			__m256 res1 = _mm256_div_ps(A1, B1);
			
			return SEML::floatToInt32(res1);
		}
		
		void operator+=(const SIMD_AVX<int>& other) {values = operator+(other).values;}
		void operator-=(const SIMD_AVX<int>& other) {values = operator-(other).values;}
		void operator*=(const SIMD_AVX<int>& other) {values = operator*(other).values;}
		void operator/=(const SIMD_AVX<int>& other) {values = operator/(other).values;}
		
		//memory access and modification
		
		//bitwise
		SIMD_AVX<int> operator>>(const int shift) const {return _mm256_srli_epi32(values, shift);}
		SIMD_AVX<int> operator<<(const int shift) const {return _mm256_slli_epi32(values, shift);}
		
		void operator>>=(const int shift) {values = operator>>(shift).values;}
		void operator<<=(const int shift) {values = operator<<(shift).values;}
		
		SIMD_AVX<int> operator&(const int v) const {return _mm256_and_si256(values, _mm256_set1_epi32(v));}
		SIMD_AVX<int> operator&(const SIMD_AVX<int>& other) const {return _mm256_and_si256(values, other.values);}
		
		void operator&=(const int v) {values = operator&(v).values;}
		void operator&=(const SIMD_AVX<int>& other) {values = operator&(other).values;}
		
		SIMD_AVX<int> bitwiseAndNot(const int v) const {return _mm256_andnot_si256(values, _mm256_set1_epi32(v));}
		SIMD_AVX<int> bitwiseAndNot(const SIMD_AVX<int>& other) const {return _mm256_andnot_si256(values, other.values);}
		
		void bitwiseAndNot(const int v) {values = _mm256_andnot_si256(values, _mm256_set1_epi32(v));}
		void bitwiseAndNot(const SIMD_AVX<int>& other) {values = _mm256_andnot_si256(values, other.values);}
		
		//comparison
		SIMD_AVX<int> operator>(const unsigned int byte) const {return _mm256_cmpgt_epi32(values, _mm256_set1_epi32(byte));}
		SIMD_AVX<int> operator>(const SIMD_AVX<int>& other) const {return _mm256_cmpgt_epi32(values, other.values);}
		
		SIMD_AVX<int> operator<(const unsigned int byte) const {return _mm256_cmpgt_epi32(_mm256_set1_epi32(byte), values);} //incorrect by the way. This is "<=" and not "<"
		SIMD_AVX<int> operator<(const SIMD_AVX<int>& other) const {return _mm256_cmpgt_epi32(other.values, values);} //incorrect by the way. This is "<=" and not "<"
		
		SIMD_AVX<int> operator==(const unsigned int byte) const {return _mm256_cmpeq_epi32(values, _mm256_set1_epi32(byte));}
		SIMD_AVX<int> operator==(const SIMD_AVX<int>& other) const {return _mm256_cmpeq_epi32(values, other.values);}
		
		SIMD_AVX<int> operator!=(const unsigned int byte) const 
		{
			__m256i temp = _mm256_cmpeq_epi32(values, _mm256_set1_epi32(byte));
			return _mm256_andnot_si256(temp, temp); //does not bitwise not
		}
		SIMD_AVX<int> operator!=(const SIMD_AVX<int>& other) const
		{
			__m256i temp = _mm256_cmpeq_epi32(values, other.values);
			return _mm256_andnot_si256(temp, temp); //does not bitwise not
		}
		SIMD_AVX<int> blend(const SIMD_AVX<int>& other, const SIMD_AVX<int>& blendFactor) const
		{
			return _mm256_castps_si256(_mm256_blendv_ps(_mm256_castsi256_ps(values), _mm256_castsi256_ps(other.values), _mm256_castsi256_ps(blendFactor.values)));
		}
		
		//special case functions
		SIMD_AVX<int> horizontalAdd(const SIMD_AVX<int>& other) const {return _mm256_hadd_epi32(values, other.values);}
		long long sum() const
		{
			//sum of all items into the largest datatype NEEDED to avoid overflow.
			//Ensures no overflow
			long long temp[4];
			__m256i low = _mm256_cvtepi32_epi64(_mm256_extracti128_si256(values, 0)); //(A1, A2, A3, A4)
			__m256i high = _mm256_cvtepi32_epi64(_mm256_extracti128_si256(values, 1)); //(A5, A6, A7, A8)

			//add 64 bit values
			__m256i result = _mm256_add_epi64(low, high); //(A1+A5, ... , A4+A8) Pairs of 2
			
			//the same as the AVX<int64_t> version from here
			__m256i blendedA = _mm256_castpd_si256(_mm256_shuffle_pd(_mm256_castsi256_pd(result), _mm256_castsi256_pd(result), 0b0101)); //B,A,D,C
			__m256i A = _mm256_add_epi64(result, blendedA); //A+B, B+A, C+D, D+C
			_mm256_storeu_si256((__m256i*)temp, A);

			return temp[0] + temp[2];
		}

		operator SIMD_AVX<unsigned int>() const;
		operator SIMD_AVX<float>() const;
		__m256i values;

	private:
	};
	inline const int SIMD_AVX<int>::SIZE = 8;

	template<>
	class SIMD_AVX<unsigned int> : public SIMD_AVX<int>
	{
	public:
		SIMD_AVX(){}
		SIMD_AVX(unsigned int byte){values = _mm256_set1_epi32(byte);}
		SIMD_AVX(__m256i byte){values = byte;}
		
		SIMD_AVX(const SIMD_AVX<unsigned int>& other) {values = other.values;}
		void operator=(const SIMD_AVX<unsigned int>& other) {values = other.values;}
		
		SIMD_AVX(const SIMD_AVX<unsigned int>&& other) noexcept {values = other.values;}
		void operator=(const SIMD_AVX<unsigned int>&& other) noexcept {values = other.values;}
		
		~SIMD_AVX(){}

		SIMD_AVX<unsigned int> operator/(const SIMD_AVX<unsigned int>& other) const
		{
			__m256 A = SEML::uint32ToFloat(values);
			__m256 B = SEML::uint32ToFloat(other.values);
			__m256 res1 = _mm256_div_ps(A, B);
			
			return SEML::floatToUInt32(res1);
		}
		
		SIMD_AVX<unsigned int> operator>(const unsigned int byte) const
		{
			__m256i data = _mm256_set1_epi32(byte);
			return _mm256_cmpeq_epi32(_mm256_max_epu32(values, data), values);
		}
		SIMD_AVX<unsigned int> operator>(const SIMD_AVX<unsigned int>& other) const
		{
			return _mm256_cmpeq_epi32(_mm256_max_epu32(values, other.values), values);
		}
		
		SIMD_AVX<unsigned int> operator<(const unsigned int byte) const
		{
			__m256i data = _mm256_set1_epi32(byte);
			return _mm256_cmpeq_epi32(_mm256_min_epu32(values, data), values);
		}
		SIMD_AVX<unsigned int> operator<(const SIMD_AVX<unsigned int>& other) const
		{
			return _mm256_cmpeq_epi32(_mm256_min_epu32(values, other.values), values);
		}
		SIMD_AVX<unsigned int> blend(const SIMD_AVX<unsigned int>& other, const SIMD_AVX<unsigned int>& blendFactor) const
		{
			return _mm256_castps_si256(_mm256_blendv_ps(_mm256_castsi256_ps(values), _mm256_castsi256_ps(other.values), _mm256_castsi256_ps(blendFactor.values)));
		}

		unsigned long long sum() const
		{
			//sum of all items into the largest datatype NEEDED to avoid overflow.
			//Ensures no overflow
			unsigned long long temp[4];
			__m256i low = _mm256_cvtepi32_epi64(_mm256_extracti128_si256(values, 0)); //(A1, A2, A3, A4)
			__m256i high = _mm256_cvtepi32_epi64(_mm256_extracti128_si256(values, 1)); //(A5, A6, A7, A8)

			//add 64 bit values
			__m256i result = _mm256_add_epi64(low, high); //(A1+A5, ... , A4+A8) Pairs of 2
			
			//the same as the AVX<int64_t> version from here
			__m256i blendedA = _mm256_castpd_si256(_mm256_shuffle_pd(_mm256_castsi256_pd(result), _mm256_castsi256_pd(result), 0b0101)); //B,A,D,C
			__m256i A = _mm256_add_epi64(result, blendedA); //A+B, B+A, C+D, D+C
			_mm256_storeu_si256((__m256i*)temp, A);

			return temp[0] + temp[2];
		}

		//load / store
		static SIMD_AVX<unsigned int>load(const unsigned int* pointer){return _mm256_loadu_si256((__m256i*)pointer);}
		void store(unsigned int* pointer){_mm256_storeu_si256((__m256i*)pointer, values);}
		
		operator SIMD_AVX<float>() const;
	};
	#endif
}