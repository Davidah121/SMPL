#include "Mat2f.h"

namespace glib
{

	const Class Mat2f::myClass = Class("Mat2f", {&Matrix::myClass});
	const Class* Mat2f::getClass()
	{
		return &Mat2f::myClass;
	}

	Mat2f::Mat2f() : Matrix(2,2)
	{
	}

	Mat2f::Mat2f(double v1, double v2, double v3, double v4) : Matrix(2,2)
	{
		data[0][0] = v1;
		data[0][1] = v2;
		data[1][0] = v3;
		data[1][1] = v4;
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

	double* Mat2f::operator[](int row)
	{
		return data[row];
	}

	Mat2f Mat2f::operator*(double value)
	{
		Mat2f newMat = Mat2f();
		newMat.data[0][0] = data[0][0] * value;
		newMat.data[1][0] = data[1][0] * value;
		newMat.data[0][1] = data[0][1] * value;
		newMat.data[1][1] = data[1][1] * value;

		return newMat;
	}

	Mat2f Mat2f::operator*(int value)
	{
		Mat2f newMat = Mat2f();
		newMat.data[0][0] = data[0][0] * value;
		newMat.data[1][0] = data[1][0] * value;
		newMat.data[0][1] = data[0][1] * value;
		newMat.data[1][1] = data[1][1] * value;

		return newMat;
	}

	Mat2f Mat2f::operator*(float value)
	{
		Mat2f newMat = Mat2f();
		newMat.data[0][0] = data[0][0] * value;
		newMat.data[1][0] = data[1][0] * value;
		newMat.data[0][1] = data[0][1] * value;
		newMat.data[1][1] = data[1][1] * value;

		return newMat;
	}

	void Mat2f::operator*=(double value)
	{
		data[0][0] *= value;
		data[1][0] *= value;
		data[0][1] *= value;
		data[1][1] *= value;
	}

	void Mat2f::operator*=(int value)
	{
		data[0][0] *= value;
		data[1][0] *= value;
		data[0][1] *= value;
		data[1][1] *= value;
	}

	void Mat2f::operator*=(float value)
	{
		data[0][0] *= value;
		data[1][0] *= value;
		data[0][1] *= value;
		data[1][1] *= value;
	}

	Mat2f Mat2f::operator+(Mat2f other)
	{
		Mat2f newMat = Mat2f();
		newMat.data[0][0] = data[0][0] + other.data[0][0];
		newMat.data[1][0] = data[1][0] + other.data[1][0];
		newMat.data[0][1] = data[0][1] + other.data[0][1];
		newMat.data[1][1] = data[1][1] + other.data[1][1];

		return newMat;
	}

	void Mat2f::operator+=(Mat2f other)
	{
		data[0][0] += other.data[0][0];
		data[1][0] += other.data[1][0];
		data[0][1] += other.data[0][1];
		data[1][1] += other.data[1][1];
	}

	Mat2f Mat2f::operator-(Mat2f other)
	{
		Mat2f newMat = Mat2f();
		newMat.data[0][0] = data[0][0] - other.data[0][0];
		newMat.data[1][0] = data[1][0] - other.data[1][0];
		newMat.data[0][1] = data[0][1] - other.data[0][1];
		newMat.data[1][1] = data[1][1] - other.data[1][1];

		return newMat;
	}

	void Mat2f::operator-=(Mat2f other)
	{
		data[0][0] -= other.data[0][0];
		data[1][0] -= other.data[1][0];
		data[0][1] -= other.data[0][1];
		data[1][1] -= other.data[1][1];
	}

	Mat2f Mat2f::operator*(Mat2f other)
	{
		Mat2f n = Mat2f();
		n[0][0] = data[0][0] * other[0][0] + data[1][0] * other[0][1];
		n[1][0] = data[0][0] * other[1][0] + data[1][0] * other[1][1];

		n[0][1] = data[0][1] * other[0][0] + data[1][1] * other[0][1];
		n[1][1] = data[0][1] * other[1][0] + data[1][1] * other[1][1];

		return n;
	}

	Vec2f Mat2f::operator*(Vec2f other)
	{
		Vec2f v = Vec2f();
		v.x = data[0][0] * other.x + data[0][1]*other.y;
		v.y = data[1][0] * other.x + data[1][1]*other.y;
		
		return v;
	}

	bool Mat2f::operator==(Mat2f other)
	{
		bool same = true;
		for(int y=0; y<2; y++)
		{
			for(int x=0; x<2; x++)
			{
				if(data[y][x] != other.data[y][x])
				{
					same = false;
					break;
				}
			}
			if(same==false)
			{
				break;
			}
		}
		return same;
	}

	bool Mat2f::operator!=(Mat2f other)
	{
		return !(this->operator==(other));
	}

} //NAMESPACE glib END