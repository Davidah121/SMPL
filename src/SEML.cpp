#include "SEML.h"
#include <cmath>
#include <iostream>

namespace SEML
{
    #if (OPTI >= 1)
    __m128 packDoublesIntoFloat(__m128d x1, __m128d x2)
    {
        __m128 outputLow = _mm_cvtpd_ps(x1);
        __m128 outputHigh = _mm_cvtpd_ps(x2);

        return _mm_shuffle_ps(outputLow, outputHigh, 0b01000100);
    }

    __m128i fastDoubleToInt64(__m128d x)
    {
        x = _mm_add_pd(x, _mm_set1_pd(0x0018000000000000));
        return _mm_xor_si128(_mm_castpd_si128(x), _mm_castpd_si128(_mm_set1_pd(0x0018000000000000)));
    }
    __m128i fastDoubleToUInt64(__m128d x)
    {
        x = _mm_add_pd(x, _mm_set1_pd(0x0010000000000000));
        return _mm_xor_si128(_mm_castpd_si128(x), _mm_castpd_si128(_mm_set1_pd(0x0010000000000000)));
    }

    __m128d fastInt64ToDouble(__m128i x)
    {
        x = _mm_add_epi64(x, _mm_castpd_si128(_mm_set1_pd(0x0018000000000000)));
        return _mm_sub_pd(_mm_castsi128_pd(x), _mm_set1_pd(0x0018000000000000));
    }
    __m128d fastUInt64ToDouble(__m128i x)
    {
        x = _mm_or_si128(x, _mm_castpd_si128(_mm_set1_pd(0x0010000000000000)));
        return _mm_sub_pd(_mm_castsi128_pd(x), _mm_set1_pd(0x0010000000000000));
    }

    __m128d int64ToDouble(__m128i x)
    {
        __m128i xH = _mm_srai_epi32(x, 16);
        xH = _mm_blend_epi16(xH, _mm_setzero_si128(), 0x33);
        xH = _mm_add_epi64(xH, _mm_castpd_si128(_mm_set1_pd(442721857769029238784.)));              //  3*2^67
        __m128i xL = _mm_blend_epi16(x, _mm_castpd_si128(_mm_set1_pd(0x0010000000000000)), 0x88);   //  2^52
        __m128d f = _mm_sub_pd(_mm_castsi128_pd(xH), _mm_set1_pd(442726361368656609280.));          //  3*2^67 + 2^52
        return _mm_add_pd(f, _mm_castsi128_pd(xL));
    }
    __m128d uint64ToDouble(__m128i x)
    {
        __m128i xH = _mm_srli_epi64(x, 32);
        xH = _mm_or_si128(xH, _mm_castpd_si128(_mm_set1_pd(19342813113834066795298816.)));          //  2^84
        __m128i xL = _mm_blend_epi16(x, _mm_castpd_si128(_mm_set1_pd(0x0010000000000000)), 0xcc);   //  2^52
        __m128d f = _mm_sub_pd(_mm_castsi128_pd(xH), _mm_set1_pd(19342813118337666422669312.));     //  2^84 + 2^52
        return _mm_add_pd(f, _mm_castsi128_pd(xL));
    }

    //converts range to [-PI, PI]
    __m128 piRangeReduction(__m128 x)
    {
        const __m128 PI2DIV = _mm_set1_ps(1.0/SEML_PI2);
        const __m128 PISSE = _mm_set1_ps(SEML_PI);
        const __m128 PI2SSE = _mm_set1_ps(SEML_PI2);
        __m128 xMinusPI = _mm_sub_ps(x, PISSE);
        __m128 xMinus2PI = _mm_sub_ps(x, PI2SSE);
        __m128 subValue = _mm_mul_ps(PI2SSE, _mm_floor_ps(_mm_mul_ps(xMinusPI, PI2DIV)));

        return _mm_sub_ps(xMinus2PI, subValue);
    }
    __m128d piRangeReduction(__m128d x)
    {
        const __m128d PI2DIV = _mm_set1_pd(1.0/SEML_PI2);
        const __m128d PISSE = _mm_set1_pd(SEML_PI);
        const __m128d PI2SSE = _mm_set1_pd(SEML_PI2);
        __m128d xMinusPI = _mm_sub_pd(x, PISSE);
        __m128d xMinus2PI = _mm_sub_pd(x, PI2SSE);
        __m128d subValue = _mm_mul_pd(PI2SSE, _mm_floor_pd(_mm_mul_pd(xMinusPI, PI2DIV)));

        return _mm_sub_pd(xMinus2PI, subValue);
    }

    __m128 radToDeg(__m128 x)
    {
        const __m128 CONVERSION_MULT = _mm_set1_ps(180.0/SEML_PI);
        return _mm_mul_ps(x, CONVERSION_MULT);
    }
    __m128d radToDeg(__m128d x)
    {
        const __m128d CONVERSION_MULT = _mm_set1_pd(180.0/SEML_PI);
        return _mm_mul_pd(x, CONVERSION_MULT);
    }

    __m128 degToRad(__m128 x)
    {
        const __m128 CONVERSION_MULT = _mm_set1_ps(SEML_PI/180.0);
        return _mm_mul_ps(x, CONVERSION_MULT);
    }
    __m128d degToRad(__m128d x)
    {
        const __m128d CONVERSION_MULT = _mm_set1_pd(SEML_PI/180.0);
        return _mm_mul_pd(x, CONVERSION_MULT);
    }

