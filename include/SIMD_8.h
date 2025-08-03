#pragma once
#include <immintrin.h>
#include "SIMD_Template.h"

namespace smpl
{
	#ifdef __SSE4_2__
	template<>
	class SIMD_SSE<char>
	{
	public:
		static const int SIZE = 16;
		static unsigned long long getSIMDBound(unsigned long long s) {return (s>>4)<<4;}

		SIMD_SSE(){}
		SIMD_SSE(char byte){values = _mm_set1_epi8(byte);}
		SIMD_SSE(__m128i byte){values = byte;}
		
		SIMD_SSE(const SIMD_SSE<char>& other) {values = other.values;}
		void operator=(const SIMD_SSE<char>& other) {values = other.values;}
		
		SIMD_SSE(const SIMD_SSE<char>&& other) noexcept {values = other.values;}
		void operator=(const SIMD_SSE<char>&& other) noexcept {values = other.values;}
		
		~SIMD_SSE(){}

		//load / store
		static SIMD_SSE<char>load(char* pointer){return _mm_loadu_si128((__m128i*)pointer);}
		void store(char* pointer){_mm_storeu_si128((__m128i*)pointer, values);}
		
		//arithmetic
		SIMD_SSE<char> operator-() const {return _mm_sub_epi8(_mm_set1_epi8(0), values);}
		SIMD_SSE<char> operator+(const SIMD_SSE<char>& other) const {return _mm_add_epi8(values, other.values);}
		SIMD_SSE<char> operator-(const SIMD_SSE<char>& other) const {return _mm_sub_epi8(values, other.values);}

		SIMD_SSE<char> operator*(const SIMD_SSE<char>& other) const
		{
			//convert to shorts -> multiply -> convert back
			__m128i lowShortsA = _mm_cvtepi8_epi16(values);
			__m128i highShortsA = _mm_cvtepi8_epi16(_mm_srli_si128(values, 8));
			__m128i lowShortsB = _mm_cvtepi8_epi16(other.values);
			__m128i highShortsB = _mm_cvtepi8_epi16(_mm_srli_si128(other.values, 8));

			__m128i lowShortMult = _mm_mullo_epi16(lowShortsA, lowShortsB);
			__m128i highShortMult = _mm_mullo_epi16(highShortsA, highShortsB);
			
			return _mm_packs_epi16(lowShortMult, highShortMult);
		}
		SIMD_SSE<char> operator/(const SIMD_SSE<char>& other) const
		{
			//doesn't really exist here. Potential operations require AVX or casting to a higher datatype.
			//Just cast to float -> divide -> cast back
			__m128 A1 = _mm_cvtepi32_ps(_mm_cvtepi8_epi32(values));
			__m128 B1 = _mm_cvtepi32_ps(_mm_cvtepi8_epi32(other.values));
			
			__m128 A2 = _mm_cvtepi32_ps(_mm_cvtepi8_epi32(_mm_srli_si128(values, 4)));
			__m128 B2 = _mm_cvtepi32_ps(_mm_cvtepi8_epi32(_mm_srli_si128(other.values, 4)));
			
			__m128 A3 = _mm_cvtepi32_ps(_mm_cvtepi8_epi32(_mm_srli_si128(values, 8)));
			__m128 B3 = _mm_cvtepi32_ps(_mm_cvtepi8_epi32(_mm_srli_si128(other.values, 8)));
			
			__m128 A4 = _mm_cvtepi32_ps(_mm_cvtepi8_epi32(_mm_srli_si128(values, 12)));
			__m128 B4 = _mm_cvtepi32_ps(_mm_cvtepi8_epi32(_mm_srli_si128(other.values, 12)));

			__m128 res1 = _mm_div_ps(A1, B1);
			__m128 res2 = _mm_div_ps(A2, B2);
			__m128 res3 = _mm_div_ps(A3, B3);
			__m128 res4 = _mm_div_ps(A4, B4);
			
			__m128i output1 = SEML::floatToInt32(res1);
			__m128i output2 = SEML::floatToInt32(res2);
			__m128i output3 = SEML::floatToInt32(res3);
			__m128i output4 = SEML::floatToInt32(res4);

			__m128i pack16Low = _mm_packus_epi32(output1, output2);
			__m128i pack16High = _mm_packus_epi32(output3, output4);

			return _mm_packs_epi16(pack16Low, pack16High);
		}
		
