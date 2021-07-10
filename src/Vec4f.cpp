#include "Vec4f.h"
#include "MathExt.h"

const Class Vec4f::myClass = Class("Vec4f", {&Object::myClass});
const Class* Vec4f::getClass()
{
	return &Vec4f::myClass;
}

Vec4f::Vec4f()
{
}

Vec4f::Vec4f(double x, double y, double z, double w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

Vec4f::Vec4f(Vec3f other)
{
	this->x = other.x;
	this->y = other.y;
	this->z = other.z;
}

Vec4f::Vec4f(Vec2f other)
{
	this->x = other.x;
	this->y = other.y;
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

Vec4f Vec4f::operator-(Vec4f other)
{
	return Vec4f(x - other.x, y - other.y, z - other.z, w - other.w);
}

Vec4f Vec4f::operator+(Vec4f other)
{
	return Vec4f(x + other.x, y + other.y, z + other.z, w + other.w);
}

Vec4f Vec4f::operator*(long other)
{
	return Vec4f(x*other, y*other, z*other, w*other);
}

Vec4f Vec4f::operator*(double other)
{
	return Vec4f(x*other, y*other, z*other, w*other);
}

Vec4f Vec4f::operator*(float other)
{
	return Vec4f(x*other, y*other, z*other, w*other);
}

Vec4f Vec4f::operator*(int other)
{
	return Vec4f(x*other, y*other, z*other, w*other);
}

Vec4f Vec4f::operator*(Vec4f other)
{
	return Vec4f(x * other.x, y * other.y, z * other.z, w * other.w);
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

void Vec4f::operator*=(Vec4f other)
{
	x *= other.x;
	y *= other.y;
	z *= other.z;
	w *= other.w;
}

void Vec4f::operator/=(Vec4f other)
{
	x /= other.x;
	y /= other.y;
	z /= other.z;
	w /= other.w;
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

void Vec4f::operator*=(long other)
{
	x *= other;
	y *= other;
	z *= other;
	w *= other;
}
void Vec4f::operator/=(long other)
{
	x /= other;
	y /= other;
	z /= other;
	w /= other;
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

void Vec4f::operator*=(int other)
{
	x *= other;
	y *= other;
	z *= other;
	w *= other;
}
void Vec4f::operator/=(int other)
{
	x /= other;
	y /= other;
	z /= other;
	w /= other;
}
Vec4f Vec4f::operator/(long other)
{
	return Vec4f(x/other, y/other, z/other, w/other);
}

Vec4f Vec4f::operator/(double other)
{
	return Vec4f(x / other, y / other, z / other, w / other);
}

Vec4f Vec4f::operator/(float other)
{
	return Vec4f(x / other, y / other, z / other, w / other);
}

Vec4f Vec4f::operator/(int other)
{
	return Vec4f(x / other, y / other, z / other, w / other);
}

Vec4f Vec4f::operator/(Vec4f other)
{
	return Vec4f(x / other.x, y / other.y, z / other.z, w / other.w);
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
