#pragma once
#include <limits>
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
#include "GeneralVector.h"
#include "ComplexNumber.h"
#include "PolarCoordinate.h"

#ifdef max
	#undef max
#endif

#ifdef min
	#undef min
#endif

#ifndef PI
	#define PI 3.14159265359
#endif

#ifndef E
	#define E 2.718281828459
#endif

//Temporary location
#ifndef LOGIC_XOR
	#define LOGIC_XOR(a,b) ((!(a)) != (!(b)))
#endif

namespace glib
{

	class MathExt
	{
	public:

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
			return ((a>b)? a:b);
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

			T max = a[0];
			for(int i=0; i<count; i++)
			{
				if(a[i] > max)
					max = a[i];
			}
			return max;
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
			T max;
			bool first = true;
			for(T elem : list)
			{	
				if(first)
				{
					max = elem;
					first = false;
				}
				
				if(elem > max)
					max = elem;
			}
			return max;
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
			T min = a[0];
			for(int i=0; i<count; i++)
			{
				if(a[i] < min)
					min = a[i];
			}
			return min;
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
			T min;
			bool first = true;
			for(T elem : list)
			{
				if(first)
				{
					min = elem;
					first = false;
				}

				if(elem < min)
					min = elem;
			}
			return min;
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
		 * @brief Returns the cube root of the value.
		 * 
		 * @param a 
		 * @return float 
		 */
		static float cubeRoot(float a);

		/**
		 * @brief Returns the cube root of the value.
		 * 
		 * @param a 
		 * @return double 
		 */
		static double cubeRoot(double a);
		
		/**
		 * @brief Returns the cube root of the template value.
		 * 
		 * @param a 
		 * @return double 
		 */
		template<typename T>
		static double cubeRoot(T a)
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
		 * @brief Returns a Vec2f that has the specified length and direction.
		 * 
		 * @param length 
		 * @param direction 
		 * @return Vec2f 
		 */
		static Vec2f lengthDir(double length, double direction);

		/**
		 * @brief Returns the dot product of the 2 vectors.
		 * 
		 * @param v1 
		 * @param v2 
		 * @return double 
		 */
		static double dot(Vec2f v1, Vec2f v2);

		/**
		 * @brief Returns the dot product of the 2 vectors.
		 * 
		 * @param v1 
		 * @param v2 
		 * @return double 
		 */
		static double dot(Vec3f v1, Vec3f v2);

		/**
		 * @brief Returns the dot product of the 2 vectors.
		 * 
		 * @param v1 
		 * @param v2 
		 * @return double 
		 */
		static double dot(Vec4f v1, Vec4f v2);

		/**
		 * @brief Returns the dot product of the 2 vectors.
		 * 		Both GeneralVector Objects must have the same size.
		 * 
		 * @param v1 
		 * @param v2 
		 * @return double 
		 */
		static double dot(GeneralVector v1, GeneralVector v2);

		/**
		 * @brief Returns the length of the vector.
		 * 
		 * @param v 
		 * @return double 
		 */
		static double vecLength(Vec2f v);

		/**
		 * @brief Returns the length of the vector.
		 * 
		 * @param v 
		 * @return double 
		 */
		static double vecLength(Vec3f v);

		/**
		 * @brief Returns the length of the vector.
		 * 
		 * @param v 
		 * @return double 
		 */
		static double vecLength(Vec4f v);

		/**
		 * @brief Returns the length of the vector.
		 * 
		 * @param v 
		 * @return double 
		 */
		static double vecLength(GeneralVector v);

		/**
		 * @brief Projects vector a onto vector b.
		 * 
		 * @param a 
		 * @param b 
		 * @return double 
		 */
		static double scalarVectorProjection(Vec2f a, Vec2f b);

		/**
		 * @brief Projects vector a onto vector b.
		 * 
		 * @param a 
		 * @param b 
		 * @return double 
		 */
		static double scalarVectorProjection(Vec3f a, Vec3f b);

		/**
		 * @brief Projects vector a onto vector b.
		 * 
		 * @param a 
		 * @param b 
		 * @return double 
		 */
		static double scalarVectorProjection(Vec4f a, Vec4f b);

		/**
		 * @brief Projects vector a onto vector b.
		 * 		Both GeneralVector Objects must have the same size.
		 * 
		 * @param a 
		 * @param b 
		 * @return double 
		 */
		static double scalarVectorProjection(GeneralVector a, GeneralVector b);
		
		/**
		 * @brief Normalizes the vector.
		 * 
		 * @param v1 
		 * @return Vec2f 
		 */
		static Vec2f normalize(Vec2f v1);

		/**
		 * @brief Normalizes the vector.
		 * 
		 * @param v1 
		 * @return Vec3f 
		 */
		static Vec3f normalize(Vec3f v1);

		/**
		 * @brief Normalizes the vector.
		 * 
		 * @param v1 
		 * @return Vec4f 
		 */
		static Vec4f normalize(Vec4f v1);

		/**
		 * @brief Normalizes the vector.
		 * 
		 * @param v1 
		 * @return GeneralVector 
		 */
		static GeneralVector normalize(GeneralVector v1);

