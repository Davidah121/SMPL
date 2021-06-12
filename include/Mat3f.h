#pragma once
#include "Matrix.h"
#include "Vec3f.h"

class Mat3f : public Matrix
{
public:
	Mat3f();
	Mat3f(double v1, double v2, double v3, double v4, double v5, double v6, double v7, double v8, double v9);
	~Mat3f();

	Mat3f(const Mat3f& c);
	void operator=(const Mat3f& o);

	//Object and Class Stuff
	const Class* getClass();
	static const Class myClass;

	static Mat3f getIdentity();

	double* operator[](int row);

	Mat3f operator+(Mat3f other);
	Mat3f operator-(Mat3f other);
	Mat3f operator*(Mat3f other);

	Mat3f operator*(int other);
	Mat3f operator*(float other);
	Mat3f operator*(double other);

	void operator*=(int other);
	void operator*=(float other);
	void operator*=(double other);

	void operator+=(Mat3f other);
	void operator-=(Mat3f other);

	Vec3f operator*(Vec3f other);

	bool operator==(Mat3f other);
	bool operator!=(Mat3f other);
};

