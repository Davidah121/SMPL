#include "Mat2f.h"

namespace smpl
{
	Mat2f::Mat2f() : Matrix(2,2)
	{
	}

	Mat2f::Mat2f(float v1, float v2, float v3, float v4) : Matrix(2,2)
	{
		data[0] = v1;
		data[1] = v2;
		data[2] = v3;
		data[3] = v4;
	}

	Mat2f::Mat2f(const Mat2f& c)
	{
		this->copy(c);
	}

	void Mat2f::operator=(const Mat2f& o)
	{
		this->copy(o);
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

	float* Mat2f::operator[](int row) const
	{
		return Matrix::operator[](row);
	}

	Mat2f Mat2f::operator*(float value) const
	{
		Mat2f v = Mat2f();

		for (int i = 0; i < 4; i++)
			v.data[i] = data[i] * value;

		return v;
	}

	void Mat2f::operator*=(float value)
	{
		for (int i = 0; i < 2; i++)
			data[i] *= value;
	}

	Mat2f Mat2f::operator+(const Mat2f& other) const
	{
		Mat2f v = Mat2f();

		for (int i = 0; i < 4; i++)
			v.data[i] = data[i] + other.data[i];

		return v;
	}

	void Mat2f::operator+=(const Mat2f& other)
	{
		for (int i = 0; i < 2; i++)
			data[i] += other.data[i];
	}

	Mat2f Mat2f::operator-(const Mat2f& other) const
	{
		Mat2f v = Mat2f();

		for (int i = 0; i < 4; i++)
			v.data[i] = data[i] - other.data[i];

		return v;
	}

	void Mat2f::operator-=(const Mat2f& other)
	{
		for (int i = 0; i < 2; i++)
			data[i] -= other.data[i];
	}

	Mat2f Mat2f::operator*(const Mat2f& other) const
	{
		Mat2f n = Mat2f();
		n[0][0] = data[0] * other.data[0] + data[1] * other.data[2];
		n[0][1] = data[0] * other.data[1] + data[1] * other.data[3];

		n[1][0] = data[2] * other.data[0] + data[3] * other.data[2];
		n[1][1] = data[2] * other.data[1] + data[3] * other.data[3];

		return n;
	}

	Vec2f Mat2f::operator*(const Vec2f& other) const
	{
		Vec2f v = Vec2f();
		v.x = data[0] * other.x + data[1]*other.y;
		v.y = data[2] * other.x + data[3]*other.y;
		
		return v;
	}

	bool Mat2f::operator==(const Mat2f& other) const
	{
		return Matrix::operator==(other);
	}

	bool Mat2f::operator!=(const Mat2f& other) const
	{
		return !(this->operator==(other));
	}

} //NAMESPACE glib END