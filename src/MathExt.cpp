#include "MathExt.h"
#include "System.h"
#include "StringTools.h"
#include "Sort.h"
#include "LCG.h"
#include <float.h>

#undef max
#undef min

namespace smpl
{

	static const unsigned char BitReverseTable256[] = 
	{
	0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0, 
	0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8, 0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8, 
	0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4, 0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4, 
	0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC, 0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC, 
	0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2, 0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2, 
	0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA, 0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
	0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6, 0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6, 
	0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE, 0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,
	0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1, 0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
	0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9, 0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9, 
	0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5, 0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,
	0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED, 0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
	0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3, 
	0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB, 0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
	0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7, 0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7, 
	0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF, 0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF
	};

	int MathExt::popcount(uint8_t x)
	{
		#ifdef _MSC_VER
		return __popcnt16(x);
		#else
		return __builtin_popcount(x);
		#endif
	}

	int MathExt::popcount(uint16_t x)
	{
		#ifdef _MSC_VER
		return __popcnt16(x);
		#else
		return __builtin_popcount(x);
		#endif
	}

	int MathExt::popcount(uint32_t x)
	{
		#ifdef _MSC_VER
		return __popcnt(x);
		#else
		return __builtin_popcount(x);
		#endif
	}

	int MathExt::popcount(uint64_t x)
	{
		#ifdef _MSC_VER
		return __popcnt64(x);
		#else
		return __builtin_popcountll(x);
		#endif
	}


	uint8_t MathExt::bitReversal(uint8_t a)
	{
		return BitReverseTable256[a];
	}

	uint16_t MathExt::bitReversal(uint16_t a)
	{
		uint16_t c =(BitReverseTable256[a & 0xff] << 8) | 
					(BitReverseTable256[(a >> 8) & 0xff]);
		return c;
	}

	uint32_t MathExt::bitReversal(uint32_t a)
	{
		uint32_t c =(BitReverseTable256[a & 0xff] << 24) | 
					(BitReverseTable256[(a >> 8) & 0xff] << 16) | 
					(BitReverseTable256[(a >> 16) & 0xff] << 8) |
					(BitReverseTable256[(a >> 24) & 0xff]);
		return c;
	}
	
	uint64_t MathExt::bitReversal(uint64_t a)
	{
		uint64_t c =((uint64_t)BitReverseTable256[a & 0xff] << 56) | 
					((uint64_t)BitReverseTable256[(a >> 8) & 0xff] << 48) | 
					((uint64_t)BitReverseTable256[(a >> 16) & 0xff] << 40) |
					((uint64_t)BitReverseTable256[(a >> 24) & 0xff] << 32)|
					((uint64_t)BitReverseTable256[(a >> 32) & 0xff] << 24) | 
					((uint64_t)BitReverseTable256[(a >> 40) & 0xff] << 16) | 
					((uint64_t)BitReverseTable256[(a >> 48) & 0xff] << 8) |
					((uint64_t)BitReverseTable256[(a >> 56) & 0xff]);
		return c;
	}

	
	int MathExt::hammingDistance(uint8_t v1, uint8_t v2)
	{
		return MathExt::popcount((uint8_t)(v1 ^ v2));
	}
	int MathExt::hammingDistance(uint16_t v1, uint16_t v2)
	{
		return MathExt::popcount((uint16_t)(v1 ^ v2));
	}
	int MathExt::hammingDistance(uint32_t v1, uint32_t v2)
	{
		return MathExt::popcount((uint32_t)(v1 ^ v2));
	}
	int MathExt::hammingDistance(uint64_t v1, uint64_t v2)
	{
		return MathExt::popcount((uint64_t)(v1 ^ v2));
	}

	
	uint8_t MathExt::saturatedAdd(uint8_t v1, uint8_t v2)
	{
		//https://locklessinc.com/articles/sat_arithmetic/
		uint8_t c = v1+v2;
		c |= -(c < v1);
		return c;
	}
	uint16_t MathExt::saturatedAdd(uint16_t v1, uint16_t v2)
	{
		//https://locklessinc.com/articles/sat_arithmetic/
		uint16_t c = v1+v2;
		c |= -(c < v1);
		return c;
	}
	uint32_t MathExt::saturatedAdd(uint32_t v1, uint32_t v2)
	{
		//https://locklessinc.com/articles/sat_arithmetic/
		uint32_t c = v1+v2;
		c |= -(c < v1);
		return c;
	}
	uint64_t MathExt::saturatedAdd(uint64_t v1, uint64_t v2)
	{
		//https://locklessinc.com/articles/sat_arithmetic/
		uint64_t c = v1+v2;
		c |= -(c < v1);
		return c;
	}
	
