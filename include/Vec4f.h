#pragma once
#include "Vec3f.h"
#include "Vec2f.h"

class Vec4f : public Object
{
public:
	Vec4f();
	Vec4f(float x, float y, float z, float w);
	Vec4f(Vec3f other);
	Vec4f(Vec2f other);

	~Vec4f();

	//Object and Class Stuff
	const Class* getClass();
	static const Class* myClass;

	float x = 0;
	float y = 0;
	float z = 0;
	float w = 0;

	float getX();
	float getY();
	float getZ();
	float getW();

	void setX(float value);
	void setY(float value);
	void setZ(float value);
	void setW(float value);

	void setValues(Vec2f value);
	void setValues(Vec3f value);
	void setValues(Vec4f value);

	Vec4f operator-(Vec4f other);
	Vec4f operator+(Vec4f other);

	Vec4f operator*(long other);
	Vec4f operator*(double other);
	Vec4f operator*(float other);
	Vec4f operator*(int other);

	bool operator==(Vec4f other);
	bool operator!=(Vec4f other);
	Vec4f operator-();

	void operator+=(Vec4f other);
	void operator-=(Vec4f other);
	void operator*=(Vec4f other);
	void operator/=(Vec4f other);

	void operator*=(double other);
	void operator/=(double other);

	void operator*=(int other);
	void operator/=(int other);

	void operator*=(long other);
	void operator/=(long other);

	void operator*=(float other);
	void operator/=(float other);

	Vec4f operator/(long other);
	Vec4f operator/(double other);
	Vec4f operator/(float other);
	Vec4f operator/(int other);

	GeneralVector toGeneralVector();
	operator GeneralVector() const;
};

