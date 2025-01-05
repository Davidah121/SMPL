#pragma once
#include "Opti.h"
#include <utility>

#ifndef SEML_PI
    #define SEML_PI 3.14159265359
#endif
#ifndef SEML_PI2
    #define SEML_PI2 6.28318530718
#endif
#ifndef SEML_E
	#define SEML_E 2.71828182846
#endif


namespace SEML
{
    #if (OPTI >= 1)
    /**
     * @brief Converts doubles to a 64 bit number. Does not work with all possible values.
     *      Range [0, 2^52] for unsigned. [-2^51, 2^51] for signed.
     *      Faster as it is 2 instructions
     *      https://stackoverflow.com/questions/41144668/how-to-efficiently-perform-double-int64-conversions-with-sse-avx
     * @param x 
     * @return __m128i 
     */
    __m128i fastDoubleToInt64(__m128d x);
    __m128i fastDoubleToUInt64(__m128d x);

    /**
     * @brief Converts a 64 bit number to a double. Does not work with all possible values.
     *      Range [0, 2^52] for unsigned. [-2^51, 2^51] for signed.
     *      Faster as it is 2 instructions
     *      https://stackoverflow.com/questions/41144668/how-to-efficiently-perform-double-int64-conversions-with-sse-avx
     * @param x 
     * @return __m128d 
     */
    __m128d fastInt64ToDouble(__m128i x);
    __m128d fastUInt64ToDouble(__m128i x);

    /**
     * @brief Converts a 64 bit number to a double. Works across the entire range
     *      https://stackoverflow.com/questions/41144668/how-to-efficiently-perform-double-int64-conversions-with-sse-avx
     * @param x 
     * @return __m128d 
     */
    __m128d int64ToDouble(__m128i x);
    __m128d uint64ToDouble(__m128i x);

    /**
     * @brief Reduces x to the range [-PI, PI]
     *      for trignometric functions
     * 
     * @param x 
     * @return __m128 
     */
    __m128 piRangeReduction(__m128 x);
    __m128d piRangeReduction(__m128d x);

    /**
     * @brief Converts radians to degrees
     * 
     * @param x 
     * @return __m128 
     */
    __m128 radToDeg(__m128 x);
    __m128d radToDeg(__m128d x);

    /**
     * @brief Converts degrees to radians
     * 
     * @param x 
     * @return __m128 
     */
    __m128 degToRad(__m128 x);
    __m128d degToRad(__m128d x);

    /**
     * @brief Computes the absolute value of x.
     *      Does so by flipping the sign bit in the floats.
     * 
     * @param x 
     * @return __m128 
     */
    __m128 abs(__m128 x);
    __m128d abs(__m128d x);

    /**
     * @brief Computes the negative of x.
     *      Does not use multiplication so it is a bit faster.
     * 
     * @param x 
     * @return __m128 
     */
    __m128 negate(__m128 x);
    __m128d negate(__m128d x);

    /**
     * @brief Extracts the sign of x as a value that can
     *      be multiplied back in after using abs(x).
     *      I.E.
     *          sign(x)*abs(x) = x
     * 
     * @param x 
     * @return __m128 
     */
    __m128 sign(__m128 x);
    __m128d sign(__m128d x);

    /**
     * @brief Computes 1/x
     *      specifically computes _mm_div_ps(_mm_set1_ps(1), x)
     *      The fast version that is less accurate uses _mm_rcp_ps()
     * @param a 
     * @return __m128 
     */
    __m128 reciprocal(__m128 a);
    __m128d reciprocal(__m128d a);
    __m128 fastReciprocal(__m128 a);
    
    /**
     * @brief Computes the square of x.
     *      Is equivalent to _mm_mul_ps(x, x)
     * 
     * @param x 
     * @return __m128 
     */
    __m128 sqr(__m128 x);
    __m128d sqr(__m128d x);
    
    /**
     * @brief Computes the cube of x.
     * 
     * @param a 
     * @return __m128 
     */
	__m128 cube(__m128 a);
	__m128d cube(__m128d a);

    /**
     * @brief Computes the Square Root of x.
     *      Is equivalent to _mm_sqrt_ps(x)
     * 
     * @param x 
     * @return __m128 
     */
    __m128 sqrt(__m128 x);
    __m128d sqrt(__m128d x);

    /**
     * @brief Computes the inverse sqrt of x.
     * @param a 
     * @return __m128 
     */
    __m128 invSqrt(__m128 a);
    __m128d invSqrt(__m128d a);

    /**
     * @brief Computes the inverse sqrt of x.
     *      While faster, it is a little less accurate.
     * @param a 
     * @return __m128 
     */
    __m128 fastInvSqrt(__m128 a);

    /**
     * @brief Computes the cos close to 0 with relatively high accuracy.
     *      used in the computation of sin and cos for single precision floating points
     * 
     * @param x 
     * @return __m128d 
     */
    __m128d cosAround0(__m128d x);

