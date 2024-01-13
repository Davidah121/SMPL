#include "Vec4f.h"
#include <cmath>
#include <math.h>

namespace glib
{
		
	const RootClass Vec4f::globalClass = RootClass("Vec4f", {"Object"});

	Vec4f::Vec4f()
	{
		setClass(globalClass);
	}

	Vec4f::Vec4f(double x, double y, double z, double w)
	{
		setClass(globalClass);
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	Vec4f::Vec4f(const Vec4f& other)
	{
		setClass(globalClass);
		x = other.x;
		y = other.y;
		z = other.z;
		w = other.w;
	}

	void Vec4f::operator=(const Vec4f& other)
	{
		x = other.x;
		y = other.y;
		z = other.z;
		w = other.w;
	}

	Vec4f::Vec4f(Vec3f other, double w)
	{
		setClass(globalClass);
		this->x = other.x;
		this->y = other.y;
		this->z = other.z;
		this->w = w;
	}

	Vec4f::Vec4f(Vec2f other, double z, double w)
	{
		setClass(globalClass);
		this->x = other.x;
		this->y = other.y;
		this->z = z;
		this->w = w;
	}

	Vec4f::~Vec4f()
	{
	}

	double Vec4f::getX()
	{
		return x;
	}

	double Vec4f::getY()
	{
		return y;
	}

	double Vec4f::getZ()
	{
		return z;
	}

	double Vec4f::getW()
	{
		return w;
	}

	void Vec4f::setX(double value)
	{
		x = value;
	}

	void Vec4f::setY(double value)
	{
		y = value;
	}

	void Vec4f::setZ(double value)
	{
		z = value;
	}

	void Vec4f::setW(double value)
	{
		w = value;
	}

	void Vec4f::setValues(Vec2f value)
	{
		x = value.x;
		y = value.y;
	}

	void Vec4f::setValues(Vec3f value)
	{
		x = value.x;
		y = value.y;
		z = value.z;
	}

	void Vec4f::setValues(Vec4f value)
	{
		x = value.x;
		y = value.y;
		z = value.z;
		w = value.w;
	}

	Vec3f Vec4f::toVec3f()
	{
		return Vec3f(x,y,z);
	}

	double Vec4f::getLength()
	{
		return std::sqrt((x*x) + (y*y) + (z*z) + (w*w));
	}

	Vec4f Vec4f::normalize()
	{
		double length = getLength();
		if(length!=0)
			return Vec4f(x/length, y/length, z/length, w/length);
		else
			return Vec4f();
	}

	Vec4f Vec4f::operator-(Vec4f other)
	{
		return Vec4f(x - other.x, y - other.y, z - other.z, w - other.w);
	}

	Vec4f Vec4f::operator+(Vec4f other)
	{
		return Vec4f(x + other.x, y + other.y, z + other.z, w + other.w);
	}

	Vec4f Vec4f::operator*(double other)
	{
		return Vec4f(x*other, y*other, z*other, w*other);
	}

	Vec4f Vec4f::operator-()
	{
		return Vec4f(-x, -y, -z, -w);
	}

	bool Vec4f::operator==(Vec4f other)
	{
		return (x==other.x) && (y==other.y) && (z==other.z) && (w==other.w);
	}

	bool Vec4f::operator!=(Vec4f other)
	{
		return (x!=other.x) || (y!=other.y) || (z!=other.z) || (w!=other.w);
	}

	void Vec4f::operator+=(Vec4f other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
		w += other.w;
	}

	void Vec4f::operator-=(Vec4f other)
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
		w -= other.w;
	}

	void Vec4f::operator*=(double other)
	{
		x *= other;
		y *= other;
		z *= other;
		w *= other;
	}
	void Vec4f::operator/=(double other)
	{
		x /= other;
		y /= other;
		z /= other;
		w /= other;
	}

	Vec4f Vec4f::operator/(double other)
	{
		return Vec4f(x / other, y / other, z / other, w / other);
	}

	void Vec4f::fillArray(float* buffer)
	{
		buffer[0] = (float)x;
		buffer[1] = (float)y;
		buffer[2] = (float)z;
		buffer[3] = (float)w;
	}
	void Vec4f::fillArray(double* buffer)
	{
		buffer[0] = x;
		buffer[1] = y;
		buffer[2] = z;
		buffer[3] = w;
	}

	float* Vec4f::convertToFloatArray()
	{
		return new float[4] {(float)x, (float)y, (float)z, (float)z};
	}
	double* Vec4f::convertToDoubleArray()
	{
		return new double[4] {x, y, z, w};
	}

	GeneralVector Vec4f::toGeneralVector()
	{
		return (GeneralVector)* this;
	}

	Vec4f::operator GeneralVector() const
	{
		GeneralVector k = GeneralVector(4);
		k.setValue(x, 0);
		k.setValue(y, 1);
		k.setValue(z, 2);
		k.setValue(w, 3);

		return k;
	}

} //NAMESPACE glib END