    __m128 abs(__m128 x)
    {
        //NOTE: Only works with IEEE-754 single precision floats.
        //Should be valid on all x86 processors especially if it uses SSE / AVX / etc.
        const __m128i allButSignBit = _mm_set1_epi32(0x7FFFFFFF);
        return _mm_and_ps(x, _mm_castsi128_ps(allButSignBit));
    }
    __m128d abs(__m128d x)
    {
        //NOTE: Only works with IEEE-754 double precision floats.
        //Should be valid on all x86 processors especially if it uses SSE / AVX / etc.
        const __m128i allButSignBit = _mm_set1_epi64x(0x7FFFFFFFFFFFFFFF);
        return _mm_and_pd(x, _mm_castsi128_pd(allButSignBit));
    }

    __m128 negate(__m128 x)
    {
        //NOTE: Only works with IEEE-754 single precision floats.
        //Should be valid on all x86 processors especially if it uses SSE / AVX / etc.
        const __m128i onlySignBit = _mm_set1_epi32(0x80000000);
        return _mm_xor_ps(x, _mm_castsi128_ps(onlySignBit));
    }
    __m128d negate(__m128d x)
    {
        //NOTE: Only works with IEEE-754 double precision floats.
        //Should be valid on all x86 processors especially if it uses SSE / AVX / etc.
        const __m128i onlySignBit = _mm_set1_epi64x(0x8000000000000000);
        return _mm_xor_pd(x, _mm_castsi128_pd(onlySignBit));
    }

    __m128 sign(__m128 x)
    {
        __m128 signMultBlend = _mm_cmpge_ps(x, _mm_set1_ps(0));
        __m128 signMult = _mm_blendv_ps(_mm_set1_ps(-1), _mm_set1_ps(1), signMultBlend);
        return signMult;
    }

    __m128d sign(__m128d x)
    {
        __m128d signMultBlend = _mm_cmpge_pd(x, _mm_set1_pd(0));
        __m128d signMult = _mm_blendv_pd(_mm_set1_pd(-1), _mm_set1_pd(1), signMultBlend);
        return signMult;
    }
    
    __m128 reciprocal(__m128 a)
    {
        return _mm_div_ps(_mm_set1_ps(1), a);
    }
    __m128d reciprocal(__m128d a)
    {
        return _mm_div_pd(_mm_set1_pd(1), a);
    }
    __m128 fastReciprocal(__m128 a)
    {
        return _mm_rcp_ps(a);
    }

    __m128 sqr(__m128 x)
    {
        return _mm_mul_ps(x, x);
    }
    __m128d sqr(__m128d x)
    {
        return _mm_mul_pd(x, x);
    }
    
	__m128 cube(__m128 a)
    {
        return _mm_mul_ps(_mm_mul_ps(a, a), a);
    }
	__m128d cube(__m128d a)
    {
        return _mm_mul_pd(_mm_mul_pd(a, a), a);
    }

    __m128 sqrt(__m128 x)
    {
        return _mm_sqrt_ps(x);
    }
    __m128d sqrt(__m128d x)
    {
        return _mm_sqrt_pd(x);
    }
    
    __m128 invSqrt(__m128 a)
    {
        return reciprocal(sqrt(a));
    }
    __m128d invSqrt(__m128d a)
    {
        return reciprocal(sqrt(a));
    }
    
    __m128 fastInvSqrt(__m128 a)
    {
        return _mm_rsqrt_ps(a);
    }

    __m128d cosAround0(__m128d x)
    {
        const __m128d div1 = _mm_set1_pd(1.0/2);
        const __m128d div2 = _mm_set1_pd(1.0/24);
        const __m128d div3 = _mm_set1_pd(1.0/720);
        const __m128d div4 = _mm_set1_pd(1.0/40320);
        const __m128d div5 = _mm_set1_pd(1.0/362880);
        // const __m128d div6 = _mm_set1_pd(1.0/479001600);

        __m128d xSqr = _mm_mul_pd(x, x);
        __m128d numerator = xSqr;
        __m128d sum = _mm_set1_pd(1);

        sum = _mm_sub_pd(sum, _mm_mul_pd(numerator, div1));
        numerator = _mm_mul_pd(xSqr, numerator);
        sum = _mm_add_pd(sum, _mm_mul_pd(numerator, div2));
        numerator = _mm_mul_pd(xSqr, numerator);
        sum = _mm_sub_pd(sum, _mm_mul_pd(numerator, div3));
        numerator = _mm_mul_pd(xSqr, numerator);
        sum = _mm_add_pd(sum, _mm_mul_pd(numerator, div4));
        numerator = _mm_mul_pd(xSqr, numerator);
        sum = _mm_sub_pd(sum, _mm_mul_pd(numerator, div5));
        // numerator = _mm_mul_pd(xSqr, numerator);
        // sum = _mm_add_pd(sum, _mm_mul_pd(numerator, div6));
        return sum;
    }