		void operator+=(const SIMD_SSE<char>& other) {values = operator+(other).values;}
		void operator-=(const SIMD_SSE<char>& other) {values = operator-(other).values;}
		void operator*=(const SIMD_SSE<char>& other) {values = operator*(other).values;}
		void operator/=(const SIMD_SSE<char>& other) {values = operator/(other).values;}
		
		//memory access and modification
		
		//bitwise
		SIMD_SSE<char> operator>>(const int shift) const 
		{
			__m128i low = _mm_cvtepi8_epi16(values);
			__m128i high = _mm_cvtepi8_epi16(_mm_srli_si128(values, 8));
			low = _mm_srli_epi16(low, shift);
			high = _mm_srli_epi16(high, shift);
			
			return _mm_packs_epi16(low, high);
		}
		SIMD_SSE<char> operator<<(const int shift) const
		{
			__m128i low = _mm_cvtepi8_epi16(values);
			__m128i high = _mm_cvtepi8_epi16(_mm_srli_si128(values, 8));
			low = _mm_slli_epi16(low, shift);
			high = _mm_slli_epi16(high, shift);
			
			return _mm_packs_epi16(low, high);
		}
		
		void operator>>=(const int shift) {values = operator>>(shift).values;}
		void operator<<=(const int shift) {values = operator<<(shift).values;}
		
		SIMD_SSE<char> operator&(const int v) const {return _mm_and_si128(values, _mm_set1_epi8(v));}
		SIMD_SSE<char> operator&(const SIMD_SSE<char>& other) const {return _mm_and_si128(values, other.values);}
		
		void operator&=(const int v) {values = operator&(v).values;}
		void operator&=(const SIMD_SSE<char>& other) {values = operator&(other).values;}
		
		SIMD_SSE<char> bitwiseAndNot(const int v) const {return _mm_andnot_si128(values, _mm_set1_epi8(v));}
		SIMD_SSE<char> bitwiseAndNot(const SIMD_SSE<char>& other) const {return _mm_andnot_si128(values, other.values);}
		
		void bitwiseAndNot(const int v) {values = _mm_andnot_si128(values, _mm_set1_epi8(v));}
		void bitwiseAndNot(const SIMD_SSE<char>& other) {values = _mm_andnot_si128(values, other.values);}
		
		//comparison
		SIMD_SSE<char> operator>(const unsigned char byte) const {return _mm_cmpgt_epi8(values, _mm_set1_epi8(byte));}
		SIMD_SSE<char> operator>(const SIMD_SSE<char>& other) const {return _mm_cmpgt_epi8(values, other.values);}
		
		SIMD_SSE<char> operator<(const unsigned char byte) const {return _mm_cmplt_epi8(values, _mm_set1_epi8(byte));}
		SIMD_SSE<char> operator<(const SIMD_SSE<char>& other) const {return _mm_cmplt_epi8(values, other.values);}
		
		SIMD_SSE<char> operator==(const unsigned char byte) const {return _mm_cmpeq_epi8(values, _mm_set1_epi8(byte));}
		SIMD_SSE<char> operator==(const SIMD_SSE<char>& other) const {return _mm_cmpeq_epi8(values, other.values);}
		
		SIMD_SSE<char> operator!=(const unsigned char byte) const 
		{
			__m128i temp = _mm_cmpeq_epi8(values, _mm_set1_epi8(byte));
			return _mm_andnot_si128(temp, temp); //does not bitwise not
		}
		SIMD_SSE<char> operator!=(const SIMD_SSE<char>& other) const
		{
			__m128i temp = _mm_cmpeq_epi8(values, other.values);
			return _mm_andnot_si128(temp, temp); //does not bitwise not
		}
		
