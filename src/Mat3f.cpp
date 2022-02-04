#include "Mat3f.h"

namespace glib
{
		
	const Class Mat3f::myClass = Class("Mat3f", {&Matrix::myClass});
	const Class* Mat3f::getClass()
	{
		return &Mat3f::myClass;
	}

	Mat3f::Mat3f() : Matrix(3, 3)
	{
	}

	Mat3f::Mat3f(double v1, double v2, double v3, double v4, double v5, double v6, double v7, double v8, double v9) : Matrix(3,3)
	{
		data[0][0] = v1;
		data[0][1] = v2;
		data[0][2] = v3;
		
		data[1][0] = v4;
		data[1][1] = v5;
		data[1][2] = v6;

		data[2][0] = v7;
		data[2][1] = v8;
		data[2][2] = v9;
	}

	Mat3f::~Mat3f()
	{

	}

	Mat3f::Mat3f(const Mat3f& c)
	{
		this->copy(c);
	}

	void Mat3f::operator=(const Mat3f& o)
	{
		this->copy(o);
	}

	Mat3f Mat3f::getIdentity()
	{
		Mat3f k = Mat3f();
		k[0][0] = 1;
		k[1][1] = 1;
		k[2][2] = 1;

		return k;
	}

	double* Mat3f::operator[](int row)
	{
		return data[row];
	}

	Mat3f Mat3f::operator+(Mat3f other)
	{
		Mat3f v = Mat3f();

		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				v[i][j] = data[i][j] + other[i][j];

		return v;
	}

	Mat3f Mat3f::operator-(Mat3f other)
	{
		Mat3f v = Mat3f();

		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				v[i][j] = data[i][j] - other[i][j];

		return v;
	}

	Mat3f Mat3f::operator*(Mat3f other)
	{
		Mat3f v = Mat3f();

		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				for(int k=0; k < 3; k++)
					v[i][j] += data[i][k] * other[k][j];

		return v;
	}

	Mat3f Mat3f::operator*(double other)
	{
		Mat3f v = Mat3f();

		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				v[i][j] = data[i][j] * other;

		return v;
	}

	void Mat3f::operator*=(double other)
	{
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				data[i][j] *= other;
	}

	void Mat3f::operator+=(Mat3f other)
	{
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				data[i][j] += other[i][j];
	}

	void Mat3f::operator-=(Mat3f other)
	{
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				data[i][j] -= other[i][j];
	}

	Vec3f Mat3f::operator*(Vec3f other)
	{
		Vec3f v = Vec3f();
		v.x = data[0][0] * other.x + data[0][1]*other.y + data[0][2]*other.z;
		v.y = data[1][0] * other.x + data[1][1]*other.y + data[1][2]*other.z;
		v.z = data[2][0] * other.x + data[2][1]*other.y + data[2][2]*other.z;
		
		return v;
	}

	bool Mat3f::operator==(Mat3f other)
	{
		bool same = true;
		for(int y=0; y<3; y++)
		{
			for(int x=0; x<3; x++)
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

	bool Mat3f::operator!=(Mat3f other)
	{
		return !(this->operator==(other));
	}

	void Mat3f::fillArray(float* buffer)
	{
		buffer[0] = (float)data[0][0];
		buffer[1] = (float)data[0][1];
		buffer[2] = (float)data[0][2];

		buffer[3] = (float)data[1][0];
		buffer[4] = (float)data[1][1];
		buffer[5] = (float)data[1][2];
		
		buffer[6] = (float)data[2][0];
		buffer[7] = (float)data[2][1];
		buffer[8] = (float)data[2][2];
	}
	void Mat3f::fillArray(double* buffer)
	{
		buffer[0] = data[0][0];
		buffer[1] = data[0][1];
		buffer[2] = data[0][2];

		buffer[3] = data[1][0];
		buffer[4] = data[1][1];
		buffer[5] = data[1][2];
		
		buffer[6] = data[2][0];
		buffer[7] = data[2][1];
		buffer[8] = data[2][2];
	}

	float* Mat3f::convertToFloatArray()
	{
		return new float[9]{
			(float)data[0][0], (float)data[0][1], (float)data[0][2],
			(float)data[1][0], (float)data[1][1], (float)data[1][2],
			(float)data[2][0], (float)data[2][1], (float)data[2][2]
		};
	}
	double* Mat3f::convertToDoubleArray()
	{
		return new double[9]{
			data[0][0], data[0][1], data[0][2],
			data[1][0], data[1][1], data[1][2],
			data[2][0], data[2][1], data[2][2]
		};
	}

} //NAMESPACE glib END