	uint8_t MathExt::saturatedSub(uint8_t v1, uint8_t v2)
	{
		//https://locklessinc.com/articles/sat_arithmetic/
		uint8_t c = v1-v2;
		c &= -(c <= v1);
		return c;
	}
	uint16_t MathExt::saturatedSub(uint16_t v1, uint16_t v2)
	{
		//https://locklessinc.com/articles/sat_arithmetic/
		uint16_t c = v1-v2;
		c &= -(c <= v1);
		return c;
	}
	uint32_t MathExt::saturatedSub(uint32_t v1, uint32_t v2)
	{
		//https://locklessinc.com/articles/sat_arithmetic/
		uint32_t c = v1-v2;
		c &= -(c <= v1);
		return c;
	}
	uint64_t MathExt::saturatedSub(uint64_t v1, uint64_t v2)
	{
		//https://locklessinc.com/articles/sat_arithmetic/
		uint64_t c = v1-v2;
		c &= -(c <= v1);
		return c;
	}

	
	int8_t MathExt::saturatedAdd(int8_t v1, int8_t v2)
	{
		//https://locklessinc.com/articles/sat_arithmetic/
		uint8_t a = v1;
		uint8_t b = v2;
		uint8_t res = a + b;

		a = (a>>7) + INT8_MAX; //overflow stuff
		if( (int8_t)((a^b) | ~(b^res)) >= 0) //force cmovns
			res = a;
		return res;
	}
	int16_t MathExt::saturatedAdd(int16_t v1, int16_t v2)
	{
		//https://locklessinc.com/articles/sat_arithmetic/
		uint16_t a = v1;
		uint16_t b = v2;
		uint16_t res = a + b;

		a = (a>>15) + INT16_MAX; //overflow stuff
		if( (int16_t)((a^b) | ~(b^res)) >= 0) //force cmovns
			res = a;
		return res;
	}
	int32_t MathExt::saturatedAdd(int32_t v1, int32_t v2)
	{
		//https://locklessinc.com/articles/sat_arithmetic/
		uint32_t a = v1;
		uint32_t b = v2;
		uint32_t res = a + b;

		a = (a>>31) + INT32_MAX; //overflow stuff
		if( (int32_t)((a^b) | ~(b^res)) >= 0) //force cmovns
			res = a;
		return res;
	}
	int64_t MathExt::saturatedAdd(int64_t v1, int64_t v2)
	{
		//https://locklessinc.com/articles/sat_arithmetic/
		uint64_t a = v1;
		uint64_t b = v2;
		uint64_t res = a + b;

		a = (a>>63) + INT64_MAX; //overflow stuff
		if( (int64_t)((a^b) | ~(b^res)) >= 0) //force cmovns
			res = a;
		return res;
	}

	int8_t MathExt::saturatedSub(int8_t v1, int8_t v2)
	{
		//https://locklessinc.com/articles/sat_arithmetic/
		uint8_t a = v1;
		uint8_t b = v2;
		uint8_t res = a - b;

		a = (a>>7) + INT8_MAX; //overflow stuff
		if( (int8_t)((a^b) & (a^res)) < 0) //force cmovns
			res = a;
		return res;
	}
	int16_t MathExt::saturatedSub(int16_t v1, int16_t v2)
	{
		//https://locklessinc.com/articles/sat_arithmetic/
		uint16_t a = v1;
		uint16_t b = v2;
		uint16_t res = a - b;

		a = (a>>15) + INT16_MAX; //overflow stuff
		if( (int16_t)((a^b) & (a^res)) < 0) //force cmovns
			res = a;
		return res;
	}
	int32_t MathExt::saturatedSub(int32_t v1, int32_t v2)
	{
		//https://locklessinc.com/articles/sat_arithmetic/
		uint32_t a = v1;
		uint32_t b = v2;
		uint32_t res = a - b;

		a = (a>>31) + INT32_MAX; //overflow stuff
		if( (int32_t)((a^b) & (a^res)) < 0) //force cmovns
			res = a;
		return res;
	}
	int64_t MathExt::saturatedSub(int64_t v1, int64_t v2)
	{
		//https://locklessinc.com/articles/sat_arithmetic/
		uint64_t a = v1;
		uint64_t b = v2;
		uint64_t res = a - b;

		a = (a>>63) + INT64_MAX; //overflow stuff
		if( (int64_t)((a^b) & (a^res)) < 0) //force cmovns
			res = a;
		return res;
	}
	
	float MathExt::floor(float a)
	{
		return ::floorf(a);
	}
	double MathExt::floor(double a)
	{
		return std::floor(a);
	}

	float MathExt::ceil(float a)
	{
		return ::ceilf(a);
	}
	double MathExt::ceil(double a)
	{
		return std::ceil(a);
	}

	float MathExt::round(float a)
	{
		return ::roundf(a);
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
		int32_t i;
		float x2, y;
		const float threehalfs = 1.5F;

		x2 = number * 0.5F;
		y  = number;
		i  = * ( int32_t * ) &y;
		i  = 0x5f3759df - ( i >> 1 );
		y  = * ( float * ) &i;
		y  = y * ( threehalfs - ( x2 * y * y ) );
		y  = y * ( threehalfs - ( x2 * y * y ) );

		return y;
	}

	float MathExt::cuberoot(float a)
	{
		return ::cbrtf(a);
	}

	double MathExt::cuberoot(double a)
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

