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

	Vec3f::Vec3f(const Vec2f& other, float z)
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

	Vec2f Vec3f::toVec2f() const
	{
		return Vec2f(x,y);
	}

	float Vec3f::getLength() const
	{
		return std::sqrt((x*x) + (y*y) + (z*z));
	}

	Vec3f Vec3f::normalize() const
	{
		float length = getLength();
		if(length!=0)
			return Vec3f(x/length, y/length, z/length);
		else
			return Vec3f();
	}

	Vec3f Vec3f::hadamardProduct(const Vec3f& other) const
	{
		return Vec3f(x*other.x, y*other.y, z*other.z);
	}
	
	float Vec3f::dot(const Vec3f& b) const
	{
		return x*b.x + y*b.y + z*b.z;
	}
	float Vec3f::project(const Vec3f& b) const
	{
		return dot(b.normalize());
	}
	Vec3f Vec3f::reflect(const Vec3f& b) const
	{
		Vec3f n = b.normalize();
		return this->operator-(n*(dot(n)*2));
	}

	Vec3f Vec3f::crossProduct(const Vec3f& v2) const
	{
		Vec3f cross = Vec3f();
		cross.x = y*v2.z - z*v2.y;
		cross.y = z*v2.x - x*v2.z;
		cross.z = x*v2.y - y*v2.x;
		return cross;
	}

	Vec3f Vec3f::operator-(const Vec3f& other) const
	{
		return Vec3f(x - other.x, y - other.y, z - other.z);
	}

	Vec3f Vec3f::operator+(const Vec3f& other) const
	{
		return Vec3f(x + other.x, y + other.y, z + other.z);
	}

	Vec3f Vec3f::operator-() const
	{
		return Vec3f(-x, -y, -z);
	}

	bool Vec3f::operator==(const Vec3f& other) const
	{
		return (x==other.x) && (y==other.y) && (z==other.z);
	}

	bool Vec3f::operator!=(const Vec3f& other) const
	{
		return (x!=other.x) || (y!=other.y) || (z!=other.z);
	}

	void Vec3f::operator+=(const Vec3f& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
	}

	void Vec3f::operator-=(const Vec3f& other)
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

	Vec3f Vec3f::operator*(float value) const
	{
		return Vec3f(x*value, y*value, z*value);
	}

	Vec3f Vec3f::operator/(float value) const
	{
		return Vec3f(x/value, y/value, z/value);
	}
	
	Vec3f Vec3f::broadcastAdd(float v)
	{
		return Vec3f(x + v, y + v, z + v);
	}
	Vec3f Vec3f::broadcastSubtract(float v)
	{
		return Vec3f(x - v, y - v, z - v);
	}
	Vec3f Vec3f::broadcastFunction(std::function<float(float)> func)
	{
		if(func == nullptr)
			throw std::bad_function_call();
		return Vec3f(func(x), func(y), func(z));
	}
	Vec3f Vec3f::broadcastFunction(float (*func)(float))
	{
		if(func == nullptr)
			throw std::bad_function_call();
		return Vec3f(func(x), func(y), func(z));
	}
	
	Matrix Vec3f::toMatrix() const
	{
		return (Matrix)* this;
	}

	Vec3f::operator Matrix() const
	{
		Matrix k = Matrix(1, 3);
		k[0][0] = x;
		k[0][1] = y;
		k[0][2] = z;
		return k;
	}

} //NAMESPACE glib END