    std::pair<__m128, __m128> sincos(__m128 x)
    {
        const __m128 PIHalfHalf = _mm_set1_ps(SEML_PI / 4);
        const __m128 PIHalf = _mm_set1_ps(SEML_PI / 2);
        const __m128 PIHalfReci = _mm_set1_ps(1.0 / (SEML_PI / 2));

        //reduce x into [-pi, pi]
        x = piRangeReduction(x);
        
        //record sign multipliers
        __m128 ltZero = _mm_cmplt_ps(x, _mm_set1_ps(0));
        __m128 gtPIHalf = _mm_cmpgt_ps(abs(x), PIHalf);
        __m128 sinSign = _mm_blendv_ps(_mm_set1_ps(1), _mm_set1_ps(-1), ltZero);
        __m128 cosSign = _mm_blendv_ps(_mm_set1_ps(1), _mm_set1_ps(-1), gtPIHalf);

        //reduce x into [-pi/4, pi/4].
        __m128 floorAmount = _mm_floor_ps( _mm_mul_ps(_mm_add_ps(x, PIHalfHalf), PIHalfReci));
        __m128i isEven = _mm_and_si128(_mm_cvtps_epi32(floorAmount), _mm_set1_epi32(1)); //do something with this. If even, its cosine
        __m128 isCos = _mm_castsi128_ps(_mm_cmpeq_epi32(isEven, _mm_set1_epi32(0)));
        
        __m128 xAdjusted = _mm_sub_ps(x, _mm_mul_ps(floorAmount, PIHalf));
        xAdjusted = abs(xAdjusted);

        //convert to double for higher accuracy
        __m128d lowHalf = _mm_cvtps_pd(xAdjusted);
        __m128d highHalf = _mm_cvtps_pd(_mm_castsi128_ps(_mm_srli_si128(_mm_castps_si128(xAdjusted), 8)));

        __m128d lowCosValue = cosAround0(lowHalf);
        __m128d highCosValue = cosAround0(highHalf);
        
        __m128d lowSinValue = sqrt(_mm_sub_pd(_mm_set1_pd(1), sqr(lowCosValue)));
        __m128d highSinValue = sqrt(_mm_sub_pd(_mm_set1_pd(1), sqr(highCosValue)));
        
        //convert back to floats
        __m128 cosValue = packDoublesIntoFloat(lowCosValue, highCosValue);
        __m128 sinValue = packDoublesIntoFloat(lowSinValue, highSinValue);

        //swap cosValue and sinValue depending on isCos.
        __m128 trueCosValue = _mm_blendv_ps(sinValue, cosValue, isCos);
        __m128 trueSinValue = _mm_blendv_ps(cosValue, sinValue, isCos);
        
        return {_mm_mul_ps(sinSign, trueSinValue), _mm_mul_ps(cosSign, trueCosValue)};
    }

    std::pair<__m128d, __m128d> sincos(__m128d x)
    {
        const __m128d PIHalfHalf = _mm_set1_pd(SEML_PI / 4);
        const __m128d PIHalf = _mm_set1_pd(SEML_PI / 2);
        const __m128d PIHalfReci = _mm_set1_pd(1.0 / (SEML_PI / 2));

        //reduce x into [-pi, pi]
        x = piRangeReduction(x);
        
        //record sign multipliers
        __m128d ltZero = _mm_cmplt_pd(x, _mm_set1_pd(0));
        __m128d gtPIHalf = _mm_cmpgt_pd(abs(x), PIHalf);
        __m128d sinSign = _mm_blendv_pd(_mm_set1_pd(1), _mm_set1_pd(-1), ltZero);
        __m128d cosSign = _mm_blendv_pd(_mm_set1_pd(1), _mm_set1_pd(-1), gtPIHalf);

        //reduce x into [-pi/4, pi/4].
        __m128d floorAmount = _mm_floor_pd( _mm_mul_pd(_mm_add_pd(x, PIHalfHalf), PIHalfReci));
        __m128i isEven = _mm_and_si128(fastDoubleToInt64(floorAmount), _mm_set1_epi64x(1)); //do something with this. If even, its cosine
        __m128d isCos = _mm_castsi128_pd(_mm_cmpeq_epi64(isEven, _mm_set1_epi64x(0)));
        
        __m128d xAdjusted = _mm_sub_pd(x, _mm_mul_pd(floorAmount, PIHalf));
        xAdjusted = abs(xAdjusted);

        //convert to double for higher accuracy
        __m128d cosValue = cosAround0(xAdjusted);
        __m128d sinValue = sqrt(_mm_sub_pd(_mm_set1_pd(1), sqr(cosValue)));

        //swap cosValue and sinValue depending on isCos.
        __m128d trueCosValue = _mm_blendv_pd(sinValue, cosValue, isCos);
        __m128d trueSinValue = _mm_blendv_pd(cosValue, sinValue, isCos);
        
        return {_mm_mul_pd(sinSign, trueSinValue), _mm_mul_pd(cosSign, trueCosValue)};
    }

    __m128 sin(__m128 x)
    {
        return sincos(x).first;
    }
    __m128d sin(__m128d x)
    {
        return sincos(x).first;
    }

    __m128 cos(__m128 x)
    {
        return sincos(x).second;
    }
    __m128d cos(__m128d x)
    {
        return sincos(x).second;
    }