		//special case functions
		SIMD_SSE<char> horizontalAdd(const SIMD_SSE<char>& other) const 
		{
			//shuffle then add
			const __m128i shuffle1 = _mm_set_epi8(14, 12, 10, 8, 6, 4, 2, 0, 14, 12, 10, 8, 6, 4, 2, 0);
			const __m128i shuffle2 = _mm_set_epi8(15, 13, 11, 9, 7, 5, 3, 1, 15, 13, 11, 9, 7, 5, 3, 1);
			__m128i temp = _mm_shuffle_epi8(values, shuffle1); //A0, A2, A4, A6, A8, A10, A12, A14, A0, A2, A4, A6, A8, A10, A12, A14
			__m128i temp2 = _mm_shuffle_epi8(other.values, shuffle2); //A1, A3, A5, A7, A9, A11, A13, A15, A1, A3, A5, A7, A9, A11, A13, A15
			return _mm_add_epi8(temp, temp2);
		}
		short sum() const
		{
			//sum of all items into the largest datatype NEEDED to avoid overflow.
			//Ensures no overflow
			short temp[8];
			__m128i low = _mm_cvtepi8_epi16(values); //(A1, A2, A3, A4, A5, A6, A7, A8)
			__m128i high = _mm_cvtepi8_epi16(_mm_srli_si128(values, 8)); //(A9, A10, A11, A12, A13, A14, A15, A16)

			//add 16 bit values
			__m128i result = _mm_add_epi16(low, high); //(A1+A9, A2+A10, A3+A11, A4+A12, A5+A13, A6+A14, A7+A15, A8+A16)
			result = _mm_hadd_epi16(result, result); //(A1+A9+A2+10, A3+A11+A4+12, A5+A13+A6+A14, A7+A15+A8+A16, dups, dups, dups, dups)
			result = _mm_hadd_epi16(result, result); //first 2 elements contain the working sum
			_mm_storeu_si128((__m128i*)temp, values);
			return temp[0] + temp[1];
		}

		__m128i values;
	};

	template<>
	class SIMD_SSE<unsigned char> : public SIMD_SSE<char>
	{
	public:
		SIMD_SSE(){}
		SIMD_SSE(unsigned char byte){values = _mm_set1_epi8(byte);}
		SIMD_SSE(__m128i byte){values = byte;}
		
		SIMD_SSE(const SIMD_SSE<unsigned char>& other) {values = other.values;}
		void operator=(const SIMD_SSE<unsigned char>& other) {values = other.values;}
		
		SIMD_SSE(const SIMD_SSE<unsigned char>&& other) noexcept {values = other.values;}
		void operator=(const SIMD_SSE<unsigned char>&& other) noexcept {values = other.values;}
		
		~SIMD_SSE(){}

		SIMD_SSE<unsigned char> operator*(const SIMD_SSE<unsigned char>& other) const
		{
			//convert to shorts -> multiply -> convert back
			__m128i lowShortsA = _mm_cvtepu8_epi16(values);
			__m128i highShortsA = _mm_cvtepu8_epi16(_mm_srli_si128(values, 8));
			__m128i lowShortsB = _mm_cvtepu8_epi16(other.values);
			__m128i highShortsB = _mm_cvtepu8_epi16(_mm_srli_si128(other.values, 8));

			__m128i lowShortMult = _mm_mullo_epi16(lowShortsA, lowShortsB);
			__m128i highShortMult = _mm_mullo_epi16(highShortsA, highShortsB);
			
			return _mm_packus_epi16(lowShortMult, highShortMult);
		}
		SIMD_SSE<unsigned char> operator/(const SIMD_SSE<unsigned char>& other) const
		{
			//doesn't really exist here. Potential operations require AVX or casting to a higher datatype.
			//Just cast to float -> divide -> cast back
			__m128 A1 = _mm_cvtepi32_ps(_mm_cvtepu8_epi32(values));
			__m128 B1 = _mm_cvtepi32_ps(_mm_cvtepu8_epi32(other.values));
			
			__m128 A2 = _mm_cvtepi32_ps(_mm_cvtepu8_epi32(_mm_srli_si128(values, 4)));
			__m128 B2 = _mm_cvtepi32_ps(_mm_cvtepu8_epi32(_mm_srli_si128(other.values, 4)));
			
			__m128 A3 = _mm_cvtepi32_ps(_mm_cvtepu8_epi32(_mm_srli_si128(values, 8)));
			__m128 B3 = _mm_cvtepi32_ps(_mm_cvtepu8_epi32(_mm_srli_si128(other.values, 8)));
			
			__m128 A4 = _mm_cvtepi32_ps(_mm_cvtepu8_epi32(_mm_srli_si128(values, 12)));
			__m128 B4 = _mm_cvtepi32_ps(_mm_cvtepu8_epi32(_mm_srli_si128(other.values, 12)));

			__m128 res1 = _mm_div_ps(A1, B1);
			__m128 res2 = _mm_div_ps(A2, B2);
			__m128 res3 = _mm_div_ps(A3, B3);
			__m128 res4 = _mm_div_ps(A4, B4);
			
			__m128i output1 = SEML::floatToInt32(res1);
			__m128i output2 = SEML::floatToInt32(res2);
			__m128i output3 = SEML::floatToInt32(res3);
			__m128i output4 = SEML::floatToInt32(res4);

			__m128i pack16Low = _mm_packus_epi32(output1, output2);
			__m128i pack16High = _mm_packus_epi32(output3, output4);

			return _mm_packus_epi16(pack16Low, pack16High);
		}

