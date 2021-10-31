#include "Mat4f.h"

namespace glib
{
		
	const Class Mat4f::myClass = Class("Mat4f", {&Matrix::myClass});
	const Class* Mat4f::getClass()
	{
		return &Mat4f::myClass;
	}

	Mat4f::Mat4f() : Matrix(4, 4)
	{
	}

	Mat4f::Mat4f(double v1, double v2, double v3, double v4, 
			double v5, double v6, double v7, double v8,
			double v9, double v10, double v11, double v12, 
			double v13, double v14, double v15, double v16) : Matrix(4, 4)
	{
		data[0][0] = v1;
		data[0][1] = v2;
		data[0][2] = v3;
		data[0][3] = v4;
		
		data[1][0] = v5;
		data[1][1] = v6;
		data[1][2] = v7;
		data[1][3] = v8;

		data[2][0] = v9;
		data[2][1] = v10;
		data[2][2] = v11;
		data[2][3] = v12;

		data[3][0] = v13;
		data[3][1] = v14;
		data[3][2] = v15;
		data[3][3] = v16;
	}

	Mat4f::Mat4f(const Mat4f& c)
	{
		this->copy(c);
	}

	void Mat4f::operator=(const Mat4f& o)
	{
		this->copy(o);
	}

	Mat4f::~Mat4f()
	{

	}

	Mat4f Mat4f::getIdentity()
	{
		Mat4f k = Mat4f();
		k[0][0] = 1;
		k[1][1] = 1;
		k[2][2] = 1;
		k[3][3] = 1;

		return k;
	}

	double* Mat4f::operator[](int row)
	{
		return data[row];
	}

	Mat4f Mat4f::operator+(Mat4f other)
	{
		Mat4f v = Mat4f();

		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				v[i][j] = data[i][j] + other[i][j];

		return v;
	}

	Mat4f Mat4f::operator-(Mat4f other)
	{
		Mat4f v = Mat4f();

		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				v[i][j] = data[i][j] - other[i][j];

		return v;
	}

	Mat4f Mat4f::operator*(Mat4f other)
	{
		Mat4f v = Mat4f();

		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				for(int k=0; k < 4; k++)
					v[i][j] += data[i][k] * other[k][j];

		return v;
	}

	Mat4f Mat4f::operator*(double other)
	{
		Mat4f v = Mat4f();

		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				v[i][j] = data[i][j] * other;

		return v;
	}

	void Mat4f::operator*=(double other)
	{
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				data[i][j] *= other;
	}

	void Mat4f::operator+=(Mat4f other)
	{
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				data[i][j] += other[i][j];
	}

	void Mat4f::operator-=(Mat4f other)
	{
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				data[i][j] -= other[i][j];
	}

	Vec4f Mat4f::operator*(Vec4f other)
	{
		Vec4f v = Vec4f();
		v.x = (data[0][0]*other.x) + (data[0][1]*other.y) + (data[0][2]*other.z) + (data[0][3]*other.w);
		v.y = (data[1][0]*other.x) + (data[1][1]*other.y) + (data[1][2]*other.z) + (data[1][3]*other.w);
		v.z = (data[2][0]*other.x) + (data[2][1]*other.y) + (data[2][2]*other.z) + (data[2][3]*other.w);
		v.w = (data[3][0]*other.x) + (data[3][1]*other.y) + (data[3][2]*other.z) + (data[3][3]*other.w);
		
		return v;
	}

	bool Mat4f::operator==(Mat4f other)
	{
		bool same = true;
		for(int y=0; y<4; y++)
		{
			for(int x=0; x<4; x++)
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

	bool Mat4f::operator!=(Mat4f other)
	{
		return !(this->operator==(other));
	}

} //NAMESPACE glib END