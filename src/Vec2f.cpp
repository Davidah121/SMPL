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
	Vec2f::Vec2f(float x, float y)
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

	float Vec2f::getX()
	{
		return x;
	}

	float Vec2f::getY()
	{
		return y;
	}


	void Vec2f::setX(float value)
	{
		x = value;
	}

	void Vec2f::setY(float value)
	{
		y = value;
	}

	void Vec2f::setValues(Vec2f other)
	{
		x = other.x;
		y = other.y;
	}

	float Vec2f::getLength()
	{
		return std::sqrt((x*x) + (y*y));
	}

	Vec2f Vec2f::normalize()
	{
		float length = getLength();
		if(length!=0)
			return Vec2f(x/length, y/length);
		else
			return Vec2f();
	}
	
	Vec2f Vec2f::hadamardProduct(Vec2f other)
	{
		return Vec2f(x*other.x, y*other.y);
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

	void Vec2f::operator*=(float other)
	{
		x *= other;
		y *= other;
	}

	void Vec2f::operator/=(float other)
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

	Vec2f Vec2f::operator*(float value)
	{
		return Vec2f(x*value, y*value);
	}

	Vec2f Vec2f::operator/(float value)
	{
		return Vec2f(x / value, y / value);
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