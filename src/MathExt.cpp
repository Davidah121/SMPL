#include "MathExt.h"
#include "System.h"
#include "StringTools.h"
#include "Sort.h"
#include "LCG.h"
#include <float.h>

#undef max
#undef min

namespace glib
{

	
	int MathExt::popcount(uint32_t x)
	{
		x = x - ((x >> 1) & 0x55555555);
		x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
		x = (x + (x >> 4)) & 0x0F0F0F0F;
		return (x * 0x01010101) >> 24;
	}

	int MathExt::popcount(uint64_t x)
	{
		uint64_t temp;
		temp = x * 0x00020004000800010ULL;
		temp = temp & 0x1111111111111111ULL;
		temp = temp * 0x1111111111111111ULL;
		return (temp >> 60);
	}
	
	float MathExt::floor(float a)
	{
		return ::floorf(a);
	}

	float MathExt::ceil(float a)
	{
		return ::ceilf(a);
	}

	float MathExt::round(float a)
	{
		return ::roundf(a);
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
		float b = a * MathExt::pow(10.0f, (float)decimalPlaces);
		b = MathExt::round(b);
		return b * MathExt::pow(10.0f, (float)-decimalPlaces);
	}

	double MathExt::roundToDecimal(double a, int decimalPlaces)
	{
		double b = a * MathExt::pow(10.0, (double)decimalPlaces);
		b = MathExt::round(b);
		return b * MathExt::pow(10.0, (double)-decimalPlaces);
	}

	float MathExt::sqrt(float a)
	{
		return ::sqrtf(a);
	}

	double MathExt::sqrt(double a)
	{
		return std::sqrt(a);
	}

	float MathExt::fastSqrt(float a)
	{
		return 1.0f/MathExt::fastInvSqrt(a);
	}

	float MathExt::fastInvSqrt(float number)
	{
		long i;
		float x2, y;
		const float threehalfs = 1.5F;

		x2 = number * 0.5F;
		y  = number;
		i  = * ( long * ) &y;
		i  = 0x5f3759df - ( i >> 1 );
		y  = * ( float * ) &i;
		y  = y * ( threehalfs - ( x2 * y * y ) );
		y  = y * ( threehalfs - ( x2 * y * y ) );

		return y;
	}

	float MathExt::cubeRoot(float a)
	{
		return ::cbrtf(a);
	}

	double MathExt::cubeRoot(double a)
	{
		return std::cbrt(a);
	}

