#pragma once
#include "Matrix.h"

class Mat3f : public Matrix
{
public:
	Mat3f();
	~Mat3f();

	//Object and Class Stuff
	const Class* getClass();
	static const Class* myClass;

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

};

