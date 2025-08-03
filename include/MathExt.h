#pragma once
#include "BuildOptions.h"
#include "SIMD.h"
#include <limits>
#include <math.h>
#include <cmath>
#include "Line.h"
#include "Vec2f.h"
#include "Vec3f.h"
#include "Vec4f.h"
#include "Matrix.h"
#include "Mat2f.h"
#include "Mat3f.h"
#include "Mat4f.h"
#include "Quaternion.h"
#include "ComplexNumber.h"
#include "PolarCoordinate.h"
#include "MathFunction.h"
#include "Float16.h"

#include "SEML.h"
#include "SEML_256.h"

#ifdef max
	#undef max
#endif

#ifdef min
	#undef min
#endif

#ifndef PI
	#define PI 3.14159265
#endif

#ifndef E
	#define E 2.718281828
#endif

#ifndef EPSILON
	#define EPSILON 1E-6
#endif

//Temporary location
#ifndef LOGIC_XOR
	#define LOGIC_XOR(a,b) ((!(a)) != (!(b)))
#endif

#ifndef EPSILON
	#define EPSILON 1e-10
#endif

#ifndef IS_POWER_2
	#define IS_POWER_2(x) (((x) & ((x)-1)) == 0)
#endif

#ifndef TYPE_PUN
	#define TYPE_PUN(x, type) (*(type*)&x)
#endif

#include "FixedDecimalFloat.h"

namespace smpl
{

	class DLL_OPTION MathExt
	{
	public:
		static const int CONVOLVE_FULL = 0;
		static const int CONVOLVE_SAME = 1;
		static const int CONVOLVE_VALID = 2;

		static const int CONVOLVE_ZERO_PADDING = 0;
		static const int CONVOLVE_CLAMP = 1;
		static const int CONVOLVE_WRAP = 2;

		/**
		 * @brief Returns the number of 1 bits in the value x.
		 * 		Uses some bit hacks and is cross platform.
		 * 
		 * @param x 
		 * @return int 
		 */
		static int popcount(uint8_t x);
		static int popcount(uint16_t x);
		static int popcount(uint32_t x);
		static int popcount(uint64_t x);

		/**
		 * @brief Reverses the bits from MSB to LSB or vice versa.
		 * 		Takes from https://stackoverflow.com/questions/746171/efficient-algorithm-for-bit-reversal-from-msb-lsb-to-lsb-msb-in-c
		 * 			Note that the 64 bit version does not use a larger lookup table and is not as fast as it could possibly be.
		 * 
		 * @param v1 
		 * @return uint8_t 
		 */
		static uint8_t bitReversal(uint8_t v1);
		static uint16_t bitReversal(uint16_t v1);
		static uint32_t bitReversal(uint32_t v1);
		static uint64_t bitReversal(uint64_t v1);
		
		static int hammingDistance(uint8_t v1, uint8_t v2);
		static int hammingDistance(uint16_t v1, uint16_t v2);
		static int hammingDistance(uint32_t v1, uint32_t v2);
		static int hammingDistance(uint64_t v1, uint64_t v2);

		/**
		 * @brief Calculates the unsigned saturated addition of two numbers.
		 * 		Specifically:
		 * 			v1+v2 but if it overflows, it is set to the maximum value allowed
		 * 		Good in scenarios where wrapping around is undesirable
		 * 
		 * @param v1 
		 * @param v2 
		 * @return uint8_t 
		 */
		static uint8_t saturatedAdd(uint8_t v1, uint8_t v2);
		static uint16_t saturatedAdd(uint16_t v1, uint16_t v2);
		static uint32_t saturatedAdd(uint32_t v1, uint32_t v2);
		static uint64_t saturatedAdd(uint64_t v1, uint64_t v2);
		
		/**
		 * @brief Calculates the unsigned saturated subtraction of two numbers.
		 * 		Specifically:
		 * 			v1-v2 but if it underflows, it is set to the minimum value allowed
		 * 		Good in scenarios where wrapping around is undesirable
		 * 
		 * @param v1 
		 * @param v2 
		 * @return uint8_t 
		 */
		static uint8_t saturatedSub(uint8_t v1, uint8_t v2);
		static uint16_t saturatedSub(uint16_t v1, uint16_t v2);
		static uint32_t saturatedSub(uint32_t v1, uint32_t v2);
		static uint64_t saturatedSub(uint64_t v1, uint64_t v2);

		/**
		 * @brief Calculates the signed saturated addition of two numbers.
		 * 		Specifically:
		 * 			v1+v2 but if it overflows/underflows, it is set to the maximum/minimum value allowed
		 * 		Good in scenarios where wrapping around is undesirable
		 * 
		 * @param v1 
		 * @param v2 
		 * @return uint8_t 
		 */
		static int8_t saturatedAdd(int8_t v1, int8_t v2);
		static int16_t saturatedAdd(int16_t v1, int16_t v2);
		static int32_t saturatedAdd(int32_t v1, int32_t v2);
		static int64_t saturatedAdd(int64_t v1, int64_t v2);
		
		static int8_t saturatedSub(int8_t v1, int8_t v2);
		static int16_t saturatedSub(int16_t v1, int16_t v2);
		static int32_t saturatedSub(int32_t v1, int32_t v2);
		static int64_t saturatedSub(int64_t v1, int64_t v2);

		/**
		 * @brief Returns the max of the 2 template values.
		 * 
		 * @param a 
		 * @param b 
		 * @return T 
		 */
		template<typename T>
		static T max(T a, T b)
		{
			return (((a)>(b))? (a):(b));
		}
		
		/**
		 * @brief Returns the max from the array of template values
		 * 
		 * @param a 
		 * 		The array
		 * @param count
		 * 		The size of the array
		 * @return T 
		 */
		template<typename T>
		static T max(T* a, int count)
		{
			if(a == nullptr || count == 0)
			{
				return T();
			}

			T maxV = a[0];
			for(int i=0; i<count; i++)
			{
				if(a[i] > maxV)
					maxV = a[i];
			}
			return maxV;
		}

		/**
		 * @brief Returns the max from the array of template values using initializer_list.
		 * 		Allows the use of { value, value, value } as arguments.
		 * 
		 * @param list
		 * 		The list of values.
		 * @return T 
		 */
		template<typename T>
		static T max( std::initializer_list<T> list)
		{
			T maxV;
			bool first = true;
			for(T elem : list)
			{	
				if(first)
				{
					maxV = elem;
					first = false;
				}
				
				if(elem > maxV)
					maxV = elem;
			}
			return maxV;
		}

		/**
		 * @brief Returns the min of the 2 template values.
		 * 
		 * @param a 
		 * @param b 
		 * @return T 
		 */
		template<typename T>
		static T min(T a, T b)
		{
			return ((a<b)? a:b);
		}

		/**
		 * @brief Returns the max from the array of template values
		 * 
		 * @param a 
		 * 		The array
		 * @param count
		 * 		The size of the array
		 * @return T 
		 */
		template<typename T>
		static T min(T* a, int count)
		{
			if(a == nullptr || count == 0)
			{
				return T();
			}
			T minV = a[0];
			for(int i=0; i<count; i++)
			{
				if(a[i] < minV)
					minV = a[i];
			}
			return minV;
		}

		/**
		 * @brief Returns the max from the array of template values using initializer_list.
		 * 		Allows the use of { value, value, value } as arguments.
		 * 
		 * @param list
		 * 		The list of values.
		 * @return T 
		 */
		template<typename T>
		static T min( std::initializer_list<T> list)
		{
			T minV;
			bool first = true;
			for(T elem : list)
			{
				if(first)
				{
					minV = elem;
					first = false;
				}

				if(elem < minV)
					minV = elem;
			}
			return minV;
		}
		
		/**
		 * @brief Rounds the value up to the nearest power of 2.
		 * 
		 * @param a 
		 * @return T 
		 */
		template<typename T>
		static T roundToNearestPower2(T a)
		{
			unsigned long long v = a; // compute the next highest power of 2 of 64-bit v
			v--;
			v |= v >> 1;
			v |= v >> 2;
			v |= v >> 4;
			v |= v >> 8;
			v |= v >> 16;
			v |= v >> 32;
			v++;
			return v;
		}
		
		/**
		 * @brief Returns the floor of the value.
		 * 		The floor operation always rounds down.
		 * 
		 * @param a 
		 * @return float 
		 */
		static float floor(float a);

		/**
		 * @brief Returns the ceil of the value.
		 * 		The ceiling operation always rounds up.
		 * 
		 * @param a 
		 * @return float 
		 */
		static float ceil(float a);

		/**
		 * @brief Returns the rounded value.
		 * 
		 * @param a 
		 * @return float 
		 */
		static float round(float a);

		/**
		 * @brief Returns the floor of the value.
		 * 		The floor operation always rounds down.
		 * 
		 * @param a 
		 * @return double 
		 */
		static double floor(double a);

		/**
		 * @brief Returns the ceil of the value.
		 * 		The ceiling operation always rounds up.
		 * 
		 * @param a 
		 * @return double 
		 */
		static double ceil(double a);

		/**
		 * @brief Returns the rounded value.
		 * 
		 * @param a 
		 * @return double 
		 */
		static double round(double a);

		/**
		 * @brief Returns the fractional or decimal part of the number.
		 * 
		 * @param a 
		 * @return double 
		 */
		static double frac(double a);

