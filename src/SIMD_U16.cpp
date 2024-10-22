#include "SIMD_16.h"

#if (OPTI >= 1)
unsigned long long SIMD_128_U16::getSIMDBound(unsigned long long s)
{
	return (s>>3)<<3;
}

SIMD_128_U16::SIMD_128_U16()
{

}
SIMD_128_U16::SIMD_128_U16(unsigned short byte)
{
    values = _mm_set1_epi16(byte);
}


SIMD_128_U16::SIMD_128_U16(__m128i sseValue)
{
    values = sseValue;
}

SIMD_128_U16::SIMD_128_U16(const SIMD_128_U16& other)
{
    this->values = other.values;
}
void SIMD_128_U16::operator=(const SIMD_128_U16& other)
{
    this->values = other.values;
}

SIMD_128_U16::SIMD_128_U16(const SIMD_128_U16&& other) noexcept
{
    this->values = other.values;
}
void SIMD_128_U16::operator=(const SIMD_128_U16&& other) noexcept
{
    this->values = other.values;
}

SIMD_128_U16::~SIMD_128_U16()
{

}

//load / store
SIMD_128_U16 SIMD_128_U16::load(unsigned short* pointer)
{
    SIMD_128_U16 v;
    v.values = _mm_loadu_si128((__m128i*)pointer);
    return v;
}
void SIMD_128_U16::store(unsigned short* pointer)
{
    _mm_storeu_si128((__m128i*)pointer, values);
}

//arithmetic
SIMD_128_U16 SIMD_128_U16::operator+(const SIMD_128_U16& other) const
{
    return _mm_add_epi16(values, other.values);
}
SIMD_128_U16 SIMD_128_U16::operator-(const SIMD_128_U16& other) const
{
    return _mm_sub_epi16(values, other.values);
}
SIMD_128_U16 SIMD_128_U16::operator*(const SIMD_128_U16& other) const
{
    return _mm_mullo_epi16(values, other.values);
}
SIMD_128_U16 SIMD_128_U16::operator/(const SIMD_128_U16& other) const
{
    //doesn't really exist here. Potential operations require AVX or casting to a higher datatype.
    //Just cast to float -> divide -> cast back
    __m128 A1 = _mm_cvtepi32_ps(_mm_cvtepu16_epi32(values));
    __m128 B1 = _mm_cvtepi32_ps(_mm_cvtepu16_epi32(other.values));
    
    __m128 A2 = _mm_cvtepi32_ps(_mm_cvtepu16_epi32(_mm_srli_si128(values, 8)));
    __m128 B2 = _mm_cvtepi32_ps(_mm_cvtepu16_epi32(_mm_srli_si128(other.values, 8)));

    __m128 res1 = _mm_div_ps(A1, B1);
    __m128 res2 = _mm_div_ps(A2, B2);
    
    __m128i output1 = _mm_cvtps_epi32(res1);
    __m128i output2 = _mm_cvtps_epi32(res2);

    __m128i pack16Low = _mm_packus_epi32(output1, output2);

    return pack16Low;
}

void SIMD_128_U16::operator+=(const SIMD_128_U16& other)
{
    values = _mm_add_epi16(values, other.values);
}
void SIMD_128_U16::operator-=(const SIMD_128_U16& other)
{
    values = _mm_sub_epi16(values, other.values);
}
void SIMD_128_U16::operator*=(const SIMD_128_U16& other)
{
    values = _mm_mullo_epi16(values, other.values);
}
void SIMD_128_U16::operator/=(const SIMD_128_U16& other)
{
    //doesn't really exist here. Potential operations require AVX or casting to a higher datatype.
    //Just cast to float -> divide -> cast back
    __m128 A1 = _mm_cvtepi32_ps(_mm_cvtepu16_epi32(values));
    __m128 B1 = _mm_cvtepi32_ps(_mm_cvtepu16_epi32(other.values));
    
    __m128 A2 = _mm_cvtepi32_ps(_mm_cvtepu16_epi32(_mm_srli_si128(values, 8)));
    __m128 B2 = _mm_cvtepi32_ps(_mm_cvtepu16_epi32(_mm_srli_si128(other.values, 8)));

    __m128 res1 = _mm_div_ps(A1, B1);
    __m128 res2 = _mm_div_ps(A2, B2);
    
    __m128i output1 = _mm_cvtps_epi32(res1);
    __m128i output2 = _mm_cvtps_epi32(res2);

    values = _mm_packus_epi32(output1, output2);
}

