#include "MathExt.h"
#include "System.h"
#include "StringTools.h"
#include "Sort.h"
#include "LCG.h"
#include <float.h>

#undef max
#undef min

MathExt::MathExt()
{
	
}


MathExt::~MathExt()
{
}


int MathExt::max(int a, int b)
{
	return ((a > b) ? a : b);
}

double MathExt::max(double a, double b)
{
	return ((a > b) ? a : b);
}

float MathExt::max(float a, float b)
{
	return ((a > b) ? a : b);
}

long MathExt::max(long a, long b)
{
	return ((a > b) ? a : b);
}

int MathExt::max(int * ar, int size)
{
	int maxValue = ar[0];

	for (int i = 1; i < size; i++)
	{
		maxValue = max(ar[i], maxValue);
	}

	return maxValue;
}

double MathExt::max(double * ar, int size)
{
	double maxValue = ar[0];

	for (int i = 1; i < size; i++)
	{
		maxValue = max(ar[i], maxValue);
	}

	return maxValue;
}

float MathExt::max(float * ar, int size)
{
	float maxValue = ar[0];

	for (int i = 1; i < size; i++)
	{
		maxValue = max(ar[i], maxValue);
	}

	return maxValue;
}

long MathExt::max(long * ar, int size)
{
	long maxValue = ar[0];

	for (int i = 1; i < size; i++)
	{
		maxValue = max(ar[i], maxValue);
	}

	return maxValue;
}

int MathExt::min(int a, int b)
{
	return ((a < b) ? a : b);
}

double MathExt::min(double a, double b)
{
	return ((a < b) ? a : b);
}

float MathExt::min(float a, float b)
{
	return ((a < b) ? a : b);
}

long MathExt::min(long a, long b)
{
	return ((a < b) ? a : b);
}

int MathExt::min(int * ar, int size)
{
	int minValue = ar[0];

	for (int i = 1; i < size; i++)
	{
		minValue = min(ar[i], minValue);
	}

	return minValue;
}

double MathExt::min(double * ar, int size)
{
	double minValue = ar[0];

	for (int i = 1; i < size; i++)
	{
		minValue = min(ar[i], minValue);
	}

	return minValue;
}

float MathExt::min(float * ar, int size)
{
	float minValue = ar[0];

	for (int i = 1; i < size; i++)
	{
		minValue = min(ar[i], minValue);
	}

	return minValue;
}

long MathExt::min(long * ar, int size)
{
	long minValue = ar[0];

	for (int i = 1; i < size; i++)
	{
		minValue = min(ar[i], minValue);
	}

	return minValue;
}

float MathExt::floor(float a)
{
	return std::floorf(a);
}

float MathExt::ceil(float a)
{
	return std::ceilf(a);
}

float MathExt::round(float a)
{
	return std::roundf(a);
}

double MathExt::floor(double a)
{
	return std::floor(a);
}

double MathExt::ceil(double a)
{
	return std::ceil(a);
}

double MathExt::round(double a)
{
	return std::round(a);
}

double MathExt::frac(double a)
{
	return (a - MathExt::floor(a));
}

float MathExt::frac(float a)
{
	return (a - MathExt::floor(a));
}


float MathExt::roundToDecimal(float a, int decimalPlaces)
{
	float b = a * MathExt::pow(10.0f, decimalPlaces);
	b = MathExt::round(b);
	return b * MathExt::pow(10.0f, -decimalPlaces);
}
float MathExt::roundToDecimal(double a, int decimalPlaces)
{
	double b = a * MathExt::pow(10.0, decimalPlaces);
	b = MathExt::round(b);
	return b * MathExt::pow(10.0, -decimalPlaces);
}

int MathExt::sqr(int a)
{
	return a*a;
}

float MathExt::sqr(float a)
{
	return a*a;
}

double MathExt::sqr(double a)
{
	return a*a;
}

long MathExt::sqr(long a)
{
	return a*a;
}

int MathExt::cube(int a)
{
	return a*a*a;
}

float MathExt::cube(float a)
{
	return a*a*a;
}

double MathExt::cube(double a)
{
	return a*a*a;
}

long MathExt::cube(long a)
{
	return a*a*a;
}

double MathExt::sqrt(int a)
{
	return std::sqrt(a);
}

float MathExt::sqrt(float a)
{
	return std::sqrtf(a);
}

double MathExt::sqrt(double a)
{
	return std::sqrt(a);
}

double MathExt::cubeRoot(int a)
{
	return std::cbrt(a);
}

float MathExt::cubeRoot(float a)
{
	return std::cbrtf(a);
}

double MathExt::cubeRoot(double a)
{
	return std::cbrt(a);
}

float MathExt::pow(float value, float power)
{
	return std::powf(value, power);
}

double MathExt::pow(double value, double power)
{
	return std::pow(value, power);
}

float MathExt::log(float value, float base)
{
	return std::log(value)/std::log(base);
}

double MathExt::log(double value, double base)
{
	return std::log(value)/std::log(base);
}

int MathExt::abs(int a)
{
	return std::abs(a);
}

double MathExt::abs(double a)
{
	return std::abs(a);
}