    __m128 tan(__m128 x)
    {
        std::pair<__m128, __m128> values = sincos(x);
        return _mm_div_ps(values.first, values.second);
    }
    __m128d tan(__m128d x)
    {
        std::pair<__m128d, __m128d> values = sincos(x);
        return _mm_div_pd(values.first, values.second);
    }

    __m128 sec(__m128 x)
    {
        return _mm_rcp_ps(sin(x));
    }
    __m128d sec(__m128d x)
    {
        return _mm_div_pd(_mm_set1_pd(1), sin(x));
    }

    __m128 csc(__m128 x)
    {
        return _mm_rcp_ps(cos(x));
    }
    __m128d csc(__m128d x)
    {
        return _mm_div_pd(_mm_set1_pd(1), cos(x));
    }

    __m128 cot(__m128 x)
    {
        std::pair<__m128, __m128> values = sincos(x);
        return _mm_div_ps(values.second, values.first);
    }
    __m128d cot(__m128d x)
    {
        std::pair<__m128d, __m128d> values = sincos(x);
        return _mm_div_pd(values.second, values.first);
    }

    __m128d lnAround1(__m128d x)
    {
        //https://math.stackexchange.com/questions/977586/is-there-an-approximation-to-the-natural-log-function-at-large-values
        //Approximates the integral of x^t
        //Absolute error under the epsilon of single precision floating points (10^-6)
        //assume x is close to 1.
        const __m128d N1 = _mm_set1_pd(90);
        const __m128d D1 = _mm_set1_pd(7);
        const __m128d D2 = _mm_set1_pd(32);
        const __m128d D3 = _mm_set1_pd(12);
        const __m128d LN2 = _mm_set1_pd(0.69314718056);

        //rule -> ln(x) = ln(x/2) + ln(2)
        x = _mm_mul_pd(x, _mm_set1_pd(0.5));

        //more instructions than a polynomial but far more accurate
        //2 ops
        __m128d numerator = _mm_mul_pd(N1, _mm_sub_pd(x, _mm_set1_pd(1)));

        //2 ops of setup. potentially fast
        __m128d rootX = sqrt(x);
        __m128d fourthRoot = sqrt(rootX);
        
        //8 ops
        __m128d denominator = _mm_mul_pd(D1, fourthRoot);
        denominator = _mm_mul_pd(fourthRoot, _mm_add_pd(D2, denominator));
        denominator = _mm_mul_pd(fourthRoot, _mm_add_pd(D3, denominator));
        denominator = _mm_mul_pd(fourthRoot, _mm_add_pd(D2, denominator));
        denominator = _mm_add_pd(D1, denominator);

        __m128d result = _mm_div_pd(numerator, denominator); //1 op - slow
        return _mm_add_pd(result, LN2);
    }

    // __m128d lnAround1(__m128d x)
    // {
    //     //Taylor series around 1.5. more accurate but slower.
    //     const __m128d ln1_5 = _mm_set1_pd(0.405465108108); //ln(1.5)
    //     const __m128d div1 = _mm_set1_pd(1.0/3.0);
    //     const __m128d div2 = _mm_set1_pd(1.0/5.0);
    //     const __m128d div3 = _mm_set1_pd(1.0/7.0);
    //     const __m128d div4 = _mm_set1_pd(1.0/9.0);
    //     const __m128d div5 = _mm_set1_pd(1.0/11.0);
    //     const __m128d div6 = _mm_set1_pd(1.0/13.0);

    //     __m128d tempX = _mm_sub_pd(x, _mm_set1_pd(1.5));
    //     __m128d K = _mm_div_pd(tempX, _mm_add_pd(tempX, _mm_set1_pd(3)));
    //     __m128d tempK2 = sqr(K);
    //     __m128d result = _mm_add_pd(div5, _mm_mul_pd(tempK2, div6));
    //     result = _mm_add_pd(div4, _mm_mul_pd(tempK2, result));
    //     result = _mm_add_pd(div3, _mm_mul_pd(tempK2, result));
    //     result = _mm_add_pd(div2, _mm_mul_pd(tempK2, result));
    //     result = _mm_add_pd(div1, _mm_mul_pd(tempK2, result));
    //     result = _mm_add_pd(_mm_set1_pd(1), _mm_mul_pd(tempK2, result));
    //     result = _mm_mul_pd(K, result);
    //     result = _mm_mul_pd(_mm_set1_pd(2), result);
    //     result = _mm_add_pd(ln1_5, result);
    //     return result;
    // }

    __m128 ln(__m128 x)
    {
        //note that at ln(x<=0) = -INF
        //constants found by fitting a polynomial to ln(x) on [1, 2]
        __m128i exponentExtraction = _mm_set1_epi32(0x7F800000);
        __m128 LN2 = _mm_set1_ps(0.69314718056f);
        //extract exponent from float
        //some type punning stuff
        __m128i exponent = _mm_and_si128(_mm_castps_si128(x), exponentExtraction);
        __m128 divisor = _mm_castsi128_ps(exponent);
        __m128 exponentAdd = _mm_cvtepi32_ps(_mm_sub_epi32(_mm_srli_epi32(exponent, 23), _mm_set1_epi32(127)));
        exponentAdd = _mm_mul_ps(exponentAdd, LN2);

        //divide by exponent
        x = _mm_div_ps(x, divisor);

        //approximate the fractional part between [1, 2]
        __m128d result1 = lnAround1(_mm_cvtps_pd(x));
        __m128d result2 = lnAround1(_mm_cvtps_pd( _mm_castsi128_ps(_mm_srli_si128(_mm_castps_si128(x), 8)) ));

        __m128 result = packDoublesIntoFloat(result1, result2);

        //add approximation and exponent
        result = _mm_add_ps(result, exponentAdd);
        return result;
    }

