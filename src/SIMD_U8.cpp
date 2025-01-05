#include "SIMD_8.h"

#if (OPTI >= 1)
unsigned long long SIMD_128_U8::getSIMDBound(unsigned long long s)
{
	return (s>>4)<<4;
}

SIMD_128_U8::SIMD_128_U8()
{

}
SIMD_128_U8::SIMD_128_U8(unsigned char byte)
{
    values = _mm_set1_epi8(byte);
}


SIMD_128_U8::SIMD_128_U8(__m128i sseValue)
{
    values = sseValue;
}

SIMD_128_U8::SIMD_128_U8(const SIMD_128_U8& other)
{
    this->values = other.values;
}
void SIMD_128_U8::operator=(const SIMD_128_U8& other)
{
    this->values = other.values;
}

SIMD_128_U8::SIMD_128_U8(const SIMD_128_U8&& other) noexcept
{
    this->values = other.values;
}
void SIMD_128_U8::operator=(const SIMD_128_U8&& other) noexcept
{
    this->values = other.values;
}

SIMD_128_U8::~SIMD_128_U8()
{

}

//load / store
SIMD_128_U8 SIMD_128_U8::load(unsigned char* pointer)
{
    SIMD_128_U8 v;
    v.values = _mm_loadu_si128((__m128i*)pointer);
    return v;
}
void SIMD_128_U8::store(unsigned char* pointer)
{
    _mm_storeu_si128((__m128i*)pointer, values);
}

//arithmetic
SIMD_128_U8 SIMD_128_U8::operator+(const SIMD_128_U8& other) const
{
    return _mm_add_epi8(values, other.values);
}
SIMD_128_U8 SIMD_128_U8::operator-(const SIMD_128_U8& other) const
{
    return _mm_sub_epi8(values, other.values);
}
SIMD_128_U8 SIMD_128_U8::operator*(const SIMD_128_U8& other) const
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
SIMD_128_U8 SIMD_128_U8::operator/(const SIMD_128_U8& other) const
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
    
    __m128i output1 = _mm_cvtps_epi32(res1);
    __m128i output2 = _mm_cvtps_epi32(res2);
    __m128i output3 = _mm_cvtps_epi32(res3);
    __m128i output4 = _mm_cvtps_epi32(res4);

    __m128i pack16Low = _mm_packus_epi32(output1, output2);
    __m128i pack16High = _mm_packus_epi32(output3, output4);

    return _mm_packus_epi16(pack16Low, pack16High);
}

void SIMD_128_U8::operator+=(const SIMD_128_U8& other)
{
    values = _mm_add_epi8(values, other.values);
}
void SIMD_128_U8::operator-=(const SIMD_128_U8& other)
{
    values = _mm_sub_epi8(values, other.values);
}
void SIMD_128_U8::operator*=(const SIMD_128_U8& other)
{
    //convert to shorts -> multiply -> convert back
    __m128i lowShortsA = _mm_cvtepu8_epi16(values);
    __m128i highShortsA = _mm_cvtepu8_epi16(_mm_srli_si128(values, 8));
    __m128i lowShortsB = _mm_cvtepu8_epi16(other.values);
    __m128i highShortsB = _mm_cvtepu8_epi16(_mm_srli_si128(other.values, 8));

    __m128i lowShortMult = _mm_mullo_epi16(lowShortsA, lowShortsB);
    __m128i highShortMult = _mm_mullo_epi16(highShortsA, highShortsB);
    
    values = _mm_packus_epi16(lowShortMult, highShortMult);
}
void SIMD_128_U8::operator/=(const SIMD_128_U8& other)
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
    
    __m128i output1 = _mm_cvtps_epi32(res1);
    __m128i output2 = _mm_cvtps_epi32(res2);
    __m128i output3 = _mm_cvtps_epi32(res3);
    __m128i output4 = _mm_cvtps_epi32(res4);

    __m128i pack16Low = _mm_packus_epi32(output1, output2);
    __m128i pack16High = _mm_packus_epi32(output3, output4);

    values = _mm_packus_epi16(pack16Low, pack16High);
}

//memory access and modification


