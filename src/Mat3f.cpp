#include "Mat3f.h"

namespace smpl
{
		
	const RootClass Mat3f::globalClass = CREATE_ROOT_CLASS(Mat3f, &Matrix::globalClass);
	const RootClass* Mat3f::getClass()
	{
		return &Mat3f::globalClass;
	}
	
	Mat3f::Mat3f() : Matrix(3, 3)
	{
	}

	Mat3f::Mat3f(float v1, float v2, float v3, float v4, float v5, float v6, float v7, float v8, float v9) : Matrix(3,3)
	{
		data[0] = v1;
		data[1] = v2;
		data[2] = v3;
		
		data[3] = v4;
		data[4] = v5;
		data[5] = v6;

		data[6] = v7;
		data[7] = v8;
		data[8] = v9;
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

	float* Mat3f::operator[](int row) const
	{
		return Matrix::operator[](row);
	}

	Mat3f Mat3f::operator+(const Mat3f& other) const
	{
		Mat3f v = Mat3f();

		for (int i = 0; i < 9; i++)
			v.data[i] = data[i] + other.data[i];

		return v;
	}

	Mat3f Mat3f::operator-(const Mat3f& other) const
	{
		Mat3f v = Mat3f();

		for (int i = 0; i < 9; i++)
			v.data[i] = data[i] - other.data[i];

		return v;
	}

	Mat3f Mat3f::operator*(const Mat3f& other) const
	{
		Mat3f v = Mat3f();

		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				for(int k=0; k < 3; k++)
					v[i][j] += data[k + i*3] * other[k][j];

		return v;
	}

	Mat3f Mat3f::operator*(float other) const
	{
		Mat3f v = Mat3f();

		for (int i = 0; i < 9; i++)
			v.data[i] = data[i] * other;

		return v;
	}

	void Mat3f::operator*=(float other)
	{
		for (int i = 0; i < 9; i++)
			data[i] *= other;
	}

	void Mat3f::operator+=(const Mat3f& other)
	{
		for (int i = 0; i < 9; i++)
			data[i] += other.data[i];
	}

	void Mat3f::operator-=(const Mat3f& other)
	{
		for (int i = 0; i < 9; i++)
			data[i] -= other.data[i];
	}

	Vec3f Mat3f::operator*(const Vec3f& other) const
	{
		Vec3f v = Vec3f();
		v.x = data[0] * other.x + data[1]*other.y + data[2]*other.z;
		v.y = data[3] * other.x + data[4]*other.y + data[5]*other.z;
		v.z = data[6] * other.x + data[7]*other.y + data[8]*other.z;
		
		return v;
	}

	bool Mat3f::operator==(const Mat3f& other) const
	{
		return Matrix::operator==(other);
	}

	bool Mat3f::operator!=(const Mat3f& other) const
	{
		return !(this->operator==(other));
	}
} //NAMESPACE glib END