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
		MathExt();
		~MathExt();

		template<typename T>
		static T max(T a, T b)
		{
			return ((a>b)? a:b);
		}
		
		template<typename T>
		static T max(T* a, int count)
		{
			T max = a[0];
			for(int i=0; i<count; i++)
			{
				if(a[i] > max)
					max = a[i];
			}
			return max;
		}

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

		template<typename T>
		static T min(T a, T b)
		{
			return ((a<b)? a:b);
		}

		template<typename T>
		static T min(T* a, int count)
		{
			T min = a[0];
			for(int i=0; i<count; i++)
			{
				if(a[i] < min)
					min = a[i];
			}
			return min;
		}

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
		
		static float floor(float a);
		static float ceil(float a);
		static float round(float a);

		static double floor(double a);
		static double ceil(double a);
		static double round(double a);

		static double frac(double a);
		static float frac(float a);

		static float roundToDecimal(float a, int decimalPlaces=6);
		static double roundToDecimal(double a, int decimalPlaces=6);

		template<typename T>
		static T sqr(T a)
		{
			return a*a;
		}

		template<typename T>
		static T cube(T a)
		{
			return a*a*a;
		}

		// static int sqr(int a);
		// static float sqr(float a);
		// static double sqr(double a);
		// static long sqr(long a);
		
		// static int cube(int a);
		// static float cube(float a);
		// static double cube(double a);
		// static long cube(long a);

		static float sqrt(float a);
		static double sqrt(double a);

		template<typename T>
		static double sqrt(T a)
		{
			return std::sqrt(a);
		}
		
		static float cubeRoot(float a);
		static double cubeRoot(double a);
		
		template<typename T>
		static double cubeRoot(T a)
		{
			return std::cbrt(a);
		}

		static float pow(float value, float power);
		static double pow(double value, double power);
		
		static float log(float value, float base = 10.0f);
		static double log(double value, double base = 10.0);

		static float lerp(float a, float b, float blend);
		static double lerp(double a, double b, double blend);

		static int abs(int a);
		static double abs(double a);
		static float abs(float a);
		static long abs(long a);

		static unsigned int random();
		static int randomRange(int min, int max);
		
		template<typename T>
		static T clamp(T value, T min, T max)
		{
			if(value <= min)
				return min;
			if(value >= max)
				return max;
			return value;
		}

		static float toDeg(float rad);
		static double toDeg(double rad);

		static float toRad(float deg);
		static double toRad(double deg);

		static float angleToStandardRange(float angle, bool isDegree);
		static double angleToStandardRange(double angle, bool isDegree);
		
		static float cos(float value);
		static double cos(double value);

		static float dcos(float value);
		static double dcos(double value);

		static float sin(float value);
		static double sin(double value);
		
		static float dsin(float value);
		static double dsin(double value);

		static float tan(float value);
		static double tan(double value);

		static float dtan(float value);
		static double dtan(double value);

		static float arccos(float value);
		static double arccos(double value);

		static float darccos(float value);
		static double darccos(double value);

		static float arcsin(float value);
		static double arcsin(double value);

		static float darcsin(float value);
		static double darcsin(double value);

		static float arctan(float value);
		static double arctan(double value);
		
		static float arctan2(float y, float x);
		static double arctan2(double y, double x);

		static float darctan(float value);
		static double darctan(double value);

		static float darctan2(float y, float x);
		static double darctan2(double y, double x);

		static double dirToPoint(double x, double y, double x2, double y2);
		static double dirToPointDeg(double x, double y, double x2, double y2);

		static bool angleBetween(double testAngle, double startAngle, double endAngle, bool clockwise);
		static bool angleBetweenDeg(double testAngle, double startAngle, double endAngle, bool clockwise);
		
		static double closestAngle(double testAngle, double angle1, double angle2);
		static double closestAngleDeg(double testAngle, double angle1, double angle2);
		
		static double angleClamp(double testAngle, double startAngle, double endAngle, bool clockwise);
		static double angleClampDeg(double testAngle, double startAngle, double endAngle, bool clockwise);
		
		static Vec2f lengthDir(double length, double direction);

		static double dot(Vec2f v1, Vec2f v2);
		static double dot(Vec3f v1, Vec3f v2);
		static double dot(Vec4f v1, Vec4f v2);
		static double dot(GeneralVector v1, GeneralVector v2);

		static double vecLength(Vec2f v);
		static double vecLength(Vec3f v);
		static double vecLength(Vec4f v);
		static double vecLength(GeneralVector v);

		static double scalarVectorProjection(Vec2f a, Vec2f b);
		static double scalarVectorProjection(Vec3f a, Vec3f b);
		static double scalarVectorProjection(Vec4f a, Vec4f b);
		static double scalarVectorProjection(GeneralVector a, GeneralVector b);
		
		static Vec2f normalize(Vec2f v1);
		static Vec3f normalize(Vec3f v1);
		static Vec4f normalize(Vec4f v1);
		static GeneralVector normalize(GeneralVector v1);
		static Quaternion normalize(Quaternion q1);

		static Vec3f crossProduct(Vec3f v1, Vec3f v2);

		static Vec2f reflect(Vec2f v1, Vec2f v2);
		static Vec3f reflect(Vec3f v1, Vec3f v2);
		static Vec4f reflect(Vec4f v1, Vec4f v2);
		static GeneralVector reflect(GeneralVector v1, GeneralVector v2);

		static Vec2f inverseVec(Vec2f f);

		static double dotNorm(Vec2f v1, Vec2f v2);
		static double dotNorm(Vec3f v1, Vec3f v2);
		static double dotNorm(Vec4f v1, Vec4f v2);
		static double dotNorm(GeneralVector v1, GeneralVector v2);

		static double distanceTo(double x1, double y1, double x2, double y2);

		static double distanceTo(Vec2f p1, Vec2f p2);
		static double distanceTo(Vec3f p1, Vec3f p2);
		static double distanceTo(Vec4f p1, Vec4f p2);
		static double distanceTo(GeneralVector p1, GeneralVector p2);

		static double distanceTo(PolarCoordinate p1, PolarCoordinate p2);
		
		//2D transformations
		static Mat3f rotation2D(double rotation);
		static Mat3f rotation2D(double rotation, double x, double y);
		static Mat3f rotation2D(double rotation, Vec2f pos);
		
		static Mat3f translation2D(double x, double y);
		static Mat3f translation2D(Vec2f trans);

		static Mat3f scale2D(double x, double y);
		static Mat3f scale2D(Vec2f trans);

		static Mat3f skew2D(double x, double y);
		static Mat3f skew2D(Vec2f skew);

		//3D transformations
		static Quaternion getRotationQuaternion(double rotation, double xAxis, double yAxis, double zAxis);
		static Quaternion getRotationQuaternion(double rotation, Vec3f rotationAxis);
		
		static Mat4f QuaternionToMatrix(Quaternion q);
		
		static Mat4f rotation3DX(double rotation);
		static Mat4f rotation3DY(double rotation);
		static Mat4f rotation3DZ(double rotation);

		static Mat4f translation3D(double x, double y, double z);
		static Mat4f translation3D(Vec3f trans);

		static Mat4f scale3D(double x, double y, double z);
		static Mat4f scale3D(Vec3f trans);

		static Mat4f skew3D(double x, double y, double z);
		static Mat4f skew3D(Vec3f trans);
		
		static std::vector<double> solveLinear(double A, double B);
		static std::vector<ComplexNumber> solveQuadratic(double A, double B, double C);
		static std::vector<ComplexNumber> solveCubic(double A, double B, double C, double D);

		static std::vector<double> solveQuadraticReal(double A, double B, double C);
		static std::vector<double> solveCubicReal(double A, double B, double C, double D);

		static std::vector<double> reducePolynomial(std::vector<double> constants, double zero);
		
		static double binomialCoefficient(int n, int k);

		static double logisticsSigmoid(double x);


		static ComplexNumber discreteFourierTransform(ComplexNumber* arr, int size, double x, bool inverse=false);
		static std::vector<ComplexNumber> fourierTransform(ComplexNumber* arr, int size, bool inverse=false);
		static std::vector<ComplexNumber> fastFourierTransform(ComplexNumber* arr, int size, bool inverse=false);
		static std::vector<ComplexNumber> fastFourierTransformTest(ComplexNumber* arr, int size, bool inverse=false);

		static double discreteCosineTransform(double*, int size, int u, bool inverse=false);
		static std::vector<double> cosineTransform(double* arr, int size, bool inverse=false);
		static std::vector<double> fastCosineTransform(double* arr, int size, bool inverse=false);

		static double discreteCosineTransform2D(Matrix& arr, int u, int v, bool inverse=false);
		static Matrix cosineTransform2D(Matrix& arr, bool inverse=false);
		static Matrix fastCosineTransform2D(Matrix& arr, bool inverse=false);
		
		
		//Special Case for JPEG
		static void FCT8(double* arr, double* output, bool inverse=false);
		static void FCT8x8(Matrix& arr, Matrix* output, bool inverse=false);


		static double discreteSineTransform(double*, int size, int u, bool inverse=false);
		static std::vector<double> sineTransform(double* arr, int size, bool inverse=false);
		static std::vector<double> fastSineTransform(double* arr, int size, bool inverse=false);

		static double discreteSineTransform2D(Matrix& arr, int u, int v, bool inverse=false);
		static Matrix sineTransform2D(Matrix& arr, bool inverse=false);
		static Matrix fastSineTransform2D(Matrix& arr, bool inverse=false);

		//Clustering algorigthms
		static std::vector<std::vector<Vec2f>> meanCut(std::vector<Vec2f> arr, int clusters, bool meansOnly = false);
		static std::vector<std::vector<Vec2f>> medianCut(std::vector<Vec2f> arr, int clusters, bool meansOnly = false);
		static std::vector<std::vector<Vec2f>> kMeans(std::vector<Vec2f> arr, int clusters, int maxIterations, bool meansOnly = false);
		
		static std::vector<std::vector<Vec3f>> meanCut(std::vector<Vec3f> arr, int clusters, bool meansOnly = false);
		static std::vector<std::vector<Vec3f>> medianCut(std::vector<Vec3f> arr, int clusters, bool meansOnly = false);
		static std::vector<std::vector<Vec3f>> kMeans(std::vector<Vec3f> arr, int clusters, int maxIterations, bool meansOnly = false);
		
		static std::vector<std::vector<Vec4f>> meanCut(std::vector<Vec4f> arr, int clusters, bool meansOnly = false);
		static std::vector<std::vector<Vec4f>> medianCut(std::vector<Vec4f> arr, int clusters, bool meansOnly = false);
		static std::vector<std::vector<Vec4f>> kMeans(std::vector<Vec4f> arr, int clusters, int maxIterations, bool meansOnly = false);
		
		static std::vector<std::vector<GeneralVector>> meanCut(std::vector<GeneralVector> arr, int clusters, bool meansOnly = false);
		static std::vector<std::vector<GeneralVector>> medianCut(std::vector<GeneralVector> arr, int clusters, bool meansOnly = false);
		static std::vector<std::vector<GeneralVector>> kMeans(std::vector<GeneralVector> arr, int clusters, int maxIterations, bool meansOnly = false);
		
	private:
		static std::vector<ComplexNumber> doFFT(ComplexNumber* arr, int size, bool inverse=false);
		static void doFFTTest(ComplexNumber* output, int size, int incVal, bool inverse=false, ComplexNumber* tempData=nullptr);
		
	};

} //NAMESPACE glib END