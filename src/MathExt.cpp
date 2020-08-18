#include "MathExt.h"
#include "System.h"
#include "StringTools.h"

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

GeneralVector MathExt::normalize(GeneralVector v1)
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
		return k;
	}

	return GeneralVector(v1.getSize());
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
		return 1.0 / (1.0 - pow(E, -x));
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