		/**
		 * @brief Returns the fractional or decimal part of the number.
		 * 
		 * @param a 
		 * @return float 
		 */
		static float frac(float a);

		/**
		 * @brief Rounds the number to the specified number of decimal places.
		 * 
		 * @param a 
		 * @param decimalPlaces 
		 * 		The amount of decmial places to round to.
		 * 		The default is 6
		 * @return float 
		 */
		static float roundToDecimal(float a, int decimalPlaces=6);

		/**
		 * @brief Rounds the number to the specified number of decimal places.
		 * 
		 * @param a 
		 * @param decimalPlaces 
		 * 		The amount of decmial places to round to.
		 * 		The default is 6
		 * @return double 
		 */
		static double roundToDecimal(double a, int decimalPlaces=6);

		/**
		 * @brief Returns the square of the template value.
		 * 
		 * @param a 
		 * @return T 
		 */
		template<typename T>
		static T sqr(T a)
		{
			return a*a;
		}

		/**
		 * @brief Returns the cube of the template value.
		 * 
		 * @param a 
		 * @return T 
		 */
		template<typename T>
		static T cube(T a)
		{
			return a*a*a;
		}

		/**
		 * @brief Returns the sqrt of the value.
		 * 
		 * @param a 
		 * @return float 
		 */
		static float sqrt(float a);

		/**
		 * @brief Returns the sqrt of the value.
		 * 
		 * @param a 
		 * @return double 
		 */
		static double sqrt(double a);

		/**
		 * @brief Returns the sqrt of the template value.
		 * 
		 * @param a 
		 * @return double 
		 */
		template<typename T>
		static double sqrt(T a)
		{
			return std::sqrt(a);
		}
		
		/**
		 * @brief Returns the sqrt of a float using a faster approximation.
		 * 		Approximation from the Fast Inverse Square Root code from Quake 3 Arena
		 * 
		 * @param a 
		 * @return float 
		 */
		static float fastSqrt(float a);

		/**
		 * @brief Returns the Inverse sqrt of a float using a faster approximation. (1/sqrt(a))
		 * 		Approximation from the Fast Inverse Square Root code from Quake 3 Arena
		 * 
		 * @param a 
		 * @return float 
		 */
		static float fastInvSqrt(float a);

		/**
		 * @brief Returns the cube root of the value.
		 * 
		 * @param a 
		 * @return float 
		 */
		static float cuberoot(float a);

		/**
		 * @brief Returns the cube root of the value.
		 * 
		 * @param a 
		 * @return double 
		 */
		static double cuberoot(double a);
		
		/**
		 * @brief Returns the cube root of the template value.
		 * 
		 * @param a 
		 * @return double 
		 */
		template<typename T>
		static double cuberoot(T a)
		{
			return std::cbrt(a);
		}

		/**
		 * @brief Returns the value raise to the specified power. Negative powers are allowed.
		 * 
		 * @param value 
		 * @param power 
		 * @return float 
		 */
		static float pow(float value, float power);

		/**
		 * @brief Returns the value raise to the specified power. Negative powers are allowed.
		 * 
		 * @param value 
		 * @param power 
		 * @return double 
		 */
		static double pow(double value, double power);
		
		/**
		 * @brief Returns the logarithm of the value with the specified base.
		 * 
		 * @param value 
		 * @param base 
		 * 		Default value is 10
		 * @return float 
		 */
		static float log(float value, float base = 10.0f);

		/**
		 * @brief Returns the logarithm of the value with the specified base.
		 * 
		 * @param value 
		 * @param base 
		 * 		Default value is 10
		 * @return double 
		 */
		static double log(double value, double base = 10.0);

		/**
		 * @brief Linearly blend 2 values together using a blend value between [0, 1]
		 * 
		 * @param a 
		 * @param b 
		 * @param blend 
		 * @return float 
		 */
		static float lerp(float a, float b, float blend);

		/**
		 * @brief Linearly blend 2 values together using a blend value between [0, 1]
		 * 
		 * @param a 
		 * @param b 
		 * @param blend 
		 * @return double 
		 */
		static double lerp(double a, double b, double blend);

		/**
		 * @brief Returns the absolute value
		 * 
		 * @param a 
		 * @return int 
		 */
		static int abs(int a);

		/**
		 * @brief Returns the absolute value
		 * 
		 * @param a 
		 * @return double 
		 */
		static double abs(double a);

		/**
		 * @brief Returns the absolute value
		 * 
		 * @param a 
		 * @return float 
		 */
		static float abs(float a);

		/**
		 * @brief Returns the absolute value
		 * 
		 * @param a 
		 * @return long 
		 */
		static long abs(long a);

		/**
		 * @brief Returns a random number using std::rand()
		 * 		std::srand() should be used to set the seed.
		 * 
		 * @return unsigned int 
		 */
		static unsigned int random();

		/**
		 * @brief Returns a random number using std::rand() within the specified range
		 * 		std::srand() should be used to set the seed.
		 * @param min
		 * 		The minimum range
		 * @param max
		 * 		The maximum range
		 * @return unsigned int 
		 */
		static unsigned int randomRange(unsigned int min, unsigned int max);
		
		/**
		 * @brief Clamps a value between the minimum and maximum range.
		 * 		Returns the original value if it is between the min and max.
		 * @param value 
		 * @param min 
		 * @param max 
		 * @return T 
		 */
		template<typename T>
		static T clamp(T value, T min, T max)
		{
			if(value <= min)
				return min;
			if(value >= max)
				return max;
			return value;
		}

		/**
		 * @brief Converts a radian value to a degree value.
		 * 
		 * @param rad 
		 * @return float 
		 */
		static float toDeg(float rad);

		/**
		 * @brief Converts a radian value to a degree value.
		 * 
		 * @param rad 
		 * @return double 
		 */
		static double toDeg(double rad);

		/**
		 * @brief Converts a degree value to a radian value.
		 * 
		 * @param deg 
		 * @return float 
		 */
		static float toRad(float deg);

		/**
		 * @brief Converts a degree value to a radian value.
		 * 
		 * @param deg 
		 * @return double 
		 */
		static double toRad(double deg);

		/**
		 * @brief Converts an angle to the standard range.
		 * 		Standard range for degrees is defined here as [0, 360)
		 * 		Standard range for radians is defined here as [0, 2*PI)
		 * 		Neither include the maximum value in the range.
		 * 
		 * @param angle 
		 * @param isDegree 
		 * 		Determines if the angle is in degrees or in radians.
		 * @return float 
		 */
		static float angleToStandardRange(float angle, bool isDegree);

		/**
		 * @brief Converts an angle to the standard range.
		 * 		Standard range for degrees is defined here as [0, 360)
		 * 		Standard range for radians is defined here as [0, 2*PI)
		 * 		Neither include the maximum value in the range.
		 * 
		 * @param angle 
		 * @param isDegree 
		 * 		Determines if the angle is in degrees or in radians.
		 * @return double 
		 */
		static double angleToStandardRange(double angle, bool isDegree);
		
		/**
		 * @brief Returns the cosine of the value.
		 * 
		 * @param value 
		 * @return float 
		 */
		static float cos(float value);

		/**
		 * @brief Returns the cosine of the value.
		 * 
		 * @param value 
		 * @return double 
		 */
		static double cos(double value);

		/**
		 * @brief Returns the cosine of the value converting the value to radians before solving.
		 * 		Useful if the value is in degrees.
		 * 
		 * @param value 
		 * @return float 
		 */
		static float dcos(float value);

		/**
		 * @brief Returns the cosine of the value converting the value to radians before solving.
		 * 		Useful if the value is in degrees.
		 * 
		 * @param value 
		 * @return double 
		 */
		static double dcos(double value);

		/**
		 * @brief Returns the sine of the value.
		 * 
		 * @param value 
		 * @return float 
		 */
		static float sin(float value);

		/**
		 * @brief Returns the sine of the value.
		 * 
		 * @param value 
		 * @return double 
		 */
		static double sin(double value);
		
		/**
		 * @brief Returns the sine of the value converting the value to radians before solving.
		 * 		Useful if the value is in degrees.
		 * 
		 * @param value 
		 * @return float 
		 */
		static float dsin(float value);

		/**
		 * @brief Returns the sine of the value converting the value to radians before solving.
		 * 		Useful if the value is in degrees.
		 * 
		 * @param value 
		 * @return double 
		 */
		static double dsin(double value);

		/**
		 * @brief Returns the tangent of the value.
		 * 
		 * @param value 
		 * @return float 
		 */
		static float tan(float value);

		/**
		 * @brief Returns the tangent of the value.
		 * 
		 * @param value 
		 * @return double 
		 */
		static double tan(double value);

		/**
		 * @brief Returns the tangent of the value converting the value to radians before solving.
		 * 		Useful if the value is in degrees.
		 * 
		 * @param value 
		 * @return float 
		 */
		static float dtan(float value);

		/**
		 * @brief Returns the tangent of the value converting the value to radians before solving.
		 * 		Useful if the value is in degrees.
		 * 
		 * @param value 
		 * @return double 
		 */
		static double dtan(double value);

		/**
		 * @brief Returns the ArcCosine or inverse of the value.
		 * 
		 * @param value 
		 * @return float 
		 * 		Returns an angle in radians
		 */
		static float arccos(float value);