    /**
     * @brief Computes both sin and cos of the given angle.
     *      This is what is internally used for both sin and cos functions
     *      so this comes at no additional cost.
     * 
     *      Returns a pair where
     *          first = sin
     *          second = cos
     * 
     * @param x 
     * @return std::pair<__m128, __m128> 
     */
    std::pair<__m128, __m128> sincos(__m128 x);
    std::pair<__m128d, __m128d> sincos(__m128d x);

    /**
     * @brief Approximates the sine of x.
     *      Maintains the proper relationship between sine and cosine of x.
     *          cos(x) = sqrt(1 - sin(x)^2)
     * 
     * @param x 
     * @return __m128 
     */
    __m128 sin(__m128 x);
    __m128d sin(__m128d x);

    /**
     * @brief Approximates the cosine of x.
     *      Maintains the proper relationship between sine and cosine of x.
     *          cos(x) = sqrt(1 - sin(x)^2)
     * 
     * @param x 
     * @return __m128 
     */
    __m128 cos(__m128 x);
    __m128d cos(__m128d x);

    /**
     * @brief Approximates the tangent of x.
     * 
     * @param x 
     * @return __m128 
     */
    __m128 tan(__m128 x);
    __m128d tan(__m128d x);

    /**
     * @brief Approximates the secant of x.
     * 
     * @param x 
     * @return __m128 
     */
    __m128 sec(__m128 x);
    __m128d sec(__m128d x);

    /**
     * @brief Approximates the cosecant of x.
     * 
     * @param x 
     * @return __m128 
     */
    __m128 csc(__m128 x);
    __m128d csc(__m128d x);

    /**
     * @brief Approximates the cotangent of x.
     * 
     * @param x 
     * @return __m128 
     */
    __m128 cot(__m128 x);
    __m128d cot(__m128d x);

    /**
     * @brief Approximates the natural log of x.
     * 
     * @param x 
     * @return __m128 
     */
    __m128 ln(__m128 x);
    __m128d ln(__m128d x);

    /**
     * @brief Approximates the natural log of x for values close to 1.
     *      Internally is used to approximate ln(x) for all x
     * 
     * @param x 
     * @return __m128d 
     */
    __m128d lnAround1(__m128d x);

    /**
     * @brief Approximates the log base 2 of x.
     * 
     * @param x 
     * @return __m128 
     */
    __m128 log2(__m128 x);
    __m128d log2(__m128d x);

    /**
     * @brief Approximates the log base 10 of x.
     * 
     * @param x 
     * @return __m128 
     */
    __m128 log(__m128 x);
    __m128d log(__m128d x);

    /**
     * @brief Approximates the log of x with the specified base.
     * 
     * @param x 
     * @param base 
     * @return __m128 
     */
    __m128 log(__m128 x, float base);
    __m128d log(__m128d x, double base);

    /**
     * @brief Approximates the log of x with the specified base.
     * 
     * @param x 
     * @param base 
     * @return __m128 
     */
    __m128 log(__m128 x, __m128 base);
    __m128d log(__m128d x, __m128d base);

    /**
     * @brief Approximates e^x
     * 
     * @param x 
     * @return __m128 
     */
    __m128 exp(__m128 x);
    __m128d exp(__m128d x);

    /**
     * @brief Approximates e^x for values close to the origin 0
     *      Internally is used to approximate exp(x) for all x
     *      Uses double precision for more accuracy
     * 
     * @param x 
     * @return __m128d 
     */
    __m128d expAround0(__m128d x);

    /**
     * @brief Approximates x to the specified power
     * 
     * @param x 
     * @param power 
     * @return __m128 
     */
    __m128 pow(__m128 x, float power);
    __m128d pow(__m128d x, double power);

    /**
     * @brief Approximates x to the specified power
     * 
     * @param x 
     * @param power 
     * @return __m128 
     */
    __m128 pow(__m128 x, __m128 power);
    __m128d pow(__m128d x, __m128d power);


    /**
     * @brief Approximates the arcsine (sine inverse) of x
     *      Note that the inverse is only valid from [-1, 1] and returns the result in radians
     * 
     *      Approach used:
     *          https://dsp.stackexchange.com/questions/20444/books-resources-for-implementing-various-mathematical-functions-in-fixed-point-a/20482#20482
     * 
     * @param x 
     * @param power 
     * @return __m128 
     */
    __m128 arcsin(__m128 x);
    __m128d arcsin(__m128d x);

    /**
     * @brief Approximates the arccosine (cosine inverse) of x
     *      Note that the inverse is only valid from [-1, 1] and returns the result in radians
     * 
     *      Approach used:
     *          https://dsp.stackexchange.com/questions/20444/books-resources-for-implementing-various-mathematical-functions-in-fixed-point-a/20482#20482
     * 
     * @param x 
     * @param power 
     * @return __m128 
     */
    __m128 arccos(__m128 x);
    __m128d arccos(__m128d x);

    /**
     * @brief Approximates the arctan of x for values 1 or larger.
     *      Uses doubles for higher precision. For values less than 1, it diverges by a lot.
     *      accurate to 7 decimal places at 1. Higher accuracy as x increases.
     * 
     * @param x 
     * @return __m128d 
     */
    __m128d arctanApproxHigherThan1(__m128d x);

