#pragma once
#include "StandardTypes.h"
#include <emmintrin.h>
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
	class SIMD_SSE<short>
	{
	public:
		static const int SIZE = 8;
		static unsigned long long getSIMDBound(unsigned long long s) {return (s>>3)<<3;}

		SIMD_SSE(){}
		SIMD_SSE(short byte){values = _mm_set1_epi16(byte);}
		SIMD_SSE(short v1, short v2, short v3, short v4, short v5, short v6, short v7, short v8){values = _mm_set_epi16(v8, v7, v6, v5, v4, v3, v2, v1);}
		SIMD_SSE(__m128i byte){values = byte;}
		
		SIMD_SSE(const SIMD_SSE<short>& other) {values = other.values;}
		void operator=(const SIMD_SSE<short>& other) {values = other.values;}
		
		SIMD_SSE(const SIMD_SSE<short>&& other) noexcept {values = other.values;}
		void operator=(const SIMD_SSE<short>&& other) noexcept {values = other.values;}
		
		~SIMD_SSE(){}

		//load / store
		static SIMD_SSE<short>load(const short* pointer){return _mm_loadu_si128((__m128i*)pointer);}
		void store(short* pointer) const {_mm_storeu_si128((__m128i*)pointer, values);}
		
		//arithmetic
		SIMD_SSE<short> operator-() const {return _mm_sub_epi16(_mm_set1_epi16(0), values);}
		SIMD_SSE<short> operator+(const SIMD_SSE<short>& other) const {return _mm_add_epi16(values, other.values);}
		SIMD_SSE<short> operator-(const SIMD_SSE<short>& other) const {return _mm_sub_epi16(values, other.values);}
		SIMD_SSE<short> operator*(const SIMD_SSE<short>& other) const {return _mm_mullo_epi16(values, other.values);}

		SIMD_SSE<short> operator/(const SIMD_SSE<short>& other) const
		{
			
			//doesn't really exist here. Potential operations require AVX or casting to a higher datatype.
			//Just cast to float -> divide -> cast back
			__m128 A1 = _mm_cvtepi32_ps(_mm_cvtepi16_epi32(values));
			__m128 B1 = _mm_cvtepi32_ps(_mm_cvtepi16_epi32(other.values));
			
			__m128 A2 = _mm_cvtepi32_ps(_mm_cvtepi16_epi32(_mm_srli_si128(values, 8)));
			__m128 B2 = _mm_cvtepi32_ps(_mm_cvtepi16_epi32(_mm_srli_si128(other.values, 8)));

			__m128 res1 = _mm_div_ps(A1, B1);
			__m128 res2 = _mm_div_ps(A2, B2);
			
			__m128i output1 = SEML::floatToInt32(res1);
			__m128i output2 = SEML::floatToInt32(res2);

			__m128i pack16Low = _mm_packs_epi32(output1, output2);

			return pack16Low;
		}
		
		void operator+=(const SIMD_SSE<short>& other) {values = operator+(other).values;}
		void operator-=(const SIMD_SSE<short>& other) {values = operator-(other).values;}
		void operator*=(const SIMD_SSE<short>& other) {values = operator*(other).values;}
		void operator/=(const SIMD_SSE<short>& other) {values = operator/(other).values;}
		
		//memory access and modification
		
		//bitwise
		SIMD_SSE<short> operator>>(const int shift) const {return _mm_srli_epi16(values, shift);}
		SIMD_SSE<short> operator<<(const int shift) const {return _mm_slli_epi16(values, shift);}
		
		void operator>>=(const int shift) {values = operator>>(shift).values;}
		void operator<<=(const int shift) {values = operator<<(shift).values;}
		
		SIMD_SSE<short> operator&(const SIMD_SSE<short>& other) const {return _mm_and_si128(values, other.values);}
		void operator&=(const SIMD_SSE<short>& other) {values = operator&(other).values;}
		
		SIMD_SSE<short> bitwiseAndNot(const SIMD_SSE<short>& other) const {return _mm_andnot_si128(values, other.values);}
		
		//comparison
		SIMD_SSE<short> operator>(const SIMD_SSE<short>& other) const {return _mm_cmpgt_epi16(values, other.values);}
		SIMD_SSE<short> operator>=(const SIMD_SSE<short>& other) const
		{
			__m128i temp = operator<(other).values;
			return _mm_andnot_si128(temp, temp);
		}
		
		SIMD_SSE<short> operator<(const SIMD_SSE<short>& other) const {return _mm_cmplt_epi16(values, other.values);}
		SIMD_SSE<short> operator<=(const SIMD_SSE<short>& other) const
		{
			__m128i temp = operator>(other).values;
			return _mm_andnot_si128(temp, temp);
		}

		SIMD_SSE<short> operator==(const SIMD_SSE<short>& other) const {return _mm_cmpeq_epi16(values, other.values);}
		SIMD_SSE<short> operator!=(const SIMD_SSE<short>& other) const
		{
			__m128i temp = _mm_cmpeq_epi16(values, other.values);
			return _mm_andnot_si128(temp, temp); //does not bitwise not
		}

		//requires that blendFactor is either 0x0000 or 0xFFFF to work properly which I believe is required already
		SIMD_SSE<short> blend(const SIMD_SSE<short>& other, const SIMD_SSE<short>& blendFactor) const
		{
			return _mm_blendv_epi8(values, other.values, blendFactor.values);
		}
		
		//special case functions
		SIMD_SSE<short> horizontalAdd(const SIMD_SSE<short>& other) const 
		{
			return _mm_hadd_epi16(values, other.values);
		}
		int sum() const
		{
			//sum of all items into the largest datatype NEEDED to avoid overflow.
			//Ensures no overflow
			int temp[4];
			__m128i low = _mm_cvtepi16_epi32(values); //(A1, A2, A3, A4)
			__m128i high = _mm_cvtepi16_epi32(_mm_srli_si128(values, 8)); //(A5, A6, A7, A8)

			//add 32 bit values
			__m128i result = _mm_add_epi32(low, high); //(A1+A5, A2+A6, A3+A7, A4+A8)
			result = _mm_hadd_epi32(result, result); //(A1+A5+A2+A6, A3+A7+A4+A8, duplicates)
			_mm_storeu_si128((__m128i*)temp, result);
			return temp[0] + temp[1];
		}
		
		//additional arithmetic functions
		SIMD_SSE<short> max(const SIMD_SSE<short>& other) const
		{
			return _mm_max_epi16(values, other.values);
		}
		SIMD_SSE<short> min(const SIMD_SSE<short>& other) const
		{
			return _mm_min_epi16(values, other.values);
		}
		SIMD_SSE<short> clamp(const SIMD_SSE<short>& min, const SIMD_SSE<short>& max) const
		{
			return _mm_min_epi16(_mm_max_epi16(values, min.values), max.values);
		}

		operator SIMD_SSE<uint16_t>() const;
		__m128i values;
	};
	
	template<>
	inline SIMD_SSE<short> min(const SIMD_SSE<short>& a, const SIMD_SSE<short>& b)
	{
		return a.min(b);
	}
	template<>
	inline SIMD_SSE<short> max(const SIMD_SSE<short>& a, const SIMD_SSE<short>& b)
	{
		return a.max(b);
	}

	template<>
	class SIMD_SSE<uint16_t> : public SIMD_SSE<short>
	{
	public:
		SIMD_SSE(){}
		SIMD_SSE(uint16_t byte){values = _mm_set1_epi16(byte);}
		SIMD_SSE(uint16_t v1, uint16_t v2, uint16_t v3, uint16_t v4, uint16_t v5, uint16_t v6, uint16_t v7, uint16_t v8){values = _mm_set_epi16(v8, v7, v6, v5, v4, v3, v2, v1);}
		SIMD_SSE(__m128i byte){values = byte;}
		
		SIMD_SSE(const SIMD_SSE<uint16_t>& other) {values = other.values;}
		void operator=(const SIMD_SSE<uint16_t>& other) {values = other.values;}
		
		SIMD_SSE(const SIMD_SSE<uint16_t>&& other) noexcept {values = other.values;}
		void operator=(const SIMD_SSE<uint16_t>&& other) noexcept {values = other.values;}
		
		~SIMD_SSE(){}

		SIMD_SSE<uint16_t> operator/(const SIMD_SSE<uint16_t>& other) const
		{
			//doesn't really exist here. Potential operations require AVX or casting to a higher datatype.
			//Just cast to float -> divide -> cast back
			__m128 A1 = _mm_cvtepi32_ps(_mm_cvtepu16_epi32(values));
			__m128 B1 = _mm_cvtepi32_ps(_mm_cvtepu16_epi32(other.values));
			
			__m128 A2 = _mm_cvtepi32_ps(_mm_cvtepu16_epi32(_mm_srli_si128(values, 8)));
			__m128 B2 = _mm_cvtepi32_ps(_mm_cvtepu16_epi32(_mm_srli_si128(other.values, 8)));

			__m128 res1 = _mm_div_ps(A1, B1);
			__m128 res2 = _mm_div_ps(A2, B2);
			
			__m128i output1 = SEML::floatToInt32(res1);
			__m128i output2 = SEML::floatToInt32(res2);

			__m128i pack16Low = _mm_packus_epi32(output1, output2);

			return pack16Low;
		}
		
		SIMD_SSE<uint16_t> operator>(const SIMD_SSE<uint16_t>& other) const
		{
			return _mm_cmpeq_epi16(_mm_max_epu16(values, other.values), values);
		}
		SIMD_SSE<uint16_t> operator>=(const SIMD_SSE<uint16_t>& other) const
		{
			__m128i temp = operator<(other).values;
			return _mm_andnot_si128(temp, temp);
		}
		
		SIMD_SSE<uint16_t> operator<(const SIMD_SSE<uint16_t>& other) const
		{
			return _mm_cmpeq_epi16(_mm_min_epu16(values, other.values), values);
		}
		SIMD_SSE<uint16_t> operator<=(const SIMD_SSE<uint16_t>& other) const
		{
			__m128i temp = operator>(other).values;
			return _mm_andnot_si128(temp, temp);
		}

		//requires that blendFactor is either 0x0000 or 0xFFFF to work properly which I believe is required already
		SIMD_SSE<uint16_t> blend(const SIMD_SSE<uint16_t>& other, const SIMD_SSE<uint16_t>& blendFactor) const
		{
			return _mm_blendv_epi8(values, other.values, blendFactor.values);
		}

		unsigned int sum() const
		{
			//sum of all items into the largest datatype NEEDED to avoid overflow.
			//Ensures no overflow
			unsigned int temp[4];
			__m128i low = _mm_cvtepu16_epi32(values); //(A1, A2, A3, A4)
			__m128i high = _mm_cvtepu16_epi32(_mm_srli_si128(values, 8)); //(A5, A6, A7, A8)

			//add 32 bit values
			__m128i result = _mm_add_epi32(low, high); //(A1+A5, A2+A6, A3+A7, A4+A8)
			result = _mm_hadd_epi32(result, result); //(A1+A5+A2+A6, A3+A7+A4+A8, duplicates)
			_mm_storeu_si128((__m128i*)temp, result);
			return temp[0] + temp[1];
		}
		
		//additional arithmetic functions
		SIMD_SSE<uint16_t> max(const SIMD_SSE<uint16_t>& other) const
		{
			return _mm_max_epu16(values, other.values);
		}
		SIMD_SSE<uint16_t> min(const SIMD_SSE<uint16_t>& other) const
		{
			return _mm_min_epu16(values, other.values);
		}
		SIMD_SSE<uint16_t> clamp(const SIMD_SSE<uint16_t>& min, const SIMD_SSE<uint16_t>& max) const
		{
			return _mm_min_epu16(_mm_max_epu16(values, min.values), max.values);
		}

		//load / store
		static SIMD_SSE<uint16_t>load(const uint16_t* pointer){return _mm_loadu_si128((__m128i*)pointer);}
		void store(uint16_t* pointer) const {_mm_storeu_si128((__m128i*)pointer, values);}
	};
	
	template<>
	inline SIMD_SSE<uint16_t> min(const SIMD_SSE<uint16_t>& a, const SIMD_SSE<uint16_t>& b)
	{
		return a.min(b);
	}
	template<>
	inline SIMD_SSE<uint16_t> max(const SIMD_SSE<uint16_t>& a, const SIMD_SSE<uint16_t>& b)
	{
		return a.max(b);
	}

	#endif


	#ifdef __AVX2__
	template<>
	class SIMD_AVX<short>
	{
	public:
		static const int SIZE = 16;
		static unsigned long long getSIMDBound(unsigned long long s) {return (s>>4)<<4;}

		SIMD_AVX(){}
		SIMD_AVX(short byte){values = _mm256_set1_epi16(byte);}
		SIMD_AVX(short v1, short v2, short v3, short v4, short v5, short v6, short v7, short v8,
				short v9, short v10, short v11, short v12, short v13, short v14, short v15, short v16){values = _mm256_set_epi16(v16,v15,v14,v13,v12,v11,v10,v9,v8, v7, v6, v5, v4, v3, v2, v1);}
		SIMD_AVX(__m256i byte){values = byte;}
		
		SIMD_AVX(const SIMD_AVX<short>& other) {values = other.values;}
		void operator=(const SIMD_AVX<short>& other) {values = other.values;}
		
		SIMD_AVX(const SIMD_AVX<short>&& other) noexcept {values = other.values;}
		void operator=(const SIMD_AVX<short>&& other) noexcept {values = other.values;}
		
		~SIMD_AVX(){}

		//load / store
		static SIMD_AVX<short>load(const short* pointer){return _mm256_loadu_si256((__m256i*)pointer);}
		void store(short* pointer) const {_mm256_storeu_si256((__m256i*)pointer, values);}
		
		//arithmetic
		SIMD_AVX<short> operator-() const {return _mm256_sub_epi16(_mm256_set1_epi16(0), values);}
		SIMD_AVX<short> operator+(const SIMD_AVX<short>& other) const {return _mm256_add_epi16(values, other.values);}
		SIMD_AVX<short> operator-(const SIMD_AVX<short>& other) const {return _mm256_sub_epi16(values, other.values);}

		SIMD_AVX<short> operator*(const SIMD_AVX<short>& other) const {return _mm256_mullo_epi16(values, other.values);}
		SIMD_AVX<short> operator/(const SIMD_AVX<short>& other) const
		{
			//Just cast to float -> divide -> cast back
			__m256 A1 = _mm256_cvtepi32_ps(_mm256_cvtepi16_epi32(_mm256_extracti128_si256(values, 0)));
			__m256 B1 = _mm256_cvtepi32_ps(_mm256_cvtepi16_epi32(_mm256_extracti128_si256(other.values, 0)));
			
			__m256 A2 = _mm256_cvtepi32_ps(_mm256_cvtepi16_epi32(_mm256_extracti128_si256(values, 1)));
			__m256 B2 = _mm256_cvtepi32_ps(_mm256_cvtepi16_epi32(_mm256_extracti128_si256(other.values, 1)));

			__m256 res1 = _mm256_div_ps(A1, B1);
			__m256 res2 = _mm256_div_ps(A2, B2);
			
			__m256i output1 = SEML::floatToInt32(res1);
			__m256i output2 = SEML::floatToInt32(res2);

			__m256i pack16Low = _mm256_packs_epi32(output1, output2);

			return _mm256_permute4x64_epi64(pack16Low, 0b11011000);
		}
		
		void operator+=(const SIMD_AVX<short>& other) {values = operator+(other).values;}
		void operator-=(const SIMD_AVX<short>& other) {values = operator-(other).values;}
		void operator*=(const SIMD_AVX<short>& other) {values = operator*(other).values;}
		void operator/=(const SIMD_AVX<short>& other) {values = operator/(other).values;}
		
		//memory access and modification
		
		//bitwise
		SIMD_AVX<short> operator>>(const int shift) const {return _mm256_srli_epi16(values, shift);}
		SIMD_AVX<short> operator<<(const int shift) const {return _mm256_slli_epi16(values, shift);}
		
		void operator>>=(const int shift) {values = operator>>(shift).values;}
		void operator<<=(const int shift) {values = operator<<(shift).values;}
		
		SIMD_AVX<short> operator&(const SIMD_AVX<short>& other) const {return _mm256_and_si256(values, other.values);}
		void operator&=(const SIMD_AVX<short>& other) {values = operator&(other).values;}
		
		SIMD_AVX<short> bitwiseAndNot(const SIMD_AVX<short>& other) const {return _mm256_andnot_si256(values, other.values);}
		
		//comparison
		SIMD_AVX<short> operator>(const SIMD_AVX<short>& other) const {return _mm256_cmpgt_epi16(values, other.values);}
		SIMD_AVX<short> operator>=(const SIMD_AVX<short>& other) const
		{
			__m256i temp = operator<(other).values;
			return _mm256_andnot_si256(temp, temp);
		}

		SIMD_AVX<short> operator<(const SIMD_AVX<short>& other) const {return _mm256_cmpgt_epi16(other.values, values);}
		SIMD_AVX<short> operator<=(const SIMD_AVX<short>& other) const
		{
			__m256i temp = operator>(other).values;
			return _mm256_andnot_si256(temp, temp);
		}
		
		SIMD_AVX<short> operator==(const SIMD_AVX<short>& other) const {return _mm256_cmpeq_epi16(values, other.values);}
		SIMD_AVX<short> operator!=(const SIMD_AVX<short>& other) const
		{
			__m256i temp = _mm256_cmpeq_epi16(values, other.values);
			return _mm256_andnot_si256(temp, temp); //does not bitwise not
		}

		//requires that blendFactor is either 0x0000 or 0xFFFF to work properly which I believe is required already
		SIMD_AVX<short> blend(const SIMD_AVX<short>& other, const SIMD_AVX<short>& blendFactor) const
		{
			return _mm256_blendv_epi8(values, other.values, blendFactor.values);
		}
		
		//special case functions
		SIMD_AVX<short> horizontalAdd(const SIMD_AVX<short>& other) const {return _mm256_hadd_epi16(values, other.values);}
		int sum() const
		{
			//sum of all items into the largest datatype NEEDED to avoid overflow.
			//Ensures no overflow
			int temp[8];
			__m256i low = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(values, 0)); //(A1, A2, A3, A4, A5, A6, A7, A8)
			__m256i high = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(values, 1)); //(A9, A10, A11, A12, A13, A14, A15, A16)

			//add 32 bit values
			__m256i result = _mm256_add_epi32(low, high); //(A1+A9, ... , A8+A16) Pairs of 2
			result = _mm256_hadd_epi32(result, result); //Pairs of 4
			result = _mm256_hadd_epi32(result, result); //Pairs of 8
			_mm256_storeu_si256((__m256i*)temp, result);
			return temp[0] + temp[4];
		}
		
		//additional arithmetic functions
		SIMD_AVX<short> max(const SIMD_AVX<short>& other) const
		{
			return _mm256_max_epi16(values, other.values);
		}
		SIMD_AVX<short> min(const SIMD_AVX<short>& other) const
		{
			return _mm256_min_epi16(values, other.values);
		}
		SIMD_AVX<short> clamp(const SIMD_AVX<short>& min, const SIMD_AVX<short>& max) const
		{
			return _mm256_min_epi16(_mm256_max_epi16(values, min.values), max.values);
		}

		operator SIMD_AVX<uint16_t>() const;
		__m256i values;

	private:
	};
	
	template<>
	inline SIMD_AVX<short> min(const SIMD_AVX<short>& a, const SIMD_AVX<short>& b)
	{
		return a.min(b);
	}
	template<>
	inline SIMD_AVX<short> max(const SIMD_AVX<short>& a, const SIMD_AVX<short>& b)
	{
		return a.max(b);
	}

	template<>
	class SIMD_AVX<uint16_t> : public SIMD_AVX<short>
	{
	public:
		SIMD_AVX(){}
		SIMD_AVX(uint16_t byte){values = _mm256_set1_epi16(byte);}
		SIMD_AVX(uint16_t v1, uint16_t v2, uint16_t v3, uint16_t v4, uint16_t v5, uint16_t v6, uint16_t v7, uint16_t v8,
				uint16_t v9, uint16_t v10, uint16_t v11, uint16_t v12, uint16_t v13, uint16_t v14, uint16_t v15, uint16_t v16){values = _mm256_set_epi16(v16,v15,v14,v13,v12,v11,v10,v9,v8, v7, v6, v5, v4, v3, v2, v1);}
		SIMD_AVX(__m256i byte){values = byte;}
		
		SIMD_AVX(const SIMD_AVX<uint16_t>& other) {values = other.values;}
		void operator=(const SIMD_AVX<uint16_t>& other) {values = other.values;}
		
		SIMD_AVX(const SIMD_AVX<uint16_t>&& other) noexcept {values = other.values;}
		void operator=(const SIMD_AVX<uint16_t>&& other) noexcept {values = other.values;}
		
		~SIMD_AVX(){}

		SIMD_AVX<uint16_t> operator/(const SIMD_AVX<uint16_t>& other) const
		{
			//Just cast to float -> divide -> cast back
			__m256 A1 = _mm256_cvtepi32_ps(_mm256_cvtepu16_epi32(_mm256_extracti128_si256(values, 0)));
			__m256 B1 = _mm256_cvtepi32_ps(_mm256_cvtepu16_epi32(_mm256_extracti128_si256(other.values, 0)));
			
			__m256 A2 = _mm256_cvtepi32_ps(_mm256_cvtepu16_epi32(_mm256_extracti128_si256(values, 1)));
			__m256 B2 = _mm256_cvtepi32_ps(_mm256_cvtepu16_epi32(_mm256_extracti128_si256(other.values, 1)));

			__m256 res1 = _mm256_div_ps(A1, B1);
			__m256 res2 = _mm256_div_ps(A2, B2);
			
			__m256i output1 = SEML::floatToInt32(res1);
			__m256i output2 = SEML::floatToInt32(res2);
			
			__m256i pack16Low = _mm256_packus_epi32(output1, output2);

			return _mm256_permute4x64_epi64(pack16Low, 0b11011000);
		}
		
		SIMD_AVX<uint16_t> operator>(const SIMD_AVX<uint16_t>& other) const
		{
			return _mm256_cmpeq_epi16(_mm256_max_epu16(values, other.values), values);
		}
		SIMD_AVX<uint16_t> operator>=(const SIMD_AVX<uint16_t>& other) const
		{
			__m256i temp = operator<(other).values;
			return _mm256_andnot_si256(temp, temp);
		}

		SIMD_AVX<uint16_t> operator<(const SIMD_AVX<uint16_t>& other) const
		{
			return _mm256_cmpeq_epi16(_mm256_min_epu16(values, other.values), values);
		}
		SIMD_AVX<uint16_t> operator<=(const SIMD_AVX<uint16_t>& other) const
		{
			__m256i temp = operator>(other).values;
			return _mm256_andnot_si256(temp, temp);
		}
		//requires that blendFactor is either 0x0000 or 0xFFFF to work properly which I believe is required already
		SIMD_AVX<uint16_t> blend(const SIMD_AVX<uint16_t>& other, const SIMD_AVX<uint16_t>& blendFactor) const
		{
			return _mm256_blendv_epi8(values, other.values, blendFactor.values);
		}

		unsigned int sum() const
		{
			//sum of all items into the largest datatype NEEDED to avoid overflow.
			//Ensures no overflow
			unsigned int temp[8];
			__m256i low = _mm256_cvtepu16_epi32(_mm256_extracti128_si256(values, 0)); //(A1, A2, A3, A4, A5, A6, A7, A8)
			__m256i high = _mm256_cvtepu16_epi32(_mm256_extracti128_si256(values, 1)); //(A9, A10, A11, A12, A13, A14, A15, A16)

			//add 32 bit values
			__m256i result = _mm256_add_epi32(low, high); //(A1+A9, ... , A8+A16) Pairs of 2
			result = _mm256_hadd_epi32(result, result); //Pairs of 4
			result = _mm256_hadd_epi32(result, result); //Pairs of 8
			_mm256_storeu_si256((__m256i*)temp, result);
			return temp[0] + temp[4];
		}
		
		//additional arithmetic functions
		SIMD_AVX<uint16_t> max(const SIMD_AVX<uint16_t>& other) const
		{
			return _mm256_max_epu16(values, other.values);
		}
		SIMD_AVX<uint16_t> min(const SIMD_AVX<uint16_t>& other) const
		{
			return _mm256_min_epu16(values, other.values);
		}
		SIMD_AVX<uint16_t> clamp(const SIMD_AVX<uint16_t>& min, const SIMD_AVX<uint16_t>& max) const
		{
			return _mm256_min_epu16(_mm256_max_epu16(values, min.values), max.values);
		}

		//load / store
		static SIMD_AVX<uint16_t>load(const uint16_t* pointer){return _mm256_loadu_si256((__m256i*)pointer);}
		void store(uint16_t* pointer) const {_mm256_storeu_si256((__m256i*)pointer, values);}
	};
	
	template<>
	inline SIMD_AVX<uint16_t> min(const SIMD_AVX<uint16_t>& a, const SIMD_AVX<uint16_t>& b)
	{
		return a.min(b);
	}
	template<>
	inline SIMD_AVX<uint16_t> max(const SIMD_AVX<uint16_t>& a, const SIMD_AVX<uint16_t>& b)
	{
		return a.max(b);
	}
	#endif
}