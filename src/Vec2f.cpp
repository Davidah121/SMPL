#include "Vec2f.h"

const Class Vec2f::myClass = Class("Vec2f", {&Object::myClass});
const Class* Vec2f::getClass()
{
	return &Vec2f::myClass;
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

void Vec2f::operator*=(Vec2f other)
{
	x *= other.x;
	y *= other.y;
}

void Vec2f::operator/=(Vec2f other)
{
	x /= other.x;
	y /= other.y;
}

void Vec2f::operator*=(double other)
{
	x *= other;
	y *= other;
}
void Vec2f::operator*=(long other)
{
	x *= other;
	y *= other;
}
void Vec2f::operator*=(int other)
{
	x *= other;
	y *= other;
}
void Vec2f::operator*=(float other)
{
	x *= other;
	y *= other;
}

void Vec2f::operator/=(double other)
{
	x /= other;
	y /= other;
}
void Vec2f::operator/=(long other)
{
	x /= other;
	y /= other;
}
void Vec2f::operator/=(int other)
{
	x /= other;
	y /= other;
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

Vec2f Vec2f::operator*(int value)
{
	return Vec2f(x*value, y*value);
}

Vec2f Vec2f::operator*(float value)
{
	return Vec2f(x*value, y*value);
}

Vec2f Vec2f::operator*(double value)
{
	return Vec2f(x*value, y*value);
}

Vec2f Vec2f::operator*(long value)
{
	return Vec2f(x*value, y*value);
}

Vec2f Vec2f::operator*(Vec2f value)
{
	return Vec2f(x * value.x, y * value.y);
}

Vec2f Vec2f::operator/(int value)
{
	return Vec2f(x/value, y/value);
}

Vec2f Vec2f::operator/(float value)
{
	return Vec2f(x / value, y / value);
}

Vec2f Vec2f::operator/(double value)
{
	return Vec2f(x / value, y / value);
}

Vec2f Vec2f::operator/(long value)
{
	return Vec2f(x / value, y / value);
}

Vec2f Vec2f::operator/(Vec2f value)
{
	return Vec2f(x / value.x, y / value.y);
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
