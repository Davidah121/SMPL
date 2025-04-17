#include "SIMD_FP64.h"
#include "SEML.h"
#if (OPTI >= 1)
unsigned long long SIMD_128_FP64::getSIMDBound(unsigned long long s)
{
	return (s>>1)<<1;
}

SIMD_128_FP64::SIMD_128_FP64()
{

}
SIMD_128_FP64::SIMD_128_FP64(double val)
{
    values = _mm_set1_pd(val);
}

SIMD_128_FP64::SIMD_128_FP64(__m128d sseValue)
{
    values = sseValue;
}

SIMD_128_FP64::SIMD_128_FP64(const SIMD_128_FP64& other)
{
    this->values = other.values;
}
void SIMD_128_FP64::operator=(const SIMD_128_FP64& other)
{
    this->values = other.values;
}

SIMD_128_FP64::SIMD_128_FP64(const SIMD_128_FP64&& other) noexcept
{
    this->values = other.values;
}
void SIMD_128_FP64::operator=(const SIMD_128_FP64&& other) noexcept
{
    this->values = other.values;
}

SIMD_128_FP64::~SIMD_128_FP64()
{

}

//load / store
SIMD_128_FP64 SIMD_128_FP64::load(double* pointer)
{
    SIMD_128_FP64 v;
    v.values = _mm_loadu_pd(pointer);
    return v;
}
void SIMD_128_FP64::store(double* pointer)
{
    _mm_storeu_pd(pointer, values);
}

//arithmetic
SIMD_128_FP64 SIMD_128_FP64::operator+(const SIMD_128_FP64& other) const
{
    return _mm_add_pd(values, other.values);
}
SIMD_128_FP64 SIMD_128_FP64::operator-(const SIMD_128_FP64& other) const
{
    return _mm_sub_pd(values, other.values);
}
SIMD_128_FP64 SIMD_128_FP64::operator-() const
{
    return SEML::negate(values); //should be slightly faster than 0 - values
}
SIMD_128_FP64 SIMD_128_FP64::operator*(const SIMD_128_FP64& other) const
{
    return _mm_mul_pd(values, other.values);
}
SIMD_128_FP64 SIMD_128_FP64::operator/(const SIMD_128_FP64& other) const
{
    return _mm_div_pd(values, other.values);
}

void SIMD_128_FP64::operator+=(const SIMD_128_FP64& other)
{
    values = _mm_add_pd(values, other.values);
}
void SIMD_128_FP64::operator-=(const SIMD_128_FP64& other)
{
    values = _mm_sub_pd(values, other.values);
}
void SIMD_128_FP64::operator*=(const SIMD_128_FP64& other)
{
    values = _mm_mul_pd(values, other.values);
}
void SIMD_128_FP64::operator/=(const SIMD_128_FP64& other)
{
    values = _mm_div_pd(values, other.values);
}

//bitwise
SIMD_128_FP64 SIMD_128_FP64::operator&(const double v) const
{
    return _mm_and_pd(values, _mm_set1_pd(v));
}
SIMD_128_FP64 SIMD_128_FP64::operator&(const SIMD_128_FP64& other) const
{
    return _mm_and_pd(values, other.values);
}

void SIMD_128_FP64::operator&=(const double v)
{
    values = _mm_and_pd(values, _mm_set1_pd(v));
}
void SIMD_128_FP64::operator&=(const SIMD_128_FP64& other)
{
    values = _mm_and_pd(values, other.values);
}

SIMD_128_FP64 SIMD_128_FP64::bitwiseAndNot(const double v) const
{
    return _mm_andnot_pd(values, _mm_set1_pd(v));
}
SIMD_128_FP64 SIMD_128_FP64::bitwiseAndNot(const SIMD_128_FP64& other) const
{
    return _mm_andnot_pd(values, other.values);
}

void SIMD_128_FP64::bitwiseAndNot(const double v)
{
    values = _mm_andnot_pd(values, _mm_set1_pd(v));
}
void SIMD_128_FP64::bitwiseAndNot(const SIMD_128_FP64& other)
{
    values = _mm_andnot_pd(values, other.values);
}

//comparison
SIMD_128_FP64 SIMD_128_FP64::operator>(const double byte) const
{
    return _mm_cmpgt_pd(values, _mm_set1_pd(byte));
}
SIMD_128_FP64 SIMD_128_FP64::operator>(const SIMD_128_FP64& other) const
{
    return _mm_cmpgt_pd(values, other.values);
}

SIMD_128_FP64 SIMD_128_FP64::operator<(const double byte) const
{
    return _mm_cmplt_pd(values, _mm_set1_pd(byte));
}
SIMD_128_FP64 SIMD_128_FP64::operator<(const SIMD_128_FP64& other) const
{
    return _mm_cmplt_pd(values, other.values);
}

SIMD_128_FP64 SIMD_128_FP64::operator==(const double byte) const
{
    return _mm_cmpeq_pd(values, _mm_set1_pd(byte));
}
SIMD_128_FP64 SIMD_128_FP64::operator==(const SIMD_128_FP64& other) const
{
    return _mm_cmpeq_pd(values, other.values);
}

SIMD_128_FP64 SIMD_128_FP64::operator!=(const double byte) const
{
    return _mm_cmpneq_pd(values, _mm_set1_pd(byte));
}
SIMD_128_FP64 SIMD_128_FP64::operator!=(const SIMD_128_FP64& other) const
{
    return _mm_cmpneq_pd(values, other.values);
}

SIMD_128_FP64 SIMD_128_FP64::horizontalAdd(const SIMD_128_FP64& other) const
{
    return _mm_hadd_pd(values, other.values);
}
double SIMD_128_FP64::sum() const
{
    __m128d temp = _mm_hadd_pd(values, values); //contains A1+B1, A1+B1
    return _mm_cvtsd_f64(temp);
}
#endif