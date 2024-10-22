#include "MathExt.h"

#if (OPTI >= 1)
namespace smpl
{
    __m128  MathExt::sin(__m128 a)
    {
        //solve between [0, pi/2]
        //flip across pi/2 for [0, pi]
        //negate if x < 0
        
    }
    __m128d MathExt::sin(__m128d a)
    {

    }

    __m128  MathExt::cos(__m128 a)
    {

    }
    __m128d MathExt::cos(__m128d a)
    {

    }

    __m128  MathExt::tan(__m128 a)
    {

    }
    __m128d MathExt::tan(__m128d a)
    {

    }

    __m128  MathExt::ln(__m128 a)
    {
        //let a = 1. Center around 1 such that ln(1) = 0
        //uses the ln(x + a) taylor series. Converges faster
        const int ONLY_EXPONENT = 0x7F800000;
        const __m128 LN2  = _mm_set1_ps(0.69314718056);
        const __m128 DIV1 = _mm_set1_ps(1.0/3.0);
        const __m128 DIV2 = _mm_set1_ps(1.0/5.0);
        const __m128 DIV3 = _mm_set1_ps(1.0/7.0);
        const __m128 DIV4 = _mm_set1_ps(1.0/9.0);
        const __m128 DIV5 = _mm_set1_ps(1.0/11.0);
        const __m128 DIV6 = _mm_set1_ps(1.0/13.0);

        //extract the base 2 exponent
        __m128i exponent = _mm_srli_epi32(TYPE_PUN(a, __m128i), 23);
        exponent = _mm_and_si128(exponent, _mm_set1_epi32(0xFF));
        exponent = _mm_sub_epi32(exponent, _mm_set1_epi32(127));
        __m128 finalAdjustment = _mm_mul_ps(_mm_cvtepi32_ps(exponent), LN2);
        __m128 exponentAdjust = _mm_and_ps(a, _mm_set1_ps(TYPE_PUN(ONLY_EXPONENT, float)));

        a = _mm_sub_ps(_mm_div_ps(a, exponentAdjust), _mm_set1_ps(1));
        __m128 y = _mm_div_ps(a, _mm_add_ps(a, _mm_set1_ps(2)));

        __m128 approx = y;
        __m128 ySqr = _mm_mul_ps(y, y);
        __m128 yExp = _mm_mul_ps(y, ySqr);

        approx = _mm_add_ps(approx, _mm_mul_ps(yExp, DIV1));
        yExp = _mm_mul_ps(yExp, ySqr);
        approx = _mm_add_ps(approx, _mm_mul_ps(yExp, DIV2));
        yExp = _mm_mul_ps(yExp, ySqr);
        approx = _mm_add_ps(approx, _mm_mul_ps(yExp, DIV3));
        yExp = _mm_mul_ps(yExp, ySqr);
        approx = _mm_add_ps(approx, _mm_mul_ps(yExp, DIV4));
        yExp = _mm_mul_ps(yExp, ySqr);
        approx = _mm_add_ps(approx, _mm_mul_ps(yExp, DIV5));
        yExp = _mm_mul_ps(yExp, ySqr);
        approx = _mm_add_ps(approx, _mm_mul_ps(yExp, DIV6));

        approx = _mm_mul_ps(approx, _mm_set1_ps(2));
        return _mm_add_ps(approx, finalAdjustment);
    }
    __m128d MathExt::ln(__m128d a)
    {
        // //let a = 1. Center around 1 such that ln(1) = 0
        // //uses the ln(x + a) taylor series. Converges faster
        // const long long ONLY_EXPONENT = 0x7FF0000000000000LL;
        // const __m128d LN2  = _mm_set1_pd(0.69314718056);
        // const __m128d DIV1 = _mm_set1_pd(1.0/3.0);
        // const __m128d DIV2 = _mm_set1_pd(1.0/5.0);
        // const __m128d DIV3 = _mm_set1_pd(1.0/7.0);
        // const __m128d DIV4 = _mm_set1_pd(1.0/9.0);
        // const __m128d DIV5 = _mm_set1_pd(1.0/11.0);
        // const __m128d DIV6 = _mm_set1_pd(1.0/13.0);

        // //extract the base 2 exponent
        // __m128i exponent = _mm_srli_epi64(TYPE_PUN(a, __m128i), 52);
        // exponent = _mm_and_si128(exponent, _mm_set1_epi64x(0b11111111111));
        // exponent = _mm_sub_epi64(exponent, _mm_set1_epi64x(1023));
        // __m128d finalAdjustment = _mm_mul_pd(_mm_cvtepi64_pd(exponent), LN2);
        // __m128d exponentAdjust = _mm_and_pd(a, _mm_set1_pd(TYPE_PUN(ONLY_EXPONENT, double)));

        // a = _mm_sub_ps(_mm_div_ps(a, exponentAdjust), _mm_set1_ps(1));
        // __m128 y = _mm_div_ps(a, _mm_add_ps(a, _mm_set1_ps(2)));

        // __m128 approx = y;
        // __m128 ySqr = _mm_mul_ps(y, y);
        // __m128 yExp = _mm_mul_ps(y, ySqr);

        // approx = _mm_add_ps(approx, _mm_mul_ps(yExp, DIV1));
        // yExp = _mm_mul_ps(yExp, ySqr);
        // approx = _mm_add_ps(approx, _mm_mul_ps(yExp, DIV2));
        // yExp = _mm_mul_ps(yExp, ySqr);
        // approx = _mm_add_ps(approx, _mm_mul_ps(yExp, DIV3));
        // yExp = _mm_mul_ps(yExp, ySqr);
        // approx = _mm_add_ps(approx, _mm_mul_ps(yExp, DIV4));
        // yExp = _mm_mul_ps(yExp, ySqr);
        // approx = _mm_add_ps(approx, _mm_mul_ps(yExp, DIV5));
        // yExp = _mm_mul_ps(yExp, ySqr);
        // approx = _mm_add_ps(approx, _mm_mul_ps(yExp, DIV6));

        // approx = _mm_mul_ps(approx, _mm_set1_ps(2));
        // return _mm_add_ps(approx, finalAdjustment);
        return _mm_set1_pd(0);
    }