		/**
		 * @brief Returns the ArcCosine or inverse of the value.
		 * 
		 * @param value 
		 * @return double 
		 * 		Returns an angle in radians
		 */
		static double arccos(double value);

		/**
		 * @brief Returns the ArcCosine or inverse of the value.
		 * 
		 * @param value 
		 * @return float 
		 * 		Returns an angle in degrees
		 */
		static float darccos(float value);

		/**
		 * @brief Returns the ArcCosine or inverse of the value.
		 * 
		 * @param value 
		 * @return double 
		 * 		Returns an angle in degrees
		 */
		static double darccos(double value);

		/**
		 * @brief Returns the ArcSine or inverse of the value.
		 * 
		 * @param value 
		 * @return float 
		 * 		Returns an angle in radians
		 */
		static float arcsin(float value);

		/**
		 * @brief Returns the ArcSine or inverse of the value.
		 * 
		 * @param value 
		 * @return double 
		 * 		Returns an angle in radians
		 */
		static double arcsin(double value);

		/**
		 * @brief Returns the ArcSine or inverse of the value.
		 * 
		 * @param value 
		 * @return float 
		 * 		Returns an angle in degrees
		 */
		static float darcsin(float value);

		/**
		 * @brief Returns the ArcSine or inverse of the value.
		 * 
		 * @param value 
		 * @return double 
		 * 		Returns an angle in degrees
		 */
		static double darcsin(double value);

		/**
		 * @brief Returns the ArcTangent or inverse of the value.
		 * 
		 * @param value 
		 * @return float 
		 * 		Returns an angle in radians
		 */
		static float arctan(float value);

		/**
		 * @brief Returns the ArcTangent or inverse of the value.
		 * 
		 * @param value 
		 * @return double 
		 * 		Returns an angle in radians
		 */
		static double arctan(double value);

		/**
		 * @brief Returns the ArcTangent or inverse of the value.
		 * 
		 * @param value 
		 * @return float 
		 * 		Returns an angle in degrees
		 */
		static float darctan(float value);

		/**
		 * @brief Returns the ArcTangent or inverse of the value.
		 * 
		 * @param value 
		 * @return double 
		 * 		Returns an angle in degrees
		 */
		static double darctan(double value);
		
		/**
		 * @brief Returns the ArcTangent avoiding divide by zero errors and accounting correctly for
		 * 		the quadrant the point is in.
		 * 
		 * @param y 
		 * @param x 
		 * @return float 
		 * 		Returns an angle in radians
		 */
		static float arctan2(float y, float x);

		/**
		 * @brief Returns the ArcTangent avoiding divide by zero errors and accounting correctly for
		 * 		the quadrant the point is in.
		 * 
		 * @param y 
		 * @param x 
		 * @return double 
		 * 		Returns an angle in radians
		 */
		static double arctan2(double y, double x);

		/**
		 * @brief Returns the ArcTangent avoiding divide by zero errors and accounting correctly for
		 * 		the quadrant the point is in.
		 * 
		 * @param y 
		 * @param x 
		 * @return float 
		 * 		Returns an angle in degrees
		 */
		static float darctan2(float y, float x);

		/**
		 * @brief Returns the ArcTangent avoiding divide by zero errors and accounting correctly for
		 * 		the quadrant the point is in.
		 * 
		 * @param y 
		 * @param x 
		 * @return double 
		 * 		Returns an angle in degrees
		 */
		static double darctan2(double y, double x);

		/**
		 * @brief Returns the direction from x,y to x2,y2 in radians
		 * 
		 * @param x 
		 * @param y 
		 * @param x2 
		 * @param y2 
		 * @return double 
		 */
		static double dirToPoint(double x, double y, double x2, double y2);

		/**
		 * @brief Returns the direction from x,y to x2,y2 in degrees
		 * 
		 * @param x 
		 * @param y 
		 * @param x2 
		 * @param y2 
		 * @return double 
		 */
		static double dirToPointDeg(double x, double y, double x2, double y2);

		/**
		 * @brief Returns if the testAngle is between the startAngle and endAngle. 
		 * 		Expects the angles to be in radians.
		 * 
		 * @param testAngle 
		 * @param startAngle 
		 * @param endAngle 
		 * @param clockwise 
		 * 		Whether to check in the clockwise direction or counter clockwise direction.
		 * @return true 
		 * @return false 
		 */
		static bool angleBetween(double testAngle, double startAngle, double endAngle, bool clockwise);

		/**
		 * @brief Returns if the testAngle is between the startAngle and endAngle. 
		 * 		Expects the angles to be in degrees.
		 * 
		 * @param testAngle 
		 * @param startAngle 
		 * @param endAngle 
		 * @param clockwise 
		 * 		Whether to check in the clockwise direction or counter clockwise direction.
		 * @return true 
		 * @return false 
		 */
		static bool angleBetweenDeg(double testAngle, double startAngle, double endAngle, bool clockwise);
		
		/**
		 * @brief Returns which of the 2 angles are the closest to the test angle.
		 * 		Expects the angles to be in radians.
		 * 
		 * @param testAngle 
		 * @param angle1 
		 * @param angle2 
		 * @return double 
		 */
		static double closestAngle(double testAngle, double angle1, double angle2);

		/**
		 * @brief Returns which of the 2 angles are the closest to the test angle.
		 * 		Expects the angles to be in degrees.
		 * 
		 * @param testAngle 
		 * @param angle1 
		 * @param angle2 
		 * @return double 
		 */
		static double closestAngleDeg(double testAngle, double angle1, double angle2);
		
		/**
		 * @brief Clamps the test angle between the start angle and end angle.
		 * 		Expects the angles to be in radians.
		 * 
		 * @param testAngle 
		 * @param startAngle 
		 * @param endAngle 
		 * @param clockwise 
		 * 		Whether to check in the clockwise direction or counter clockwise direction.
		 * @return double 
		 */
		static double angleClamp(double testAngle, double startAngle, double endAngle, bool clockwise);

		/**
		 * @brief Clamps the test angle between the start angle and end angle.
		 * 		Expects the angles to be in degrees.
		 * 
		 * @param testAngle 
		 * @param startAngle 
		 * @param endAngle 
		 * @param clockwise 
		 * 		Whether to check in the clockwise direction or counter clockwise direction.
		 * @return double 
		 */
		static double angleClampDeg(double testAngle, double startAngle, double endAngle, bool clockwise);
		
		/**
		 * @brief Returns 2 values that represent the range of all angles between the 2 values.
		 * 		This has the option of returning the smallest angle range or largest angle range.
		 * 		The output may or may not match the input values.
		 * 		Returns answer in radians and may not be in standard range.
		 * 
		 * @param angle1 
		 * @param angle2 
		 * @param smallest 
		 * @return Vec2f 
		 */
		static Vec2f angleRange(double angle1, double angle2, bool smallest=true);
		
		/**
		 * @brief Returns 2 values that represent the range of all angles between the 2 values.
		 * 		This has the option of returning the smallest angle range or largest angle range.
		 * 		The output may or may not match the input values.
		 * 		Expects the angles to be in degrees and will return degrees as well and may not be in standard range.
		 * 
		 * @param angle1 
		 * @param angle2 
		 * @param smallest 
		 * @return Vec2f 
		 */
		static Vec2f angleRangeDeg(double angle1, double angle2, bool smallest=true);

		/**
		 * @brief Returns a Vec2f that has the specified length and direction.
		 * 
		 * @param length 
		 * @param direction 
		 * @return Vec2f 
		 */
		static Vec2f lengthDir(double length, double direction, bool counterClockwise=false);
		
		static Vec3f sphericalCoord(double length, double theta, double phi);

		/**
		 * @brief Normalizes the Quaternion.
		 * 
		 * @param q1 
		 * @return Quaternion
		 */
		static Quaternion normalize(Quaternion q1);

		/**
		 * @brief Returns the distance from (x1,y1) to (x2,y2)
		 * 
		 * @param x1 
		 * @param y1 
		 * @param x2 
		 * @param y2 
		 * @return double 
		 */
		static double distanceTo(double x1, double y1, double x2, double y2);

		/**
		 * @brief Returns the distance from p1 to p2.
		 * 
		 * @param p1 
		 * @param p2 
		 * @return double 
		 */
		static double distanceTo(Vec2f p1, Vec2f p2);

		/**
		 * @brief Returns the distance from p1 to p2.
		 * 
		 * @param p1 
		 * @param p2 
		 * @return double 
		 */
		static double distanceTo(Vec3f p1, Vec3f p2);

		/**
		 * @brief Returns the distance from p1 to p2.
		 * 
		 * @param p1 
		 * @param p2 
		 * @return double 
		 */
		static double distanceTo(Vec4f p1, Vec4f p2);

		/**
		 * @brief Returns the distance from p1 to p2.
		 * 
		 * @param p1 
		 * @param p2 
		 * @return double 
		 */
		static double distanceTo(PolarCoordinate p1, PolarCoordinate p2);
		
		//Matrix operations

		//2D transformations
		/**
		 * @brief Returns a 2D rotation matrix.
		 * 
		 * @param rotation 
		 * 		Radians are expected
		 * @return Mat3f 
		 */
		static Mat3f rotation2D(double rotation);