		/**
		 * @brief Normalizes the Quaternion.
		 * 
		 * @param q1 
		 * @return Quaternion
		 */
		static Quaternion normalize(Quaternion q1);

		/**
		 * @brief Returns the cross product of the 2 vectors.
		 * 
		 * @param v1 
		 * @param v2 
		 * @return Vec3f 
		 */
		static Vec3f crossProduct(Vec3f v1, Vec3f v2);

		/**
		 * @brief Reflects the Vector a across Vector b.
		 * 
		 * @param a 
		 * @param b 
		 * @return Vec2f 
		 */
		static Vec2f reflect(Vec2f a, Vec2f b);

		/**
		 * @brief Reflects the Vector a across Vector b.
		 * 
		 * @param a 
		 * @param b 
		 * @return Vec3f 
		 */
		static Vec3f reflect(Vec3f a, Vec3f b);

		/**
		 * @brief Reflects the Vector a across Vector b.
		 * 
		 * @param a 
		 * @param b 
		 * @return Vec4f 
		 */
		static Vec4f reflect(Vec4f a, Vec4f b);

		/**
		 * @brief Reflects the Vector a across Vector b.
		 * 		Both GeneralVector Objects must have the same size.
		 * 
		 * @param a 
		 * @param b 
		 * @return Vec4f 
		 */
		static GeneralVector reflect(GeneralVector a, GeneralVector b);

		/**
		 * @brief Returns a new vector that is the inverse.
		 * 		Returns this : Vec2f( -f.y, f.x )
		 * 
		 * @param f 
		 * @return Vec2f 
		 */
		static Vec2f inverseVec(Vec2f f);

		/**
		 * @brief Returns the dot product of both vectors after normalizing them.
		 * 
		 * @param v1 
		 * @param v2 
		 * @return double 
		 */
		static double dotNorm(Vec2f v1, Vec2f v2);

		/**
		 * @brief Returns the dot product of both vectors after normalizing them.
		 * 
		 * @param v1 
		 * @param v2 
		 * @return double 
		 */
		static double dotNorm(Vec3f v1, Vec3f v2);

		/**
		 * @brief Returns the dot product of both vectors after normalizing them.
		 * 
		 * @param v1 
		 * @param v2 
		 * @return double 
		 */
		static double dotNorm(Vec4f v1, Vec4f v2);

		/**
		 * @brief Returns the dot product of both vectors after normalizing them.
		 * 		Both GeneralVector Objects must have the same size.
		 * 
		 * @param v1 
		 * @param v2 
		 * @return double 
		 */
		static double dotNorm(GeneralVector v1, GeneralVector v2);

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
		 * 		Both GeneralVector Objects must have the same size.
		 * 
		 * @param p1 
		 * @param p2 
		 * @return double 
		 */
		static double distanceTo(GeneralVector p1, GeneralVector p2);

		/**
		 * @brief Returns the distance from p1 to p2.
		 * 
		 * @param p1 
		 * @param p2 
		 * @return double 
		 */
		static double distanceTo(PolarCoordinate p1, PolarCoordinate p2);
		
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
		static Mat4f QuaternionToMatrix(Quaternion q);
		
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
		 * 
		 * @param A 
		 * @param B 
		 * @param C 
		 * @return std::vector<double> 
		 * 		Returns all solutions found.
		 */
		static std::vector<double> solveQuadraticReal(double A, double B, double C);

		/**
		 * @brief Solves a Cubic equation but only returns real solutions
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
		 * @brief Computes the Fast Discrete Fourier Transform using the Cooley Tukey algorithm
		 * 		Runs in O(NLogN) time.
		 * 		Fails if the input array is not a power of 2.
		 * 
		 * @param arr 
		 * 		An array of Complex Numbers to perform the Fourier Transform on.
		 * 		If not doing the inverse, the imaginary values can be ignored and set to 0.
		 * @param size 
		 * 		The size of the array.
		 * 		Must be a power of 2.
		 * @param inverse 
		 * 		Whether to solve for the inverse.
		 * 		Default is false.
		 * @return std::vector<ComplexNumber> 
		 */
		static std::vector<ComplexNumber> fastFourierTransform(ComplexNumber* arr, int size, bool inverse=false);

		/**
		 * @brief Computes the Fast Discrete Fourier Transform using the Cooley Tukey algorithm
		 * 		Runs in O(NLogN) time.
		 * 		Fails if the input array is not a power of 2.
		 * 		This is a test version that runs faster. Soon to be removed to replace the original version.
		 * 
		 * @param arr 
		 * 		An array of Complex Numbers to perform the Fourier Transform on.
		 * 		If not doing the inverse, the imaginary values can be ignored and set to 0.
		 * @param size 
		 * 		The size of the array.
		 * 		Must be a power of 2.
		 * @param inverse 
		 * 		Whether to solve for the inverse.
		 * 		Default is false.
		 * @return std::vector<ComplexNumber> 
		 */
		static std::vector<ComplexNumber> fastFourierTransformTest(ComplexNumber* arr, int size, bool inverse=false);

