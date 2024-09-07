#include "Vec4f.h"
#include <cmath>
#include <math.h>

namespace smpl
{
		
	const RootClass Vec4f::globalClass = CREATE_ROOT_CLASS(Vec4f, &SerializedObject::globalClass);
    const RootClass* Vec4f::getClass()
	{
		return &Vec4f::globalClass;
	}
	Vec4f::Vec4f()
	{
	}

	Vec4f::Vec4f(float x, float y, float z, float w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	Vec4f::Vec4f(Vec3f other, float w)
	{
		this->x = other.x;
		this->y = other.y;
		this->z = other.z;
		this->w = w;
	}

	Vec4f::Vec4f(Vec2f other, float z, float w)
	{
		this->x = other.x;
		this->y = other.y;
		this->z = z;
		this->w = w;
	}

	Vec4f::~Vec4f()
	{
	}
	
	std::unordered_map<std::string, SerializedData> Vec4f::getSerializedVariables()
	{
		return {SERIALIZE_MAP(x), SERIALIZE_MAP(y), SERIALIZE_MAP(z), SERIALIZE_MAP(w)};
	}

	float Vec4f::getX()
	{
		return x;
	}

	float Vec4f::getY()
	{
		return y;
	}

	float Vec4f::getZ()
	{
		return z;
	}

	float Vec4f::getW()
	{
		return w;
	}

	void Vec4f::setX(float value)
	{
		x = value;
	}

	void Vec4f::setY(float value)
	{
		y = value;
	}

	void Vec4f::setZ(float value)
	{
		z = value;
	}

	void Vec4f::setW(float value)
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

	float Vec4f::getLength()
	{
		return std::sqrt((x*x) + (y*y) + (z*z) + (w*w));
	}

	Vec4f Vec4f::normalize()
	{
		float length = getLength();
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

	Vec4f Vec4f::operator*(float other)
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

	void Vec4f::operator*=(float other)
	{
		x *= other;
		y *= other;
		z *= other;
		w *= other;
	}
	void Vec4f::operator/=(float other)
	{
		x /= other;
		y /= other;
		z /= other;
		w /= other;
	}

	Vec4f Vec4f::operator/(float other)
	{
		return Vec4f(x / other, y / other, z / other, w / other);
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