		/**
		 * @brief Returns a 2D rotation matrix about a specific point.
		 * 
		 * @param rotation 
		 * 		Radians are expected
		 * @param x
		 * @param y
		 * @return Mat3f 
		 */
		static Mat3f rotation2D(double rotation, double x, double y);

		/**
		 * @brief Returns a 2D rotation matrix about a specific point.
		 * 
		 * @param rotation 
		 * 		Radians are expected
		 * @param pos
		 * @return Mat3f 
		 */
		static Mat3f rotation2D(double rotation, Vec2f pos);
		
		/**
		 * @brief Returns a 2D translation matrix
		 * 
		 * @param x 
		 * @param y 
		 * @return Mat3f 
		 */
		static Mat3f translation2D(double x, double y);

		/**
		 * @brief Returns a 2D translation matrix
		 * 
		 * @param pos 
		 * @return Mat3f 
		 */
		static Mat3f translation2D(Vec2f pos);

		/**
		 * @brief Returns a 2D scale matrix.
		 * 
		 * @param x 
		 * @param y 
		 * @return Mat3f 
		 */
		static Mat3f scale2D(double x, double y);

		/**
		 * @brief Returns a 2D scale matrix
		 * 
		 * @param scaleVals 
		 * @return Mat3f 
		 */
		static Mat3f scale2D(Vec2f scaleVals);

		/**
		 * @brief Returns a 2D skew matrix
		 * 
		 * @param x 
		 * @param y 
		 * @return Mat3f 
		 */
		static Mat3f skew2D(double x, double y);

		/**
		 * @brief Returns a 2D skew matrix
		 * 
		 * @param skew 
		 * @return Mat3f 
		 */
		static Mat3f skew2D(Vec2f skew);

		/**
		 * @brief Creates a Matrix to transform a model about the origin (0,0)
		 * 		Applies transforms in the following order
		 * 		Scale, Rotation, Position
		 * 
		 * @param position 
		 * @param rotation 
		 * @param scale 
		 * @return Mat3f 
		 */
		static Mat3f modelMatrix2D(Vec2f position, double rotation, Vec2f scale);

		//3D transformations

		/**
		 * @brief Gets a quaternion that rotates about the specified axis by a specified amount.
		 * 
		 * @param rotation 
		 * 		Radians are expected.
		 * @param xAxis 
		 * @param yAxis 
		 * @param zAxis 
		 * @return Quaternion 
		 */
		static Quaternion getRotationQuaternion(double rotation, double xAxis, double yAxis, double zAxis);

		/**
		 * @brief Gets a quaternion that rotates about the specified axis by a specified amount.
		 * 
		 * @param rotation 
		 * 		Radians are expected.
		 * @param rotationAxis
		 * @return Quaternion 
		 */
		static Quaternion getRotationQuaternion(double rotation, Vec3f rotationAxis);
		
		/**
		 * @brief Converts a Quaternion to a Matrix.
		 * 
		 * @param q 
		 * @return Mat4f 
		 */
		static Mat4f quaternionToMatrix(Quaternion q);
		
		/**
		 * @brief Returns a 3D rotation matrix about the X axis.
		 * 
		 * @param rotation 
		 * 		Radians are expected
		 * @return Mat4f 
		 */
		static Mat4f rotation3DX(double rotation);

		/**
		 * @brief Returns a 3D rotation matrix about the Y axis.
		 * 
		 * @param rotation 
		 * 		Radians are expected
		 * @return Mat4f 
		 */
		static Mat4f rotation3DY(double rotation);
		
		/**
		 * @brief Returns a 3D rotation matrix about the Z axis.
		 * 
		 * @param rotation 
		 * 		Radians are expected
		 * @return Mat4f 
		 */
		static Mat4f rotation3DZ(double rotation);

		/**
		 * @brief Returns a 3D translation matrix 
		 * 
		 * @param x 
		 * @param y 
		 * @param z 
		 * @return Mat4f 
		 */
		static Mat4f translation3D(double x, double y, double z);

		/**
		 * @brief Returns a 3D translation matrix
		 * 
		 * @param pos 
		 * @return Mat4f 
		 */
		static Mat4f translation3D(Vec3f pos);

		/**
		 * @brief Returns a 3D scale matrix
		 * 
		 * @param x 
		 * @param y 
		 * @param z 
		 * @return Mat4f 
		 */
		static Mat4f scale3D(double x, double y, double z);

		/**
		 * @brief Returns a 3D scale matrix
		 * 
		 * @param scale
		 * @return Mat4f 
		 */
		static Mat4f scale3D(Vec3f scale);

		/**
		 * @brief Returns a 3D skew matrix
		 * 
		 * @param x 
		 * @param y 
		 * @param z 
		 * @return Mat4f 
		 */
		static Mat4f skew3D(double x, double y, double z);

		/**
		 * @brief Returns a 3D skew matrix
		 * 
		 * @param skew
		 * @return Mat4f 
		 */
		static Mat4f skew3D(Vec3f skew);

		/**
		 * @brief Creates a Matrix to transform a model about the origin (0,0,0)
		 * 		Applies transforms in the following order
		 * 		Scale, Rotation, Position
		 * 
		 * 		Rotations are created using Quaternions to avoid gimbal lock
		 * 
		 * @param position 
		 * @param rotation 
		 * @param scale 
		 * @return Mat4f 
		 */
		static Mat4f modelMatrix3D(Vec3f position, Vec3f rotation, Vec3f scale);


		//PROJECTION VIEW MATRIX STUFF
		
		/**
		 * @brief Calculates a 3D View Matrix that assumes that 
		 * 		x is fowards and backwards (x+ is forwards), 
		 * 		y is left and right (y+ is right),
		 * 		z is up and down (z+ is up).
		 * 		
		 * 		It uses quaternions to avoid Gimbal lock.
		 * 
		 * @param position 
		 * @param rotation 
		 * 		Assumed to be in radians
		 * @param counterClockwise
		 * 		Allows the ability to flip y+ so that y+ is left
		 * 		and y- is right.
		 * 		By default, it is false
		 * @return Mat4f 
		 */
		static Mat4f viewMatrix(Vec3f position, Vec3f rotation, bool counterClockwise = false);

		static Mat4f lookAtMatrix(Vec3f eyePos, Vec3f toPos, Vec3f upVec);

		/**
		 * @brief Calculates a 2D View Matrix that assumes that 
		 * 		x is left and right,
		 * 		y is up and down
		 * 
		 * @param position 
		 * @param rotation 
		 * 		Assumed to be in radians
		 * @return Mat4f 
		 */
		static Mat4f viewMatrix2D(Vec2f position, double rotation);

		/**
		 * @brief Calculates a 3D Perspective Projection Matrix.
		 * 
		 * @param width 
		 * @param height 
		 * @param near 
		 * @param far 
		 * @param fov 
		 * @return Mat4f 
		 */
		static Mat4f perspectiveProjectionMatrix(double width, double height, double near, double far, double fov);

		/**
		 * @brief Calculates a 3D Orthographic Projection Matrix.
		 * 
		 * 		If near and far are set to the same value or left at the default, a Orthographic Projection Matrix
		 * 		for 2D will be generated. Z values in that case will represent depth.
		 * 
		 * @param width 
		 * @param height 
		 * @param near
		 * 		Default value is 0
		 * @param far
		 * 		Default value is 0
		 * @return Mat4f 
		 */
		static Mat4f orthographicProjectionMatrix(double width, double height, double near = 0, double far = 0);
		
		/**
		 * @brief Calculates a 3D Orthographic Projection Matrix.
		 * 		This method uses left, right, top, and bottom to refer to the scene bounding box.
		 * 
		 * 		If near and far are set to the same value or left at the default, a Orthographic Projection Matrix
		 * 		for 2D will be generated. Z values in that case will represent depth.
		 * 
		 * @param left 
		 * @param right 
		 * @param top 
		 * @param bottom 
		 * @param near
		 * 		Default value is 0
		 * @param far
		 * 		Default value is 0
		 * @return Mat4f 
		 */
		static Mat4f orthographicProjectionMatrix2(double left, double right, double top, double bottom, double near = 0, double far = 0);
		

		//Equations and Functions

		/**
		 * @brief Solves a Linear equation in the form:
		 * 		Ax + B = 0
		 * 
		 * @param A 
		 * @param B 
		 * @return std::vector<double> 
		 * 		Returns all solutions found.
		 */
		static std::vector<double> solveLinear(double A, double B);

		/**
		 * @brief Solves a Quadratic equation in the form:
		 * 		Ax^2 + Bx + C = 0
		 * 
		 * @param A 
		 * @param B 
		 * @param C 
		 * @return std::vector<ComplexNumber> 
		 * 		Returns all solutions found.
		 */
		static std::vector<ComplexNumber> solveQuadratic(double A, double B, double C);

		/**
		 * @brief Solves a Cubic equation in the form:
		 * 		Ax^3 + Bx^2 + Cx + D = 0
		 * 
		 * @param A 
		 * @param B 
		 * @param C 
		 * @param D 
		 * @return std::vector<ComplexNumber> 
		 * 		Returns all solutions found.
		 */
		static std::vector<ComplexNumber> solveCubic(double A, double B, double C, double D);

