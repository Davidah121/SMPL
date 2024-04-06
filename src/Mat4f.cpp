#include "Mat4f.h"

namespace smpl
{
		
	const RootClass Mat4f::globalClass = RootClass("Mat4f", {"Matrix"});

	Mat4f::Mat4f() : Matrix(4, 4)
	{
		setClass(globalClass);
	}

	Mat4f::Mat4f(double v1, double v2, double v3, double v4, 
			double v5, double v6, double v7, double v8,
			double v9, double v10, double v11, double v12, 
			double v13, double v14, double v15, double v16) : Matrix(4, 4)
	{
		setClass(globalClass);
		data[0] = v1;
		data[1] = v2;
		data[2] = v3;
		data[3] = v4;
		
		data[4] = v5;
		data[5] = v6;
		data[6] = v7;
		data[7] = v8;

		data[8] = v9;
		data[9] = v10;
		data[10] = v11;
		data[11] = v12;

		data[12] = v13;
		data[13] = v14;
		data[14] = v15;
		data[15] = v16;
	}

	Mat4f::Mat4f(const Mat4f& c)
	{
		this->copy(c);
		setClass(globalClass);
	}

	void Mat4f::operator=(const Mat4f& o)
	{
		this->copy(o);
		setClass(globalClass);
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
	
	Mat4f Mat4f::getTranspose()
	{
		Mat4f k = Mat4f();
		for(int i=0; i<4; i++)
		{
			for(int j=0; j<4; j++)
			{
				k.data[i + j*columns] = data[j + i*columns];
			}
		}

		return k;
	}

	double* Mat4f::operator[](int row)
	{
		return Matrix::operator[](row);
	}

	Mat4f Mat4f::operator+(Mat4f other)
	{
		Mat4f v = Mat4f();

		for (int i = 0; i < 16; i++)
			v.data[i] = data[i] + other.data[i];

		return v;
	}

	Mat4f Mat4f::operator-(Mat4f other)
	{
		Mat4f v = Mat4f();

		for (int i = 0; i < 16; i++)
			v.data[i] = data[i] - other.data[i];

		return v;
	}

	Mat4f Mat4f::operator*(Mat4f other)
	{
		Mat4f v = Mat4f();

		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				for(int k=0; k < 4; k++)
					v.data[j + i*4] += data[k + i*4] * other.data[j + k*4];

		return v;
	}

	Mat4f Mat4f::operator*(double other)
	{
		Mat4f v = Mat4f();

		for (int i = 0; i < 16; i++)
			v.data[i] = data[i] * other;

		return v;
	}

	void Mat4f::operator*=(double other)
	{
		for (int i = 0; i < 16; i++)
			data[i] *= other;
	}

	void Mat4f::operator+=(Mat4f other)
	{
		for (int i = 0; i < 16; i++)
			data[i] += other.data[i];
	}

	void Mat4f::operator-=(Mat4f other)
	{
		for (int i = 0; i < 16; i++)
			data[i] -= other.data[i];
	}

	Vec4f Mat4f::operator*(Vec4f other)
	{
		Vec4f v = Vec4f();
		v.x = (data[0]*other.x) + (data[1]*other.y) + (data[2]*other.z) + (data[3]*other.w);
		v.y = (data[4]*other.x) + (data[5]*other.y) + (data[6]*other.z) + (data[7]*other.w);
		v.z = (data[8]*other.x) + (data[9]*other.y) + (data[10]*other.z) + (data[11]*other.w);
		v.w = (data[12]*other.x) + (data[13]*other.y) + (data[14]*other.z) + (data[15]*other.w);
		
		return v;
	}

	bool Mat4f::operator==(Mat4f other)
	{
		return Matrix::operator==(other);
	}

	bool Mat4f::operator!=(Mat4f other)
	{
		return !(this->operator==(other));
	}

	void Mat4f::fillArray(float* buffer)
	{
		for(int i=0; i<16; i++)
		{
			buffer[i] = (float)data[i];
		}
	}
	void Mat4f::fillArray(double* buffer)
	{
		for(int i=0; i<16; i++)
		{
			buffer[i] = data[i];
		}
	}

} //NAMESPACE glib END