		SIMD_SSE<unsigned char> operator>>(const int shift) const 
		{
			__m128i low = _mm_cvtepu8_epi16(values);
			__m128i high = _mm_cvtepu8_epi16(_mm_srli_si128(values, 8));
			low = _mm_srli_epi16(low, shift);
			high = _mm_srli_epi16(high, shift);
			
			return _mm_packus_epi16(low, high);
		}
		SIMD_SSE<unsigned char> operator<<(const int shift) const
		{
			__m128i low = _mm_cvtepu8_epi16(values);
			__m128i high = _mm_cvtepu8_epi16(_mm_srli_si128(values, 8));
			low = _mm_slli_epi16(low, shift);
			high = _mm_slli_epi16(high, shift);
			
			return _mm_packus_epi16(low, high);
		}
		
		SIMD_SSE<unsigned char> operator>(const unsigned char byte) const
		{
			__m128i data = _mm_set1_epi8(byte);
			return _mm_cmpeq_epi8(_mm_max_epu8(values, data), values);
		}
		SIMD_SSE<unsigned char> operator>(const SIMD_SSE<unsigned char>& other) const
		{
			return _mm_cmpeq_epi8(_mm_max_epu8(values, other.values), values);
		}
		
		SIMD_SSE<unsigned char> operator<(const unsigned char byte) const
		{
			__m128i data = _mm_set1_epi8(byte);
			return _mm_cmpeq_epi8(_mm_min_epu8(values, data), values);
		}
		SIMD_SSE<unsigned char> operator<(const SIMD_SSE<unsigned char>& other) const
		{
			return _mm_cmpeq_epi8(_mm_min_epu8(values, other.values), values);
		}

		unsigned short sum() const
		{
			//sum of all items into the largest datatype NEEDED to avoid overflow.
			//Ensures no overflow
			unsigned short temp[8];
			__m128i low = _mm_cvtepu8_epi16(values); //(A1, A2, A3, A4, A5, A6, A7, A8)
			__m128i high = _mm_cvtepu8_epi16(_mm_srli_si128(values, 8)); //(A9, A10, A11, A12, A13, A14, A15, A16)

			//add 16 bit values
			__m128i result = _mm_add_epi16(low, high); //(A1+A9, A2+A10, A3+A11, A4+A12, A5+A13, A6+A14, A7+A15, A8+A16)
			result = _mm_hadd_epi16(result, result); //(A1+A9+A2+10, A3+A11+A4+12, A5+A13+A6+A14, A7+A15+A8+A16, dups, dups, dups, dups)
			result = _mm_hadd_epi16(result, result); //first 2 elements contain the working sum
			_mm_storeu_si128((__m128i*)temp, values);
			return temp[0] + temp[1];
		}

		//load / store
		static SIMD_SSE<unsigned char>load(unsigned char* pointer){return _mm_loadu_si128((__m128i*)pointer);}
		void store(unsigned char* pointer){_mm_storeu_si128((__m128i*)pointer, values);}
	};

	#endif


	#ifdef __AVX2__
	template<>
	class SIMD_AVX<char>
	{
	public:
		static const int SIZE = 32;
		static unsigned long long getSIMDBound(unsigned long long s) {return (s>>5)<<5;}

		SIMD_AVX(){}
		SIMD_AVX(char byte){values = _mm256_set1_epi8(byte);}
		SIMD_AVX(__m256i byte){values = byte;}
		
		SIMD_AVX(const SIMD_AVX<char>& other) {values = other.values;}
		void operator=(const SIMD_AVX<char>& other) {values = other.values;}
		
		SIMD_AVX(const SIMD_AVX<char>&& other) noexcept {values = other.values;}
		void operator=(const SIMD_AVX<char>&& other) noexcept {values = other.values;}
		
		~SIMD_AVX(){}