    __m128d ln(__m128d x)
    {
        //note that at ln(x<=0) = -INF
        //constants found by fitting a polynomial to ln(x) on [1, 2]
        __m128i exponentExtraction = _mm_set1_epi64x(0x7FF0000000000000);
        __m128d LN2 = _mm_set1_pd(0.69314718056);
        //extract exponent from float
        //some type punning stuff
        __m128i exponent = _mm_and_si128(_mm_castpd_si128(x), exponentExtraction);
        __m128d divisor = _mm_castsi128_pd(exponent);
        __m128d exponentAdd = int64ToDouble(_mm_sub_epi64(_mm_srli_epi64(exponent, 52), _mm_set1_epi64x(1023)));
        exponentAdd = _mm_mul_pd(exponentAdd, LN2);

        //divide by exponent
        x = _mm_div_pd(x, divisor);

        //approximate the fractional part between [1, 2]
        __m128d result = lnAround1(x); //use better approximation

        //add approximation and exponent
        result = _mm_add_pd(result, exponentAdd);
        return result;
    }

    __m128 log2(__m128 x)
    {
        const __m128 adjustment = _mm_set1_ps(std::log(SEML_E)/std::log(2));
        return _mm_mul_ps(ln(x), adjustment);
    }
    __m128d log2(__m128d x)
    {
        const __m128d adjustment = _mm_set1_pd(std::log(SEML_E)/std::log(2));
        return _mm_mul_pd(ln(x), adjustment);
    }

    __m128 log(__m128 x)
    {
        const __m128 adjustment = _mm_set1_ps(std::log(SEML_E));
        return _mm_mul_ps(ln(x), adjustment);
    }
    __m128d log(__m128d x)
    {
        const __m128d adjustment = _mm_set1_pd(std::log(SEML_E));
        return _mm_mul_pd(ln(x), adjustment);
    }

    __m128 log(__m128 x, float base)
    {
        __m128 adjustment = _mm_set1_ps(std::log(SEML_E) / std::log(base));
        return _mm_mul_ps(ln(x), adjustment);
    }
    __m128d log(__m128d x, double base)
    {
        __m128d adjustment = _mm_set1_pd(std::log(SEML_E) / std::log(base));
        return _mm_mul_pd(ln(x), adjustment);
    }

    __m128 log(__m128 x, __m128 base)
    {
        __m128 adjustment = _mm_set1_ps(std::log(SEML_E));
        adjustment = _mm_div_ps(adjustment, log(base));
        return _mm_mul_ps(ln(x), adjustment);
    }
    __m128d log(__m128d x, __m128d base)
    {
        __m128d adjustment = _mm_set1_pd(std::log(SEML_E));
        adjustment = _mm_div_pd(adjustment, log(base));
        return _mm_mul_pd(ln(x), adjustment);
    }

    __m128d expAround0(__m128d x)
    {
        //sigh... use double precision and a taylor series of like 11 terms
        __m128d result = _mm_set1_pd(1);
        __m128d numerator = x;
        __m128d denominator = _mm_set1_pd(1);
        
        for(int i=1; i<=11; i++)
        {
            __m128d addV = _mm_div_pd(numerator, denominator);
            
            result = _mm_add_pd(addV, result);
            numerator = _mm_mul_pd(numerator, x);
            denominator = _mm_mul_pd(denominator, _mm_set1_pd(i+1));
        }

        return result;
    }

    __m128d exp(__m128d x)
    {
        const __m128d log2E = _mm_set1_pd(1.44269504089); //log base 2 (e)
        const __m128d invLog2E = _mm_set1_pd(1.0 / 1.44269504089); //log base 2 (e)

        //so extract the whole number part of the exponent and the fractional part
        __m128d temp = _mm_mul_pd(x, log2E);
        __m128d exponent = _mm_floor_pd(temp); //the exponent part
        __m128i exponentAsInt = _mm_cvtpd_epi32(exponent);
        exponentAsInt = _mm_cvtepu32_epi64(exponentAsInt); //dumb

        __m128d fraction = _mm_sub_pd(temp, exponent); //fractional part
        fraction = _mm_mul_pd(fraction, invLog2E);

        //approximate the fractional part
        __m128d result = expAround0(fraction);
        
        //multiply them together
        __m128i expTemp = _mm_slli_epi64(exponentAsInt, 52); //preparation to convert back to double
        
        //cheating
        //multiplying by power of 2 is equivalent to adding to the exponent of the float
        //add to the exponent then cast back. Potential problem for very very high values
        result = _mm_castsi128_pd(_mm_add_epi64(expTemp, _mm_castpd_si128(result)));
        return result;
    }

