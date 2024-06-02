#include "Vec2f.h"
#include <cmath>
#include <math.h>

namespace smpl
{
	const RootClass Vec2f::globalClass = CREATE_ROOT_CLASS(Vec2f, &SerializedObject::globalClass);
    const RootClass* Vec2f::getClass()
	{
		return &Vec2f::globalClass;
	}
	Vec2f::Vec2f(double x, double y)
	{
		this->x = x;
		this->y = y;
	}

	Vec2f::Vec2f()
	{
	}

	Vec2f::~Vec2f()
	{
	}
	
	std::unordered_map<std::string, SerializedData> Vec2f::getSerializedVariables()
	{
		return {SERIALIZE_MAP(x), SERIALIZE_MAP(y)};
	}

	double Vec2f::getX()
	{
		return x;
	}

	double Vec2f::getY()
	{
		return y;
	}


	void Vec2f::setX(double value)
	{
		x = value;
	}

	void Vec2f::setY(double value)
	{
		y = value;
	}

	void Vec2f::setValues(Vec2f other)
	{
		x = other.x;
		y = other.y;
	}

	double Vec2f::getLength()
	{
		return std::sqrt((x*x) + (y*y));
	}

	Vec2f Vec2f::normalize()
	{
		double length = getLength();
		if(length!=0)
			return Vec2f(x/length, y/length);
		else
			return Vec2f();
	}
	
	Vec2f Vec2f::operator-(Vec2f other)
	{
		return Vec2f(x-other.x, y-other.y);
	}

	Vec2f Vec2f::operator+(Vec2f other)
	{
		return Vec2f(x+other.x, y+other.y);
	}

	void Vec2f::operator+=(Vec2f other)
	{
		x += other.x;
		y += other.y;
	}

	void Vec2f::operator-=(Vec2f other)
	{
		x -= other.x;
		y -= other.y;
	}

	void Vec2f::operator*=(double other)
	{
		x *= other;
		y *= other;
	}

	void Vec2f::operator/=(double other)
	{
		x /= other;
		y /= other;
	}

	bool Vec2f::operator==(Vec2f other)
	{
		return (x==other.x) && (y==other.y);
	}

	bool Vec2f::operator!=(Vec2f other)
	{
		return (x!=other.x) || (y!=other.y);
	}

	Vec2f Vec2f::operator-()
	{
		return Vec2f(-x, -y);
	}

	Vec2f Vec2f::operator*(double value)
	{
		return Vec2f(x*value, y*value);
	}

	Vec2f Vec2f::operator/(double value)
	{
		return Vec2f(x / value, y / value);
	}

	void Vec2f::fillArray(float* buffer)
	{
		buffer[0] = (float)x;
		buffer[1] = (float)y;
	}
	void Vec2f::fillArray(double* buffer)
	{
		buffer[0] = x;
		buffer[1] = y;
	}

	float* Vec2f::convertToFloatArray()
	{
		return new float[2] {(float)x, (float)y};
	}
	double* Vec2f::convertToDoubleArray()
	{
		return new double[2] {x, y};
	}

	GeneralVector Vec2f::toGeneralVector()
	{
		return (GeneralVector)* this;
	}

	Vec2f::operator GeneralVector() const
	{
		GeneralVector k = GeneralVector(2);
		k.setValue(x, 0);
		k.setValue(y, 1);

		return k;
	}

} //NAMESPACE glib END