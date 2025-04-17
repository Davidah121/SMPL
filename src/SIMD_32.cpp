#include "SIMD_32.h"

#if (OPTI >= 1)
unsigned long long SIMD_128_32::getSIMDBound(unsigned long long s)
{
	return (s>>2)<<2;
}

SIMD_128_32::SIMD_128_32()
{

}
SIMD_128_32::SIMD_128_32(int byte)
{
    values = _mm_set1_epi32(byte);
}


SIMD_128_32::SIMD_128_32(__m128i sseValue)
{
    values = sseValue;
}

SIMD_128_32::SIMD_128_32(const SIMD_128_32& other)
{
    this->values = other.values;
}
void SIMD_128_32::operator=(const SIMD_128_32& other)
{
    this->values = other.values;
}

SIMD_128_32::SIMD_128_32(const SIMD_128_32&& other) noexcept
{
    this->values = other.values;
}
void SIMD_128_32::operator=(const SIMD_128_32&& other) noexcept
{
    this->values = other.values;
}

SIMD_128_32::~SIMD_128_32()
{

}

//load / store
SIMD_128_32 SIMD_128_32::load(int* pointer)
{
    SIMD_128_32 v;
    v.values = _mm_loadu_si128((__m128i*)pointer);
    return v;
}
void SIMD_128_32::store(int* pointer)
{
    _mm_storeu_si128((__m128i*)pointer, values);
}

//arithmetic
SIMD_128_32 SIMD_128_32::operator+(const SIMD_128_32& other) const
{
    return _mm_add_epi32(values, other.values);
}
SIMD_128_32 SIMD_128_32::operator-(const SIMD_128_32& other) const
{
    return _mm_sub_epi32(values, other.values);
}
SIMD_128_32 SIMD_128_32::operator-() const
{
    const __m128i allZero = _mm_set1_epi32(0);
    return _mm_sub_epi32(allZero, values);
}
SIMD_128_32 SIMD_128_32::operator*(const SIMD_128_32& other) const
{
    return _mm_mullo_epi32(values, other.values);
}
SIMD_128_32 SIMD_128_32::operator/(const SIMD_128_32& other) const
{
    //doesn't really exist here. Potential operations require AVX or casting to a higher datatype.
    //Just cast to float -> divide -> cast back
    __m128 A1 = _mm_cvtepi32_ps(values);
    __m128 B1 = _mm_cvtepi32_ps(other.values);
    __m128 res1 = _mm_div_ps(A1, B1);
    return _mm_cvtps_epi32(res1);
}

void SIMD_128_32::operator+=(const SIMD_128_32& other)
{
    values = _mm_add_epi32(values, other.values);
}
void SIMD_128_32::operator-=(const SIMD_128_32& other)
{
    values = _mm_sub_epi32(values, other.values);
}
void SIMD_128_32::operator*=(const SIMD_128_32& other)
{
    values = _mm_mullo_epi32(values, other.values);
}
void SIMD_128_32::operator/=(const SIMD_128_32& other)
{
    //doesn't really exist here. Potential operations require AVX or casting to a higher datatype.
    //Just cast to float -> divide -> cast back
    __m128 A1 = _mm_cvtepi32_ps(values);
    __m128 B1 = _mm_cvtepi32_ps(other.values);
    __m128 res1 = _mm_div_ps(A1, B1);
    values = _mm_cvtps_epi32(res1);
}

//memory access and modification


//bitwise
SIMD_128_32 SIMD_128_32::operator>>(const int shift) const
{
    return _mm_srli_epi32(values, shift);
}
SIMD_128_32 SIMD_128_32::operator<<(const int shift) const
{
    return _mm_slli_epi32(values, shift);
}
SIMD_128_32 SIMD_128_32::operator>>(const SIMD_128_32& other) const
{
    return _mm_srl_epi32(values, other.values);
}
SIMD_128_32 SIMD_128_32::operator<<(const SIMD_128_32& other) const
{
    return _mm_sll_epi32(values, other.values);
}

void SIMD_128_32::operator>>=(const int shift)
{
    values = _mm_srli_epi32(values, shift);
}
void SIMD_128_32::operator<<=(const int shift)
{
    values = _mm_slli_epi32(values, shift);
}
void SIMD_128_32::operator>>=(const SIMD_128_32& other)
{
    values = _mm_srl_epi32(values, other.values);
}
void SIMD_128_32::operator<<=(const SIMD_128_32& other)
{
    values = _mm_sll_epi32(values, other.values);
}

SIMD_128_32 SIMD_128_32::operator&(const int v) const
{
    return _mm_and_si128(values, _mm_set1_epi32(v));
}
SIMD_128_32 SIMD_128_32::operator&(const SIMD_128_32& other) const
{
    return _mm_and_si128(values, other.values);
}

void SIMD_128_32::operator&=(const int v)
{
    values = _mm_and_si128(values, _mm_set1_epi32(v));
}
void SIMD_128_32::operator&=(const SIMD_128_32& other)
{
    values = _mm_and_si128(values, other.values);
}

SIMD_128_32 SIMD_128_32::bitwiseAndNot(const int v) const
{
    return _mm_andnot_si128(values, _mm_set1_epi32(v));
}
SIMD_128_32 SIMD_128_32::bitwiseAndNot(const SIMD_128_32& other) const
{
    return _mm_andnot_si128(values, other.values);
}

void SIMD_128_32::bitwiseAndNot(const int v)
{
    values = _mm_andnot_si128(values, _mm_set1_epi32(v));
}
void SIMD_128_32::bitwiseAndNot(const SIMD_128_32& other)
{
    values = _mm_andnot_si128(values, other.values);
}

//comparison
SIMD_128_32 SIMD_128_32::operator>(const int byte) const
{
    return _mm_cmpgt_epi32(values, _mm_set1_epi32(byte));
}
SIMD_128_32 SIMD_128_32::operator>(const SIMD_128_32& other) const
{
    return _mm_cmpgt_epi32(values, other.values);
}

SIMD_128_32 SIMD_128_32::operator<(const int byte) const
{
    return _mm_cmplt_epi32(values, _mm_set1_epi32(byte));
}
SIMD_128_32 SIMD_128_32::operator<(const SIMD_128_32& other) const
{
    return _mm_cmplt_epi32(values, other.values);
}

SIMD_128_32 SIMD_128_32::operator==(const int byte) const
{
    __m128i b = _mm_set1_epi32(byte);
    return _mm_cmpeq_epi32(values, b);
}
SIMD_128_32 SIMD_128_32::operator==(const SIMD_128_32& other) const
{
    return _mm_cmpeq_epi32(values, other.values);
}

SIMD_128_32 SIMD_128_32::operator!=(const int byte) const
{
    __m128i temp = _mm_cmpeq_epi32(values, _mm_set1_epi32(byte));
    return _mm_andnot_si128(temp, temp); //does not bitwise not
}
SIMD_128_32 SIMD_128_32::operator!=(const SIMD_128_32& other) const
{
    __m128i temp = _mm_cmpeq_epi32(values, other.values);
    return _mm_andnot_si128(temp, temp); //does not bitwise not
}

SIMD_128_32 SIMD_128_32::horizontalAdd(const SIMD_128_32& other) const
{
    return _mm_hadd_epi32(values, other.values);
}
long long SIMD_128_32::sum() const
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

#endif