float MathExt::abs(float a)
{
	return std::abs(a);
}

long MathExt::abs(long a)
{
	return std::abs(a);
}

unsigned int MathExt::random()
{
	return std::rand();
}

int MathExt::randomRange(int min, int max)
{
	int dis = max - min;

	int value = (std::rand() % dis) + min;

	return value;
}

int MathExt::clamp(int value, int min, int max)
{
	if (value <= min)
		return min;
	else if (value >= max)
		return max;
	else
		return value;
}

double MathExt::clamp(double value, double min, double max)
{
	if (value <= min)
		return min;
	else if (value >= max)
		return max;
	else
		return value;
}

float MathExt::clamp(float value, float min, float max)
{
	if (value <= min)
		return min;
	else if (value >= max)
		return max;
	else
		return value;
}

long MathExt::clamp(long value, long min, long max)
{
	if (value <= min)
		return min;
	else if (value >= max)
		return max;
	else
		return value;
}

float MathExt::toDeg(float rad)
{
	return 180 * rad / PI;
}

double MathExt::toDeg(double rad)
{
	return 180 * rad / PI;
}

float MathExt::toRad(float deg)
{
	return PI * deg / 180;
}

double MathExt::toRad(double deg)
{
	return PI * deg / 180;
}

float MathExt::cos(float value)
{
	return std::cosf(value);
}

double MathExt::cos(double value)
{
	return std::cos(value);
}

float MathExt::dcos(float value)
{
	return std::cosf(toRad(value));
}

double MathExt::dcos(double value)
{
	return std::cos(toRad(value));
}

float MathExt::arccos(float value)
{
	return std::acosf(value);
}

double MathExt::arccos(double value)
{
	return std::acos(value);
}

float MathExt::darccos(float value)
{
	return toDeg(std::acosf(value));
}

double MathExt::darccos(double value)
{
	return toDeg(std::acos(value));
}

float MathExt::sin(float value)
{
	return std::sinf(value);
}

double MathExt::sin(double value)
{
	return std::sin(value);
}

float MathExt::dsin(float value)
{
	return std::sinf(toRad(value));
}

double MathExt::dsin(double value)
{
	return std::sin(toRad(value));
}

float MathExt::arcsin(float value)
{
	return std::asinf(value);
}

double MathExt::arcsin(double value)
{
	return std::asin(value);
}

float MathExt::darcsin(float value)
{
	return toDeg(std::asinf(value));
}

double MathExt::darcsin(double value)
{
	return toDeg(std::asin(value));
}

float MathExt::tan(float value)
{
	return std::tanf(value);
}

double MathExt::tan(double value)
{
	return std::tan(value);
}

float MathExt::dtan(float value)
{
	return std::tanf(toRad(value));
}

double MathExt::dtan(double value)
{
	return std::tan(toRad(value));
}

float MathExt::arctan(float value)
{
	return std::atanf(value);
}

double MathExt::arctan(double value)
{
	return std::atan(value);
}

float MathExt::arctan2(float y, float x)
{
	return std::atan2f(y, x);
}

double MathExt::arctan2(double y, double x)
{
	return std::atan2(y, x);
}

float MathExt::darctan(float value)
{
	return toDeg(std::atanf(value));
}

double MathExt::darctan(double value)
{
	return toDeg(std::atan(value));
}

float MathExt::darctan2(float y, float x)
{
	return toDeg(std::atan2f(y,x));
}

double MathExt::darctan2(double y, double x)
{
	return toDeg(std::atan2(y,x));
}

double MathExt::dirToPoint(double x, double y, double x2, double y2)
{
	Line l = Line(x, y, x2, y2);
	double degVal = 0;

	if (l.getToPoint().x != 0)
	{
		degVal = arctan(l.getToPoint().y / l.getToPoint().x);

		if (x > 0)
		{
			if (y > 0)
			{
				//nothing
			}
			else
			{
				degVal += 2 * PI;
			}
		}
		else
		{
			if (y > 0)
			{
				degVal += PI;
			}
			else
			{
				degVal += PI;
			}
		}
	}
	else
	{
		if (y > 0)
		{
			degVal = PI/2;
		}
		else
		{
			degVal = 3*PI/2;
		}
	}

	return degVal;
}

double MathExt::dirToPointDeg(double x, double y, double x2, double y2)
{
	double radVal = dirToPoint(x, y, x2, y2);

	return toDeg(radVal);
}

double MathExt::dot(Vec2f v1, Vec2f v2)
{
	return v1.x*v2.x + v1.y*v2.y;
}