		//load / store
		static SIMD_AVX<char>load(char* pointer){return _mm256_loadu_si256((__m256i*)pointer);}
		void store(char* pointer){_mm256_storeu_si256((__m256i*)pointer, values);}
		
		//arithmetic
		SIMD_AVX<char> operator-() const {return _mm256_sub_epi8(_mm256_set1_epi8(0), values);}
		SIMD_AVX<char> operator+(const SIMD_AVX<char>& other) const {return _mm256_add_epi8(values, other.values);}
		SIMD_AVX<char> operator-(const SIMD_AVX<char>& other) const {return _mm256_sub_epi8(values, other.values);}

		SIMD_AVX<char> operator*(const SIMD_AVX<char>& other) const
		{
			//convert to shorts -> multiply -> convert back
			__m256i lowShortsA = _mm256_cvtepi8_epi16(_mm256_extracti128_si256(values, 0));
			__m256i highShortsA = _mm256_cvtepi8_epi16(_mm256_extracti128_si256(values, 1));
			__m256i lowShortsB = _mm256_cvtepi8_epi16(_mm256_extracti128_si256(other.values, 0));
			__m256i highShortsB = _mm256_cvtepi8_epi16(_mm256_extracti128_si256(other.values, 1));

			__m256i lowShortMult = _mm256_mullo_epi16(lowShortsA, lowShortsB);
			__m256i highShortMult = _mm256_mullo_epi16(highShortsA, highShortsB);
			
			return _mm256_packs_epi16(lowShortMult, highShortMult);
		}
		SIMD_AVX<char> operator/(const SIMD_AVX<char>& other) const
		{
			//doesn't really exist here. Potential operations require AVX or casting to a higher datatype.
			//Just cast to float -> divide -> cast back
			__m128i A1_Extracted128 = _mm256_extracti128_si256(values, 0);
			__m128i A2_Extracted128 = _mm256_extracti128_si256(values, 1);
			__m128i B1_Extracted128 = _mm256_extracti128_si256(other.values, 0);
			__m128i B2_Extracted128 = _mm256_extracti128_si256(other.values, 1);
			
			__m256 A1 = _mm256_cvtepi32_ps(_mm256_cvtepi8_epi32(A1_Extracted128));
			__m256 B1 = _mm256_cvtepi32_ps(_mm256_cvtepi8_epi32(B1_Extracted128));
			
			__m256 A2 = _mm256_cvtepi32_ps(_mm256_cvtepi8_epi32(_mm_srli_si128(A1_Extracted128, 8)));
			__m256 B2 = _mm256_cvtepi32_ps(_mm256_cvtepi8_epi32(_mm_srli_si128(B1_Extracted128, 8)));
			
			__m256 A3 = _mm256_cvtepi32_ps(_mm256_cvtepi8_epi32(A2_Extracted128));
			__m256 B3 = _mm256_cvtepi32_ps(_mm256_cvtepi8_epi32(B2_Extracted128));

			__m256 A4 = _mm256_cvtepi32_ps(_mm256_cvtepi8_epi32(_mm_srli_si128(A2_Extracted128, 8)));
			__m256 B4 = _mm256_cvtepi32_ps(_mm256_cvtepi8_epi32(_mm_srli_si128(B2_Extracted128, 8)));

			__m256 res1 = _mm256_div_ps(A1, B1);
			__m256 res2 = _mm256_div_ps(A2, B2);
			__m256 res3 = _mm256_div_ps(A3, B3);
			__m256 res4 = _mm256_div_ps(A4, B4);
			
			__m256i output1 = _mm256_cvtps_epi32(res1);
			__m256i output2 = _mm256_cvtps_epi32(res2);
			__m256i output3 = _mm256_cvtps_epi32(res3);
			__m256i output4 = _mm256_cvtps_epi32(res4);

			__m256i pack16Low = _mm256_packs_epi32(output1, output2);
			__m256i pack16High = _mm256_packs_epi32(output3, output4);

			return _mm256_packs_epi16(pack16Low, pack16High);
		}
		
		void operator+=(const SIMD_AVX<char>& other) {values = operator+(other).values;}
		void operator-=(const SIMD_AVX<char>& other) {values = operator-(other).values;}
		void operator*=(const SIMD_AVX<char>& other) {values = operator*(other).values;}
		void operator/=(const SIMD_AVX<char>& other) {values = operator/(other).values;}
		
		//memory access and modification
		
