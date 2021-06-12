#pragma once
#include "Object.h"
#include "GeneralVector.h"

class Vec2f : public Object
{
public:
	Vec2f(double x, double y);
	Vec2f();
	~Vec2f();

	//Object and Class Stuff
	const Class* getClass();
	static const Class myClass;

	double x = 0;
	double y = 0;

	double getX();
	double getY();

	void setX(double value);
	void setY(double value);

	void setValues(Vec2f other);

	Vec2f operator-(Vec2f other);
	Vec2f operator+(Vec2f other);

	void operator+=(Vec2f other);
	void operator-=(Vec2f other);
	void operator*=(Vec2f other);
	void operator/=(Vec2f other);


	void operator*=(double other);
	void operator/=(double other);

	void operator*=(int other);
	void operator/=(int other);

	void operator*=(long other);
	void operator/=(long other);

	void operator*=(float other);
	void operator/=(float other);

	bool operator==(Vec2f other);
	bool operator!=(Vec2f other);

	Vec2f operator-();

	Vec2f operator*(int value);
	Vec2f operator*(float value);
	Vec2f operator*(double value);
	Vec2f operator*(long value);

	Vec2f operator/(int value);
	Vec2f operator/(float value);
	Vec2f operator/(double value);
	Vec2f operator/(long value);

	//casting to generic vector
	GeneralVector toGeneralVector();
	operator GeneralVector() const;

};