		/**
		 * @brief Computes the Discrete Cosine Transform on the array.
		 * 		Runs in O(N) time. Solves for a single point.
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

		/**
		 * @brief NOT IMPLEMENTED YET
		 * 
		 * @param arr 
		 * @param size 
		 * @param inverse 
		 * @return std::vector<double> 
		 */
		static std::vector<double> fastCosineTransform(double* arr, int size, bool inverse=false);

		/**
		 * @brief Computes the 2D Discrete Cosine Transform for a specific location.
		 * 
		 * @param arr 
		 * @param u 
		 * @param v 
		 * @param inverse 
		 * 		Whether to solve for the inverse.
		 * 		Default is false.
		 * @return double 
		 */
		static double discreteCosineTransform2D(Matrix& arr, int u, int v, bool inverse=false);

		/**
		 * @brief Computes the 2D Discrete Cosine Transform for all possible locations.
		 * 
		 * @param arr 
		 * @param inverse 
		 * 		Whether to solve for the inverse.
		 * 		Default is false.
		 * @return double 
		 */
		static Matrix cosineTransform2D(Matrix& arr, bool inverse=false);

		/**
		 * @brief NOT IMPLEMENTED YET
		 * 
		 * @param arr 
		 * @param size 
		 * @param inverse 
		 * @return std::vector<double> 
		 */
		static Matrix fastCosineTransform2D(Matrix& arr, bool inverse=false);
		
		
		//Special Case for JPEG
		/**
		 * @brief Computes the Fast Discrete Cosine Transform for a list of 8 values.
		 * 		Special case.
		 * 
		 * @param arr 
		 * 		The input array
		 * @param output 
		 * 		The output array
		 * @param inverse 
		 * 		Whether to solve for the inverse.
		 * 		Default is false.
		 */
		static void FCT8(double* arr, double* output, bool inverse=false);

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
		static void FCT8x8(Matrix& arr, Matrix* output, bool inverse=false);


		/**
		 * @brief NOT IMPLEMENTED
		 * 
		 * @param size 
		 * @param u 
		 * @param inverse 
		 * @return double 
		 */
		static double discreteSineTransform(double*, int size, int u, bool inverse=false);

		/**
		 * @brief NOT IMPLEMENTED
		 * 
		 * @param arr 
		 * @param size 
		 * @param inverse 
		 * @return std::vector<double> 
		 */
		static std::vector<double> sineTransform(double* arr, int size, bool inverse=false);

		/**
		 * @brief NOT IMPLEMENTED
		 * 
		 * @param arr 
		 * @param size 
		 * @param inverse 
		 * @return std::vector<double> 
		 */
		static std::vector<double> fastSineTransform(double* arr, int size, bool inverse=false);

		/**
		 * @brief NOT IMPLEMENTED
		 * 
		 * @param arr 
		 * @param u 
		 * @param v 
		 * @param inverse 
		 * @return double 
		 */
		static double discreteSineTransform2D(Matrix& arr, int u, int v, bool inverse=false);

		/**
		 * @brief NOT IMPLEMENTED
		 * 
		 * @param arr 
		 * @param inverse 
		 * @return Matrix 
		 */
		static Matrix sineTransform2D(Matrix& arr, bool inverse=false);

		/**
		 * @brief NOT IMPLEMENTED
		 * 
		 * @param arr 
		 * @param inverse 
		 * @return Matrix 
		 */
		static Matrix fastSineTransform2D(Matrix& arr, bool inverse=false);

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
		 * @brief Clusters data into groups using a custom made function.
		 * 
		 * @param arr 
		 * 		The data to separate into clusters
		 * @param clusters 
		 * 		The amount of clusters desired.
		 * @param meansOnly 
		 * 		Whether to return the average of each cluster or return all values in the clusters.
		 * @return std::vector<std::vector<GeneralVector>> 
		 */
		static std::vector<std::vector<GeneralVector>> meanCut(std::vector<GeneralVector> arr, int clusters, bool meansOnly = false);

		/**
		 * @brief Clusters data into groups using a MedianCut.
		 * 
		 * @param arr 
		 * 		The data to separate into clusters
		 * @param clusters 
		 * 		The amount of clusters desired.
		 * @param meansOnly 
		 * 		Whether to return the average of each cluster or return all values in the clusters.
		 * @return std::vector<std::vector<GeneralVector>> 
		 */
		static std::vector<std::vector<GeneralVector>> medianCut(std::vector<GeneralVector> arr, int clusters, bool meansOnly = false);

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
		 * @return std::vector<std::vector<GeneralVector>> 
		 */
		static std::vector<std::vector<GeneralVector>> kMeans(std::vector<GeneralVector> arr, int clusters, int maxIterations, bool meansOnly = false);
		
	private:
		static std::vector<ComplexNumber> doFFT(ComplexNumber* arr, int size, bool inverse=false);
		static void doFFTTest(ComplexNumber* output, int size, int incVal, bool inverse=false, ComplexNumber* tempData=nullptr);
		
	};

} //NAMESPACE glib END