		//bitwise
		SIMD_AVX<char> operator>>(const int shift) const 
		{
			__m256i low = _mm256_cvtepi8_epi16(_mm256_extracti128_si256(values, 0));
			__m256i high = _mm256_cvtepi8_epi16(_mm256_extracti128_si256(values, 1));
			low = _mm256_srli_epi16(low, shift);
			high = _mm256_srli_epi16(high, shift);
			
			return _mm256_packs_epi16(low, high);
		}
		SIMD_AVX<char> operator<<(const int shift) const
		{
			__m256i low = _mm256_cvtepi8_epi16(_mm256_extracti128_si256(values, 0));
			__m256i high = _mm256_cvtepi8_epi16(_mm256_extracti128_si256(values, 1));
			low = _mm256_slli_epi16(low, shift);
			high = _mm256_slli_epi16(high, shift);
			
			return _mm256_packs_epi16(low, high);
		}
		
		void operator>>=(const int shift) {values = operator>>(shift).values;}
		void operator<<=(const int shift) {values = operator<<(shift).values;}
		
		SIMD_AVX<char> operator&(const int v) const {return _mm256_and_si256(values, _mm256_set1_epi8(v));}
		SIMD_AVX<char> operator&(const SIMD_AVX<char>& other) const {return _mm256_and_si256(values, other.values);}
		
		void operator&=(const int v) {values = operator&(v).values;}
		void operator&=(const SIMD_AVX<char>& other) {values = operator&(other).values;}
		
		SIMD_AVX<char> bitwiseAndNot(const int v) const {return _mm256_andnot_si256(values, _mm256_set1_epi8(v));}
		SIMD_AVX<char> bitwiseAndNot(const SIMD_AVX<char>& other) const {return _mm256_andnot_si256(values, other.values);}
		
		void bitwiseAndNot(const int v) {values = _mm256_andnot_si256(values, _mm256_set1_epi8(v));}
		void bitwiseAndNot(const SIMD_AVX<char>& other) {values = _mm256_andnot_si256(values, other.values);}
		
		//comparison
		SIMD_AVX<char> operator>(const unsigned char byte) const {return _mm256_cmpgt_epi8(values, _mm256_set1_epi8(byte));}
		SIMD_AVX<char> operator>(const SIMD_AVX<char>& other) const {return _mm256_cmpgt_epi8(values, other.values);}
		
		SIMD_AVX<char> operator<(const unsigned char byte) const {return _mm256_cmpgt_epi8(_mm256_set1_epi8(byte), values);}
		SIMD_AVX<char> operator<(const SIMD_AVX<char>& other) const {return _mm256_cmpgt_epi8(other.values, values);}
		
		SIMD_AVX<char> operator==(const unsigned char byte) const {return _mm256_cmpeq_epi8(values, _mm256_set1_epi8(byte));}
		SIMD_AVX<char> operator==(const SIMD_AVX<char>& other) const {return _mm256_cmpeq_epi8(values, other.values);}
		
		SIMD_AVX<char> operator!=(const unsigned char byte) const 
		{
			__m256i temp = _mm256_cmpeq_epi8(values, _mm256_set1_epi8(byte));
			return _mm256_andnot_si256(temp, temp); //does not bitwise not
		}
		SIMD_AVX<char> operator!=(const SIMD_AVX<char>& other) const
		{
			__m256i temp = _mm256_cmpeq_epi8(values, other.values);
			return _mm256_andnot_si256(temp, temp); //does not bitwise not
		}
		