    /**
     * @brief Approximates the arctangent (tangent inverse) of x
     *      Returns the result in radians
     *      
     *      Approach used:
     *          https://dsp.stackexchange.com/questions/20444/books-resources-for-implementing-various-mathematical-functions-in-fixed-point-a/20482#20482
     * 
     * @param x 
     * @param power 
     * @return __m128 
     */
    __m128 arctan(__m128 x);
    __m128d arctan(__m128d x);

    /**
     * @brief Approximates the arc cosecant (cosecant inverse) of x
     *      Note that the inverse is only valid from x <= -1 && x >= 1 and returns the result in radians
     *          Invalid in range [-1, 1]
     *      Approach used:
     *          https://dsp.stackexchange.com/questions/20444/books-resources-for-implementing-various-mathematical-functions-in-fixed-point-a/20482#20482
     * 
     * @param x 
     * @param power 
     * @return __m128 
     */
    __m128 arccsc(__m128 x);
    __m128d arccsc(__m128d x);

    /**
     * @brief Approximates the arc secant (secant inverse) of x
     *      Note that the inverse is only valid from x <= -1 && x >= 1 and returns the result in radians
     *          Invalid in range [-1, 1]
     * 
     *      Approach used:
     *          https://dsp.stackexchange.com/questions/20444/books-resources-for-implementing-various-mathematical-functions-in-fixed-point-a/20482#20482
     * 
     * @param x 
     * @param power 
     * @return __m128 
     */
    __m128 arcsec(__m128 x);
    __m128d arcsec(__m128d x);

    /**
     * @brief Approximates the arc cotangent (cotangent inverse) of x
     *      Returns the result in radians
     *      
     *      Approach used:
     *          https://dsp.stackexchange.com/questions/20444/books-resources-for-implementing-various-mathematical-functions-in-fixed-point-a/20482#20482
     * 
     * @param x 
     * @param power 
     * @return __m128 
     */
    __m128 arccot(__m128 x);
    __m128d arccot(__m128d x);

    /**
     * @brief Approximates the hyperbolic sine of x.
     *      Relies on the accuracy of exp(x)
     * 
     * @param x 
     * @return __m128 
     */
    __m128 sinh(__m128 x);
    __m128d sinh(__m128d x);

    /**
     * @brief Approximates the hyperbolic cosine of x.
     *      Relies on the accuracy of exp(x)
     * 
     * @param x 
     * @return __m128 
     */
    __m128 cosh(__m128 x);
    __m128d cosh(__m128d x);

    /**
     * @brief Approximates the hyperbolic tangent of x.
     *      Relies on the accuracy of exp(x)
     * 
     * @param x 
     * @return __m128 
     */
    __m128 tanh(__m128 x);
    __m128d tanh(__m128d x);

    /**
     * @brief Approximates the hyperbolic secant of x.
     *      Relies on the accuracy of exp(x)
     * 
     * @param x 
     * @return __m128 
     */
    __m128 sech(__m128 x);
    __m128d sech(__m128d x);

    /**
     * @brief Approximates the hyperbolic cosecant of x.
     *      Relies on the accuracy of exp(x)
     * 
     * @param x 
     * @return __m128 
     */
    __m128 csch(__m128 x);
    __m128d csch(__m128d x);

    /**
     * @brief Approximates the hyperbolic cotangent of x.
     *      Relies on the accuracy of exp(x)
     * 
     * @param x 
     * @return __m128 
     */
    __m128 coth(__m128 x);
    __m128d coth(__m128d x);

    /**
     * @brief Approximates the inverse hyperbolic sine of x.
     *      Relies on the accuracy of ln(x)
     * 
     * @param x 
     * @return __m128 
     */
    __m128 arcsinh(__m128 x);
    __m128d arcsinh(__m128d x);

    /**
     * @brief Approximates the inverse hyperbolic cosine of x.
     *      Relies on the accuracy of ln(x)
     * 
     * @param x 
     * @return __m128 
     */
    __m128 arccosh(__m128 x);
    __m128d arccosh(__m128d x);

    /**
     * @brief Approximates the inverse hyperbolic tangent of x.
     *      Relies on the accuracy of ln(x)
     * 
     * @param x 
     * @return __m128 
     */
    __m128 arctanh(__m128 x);
    __m128d arctanh(__m128d x);

    /**
     * @brief Approximates the inverse hyperbolic secant of x.
     *      Relies on the accuracy of ln(x)
     * 
     * @param x 
     * @return __m128 
     */
    __m128 arcsech(__m128 x);
    __m128d arcsech(__m128d x);

    /**
     * @brief Approximates the inverse hyperbolic cosecant of x.
     *      Relies on the accuracy of ln(x)
     * 
     * @param x 
     * @return __m128 
     */
    __m128 arccsch(__m128 x);
    __m128d arccsch(__m128d x);

    /**
     * @brief Approximates the inverse hyperbolic cotangent of x.
     *      Relies on the accuracy of ln(x)
     * 
     * @param x 
     * @return __m128 
     */
    __m128 arccoth(__m128 x);
    __m128d arccoth(__m128d x);
    #endif
}