    __m128 exp(__m128 x)
    {
        //lazy. could potentially be faster if both are intertwined
        __m128d lowValues = _mm_cvtps_pd(x);
        __m128d highValues = _mm_cvtps_pd(_mm_castsi128_ps(_mm_srli_si128(_mm_castps_si128(x), 8)));
        __m128d result1 = exp(lowValues);
        __m128d result2 = exp(highValues);
        return packDoublesIntoFloat(result1, result2);
    }

    __m128 pow(__m128 x, float power)
    {
        //already inaccurate but normal pow(x, y) is also inaccurate
        //e^(power*ln(x))
        __m128 blendMask = _mm_cmpeq_ps(x, _mm_set1_ps(0));
        __m128 result = exp( _mm_mul_ps(_mm_set1_ps(power), ln(x)));
        result = _mm_blendv_ps(result, _mm_set1_ps(0), blendMask);
        return result;
    }
    __m128d pow(__m128d x, double power)
    {
        //e^(power*ln(x))
        __m128d blendMask = _mm_cmpeq_pd(x, _mm_set1_pd(0));
        __m128d result = exp( _mm_mul_pd(_mm_set1_pd(power), ln(x)));
        result = _mm_blendv_pd(result, _mm_set1_pd(0), blendMask);
        return result;
    }

    __m128 pow(__m128 x, __m128 power)
    {
        //e^(power*ln(x))
        __m128 blendMask = _mm_cmpeq_ps(x, _mm_set1_ps(0));
        __m128 result = exp( _mm_mul_ps(power, ln(x)));
        result = _mm_blendv_ps(result, _mm_set1_ps(0), blendMask);
        return result;
    }
    __m128d pow(__m128d x, __m128d power)
    {
        //e^(power*ln(x))
        __m128d blendMask = _mm_cmpeq_pd(x, _mm_set1_pd(0));
        __m128d result = exp( _mm_mul_pd(power, ln(x)));
        result = _mm_blendv_pd(result, _mm_set1_pd(0), blendMask);
        return result;
    }

    __m128 arcsin(__m128 x)
    {
        //arctan of adjusted input: x / sqrt(1 - x^2)
        __m128 newInput = sqrt( _mm_sub_ps(_mm_set1_ps(1), sqr(x)) );
        newInput = _mm_div_ps(x, newInput);
        __m128 result = arctan(newInput);
        //special case. if x == 1, result is pi/2. if x==-1, result is -pi/2
        __m128 blendValue = _mm_cmpeq_ps(abs(x), _mm_set1_ps(1));
        __m128 possibleOutput = _mm_mul_ps(_mm_set1_ps(SEML_PI/2), sign(x));

        return _mm_blendv_ps(result, possibleOutput, blendValue);
    }
    __m128d arcsin(__m128d x)
    {
        //arctan of adjusted input: x / sqrt(1 - x^2)
        __m128d newInput = sqrt( _mm_sub_pd(_mm_set1_pd(1), sqr(x)) );
        newInput = _mm_div_pd(x, newInput);
        __m128d result = arctan(newInput);
        //special case. if x == 1, result is pi/2. if x==-1, result is -pi/2
        __m128d blendValue = _mm_cmpeq_pd(abs(x), _mm_set1_pd(1));
        __m128d possibleOutput = _mm_mul_pd(_mm_set1_pd(SEML_PI/2), sign(x));

        return _mm_blendv_pd(result, possibleOutput, blendValue);
    }

    __m128 arccos(__m128 x)
    {
        const __m128 PIDIV2 = _mm_set1_ps(SEML_PI/2);
        return _mm_sub_ps(PIDIV2, arcsin(x));
    }
    __m128d arccos(__m128d x)
    {
        const __m128d PIDIV2 = _mm_set1_pd(SEML_PI/2);
        return _mm_sub_pd(PIDIV2, arcsin(x));
    }

    __m128d arctanApproxHigherThan1(__m128d x)
    {
        //https://mae.ufl.edu/~uhk/ARCTAN-APPROX-PAPER.pdf
        //using n=8 instead of n=4. solved manually for n=8
        const __m128d N1 = _mm_set1_pd(307835);
        const __m128d N2 = _mm_set1_pd(4813380);
        const __m128d N3 = _mm_set1_pd(19801782);
        const __m128d N4 = _mm_set1_pd(29609580);
        const __m128d N5 = _mm_set1_pd(14549535);

        const __m128d D1 = _mm_set1_pd(19845);
        const __m128d D2 = _mm_set1_pd(1091475);
        const __m128d D3 = _mm_set1_pd(9459450);
        const __m128d D4 = _mm_set1_pd(28378350);
        const __m128d D5 = _mm_set1_pd(34459425);
        const __m128d D6 = _mm_set1_pd(14549535);

        __m128d xSqr = sqr(x);
        __m128d numerator = _mm_add_pd(N4, _mm_mul_pd(xSqr, N5));
        numerator = _mm_add_pd(N3, _mm_mul_pd(xSqr, numerator));
        numerator = _mm_add_pd(N2, _mm_mul_pd(xSqr, numerator));
        numerator = _mm_add_pd(N1, _mm_mul_pd(xSqr, numerator));

        __m128d denominator = _mm_add_pd(D5, _mm_mul_pd(xSqr, D6));
        denominator = _mm_add_pd(D4, _mm_mul_pd(xSqr, denominator));
        denominator = _mm_add_pd(D3, _mm_mul_pd(xSqr, denominator));
        denominator = _mm_add_pd(D2, _mm_mul_pd(xSqr, denominator));
        denominator = _mm_add_pd(D1, _mm_mul_pd(xSqr, denominator));
        return _mm_div_pd(numerator, denominator);
    }