//memory access and modification


//bitwise
SIMD_128_U16 SIMD_128_U16::operator>>(const int shift) const
{
    return _mm_srli_epi16(values, shift);
}
SIMD_128_U16 SIMD_128_U16::operator<<(const int shift) const
{
    return _mm_slli_epi16(values, shift);
}
SIMD_128_U16 SIMD_128_U16::operator>>(const SIMD_128_U16& other) const
{
    return _mm_srl_epi16(values, other.values);
}
SIMD_128_U16 SIMD_128_U16::operator<<(const SIMD_128_U16& other) const
{
    return _mm_sll_epi16(values, other.values);
}

void SIMD_128_U16::operator>>=(const int shift)
{
    values = _mm_srli_epi16(values, shift);
}
void SIMD_128_U16::operator<<=(const int shift)
{
    values = _mm_slli_epi16(values, shift);
}
void SIMD_128_U16::operator>>=(const SIMD_128_U16& other)
{
    values = _mm_srl_epi16(values, other.values);
}
void SIMD_128_U16::operator<<=(const SIMD_128_U16& other)
{
    values = _mm_sll_epi16(values, other.values);
}

SIMD_128_U16 SIMD_128_U16::operator&(const int v) const
{
    return _mm_and_si128(values, _mm_set1_epi16(v));
}
SIMD_128_U16 SIMD_128_U16::operator&(const SIMD_128_U16& other) const
{
    return _mm_and_si128(values, other.values);
}

void SIMD_128_U16::operator&=(const int v)
{
    values = _mm_and_si128(values, _mm_set1_epi16(v));
}
void SIMD_128_U16::operator&=(const SIMD_128_U16& other)
{
    values = _mm_and_si128(values, other.values);
}

SIMD_128_U16 SIMD_128_U16::bitwiseAndNot(const int v) const
{
    return _mm_andnot_si128(values, _mm_set1_epi16(v));
}
SIMD_128_U16 SIMD_128_U16::bitwiseAndNot(const SIMD_128_U16& other) const
{
    return _mm_andnot_si128(values, other.values);
}

void SIMD_128_U16::bitwiseAndNot(const int v)
{
    values = _mm_andnot_si128(values, _mm_set1_epi16(v));
}
void SIMD_128_U16::bitwiseAndNot(const SIMD_128_U16& other)
{
    values = _mm_andnot_si128(values, other.values);
}

//comparison
SIMD_128_U16 SIMD_128_U16::operator>(const unsigned short byte) const
{
    __m128i data = _mm_set1_epi16(byte);
    return _mm_cmpeq_epi16(_mm_max_epu16(values, data), values);
}
SIMD_128_U16 SIMD_128_U16::operator>(const SIMD_128_U16& other) const
{
    return _mm_cmpeq_epi16(_mm_max_epu16(values, other.values), other.values);
}

SIMD_128_U16 SIMD_128_U16::operator<(const unsigned short byte) const
{
    __m128i data = _mm_set1_epi16(byte);
    return _mm_cmpeq_epi16(_mm_min_epu16(values, data), values);
}
SIMD_128_U16 SIMD_128_U16::operator<(const SIMD_128_U16& other) const
{
    return _mm_cmpeq_epi16(_mm_min_epu16(values, other.values), other.values);
}

SIMD_128_U16 SIMD_128_U16::operator==(const unsigned short byte) const
{
    __m128i b = _mm_set1_epi16(byte);
    return _mm_cmpeq_epi16(values, b);
}
SIMD_128_U16 SIMD_128_U16::operator==(const SIMD_128_U16& other) const
{
    return _mm_cmpeq_epi16(values, other.values);
}

SIMD_128_U16 SIMD_128_U16::operator!=(const unsigned short byte) const
{
    __m128i temp = _mm_cmpeq_epi16(values, _mm_set1_epi16(byte));
    return _mm_andnot_si128(temp, temp); //does not bitwise not
}
SIMD_128_U16 SIMD_128_U16::operator!=(const SIMD_128_U16& other) const
{
    __m128i temp = _mm_cmpeq_epi16(values, other.values);
    return _mm_andnot_si128(temp, temp); //does not bitwise not
}

#endif