		/**
		 * @brief Solve a Quadratic equation but only returns real solutions
		 * 		Assumes the form Ax^2 + Bx + C
		 * 
		 * @param A 
		 * @param B 
		 * @param C 
		 * @return std::vector<double> 
		 * 		Returns all solutions found.
		 */
		static std::vector<double> solveQuadraticReal(double A, double B, double C);

		/**
		 * @brief Solves a Cubic equation but only returns real solutions.
		 * 		Assumes the form Ax^3 + Bx^2 + Cx + D
		 * 
		 * @param A 
		 * @param B 
		 * @param C 
		 * @param D 
		 * @return std::vector<double> 
		 * 		Returns all solutions found.
		 */
		static std::vector<double> solveCubicReal(double A, double B, double C, double D);
		
		/**
		 * @brief Reduces a polynomial to a lower degree using a known zero.
		 * 
		 * @param constants 
		 * @param zero 
		 * 		A known solution that solves the polynomial for 0.
		 * @return std::vector<double> 
		 * 		The constants for the new polynomial.
		 */
		static std::vector<double> reducePolynomial(std::vector<double> constants, double zero);

		/**
		 * @brief Reduces a polynomial to a lower degree using a known zero.
		 * 
		 * @param f 
		 * @param zero 
		 * 		A known solution that solves the polynomial for 0.
		 * @return PolynomialMathFunction 
		 */
		static PolynomialMathFunction reducePolynomial(PolynomialMathFunction f, double zero);

		/**
		 * @brief Solves a MathFunction for 0 using the Bisection Numerical Method.
		 * 		Bisection requires that f(a)*f(b) < 0 to work.
		 * 		It also requires that f(c)==0 for some c value in [a, b].
		 * 			the solution must be within the specified range.
		 * 
		 * 		Max Iterations is used to prevent the algorithm from running forever.
		 * 		The method approximate the solution and will eventually converge to the correct answer.
		 * 		
		 * 		This will only converge on one solution at a time.
		 * @param f 
		 * 		The math function to solve
		 * @param a 
		 * 		a does not have to be less than b meaning
		 * 		If f(a) > f(b), the a and b values will be swapped internally.
		 * @param b 
		 * 		b does not have to be greater than a meaning
		 * 		If f(a) > f(b), the a and b values will be swapped internally.
		 * @param maxIterations 
		 * 		The maximum amount of iterations to solve for 0.
		 * 		If the solution is not found before the maximum number of iterations,
		 * 		the an approximation is returned.
		 * 		Default value is 10.
		 * @return double 
		 * 		If solution is not possible with the given parameters, NAN is returned
		 */
		static double bisectionMethod(MathFunction* f, double a, double b, int maxIterations = 10);

		/**
		 * @brief Solves a MathFunction for 0 using Newton's Method.
		 * 		Newton's Method requires the derivative of the function and a decent starting point to work.
		 * 		If the derivative at any point when solving is 0, the function will fail.
		 * 		Also requires that the solution exists.
		 * 		
		 * 		This will only converge on one solution at a time.
		 * @param f 
		 * 		The math function to solve
		 * @param derivative 
		 * 		The derivative of the math function to solve
		 * @param startPoint 
		 * 		A starting point to use for approximating the solution.
		 * 		Better starting points results in faster convergence and more accurate solution when bound
		 * 		by a maximum number of iterations.
		 * @param maxIterations 
		 * 		The maximum amount of iterations to solve for 0.
		 * 		If the solution is not found before the maximum number of iterations,
		 * 		the an approximation is returned.
		 * 		Default value is 10.
		 * @return double 
		 * 		If solution is not possible with the given parameters, NAN is returned
		 */
		static double newtonsMethod(MathFunction* f, MathFunction* derivative, double startPoint, int maxIterations = 10);

		/**
		 * @brief Solves a MathFunction for 0 using Secant Method.
		 * 		It is an approximation for Newton's Method when the derivative is not available.
		 * 		It requires 2 initial values that are idealy close to the solution.
		 * 
		 * 		While it converges slower, it can be computationly faster due to not needing a derivative.
		 * 		It can also fail to converge just like newton's method for similar reasons.
		 * 		If a and b are not close enough to the solution, the method may not converge.
		 * 		
		 * 		This will only converge on one solution at a time.
		 * @param f 
		 * 		The math function to solve
		 * @param a 
		 * 		The first initial value
		 * @param b 
		 * 		The second initial value
		 * @param maxIterations 
		 * 		The maximum amount of iterations to solve for 0.
		 * 		If the solution is not found before the maximum number of iterations,
		 * 		the an approximation is returned.
		 * 		Default value is 10.
		 * @return double 
		 * 		If solution is not possible with the given parameters, NAN is returned
		 */
		static double secantMethod(MathFunction* f, double a, double b, int maxIterations = 10);

		/**
		 * @brief Creates a polynomial of n degree that closely represents the data points.
		 * 
		 * @param points 
		 * 		The x,y points of the sampled data.
		 * @param degree 
		 * 		The degree of the polynomial.
		 * 		Must be 1 or more.
		 * @return PolynomialMathFunction 
		 */
		static PolynomialMathFunction linearRegression(std::vector<Vec2f> points, int degree);

		/**
		 * @brief Fits a polynomial of at most n degree that crosses all points where n is the number of input data points.
		 * 		Can be less that n degree.
		 * 		May fail if it is not possible to fit a polynomial that crosses all points.
		 * 
		 * @param points 
		 * @return PolynomialMathFunction 
		 */
		static PolynomialMathFunction fitPolynomial(std::vector<Vec2f> points);

		/**
		 * @brief Finds the intersection between the 2 quadratic functions
		 * 		Provided that they actually intersect.
		 * 
		 * @param A1 
		 * @param B1 
		 * @param C1 
		 * @param A2 
		 * @param B2 
		 * @param C2 
		 * @return std::vector<double> 
		 */
		static std::vector<double> getIntersectionQuadratic(double A1, double B1, double C1, double A2, double B2, double C2);
		static std::vector<double> getIntersectionQuadratic(PolynomialMathFunction a, PolynomialMathFunction b);

		/**
		 * @brief Creates the chebyshev polynomial of the first or second kind of the specified degree.
		 * 		It follows the following pattern
		 * 			T[0] = 1
		 * 			T[1] = x
		 * 			T[2] = 2x^2 - 1
		 * 			T[n+1] = 2xT[n] - T[n-1]
		 * 
		 * @param degree 
		 * @return PolynomialMathFunction 
		 */
		static PolynomialMathFunction chebyshevPolynomial(int degree, bool secondKind = false);

		/**
		 * @brief Attempts to approximate the given math function over some interval [a, b]
		 * 		with chebyshev polynomial approximation to some specified degree.
		 * 		Must be continous over the specified interval.
		 * 
		 * @param degree 
		 * @param secondKind 
		 * @return PolynomialMathFunction 
		 */
		static PolynomialMathFunction chebyshevApproximation(MathFunction* f, double a, double b, int degree, bool secondKind = false);
		
		/**
		 * @brief Fits a circle to either the 3 points of the triangle or 
		 * 		fits the largest circle inside of the triangle.
		 * 		Returns 3 values.
		 * 			x, y, radius
		 * 
		 * @param p1 
		 * @param p2 
		 * @param p3 
		 * @param inside 
		 * @return Vec3f 
		 */
		static Vec3f fitCircleToTriangle(Vec2f p1, Vec2f p2, Vec2f p3, bool inside);

		/**
		 * @brief Returns the factorial of the specified input
		 * 
		 * @param k 
		 * @return int 
		 */
		static int factorial(int k);
		
		/**
		 * @brief Returns the binomial coefficient using n and k.
		 * 
		 * @param n 
		 * @param k 
		 * @return double 
		 */
		static double binomialCoefficient(int n, int k);

		/**
		 * @brief Returns the solution to the basic logistics sigmoid function
		 * 		Defined as: 
		 * 			1 / (1 + e^(-x) )
		 * 
		 * @param x 
		 * @return double 
		 */
		static double logisticsSigmoid(double x);


		/**
		 * @brief Computes the Discrete Fourier Transform using the definition.
		 * 		Runs in O(N) time.
		 * 
		 * @param arr 
		 * 		An array of Complex Numbers to perform the Fourier Transform on.
		 * 		If not doing the inverse, the imaginary values can be ignored and set to 0.
		 * @param size 
		 * 		The size of the array.
		 * @param x 
		 * 		The frequency to solve for.
		 * @param inverse 
		 * 		Whether to solve for the inverse.
		 * 		Default is false.
		 * @return ComplexNumber 
		 */
		static ComplexNumber discreteFourierTransform(ComplexNumber* arr, int size, double x, bool inverse=false);

		/**
		 * @brief Computes the Discrete Fourier Transform using the definition for all possible frequencies.
		 * 		Runs in O(N^2) time.
		 * 		The vector returned is the same size as the input array.
		 * 
		 * @param arr 
		 * 		An array of Complex Numbers to perform the Fourier Transform on.
		 * 		If not doing the inverse, the imaginary values can be ignored and set to 0.
		 * @param size 
		 * 		The size of the array.
		 * @param inverse 
		 * 		Whether to solve for the inverse.
		 * 		Default is false.
		 * @return std::vector<ComplexNumber>
		 */
		static std::vector<ComplexNumber> fourierTransform(ComplexNumber* arr, int size, bool inverse=false);