		//special case functions
		SIMD_AVX<char> horizontalAdd(const SIMD_AVX<char>& other) const 
		{
			//shuffle then add
			const __m256i shuffle1 = _mm256_set_epi8(30, 28, 26, 24, 22, 20, 18, 16, 14, 12, 10, 8, 6, 4, 2, 0, 30, 28, 26, 24, 22, 20, 18, 16, 14, 12, 10, 8, 6, 4, 2, 0);
			const __m256i shuffle2 = _mm256_set_epi8(31, 29, 27, 25, 23, 21, 19, 17, 15, 13, 11, 9, 7, 5, 3, 1, 31, 29, 27, 25, 23, 21, 19, 17, 15, 13, 11, 9, 7, 5, 3, 1);
			__m256i temp = _mm256_shuffle_epi8(values, shuffle1); //A0, A2, A4, A6, A8, A10, A12, A14, A0, A2, A4, A6, A8, A10, A12, A14
			__m256i temp2 = _mm256_shuffle_epi8(other.values, shuffle2); //A1, A3, A5, A7, A9, A11, A13, A15, A1, A3, A5, A7, A9, A11, A13, A15
			return _mm256_add_epi8(temp, temp2);
		}
		short sum() const
		{
			//sum of all items into the largest datatype NEEDED to avoid overflow.
			//Ensures no overflow
			short temp[16];
			__m256i low = _mm256_cvtepi8_epi16(_mm256_extracti128_si256(values, 0)); //(A1, ..., A16)
			__m256i high = _mm256_cvtepi8_epi16(_mm256_extracti128_si256(values, 1)); //(A17, ... , A32)

			//add 16 bit values
			__m256i result = _mm256_add_epi16(low, high); //(A1+A17, ... , A16+A32). Pairs of 2
			result = _mm256_hadd_epi16(result, result);//(A1+A17+A2+A18, ... , A15+A31+A16+A32, duplicates). Pairs of 4
			result = _mm256_hadd_epi16(result, result);//Pairs of 8
			result = _mm256_hadd_epi16(result, result);//Pairs of 16
			result = _mm256_hadd_epi16(result, result);//Pairs of 32
			
			_mm256_storeu_si256((__m256i*)temp, result);
			return temp[0] + temp[1];
		}

		__m256i values;

	private:
	};

	template<>
	class SIMD_AVX<unsigned char> : public SIMD_AVX<char>
	{
	public:
		SIMD_AVX(){}
		SIMD_AVX(unsigned char byte){values = _mm256_set1_epi8(byte);}
		SIMD_AVX(__m256i byte){values = byte;}
		
		SIMD_AVX(const SIMD_AVX<unsigned char>& other) {values = other.values;}
		void operator=(const SIMD_AVX<unsigned char>& other) {values = other.values;}
		
		SIMD_AVX(const SIMD_AVX<unsigned char>&& other) noexcept {values = other.values;}
		void operator=(const SIMD_AVX<unsigned char>&& other) noexcept {values = other.values;}
		
		~SIMD_AVX(){}

		SIMD_AVX<unsigned char> operator*(const SIMD_AVX<unsigned char>& other) const
		{
			//convert to shorts -> multiply -> convert back
			__m256i lowShortsA = _mm256_cvtepu8_epi16(_mm256_extracti128_si256(values, 0));
			__m256i highShortsA = _mm256_cvtepu8_epi16(_mm256_extracti128_si256(values, 1));
			__m256i lowShortsB = _mm256_cvtepu8_epi16(_mm256_extracti128_si256(other.values, 0));
			__m256i highShortsB = _mm256_cvtepu8_epi16(_mm256_extracti128_si256(other.values, 1));

			__m256i lowShortMult = _mm256_mullo_epi16(lowShortsA, lowShortsB);
			__m256i highShortMult = _mm256_mullo_epi16(highShortsA, highShortsB);
			
			return _mm256_packus_epi16(lowShortMult, highShortMult);
		}
		SIMD_AVX<unsigned char> operator/(const SIMD_AVX<unsigned char>& other) const
		{
			//doesn't really exist here. Potential operations require AVX or casting to a higher datatype.
			//Just cast to float -> divide -> cast back
			__m128i A1_Extracted128 = _mm256_extracti128_si256(values, 0);
			__m128i A2_Extracted128 = _mm256_extracti128_si256(values, 1);
			__m128i B1_Extracted128 = _mm256_extracti128_si256(other.values, 0);
			__m128i B2_Extracted128 = _mm256_extracti128_si256(other.values, 1);
			
			__m256 A1 = _mm256_cvtepi32_ps(_mm256_cvtepu8_epi32(A1_Extracted128));
			__m256 B1 = _mm256_cvtepi32_ps(_mm256_cvtepu8_epi32(B1_Extracted128));
			
			__m256 A2 = _mm256_cvtepi32_ps(_mm256_cvtepu8_epi32(_mm_srli_si128(A1_Extracted128, 8)));
			__m256 B2 = _mm256_cvtepi32_ps(_mm256_cvtepu8_epi32(_mm_srli_si128(B1_Extracted128, 8)));
			
			__m256 A3 = _mm256_cvtepi32_ps(_mm256_cvtepu8_epi32(A2_Extracted128));
			__m256 B3 = _mm256_cvtepi32_ps(_mm256_cvtepu8_epi32(B2_Extracted128));

			__m256 A4 = _mm256_cvtepi32_ps(_mm256_cvtepu8_epi32(_mm_srli_si128(A2_Extracted128, 8)));
			__m256 B4 = _mm256_cvtepi32_ps(_mm256_cvtepu8_epi32(_mm_srli_si128(B2_Extracted128, 8)));

			__m256 res1 = _mm256_div_ps(A1, B1);
			__m256 res2 = _mm256_div_ps(A2, B2);
			__m256 res3 = _mm256_div_ps(A3, B3);
			__m256 res4 = _mm256_div_ps(A4, B4);
			
			__m256i output1 = _mm256_cvtps_epi32(res1);
			__m256i output2 = _mm256_cvtps_epi32(res2);
			__m256i output3 = _mm256_cvtps_epi32(res3);
			__m256i output4 = _mm256_cvtps_epi32(res4);

			__m256i pack16Low = _mm256_packus_epi32(output1, output2);
			__m256i pack16High = _mm256_packus_epi32(output3, output4);

			return _mm256_packus_epi16(pack16Low, pack16High);
		}

