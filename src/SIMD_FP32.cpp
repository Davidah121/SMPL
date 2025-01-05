#include "SIMD_FP32.h"

#if (OPTI >= 1)
unsigned long long SIMD_128_FP32::getSIMDBound(unsigned long long s)
{
	return (s>>2)<<2;
}

SIMD_128_FP32::SIMD_128_FP32()
{

}
SIMD_128_FP32::SIMD_128_FP32(float val)
{
    values = _mm_set1_ps(val);
}

SIMD_128_FP32::SIMD_128_FP32(__m128 sseValue)
{
    values = sseValue;
}

SIMD_128_FP32::SIMD_128_FP32(const SIMD_128_FP32& other)
{
    this->values = other.values;
}
void SIMD_128_FP32::operator=(const SIMD_128_FP32& other)
{
    this->values = other.values;
}

SIMD_128_FP32::SIMD_128_FP32(const SIMD_128_FP32&& other) noexcept
{
    this->values = other.values;
}
void SIMD_128_FP32::operator=(const SIMD_128_FP32&& other) noexcept
{
    this->values = other.values;
}

SIMD_128_FP32::~SIMD_128_FP32()
{

}

//load / store
SIMD_128_FP32 SIMD_128_FP32::load(float* pointer)
{
    SIMD_128_FP32 v;
    v.values = _mm_loadu_ps(pointer);
    return v;
}
void SIMD_128_FP32::store(float* pointer)
{
    _mm_storeu_ps(pointer, values);
}

//arithmetic
SIMD_128_FP32 SIMD_128_FP32::operator+(const SIMD_128_FP32& other) const
{
    return _mm_add_ps(values, other.values);
}
SIMD_128_FP32 SIMD_128_FP32::operator-(const SIMD_128_FP32& other) const
{
    return _mm_sub_ps(values, other.values);
}
SIMD_128_FP32 SIMD_128_FP32::operator*(const SIMD_128_FP32& other) const
{
    return _mm_mul_ps(values, other.values);
}
SIMD_128_FP32 SIMD_128_FP32::operator/(const SIMD_128_FP32& other) const
{
    return _mm_div_ps(values, other.values);
}

void SIMD_128_FP32::operator+=(const SIMD_128_FP32& other)
{
    values = _mm_add_ps(values, other.values);
}
void SIMD_128_FP32::operator-=(const SIMD_128_FP32& other)
{
    values = _mm_sub_ps(values, other.values);
}
void SIMD_128_FP32::operator*=(const SIMD_128_FP32& other)
{
    values = _mm_mul_ps(values, other.values);
}
void SIMD_128_FP32::operator/=(const SIMD_128_FP32& other)
{
    values = _mm_div_ps(values, other.values);
}

//bitwise
SIMD_128_FP32 SIMD_128_FP32::operator&(const float v) const
{
    return _mm_and_ps(values, _mm_set1_ps(v));
}
SIMD_128_FP32 SIMD_128_FP32::operator&(const SIMD_128_FP32& other) const
{
    return _mm_and_ps(values, other.values);
}

void SIMD_128_FP32::operator&=(const float v)
{
    values = _mm_and_ps(values, _mm_set1_ps(v));
}
void SIMD_128_FP32::operator&=(const SIMD_128_FP32& other)
{
    values = _mm_and_ps(values, other.values);
}

SIMD_128_FP32 SIMD_128_FP32::bitwiseAndNot(const float v) const
{
    return _mm_andnot_ps(values, _mm_set1_ps(v));
}
SIMD_128_FP32 SIMD_128_FP32::bitwiseAndNot(const SIMD_128_FP32& other) const
{
    return _mm_andnot_ps(values, other.values);
}

void SIMD_128_FP32::bitwiseAndNot(const float v)
{
    values = _mm_andnot_ps(values, _mm_set1_ps(v));
}
void SIMD_128_FP32::bitwiseAndNot(const SIMD_128_FP32& other)
{
    values = _mm_andnot_ps(values, other.values);
}

//comparison
SIMD_128_FP32 SIMD_128_FP32::operator>(const float byte) const
{
    return _mm_cmpgt_ps(values, _mm_set1_ps(byte));
}
SIMD_128_FP32 SIMD_128_FP32::operator>(const SIMD_128_FP32& other) const
{
    return _mm_cmpgt_ps(values, other.values);
}

SIMD_128_FP32 SIMD_128_FP32::operator<(const float byte) const
{
    return _mm_cmplt_ps(values, _mm_set1_ps(byte));
}
SIMD_128_FP32 SIMD_128_FP32::operator<(const SIMD_128_FP32& other) const
{
    return _mm_cmplt_ps(values, other.values);
}

SIMD_128_FP32 SIMD_128_FP32::operator==(const float byte) const
{
    return _mm_cmpeq_ps(values, _mm_set1_ps(byte));
}
SIMD_128_FP32 SIMD_128_FP32::operator==(const SIMD_128_FP32& other) const
{
    return _mm_cmpeq_ps(values, other.values);
}

SIMD_128_FP32 SIMD_128_FP32::operator!=(const float byte) const
{
    return _mm_cmpneq_ps(values, _mm_set1_ps(byte));
}
SIMD_128_FP32 SIMD_128_FP32::operator!=(const SIMD_128_FP32& other) const
{
    return _mm_cmpneq_ps(values, other.values);
}


SIMD_128_FP32 SIMD_128_FP32::horizontalAdd(const SIMD_128_FP32& other) const
{
    return _mm_hadd_ps(values, other.values);
}
float SIMD_128_FP32::sum() const
{
    //https://stackoverflow.com/questions/6996764/fastest-way-to-do-horizontal-sse-vector-sum-or-other-reduction
    //faster than 2 hadd_ps. has a throughput faster by 2 than using hadd_ps
    __m128 shuf = _mm_movehdup_ps(values);
    __m128 sums = _mm_add_ps(values, shuf);
    shuf = _mm_movehl_ps(shuf, sums);
    sums = _mm_add_ps(sums, shuf);
    return _mm_cvtss_f32(sums);
}
#endif