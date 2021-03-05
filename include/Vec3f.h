#pragma once
#include "Vec2f.h"
#include "Object.h"

class Vec3f : public Object
{
public:
	Vec3f(float x, float y, float z);
	Vec3f();
	Vec3f(Vec2f other);
	~Vec3f();

	//Object and Class Stuff
	const Class* getClass();
	static const Class* myClass;

	float x = 0;
	float y = 0;
	float z = 0;

	float getX();
	float getY();
	float getZ();

	void setX(float value);
	void setY(float value);
	void setZ(float value);

	void setValues(Vec3f other);

	Vec3f operator-(Vec3f other);
	Vec3f operator+(Vec3f other);
	Vec3f operator-();

	void operator+=(Vec3f other);
	void operator-=(Vec3f other);
	void operator*=(Vec3f other);
	void operator/=(Vec3f other);

	void operator*=(double other);
	void operator/=(double other);

	void operator*=(int other);
	void operator/=(int other);

	void operator*=(long other);
	void operator/=(long other);

	void operator*=(float other);
	void operator/=(float other);

	bool operator==(Vec3f other);
	bool operator!=(Vec3f other);

	Vec3f operator*(int value);
	Vec3f operator*(float value);
	Vec3f operator*(double value);
	Vec3f operator*(long value);

	Vec3f operator/(int value);
	Vec3f operator/(float value);
	Vec3f operator/(double value);
	Vec3f operator/(long value);

	GeneralVector toGeneralVector();
	operator GeneralVector() const;

};

