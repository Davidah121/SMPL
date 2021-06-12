#pragma once
#include "Vec2f.h"
#include "Object.h"

class Vec3f : public Object
{
public:
	Vec3f(double x, double y, double z);
	Vec3f();
	Vec3f(Vec2f other);
	~Vec3f();

	//Object and Class Stuff
	const Class* getClass();
	static const Class myClass;

	double x = 0;
	double y = 0;
	double z = 0;

	double getX();
	double getY();
	double getZ();

	void setX(double value);
	void setY(double value);
	void setZ(double value);

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