//bitwise
SIMD_128_U8 SIMD_128_U8::operator>>(const int shift) const
{
    __m128i low = _mm_cvtepi8_epi16(values);
    __m128i high = _mm_cvtepi8_epi16(_mm_srli_si128(values, 8));
    low = _mm_srli_epi16(low, shift);
    high = _mm_srli_epi16(high, shift);
    
    return _mm_packus_epi16(low, high);
}
SIMD_128_U8 SIMD_128_U8::operator<<(const int shift) const
{
    __m128i low = _mm_cvtepi8_epi16(values);
    __m128i high = _mm_cvtepi8_epi16(_mm_srli_si128(values, 8));
    low = _mm_slli_epi16(low, shift);
    high = _mm_slli_epi16(high, shift);
    
    return _mm_packus_epi16(low, high);
}
SIMD_128_U8 SIMD_128_U8::operator>>(const SIMD_128_U8& other) const
{
    __m128i low = _mm_cvtepi8_epi16(values);
    __m128i high = _mm_cvtepi8_epi16(_mm_srli_si128(values, 8));
    
    __m128i lowShift = _mm_cvtepi8_epi16(other.values);
    __m128i highShift = _mm_cvtepi8_epi16(_mm_srli_si128(other.values, 8));
    low = _mm_srl_epi16(low, lowShift);
    high = _mm_srl_epi16(high, highShift);
    
    return _mm_packus_epi16(low, high);
}
SIMD_128_U8 SIMD_128_U8::operator<<(const SIMD_128_U8& other) const
{
    __m128i low = _mm_cvtepi8_epi16(values);
    __m128i high = _mm_cvtepi8_epi16(_mm_srli_si128(values, 8));
    
    __m128i lowShift = _mm_cvtepi8_epi16(other.values);
    __m128i highShift = _mm_cvtepi8_epi16(_mm_srli_si128(other.values, 8));
    low = _mm_sll_epi16(low, lowShift);
    high = _mm_sll_epi16(high, highShift);
    
    return _mm_packus_epi16(low, high);
}

void SIMD_128_U8::operator>>=(const int shift)
{
    __m128i low = _mm_cvtepi8_epi16(values);
    __m128i high = _mm_cvtepi8_epi16(_mm_srli_si128(values, 8));
    
    low = _mm_srli_epi16(low, shift);
    high = _mm_srli_epi16(high, shift);
    
    values = _mm_packus_epi16(low, high);
}
void SIMD_128_U8::operator<<=(const int shift)
{
    __m128i low = _mm_cvtepi8_epi16(values);
    __m128i high = _mm_cvtepi8_epi16(_mm_srli_si128(values, 8));
    
    low = _mm_slli_epi16(low, shift);
    high = _mm_slli_epi16(high, shift);
    
    values = _mm_packus_epi16(low, high);
}
void SIMD_128_U8::operator>>=(const SIMD_128_U8& other)
{
    __m128i low = _mm_cvtepi8_epi16(values);
    __m128i high = _mm_cvtepi8_epi16(_mm_srli_si128(values, 8));
    
    __m128i lowShift = _mm_cvtepi8_epi16(other.values);
    __m128i highShift = _mm_cvtepi8_epi16(_mm_srli_si128(other.values, 8));
    low = _mm_srl_epi16(low, lowShift);
    high = _mm_srl_epi16(high, highShift);
    
    values = _mm_packus_epi16(low, high);
}
void SIMD_128_U8::operator<<=(const SIMD_128_U8& other)
{
    __m128i low = _mm_cvtepi8_epi16(values);
    __m128i high = _mm_cvtepi8_epi16(_mm_srli_si128(values, 8));
    
    __m128i lowShift = _mm_cvtepi8_epi16(other.values);
    __m128i highShift = _mm_cvtepi8_epi16(_mm_srli_si128(other.values, 8));
    low = _mm_sll_epi16(low, lowShift);
    high = _mm_sll_epi16(high, highShift);
    
    values = _mm_packus_epi16(low, high);
}

SIMD_128_U8 SIMD_128_U8::operator&(const int v) const
{
    return _mm_and_si128(values, _mm_set1_epi8(v));
}
SIMD_128_U8 SIMD_128_U8::operator&(const SIMD_128_U8& other) const
{
    return _mm_and_si128(values, other.values);
}

