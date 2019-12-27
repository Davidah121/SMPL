#include "MathExt.h"
#include "System.h"

#undef max
#undef min

unsigned long MathExt::seed = 0xFFFF0000;
unsigned long MathExt::mod = 0xFFFFFFFF;
unsigned long MathExt::incr = 2531011;
unsigned long MathExt::mult = 214013;

unsigned long MathExt::preValue = 0xFFFF0000;

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
	preValue = (mult*preValue + incr) % mod;
	return (int)preValue;
	//return std::rand();
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

float MathExt::darctan(float value)
{
	return std::atanf(toRad(value));
}

double MathExt::darctan(double value)
{
	return std::atan(toRad(value));
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

Vec2f MathExt::normalize(Vec2f v1)
{
	double length = sqrt(sqr(v1.x) + sqr(v1.y));

	if (length != 0)
		return Vec2f(v1.x / length, v1.y / length);
	else
		return Vec2f(0, 0);
}

Vec3f MathExt::normalize(Vec3f v1)
{
	double length = sqrt(sqr(v1.x) + sqr(v1.y) + sqr(v1.z));

	if (length != 0)
		return Vec3f(v1.x / length, v1.y / length, v1.z / length);
	else
		return Vec3f(0, 0, 0);
}

Vec4f MathExt::normalize(Vec4f v1)
{
	double length = sqrt(sqr(v1.x) + sqr(v1.y) + sqr(v1.z) + sqr(v1.w));

	if (length != 0)
		return Vec4f(v1.x / length, v1.y / length, v1.z / length, v1.w / length);
	else
		return Vec4f(0, 0, 0, 0);
}

GeneralVector MathExt::normalized(GeneralVector v1)
{
	double len = 0;
	for (int i = 0; i < v1.getSize(); i++)
	{
		len += MathExt::sqr(v1.getValue(i));
	}
	len = MathExt::sqrt(len);

	if (len != 0)
	{
		GeneralVector k = GeneralVector(v1.getSize());
		for (int i = 0; i < k.getSize(); i++)
		{
			k.setValue(v1.getValue(i) / len, i);
		}
	}
	return GeneralVector();
}

double MathExt::logisticsSigmoid(double x)
{
	if (x != 0)
	{
		return 1.0 / (1.0 - pow(E, -x));
	}
	else
	{
		//divide by zero error
		return 0;
	}
}

ComplexNumber MathExt::discreteFouierTransform(double* arr, int size, double x)
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