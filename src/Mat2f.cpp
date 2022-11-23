#include "Mat2f.h"

namespace glib
{

	const Class Mat2f::globalClass = Class("Mat2f", {&Matrix::globalClass});

	Mat2f::Mat2f() : Matrix(2,2)
	{
		setClass(globalClass);
	}

	Mat2f::Mat2f(double v1, double v2, double v3, double v4) : Matrix(2,2)
	{
		setClass(globalClass);
		data[0] = v1;
		data[1] = v2;
		data[2] = v3;
		data[3] = v4;
	}

	Mat2f::Mat2f(const Mat2f& c)
	{
		this->copy(c);
		setClass(globalClass);
	}

	void Mat2f::operator=(const Mat2f& o)
	{
		this->copy(o);
		setClass(globalClass);
	}

	Mat2f::~Mat2f()
	{

	}

	Mat2f Mat2f::getIdentity()
	{
		Mat2f k = Mat2f();
		k[0][0] = 1;
		k[1][1] = 1;

		return k;
	}

	double* Mat2f::operator[](int row)
	{
		return Matrix::operator[](row);
	}

	Mat2f Mat2f::operator*(double value)
	{
		Mat2f v = Mat2f();

		for (int i = 0; i < 4; i++)
			v.data[i] = data[i] * value;

		return v;
	}

	void Mat2f::operator*=(double value)
	{
		for (int i = 0; i < 2; i++)
			data[i] *= value;
	}

	Mat2f Mat2f::operator+(Mat2f other)
	{
		Mat2f v = Mat2f();

		for (int i = 0; i < 4; i++)
			v.data[i] = data[i] + other.data[i];

		return v;
	}

	void Mat2f::operator+=(Mat2f other)
	{
		for (int i = 0; i < 2; i++)
			data[i] += other.data[i];
	}

	Mat2f Mat2f::operator-(Mat2f other)
	{
		Mat2f v = Mat2f();

		for (int i = 0; i < 4; i++)
			v.data[i] = data[i] - other.data[i];

		return v;
	}

	void Mat2f::operator-=(Mat2f other)
	{
		for (int i = 0; i < 2; i++)
			data[i] -= other.data[i];
	}

	Mat2f Mat2f::operator*(Mat2f other)
	{
		Mat2f n = Mat2f();
		n[0][0] = data[0] * other.data[0] + data[1] * other.data[2];
		n[0][1] = data[0] * other.data[1] + data[1] * other.data[3];

		n[1][0] = data[2] * other.data[0] + data[3] * other.data[2];
		n[1][1] = data[2] * other.data[1] + data[3] * other.data[3];

		return n;
	}

	Vec2f Mat2f::operator*(Vec2f other)
	{
		Vec2f v = Vec2f();
		v.x = data[0] * other.x + data[1]*other.y;
		v.y = data[2] * other.x + data[3]*other.y;
		
		return v;
	}

	bool Mat2f::operator==(Mat2f other)
	{
		return Matrix::operator==(other);
	}

	bool Mat2f::operator!=(Mat2f other)
	{
		return !(this->operator==(other));
	}

	void Mat2f::fillArray(float* buffer)
	{
		for(int i=0; i<4; i++)
		{
			buffer[i] = (float)data[i];
		}
	}
	void Mat2f::fillArray(double* buffer)
	{
		for(int i=0; i<4; i++)
		{
			buffer[i] = data[i];
		}
	}

} //NAMESPACE glib END