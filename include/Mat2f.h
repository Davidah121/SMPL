#pragma once
#include "Matrix.h"

///A special Matrix that is a 2x2 matrix. Commonly used
class Mat2f : public Matrix
{
public:
	Mat2f();
	
	//Object and Class Stuff
	const Class* getClass();
	static const Class* myClass;

	static Mat2f getIdentity();

	double* operator[](int row);

	Mat2f operator*(double value);
	Mat2f operator*(int value);
	Mat2f operator*(float value);

	void operator*=(double value);
	void operator*=(int value);
	void operator*=(float value);

	Mat2f operator+(Mat2f other);
	void operator+=(Mat2f other);

	Mat2f operator-(Mat2f other);
	void operator-=(Mat2f other);

	Mat2f operator*(Mat2f other);

};