    __m128 arctan(__m128 x)
    {
        __m128 signMult = sign(x);
        x = abs(x);
        __m128 isZero = _mm_cmpeq_ps(x, _mm_set1_ps(0)); //if 0, return 0
        __m128 ltBlend = _mm_cmplt_ps(x, _mm_set1_ps(1)); //if less than 1, use 1/x
        __m128 adjustment = _mm_blendv_ps(_mm_set1_ps(SEML_PI/2), _mm_set1_ps(0), ltBlend); //at the end, pi/2 - approx or 0 - approx
        x = _mm_blendv_ps(x, _mm_div_ps(_mm_set1_ps(1), x), ltBlend); //sigh... rcp_ps is too inaccurate to use here

        __m128d tempX = _mm_cvtps_pd(x);
        __m128d tempX2 = _mm_cvtps_pd( _mm_castsi128_ps(_mm_srli_si128(_mm_castps_si128(x), 8)));

        __m128d result1 = arctanApproxHigherThan1(tempX);
        __m128d result2 = arctanApproxHigherThan1(tempX2);
        __m128 finalResult = _mm_mul_ps(x, packDoublesIntoFloat(result1, result2));

        //multiply sign into |adjustment-result|
        finalResult = _mm_mul_ps(signMult, abs(_mm_sub_ps(adjustment, finalResult)));
        finalResult = _mm_blendv_ps(finalResult, _mm_set1_ps(0), isZero); //x=0 must be accounted for
        return finalResult;
    }
    __m128d arctan(__m128d x)
    {
        __m128d signMult = sign(x);
        x = abs(x);
        __m128d isZero = _mm_cmpeq_pd(x, _mm_set1_pd(0)); //if 0, return 0
        __m128d ltBlend = _mm_cmplt_pd(x, _mm_set1_pd(1)); //if less than 1, use 1/x
        __m128d adjustment = _mm_blendv_pd(_mm_set1_pd(SEML_PI/2), _mm_set1_pd(0), ltBlend); //at the end, pi/2 - approx or 0 - approx
        x = _mm_blendv_pd(x, _mm_div_pd(_mm_set1_pd(1), x), ltBlend);

        __m128d finalResult = _mm_mul_pd(x, arctanApproxHigherThan1(x));
        
        //multiply sign into |adjustment-result|
        finalResult = _mm_mul_pd(signMult, abs(_mm_sub_pd(adjustment, finalResult)));
        finalResult = _mm_blendv_pd(finalResult, _mm_set1_pd(0), isZero); //x=0 must be accounted for
        return finalResult;
    }

    __m128 arccsc(__m128 x)
    {
        return arcsin(_mm_rcp_ps(x));
    }
    __m128d arccsc(__m128d x)
    {
        return arcsin(_mm_div_pd(_mm_set1_pd(1), x));
    }

    __m128 arcsec(__m128 x)
    {
        return arccos(_mm_rcp_ps(x));
    }
    __m128d arcsec(__m128d x)
    {
        return arccos(_mm_div_pd(_mm_set1_pd(1), x));
    }

    __m128 arccot(__m128 x)
    {
        const __m128 PIDIV2 = _mm_set1_ps(SEML_PI/2);
        return _mm_sub_ps(PIDIV2, arctan(x));
    }
    __m128d arccot(__m128d x)
    {
        const __m128d PIDIV2 = _mm_set1_pd(SEML_PI/2);
        return _mm_sub_pd(PIDIV2, arctan(x));
    }

    __m128 sinh(__m128 x)
    {
        __m128 eX = exp(x);
        __m128 negativeEX = reciprocal(eX);
        __m128 numerator = _mm_sub_ps(eX, negativeEX);
        return _mm_mul_ps(numerator, _mm_set1_ps(0.5));
    }
    __m128d sinh(__m128d x)
    {
        __m128d eX = exp(x);
        __m128d negativeEX = reciprocal(eX);
        __m128d numerator = _mm_sub_pd(eX, negativeEX);
        return _mm_mul_pd(numerator, _mm_set1_pd(0.5));
    }

    __m128 cosh(__m128 x)
    {
        __m128 eX = exp(x);
        __m128 negativeEX = reciprocal(eX);
        __m128 numerator = _mm_add_ps(eX, negativeEX);
        return _mm_mul_ps(numerator, _mm_set1_ps(0.5));
    }
    __m128d cosh(__m128d x)
    {
        __m128d eX = exp(x);
        __m128d negativeEX = reciprocal(eX);
        __m128d numerator = _mm_add_pd(eX, negativeEX);
        return _mm_mul_pd(numerator, _mm_set1_pd(0.5));
    }