		SIMD_AVX<unsigned char> operator>>(const int shift) const 
		{
			__m256i low = _mm256_cvtepu8_epi16(_mm256_extracti128_si256(values, 0));
			__m256i high = _mm256_cvtepu8_epi16(_mm256_extracti128_si256(values, 1));
			low = _mm256_srli_epi16(low, shift);
			high = _mm256_srli_epi16(high, shift);
			
			return _mm256_packus_epi16(low, high);
		}
		SIMD_AVX<unsigned char> operator<<(const int shift) const
		{
			__m256i low = _mm256_cvtepu8_epi16(_mm256_extracti128_si256(values, 0));
			__m256i high = _mm256_cvtepu8_epi16(_mm256_extracti128_si256(values, 1));
			low = _mm256_slli_epi16(low, shift);
			high = _mm256_slli_epi16(high, shift);
			
			return _mm256_packus_epi16(low, high);
		}
		
		SIMD_AVX<unsigned char> operator>(const unsigned char byte) const
		{
			__m256i data = _mm256_set1_epi8(byte);
			return _mm256_cmpeq_epi8(_mm256_max_epu8(values, data), values);
		}
		SIMD_AVX<unsigned char> operator>(const SIMD_AVX<unsigned char>& other) const
		{
			return _mm256_cmpeq_epi8(_mm256_max_epu8(values, other.values), values);
		}
		
		SIMD_AVX<unsigned char> operator<(const unsigned char byte) const
		{
			__m256i data = _mm256_set1_epi8(byte);
			return _mm256_cmpeq_epi8(_mm256_min_epu8(values, data), values);
		}
		SIMD_AVX<unsigned char> operator<(const SIMD_AVX<unsigned char>& other) const
		{
			return _mm256_cmpeq_epi8(_mm256_min_epu8(values, other.values), values);
		}

		unsigned short sum() const
		{
			//sum of all items into the largest datatype NEEDED to avoid overflow.
			//Ensures no overflow
			unsigned short temp[16];
			__m256i low = _mm256_cvtepu8_epi16(_mm256_extracti128_si256(values, 0)); //(A1, ..., A16)
			__m256i high = _mm256_cvtepu8_epi16(_mm256_extracti128_si256(values, 1)); //(A17, ... , A32)
			
			__m256i result = _mm256_add_epi16(low, high); //(A1+A17, ... , A16+A32). Pairs of 2
			result = _mm256_hadd_epi16(result, result);//(A1+A17+A2+A18, ... , A15+A31+A16+A32, duplicates). Pairs of 4
			result = _mm256_hadd_epi16(result, result);//Pairs of 8
			result = _mm256_hadd_epi16(result, result);//Pairs of 16
			result = _mm256_hadd_epi16(result, result);//Pairs of 32
			
			_mm256_storeu_si256((__m256i*)temp, result);
			return temp[0] + temp[1];
		}

		//load / store
		static SIMD_AVX<unsigned char>load(unsigned char* pointer){return _mm256_loadu_si256((__m256i*)pointer);}
		void store(unsigned char* pointer){_mm256_storeu_si256((__m256i*)pointer, values);}
	};
	#endif
}