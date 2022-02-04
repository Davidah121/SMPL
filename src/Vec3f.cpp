#include "Vec3f.h"

namespace glib
{

	const Class Vec3f::myClass = Class("Vec3f", {&Object::myClass});
	const Class* Vec3f::getClass()
	{
		return &Vec3f::myClass;
	}

	Vec3f::Vec3f(double x, double y, double z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	Vec3f::Vec3f()
	{
	}

	Vec3f::Vec3f(Vec2f other, double z)
	{
		this->x = other.x;
		this->y = other.y;
		this->z = z;
	}


	Vec3f::~Vec3f()
	{
	}


	double Vec3f::getX()
	{
		return x;
	}

	double Vec3f::getY()
	{
		return y;
	}

	double Vec3f::getZ()
	{
		return z;
	}

	void Vec3f::setX(double value)
	{
		x = value;
	}

	void Vec3f::setY(double value)
	{
		y = value;
	}

	void Vec3f::setZ(double value)
	{
		z = value;
	}

	void Vec3f::setValues(Vec3f other)
	{
		x = other.x;
		y = other.y;
		z = other.z;
	}

	void Vec3f::setValues(Vec2f other)
	{
		x = other.x;
		y = other.y;
	}

	Vec2f Vec3f::toVec2f()
	{
		return Vec2f(x,y);
	}

	double Vec3f::getLength()
	{
		return sqrt((x*x) + (y*y) + (z*z));
	}

	Vec3f Vec3f::normalize()
	{
		double length = getLength();
		if(length!=0)
			return Vec3f(x/length, y/length, z/length);
		else
			return Vec3f();
	}

	Vec3f Vec3f::operator-(Vec3f other)
	{
		return Vec3f(x - other.x, y - other.y, z - other.z);
	}

	Vec3f Vec3f::operator+(Vec3f other)
	{
		return Vec3f(x + other.x, y + other.y, z + other.z);
	}

	Vec3f Vec3f::operator-()
	{
		return Vec3f(-x, -y, -z);
	}

	bool Vec3f::operator==(Vec3f other)
	{
		return (x==other.x) && (y==other.y) && (z==other.z);
	}

	bool Vec3f::operator!=(Vec3f other)
	{
		return (x!=other.x) || (y!=other.y) || (z!=other.z);
	}

	void Vec3f::operator+=(Vec3f other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
	}

	void Vec3f::operator-=(Vec3f other)
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
	}

	void Vec3f::operator*=(double other)
	{
		x *= other;
		y *= other;
		z *= other;
	}

	void Vec3f::operator/=(double other)
	{
		x /= other;
		y /= other;
		z /= other;
	}

	Vec3f Vec3f::operator*(double value)
	{
		return Vec3f(x*value, y*value, z*value);
	}

	Vec3f Vec3f::operator/(double value)
	{
		return Vec3f(x/value, y/value, z/value);
	}

	void Vec3f::fillArray(float* buffer)
	{
		buffer[0] = (float)x;
		buffer[1] = (float)y;
		buffer[2] = (float)z;
	}
	void Vec3f::fillArray(double* buffer)
	{
		buffer[0] = x;
		buffer[1] = y;
		buffer[2] = z;
	}

	float* Vec3f::convertToFloatArray()
	{
		return new float[3] {(float)x, (float)y, (float)z};
	}
	double* Vec3f::convertToDoubleArray()
	{
		return new double[3] {x, y, z};
	}

	GeneralVector Vec3f::toGeneralVector()
	{
		return (GeneralVector)* this;
	}

	Vec3f::operator GeneralVector() const
	{
		GeneralVector k = GeneralVector(3);
		k.setValue(x, 0);
		k.setValue(y, 1);
		k.setValue(z, 2);
		return k;
	}

} //NAMESPACE glib END