		/**
		 * @brief Computes the Fast Discrete Fourier Transform using an adaptation of the Cooley Tukey algorithm
		 * 		Runs in O(NLogN) time.
		 * 		The input is highly encouraged to be a power of 2 already. If it is not, it will be expanded to be a power of 2 by copying data to a new buffer.
		 * 			If the size is already a power of 2, the copying process is skipped.
		 * 		The output will be a power of 2.
		 * 		
		 * 		Reference: 
		 * 			(https://cp-algorithms.com/algebra/fft.html#improved-implementation-in-place-computation)
		 * 
		 * @param arr 
		 * 		A column matrix of Complex Numbers to perform the Fourier Transform on.
		 * 		If not doing the inverse, the imaginary values can be ignored and set to 0.
		 * 			Otherwise, the imaginary values must be provided to give back the correct result.
		 * @param size 
		 * 		The size of the array.
		 * @param inverse 
		 * 		Whether to solve for the inverse.
		 * 		Default is false.
		 * @return Matrix<ComplexNumber> 
		 */
		static Matrix<ComplexNumber> fastFourierTransform(const Matrix<ComplexNumber>& arr, bool inverse=false);
		
		/**
		 * @brief Computes the Fast Discrete Fourier Transform using an adaptation of the Cooley Tukey algorithm
		 * 		Runs in O(NLogN) time.
		 * 		Fails if the input matrix's size is not a power of 2 and the input matrix is not a column matrix.
		 * 		This performs the operation on arr itself and therefore modifies it.
		 * 		Unlike the normal version, this version CAN NOT just copy data into a new buffer and therefore requires proper sizing beforehand.
		 * 		
		 * 		Reference: 
		 * 			(https://cp-algorithms.com/algebra/fft.html#improved-implementation-in-place-computation)
		 * 
		 * 
		 * @param arr 
		 * @param size 
		 * @param inverse 
		 * @return void
		 */
		static void fastFourierTransformInline(const Matrix<ComplexNumber>& arr, bool inverse=false);

		/**
		 * @brief Computes the 2D Fast Fourier Transform utilizing the 1D versions.
		 * 		Highly encourages a matrix that is a power of 2. Meaning the rows and columns must be a valid power of 2
		 * 			Note that the matrix does not need to be square.
		 * 		If the matrix rows and columns aren't the required size, they will be padded with 0s to the correct size
		 * 			and the output will reflect that.
		 * 
		 * @param mat 
		 * @param inverse 
		 * @return Matrix<ComplexNumber> 
		 */
		static Matrix<ComplexNumber> fastFourierTransform2D(const Matrix<ComplexNumber>& mat, bool inverse=false);

		/**
		 * @brief Computes the 2D Fast Fourier Transform utilizing the 1D versions and modifying the input matrix itself.
		 * 		Requires a matrix that is a power of 2. Meaning the rows and columns must be a valid power of 2
		 * 			Note that the matrix does not need to be square.
		 * 		THIS METHOD WILL NOT WORK WITHOUT THE PROPER SIZE.
		 * 		
		 * 		Returns if the operation is successful.
		 * 
		 * 		Useful when you don't need the original data and only need the FFT / want to minimize the total number of copies created.
		 * @param mat 
		 * @param inverse 
		 * @return true 
		 * @return false 
		 */
		static bool fastFourierTransform2DInline(Matrix<ComplexNumber>& mat, bool inverse = false);
		
		/**
		 * @brief Computes the Discrete Cosine Transform on the array.
		 * 		Runs in O(N) time. Solves for a single point.
		 * 
		 * 		Also known as DCT-II
		 * 
		 * @param arr
		 * @param size 
		 * @param u 
		 * @param inverse 
		 * 		Whether to solve for the inverse.
		 * 		Default is false.
		 * @return double 
		 */
		static double discreteCosineTransform(double* arr, int size, int u, bool inverse=false);
		static float discreteCosineTransform(float* arr, int size, int u, bool inverse=false);

		/**
		 * @brief Computes the Discrete Cosine Transform for all possible values.
		 * 		Runs in O(N^2) time.
		 * 
		 * @param arr 
		 * @param size 
		 * @param inverse 
		 * 		Whether to solve for the inverse.
		 * 		Default is false.
		 * @return std::vector<double> 
		 */
		static std::vector<double> cosineTransform(double* arr, int size, bool inverse=false);
		static std::vector<float> cosineTransform(float* arr, int size, bool inverse=false);

		/**
		 * @brief Computes the Discrete Cosine Transform for all possible values.
		 * 		Runs in O(NlogN)
		 * 		Can fail if the size is not a power of 2.
		 * 			Returns an empty vector if it fails.
		 * 
		 * 		References:
		 * 			(https://www.nayuki.io/res/fast-discrete-cosine-transform-algorithms/lee-new-algo-discrete-cosine-transform.pdf)
		 * 			(https://www.nayuki.io/page/fast-discrete-cosine-transform-algorithms)
		 * @param arr 
		 * @param size 
		 * 		Must be a power of 2
		 * @param inverse 
		 * 		Whether to solve for the inverse.
		 * 		Default is false.
		 * @return std::vector<float> 
		 */
		static std::vector<float> fastCosineTransform(float* arr, size_t size, bool inverse=false);

		/**
		 * @brief Computes the 2D Discrete Cosine Transform for all possible locations.
		 * 
		 * @param arr 
		 * @param inverse 
		 * 		Whether to solve for the inverse.
		 * 		Default is false.
		 * @return double 
		 */
		static MatrixF cosineTransform2D(const MatrixF& arr, bool inverse=false);
		static MatrixD cosineTransform2D(const MatrixD& arr, bool inverse=false);

		/**
		 * @brief Computes the 2D Discrete Cosine Transform for all possible locations.
		 * 		Uses the FCT (Fast Cosine Transform) algorithm to speed up computation.
		 * 
		 * @param arr 
		 * @param size 
		 * @param inverse 
		 * @return std::vector<double> 
		 */
		static MatrixF fastCosineTransform2D(const MatrixF& arr, bool inverse=false);
		static MatrixD fastCosineTransform2D(const MatrixD& arr, bool inverse=false);
		
		
		//Special Case for JPEG
		/**
		 * @brief Computes the Fast Discrete Cosine Transform for a list of 8 values.
		 * 		Special case.
		 * 
		 * 		New version comes from https://github.com/norishigefukushima/dct_simd/blob/master/dct/dct8x8_simd.cpp#L410
		 * 			Based off of https://ieeexplore.ieee.org/document/266596
		 * 				Practical_fast_1-D_DCT_algorithms_with_11_multiplications
		 * 
		 * @param arr 
		 * 		The input array
		 * @param output 
		 * 		The output array
		 * @param inverse 
		 * 		Whether to solve for the inverse.
		 * 		Default is false.
		 */
		static void FCT8(float* arr, float* output, bool inverse=false);

		/**
		 * @brief Computes the Fast Discrete Cosine Transform for an 8x8 matrix.
		 * 		Special case.
		 * 
		 * @param arr 
		 * 		The input Matrix
		 * @param output 
		 * 		The output Matrix
		 * @param inverse 
		 * 		Whether to solve for the inverse.
		 * 		Default is false.
		 */
		static void FCT8x8(const MatrixF& arr, MatrixF* output, bool inverse=false);
		static void FCT8x8(const MatrixD& arr, MatrixD* output, bool inverse=false);


		/**
		 * @brief Calculates the Discrete Sine Transform using DST-I
		 * 		This formula is muliplied by sqrt(2.0 / (size+1)) so it can also calculate the inverse without changing anything.
		 * 		
		 * 		Reference: 
		 * 			(https://en.wikipedia.org/wiki/Discrete_sine_transform)
		 * 
		 * @param size 
		 * @param u 
		 * @return double 
		 */
		static double discreteSineTransform(double*, size_t size, size_t u);
		static float discreteSineTransform(float*, size_t size, size_t u);

		/**
		 * @brief Calculates the Discrete Sine Transform using DST-I
		 * 		Calls discreteSineTransform multiple times.
		 * 		Calculates the inverse without needing additional parameters.
		 * 		Runs in O(N^2) time
		 * 
		 * @param arr 
		 * @param size 
		 * @return std::vector<double> 
		 */
		static std::vector<double> sineTransform(double* arr, size_t size);
		static std::vector<float> sineTransform(float* arr, size_t size);

		/**
		 * @brief NOT IMPLEMENTED
		 * 
		 * @param arr 
		 * @param size 
		 * @param inverse 
		 * @return std::vector<float> 
		 */
		static std::vector<float> fastSineTransform(float* arr, size_t size, bool inverse=false);

		/**
		 * @brief Calculates the 2D Discrete Sine Transform for a matrix.
		 * 		Utilizes the function sineTransform (slow version)
		 * 		The inverse can be calculated by calling this function as is with no additional options.
		 * 
		 * @param arr 
		 * @return Matrix 
		 */
		static MatrixF sineTransform2D(const MatrixF& arr);
		static MatrixD sineTransform2D(const MatrixD& arr);

