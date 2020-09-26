#pragma once
#include<cmath>
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

#undef max
#undef min

#define PI 3.1415927
#define E 2.7182818

class MathExt
{
public:
	MathExt();
	~MathExt();

	static int max(int a, int b);
	static double max(double a, double b);
	static float max(float a, float b);
	static long max(long a, long b);

	static int max(int* ar, int size);
	static double max(double* ar, int size);
	static float max(float* ar, int size);
	static long max(long* ar, int size);

	static int min(int a, int b);
	static double min(double a, double b);
	static float min(float a, float b);
	static long min(long a, long b);

	static int min(int* ar, int size);
	static double min(double* ar, int size);
	static float min(float* ar, int size);
	static long min(long* ar, int size);
	
	static float floor(float a);
	static float ceil(float a);
	static float round(float a);

	static double floor(double a);
	static double ceil(double a);
	static double round(double a);

	static double frac(double a);
	static float frac(float a);

	static float roundToDecimal(float a, int decimalPlaces=6);
	static float roundToDecimal(double a, int decimalPlaces=6);

	static int sqr(int a);
	static float sqr(float a);
	static double sqr(double a);
	static long sqr(long a);
	
	static int cube(int a);
	static float cube(float a);
	static double cube(double a);
	static long cube(long a);

	static double sqrt(int a);
	static float sqrt(float a);
	static double sqrt(double a);

	static double cubeRoot(int a);
	static float cubeRoot(float a);
	static double cubeRoot(double a);

	static float pow(float value, float power);
	static double pow(double value, double power);
	
	static float log(float value, float base = 10.0f);
	static double log(double value, double base = 10.0);

	static int abs(int a);
	static double abs(double a);
	static float abs(float a);
	static long abs(long a);

	static unsigned int random();
	static int randomRange(int min, int max);
	
	static int clamp(int value, int min, int max);
	static float clamp(float value, float min, float max);
	static double clamp(double value, double min, double max);
	static long clamp(long value, long min, long max);

	static float toDeg(float rad);
	static double toDeg(double rad);

	static float toRad(float deg);
	static double toRad(double deg);

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

	static float darctan(float value);
	static double darctan(double value);

	static double dirToPoint(double x, double y, double x2, double y2);
	static double dirToPointDeg(double x, double y, double x2, double y2);

	static double dot(Vec2f v1, Vec2f v2);
	static double dot(Vec3f v1, Vec3f v2);
	static double dot(Vec4f v1, Vec4f v2);
	static double dot(GeneralVector v1, GeneralVector v2);

	static double vecLength(Vec2f v);
	static double vecLength(Vec3f v);
	static double vecLength(Vec4f v);
	static double vecLength(GeneralVector v);
	
	static Vec2f normalize(Vec2f v1);
	static Vec3f normalize(Vec3f v1);
	static Vec4f normalize(Vec4f v1);
	static GeneralVector normalize(GeneralVector v1);
	static Quaternion normalize(Quaternion q1);

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

	//2D transformations
	static Mat3f rotation2D(double rotation, double x = 0, double y = 0);
	static Mat3f rotation2D(double rotation, Vec2f pos = Vec2f());
	
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
	static ComplexNumber discreteFourierTransform(double* arr, int size, double x);
	static ComplexNumber discreteFastFourierTransform(double* arr, int size, double x);
	static ComplexNumber* fourierTransform(double* arr, int size);
	static ComplexNumber* fastFourierTransform(double* arr, int size);

private:

	static ComplexNumber doFFT(double* arr, int size, int index, int globalSize, int multVal, double value);
};

