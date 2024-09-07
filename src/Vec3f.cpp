#include "Vec3f.h"
#include <cmath>
#include <math.h>

namespace smpl
{

	const RootClass Vec3f::globalClass = CREATE_ROOT_CLASS(Vec3f, &SerializedObject::globalClass);
    const RootClass* Vec3f::getClass()
	{
		return &Vec3f::globalClass;
	}
	
	Vec3f::Vec3f(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	Vec3f::Vec3f()
	{
	}

	Vec3f::Vec3f(Vec2f other, float z)
	{
		this->x = other.x;
		this->y = other.y;
		this->z = z;
	}


	Vec3f::~Vec3f()
	{
	}

	std::unordered_map<std::string, SerializedData> Vec3f::getSerializedVariables()
	{
		return {SERIALIZE_MAP(x), SERIALIZE_MAP(y), SERIALIZE_MAP(z)};
	}

	float Vec3f::getX()
	{
		return x;
	}

	float Vec3f::getY()
	{
		return y;
	}

	float Vec3f::getZ()
	{
		return z;
	}

	void Vec3f::setX(float value)
	{
		x = value;
	}

	void Vec3f::setY(float value)
	{
		y = value;
	}

	void Vec3f::setZ(float value)
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

	float Vec3f::getLength()
	{
		return std::sqrt((x*x) + (y*y) + (z*z));
	}

	Vec3f Vec3f::normalize()
	{
		float length = getLength();
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

	void Vec3f::operator*=(float other)
	{
		x *= other;
		y *= other;
		z *= other;
	}

	void Vec3f::operator/=(float other)
	{
		x /= other;
		y /= other;
		z /= other;
	}

	Vec3f Vec3f::operator*(float value)
	{
		return Vec3f(x*value, y*value, z*value);
	}

	Vec3f Vec3f::operator/(float value)
	{
		return Vec3f(x/value, y/value, z/value);
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