		/**
		 * @brief NOT IMPLEMENTED
		 * 
		 * @param arr 
		 * @param inverse 
		 * @return Matrix 
		 */
		static MatrixF fastSineTransform2D(const MatrixF& arr, bool inverse=false);
		static MatrixD fastSineTransform2D(const MatrixD& arr, bool inverse=false);
		
		/**
		 * @brief Computes the convolution of a matrix and a kernel (which is a matrix).
		 * 		The convolution is the sum of the kernel applied centered on a point for each point in the base image.
		 * 			Meaning (f*g)(x) = SUM( f[i]*g[x-i] ) from -INF to INF
		 * 			which for the discrete case, the sum is limited by the size of the kernel.
		 * 
		 * 		Computed in O(N^2) operations but it is possible to do it in O(NLogN) with a FFT.
		 * 		Returns the baseImage convolved with the kernel.
		 * @param baseImage 
		 * @param kernel 
		 * @param mode
		 * 		Describes the mode of the convolution. These modes control the size of the output matrix
		 * 		Valid Modes:
		 * 			FULL - The proper convolution by definition. Returns a matrix of size N+M-1
		 * 			SAME - Returns a Matrix that is the same size as the BaseImage. Default
		 * 			VALID - Returns a Matrix that is only the sections that require no padding. Returns a matrix of size N-(M/2)
		 * @param paddingMode
		 * 		Valid Modes:
		 * 			ZERO_PADDING - Boundary is padded with 0s
		 * 			CLAMP - Boundary clamps to the closest valid value of the baseImage
		 * 			WRAP - Boundary wraps around to the otherside of the baseImage
		 * @return Matrix 
		 */
		template<typename T>
		static Matrix<T> convolution(const Matrix<T>& baseImage, const Matrix<T>& kernel, int type, int paddingMode);

		/**
		 * @brief Computes the cross correlation of a matrix and a kernel (which is a matrix).
		 * 		The cross correlation is the sum of the kernel applied centered on a point for each point in the base image.
		 * 			Meaning (f*g)(x) = SUM( f[i]*g[x+i] ) from -INF to INF
		 * 			which for the discrete case, the sum is limited by the size of the kernel.
		 * 
		 * 		Similar to convolution, but effectively computes the similarity of the baseImage to the kernel at a given point.
		 * 			Note that it is not communitive like a convolution
		 * 
		 * 		Computed in O(N^2) operations but it is possible to do it in O(NLogN) with a FFT.
		 * 		Returns the baseImage convolved with the kernel.
		 * @param baseImage 
		 * @param kernel 
		 * @param normalized
		 * @return Matrix 
		 */
		template<typename T>
		static Matrix<T> crossCorrelation(const Matrix<T>& baseImage, const Matrix<T>& kernel, int type, int paddingMode);

		/**
		 * @brief Computes the convolution utilizing the Fast Fourier Transform to achieve much higher speeds.
		 * 		Requires the matrices be composed of Complex Numbers.
		 * 		Note that the matrices do not need to be the same size.
		 * 
		 * @param baseImage 
		 * @param kernel 
		 * @param type 
		 * @param paddingMode 
		 * @return Matrix<ComplexNumber> 
		 */
		static Matrix<ComplexNumber> convolutionFFT(const Matrix<ComplexNumber>& baseImage, const Matrix<ComplexNumber>& kernel, int type, int paddingMode);

		/**
		 * @brief Computes the convolution utilizing the Fast Fourier Transform to achieve much higher speeds.
		 * 		Requires the matrices be composed of Complex Numbers.
		 * 		Note that the matrices do not need to be the same size.
		 * 
		 * @param baseImage 
		 * @param kernel 
		 * @param type 
		 * @param paddingMode 
		 * @return Matrix<ComplexNumber> 
		 */
		static Matrix<ComplexNumber> crossCorrelationFFT(const Matrix<ComplexNumber>& baseImage, const Matrix<ComplexNumber>& kernel, int type, int paddingMode);

		//Clustering algorigthms

		/**
		 * @brief Clusters data into groups using a custom made function.
		 * 
		 * @param arr 
		 * 		The data to separate into clusters
		 * @param clusters 
		 * 		The amount of clusters desired.
		 * @param meansOnly 
		 * 		Whether to return the average of each cluster or return all values in the clusters.
		 * @return std::vector<std::vector<Vec2f>> 
		 */
		static std::vector<std::vector<Vec2f>> meanCut(std::vector<Vec2f> arr, int clusters, bool meansOnly = false);

		/**
		 * @brief Clusters data into groups using a MedianCut.
		 * 
		 * @param arr 
		 * 		The data to separate into clusters
		 * @param clusters 
		 * 		The amount of clusters desired.
		 * @param meansOnly 
		 * 		Whether to return the average of each cluster or return all values in the clusters.
		 * @return std::vector<std::vector<Vec2f>> 
		 */
		static std::vector<std::vector<Vec2f>> medianCut(std::vector<Vec2f> arr, int clusters, bool meansOnly = false);

		/**
		 * @brief Clusters data into groups using KMeans.
		 * 		Initial means are chosen at random.
		 * 
		 * @param arr 
		 * 		The data to separate into clusters
		 * @param clusters 
		 * 		The amount of clusters desired.
		 * @param maxIterations
		 * 		The maximum amount of times to iterate over the data to find the best clusters.
		 * @param meansOnly 
		 * 		Whether to return the average of each cluster or return all values in the clusters.
		 * @return std::vector<std::vector<Vec2f>> 
		 */
		static std::vector<std::vector<Vec2f>> kMeans(std::vector<Vec2f> arr, int clusters, int maxIterations, bool meansOnly = false);
		
		/**
		 * @brief Clusters data into groups using a custom made function.
		 * 
		 * @param arr 
		 * 		The data to separate into clusters
		 * @param clusters 
		 * 		The amount of clusters desired.
		 * @param meansOnly 
		 * 		Whether to return the average of each cluster or return all values in the clusters.
		 * @return std::vector<std::vector<Vec3f>> 
		 */
		static std::vector<std::vector<Vec3f>> meanCut(std::vector<Vec3f> arr, int clusters, bool meansOnly = false);

		/**
		 * @brief Clusters data into groups using a MedianCut.
		 * 
		 * @param arr 
		 * 		The data to separate into clusters
		 * @param clusters 
		 * 		The amount of clusters desired.
		 * @param meansOnly 
		 * 		Whether to return the average of each cluster or return all values in the clusters.
		 * @return std::vector<std::vector<Vec3f>> 
		 */
		static std::vector<std::vector<Vec3f>> medianCut(std::vector<Vec3f> arr, int clusters, bool meansOnly = false);

		/**
		 * @brief Clusters data into groups using KMeans.
		 * 		Initial means are chosen at random.
		 * 
		 * @param arr 
		 * 		The data to separate into clusters
		 * @param clusters 
		 * 		The amount of clusters desired.
		 * @param maxIterations
		 * 		The maximum amount of times to iterate over the data to find the best clusters.
		 * @param meansOnly 
		 * 		Whether to return the average of each cluster or return all values in the clusters.
		 * @return std::vector<std::vector<Vec3f>> 
		 */
		static std::vector<std::vector<Vec3f>> kMeans(std::vector<Vec3f> arr, int clusters, int maxIterations, bool meansOnly = false);
		
		/**
		 * @brief Clusters data into groups using a custom made function.
		 * 
		 * @param arr 
		 * 		The data to separate into clusters
		 * @param clusters 
		 * 		The amount of clusters desired.
		 * @param meansOnly 
		 * 		Whether to return the average of each cluster or return all values in the clusters.
		 * @return std::vector<std::vector<Vec4f>> 
		 */
		static std::vector<std::vector<Vec4f>> meanCut(std::vector<Vec4f> arr, int clusters, bool meansOnly = false);

		/**
		 * @brief Clusters data into groups using a MedianCut.
		 * 
		 * @param arr 
		 * 		The data to separate into clusters
		 * @param clusters 
		 * 		The amount of clusters desired.
		 * @param meansOnly 
		 * 		Whether to return the average of each cluster or return all values in the clusters.
		 * @return std::vector<std::vector<Vec4f>> 
		 */
		static std::vector<std::vector<Vec4f>> medianCut(std::vector<Vec4f> arr, int clusters, bool meansOnly = false);

		/**
		 * @brief Clusters data into groups using KMeans.
		 * 		Initial means are chosen at random.
		 * 
		 * @param arr 
		 * 		The data to separate into clusters
		 * @param clusters 
		 * 		The amount of clusters desired.
		 * @param maxIterations
		 * 		The maximum amount of times to iterate over the data to find the best clusters.
		 * @param meansOnly 
		 * 		Whether to return the average of each cluster or return all values in the clusters.
		 * @return std::vector<std::vector<Vec4f>> 
		 */
		static std::vector<std::vector<Vec4f>> kMeans(std::vector<Vec4f> arr, int clusters, int maxIterations, bool meansOnly = false);
		
		/**
		 * @brief Calculates the mean from a list of data
		 * 		Must be able to add the template type T and must be able
		 * 		to divide the template type by a number.
		 * 		Must be able to initialize T without arguments.
		 * 			T sum; //should be valid.
		 * @tparam T 
		 * @param data 
		 * @return T 
		 */
		template<typename T>
		static T mean(T* data, size_t size)
		{
			T sum;
			for(size_t i=0; i<size; i++)
			{
				sum += data[i];
			}
			return sum / size;
		}