	float MathExt::pow(float value, float power)
	{
		return ::powf(value, power);
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
	float MathExt::lerp(float a, float b, float blend)
	{
		return (a * (1-blend) + b * blend);
	}
	double MathExt::lerp(double a, double b, double blend)
	{
		return (a * (1-blend) + b * blend);
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

	unsigned int MathExt::randomRange(unsigned int min, unsigned int max)
	{
		unsigned int dis = max - min;

		unsigned int value = (std::rand() % dis) + min;

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

	float MathExt::angleToStandardRange(float angle, bool isDegree)
	{
		float v = angle;
		if(isDegree)
		{
			while(true)
			{
				if(v >= 360)
					v -= 360;
				else if(v < 0)
					v += 360;
				else
					break;
			}
		}
		else
		{
			while(true)
			{
				if( v >= 2*PI)
					v -= (float)(2*PI);
				else if(v < 0)
					v += (float)(2*PI);
				else
					break;
			}
		}
		return v;
	}

	double MathExt::angleToStandardRange(double angle, bool isDegree)
	{
		double v = angle;
		if(isDegree)
		{
			while(true)
			{
				if(v >= 360)
					v -= 360;
				else if(v < 0)
					v += 360;
				else
					break;
			}
		}
		else
		{
			while(true)
			{
				if( v >= 2*PI)
					v -= 2*PI;
				else if(v < 0)
					v += 2*PI;
				else
					break;
			}
		}
		return v;
	}

	

	float MathExt::cos(float value)
	{
		return ::cosf(value);
	}

	double MathExt::cos(double value)
	{
		return std::cos(value);
	}

	float MathExt::dcos(float value)
	{
		return ::cosf(toRad(value));
	}

	double MathExt::dcos(double value)
	{
		return std::cos(toRad(value));
	}

	float MathExt::arccos(float value)
	{
		return ::acosf(value);
	}

	double MathExt::arccos(double value)
	{
		return std::acos(value);
	}

	float MathExt::darccos(float value)
	{
		return toDeg(::acosf(value));
	}

	double MathExt::darccos(double value)
	{
		return toDeg(std::acos(value));
	}

	float MathExt::sin(float value)
	{
		return ::sinf(value);
	}

	double MathExt::sin(double value)
	{
		return std::sin(value);
	}

	float MathExt::dsin(float value)
	{
		return ::sinf(toRad(value));
	}

	double MathExt::dsin(double value)
	{
		return std::sin(toRad(value));
	}

	float MathExt::arcsin(float value)
	{
		return ::asinf(value);
	}

	double MathExt::arcsin(double value)
	{
		return std::asin(value);
	}

	float MathExt::darcsin(float value)
	{
		return toDeg(::asinf(value));
	}

	double MathExt::darcsin(double value)
	{
		return toDeg(std::asin(value));
	}

	float MathExt::tan(float value)
	{
		return ::tanf(value);
	}

	double MathExt::tan(double value)
	{
		return std::tan(value);
	}

	float MathExt::dtan(float value)
	{
		return ::tanf(toRad(value));
	}

	double MathExt::dtan(double value)
	{
		return std::tan(toRad(value));
	}

	float MathExt::arctan(float value)
	{
		return ::atanf(value);
	}

	double MathExt::arctan(double value)
	{
		return std::atan(value);
	}

	float MathExt::arctan2(float y, float x)
	{
		return ::atan2f(y, x);
	}

	double MathExt::arctan2(double y, double x)
	{
		return std::atan2(y, x);
	}

	float MathExt::darctan(float value)
	{
		return toDeg(::atanf(value));
	}

	double MathExt::darctan(double value)
	{
		return toDeg(std::atan(value));
	}

	float MathExt::darctan2(float y, float x)
	{
		return toDeg(::atan2f(y,x));
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

			if (l.getToPoint().x > 0)
			{
				if (l.getToPoint().y > 0)
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
				if (l.getToPoint().y > 0)
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
			if (l.getToPoint().y > 0)
			{
				degVal = PI/2;
			}
			else
			{
				degVal = 3*PI/2;
			}
		}

		if(degVal >= 2*PI)
			degVal -= 2*PI;

		return degVal;
	}

	double MathExt::dirToPointDeg(double x, double y, double x2, double y2)
	{
		double radVal = dirToPoint(x, y, x2, y2);

		return toDeg(radVal);
	}

	bool MathExt::angleBetween(double testAngle, double startAngle, double endAngle, bool clockwise)
	{
		double tA = MathExt::angleToStandardRange(testAngle, false);
		double sA = MathExt::angleToStandardRange(startAngle, false);
		double eA = MathExt::angleToStandardRange(endAngle, false);

		bool returnVal = true;
		double minAngle = MathExt::min(sA, eA);
		double maxAngle = MathExt::max(sA, eA);

		if(sA > eA)
			returnVal = false;

		if(tA >= minAngle && tA <= maxAngle)
		{
			return (!clockwise) ? returnVal : !returnVal;
		}
		else
		{
			return (!clockwise) ? !returnVal : returnVal;
		}
	}

	bool MathExt::angleBetweenDeg(double testAngle, double startAngle, double endAngle, bool clockwise)
	{
		double tA = MathExt::angleToStandardRange(testAngle, true);
		double sA = MathExt::angleToStandardRange(startAngle, true);
		double eA = MathExt::angleToStandardRange(endAngle, true);

		bool returnVal = true;
		double minAngle = MathExt::min(sA, eA);
		double maxAngle = MathExt::max(sA, eA);

		if(sA > eA)
			returnVal = false;

		if(tA >= minAngle && tA <= maxAngle)
		{
			return (!clockwise) ? returnVal : !returnVal;
		}
		else
		{
			return (!clockwise) ? !returnVal : returnVal;
		}
	}

	double MathExt::closestAngle(double testAngle, double angle1, double angle2)
	{
		double tA = MathExt::angleToStandardRange(testAngle, false);
		double sA = MathExt::angleToStandardRange(angle1, false);
		double eA = MathExt::angleToStandardRange(angle2, false);

		double dis1 = MathExt::abs(sA-tA);
		double dis2 = 2*PI - dis1;

		double dis3 = MathExt::abs(eA-tA);
		double dis4 = 2*PI - dis3;
		
		double minDis1 = MathExt::min( {dis1, dis2} );
		double minDis2 = MathExt::min( {dis3, dis4} );

		if(minDis1 < minDis2)
			return sA;
		else
			return eA;
	}

	double MathExt::closestAngleDeg(double testAngle, double angle1, double angle2)
	{
		double tA = MathExt::angleToStandardRange(testAngle, true);
		double sA = MathExt::angleToStandardRange(angle1, true);
		double eA = MathExt::angleToStandardRange(angle2, true);

		double dis1 = MathExt::abs(sA-tA);
		double dis2 = 360 - dis1;

		double dis3 = MathExt::abs(eA-tA);
		double dis4 = 360 - dis3;
		
		double minDis1 = MathExt::min( {dis1, dis2} );
		double minDis2 = MathExt::min( {dis3, dis4} );

		if(minDis1 < minDis2)
			return sA;
		else
			return eA;
	}

	double MathExt::angleClamp(double testAngle, double startAngle, double endAngle, bool clockwise)
	{
		double tA = MathExt::angleToStandardRange(testAngle, false);
		double sA = MathExt::angleToStandardRange(startAngle, false);
		double eA = MathExt::angleToStandardRange(endAngle, false);

		if(angleBetween(tA, sA, eA, clockwise))
			return tA;
		else
			return closestAngle(tA, sA, eA);
	}

	double MathExt::angleClampDeg(double testAngle, double startAngle, double endAngle, bool clockwise)
	{
		double tA = MathExt::angleToStandardRange(testAngle, true);
		double sA = MathExt::angleToStandardRange(startAngle, true);
		double eA = MathExt::angleToStandardRange(endAngle, true);

		if(angleBetweenDeg(tA, sA, eA, clockwise))
			return tA;
		else
			return closestAngleDeg(tA, sA, eA);
	}
	
	Vec2f MathExt::angleRange(double angle1, double angle2, bool smallest)
	{
		//first, convert to standard range
		double v1 = MathExt::angleToStandardRange(angle1, false);
		double v2 = MathExt::angleToStandardRange(angle2, false);

		if(v2 < v1)
		{
			double temp = v2;
			v2 = v1;
			v1 = temp;
		}
		
		if(smallest)
		{
			if(v2-v1 <= PI)
				return Vec2f(v1, v2);
			
			//adjust stuff
			return Vec2f(v2, v1+(2*PI));
		}
		else
		{
			if(v2-v1 >= PI)
				return Vec2f(v1, v2);
			
			//adjust stuff
			return Vec2f(v2, v1+(2*PI));
		}
	}

	Vec2f MathExt::angleRangeDeg(double angle1, double angle2, bool smallest)
	{
		//first, convert to standard range
		double v1 = MathExt::angleToStandardRange(angle1, true);
		double v2 = MathExt::angleToStandardRange(angle2, true);

		if(v2 < v1)
		{
			double temp = v2;
			v2 = v1;
			v1 = temp;
		}
		
		if(smallest)
		{
			if(v2-v1 <= 180)
				return Vec2f(v1, v2);
			
			//adjust stuff
			return Vec2f(v2, v1+(360));
		}
		else
		{
			if(v2-v1 >= 180)
				return Vec2f(v1, v2);
			
			//adjust stuff
			return Vec2f(v2, v1+(360));
		}
	}

	Vec2f MathExt::lengthDir(double length, double direction, bool counterClockwise)
	{
		if(counterClockwise)
		{
			return Vec2f( MathExt::cos( direction) * length, 
					-MathExt::sin( direction) * length );
		}
		else 
		{
			return Vec2f( MathExt::cos( direction) * length, 
					MathExt::sin( direction) * length );
		}
	}

	Vec3f MathExt::sphericalCoord(double length, double theta, double phi)
	{
		Vec3f result;
		result.x = length*cos(theta)*cos(phi);
		result.y = -length*sin(theta)*cos(phi);
		result.z = length*sin(phi);
		
		return result;
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

	double MathExt::scalarVectorProjection(Vec2f a, Vec2f b)
	{
		return MathExt::dot(a, MathExt::normalize(b));
	}
	double MathExt::scalarVectorProjection(Vec3f a, Vec3f b)
	{
		return MathExt::dot(a, MathExt::normalize(b));
	}
	double MathExt::scalarVectorProjection(Vec4f a, Vec4f b)
	{
		return MathExt::dot(a, MathExt::normalize(b));
	}
	double MathExt::scalarVectorProjection(GeneralVector a, GeneralVector b)
	{
		return MathExt::dot(a, MathExt::normalize(b));
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
		double len = MathExt::sqrt( MathExt::sqr(q1.x) + MathExt::sqr(q1.y) + MathExt::sqr(q1.z) + MathExt::sqr(q1.w));
		if(len!=0)
			return q1/len;
		else
			return Quaternion();
	}

	Vec3f MathExt::crossProduct(Vec3f v1, Vec3f v2)
	{
		Vec3f cross = Vec3f();
		cross.x = v1.y*v2.z - v1.z*v2.y;
		cross.y = v1.z*v2.x - v1.x*v2.z;
		cross.z = v1.x*v2.y - v1.y*v2.x;
		
		return cross;
	}

	Vec2f MathExt::reflect(Vec2f v1, Vec2f v2)
	{
		Vec2f n = MathExt::normalize(v2);
		return v1 - (n * (MathExt::dot(v1,n) * 2));
	}

	Vec3f MathExt::reflect(Vec3f v1, Vec3f v2)
	{
		Vec3f n = MathExt::normalize(v2);
		return v1 - (n * (MathExt::dot(v1,n) * 2));
	}

	Vec4f MathExt::reflect(Vec4f v1, Vec4f v2)
	{
		Vec4f n = MathExt::normalize(v2);
		return v1 - (n * (MathExt::dot(v1,n) * 2));
	}

	GeneralVector MathExt::reflect(GeneralVector v1, GeneralVector v2)
	{
		GeneralVector n = MathExt::normalize(v2);
		return v1 - (n * (MathExt::dot(v1,n) * 2));
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

	double MathExt::distanceTo(PolarCoordinate p1, PolarCoordinate p2)
	{
		//law of cosines
		double a = MathExt::sqr(p1.getLength());
		double b = MathExt::sqr(p2.getLength());
		double c = p1.getLength()*p2.getLength()*MathExt::cos(p1.getAngle() + p2.getAngle());

		return MathExt::sqrt(a + b + c);
	}

	#pragma region Matrix_Operations

	Matrix MathExt::hadamardProduct(Matrix& a, Matrix& b)
	{
		return a.hadamardProduct(b);
	}

	Matrix MathExt::getInverse(Matrix& m)
	{
		return m.getInverse();
	}

	Matrix MathExt::getTranspose(Matrix& m)
	{
		return m.getTranspose();
	}

	double MathExt::getDeterminate(Matrix& m)
	{
		return m.getDeterminate();
	}

	Matrix MathExt::getMatrixOfMinors(Matrix& m, int row, int col)
	{
		return m.getMatrixOfMinors(row, col);
	}

	#pragma endregion

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
		// return Mat3f(
		// 	MathExt::cos(rotation), -MathExt::sin(rotation), -pos.x*MathExt::cos(rotation) - pos.y*MathExt::sin(rotation) + pos.x,
		// 	MathExt::sin(rotation), MathExt::cos(rotation), pos.x*MathExt::sin(rotation) - pos.y*MathExt::cos(rotation) + pos.y,
		// 	0, 0, 1
		// );
		return Mat3f(
			MathExt::cos(rotation), -MathExt::sin(rotation), -pos.x*MathExt::cos(rotation) + pos.y*MathExt::sin(rotation) + pos.x,
			MathExt::sin(rotation), MathExt::cos(rotation), -pos.x*MathExt::sin(rotation) - pos.y*MathExt::cos(rotation) + pos.y,
			0, 0, 1
		);
	}
		
	Mat3f MathExt::translation2D(double x, double y)
	{
		return MathExt::translation2D(Vec2f(x,y));
	}

	Mat3f MathExt::translation2D(Vec2f trans)
	{
		return Mat3f(
			1, 0, -trans.x,
			0, 1, -trans.y,
			0, 0, 1
		);
	}

	Mat3f MathExt::scale2D(double x, double y)
	{
		return MathExt::scale2D(Vec2f(x,y));
	}

	Mat3f MathExt::scale2D(Vec2f scale)
	{
		return Mat3f(
			scale.x, 0, 0,
			0, scale.y, 0,
			0, 0, 1
		);
	}

	Mat3f MathExt::skew2D(double x, double y)
	{
		return MathExt::skew2D(Vec2f(x,y));
	}

	Mat3f MathExt::skew2D(Vec2f skew)
	{
		return Mat3f(
			1, skew.x, 0,
			skew.y, 1, 0,
			0, 0, 1
		);
	}

	
	Mat3f MathExt::modelMatrix2D(Vec2f position, double rotation, Vec2f scale)
	{
		return MathExt::scale2D(scale) * MathExt::rotation2D(rotation) * MathExt::translation2D(position);
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
		
		return Quaternion(axisVals.x, axisVals.y, axisVals.z, cosHalfAngle);
	}

	Mat4f MathExt::quaternionToMatrix(Quaternion q)
	{
		Quaternion nQ = q.normalize();
		return Mat4f(
			2*(sqr(nQ.w) + sqr(nQ.x) ) - 1, 2*(nQ.x*nQ.y - nQ.w*nQ.z), 2*(nQ.x*nQ.z + nQ.w*nQ.y), 0,
			2*(nQ.x*nQ.y + nQ.w*nQ.z), 2*(sqr(nQ.w) + sqr(nQ.y)) - 1, 2*(nQ.y*nQ.z - nQ.w*nQ.x), 0,
			2*(nQ.x*nQ.z - nQ.w*nQ.y), 2*(nQ.y*nQ.z + nQ.w*nQ.x), 2*(sqr(nQ.w) + sqr(nQ.z)) - 1, 0,
			0, 0, 0, 1
		);
	}

	Mat4f MathExt::rotation3DX(double rotation)
	{
		return Mat4f(
			1, 0, 0, 0,
			0, MathExt::cos(rotation), -MathExt::sin(rotation), 0,
			0, MathExt::sin(rotation), MathExt::cos(rotation), 0,
			0, 0, 0, 1
		);
	}

	Mat4f MathExt::rotation3DY(double rotation)
	{	
		return Mat4f(
			MathExt::cos(rotation), 0, MathExt::sin(rotation), 0,
			0, 1, 0, 0,
			-MathExt::sin(rotation), 0, MathExt::cos(rotation), 0,
			0, 0, 0, 1
		);
	}

	Mat4f MathExt::rotation3DZ(double rotation)
	{
		return Mat4f(
			MathExt::cos(rotation), -MathExt::sin(rotation), 0, 0,
			MathExt::sin(rotation), MathExt::cos(rotation), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		);
	}

	Mat4f MathExt::translation3D(double x, double y, double z)
	{
		return MathExt::translation3D(Vec3f(x,y,z));
	}

	Mat4f MathExt::translation3D(Vec3f trans)
	{	
		return Mat4f(
			1, 0, 0, trans.x,
			0, 1, 0, trans.y,
			0, 0, 1, trans.z,
			0, 0, 0, 1
		);
	}

	Mat4f MathExt::scale3D(double x, double y, double z)
	{
		return MathExt::scale3D(Vec3f(x,y,z));
	}

	Mat4f MathExt::scale3D(Vec3f scale)
	{	
		return Mat4f(
			scale.x, 0, 0, 0,
			0, scale.y, 0, 0,
			0, 0, scale.z, 0,
			0, 0, 0, 1
		);
	}

	Mat4f MathExt::skew3D(double x, double y, double z)
	{
		return MathExt::skew3D(Vec3f(x,y,z));
	}

	Mat4f MathExt::skew3D(Vec3f skew)
	{
		return Mat4f(
			1, -skew.z, skew.y, 0,
			skew.z, 1, -skew.x, 0,
			-skew.y, skew.x, 1, 0,
			0, 0, 0, 1
		);
	}

	Mat4f MathExt::modelMatrix3D(Vec3f position, Vec3f rotation, Vec3f scale)
	{

		Quaternion xRot = MathExt::getRotationQuaternion(rotation.x, Vec3f(1,0,0));
		Quaternion yRot = MathExt::getRotationQuaternion(rotation.y, Vec3f(0,1,0));
		Quaternion zRot = MathExt::getRotationQuaternion(rotation.z, Vec3f(0,0,1));

		Quaternion composedRotation = xRot*yRot*zRot;

		return MathExt::scale3D(scale) * MathExt::quaternionToMatrix(composedRotation) * MathExt::translation3D(position);
	}

	Mat4f MathExt::viewMatrix(Vec3f position, Vec3f rotation, bool counterClockwise)
	{
		Vec3f r1 = Vec3f(1,0,0);
		Vec3f r2 = Vec3f(0,1,0);
		Vec3f r3 = Vec3f(0,0,1);
		
		Quaternion zRot = MathExt::getRotationQuaternion((counterClockwise)?rotation.z : -rotation.z, r3);
		Quaternion yRot = MathExt::getRotationQuaternion(-rotation.y, r2);
		Quaternion xRot = MathExt::getRotationQuaternion(-rotation.x, r1);
		
		Mat4f viewMat = Mat4f(
			0, 1, 0, 0,
			0, 0, 1, 0,
			-1, 0, 0, 0,
			0, 0, 0, 1
		);

		Quaternion finalRot = xRot*yRot*zRot;
        Mat4f quatMatrix = MathExt::quaternionToMatrix(finalRot);
		
		Mat4f cameraRotation = Mat4f(
			r1.x, r1.y, r1.z, 0,
			r2.x, r2.y, r2.z, 0,
			r3.x, r3.y, r3.z, 0,
			0, 0, 0, 1
		);

		Mat4f cameraPos = Mat4f(
			1, 0, 0, -position.x,
			0, 1, 0, -position.y,
			0, 0, 1, -position.z,
			0, 0, 0, 1
		);

		if(counterClockwise)
		{
			viewMat[0][1] = -1;
		}

		return viewMat * quatMatrix * cameraPos;
	}

	Mat4f MathExt::lookAtMatrix(Vec3f eyePos, Vec3f toPos, Vec3f upVec)
	{
		Vec3f forward = (toPos - eyePos).normalize(); 
		Vec3f right = (crossProduct(forward, upVec)).normalize(); //right
		Vec3f up = crossProduct(forward, right); //up

		Mat4f cameraView = Mat4f(
			right.x, right.y, right.z, -dot(right, eyePos),
			up.x, up.y, up.z, -dot(up, eyePos),
			forward.x, forward.y, forward.z, -dot(forward, eyePos),
			0, 0, 0, 1
		);

		return cameraView;
	}

	Mat4f MathExt::viewMatrix2D(Vec2f position, double rotation)
	{
		Vec3f r1 = Vec3f(1,0,0);
		Vec3f r2 = Vec3f(0,-1,0);

		Quaternion zRot = MathExt::getRotationQuaternion(rotation, Vec3f(0,0,1));

		r1 = zRot * r1;
		r2 = zRot * r2;
		
		return Mat4f(
			r1.x, r1.y, r1.z, -position.x,
			r2.x, r2.y, r2.z, -position.y,
			0, 0, 1, 0,
			0, 0, 0, 0
		);
	}

	Mat4f MathExt::perspectiveProjectionMatrix(double width, double height, double near, double far, double fov)
	{
		double tanVal = MathExt::dtan(fov/2);
		double aspectRatio = height/width;
		double S1 = aspectRatio / tanVal;
		double S2 = 1.0 / tanVal;
		double zRange = far-near;

		return Mat4f( 
			S1, 0, 0, 0,
			0, S2, 0, 0,
			0, 0, -(far)/zRange, -(far*near)/zRange,
			0, 0, -1, 0);
	}

	Mat4f MathExt::orthographicProjectionMatrix(double width, double height, double near, double far)
	{
		if(near == far)
		{
			//x` = 2x/width + 0y + 0z + -1w
			//y` = 0x -2y/height + 0z + 1w
			//z` = 0x + 0y + z + 0w
			//w` = 0x + 0y + 0z + w
			return Mat4f(
				2.0/width, 0, 0, -1,
				0, -2.0/height, 0, 1,
				0, 0, 1, 0,
				0, 0, 0, 1
			);
		}
		else
		{
			double zRange = far-near;
			return Mat4f(
				2.0/width, 0, 0, -1,
				0, -2.0/height, 0, -1,
				0, 0, 2.0/zRange, -(far+near)/zRange,
				0, 0, 0, 1
			);
		}
	}

	Mat4f MathExt::orthographicProjectionMatrix2(double left, double right, double top, double bottom, double near, double far)
	{
		if(near == far)
		{
			return Mat4f(
				2.0/(right-left), 0, 0, -(right+left) / (right-left),
				0, 2.0/(top-bottom), 0, -(top+bottom) / (top-bottom),
				0, 0, 1, 0,
				0, 0, 0, 1
			);
		}
		else
		{
			double zRange = far-near;
			return Mat4f(
				2.0/(right-left), 0, 0, -(right+left) / (right-left),
				0, 2.0/(top-bottom), 0, (top+bottom) / (top-bottom),
				0, 0, 2.0/zRange, -(far+near)/zRange,
				0, 0, 0, 1
			);
		}
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

		if(A == 0)
		{
			//FIX LATER
		}
		
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

		if(A == 0)
			return solveLinear(B, C);

		if( MathExt::sqr(B) - 4*A*C >= 0)
		{
			double sqrtVal = MathExt::sqrt( MathExt::sqr(B) - 4*A*C );
			double divVal = 2*A;

			k1 = (-B + sqrtVal) / divVal;
			k2 = (-B - sqrtVal) / divVal;
			
			return {k1, k2};
		}

		return {};
	}

	std::vector<double> MathExt::solveCubicReal(double A, double B, double C, double D)
	{
		if(A!=0)
		{
			if(D==0)
			{
				if(C==0)
				{
					if(B==0)
					{
						return {0,0,0};
					}
					else
					{
						//factor into linear
						std::vector<double> solutions = solveLinear(A, B);
						solutions.push_back(0); //Also a solution
						solutions.push_back(0); //Also a solution
						return solutions;
					}
				}
				else
				{
					//factor into quadratic
					std::vector<double> solutions = solveQuadraticReal(A, B, C);
					solutions.push_back(0); //Also a solution
					return solutions;
				}
			}
			
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
				double temp = (-q - MathExt::sqrt(q2 + (4.0*p3)/27.0))/2.0;
				double w = MathExt::cubeRoot(temp);
				double solution = w - p/(3.0*w);
				double actualSolution = solution - B/(3.0*A);
				
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
			for(size_t i=1; i<constants.size()-1; i++)
			{
				newConstants[i] = constants[i] + newConstants[i-1] * zero;
			}
		}
		return newConstants;
	}

	PolynomialMathFunction MathExt::reducePolynomial(PolynomialMathFunction f, double zero)
	{
		//using synthetic division
		std::vector<double> newConstants = std::vector<double>(f.size()-1);
		if(f.size()>0)
		{
			newConstants[0] = f.getConstant(0);
			for(size_t i=1; i<f.size()-1; i++)
			{
				newConstants[i] = f.getConstant(i) + newConstants[i-1] * zero;
			}
		}

		return PolynomialMathFunction(newConstants);
	}

	double MathExt::bisectionMethod(MathFunction* f, double a, double b, int maxIterations)
	{
		double minX = a;
		double maxX = b;
		double x = NAN;

		double resMin = f->solve(a);
		double resMax = f->solve(b);

		//check if a or b
		if(resMin == 0.0)
			return a;
		if(resMax == 0.0)
			return b;

		if(resMin > resMax)
		{
			double temp = maxX;
			maxX = minX;
			minX = temp;
		}

		if(resMin*resMax >= 0)
		{
			//Not possible with Bisection Method
			return NAN;
		}
		
		for(int i=0; i<maxIterations; i++)
		{
			x = (minX+maxX) / 2.0;
			double nResult = f->solve(x);

			if(nResult == 0.0)
			{
				break;
			}
			else if(nResult > 0)
			{
				maxX = x;
			}
			else
			{
				minX = x;
			}
		}

		return x;
	}

	double MathExt::newtonsMethod(MathFunction* f, MathFunction* derivative, double startPoint, int maxIterations)
	{
		double xn = startPoint;
		for(int i=0; i<maxIterations; i++)
		{
			double num = f->solve(xn);
			double div = derivative->solve(xn);

			if(num == 0)
			{
				break;
			}
			
			if(div != 0)
			{
				xn = xn - (num/div);
			}
			else
			{
				//Can not continue
				return NAN;
			}
		}

		return xn;
	}

	double MathExt::secantMethod(MathFunction* f, double a, double b, int maxIterations)
	{
		double xn[3] = {NAN, b, a};
		double sol[3] = {NAN, f->solve(b), f->solve(a)};

		if(sol[1] == 0)
			return b;
		if(sol[2] == 0)
			return a;
		
		for(int i=0; i<maxIterations; i++)
		{
			double num = sol[1]*(xn[1] - xn[2]);
			double div = sol[1] - sol[2];
			if(div == 0)
			{
				//error occured
				xn[0] = NAN;
				break;
			}

			xn[0] = xn[1] - num/div;

			sol[0] = f->solve(xn[0]);
			if(sol[0] == 0)
			{
				break;
			}

			//move xn to xn-1 and move xn-1 to xn-2
			xn[2] = xn[1];
			xn[1] = xn[0];

			sol[2] = sol[1];
			sol[1] = sol[0];
		}

		return xn[0];
	}

	PolynomialMathFunction MathExt::linearRegression(std::vector<Vec2f> points, int degree)
	{
		if(degree < 0)
			return PolynomialMathFunction();
		
		if(points.size() <= 0)
			return PolynomialMathFunction();
		
		Matrix X = Matrix(points.size(), degree+1);
		Matrix Y = Matrix(points.size(), 1);

		Matrix xTranspose = Matrix(degree+1, points.size());

		for(size_t i=0; i<points.size(); i++)
		{
			for(int j=0; j<degree+1; j++)
			{
				X[i][j] = MathExt::pow(points[i].x, j);
				xTranspose[j][i] = MathExt::pow(points[i].x, j);
			}

			Y[i][0] = points[i].y;
		}
		
		Matrix constants = (xTranspose*X).getInverse() * xTranspose * Y;
		PolynomialMathFunction f = PolynomialMathFunction();

		for(int i=0; i<constants.getRows(); i++)
		{
			f.addConstant(constants[i][0]);
		}

		return f;
	}

	PolynomialMathFunction MathExt::fitPolynomial(std::vector<Vec2f> points)
	{
		if(points.size() <= 0)
			return PolynomialMathFunction();

		Matrix A = Matrix(points.size(), points.size());
		Matrix Y = Matrix(points.size(), 1);

		for(size_t i=0; i<points.size(); i++)
		{
			for(size_t j=0; j<points.size(); j++)
			{
				A[i][j] = MathExt::pow(points[i].x, j);
			}
			Y[i][0] = points[i].y;
		}

		Matrix X = A.getInverse() * Y;
		if(!X.getValid())
			return PolynomialMathFunction();
		
		PolynomialMathFunction f = PolynomialMathFunction();
		for(int i=0; i<X.getRows(); i++)
		{
			f.addConstant(X[i][0]);
		}
		return f;
	}

	Vec3f MathExt::fitCircleToTriangle(Vec2f p1, Vec2f p2, Vec2f p3, bool inside)
	{
		if(inside)
		{
			//find mid point in triangle
			Vec2f midPoint = (p1+p2+p3)/3;

			//find radius. Using heron's formula or at least an adapted version for this.
			double a, b, c, s, radius;
			a = (p2-p1).getLength();
			b = (p3-p2).getLength();
			c = (p1-p3).getLength();
			s = (a+b+c)/2;

			if(s != 0)
				radius = MathExt::sqrt( ((s-a)*(s-b)*(s-c)) / s );
			else
				radius = 0;

			return Vec3f(midPoint, radius);
		}
		else
		{
			//get perpendicular bisector of lines p1->p2 and p2->p3
			//already goes through mid point
			Line l1 = Line(p1, p2).getPerpendicularBisector();
			Line l2 = Line(p2, p3).getPerpendicularBisector();

			//find intersection point between the lines
			Vec2f circleCenter = l1.getIntersection(l2);

			//find radius
			double radius = (circleCenter-p1).getLength();

			return Vec3f(circleCenter, radius);
		}
	}

	std::vector<double> MathExt::getIntersectionQuadratic(double A1, double B1, double C1, double A2, double B2, double C2)
	{
		//A1x^2 + B1x + C1 = A2x^2 + B2x + C2
		//(A1-A2)x^2 + (B1-B2)x + (C1-C2) = 0
		double nA = (A1-A2);
		double nB = (B1-B2);
		double nC = (C1-C2);

		return MathExt::solveQuadraticReal(nA, nB, nC);
	}

	std::vector<double> MathExt::getIntersectionQuadratic(PolynomialMathFunction a, PolynomialMathFunction b)
	{
		if(a.size() == 3 && b.size() == 3)
		{
			return MathExt::getIntersectionQuadratic(a.getConstant(0), a.getConstant(1), a.getConstant(2),
													b.getConstant(0), b.getConstant(1), b.getConstant(2));
		}

		return {};
	}

	
	int MathExt::factorial(int k)
	{
		int sum = 1;
		for(int i=1; i<=k; i++)
		{
			sum *= i;
		}
		return sum;
	}
	
	double MathExt::binomialCoefficient(int n, int k)
	{
		double value = factorial(n) / (factorial(k)*factorial(n-k));
		return value;
	}

	double MathExt::logisticsSigmoid(double x)
	{
		double temp = 1.0+exp(-x);
		return 1.0 / temp;
	}

	#pragma region FOURIER_TRANSFORM_1D

	//Should run faster. It still is faster than the normal naive way, but it is not
	//on par with other modern implementations
	std::vector<ComplexNumber> doFFT(ComplexNumber* arr, int size, bool inverse)
	{
		//split into even and odd indicies.
		//regroup them by adding them with a root of unity multiplied to the odd indicies.
		//root of unity chosen is e^(2*PI*j/N)

		if(size>1)
		{
			int newSize = (size/2);

			ComplexNumber* evens = new ComplexNumber[newSize];
			ComplexNumber* odds = new ComplexNumber[newSize];

			for(int i=0; i<newSize; i++)
			{
				evens[i] = arr[2*i];
				odds[i] = arr[2*i + 1];
			}

			std::vector<ComplexNumber> split1 = doFFT(evens, newSize, inverse);
			std::vector<ComplexNumber> split2 = doFFT(odds, newSize, inverse);
			
			double angle = 0;
			if(!inverse)
				angle = (-2.0*PI)/size;
			else
				angle = (2.0*PI)/size;

			std::vector<ComplexNumber> output = std::vector<ComplexNumber>(size);
			
			for(int i=0; i<newSize; i++)
			{
				ComplexNumber multiplier = ComplexNumber( MathExt::cos(angle*i), MathExt::sin(angle*i) );
				output[i] = split1[i] + (split2[i]*multiplier);
				output[i+newSize] = split1[i] - (split2[i]*multiplier);

				if(inverse)
				{
					output[i] /= 2;
					output[i+newSize] /= 2;
				}
			}

			delete[] evens;
			delete[] odds;

			return output;
		}
		else if(size==1)
		{
			return {arr[0]};
		}
		else
		{
			return {};
		}
	}

	//Not sure how this works just yet. Will keep old code for the recursive call. I worked hard on understanding that.
	void doFFTInline(ComplexNumber* output, size_t size, bool inverse)
	{
		//Reference: (https://cp-algorithms.com/algebra/fft.html#improved-implementation-in-place-computation)
		for(int i=1, j=0; i<size; i++)
		{
			int bit = size >> 1;
			for(; j & bit; bit >>=1)
				j ^= bit;
			j ^= bit;

			if(i<j)
				std::swap(output[i], output[j]);
		}

		double angle = (inverse) ? -2*PI : 2*PI;
		for(int l = 2; l<=size; l<<=1)
		{
			double ang = angle/l;
			ComplexNumber wLen = ComplexNumber(MathExt::cos(ang), MathExt::sin(ang));
			for(int i=0; i<size; i+=l)
			{
				ComplexNumber w = ComplexNumber(1, 0);
				for(int j=0; j<l/2; j++)
				{
					ComplexNumber u = output[i+j];
					ComplexNumber v = output[i+j+l/2] * w;
					output[i+j] = u+v;
					output[i+j+l/2] = u-v;
					w *= wLen;
				}
			}
		}

		if(inverse)
		{
			for(int i=0; i<size; i++)
			{
				output[i] /= size;
			}
		}
	}

	ComplexNumber MathExt::discreteFourierTransform(ComplexNumber* arr, int size, double x, bool inverse)
	{
		//Xk = sumFrom 0 to N-1 of ( xn * e ^ -(i2pi*kn)/N )
		//or
		//xn * cos(2pi*kn/N) - i*sin(2pi*kn/N)
		//We can separate the real and imaginary parts and only do the real part

		ComplexNumber finalAnswer = ComplexNumber();
		double xFactor = 0;

		if(!inverse)
			xFactor = (-2.0*PI*x)/size;
		else
			xFactor = (2.0*PI*x)/size;

		for (int n = 0; n < size; n++)
		{
			ComplexNumber c = ComplexNumber( MathExt::cos(xFactor*n), MathExt::sin(xFactor*n));
			finalAnswer += arr[n] * c;
		}

		if(!inverse)
			return finalAnswer;
		else
			return finalAnswer/size;
	}

	std::vector<ComplexNumber> MathExt::fourierTransform(ComplexNumber* arr, int size, bool inverse)
	{
		std::vector<ComplexNumber> output = std::vector<ComplexNumber>(size);

		for(int i=0; i<size; i++)
		{
			ComplexNumber frequency = MathExt::discreteFourierTransform(arr, size, (double)i, inverse);
			output[i] = frequency;
		}

		return output;
	}

	std::vector<ComplexNumber> MathExt::fastFourierTransform(ComplexNumber* arr, size_t size, bool inverse)
	{
		//cooley tukey algorithm
		//must be a power of 2

		std::vector<ComplexNumber> output;
		if(size == 0 || !IS_POWER_2(size))
		{
			//can't do unless it is a power of 2
			return output;
		}
		else
		{
			//copy into output
			memcpy(output.data(), arr, size);
			doFFTInline(output.data(), size, inverse);
		}

		return output;
	}

	bool MathExt::fastFourierTransformInline(ComplexNumber* arr, size_t size, bool inverse)
	{
		//cooley tukey algorithm
		//must be a power of 2
		if(size == 0 || !IS_POWER_2(size))
		{
			//can't do unless it is a power of 2
			return false;
		}
		else
		{
			doFFTInline(arr, size, inverse);
		}

		return true;
	}

	#pragma endregion

	#pragma region COSINE_TRANSFORM_1D

	void doFDCT(double* arr, double* temp, size_t size)
	{
		if(size == 1)
			return;
		size_t halfSize = size/2;
		for(size_t i=0; i<halfSize; i++)
		{
			double x = arr[i];
			double y = arr[size - 1 - i];
			temp[i] = x + y;
			temp[i + halfSize] = (x-y) / (MathExt::cos((i + 0.5) * PI / size) * 2);
		}

		doFDCT(arr, temp, halfSize);
		doFDCT(arr, &temp[halfSize], halfSize);

		for(size_t i=0; i<halfSize-1; i++)
		{
			arr[2*i] = temp[i];
			arr[2*i + 1] = temp[i+halfSize] + temp[i+halfSize+1];
		}
		arr[size-2] = temp[halfSize-1];
		arr[size-1] = temp[size-1];
	}

	void doIDCT(double* arr, double* temp, size_t size)
	{
		if(size == 1)
			return;
		size_t halfSize = size/2;

		temp[0] = arr[0];
		temp[halfSize] = arr[1];
		for(size_t i=1; i<halfSize; i++)
		{
			temp[i] = arr[i*2];
			temp[i + halfSize] = arr[i*2 - 1] + arr[i*2 + 1];
		}

		doIDCT(arr, temp, halfSize);
		doIDCT(arr, &temp[halfSize], halfSize);

		for(size_t i=0; i<halfSize-1; i++)
		{
			double x = temp[i];
			double y = temp[i + halfSize] / (MathExt::cos((i + 0.5) * PI / size) * 2);
			arr[i] = x+y;
			arr[size-1-i] = x-y;
		}
	}

	double MathExt::discreteCosineTransform(double* arr, int size, int u, bool inverse)
	{
		double sum = 0;
		double alpha = 1;

		if(u==0)
			alpha = 1.0/MathExt::sqrt(2.0);

		for(int x=0; x<size; x++)
		{
			double cosCoeff = 0;
			if(inverse)
			{
				if(x==0)
					alpha = 1.0/MathExt::sqrt(2.0);
				else
					alpha = 1.0;

				cosCoeff = (PI*( (2*u) + 1)*x) / (2*size);
			}
			else
			{
				cosCoeff = (PI*( (2*x) + 1)*u) / (2*size);
			}

			sum += alpha * arr[x] * MathExt::cos( cosCoeff );
		}

		return sum * MathExt::sqrt(2.0/size);
	}

	std::vector<double> MathExt::cosineTransform(double* arr, int size, bool inverse)
	{
		std::vector<double> newArr = std::vector<double>(size);

		for(int u=0; u<size; u++)
		{
			newArr[u] = discreteCosineTransform(arr, size, u, inverse);
		}

		return newArr;
	}

	//should be a fast version of the normal discreteCosineTransform
	std::vector<double> MathExt::fastCosineTransform(double* arr, size_t size, bool inverse)
	{
		std::vector<double> output;
		std::vector<double> temp;
		if(size == 0 || !IS_POWER_2(size))
		{
			//can't do unless it is a power of 2
			return output;
		}
		else
		{
			//copy arr into output
			output = std::vector<double>(size);
			temp = std::vector<double>(size);
			memcpy(output.data(), arr, size*sizeof(double));
			if(!inverse)
				doFDCT(output.data(), temp.data(), size);
			else
				doIDCT(output.data(), temp.data(), size);
		}

		return output;
	}

	#pragma endregion

	#pragma region SINE_TRANSFORM_1D

	double MathExt::discreteSineTransform(double* arr, size_t size, size_t u)
	{
		double sum = 0;
		double ang = PI*(u+1);
		for(int x=0; x<size; x++)
		{
			sum += arr[x]*MathExt::sin(ang*(x+1) / (size+1));
		}
		sum *= MathExt::sqrt(2.0 / (size+1));
		return sum;
	}

	std::vector<double> MathExt::sineTransform(double* arr, size_t size)
	{
		std::vector<double> newArr = std::vector<double>(size);
		for(int u=0; u<size; u++)
		{
			newArr[u] = discreteSineTransform(arr, size, u);
		}

		return newArr;
	}

	//should be a fast version of the normal discreteSineTransform
	// std::vector<double> MathExt::fastSineTransform(double* arr, size_t size, bool inverse)
	// {
	// 	// std::vector<double> output;
	// 	// std::vector<double> temp;
	// 	// if(size == 0 || !IS_POWER_2(size))
	// 	// {
	// 	// 	//can't do unless it is a power of 2
	// 	// 	return output;
	// 	// }
	// 	// else
	// 	// {
	// 	// 	//copy arr into output
	// 	// 	output = std::vector<double>(size);
	// 	// 	temp = std::vector<double>(size);
	// 	// 	memcpy(output.data(), arr, size*sizeof(double));
	// 	// 	if(!inverse)
	// 	// 		doFDST(output.data(), temp.data(), size);
	// 	// 	else
	// 	// 		doIDST(output.data(), temp.data(), size);
	// 	// }

	// 	// return output;
	// }

	// void doFDST(double* arr, double* temp, size_t size)
	// {
	// 	// if(size == 1)
	// 	// 	return;
	// 	// size_t halfSize = size/2;
	// 	// for(size_t i=0; i<halfSize; i++)
	// 	// {
	// 	// 	double x = arr[i];
	// 	// 	double y = arr[size - 1 - i];
	// 	// 	temp[i] = x + y;
	// 	// 	temp[i + halfSize] = (x-y) / (MathExt::cos((i + 0.5) * PI / size) * 2);
	// 	// }

	// 	// doFDST(arr, temp, halfSize);
	// 	// doFDST(arr, &temp[halfSize], halfSize);

	// 	// for(size_t i=0; i<halfSize-1; i++)
	// 	// {
	// 	// 	arr[2*i] = temp[i];
	// 	// 	arr[2*i + 1] = temp[i+halfSize] + temp[i+halfSize+1];
	// 	// }
	// 	// arr[size-2] = temp[halfSize-1];
	// 	// arr[size-1] = temp[size-1];
	// }

	// void doIDST(double* arr, double* temp, size_t size)
	// {
	// 	// if(size == 1)
	// 	// 	return;
	// 	// size_t halfSize = size/2;

	// 	// temp[0] = arr[0];
	// 	// temp[halfSize] = arr[1];
	// 	// for(size_t i=1; i<halfSize; i++)
	// 	// {
	// 	// 	temp[i] = arr[i*2];
	// 	// 	temp[i + halfSize] = arr[i*2 - 1] + arr[i*2 + 1];
	// 	// }

	// 	// doIDCT(arr, temp, halfSize);
	// 	// doIDCT(arr, &temp[halfSize], halfSize);

	// 	// for(size_t i=0; i<halfSize-1; i++)
	// 	// {
	// 	// 	double x = temp[i];
	// 	// 	double y = temp[i + halfSize] / (MathExt::cos((i + 0.5) * PI / size) * 2);
	// 	// 	arr[i] = x+y;
	// 	// 	arr[size-1-i] = x-y;
	// 	// }
	// }

	#pragma endregion

	#pragma region COSINE_TRANSFORM_2D

	Matrix MathExt::cosineTransform2D(Matrix& arr, bool inverse)
	{
		Matrix finalArr = Matrix(arr.getRows(), arr.getCols());

		//for each row
		for(int v=0; v<arr.getRows(); v++)
		{
			double* passArr = arr[v];
			std::vector<double> newArr = MathExt::cosineTransform(passArr, arr.getCols(), inverse);

			for(int i=0; i<arr.getCols(); i++)
			{
				finalArr[v][i] = newArr[i];
			}
		}

		//for each column
		for(int u=0; u<arr.getCols(); u++)
		{
			std::vector<double> passArr = std::vector<double>(arr.getRows());

			for(int i=0; i<arr.getRows(); i++)
			{
				passArr[i] = finalArr[i][u];
			}

			std::vector<double> newArr = MathExt::cosineTransform(passArr.data(), arr.getCols(), inverse);

			for(int i=0; i<arr.getRows(); i++)
			{
				finalArr[i][u] = newArr[i];
			}
		}

		return finalArr;
	}

	Matrix MathExt::fastCosineTransform2D(Matrix& arr, bool inverse)
	{
		if(!arr.getValid())
			return Matrix();
		if(arr.getRows() != arr.getCols())
			return Matrix();
		if(!IS_POWER_2(arr.getRows()))
			return Matrix();
		
		//copy arr into finalArr
		Matrix finalArr = Matrix(arr);
		std::vector<double> temp = std::vector<double>(finalArr.getRows());
		std::vector<double> columnArr = std::vector<double>(arr.getRows());

		if(!inverse)
		{
			//for each row
			for(int v=0; v<finalArr.getRows(); v++)
			{
				double* passArr = finalArr[v];
				doFDCT(passArr, temp.data(), temp.size());
			}

			//for each column
			for(int u=0; u<finalArr.getCols(); u++)
			{
				for(int i=0; i<finalArr.getRows(); i++)
				{
					columnArr[i] = finalArr[i][u];
				}

				doFDCT(columnArr.data(), temp.data(), temp.size());
				//copy into final arr column
				for(int i=0; i<finalArr.getRows(); i++)
				{
					finalArr[i][u] = columnArr[i];
				}
			}
		}
		else
		{
			//for each row
			for(int v=0; v<finalArr.getRows(); v++)
			{
				double* passArr = finalArr[v];
				doIDCT(passArr, temp.data(), temp.size());
			}

			//for each column
			for(int u=0; u<finalArr.getCols(); u++)
			{
				for(int i=0; i<finalArr.getRows(); i++)
				{
					columnArr[i] = finalArr[i][u];
				}

				doIDCT(columnArr.data(), temp.data(), temp.size());
				//copy into final arr column
				for(int i=0; i<finalArr.getRows(); i++)
				{
					finalArr[i][u] = columnArr[i];
				}
			}
		}

		return finalArr;
	}

	#pragma endregion

	
	#pragma region SINE_TRANSFORM_2D
	
	Matrix MathExt::sineTransform2D(Matrix& arr)
	{
		Matrix finalArr = Matrix(arr.getRows(), arr.getCols());

		//for each row
		for(int v=0; v<arr.getRows(); v++)
		{
			double* passArr = arr[v];
			std::vector<double> newArr = MathExt::sineTransform(passArr, arr.getCols());

			for(int i=0; i<arr.getCols(); i++)
			{
				finalArr[v][i] = newArr[i];
			}
		}

		//for each column
		for(int u=0; u<arr.getCols(); u++)
		{
			std::vector<double> passArr = std::vector<double>(arr.getRows());

			for(int i=0; i<arr.getRows(); i++)
			{
				passArr[i] = finalArr[i][u];
			}

			std::vector<double> newArr = MathExt::sineTransform(passArr.data(), arr.getCols());

			for(int i=0; i<arr.getRows(); i++)
			{
				finalArr[i][u] = newArr[i];
			}
		}

		return finalArr;
	}

	// Matrix MathExt::fastCosineTransform2D(Matrix& arr, bool inverse)
	// {
	// 	if(!arr.getValid())
	// 		return Matrix();
	// 	if(arr.getRows() != arr.getCols())
	// 		return Matrix();
	// 	if(!IS_POWER_2(arr.getRows()))
	// 		return Matrix();
		
	// 	//copy arr into finalArr
	// 	Matrix finalArr = Matrix(arr);
	// 	std::vector<double> temp = std::vector<double>(finalArr.getRows());
	// 	std::vector<double> columnArr = std::vector<double>(arr.getRows());

	// 	if(!inverse)
	// 	{
	// 		//for each row
	// 		for(int v=0; v<finalArr.getRows(); v++)
	// 		{
	// 			double* passArr = finalArr[v];
	// 			doFDCT(passArr, temp.data(), temp.size());
	// 		}

	// 		//for each column
	// 		for(int u=0; u<finalArr.getCols(); u++)
	// 		{
	// 			for(int i=0; i<finalArr.getRows(); i++)
	// 			{
	// 				columnArr[i] = finalArr[i][u];
	// 			}

	// 			doFDCT(columnArr.data(), temp.data(), temp.size());
	// 			//copy into final arr column
	// 			for(int i=0; i<finalArr.getRows(); i++)
	// 			{
	// 				finalArr[i][u] = columnArr[i];
	// 			}
	// 		}
	// 	}
	// 	else
	// 	{
	// 		//for each row
	// 		for(int v=0; v<finalArr.getRows(); v++)
	// 		{
	// 			double* passArr = finalArr[v];
	// 			doIDCT(passArr, temp.data(), temp.size());
	// 		}

	// 		//for each column
	// 		for(int u=0; u<finalArr.getCols(); u++)
	// 		{
	// 			for(int i=0; i<finalArr.getRows(); i++)
	// 			{
	// 				columnArr[i] = finalArr[i][u];
	// 			}

	// 			doIDCT(columnArr.data(), temp.data(), temp.size());
	// 			//copy into final arr column
	// 			for(int i=0; i<finalArr.getRows(); i++)
	// 			{
	// 				finalArr[i][u] = columnArr[i];
	// 			}
	// 		}
	// 	}

	// 	return finalArr;
	// }

	#pragma endregion

	#pragma region DCT8x8

	//Special Case for JPEG
	const double S[8] = { 1.0/(2.0*MathExt::sqrt(2)),
						1.0/(4.0*MathExt::cos(PI/16)),
						1.0/(4.0*MathExt::cos(2*PI/16)),
						1.0/(4.0*MathExt::cos(3*PI/16)),
						1.0/(4.0*MathExt::cos(4*PI/16)),
						1.0/(4.0*MathExt::cos(5*PI/16)),
						1.0/(4.0*MathExt::cos(6*PI/16)),
						1.0/(4.0*MathExt::cos(7*PI/16))
						};

	const double A[6] = { 0,
						MathExt::cos(4*PI/16),
						MathExt::cos(2*PI/16) - MathExt::cos(6*PI/16),
						MathExt::cos(4*PI/16),
						MathExt::cos(6*PI/16) + MathExt::cos(2*PI/16),
						MathExt::cos(6*PI/16),
						};

	void MathExt::FCT8(double* arr, double* output, bool inverse)
	{
		//assume arr has a size of 8
		double v[29];

		if(!inverse)
		{
			v[0] = arr[0]+arr[7];
			v[1] = arr[1]+arr[6];
			v[2] = arr[2]+arr[5];
			v[3] = arr[3]+arr[4];
			v[4] = arr[3]-arr[4];
			v[5] = arr[2]-arr[5];
			v[6] = arr[1]-arr[6];
			v[7] = arr[0]-arr[7];

			v[8] = v[0] + v[3];
			v[9] = v[1] + v[2];
			v[10] = v[1] - v[2];
			v[11] = v[0] - v[3];
			v[12] = -v[4] - v[5];
			v[13] = (v[5]+v[6]) * A[3];
			v[14] = v[6] + v[7];

			v[15] = v[8] + v[9];
			v[16] = v[8] - v[9];
			v[17] = (v[10] + v[11]) * A[1];
			v[18] = (v[12] + v[14]) * A[5];
			
			v[19] = -v[12]*A[2] - v[18];
			v[20] = v[14]*A[4] - v[18];

			v[21] = v[17] + v[11];
			v[22] = v[11] - v[17];
			v[23] = v[13] + v[7];
			v[24] = v[7] - v[13];

			v[25] = v[19] + v[24];
			v[26] = v[23] + v[20];
			v[27] = v[23] - v[20];
			v[28] = v[24] - v[19];

			output[0] = S[0] * v[15];
			output[1] = S[1] * v[26];
			output[2] = S[2] * v[21];
			output[3] = S[3] * v[28];
			output[4] = S[4] * v[16];
			output[5] = S[5] * v[25];
			output[6] = S[6] * v[22];
			output[7] = S[7] * v[27];
		}
		else
		{
			v[15] = arr[0]/S[0];
			v[26] = arr[1]/S[1];
			v[21] = arr[2]/S[2];
			v[28] = arr[3]/S[3];
			v[16] = arr[4]/S[4];
			v[25] = arr[5]/S[5];
			v[22] = arr[6]/S[6];
			v[27] = arr[7]/S[7];

			v[19] = (v[25]-v[28])/2;
			v[20] = (v[26]-v[27])/2;
			v[23] = (v[26]+v[27])/2;
			v[24] = (v[25]+v[28])/2;

			v[7] = (v[23]+v[24])/2;
			v[11] = (v[21]+v[22])/2;
			v[13] = (v[23]-v[24])/2;
			v[17] = (v[21]-v[22])/2;

			v[8] = (v[15]+v[16])/2;
			v[9] = (v[15]-v[16])/2;

			v[18] = (v[19]-v[20]) * A[5];
			v[12] = (v[19]*A[4] - v[18]) / (A[2]*A[5] - A[2]*A[4] - A[4]*A[5]);
			v[14] = (v[18]-v[20] * A[2]) / (A[2]*A[5] - A[2]*A[4] - A[4]*A[5]);

			v[6] = v[14] - v[7];
			v[5] = v[13] / A[3] - v[6];
			v[4] = -v[5] - v[12];
			v[10] = v[17] / A[1] - v[11];

			v[0] = (v[8]+v[11])/2;
			v[1] = (v[9]+v[10])/2;
			v[2] = (v[9]-v[10])/2;
			v[3] = (v[8]-v[11])/2;

			output[0] = (v[0] + v[7]) / 2;
			output[1] = (v[1] + v[6]) / 2;
			output[2] = (v[2] + v[5]) / 2;
			output[3] = (v[3] + v[4]) / 2;
			output[4] = (v[3] - v[4]) / 2;
			output[5] = (v[2] - v[5]) / 2;
			output[6] = (v[1] - v[6]) / 2;
			output[7] = (v[0] - v[7]) / 2;
		}
	}

	void MathExt::FCT8x8(Matrix& arr, Matrix* output, bool inverse)
	{
		//for each row
		double* newArr = new double[8];
		double* passArr = new double[8];

		for(int v=0; v<arr.getRows(); v++)
		{
			MathExt::FCT8(arr[v], newArr, inverse);

			for(int i=0; i<arr.getCols(); i++)
			{
				output->operator[](v)[i] = newArr[i];
			}
		}

		//for each column
		for(int u=0; u<arr.getCols(); u++)
		{
			for(int i=0; i<arr.getRows(); i++)
			{
				passArr[i] = output->operator[](i)[u];
			}

			MathExt::FCT8(passArr, newArr, inverse);

			for(int i=0; i<arr.getRows(); i++)
			{
				output->operator[](i)[u] = newArr[i];
			}
		}

		delete[] newArr;
		delete[] passArr;
	}

	#pragma endregion

} //NAMESPACE glib END