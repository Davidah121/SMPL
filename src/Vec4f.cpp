#include "Vec4f.h"
#include <cmath>
#include <math.h>

namespace smpl
{
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

	Vec4f::Vec4f(const Vec3f& other, float w)
	{
		this->x = other.x;
		this->y = other.y;
		this->z = other.z;
		this->w = w;
	}

	Vec4f::Vec4f(const Vec2f& other, float z, float w)
	{
		this->x = other.x;
		this->y = other.y;
		this->z = z;
		this->w = w;
	}

	Vec4f::~Vec4f()
	{
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

	void Vec4f::setValues(const Vec2f& value)
	{
		x = value.x;
		y = value.y;
	}

	void Vec4f::setValues(const Vec3f& value)
	{
		x = value.x;
		y = value.y;
		z = value.z;
	}

	void Vec4f::setValues(const Vec4f& value)
	{
		x = value.x;
		y = value.y;
		z = value.z;
		w = value.w;
	}

	Vec3f Vec4f::toVec3f() const
	{
		return Vec3f(x,y,z);
	}

	float Vec4f::getLength() const
	{
		return std::sqrt((x*x) + (y*y) + (z*z) + (w*w));
	}

	Vec4f Vec4f::normalize() const
	{
		float length = getLength();
		if(length!=0)
			return Vec4f(x/length, y/length, z/length, w/length);
		else
			return Vec4f();
	}

	Vec4f Vec4f::hadamardProduct(const Vec4f& other) const
	{
		return Vec4f(x*other.x, y*other.y, z*other.z, w*other.w);
	}
	
	float Vec4f::dot(const Vec4f& b) const
	{
		return x*b.x + y*b.y + z*b.z + w*b.w;
	}
	float Vec4f::project(const Vec4f& b) const
	{
		return dot(b.normalize());
	}
	Vec4f Vec4f::reflect(const Vec4f& b) const
	{
		Vec4f n = b.normalize();
		return this->operator-(n*(dot(n)*2));
	}

	Vec4f Vec4f::operator-(const Vec4f& other) const
	{
		return Vec4f(x - other.x, y - other.y, z - other.z, w - other.w);
	}

	Vec4f Vec4f::operator+(const Vec4f& other) const
	{
		return Vec4f(x + other.x, y + other.y, z + other.z, w + other.w);
	}

	Vec4f Vec4f::operator*(float other) const
	{
		return Vec4f(x*other, y*other, z*other, w*other);
	}

	Vec4f Vec4f::operator-() const
	{
		return Vec4f(-x, -y, -z, -w);
	}

	bool Vec4f::operator==(const Vec4f& other) const
	{
		return (x==other.x) && (y==other.y) && (z==other.z) && (w==other.w);
	}

	bool Vec4f::operator!=(const Vec4f& other) const
	{
		return (x!=other.x) || (y!=other.y) || (z!=other.z) || (w!=other.w);
	}

	void Vec4f::operator+=(const Vec4f& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
		w += other.w;
	}

	void Vec4f::operator-=(const Vec4f& other)
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

	Vec4f Vec4f::operator/(float other) const
	{
		return Vec4f(x / other, y / other, z / other, w / other);
	}

	Vec4f Vec4f::broadcastAdd(float v)
	{
		return Vec4f(x + v, y + v, z + v, w + v);
	}
	Vec4f Vec4f::broadcastSubtract(float v)
	{
		return Vec4f(x - v, y - v, z - v, w + v);
	}
	Vec4f Vec4f::broadcastFunction(std::function<float(float)> func)
	{
		if(func == nullptr)
			throw std::bad_function_call();
		return Vec4f(func(x), func(y), func(z), func(w));
	}
	Vec4f Vec4f::broadcastFunction(float (*func)(float))
	{
		if(func == nullptr)
			throw std::bad_function_call();
		return Vec4f(func(x), func(y), func(z), func(w));
	}
	
	MatrixF Vec4f::toMatrix() const
	{
		return (MatrixF)* this;
	}

	Vec4f::operator MatrixF() const
	{
		MatrixF k = MatrixF(1, 4);
		k[0][0] = x;
		k[0][1] = y;
		k[0][2] = z;
		k[0][3] = w;
		return k;
	}
	
	void formatToString(StringStream& stream, const Vec4f& v, const std::string& options)
	{
		stream.write('<');
		formatToString(stream, v.x, options);
		stream.write(", ");
		formatToString(stream, v.y, options);
		stream.write(", ");
		formatToString(stream, v.z, options);
		stream.write(", ");
		formatToString(stream, v.w, options);
		stream.write('>');
	}

} //NAMESPACE glib END