#pragma once
#include "Matrix.h"
#include "Vec2f.h"

///A special Matrix that is a 2x2 matrix. Commonly used
class Mat2f : public Matrix
{
public:
	Mat2f();
	Mat2f(const Mat2f& c);
	void operator=(const Mat2f& o);

	~Mat2f();
	
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

	Vec2f operator*(Vec2f other);

	bool operator==(Mat2f other);
	bool operator!=(Mat2f other);
};

