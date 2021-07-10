#pragma once
#include "Vec3f.h"
#include "Vec2f.h"

class Vec4f : public Object
{
public:
	Vec4f();
	Vec4f(double x, double y, double z, double w);
	Vec4f(Vec3f other);
	Vec4f(Vec2f other);

	~Vec4f();

	//Object and Class Stuff
	const Class* getClass();
	static const Class myClass;

	double x = 0;
	double y = 0;
	double z = 0;
	double w = 0;

	double getX();
	double getY();
	double getZ();
	double getW();

	void setX(double value);
	void setY(double value);
	void setZ(double value);
	void setW(double value);

	void setValues(Vec2f value);
	void setValues(Vec3f value);
	void setValues(Vec4f value);

	Vec4f operator-(Vec4f other);
	Vec4f operator+(Vec4f other);

	Vec4f operator*(long other);
	Vec4f operator*(double other);
	Vec4f operator*(float other);
	Vec4f operator*(int other);
	Vec4f operator*(Vec4f other);

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
	Vec4f operator/(Vec4f other);

	GeneralVector toGeneralVector();
	operator GeneralVector() const;
};

