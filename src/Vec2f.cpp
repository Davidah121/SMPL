#include "Vec2f.h"
#include <cmath>
#include <math.h>

namespace smpl
{
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

	void Vec2f::setValues(const Vec2f& other)
	{
		x = other.x;
		y = other.y;
	}

	float Vec2f::getLength() const
	{
		return std::sqrt((x*x) + (y*y));
	}

	Vec2f Vec2f::normalize() const
	{
		float length = getLength();
		if(length!=0)
			return Vec2f(x/length, y/length);
		else
			return Vec2f();
	}
	
	Vec2f Vec2f::hadamardProduct(const Vec2f& other) const
	{
		return Vec2f(x*other.x, y*other.y);
	}
	
	float Vec2f::dot(const Vec2f& b) const
	{
		return x*b.x + y*b.y;
	}
	float Vec2f::project(const Vec2f& b) const
	{
		return dot(b.normalize());
	}
	Vec2f Vec2f::reflect(const Vec2f& b) const
	{
		Vec2f n = b.normalize();
		return this->operator-(n*(dot(n)*2));
	}
	Vec2f Vec2f::inverse() const
	{
		return Vec2f(-y, x);
	}
	
	Vec2f Vec2f::operator-(const Vec2f& other) const
	{
		return Vec2f(x-other.x, y-other.y);
	}

	Vec2f Vec2f::operator+(const Vec2f& other) const
	{
		return Vec2f(x+other.x, y+other.y);
	}

	void Vec2f::operator+=(const Vec2f& other)
	{
		x += other.x;
		y += other.y;
	}

	void Vec2f::operator-=(const Vec2f& other)
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

	bool Vec2f::operator==(const Vec2f& other) const
	{
		return (x==other.x) && (y==other.y);
	}

	bool Vec2f::operator!=(const Vec2f& other) const
	{
		return (x!=other.x) || (y!=other.y);
	}

	Vec2f Vec2f::operator-() const
	{
		return Vec2f(-x, -y);
	}

	Vec2f Vec2f::operator*(float value) const
	{
		return Vec2f(x*value, y*value);
	}

	Vec2f Vec2f::operator/(float value) const
	{
		return Vec2f(x / value, y / value);
	}
	
	Vec2f Vec2f::broadcastAdd(float v)
	{
		return Vec2f(x + v, y + v);
	}
	Vec2f Vec2f::broadcastSubtract(float v)
	{
		return Vec2f(x - v, y - v);
	}
	Vec2f Vec2f::broadcastFunction(std::function<float(float)> func)
	{
		if(func == nullptr)
			throw std::bad_function_call();
		return Vec2f(func(x), func(y));
	}
	Vec2f Vec2f::broadcastFunction(float (*func)(float))
	{
		if(func == nullptr)
			throw std::bad_function_call();
		return Vec2f(func(x), func(y));
	}
	
	MatrixF Vec2f::toMatrix() const
	{
		return (MatrixF)* this;
	}

	Vec2f::operator MatrixF() const
	{
		MatrixF k = MatrixF(1, 2);
		k[0][0] = x;
		k[0][1] = y;
		return k;
	}

} //NAMESPACE glib END