double MathExt::dot(Vec3f v1, Vec3f v2)
{
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

double MathExt::dot(Vec4f v1, Vec4f v2)
{
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z + v1.w*v2.w;
}

double MathExt::dot(GeneralVector v1, GeneralVector v2)
{
	double result = 0;
	if (v1.getSize() == v2.getSize())
	{
		for (int i = 0; i < v1.getSize(); i++)
		{
			result += v1.getValue(i) * v2.getValue(i);
		}
	}

	return result;
}

double MathExt::vecLength(Vec2f v1)
{
	return sqrt(sqr(v1.x) + sqr(v1.y));
}
double MathExt::vecLength(Vec3f v1)
{
	return sqrt(sqr(v1.x) + sqr(v1.y) + sqr(v1.z));
}
double MathExt::vecLength(Vec4f v1)
{
	return sqrt(sqr(v1.x) + sqr(v1.y) + sqr(v1.z) + sqr(v1.w));
}
double MathExt::vecLength(GeneralVector v1)
{
	double lengthSum = 0;
	for(int i=0; i<v1.getSize(); i++)
	{
		lengthSum += MathExt::sqr(v1.getValue(i));
	}
	return sqrt(lengthSum);
}

Vec2f MathExt::normalize(Vec2f v1)
{
	double length = vecLength(v1);
	if (length != 0)
		return v1/length;
	else
		return Vec2f();
}

Vec3f MathExt::normalize(Vec3f v1)
{
	double length = vecLength(v1);

	if (length != 0)
		return v1/length;
	else
		return Vec3f();
}

Vec4f MathExt::normalize(Vec4f v1)
{
	double length = vecLength(v1);

	if (length != 0)
		return v1/length;
	else
		return Vec4f();
}

GeneralVector MathExt::normalize(GeneralVector v1)
{
	double len = vecLength(v1);

	if (len != 0)
		return v1/len;
	else
		return GeneralVector(v1.getSize());
}

Quaternion MathExt::normalize(Quaternion q1)
{
	double len = MathExt::sqrt( MathExt::sqr(q1.a) + MathExt::sqr(q1.b) + MathExt::sqr(q1.c) + MathExt::sqr(q1.d));
	if(len!=0)
		return q1/len;
	else
		return Quaternion();
}

Vec2f MathExt::inverseVec(Vec2f f)
{
	return Vec2f(-f.y, f.x);
}

double MathExt::dotNorm(Vec2f v1, Vec2f v2)
{
	Vec2f normV1 = MathExt::normalize(v1);
	Vec2f normV2 = MathExt::normalize(v2);
	
	return normV1.x*normV2.x + normV1.y*normV2.y;
}

double MathExt::dotNorm(Vec3f v1, Vec3f v2)
{
	Vec3f normV1 = MathExt::normalize(v1);
	Vec3f normV2 = MathExt::normalize(v2);
	
	return normV1.x*normV2.x + normV1.y*normV2.y + normV1.z*normV2.z;
}

double MathExt::dotNorm(Vec4f v1, Vec4f v2)
{
	Vec4f normV1 = MathExt::normalize(v1);
	Vec4f normV2 = MathExt::normalize(v2);
	
	return normV1.x*normV2.x + normV1.y*normV2.y + normV1.z*normV2.z + normV1.w*normV2.w;
}

double MathExt::dotNorm(GeneralVector v1, GeneralVector v2)
{
	GeneralVector normV1 = MathExt::normalize(v1);
	GeneralVector normV2 = MathExt::normalize(v2);
	double result = 0;
	if (normV1.getSize() == normV2.getSize())
	{
		for (int i = 0; i < normV1.getSize(); i++)
		{
			result += normV1.getValue(i) * normV2.getValue(i);
		}
	}

	return result;
}

double MathExt::distanceTo(double x1, double y1, double x2, double y2)
{
	return MathExt::sqrt(MathExt::sqr(x2-x1) + MathExt::sqr(y2-y1));
}

double MathExt::distanceTo(Vec2f p1, Vec2f p2)
{
	Vec2f disVec = p2-p1;
	return MathExt::sqrt(MathExt::sqr(disVec.x) + MathExt::sqr(disVec.y));
}

double MathExt::distanceTo(Vec3f p1, Vec3f p2)
{
	Vec3f disVec = p2-p1;
	return MathExt::sqrt(MathExt::sqr(disVec.x) + MathExt::sqr(disVec.y) + MathExt::sqr(disVec.z));
}

double MathExt::distanceTo(Vec4f p1, Vec4f p2)
{
	Vec4f disVec = p2-p1;
	return MathExt::sqrt(MathExt::sqr(disVec.x) + MathExt::sqr(disVec.y) + MathExt::sqr(disVec.z) + MathExt::sqr(disVec.w));
}

double MathExt::distanceTo(GeneralVector p1, GeneralVector p2)
{
	if(p1.getSize() == p2.getSize())
	{
		GeneralVector disVec = p2-p1;
		double sqrLen = 0;
		for(int i=0; i<p1.getSize(); i++)
		{
			sqrLen += MathExt::sqr(disVec.getValue(i));
		}

		return MathExt::sqrt(sqrLen);
	}

	return -1;
}

#pragma region Transformations_2D
Mat3f MathExt::rotation2D(double rotation)
{
	return rotation2D(rotation, 0, 0);
}

Mat3f MathExt::rotation2D(double rotation, double x, double y)
{
	return rotation2D(rotation, Vec2f(x,y));
}

Mat3f MathExt::rotation2D(double rotation, Vec2f pos)
{
	Mat3f mat = Mat3f();

	mat[0][0] = MathExt::cos(rotation);
	mat[0][1] = MathExt::sin(rotation);
	mat[0][2] = -pos.x*MathExt::cos(rotation) - pos.y*MathExt::sin(rotation) + pos.x;

	mat[1][0] = -MathExt::sin(rotation);
	mat[1][1] = MathExt::cos(rotation);
	mat[1][2] = pos.x*MathExt::sin(rotation) - pos.y*MathExt::cos(rotation) + pos.y;

	mat[2][0] = 0;
	mat[2][1] = 0;
	mat[2][2] = 1;
	

	return mat;
}
	
Mat3f MathExt::translation2D(double x, double y)
{
	return MathExt::translation2D(Vec2f(x,y));
}

Mat3f MathExt::translation2D(Vec2f trans)
{
	Mat3f mat = Mat3f();

	mat[0][0] = 1;
	mat[0][1] = 0;
	mat[0][2] = trans.x;

	mat[1][0] = 0;
	mat[1][1] = 1;
	mat[1][2] = trans.y;

	mat[2][0] = 0;
	mat[2][1] = 0;
	mat[2][2] = 1;
	
	return mat;
}

Mat3f MathExt::scale2D(double x, double y)
{
	return MathExt::scale2D(Vec2f(x,y));
}

Mat3f MathExt::scale2D(Vec2f scale)
{
	Mat3f mat = Mat3f();

	mat[0][0] = scale.x;
	mat[0][1] = 0;
	mat[0][2] = 0;

	mat[1][0] = 0;
	mat[1][1] = scale.y;
	mat[1][2] = 0;

	mat[2][0] = 0;
	mat[2][1] = 0;
	mat[2][2] = 1;
	
	return mat;
}

Mat3f MathExt::skew2D(double x, double y)
{
	return MathExt::skew2D(Vec2f(x,y));
}

Mat3f MathExt::skew2D(Vec2f skew)
{
	Mat3f mat = Mat3f();

	mat[0][0] = 1;
	mat[0][1] = skew.x;
	mat[0][2] = 0;

	mat[1][0] = skew.y;
	mat[1][1] = 1;
	mat[1][2] = 0;

	mat[2][0] = 0;
	mat[2][1] = 0;
	mat[2][2] = 1;
	
	return mat;
}
#pragma endregion

#pragma region Transformations_3D

Quaternion MathExt::getRotationQuaternion(double rotation, double xAxis, double yAxis, double zAxis)
{
	return getRotationQuaternion(rotation, Vec3f(xAxis, yAxis, zAxis));
}

Quaternion MathExt::getRotationQuaternion(double rotation, Vec3f rotationAxis)
{
	double halfAngle = rotation/2;
	double cosHalfAngle = MathExt::cos(halfAngle);
	Vec3f axisVals = rotationAxis * MathExt::sin(halfAngle);
	
	return Quaternion(cosHalfAngle, axisVals.x, axisVals.y, axisVals.z);
}

Mat4f MathExt::QuaternionToMatrix(Quaternion q)
{
	Mat4f matrix = Mat4f();
	
	matrix[0][0] = q.a;
	matrix[0][1] = -q.b;
	matrix[0][2] = -q.c;
	matrix[0][3] = -q.d;
	
	matrix[1][0] = q.b;
	matrix[1][1] = q.a;
	matrix[1][2] = -q.d;
	matrix[1][3] = q.c;
	
	matrix[2][0] = q.c;
	matrix[2][1] = q.d;
	matrix[2][2] = q.a;
	matrix[2][3] = -q.b;
	
	matrix[3][0] = q.d;
	matrix[3][1] = -q.c;
	matrix[3][2] = q.b;
	matrix[3][3] = q.a;

	return matrix;
}

Mat4f MathExt::rotation3DX(double rotation)
{
	Mat4f mat = Mat4f();

	mat[0][0] = 1;
	mat[0][1] = 0;
	mat[0][2] = 0;
	mat[0][3] = 0;

	mat[1][0] = 0;
	mat[1][1] = MathExt::cos(rotation);
	mat[1][2] = -MathExt::sin(rotation);
	mat[1][3] = 0;

	mat[2][0] = 0;
	mat[2][1] = MathExt::sin(rotation);
	mat[2][2] = MathExt::cos(rotation);
	mat[2][3] = 0;
	
	mat[3][0] = 0;
	mat[3][1] = 0;
	mat[3][2] = 0;
	mat[3][3] = 1;
	
	return mat;
}

Mat4f MathExt::rotation3DY(double rotation)
{
	Mat4f mat = Mat4f();

	mat[0][0] = MathExt::cos(rotation);
	mat[0][1] = 0;
	mat[0][2] = MathExt::sin(rotation);
	mat[0][3] = 0;

	mat[1][0] = 0;
	mat[1][1] = 1;
	mat[1][2] = 0;
	mat[1][3] = 0;

	mat[2][0] = -MathExt::sin(rotation);
	mat[2][1] = 0;
	mat[2][2] = MathExt::cos(rotation);
	mat[2][3] = 0;
	
	mat[3][0] = 0;
	mat[3][1] = 0;
	mat[3][2] = 0;
	mat[3][3] = 1;
	
	return mat;
}

Mat4f MathExt::rotation3DZ(double rotation)
{
	Mat4f mat = Mat4f();

	mat[0][0] = MathExt::cos(rotation);
	mat[0][1] = -MathExt::sin(rotation);
	mat[0][2] = 0;
	mat[0][3] = 0;

	mat[1][0] = MathExt::sin(rotation);
	mat[1][1] = MathExt::cos(rotation);
	mat[1][2] = 0;
	mat[1][3] = 0;

	mat[2][0] = 0;
	mat[2][1] = 0;
	mat[2][2] = 1;
	mat[2][3] = 0;
	
	mat[3][0] = 0;
	mat[3][1] = 0;
	mat[3][2] = 0;
	mat[3][3] = 1;
	
	return mat;
}

Mat4f MathExt::translation3D(double x, double y, double z)
{
	return MathExt::translation3D(Vec3f(x,y,z));
}

Mat4f MathExt::translation3D(Vec3f trans)
{
	Mat4f mat = Mat4f();

	mat[0][0] = 1;
	mat[0][1] = 0;
	mat[0][2] = 0;
	mat[0][3] = trans.x;

	mat[1][0] = 0;
	mat[1][1] = 1;
	mat[1][2] = 0;
	mat[1][3] = trans.y;

	mat[2][0] = 0;
	mat[2][1] = 0;
	mat[2][2] = 1;
	mat[2][3] = trans.z;
	
	mat[3][0] = 0;
	mat[3][1] = 0;
	mat[3][2] = 0;
	mat[3][3] = 1;
	
	return mat;
}

Mat4f MathExt::scale3D(double x, double y, double z)
{
	return MathExt::scale3D(Vec3f(x,y,z));
}

Mat4f MathExt::scale3D(Vec3f scale)
{
	Mat4f mat = Mat4f();

	mat[0][0] = scale.x;
	mat[0][1] = 0;
	mat[0][2] = 0;
	mat[0][3] = 0;

	mat[1][0] = 0;
	mat[1][1] = scale.y;
	mat[1][2] = 0;
	mat[1][3] = 0;

	mat[2][0] = 0;
	mat[2][1] = 0;
	mat[2][2] = scale.z;
	mat[2][3] = 0;
	
	mat[3][0] = 0;
	mat[3][1] = 0;
	mat[3][2] = 0;
	mat[3][3] = 1;
	
	return mat;
}

Mat4f MathExt::skew3D(double x, double y, double z)
{
	return MathExt::skew3D(Vec3f(x,y,z));
}

Mat4f MathExt::skew3D(Vec3f skew)
{
	Mat4f mat = Mat4f();

	mat[0][0] = 1;
	mat[0][1] = -skew.z;
	mat[0][2] = skew.y;
	mat[0][3] = 0;

	mat[1][0] = skew.z;
	mat[1][1] = 1;
	mat[1][2] = -skew.x;
	mat[1][3] = 0;

	mat[2][0] = -skew.y;
	mat[2][1] = skew.x;
	mat[2][2] = 1;
	mat[2][3] = 0;
	
	mat[3][0] = 0;
	mat[3][1] = 0;
	mat[3][2] = 0;
	mat[3][3] = 1;
	
	return mat;
}
#pragma endregion

std::vector<double> MathExt::solveLinear(double A, double B)
{
	// f(x) = Ax + B
	// 0 = Ax + B

	if(A!=0)
	{
		return {-B/A};
	}
	else
	{
		return {};
	}
}

std::vector<ComplexNumber> MathExt::solveQuadratic(double A, double B, double C)
{
	ComplexNumber k1;
	ComplexNumber k2;

	if( MathExt::sqr(B) - 4*A*C > 0)
	{
		k1.real = (-B + MathExt::sqrt( MathExt::sqr(B) - 4*A*C )) / 2;
		k2.real = (-B - MathExt::sqrt( MathExt::sqr(B) - 4*A*C )) / 2;
	}
	else
	{
		k1.real = -B/2;
		k2.real = -B/2;
		
		k1.imaginary = MathExt::sqrt( MathExt::sqr(B) + 4*A*C ) / 2;
		k1.imaginary = -MathExt::sqrt( MathExt::sqr(B) + 4*A*C ) / 2;
	}

	return {k1, k2};
}

std::vector<ComplexNumber> MathExt::solveCubic(double A, double B, double C, double D)
{
	//later
	return {};
}

std::vector<double> MathExt::solveQuadraticReal(double A, double B, double C)
{
	double k1;
	double k2;

	if( MathExt::sqr(B) - 4*A*C >= 0)
	{
		k1 = (-B + MathExt::sqrt( MathExt::sqr(B) - 4*A*C )) / (2*A);
		k2 = (-B - MathExt::sqrt( MathExt::sqr(B) - 4*A*C )) / (2*A);
		
		return {k1, k2};
	}

	return {};
}

std::vector<double> MathExt::solveCubicReal(double A, double B, double C, double D)
{
	if(A!=0)
	{
		double p = (3.0*A*C - MathExt::sqr(B))/ (3.0*MathExt::sqr(A));
		double q = (2.0*MathExt::cube(B) - 9.0*A*B*C + 27.0*MathExt::sqr(A)*D) / (27.0*MathExt::cube(A));

		double p3 = MathExt::cube(p);
		double q2 = MathExt::sqr(q);
		//new quadratic X^3 + Px + Q = 0
		//If (4P^3 + 27Q^2 < 0)
		//		use trig then reduce
		//else
		//		use formula, only one solution
		//resolve for real solutions since we transformed the cubic

		double testForSolutions = 4.0*p3 + 27.0*q2;

		if(testForSolutions < 0)
		{
			//StringTools::println("3 Solution");
			//could find all solutions this way, but we have to use arccos multiple times
			double temp1 = 2.0*MathExt::sqrt(-p/3.0);
			double temp2 = ((3.0*q)/(2.0*p)) * MathExt::sqrt(-3.0/p);

			double solution1 = temp1 * MathExt::cos(MathExt::arccos(temp2)/3.0);

			double actualSolution = solution1 - (B/(3.0*A));
			
			std::vector<double> quadratic = MathExt::reducePolynomial({A,B,C,D}, actualSolution);
			
			//solve quadratic
			std::vector<double> otherSolutions = solveQuadraticReal(quadratic[0], quadratic[1], quadratic[2]);

			//combine solutions into one
			return {actualSolution, otherSolutions[0], otherSolutions[1]};
		}
		else if(testForSolutions > 0)
		{
			//StringTools::println("1 Solution");
			//doesn't matter in this case whether we use + or -
			double temp = (-q - MathExt::sqrt(q2 + 4.0*p3/27.0))/2.0;
			double w = MathExt::cubeRoot(temp);
			double solution = w - p/3.0*w;
			double actualSolution = solution + B/(3.0*A);
			
			return {actualSolution};
		}
		else
		{
			//they equal zero and therfore have 3 solutions, but they can be solved
			//easier than the other ones.
			if(p==0)
			{
				return {0.0,0.0,0.0};
			}
			else
			{
				
				double sol1 = (3.0*q/p) - B/(3.0*A);
				double sol2 = ((-3.0*q)/(2.0*p)) - B/(3.0*A);
				return {sol1,sol2,sol2};
			}
			
		}
	}
	else
	{
		//its just a quadratic
		return MathExt::solveQuadraticReal(B,C,D);
	}
}

std::vector<double> MathExt::reducePolynomial(std::vector<double> constants, double zero)
{
	//using synthetic division
	std::vector<double> newConstants = std::vector<double>(constants.size()-1);
	if(constants.size()>0)
	{
		newConstants[0] = constants[0];
		for(int i=1; i<constants.size()-1; i++)
		{
			newConstants[i] = constants[i] + newConstants[i-1] * zero;
		}
	}
	return newConstants;
}

double MathExt::binomialCoefficient(int n, int k)
{
	double value = 1;

	for(int i=1; i<k; i++)
	{
		value *= (n+1-i)/i;
	}
	return value;
}

double MathExt::logisticsSigmoid(double x)
{
	if (x != 0)
	{
		return 1.0 / (1.0 + pow(E, -x));
	}
	else
	{
		//divide by zero error
		return 0;
	}
}

ComplexNumber MathExt::discreteFourierTransform(double* arr, int size, double x)
{
	//Xk = sumFrom 0 to N-1 of ( xn * e ^ -(i2pi*kn)/N )
	//or
	//xn * cos(2pi*kn/N) - i*sin(2pi*kn/N)
	//We can separate the real and imaginary parts and only do the real part

	ComplexNumber finalAnswer = ComplexNumber();

	double xFactor = (2.0 * PI * x)/size;

	for (int n = 0; n < size-1; n++)
	{
		finalAnswer.real += arr[n] * cos(-xFactor * n);
		finalAnswer.imaginary += arr[n] * sin(-xFactor * n);
	}

	return finalAnswer;
}


ComplexNumber* MathExt::fourierTransform(double* arr, int size)
{
	ComplexNumber* output = new ComplexNumber[size];
	for(int i=0; i<size; i++)
	{
		output[i] = MathExt::discreteFourierTransform(arr, size, (double)i);
	}

	return output;
}

ComplexNumber* MathExt::fastFourierTransform(double* arr, int size)
{
	//cooley tukey algorithm
	//Could be a bit faster. The second half may be completely reflective of the first half.
	//The second half can not be used regardless due to nyquist limit.
	ComplexNumber* output = new ComplexNumber[size];

	for(int i=0; i<size; i++)
	{
		ComplexNumber frequency = MathExt::doFFT(arr, size, 0, size, 1, i);
		output[i] = frequency;
	}

	return output;
}

ComplexNumber MathExt::doFFT(double* arr, int size, int index, int globalSize, int multVal, double value)
{
	//divide by 2 and do thing

	if(size>1)
	{
		int newSize = size/2;
		double anglePart = (-2*PI*value)/globalSize;

		ComplexNumber split1 = doFFT(arr, newSize, index, 2*multVal, globalSize, value);
		ComplexNumber cConst = ComplexNumber(MathExt::cos(anglePart), MathExt::sin(anglePart));
		ComplexNumber split2 = doFFT(arr, newSize, index + 1, 2*multVal, globalSize, value) * cConst;

		return split1 + split2;
	}
	else
	{
		return ComplexNumber(arr[index], 0);
	}
}


std::vector<std::vector<GeneralVector>> MathExt::meanCut(std::vector<GeneralVector> arr, int clusters, bool meansOnly)
{
	int dimensions = arr[0].getSize();
	struct BoxInfo
	{
		std::vector<GeneralVector> arr = std::vector<GeneralVector>();
		GeneralVector error;
		GeneralVector averageVal;
		bool beenSet = false;
	};

	std::vector< BoxInfo > boxes = std::vector< BoxInfo >();
	boxes.push_back( BoxInfo() );

	boxes[0].arr = arr;
	boxes[0].error = GeneralVector(dimensions);
	boxes[0].averageVal = GeneralVector(dimensions);

	while(true)
	{
		for(BoxInfo& f : boxes)
		{
			if(!f.beenSet)
			{
				//first, find mean value
				GeneralVector meanVal = GeneralVector(dimensions);
				double meanMult = 1.0/f.arr.size();
				for(int i=0; i<f.arr.size(); i++)
				{
					GeneralVector v = f.arr[i];
					for(int k=0; k<dimensions; k++)
					{
						meanVal[k] += v[k] * meanMult;
					}
				}

				//then find error^2 for all dimensions
				GeneralVector error = GeneralVector(dimensions);
				for(int i=0; i<f.arr.size(); i++)
				{
					GeneralVector v = f.arr[i];
					for(int k=0; k<dimensions; k++)
					{
						error[k] += MathExt::sqr(meanVal[k] - v[k]);
					}
				}

				f.averageVal = meanVal;
				f.error = error;
				f.beenSet = true;
			}
		}

		if(boxes.size() >= clusters)
		{
			break;
		}

		int indexOfMostError = 0;
		double mostError = 0;

		for(int i=0; i<boxes.size(); i++)
		{
			GeneralVector errVec = boxes[i].error;
			double sumError = 0;
			for(int k=0; k<dimensions; k++)
			{
				sumError += errVec[k];
			}

			if(sumError > mostError)
			{
				mostError = sumError;
				indexOfMostError = i;
			}
		}
		
		if(boxes[indexOfMostError].arr.size() <= 1)
		{
			//can't split
			break;
		}

		BoxInfo box = boxes[indexOfMostError];

		GeneralVector avg = box.averageVal;
		
		std::vector<GeneralVector> split1 = std::vector<GeneralVector>();
		std::vector<GeneralVector> split2 = std::vector<GeneralVector>();
		
		//split the box along the part with the most error
		int dimensionWithMostError = 0;
		double dimensionMostError = 0;
		for(int i=0; i<dimensions; i++)
		{
			if(box.error[i] > dimensionMostError)
			{
				dimensionWithMostError = i;
				dimensionMostError = box.error[i];
			}
		}

		for(int i=0; i<box.arr.size(); i++)
		{
			GeneralVector c = box.arr[i];
			if(c[dimensionWithMostError] < avg[dimensionWithMostError])
			{
				split1.push_back(c);
			}
			else
			{
				split2.push_back(c);
			}
		}

		boxes[indexOfMostError].arr = split1;
		boxes[indexOfMostError].beenSet = false;
		
		boxes.push_back( BoxInfo() );
		boxes.back().arr = split2;
		boxes.back().beenSet = false;

		//after split, you have 2 boxes. repeat algorithm on all boxes.
		//Split on the box with the most error which is the sum of all of the boxes x,y,z error
		//Insures that you end up with the amount of boxes you need to get the appropriate amount of clusters
		//Average the clusters on each box to get the final paletteArr.
	}

	
	std::vector< std::vector<GeneralVector>> finalGroups = std::vector<std::vector<GeneralVector>>();
	if(meansOnly)
	{
		for(BoxInfo b : boxes)
		{
			//push averages as groups
			finalGroups.push_back( {b.averageVal} );
		}
	}
	else
	{
		for(BoxInfo b : boxes)
		{
			//push groups
			finalGroups.push_back( b.arr );
		}
	}

	return finalGroups;
}

std::vector<std::vector<GeneralVector>> MathExt::medianCut(std::vector<GeneralVector> arr, int clusters, bool meansOnly)
{
	int dimensions = arr[0].getSize();
	std::vector<int> endPos = std::vector<int>();
	std::vector<GeneralVector> sortArray = std::vector<GeneralVector>(arr);

	endPos.push_back(arr.size());

	while(endPos.size() < clusters)
	{
		int currentSize = endPos.size();
		for(int i=0; i<currentSize; i++)
		{
			int start = 0;
			int end = endPos[i];
			if(i!=0)
			{
				start = endPos[i-1];
			}

			//sort by most range
			
			GeneralVector minVals = sortArray[start];
			GeneralVector maxVals = sortArray[start];
			GeneralVector ranges = GeneralVector(dimensions);
			int dimensionToSortBy = 0;
			
			for(int i=start; i<end; i++)
			{
				for(int k=0; k<dimensions; k++)
				{
					if(sortArray[i][k] < minVals[k])
					{
						minVals[k] = sortArray[i][k];
					}
					if(sortArray[i][k] > maxVals[k])
					{
						maxVals[k] = sortArray[i][k];
					}
				}
			}

			ranges[0] = maxVals[0] - minVals[0];
			double maxRange = ranges[0];
			for(int k=1; k<dimensions; k++)
			{
				ranges[k] = maxVals[k] - minVals[k];
				if(ranges[k] > maxRange)
				{
					maxRange = ranges[k];
					dimensionToSortBy = k;
				}
			}

			Sort::mergeSort<GeneralVector>(sortArray.data()+start, end-start, [dimensionToSortBy](GeneralVector a, GeneralVector b) -> bool{
				return a[dimensionToSortBy] < b[dimensionToSortBy];
			});
		}

		std::vector<int> newEndPos = std::vector<int>();

		newEndPos.push_back(endPos[0]/2);
		newEndPos.push_back(endPos[0]);

		for(int i=1; i<currentSize; i++)
		{
			int midIndex = (endPos[i-1] + endPos[i])/2;
			
			newEndPos.push_back(midIndex);
			newEndPos.push_back(endPos[i]);
		}

		endPos = newEndPos;
	}


	//average out each sections for the final set of clusters
	std::vector<std::vector<GeneralVector>> finalclusters = std::vector<std::vector<GeneralVector>>();

	if(meansOnly)
	{
		for(int i=0; i<endPos.size(); i++)
		{
			int start = 0;
			int end = endPos[i];
			if(i!=0)
			{
				start = endPos[i-1];
			}

			GeneralVector avgVal = GeneralVector(dimensions);
			int divVal = end-start;
			for(int k=start; k<end; k++)
			{
				GeneralVector c = sortArray[k];
				avgVal += c;
			}

			avgVal/=divVal;


			finalclusters.push_back( {avgVal} );
		}
	}
	else
	{
		for(int i=0; i<endPos.size(); i++)
		{
			int start = 0;
			int end = endPos[i];
			if(i!=0)
			{
				start = endPos[i-1];
			}

			std::vector<GeneralVector> group = std::vector<GeneralVector>();
			for(int k=start; k<end; k++)
			{
				group.push_back(sortArray[k]);
			}

			finalclusters.push_back( group );
		}
	}

	return finalclusters;
}

std::vector<std::vector<GeneralVector>> MathExt::kMeans(std::vector<GeneralVector> arr, int clusters, int maxIterations, bool meansOnly)
{
	int dimensions = arr[0].getSize();
	struct BoxInfo
	{
		std::vector<GeneralVector> clusters = std::vector<GeneralVector>();
		GeneralVector averageVal = GeneralVector();
	};

	//pick k means
	std::vector< BoxInfo > groups = std::vector< BoxInfo >();
	
	//pick randomly
	unsigned int currentAmount = 0;
	LCG lcg = LCG( (unsigned int)System::getCurrentTimeNano(), 12354, 0, arr.size());

	for(int i=0; i<clusters; i++)
	{
		int v = lcg.get();
		BoxInfo b = BoxInfo();
		b.averageVal = arr[v];

		groups.push_back( b );
	}

	//do k-means
	
	for(int i=0; i<maxIterations; i++)
	{
		//clear all groups
		for(int k=0; k<groups.size(); k++)
		{
			groups[k].clusters.clear();
		}

		//group into k groups
		for(int k=0; k<arr.size(); k++)
		{
			int minIndex = -1;
			double minDis = DBL_MAX;

			//measure distance from all means
			for(int j=0; j<groups.size(); j++)
			{
				double thisDis = 0;
				GeneralVector lengthVec = GeneralVector(dimensions);
				lengthVec = groups[j].averageVal - arr[k];

				thisDis = MathExt::vecLength(lengthVec);

				if(thisDis < minDis)
				{
					minIndex = j;
					minDis = thisDis;
				}
			}

			if(minIndex >= 0)
			{
				groups[minIndex].clusters.push_back( arr[k] );
			}
		}

		//recompute average
		bool same = true;
		for(int j=0; j<groups.size(); j++)
		{
			GeneralVector avg = GeneralVector(dimensions);
			double divVal = 1.0 / groups[j].clusters.size();

			for(GeneralVector c : groups[j].clusters)
			{
				avg += c * divVal;
			}

			if(groups[j].averageVal != avg)
			{
				same = false;
			}

			groups[j].averageVal = avg;
		}

		if(same)
		{
			break;
		}
	}

	std::vector<std::vector<GeneralVector>> finalGroups = std::vector<std::vector<GeneralVector>>();

	if(meansOnly)
	{
		for(BoxInfo k : groups)
		{
			finalGroups.push_back( {k.averageVal} );
		}
	}
	else
	{
		for(BoxInfo k : groups)
		{
			finalGroups.push_back( k.clusters );
		}
	}

	return finalGroups;
}