		/**
		 * @brief Calculates the variance from a list of data
		 * 		Must be able to add the template type T and must be able
		 * 		to divide the template type by a number.
		 * 		Must be able to initialize T without arguments.
		 * 			T sum; //should be valid.
		 * @tparam T 
		 * @param data 
		 * @param size 
		 * @param mean 
		 * @return T 
		 */
		template<typename T>
		static T variance(T* data, size_t size, T mean)
		{
			T sum;
			for(size_t i=0; i<size; i++)
			{
				sum += sqr(data[i] - mean);
			}
			return sum / size;
		}

		/**
		 * @brief Calculates the std deviation from a list of data
		 * 		Must be able to add the template type T and must be able
		 * 		to divide the template type by a number.
		 * 		Must be able to initialize T without arguments.
		 * 			T sum; //should be valid.
		 * @tparam T 
		 * @param data 
		 * @param size 
		 * @param mean 
		 * @return T 
		 */
		template<typename T>
		static T stddev(T* data, size_t size, T mean)
		{
			return MathExt::sqrt(variance(data, size, mean));
		}
		
	private:
		
		template<typename T>
		static Matrix<T> convolveValid(const Matrix<T>& input_matrix, const Matrix<T>& kernel);
		template<typename T>
		static Matrix<T> convolveSame(const Matrix<T>& input_matrix, const Matrix<T>& kernel, int paddingMode, bool crossCorrelation);
		template<typename T>
		static Matrix<T> convolveFull(const Matrix<T>& input_matrix, const Matrix<T>& kernel, int paddingMode);
	};

	
	template<typename T>
	inline Matrix<T> MathExt::convolveValid(const Matrix<T>& input_matrix, const Matrix<T>& kernel)
	{
		int kR = (kernel.getRows()/2);
		int kC = (kernel.getColumns()/2);
		if(kernel.getColumns() != 1 && kernel.getColumns()%2)
			kC++;
		if(kernel.getRows() != 1 && kernel.getRows()%2)
			kR++;
		Matrix<T> output = Matrix<T>(input_matrix.getRows()-kR, input_matrix.getColumns()-kC);

		LARGE_ENOUGH_CLAUSE(output.getRows())
		#pragma omp parallel for
		for(size_t i=0; i<output.getRows(); i++)
		{
			int startY = i;
			LARGE_ENOUGH_CLAUSE(output.getColumns())
			#pragma omp parallel for
			for(size_t j=0; j<output.getColumns(); j++)
			{
				int startX = j;
				//ignore all values outside the range of the input matrix. No negatives. No values greater than rows or columns
				float sum = 0;
				for(int kernelY=0; kernelY<kernel.getRows(); kernelY++)
				{
					int currY = startY+kernelY;
					for(int kernelX=0; kernelX<kernel.getColumns(); kernelX++)
					{
						int currX = startX+kernelX;
						sum += kernel[kernelY][kernelX] * input_matrix[currY][currX];
					}
				}
				output[i][j] = sum;
			}
		}
		return output;
	}
	template<typename T>
	inline Matrix<T> MathExt::convolveSame(const Matrix<T>& input_matrix, const Matrix<T>& kernel, int paddingMode, bool crossCorrelation)
	{
		int kR = (kernel.getRows()/2);
		int kC = (kernel.getColumns()/2);
		if(crossCorrelation)
		{
			if(kernel.getRows()%2 == 0)
				kR--;
			if(kernel.getColumns()%2 == 0)
				kC--;
		}
		Matrix<T> output = Matrix<T>(input_matrix.getRows(), input_matrix.getColumns());
		
		LARGE_ENOUGH_CLAUSE(output.getRows())
		#pragma omp parallel for
		for(size_t i=0; i<output.getRows(); i++)
		{
			int startY = i-kR;
			LARGE_ENOUGH_CLAUSE(output.getColumns())
			#pragma omp parallel for
			for(size_t j=0; j<output.getColumns(); j++)
			{
				int startX = j-kC;
				//ignore all values outside the range of the input matrix. No negatives. No values greater than rows or columns
				float sum = 0;
				for(int kernelY=0; kernelY<kernel.getRows(); kernelY++)
				{
					int currY = startY+kernelY;
					for(int kernelX=0; kernelX<kernel.getColumns(); kernelX++)
					{
						int currX = startX+kernelX;
						float inputMatValue = 0;
						if(paddingMode == 1)
						{
							//clamp
							int r = MathExt::clamp<int>(currY, 0, input_matrix.getRows());
							int c = MathExt::clamp<int>(currX, 0, input_matrix.getColumns());
							inputMatValue = input_matrix[r][c];
						}
						else if(paddingMode == 2)
						{
							//wrap
							int r,c;
							// int r = wrapNumber(currY, input_matrix.getRows());
							// int c = wrapNumber(currX, input_matrix.getColumns());
							inputMatValue = input_matrix[r][c];
						}
						else
						{
							//zero padding
							if(currX >= 0 && currX < input_matrix.getColumns() && currY >= 0 && currY < input_matrix.getRows())
								inputMatValue = input_matrix[currY][currX];
						}
						sum += kernel[kernelY][kernelX] * inputMatValue;
					}
				}
				output[i][j] = sum;
			}
		}
		return output;
	}
	template<typename T>
	inline Matrix<T> MathExt::convolveFull(const Matrix<T>& input_matrix, const Matrix<T>& kernel, int paddingMode)
	{
		int kR = (kernel.getRows()/2);
		int kC = (kernel.getColumns()/2);
		if(kernel.getColumns() != 1 && kernel.getColumns()%2)
			kC++;
		if(kernel.getRows() != 1 && kernel.getRows()%2)
			kR++;
		
		Matrix<T> output = Matrix<T>(input_matrix.getRows()+kR, input_matrix.getColumns()+kC);
		
		LARGE_ENOUGH_CLAUSE(output.getRows())
		#pragma omp parallel for
		for(size_t i=0; i<output.getRows(); i++)
		{
			int startY = i-kR;
			LARGE_ENOUGH_CLAUSE(output.getColumns())
			#pragma omp parallel for
			for(size_t j=0; j<output.getColumns(); j++)
			{
				int startX = j-kC;
				//ignore all values outside the range of the input matrix. No negatives. No values greater than rows or columns
				float sum = 0;
				for(int kernelY=0; kernelY<kernel.getRows(); kernelY++)
				{
					int currY = startY+kernelY;
					for(int kernelX=0; kernelX<kernel.getColumns(); kernelX++)
					{
						int currX = startX+kernelX;
						float inputMatValue = 0;
						if(paddingMode == 1)
						{
							//clamp
							int r = MathExt::clamp<int>(currY, 0, input_matrix.getRows());
							int c = MathExt::clamp<int>(currX, 0, input_matrix.getColumns());
							inputMatValue = input_matrix[r][c];
						}
						else if(paddingMode == 2)
						{
							//wrap
							int r,c;
							// int r = wrapNumber(currY, input_matrix.getRows());
							// int c = wrapNumber(currX, input_matrix.getColumns());
							inputMatValue = input_matrix[r][c];
						}
						else
						{
							//zero padding
							if(currX >= 0 && currX < input_matrix.getColumns() && currY >= 0 && currY < input_matrix.getRows())
								inputMatValue = input_matrix[currY][currX];
						}
						sum += kernel[kernelY][kernelX] * inputMatValue;
					}
				}
				output[i][j] = sum;
			}
		}
		return output;
	}

	template<typename T>
	inline Matrix<T> MathExt::convolution(const Matrix<T>& baseImage, const Matrix<T>& kernel, int type, int paddingMode)
	{
		if(!baseImage.getValid() || !kernel.getValid())
			throw InvalidMatrix();
		
		Matrix<T> flippedKernel = kernel;
		size_t i=0;
		size_t j=0;
		do
		{
			do
			{
				T temp = flippedKernel[flippedKernel.getRows()-i-1][flippedKernel.getColumns()-j-1];
				flippedKernel[flippedKernel.getRows()-i-1][flippedKernel.getColumns()-j-1] = flippedKernel[i][j];
				flippedKernel[i][j] = temp;
				j++;
			} while (j < flippedKernel.getColumns()/2);
			i++;
		} while (i < flippedKernel.getRows()/2);
	
		if(type == MathExt::CONVOLVE_FULL)
			return MathExt::convolveFull(baseImage, flippedKernel, paddingMode);
		else if(type == MathExt::CONVOLVE_VALID)
			return MathExt::convolveValid(baseImage, flippedKernel);
		else
			return MathExt::convolveSame(baseImage, flippedKernel, paddingMode, false);
	}
	
	template<typename T>
	inline Matrix<T> MathExt::crossCorrelation(const Matrix<T>& baseImage, const Matrix<T>& kernel, int type, int paddingMode)
	{
		if(!baseImage.getValid() || !kernel.getValid())
			throw InvalidMatrix();
		
		if(type == MathExt::CONVOLVE_FULL)
			return MathExt::convolveFull(baseImage, kernel, paddingMode);
		else if(type == MathExt::CONVOLVE_VALID)
			return MathExt::convolveValid(baseImage, kernel);
		else
			return MathExt::convolveSame(baseImage, kernel, paddingMode, true);
	}
	
} //NAMESPACE smpl END