void SIMD_128_U8::operator&=(const int v)
{
    values = _mm_and_si128(values, _mm_set1_epi8(v));
}
void SIMD_128_U8::operator&=(const SIMD_128_U8& other)
{
    values = _mm_and_si128(values, other.values);
}

SIMD_128_U8 SIMD_128_U8::bitwiseAndNot(const int v) const
{
    return _mm_andnot_si128(values, _mm_set1_epi8(v));
}
SIMD_128_U8 SIMD_128_U8::bitwiseAndNot(const SIMD_128_U8& other) const
{
    return _mm_andnot_si128(values, other.values);
}

void SIMD_128_U8::bitwiseAndNot(const int v)
{
    values = _mm_andnot_si128(values, _mm_set1_epi8(v));
}
void SIMD_128_U8::bitwiseAndNot(const SIMD_128_U8& other)
{
    values = _mm_andnot_si128(values, other.values);
}

//comparison
SIMD_128_U8 SIMD_128_U8::operator>(const unsigned char byte) const
{
    __m128i data = _mm_set1_epi8(byte);
    return _mm_cmpeq_epi8(_mm_max_epu8(values, data), values);
}
SIMD_128_U8 SIMD_128_U8::operator>(const SIMD_128_U8& other) const
{
    return _mm_cmpeq_epi8(_mm_max_epu8(values, other.values), other.values);
}

SIMD_128_U8 SIMD_128_U8::operator<(const unsigned char byte) const
{
    __m128i data = _mm_set1_epi8(byte);
    return _mm_cmpeq_epi8(_mm_min_epu8(values, data), values);
}
SIMD_128_U8 SIMD_128_U8::operator<(const SIMD_128_U8& other) const
{
    return _mm_cmpeq_epi8(_mm_min_epu8(values, other.values), other.values);
}

SIMD_128_U8 SIMD_128_U8::operator==(const unsigned char byte) const
{
    __m128i b = _mm_set1_epi8(byte);
    return _mm_cmpeq_epi8(values, b);
}
SIMD_128_U8 SIMD_128_U8::operator==(const SIMD_128_U8& other) const
{
    return _mm_cmpeq_epi8(values, other.values);
}

SIMD_128_U8 SIMD_128_U8::operator!=(const unsigned char byte) const
{
    __m128i temp = _mm_cmpeq_epi8(values, _mm_set1_epi8(byte));
    return _mm_andnot_si128(temp, temp); //does not bitwise not
}
SIMD_128_U8 SIMD_128_U8::operator!=(const SIMD_128_U8& other) const
{
    __m128i temp = _mm_cmpeq_epi8(values, other.values);
    return _mm_andnot_si128(temp, temp); //does not bitwise not
}

SIMD_128_U8 SIMD_128_U8::horizontalAdd(const SIMD_128_U8& other) const
{
    return sse8HorizontalAdd(values, other.values);
}

unsigned short SIMD_128_U8::sum() const
{
    //sum of all items into the largest datatype NEEDED to avoid overflow.
    //Ensures no overflow
    unsigned short temp[2];
    __m128i low = _mm_cvtepu8_epi16(values); //(A1, A2, A3, A4, A5, A6, A7, A8)
    __m128i high = _mm_cvtepu8_epi16(_mm_srli_si128(values, 8)); //(A9, A10, A11, A12, A13, A14, A15, A16)

    //add 16 bit values
    __m128i result = _mm_add_epi16(low, high); //(A1+A9, A2+A10, A3+A11, A4+A12, A5+A13, A6+A14, A7+A15, A8+A16)
    result = sse8HorizontalAdd(result, result); //(A1+A9+A2+A10, A3+A11+A4+A12, A5+A13+A6+A14, A7+A15+A8+A16, duplicates)
    result = sse8HorizontalAdd(result, result); //(A1+A9+A2+A10+A3+A11+A4+A12, A5+A13+A6+A14+A7+A15+A8+A16, duplicates)
    _mm_storeu_si128((__m128i*)temp, result);
    return temp[0] + temp[1];
}
#endif