    __m128 tanh(__m128 x)
    {
        __m128 expValue = exp(_mm_mul_ps(x, _mm_set1_ps(2)));
        __m128 numerator = _mm_sub_ps(expValue, _mm_set1_ps(1));
        __m128 denominator = _mm_add_ps(expValue, _mm_set1_ps(1));
        return _mm_div_ps(numerator, denominator);
    }
    __m128d tanh(__m128d x)
    {
        __m128d expValue = exp(_mm_mul_pd(x, _mm_set1_pd(2)));
        __m128d numerator = _mm_sub_pd(expValue, _mm_set1_pd(1));
        __m128d denominator = _mm_add_pd(expValue, _mm_set1_pd(1));
        return _mm_div_pd(numerator, denominator);
    }

    __m128 sech(__m128 x)
    {
        __m128 eX = exp(x);
        __m128 negativeEX = reciprocal(eX);
        __m128 denominator = _mm_add_ps(eX, negativeEX);
        return _mm_div_ps(_mm_set1_ps(0.5), denominator);
    }
    __m128d sech(__m128d x)
    {
        __m128d eX = exp(x);
        __m128d negativeEX = reciprocal(eX);
        __m128d denominator = _mm_add_ps(eX, negativeEX);
        return _mm_div_pd(_mm_set1_pd(0.5), denominator);
    }

    __m128 csch(__m128 x)
    {
        __m128 eX = exp(x);
        __m128 negativeEX = reciprocal(eX);
        __m128 denominator = _mm_sub_ps(eX, negativeEX);
        return _mm_div_ps(_mm_set1_ps(0.5), denominator);
    }
    __m128d csch(__m128d x)
    {
        __m128d eX = exp(x);
        __m128d negativeEX = reciprocal(eX);
        __m128d denominator = _mm_sub_ps(eX, negativeEX);
        return _mm_div_pd(_mm_set1_pd(0.5), denominator);
    }

    __m128 coth(__m128 x)
    {
        __m128 expValue = exp(_mm_mul_ps(x, _mm_set1_ps(2)));
        __m128 numerator = _mm_add_ps(expValue, _mm_set1_ps(1));
        __m128 denominator = _mm_sub_ps(expValue, _mm_set1_ps(1));
        return _mm_div_ps(numerator, denominator);
    }
    __m128d coth(__m128d x)
    {
        __m128d expValue = exp(_mm_mul_pd(x, _mm_set1_pd(2)));
        __m128d numerator = _mm_add_pd(expValue, _mm_set1_pd(1));
        __m128d denominator = _mm_sub_pd(expValue, _mm_set1_pd(1));
        return _mm_div_pd(numerator, denominator);
    }

    __m128 arcsinh(__m128 x)
    {
        __m128 value = sqrt(_mm_add_ps(x, _mm_add_ps(sqr(x), _mm_set1_ps(1))));
        return ln(value);
    }
    __m128d arcsinh(__m128d x)
    {
        __m128d value = sqrt(_mm_add_pd(x, _mm_add_pd(sqr(x), _mm_set1_pd(1))));
        return ln(value);
    }

    __m128 arccosh(__m128 x)
    {
        __m128 value = sqrt(_mm_add_ps(x, _mm_sub_ps(sqr(x), _mm_set1_ps(1))));
        return ln(value);
    }
    __m128d arccosh(__m128d x)
    {
        __m128d value = sqrt(_mm_add_pd(x, _mm_sub_pd(sqr(x), _mm_set1_pd(1))));
        return ln(value);
    }

    __m128 arctanh(__m128 x)
    {
        __m128 numerator = _mm_add_ps(_mm_set1_ps(1), x);
        __m128 denominator = _mm_sub_ps(_mm_set1_ps(1), x);
        return _mm_mul_ps(_mm_set1_ps(0.5), ln(_mm_div_ps(numerator, denominator)));
    }
    __m128d arctanh(__m128d x)
    {
        __m128d numerator = _mm_add_pd(_mm_set1_pd(1), x);
        __m128d denominator = _mm_sub_pd(_mm_set1_pd(1), x);
        return _mm_mul_pd(_mm_set1_pd(0.5), ln(_mm_div_pd(numerator, denominator)));
    }

    __m128 arccsch(__m128 x)
    {
        return arcsinh(_mm_rcp_ps(x));
    }
    __m128d arccsch(__m128d x)
    {
        return arcsinh(_mm_div_pd(_mm_set1_pd(1), x));
    }

    __m128 arcsech(__m128 x)
    {
        return arccosh(_mm_rcp_ps(x));
    }
    __m128d arcsech(__m128d x)
    {
        return arccosh(_mm_div_pd(_mm_set1_pd(1), x));
    }

    __m128 arccoth(__m128 x)
    {
        __m128 numerator = _mm_add_ps(x, _mm_set1_ps(1));
        __m128 denominator = _mm_sub_ps(x, _mm_set1_ps(1));
        return _mm_mul_ps(_mm_set1_ps(0.5), ln(_mm_div_ps(numerator, denominator)));
    }
    __m128d arccoth(__m128d x)
    {
        __m128d numerator = _mm_add_pd(x, _mm_set1_pd(1));
        __m128d denominator = _mm_sub_pd(x, _mm_set1_pd(1));
        return _mm_mul_pd(_mm_set1_pd(0.5), ln(_mm_div_pd(numerator, denominator)));
    }
    #endif
}