	Quaternion MathExt::normalize(Quaternion q1)
	{
		double len = MathExt::sqrt( MathExt::sqr(q1.x) + MathExt::sqr(q1.y) + MathExt::sqr(q1.z) + MathExt::sqr(q1.w));
		if(len!=0)
			return q1/len;
		else
			return Quaternion();
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

	double MathExt::distanceTo(PolarCoordinate p1, PolarCoordinate p2)
	{
		//law of cosines
		double a = MathExt::sqr(p1.getLength());
		double b = MathExt::sqr(p2.getLength());
		double c = p1.getLength()*p2.getLength()*MathExt::cos(p1.getAngle() + p2.getAngle());

		return MathExt::sqrt(a + b + c);
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
		Vec3f right = forward.crossProduct(upVec).normalize(); //right
		Vec3f up = forward.crossProduct(right); //up

		Mat4f cameraView = Mat4f(
			right.x, right.y, right.z, -right.dot(eyePos),
			up.x, up.y, up.z, -up.dot(eyePos),
			forward.x, forward.y, forward.z, -forward.dot(eyePos),
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
				//doesn't matter in this case whether we use + or -
				double temp = (-q - MathExt::sqrt(q2 + (4.0*p3)/27.0))/2.0;
				double w = MathExt::cuberoot(temp);
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
		
		MatrixD X = MatrixD(points.size(), degree);
		MatrixD Y = MatrixD(points.size(), 1);

		for(size_t i=0; i<points.size(); i++)
		{
			for(int j=0; j<degree; j++)
			{
				X[i][j] = MathExt::pow(points[i].x, j);
			}

			Y[i][0] = points[i].y;
		}

		MatrixD xTranspose = X.getTranspose();
		
		MatrixD constants = (xTranspose*X).getInverse() * xTranspose * Y;
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

		MatrixD A = MatrixD(points.size(), points.size());
		MatrixD Y = MatrixD(points.size(), 1);

		for(size_t i=0; i<points.size(); i++)
		{
			for(size_t j=0; j<points.size(); j++)
			{
				A[i][j] = MathExt::pow(points[i].x, j);
			}
			Y[i][0] = points[i].y;
		}

		MatrixD X = A.getInverse() * Y;
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
			Vec2f circleCenter;
			l1.getIntersection(l2, circleCenter);

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

	PolynomialMathFunction MathExt::chebyshevPolynomial(int degree, bool secondKind)
	{
		const PolynomialMathFunction multiplier = PolynomialMathFunction({0, 2}); //2x
		PolynomialMathFunction f1 = PolynomialMathFunction({1}); //T[n-1] = 1
		PolynomialMathFunction f2 = PolynomialMathFunction({0, 1}); //T[n] = x
		if(secondKind == true)
			f2[1] = 2;

		if(degree == 0)
			return f1;
		if(degree == 1)
			return f2;
		
		PolynomialMathFunction result; //T[n+1] = 2xT[n] - T[n-1]
		for(int i=2; i<=degree; i++)
		{
			result = multiplier*f2 - f1;
			f1 = f2;
			f2 = result;
		}

		return result;
	}

	PolynomialMathFunction MathExt::chebyshevApproximation(MathFunction* f, double a, double b, int degree, bool secondKind)
	{
		//generate chebyshev nodes
		//need n+1 nodes
		if(f==nullptr)
			return PolynomialMathFunction();
		
		int n = degree;
		std::vector<Vec2f> pointsToFit = std::vector<Vec2f>(n+1);
		double A = (a+b)/2;
		double B = (b-a)/2;

		for(int k=0; k<=n; k++)
		{
			Vec2f Xk;
			Xk.x = A + B*cos(((2*k)*PI) / (2*n));
			Xk.y = f->solve(Xk.x);
			pointsToFit[k] = Xk;
		}

		return fitPolynomial(pointsToFit);
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
		double temp = 1.0+std::exp(-x);
		return 1.0 / temp;
	}
	
	Matrix<ComplexNumber> MathExt::crossCorrelationFFT(const Matrix<ComplexNumber>& baseImage, const Matrix<ComplexNumber>& kernel, int type, int paddingMode)
	{
		if(!baseImage.getValid() || !kernel.getValid())
			throw InvalidMatrix();
		
		Matrix<ComplexNumber> flippedKernel = kernel;
		size_t i=0;
		size_t j=0;
		do
		{
			do
			{
				ComplexNumber temp = flippedKernel[flippedKernel.getRows()-i-1][flippedKernel.getColumns()-j-1];
				flippedKernel[flippedKernel.getRows()-i-1][flippedKernel.getColumns()-j-1] = flippedKernel[i][j];
				flippedKernel[i][j] = temp;
				j++;
			} while (j < flippedKernel.getColumns()/2);
			i++;
		} while (i < flippedKernel.getRows()/2);

		return convolutionFFT(baseImage, kernel, type, paddingMode);
	}

	Matrix<ComplexNumber> MathExt::convolutionFFT(const Matrix<ComplexNumber>& baseImage, const Matrix<ComplexNumber>& kernel, int type, int paddingMode)
	{
		if(!baseImage.getValid() || !kernel.getValid())
			throw InvalidMatrix();
		//compute the 2D FFT of both the baseImage and kernel
		//ignore padding mode

		//force padding needed for full then extract later
		size_t newRowSize = baseImage.getRows()+kernel.getRows()-1;
		size_t newColSize = baseImage.getColumns()+kernel.getColumns()-1;
		
		size_t rowPadSize = newRowSize - baseImage.getRows();
		size_t colPadSize = newColSize - baseImage.getColumns();

		size_t trueRowSize = roundToNearestPower2(baseImage.getRows()+rowPadSize);
		size_t trueColSize = roundToNearestPower2(baseImage.getColumns()+colPadSize);
		
		size_t baseFFTRowPadding = trueRowSize - baseImage.getRows();
		size_t baseFFTColPadding = trueColSize - baseImage.getColumns();
		Matrix<ComplexNumber> baseFFT = baseImage.pad(baseFFTRowPadding, baseFFTColPadding, 0);

		size_t kernelFFTRowPadding = trueRowSize - kernel.getRows();
		size_t kernelFFTColPadding = trueColSize - kernel.getColumns();
		Matrix<ComplexNumber> kernelFFT = kernel.pad(kernelFFTRowPadding, kernelFFTColPadding, 0);
		
		MathExt::fastFourierTransform2DInline(baseFFT, false);
		MathExt::fastFourierTransform2DInline(kernelFFT, false);
		baseFFT = baseFFT.hadamardProduct(kernelFFT);

		MathExt::fastFourierTransform2DInline(baseFFT, true);

		//now extract the part you want
		if(type == CONVOLVE_FULL)
		{
			return baseFFT.extract(0, 0, newRowSize-1, newColSize-1);
		}
		else if(type == CONVOLVE_VALID)
		{
			size_t validRowSize = baseImage.getRows()-kernel.getRows()+1;
			size_t validColSize = baseImage.getColumns()-kernel.getColumns()+1;
			return baseFFT.extract(0, 0, validRowSize-1, validColSize-1);
		}
		//assume same size as base image
		return baseFFT.extract(0, 0, baseImage.getRows()-1, baseImage.getColumns()-1);
	}

	#pragma region FOURIER_TRANSFORM_1D

	void doFFTInline(ComplexNumber* output, size_t size, bool inverse)
	{
		//Reference: (https://cp-algorithms.com/algebra/fft.html#improved-implementation-in-place-computation)
		int bitShift = 32-log2(size)+1;
		for(uint32_t i=0; i<size/2; i++)
		{
        	uint32_t newIndex = MathExt::bitReversal(i)>>bitShift;
			std::swap(output[i], output[newIndex]);
		}

		double angle = (inverse) ? -2*PI : 2*PI;
		for(int l = 2; l<=size; l<<=1)
		{
			double ang = angle/l;
			size_t divSize = l/2;
			ComplexNumber wLen = ComplexNumber(MathExt::cos(ang), MathExt::sin(ang));
			ComplexNumber wLenSqr = wLen*wLen;
			ComplexNumber Ws[2] = {ComplexNumber(1, 0), ComplexNumber(1, 0)*wLen};

			if(l==2)
			{
				SIMD_SSE<ComplexNumber> wAsSIMD = SIMD_SSE<ComplexNumber>::load(Ws);
				
				ComplexNumber* startOutputU = &output[0];
				ComplexNumber* startOutputV = &output[divSize];
				for(int i=0; i<size; i+=2)
				{
					SIMD_SSE<ComplexNumber> uAsSIMD = SIMD_SSE<ComplexNumber>::load(startOutputU);
					SIMD_SSE<ComplexNumber> vAsSIMD = SIMD_SSE<ComplexNumber>::load(startOutputV);

					(uAsSIMD+vAsSIMD).store(startOutputU);
					(uAsSIMD-vAsSIMD).store(startOutputV);
					startOutputU += 2;
					startOutputV += 2;
				}
				continue;
			}
			
			size_t simdBound = SIMD_TEMPLATE<ComplexNumber>::getSIMDBound(divSize);
			SIMD_TEMPLATE<ComplexNumber> multAsSIMD = wLenSqr;

			for(int i=0; i<size; i+=l)
			{
				SIMD_TEMPLATE<ComplexNumber> twiddleAsSIMD = SIMD_TEMPLATE<ComplexNumber>::load(Ws);
				ComplexNumber* startOutputU = &output[i];
				ComplexNumber* startOutputV = &output[i+divSize];

				for(size_t j=0; j<simdBound; j += SIMD_TEMPLATE<ComplexNumber>::SIZE)
				{
					SIMD_TEMPLATE<ComplexNumber> uAsSIMD = SIMD_TEMPLATE<ComplexNumber>::load(startOutputU);
					SIMD_TEMPLATE<ComplexNumber> vAsSIMD = SIMD_TEMPLATE<ComplexNumber>::load(startOutputV);
					vAsSIMD *= twiddleAsSIMD;

					(uAsSIMD+vAsSIMD).store(startOutputU);
					(uAsSIMD-vAsSIMD).store(startOutputV);

					twiddleAsSIMD *= multAsSIMD;
					startOutputU += SIMD_TEMPLATE<ComplexNumber>::SIZE;
					startOutputV += SIMD_TEMPLATE<ComplexNumber>::SIZE;
				}
			}
		}

		if(inverse)
		{
			//cheating. Division by a single number and not a complex number can take a shortcut.
			SIMD_TEMPLATE<double> divValue = size;
			SIMD_TEMPLATE<double> outputAsRawDoubles;
			double* tempOutputPointer = (double*)output;
			size_t simdBound = SIMD_TEMPLATE<double>::getSIMDBound(size);
			for(size_t j=0; j<simdBound; j += SIMD_TEMPLATE<double>::SIZE)
			{
				outputAsRawDoubles = SIMD_TEMPLATE<double>::load((double*)tempOutputPointer);
				(outputAsRawDoubles/divValue).store(tempOutputPointer);
				tempOutputPointer += SIMD_TEMPLATE<double>::SIZE;
			}
			// for(int i=0; i<size; i++)
			// {
			// 	output[i] /= size;
			// }
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

	Matrix<ComplexNumber> MathExt::fastFourierTransform(const Matrix<ComplexNumber>& arr,  bool inverse)
	{
		//cooley tukey algorithm
		//must be a power of 2

		if(!arr.getValid())
			throw 0;
		if(arr.getRows()!=1)
			throw arr.getRows();

		if(IS_POWER_2(arr.getColumns()))
		{
			Matrix<ComplexNumber> output = arr;
			doFFTInline(output.getData(), arr.getColumns(), inverse);
			return output;
		}
		else
		{
			size_t newSize = roundToNearestPower2(arr.getColumns());
			Matrix<ComplexNumber> output = Matrix<ComplexNumber>(1, newSize);
			for(int i=0; i<arr.getColumns(); i++)
			{
				output[0][i] = arr[0][i];
			}
			doFFTInline(output.getData(), newSize, inverse);
			return output;
		}
	}

	void MathExt::fastFourierTransformInline(const Matrix<ComplexNumber>& arr, bool inverse)
	{
		//cooley tukey algorithm
		//must be a power of 2
		if(!arr.getValid())
			throw 0;
		if(arr.getRows()!=1)
			throw arr.getRows();
		if(!IS_POWER_2(arr.getColumns()!=1))
			throw arr.getColumns();
		
		doFFTInline(arr.getData(), arr.getColumns(), inverse);
	}

	#pragma endregion

	#pragma region FFT_2D
	
	Matrix<ComplexNumber> MathExt::fastFourierTransform2D(const Matrix<ComplexNumber>& mat, bool inverse)
	{
		if(!mat.getValid())
			return Matrix<ComplexNumber>();

		//need to force rows and columns to nearest power of 2.
		size_t newRows = roundToNearestPower2(mat.getRows());
		size_t newColumns = roundToNearestPower2(mat.getColumns());
		
		//copy arr into finalArr
		Matrix<ComplexNumber> finalArr = mat.pad(newRows, newColumns, 0);
		bool errFree = fastFourierTransform2DInline(finalArr);
		if(errFree)
			return finalArr;
		return Matrix<ComplexNumber>();
	}
	
	bool MathExt::fastFourierTransform2DInline(Matrix<ComplexNumber>& mat, bool inverse)
	{
		if(!mat.getValid())
			return false;
		if(!IS_POWER_2(mat.getRows()) || !IS_POWER_2(mat.getColumns()))
			return false;
		
		std::vector<ComplexNumber> outputColumn = std::vector<ComplexNumber>(mat.getRows());
		//for each row
		#pragma omp parallel for
		for(int v=0; v<mat.getRows(); v++)
		{
			ComplexNumber* outputRow = mat[v];
			doFFTInline(outputRow, mat.getColumns(), inverse);
		}

		//for each column
		#pragma omp parallel for
		for(int u=0; u<mat.getColumns(); u++)
		{
			for(int i=0; i<mat.getRows(); i++)
			{
				outputColumn[i] = mat[i][u];
			}
			doFFTInline(outputColumn.data(), outputColumn.size(), inverse);

			//copy into final arr column
			for(int i=0; i<mat.getRows(); i++)
			{
				mat[i][u] = outputColumn[i];
			}
		}

		return true;
	}

	#pragma endregion

	#pragma region COSINE_TRANSFORM_1D

	#pragma region NAYUKI_FDCT_STUFF

	/* 
	* Fast discrete cosine transform algorithms (C++)
	* 
	* Copyright (c) 2019 Project Nayuki. (MIT License)
	* https://www.nayuki.io/page/fast-discrete-cosine-transform-algorithms
	* 
	* Permission is hereby granted, free of charge, to any person obtaining a copy of
	* this software and associated documentation files (the "Software"), to deal in
	* the Software without restriction, including without limitation the rights to
	* use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
	* the Software, and to permit persons to whom the Software is furnished to do so,
	* subject to the following conditions:
	* - The above copyright notice and this permission notice shall be included in
	*   all copies or substantial portions of the Software.
	* - The Software is provided "as is", without warranty of any kind, express or
	*   implied, including but not limited to the warranties of merchantability,
	*   fitness for a particular purpose and noninfringement. In no event shall the
	*   authors or copyright holders be liable for any claim, damages or other
	*   liability, whether in an action of contract, tort or otherwise, arising from,
	*   out of or in connection with the Software or the use or other dealings in the
	*   Software.
	*/

	void doFDCT(float* arr, float* temp, size_t size)
	{
		if(size == 1)
			return;
		size_t halfSize = size/2;
		for(size_t i=0; i<halfSize; i++)
		{
			float x = arr[i];
			float y = arr[size - 1 - i];
			temp[i] = x + y;
			temp[i + halfSize] = (x-y) / (MathExt::cos((i + 0.5) * PI / size) * 2);
		}

		doFDCT(temp, arr, halfSize);
		doFDCT(&temp[halfSize], arr, halfSize);

		for(size_t i=0; i<halfSize-1; i++)
		{
			arr[2*i] = temp[i];
			arr[2*i + 1] = temp[i+halfSize] + temp[i+halfSize+1];
		}
		arr[size-2] = temp[halfSize-1];
		arr[size-1] = temp[size-1];
	}

	void doIDCT(float* arr, float* temp, size_t size)
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

		doIDCT(temp, arr, halfSize);
		doIDCT(&temp[halfSize], arr, halfSize);

		for(size_t i=0; i<halfSize; i++)
		{
			float x = temp[i];
			float y = temp[i + halfSize] / (MathExt::cos((i + 0.5) * PI / size) * 2);
			arr[i] = x+y;
			arr[size-1-i] = x-y;
		}
	}

	#pragma endregion

	double MathExt::discreteCosineTransform(double* arr, int size, int u, bool inverse)
	{
		double factor = PI/size;
		if(inverse == false)
		{
			double sum = 0;
			double scaleFactor = 0;
			for(int x=0; x<size; x++)
			{
				double cosCoeff = (x+0.5)*u*factor;
				sum += arr[x]*MathExt::cos(cosCoeff);
			}

			if(u == 0)
				scaleFactor = 1.0/MathExt::sqrt(size);
			else
				scaleFactor = MathExt::sqrt(2.0/size);

			return sum * scaleFactor;
		}
		else
		{
			double sum = arr[0]/MathExt::sqrt(2.0);
			double scaleFactor = MathExt::sqrt(2.0/size);
			for(int x=1; x<size; x++)
			{
				double cosCoeff = (u+0.5)*x*factor;
				sum += arr[x]*MathExt::cos(cosCoeff);
			}
			
			return sum * scaleFactor;
		}
	}
	float MathExt::discreteCosineTransform(float* arr, int size, int u, bool inverse)
	{
		float factor = PI/size;
		if(inverse == false)
		{
			float sum = 0;
			float scaleFactor = 0;
			for(int x=0; x<size; x++)
			{
				float cosCoeff = (x+0.5)*u*factor;
				sum += arr[x]*MathExt::cos(cosCoeff);
			}

			if(u == 0)
				scaleFactor = 1.0/MathExt::sqrt(size);
			else
				scaleFactor = MathExt::sqrt(2.0/size);

			return sum * scaleFactor;
		}
		else
		{
			float sum = arr[0]/MathExt::sqrt(2.0);
			float scaleFactor = MathExt::sqrt(2.0/size);
			for(int x=1; x<size; x++)
			{
				float cosCoeff = (u+0.5)*x*factor;
				sum += arr[x]*MathExt::cos(cosCoeff);
			}
			
			return sum * scaleFactor;
		}
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
	std::vector<float> MathExt::cosineTransform(float* arr, int size, bool inverse)
	{
		std::vector<float> newArr = std::vector<float>(size);

		for(int u=0; u<size; u++)
		{
			newArr[u] = discreteCosineTransform(arr, size, u, inverse);
		}

		return newArr;
	}

	//should be a fast version of the normal discreteCosineTransform
	std::vector<float> MathExt::fastCosineTransform(float* arr, size_t size, bool inverse)
	{
		std::vector<float> output;
		std::vector<float> temp;
		if(size == 0 || !IS_POWER_2(size))
		{
			//can't do unless it is a power of 2
			return output;
		}
		else
		{
			//copy arr into output
			output = std::vector<float>(size);
			temp = std::vector<float>(size);
			memcpy(output.data(), arr, size*sizeof(float));
			if(!inverse)
			{
				float factor = MathExt::sqrt(2.0/size);
				doFDCT(output.data(), temp.data(), size);
				output[0] *= 1.0/MathExt::sqrt(size);
				for(int i=1; i<size; i++)
				{
					output[i] *= factor;
				}
			}
			else
			{
				float factor = MathExt::sqrt(2.0/size);
				output[0] /= MathExt::sqrt(2.0);
				doIDCT(output.data(), temp.data(), size);
				for(int i=0; i<size; i++)
				{
					output[i] *= factor;
				}
			}
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

	float MathExt::discreteSineTransform(float* arr, size_t size, size_t u)
	{
		float sum = 0;
		float ang = PI*(u+1);
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

	std::vector<float> MathExt::sineTransform(float* arr, size_t size)
	{
		std::vector<float> newArr = std::vector<float>(size);
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

	MatrixF MathExt::cosineTransform2D(const MatrixF& arr, bool inverse)
	{
		MatrixF finalArr = MatrixF(arr.getRows(), arr.getColumns());

		//for each row
		for(int v=0; v<arr.getRows(); v++)
		{
			float* passArr = arr[v];
			std::vector<float> newArr = MathExt::cosineTransform(passArr, arr.getColumns(), inverse);

			for(int i=0; i<arr.getColumns(); i++)
			{
				finalArr[v][i] = newArr[i];
			}
		}

		//for each column
		for(int u=0; u<arr.getColumns(); u++)
		{
			std::vector<float> passArr = std::vector<float>(arr.getRows());

			for(int i=0; i<arr.getRows(); i++)
			{
				passArr[i] = finalArr[i][u];
			}

			std::vector<float> newArr = MathExt::cosineTransform(passArr.data(), arr.getColumns(), inverse);

			for(int i=0; i<arr.getRows(); i++)
			{
				finalArr[i][u] = newArr[i];
			}
		}

		return finalArr;
	}
	MatrixD MathExt::cosineTransform2D(const MatrixD& arr, bool inverse)
	{
		MatrixD finalArr = MatrixD(arr.getRows(), arr.getColumns());

		//for each row
		for(int v=0; v<arr.getRows(); v++)
		{
			double* passArr = arr[v];
			std::vector<double> newArr = MathExt::cosineTransform(passArr, arr.getColumns(), inverse);

			for(int i=0; i<arr.getColumns(); i++)
			{
				finalArr[v][i] = newArr[i];
			}
		}

		//for each column
		for(int u=0; u<arr.getColumns(); u++)
		{
			std::vector<double> passArr = std::vector<double>(arr.getRows());

			for(int i=0; i<arr.getRows(); i++)
			{
				passArr[i] = finalArr[i][u];
			}

			std::vector<double> newArr = MathExt::cosineTransform(passArr.data(), arr.getColumns(), inverse);

			for(int i=0; i<arr.getRows(); i++)
			{
				finalArr[i][u] = newArr[i];
			}
		}

		return finalArr;
	}

	MatrixF MathExt::fastCosineTransform2D(const MatrixF& arr, bool inverse)
	{
		if(!arr.getValid())
			return MatrixF();
		if(arr.getRows() != arr.getColumns())
			return MatrixF();
		if(!IS_POWER_2(arr.getRows()))
			return MatrixF();
		
		//copy arr into finalArr
		MatrixF finalArr = MatrixF(arr);
		std::vector<float> temp = std::vector<float>(finalArr.getRows());
		std::vector<float> columnArr = std::vector<float>(arr.getRows());

		if(!inverse)
		{
			//for each row
			for(int v=0; v<finalArr.getRows(); v++)
			{
				float* passArr = finalArr[v];
				doFDCT(passArr, temp.data(), temp.size());
			}

			//for each column
			for(int u=0; u<finalArr.getColumns(); u++)
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
				float* passArr = finalArr[v];
				doIDCT(passArr, temp.data(), temp.size());
			}

			//for each column
			for(int u=0; u<finalArr.getColumns(); u++)
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
	
	MatrixF MathExt::sineTransform2D(const MatrixF& arr)
	{
		MatrixF finalArr = MatrixF(arr.getRows(), arr.getColumns());

		//for each row
		for(int v=0; v<arr.getRows(); v++)
		{
			float* passArr = arr[v];
			std::vector<float> newArr = MathExt::sineTransform(passArr, arr.getColumns());

			for(int i=0; i<arr.getColumns(); i++)
			{
				finalArr[v][i] = newArr[i];
			}
		}

		//for each column
		for(int u=0; u<arr.getColumns(); u++)
		{
			std::vector<float> passArr = std::vector<float>(arr.getRows());

			for(int i=0; i<arr.getRows(); i++)
			{
				passArr[i] = finalArr[i][u];
			}

			std::vector<float> newArr = MathExt::sineTransform(passArr.data(), arr.getColumns());

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
	// 	if(arr.getRows() != arr.getColumns())
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
	// 		for(int u=0; u<finalArr.getColumns(); u++)
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
	// 		for(int u=0; u<finalArr.getColumns(); u++)
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

	void MathExt::FCT8(float* arr, float* output, bool inverse)
	{
		// //assume arr has a size of 8
		// double v[29];

		// if(!inverse)
		// {
		// 	v[0] = arr[0]+arr[7];
		// 	v[1] = arr[1]+arr[6];
		// 	v[2] = arr[2]+arr[5];
		// 	v[3] = arr[3]+arr[4];
		// 	v[4] = arr[3]-arr[4];
		// 	v[5] = arr[2]-arr[5];
		// 	v[6] = arr[1]-arr[6];
		// 	v[7] = arr[0]-arr[7];

		// 	v[8] = v[0] + v[3];
		// 	v[9] = v[1] + v[2];
		// 	v[10] = v[1] - v[2];
		// 	v[11] = v[0] - v[3];
		// 	v[12] = -v[4] - v[5];
		// 	v[13] = (v[5]+v[6]) * A[3];
		// 	v[14] = v[6] + v[7];

		// 	v[15] = v[8] + v[9];
		// 	v[16] = v[8] - v[9];
		// 	v[17] = (v[10] + v[11]) * A[1];
		// 	v[18] = (v[12] + v[14]) * A[5];
			
		// 	v[19] = -v[12]*A[2] - v[18];
		// 	v[20] = v[14]*A[4] - v[18];

		// 	v[21] = v[17] + v[11];
		// 	v[22] = v[11] - v[17];
		// 	v[23] = v[13] + v[7];
		// 	v[24] = v[7] - v[13];

		// 	v[25] = v[19] + v[24];
		// 	v[26] = v[23] + v[20];
		// 	v[27] = v[23] - v[20];
		// 	v[28] = v[24] - v[19];

		// 	output[0] = S[0] * v[15];
		// 	output[1] = S[1] * v[26];
		// 	output[2] = S[2] * v[21];
		// 	output[3] = S[3] * v[28];
		// 	output[4] = S[4] * v[16];
		// 	output[5] = S[5] * v[25];
		// 	output[6] = S[6] * v[22];
		// 	output[7] = S[7] * v[27];
		// }
		// else
		// {
		// 	v[15] = arr[0]/S[0];
		// 	v[26] = arr[1]/S[1];
		// 	v[21] = arr[2]/S[2];
		// 	v[28] = arr[3]/S[3];
		// 	v[16] = arr[4]/S[4];
		// 	v[25] = arr[5]/S[5];
		// 	v[22] = arr[6]/S[6];
		// 	v[27] = arr[7]/S[7];

		// 	v[19] = (v[25]-v[28])/2;
		// 	v[20] = (v[26]-v[27])/2;
		// 	v[23] = (v[26]+v[27])/2;
		// 	v[24] = (v[25]+v[28])/2;

		// 	v[7] = (v[23]+v[24])/2;
		// 	v[11] = (v[21]+v[22])/2;
		// 	v[13] = (v[23]-v[24])/2;
		// 	v[17] = (v[21]-v[22])/2;

		// 	v[8] = (v[15]+v[16])/2;
		// 	v[9] = (v[15]-v[16])/2;

		// 	v[18] = (v[19]-v[20]) * A[5];
		// 	v[12] = (v[19]*A[4] - v[18]) / (A[2]*A[5] - A[2]*A[4] - A[4]*A[5]);
		// 	v[14] = (v[18]-v[20] * A[2]) / (A[2]*A[5] - A[2]*A[4] - A[4]*A[5]);

		// 	v[6] = v[14] - v[7];
		// 	v[5] = v[13] / A[3] - v[6];
		// 	v[4] = -v[5] - v[12];
		// 	v[10] = v[17] / A[1] - v[11];

		// 	v[0] = (v[8]+v[11])/2;
		// 	v[1] = (v[9]+v[10])/2;
		// 	v[2] = (v[9]-v[10])/2;
		// 	v[3] = (v[8]-v[11])/2;

		// 	output[0] = (v[0] + v[7]) / 2;
		// 	output[1] = (v[1] + v[6]) / 2;
		// 	output[2] = (v[2] + v[5]) / 2;
		// 	output[3] = (v[3] + v[4]) / 2;
		// 	output[4] = (v[3] - v[4]) / 2;
		// 	output[5] = (v[2] - v[5]) / 2;
		// 	output[6] = (v[1] - v[6]) / 2;
		// 	output[7] = (v[0] - v[7]) / 2;
		// }

		if(!inverse)
		{
			const float mx00 = arr[0] + arr[7];
			const float mx01 = arr[1] + arr[6];
			const float mx02 = arr[2] + arr[5];
			const float mx03 = arr[3] + arr[4];
			const float mx04 = arr[0] - arr[7];
			const float mx05 = arr[1] - arr[6];
			const float mx06 = arr[2] - arr[5];
			const float mx07 = arr[3] - arr[4];
			const float mx08 = mx00 + mx03;
			const float mx09 = mx01 + mx02;
			const float mx0a = mx00 - mx03;
			const float mx0b = mx01 - mx02;
			const float mx0c = 1.38703984532215f*mx04 + 0.275899379282943f*mx07;
			const float mx0d = 1.17587560241936f*mx05 + 0.785694958387102f*mx06;
			const float mx0e = -0.785694958387102f*mx05 + 1.17587560241936f*mx06;
			const float mx0f = 0.275899379282943f*mx04 - 1.38703984532215f*mx07;
			const float mx10 = 0.353553390593274f * (mx0c - mx0d);
			const float mx11 = 0.353553390593274f * (mx0e - mx0f);
			output[0] = 0.353553390593274f * (mx08 + mx09);
			output[1] = 0.353553390593274f * (mx0c + mx0d);
			output[2] = 0.461939766255643f*mx0a + 0.191341716182545f*mx0b;
			output[3] = 0.707106781186547f * (mx10 - mx11);
			output[4] = 0.353553390593274f * (mx08 - mx09);
			output[5] = 0.707106781186547f * (mx10 + mx11);
			output[6] = 0.191341716182545f*mx0a - 0.461939766255643f*mx0b;
			output[7] = 0.353553390593274f * (mx0e + mx0f);
		}
		else
		{
			const float mx00 = 1.4142135623731f  *arr[0];
			const float mx01 = 1.38703984532215f *arr[1] + 0.275899379282943f*arr[7];
			const float mx02 = 1.30656296487638f *arr[2] + 0.541196100146197f*arr[6];
			const float mx03 = 1.17587560241936f *arr[3] + 0.785694958387102f*arr[5];
			const float mx04 = 1.4142135623731f  *arr[4];
			const float mx05 = -0.785694958387102f*arr[3] + 1.17587560241936f*arr[5];
			const float mx06 = 0.541196100146197f*arr[2] - 1.30656296487638f*arr[6];
			const float mx07 = -0.275899379282943f*arr[1] + 1.38703984532215f*arr[7];
			const float mx09 = mx00 + mx04;
			const float mx0a = mx01 + mx03;
			const float mx0b = 1.4142135623731f*mx02;
			const float mx0c = mx00 - mx04;
			const float mx0d = mx01 - mx03;
			const float mx0e = 0.353553390593274f * (mx09 - mx0b);
			const float mx0f = 0.353553390593274f * (mx0c + mx0d);
			const float mx10 = 0.353553390593274f * (mx0c - mx0d);
			const float mx11 = 1.4142135623731f*mx06;
			const float mx12 = mx05 + mx07;
			const float mx13 = mx05 - mx07;
			const float mx14 = 0.353553390593274f * (mx11 + mx12);
			const float mx15 = 0.353553390593274f * (mx11 - mx12);
			const float mx16 = 0.5f*mx13;
			output[0] = 0.25f * (mx09 + mx0b) + 0.353553390593274f*mx0a;
			output[1] = 0.707106781186547f * (mx0f + mx15);
			output[2] = 0.707106781186547f * (mx0f - mx15);
			output[3] = 0.707106781186547f * (mx0e + mx16);
			output[4] = 0.707106781186547f * (mx0e - mx16);
			output[5] = 0.707106781186547f * (mx10 - mx14);
			output[6] = 0.707106781186547f * (mx10 + mx14);
			output[7] = 0.25f * (mx09 + mx0b) - 0.353553390593274f*mx0a;
		}
	}

	void MathExt::FCT8x8(const MatrixF& arr, MatrixF* output, bool inverse)
	{
		//for each row
		float* outputAsDoubleArr = output->getData();
		float newArr[8];
		float colArr[8];

		for(int v=0; v<arr.getRows(); v++)
		{
			MathExt::FCT8(arr[v], &outputAsDoubleArr[v*8], inverse);
		}

		//for each column
		for(int u=0; u<arr.getColumns(); u++)
		{
			for(int i=0; i<arr.getRows(); i++)
			{
				colArr[i] = outputAsDoubleArr[u + i*8];
			}

			MathExt::FCT8(colArr, newArr, inverse);

			for(int i=0; i<arr.getRows(); i++)
			{
				outputAsDoubleArr[u + i*8] = newArr[i];
			}
		}
	}

	#pragma endregion
} //NAMESPACE glib END