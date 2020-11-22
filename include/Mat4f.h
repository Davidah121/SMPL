#pragma once
#include "Matrix.h"
#include "Vec4f.h"

class Mat4f : public Matrix
{
public:
	Mat4f();
	Mat4f(const Mat4f& c);
	void operator=(const Mat4f& o);

	~Mat4f();

	//Object and Class Stuff
	const Class* getClass();
	static const Class* myClass;

	static Mat4f getIdentity();

	double* operator[](int row);

	Mat4f operator+(Mat4f other);
	Mat4f operator-(Mat4f other);
	Mat4f operator*(Mat4f other);

	Mat4f operator*(int other);
	Mat4f operator*(float other);
	Mat4f operator*(double other);

	void operator*=(int other);
	void operator*=(float other);
	void operator*=(double other);

	void operator+=(Mat4f other);
	void operator-=(Mat4f other);

	Vec4f operator*(Vec4f other);

};