    __m128  MathExt::log(__m128 a)
    {
        const __m128 adjustment = _mm_set1_ps(std::log(E));
        return _mm_mul_ps(MathExt::ln(a), adjustment);
    }
    __m128d MathExt::log(__m128d a)
    {
        const __m128d adjustment = _mm_set1_pd(std::log(E));
        return _mm_mul_pd(MathExt::ln(a), adjustment);
    }

    __m128  MathExt::log2(__m128 a)
    {
        const __m128 adjustment = _mm_set1_ps(std::log(E)/std::log(2));
        return _mm_mul_ps(MathExt::ln(a), adjustment);
    }
    __m128d MathExt::log2(__m128d a)
    {
        const __m128d adjustment = _mm_set1_pd(std::log(E)/std::log(2));
        return _mm_mul_pd(MathExt::ln(a), adjustment);
    }

    __m128  MathExt::log(__m128 a, float base)
    {
        __m128 adjustment = _mm_set1_ps(std::log(E) / std::log(base));
        return _mm_mul_ps(MathExt::ln(a), adjustment);
    }
    __m128d MathExt::log(__m128d a, float base)
    {
        __m128d adjustment = _mm_set1_pd(std::log(E) / std::log(base));
        return _mm_mul_pd(MathExt::ln(a), adjustment);
    }

    __m128  MathExt::exp(__m128 a)
    {

    }
    __m128d MathExt::exp(__m128d a)
    {

    }

    __m128  MathExt::pow(__m128 a, __m128 b)
    {
        //exp(y*ln(x))
        return exp( _mm_mul_ps(b, ln(a)) );
    }
    __m128d MathExt::pow(__m128d a, __m128d b)
    {
        return exp( _mm_mul_pd(b, ln(a)) );
    }
    
	__m128  MathExt::reciprocal(__m128 a)
    {
        return _mm_rcp_ps(a);
    }
	__m128d MathExt::reciprocal(__m128d a)
    {
        return _mm_div_pd(_mm_set1_pd(1), a); //need avx512 for _mm_rcp14_pd
    }

    __m128  MathExt::sqr(__m128 a)
    {
        return _mm_mul_ps(a, a);
    }
    __m128d MathExt::sqr(__m128d a)
    {
        return _mm_mul_pd(a, a);
    }

    __m128  MathExt::cube(__m128 a)
    {
        return _mm_mul_ps(_mm_mul_ps(a, a), a);
    }
    __m128d MathExt::cube(__m128d a)
    {
        return _mm_mul_pd(_mm_mul_pd(a, a), a);
    }

    __m128  MathExt::sqrt(__m128 a)
    {
        return _mm_sqrt_ps(a);
    }
    __m128d MathExt::sqrt(__m128d a)
    {
        return _mm_sqrt_pd(a);
    }

    __m128  MathExt::invSqrt(__m128 a)
    {
        return _mm_rsqrt_ps(a);
    }
    __m128d MathExt::invSqrt(__m128d a)
    {
        // return _mm_rsqrt_pd(a);
    }

    __m128  MathExt::cuberoot(__m128 a)
    {
        return MathExt::pow(a, _mm_set1_ps(1.0/3.0));
    }
    __m128d MathExt::cuberoot(__m128d a)
    {
        return MathExt::pow(a, _mm_set1_pd(1.0/3.0));
    }

    __m128  MathExt::arcsin(__m128 a)
    {

    }
    __m128d MathExt::arcsin(__m128d a)
    {

    }

    __m128  MathExt::arccos(__m128 a)
    {

    }
    __m128d MathExt::arccos(__m128d a)
    {

    }

    __m128  MathExt::arctan(__m128 a)
    {

    }
    __m128d MathExt::arctan(__m128d a)
    {

    }

    __m128  MathExt::toRad(__m128 a)
    {
        const __m128 convValue = _mm_set1_ps(PI/180.0);
        return _mm_mul_ps(a, convValue);
    }
    __m128d MathExt::toRad(__m128d a)
    {
        const __m128d convValue = _mm_set1_pd(PI/180.0);
        return _mm_mul_pd(a, convValue);
    }

    __m128  MathExt::toDeg(__m128 a)
    {
        const __m128 convValue = _mm_set1_ps(180.0/PI);
        return _mm_mul_ps(a, convValue);
    }
    __m128d MathExt::toDeg(__m128d a)
    {
        const __m128d convValue = _mm_set1_pd(180.0/PI);
        return _mm_mul_pd(a, convValue);
    }
}
#endif