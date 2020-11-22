#include "Mat3f.h"

const Class* Mat3f::myClass = new Class("Mat3f", {Matrix::myClass});
const Class* Mat3f::getClass()
{
	return Mat3f::myClass;
}

Mat3f::Mat3f() : Matrix(3, 3)
{
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

Mat3f Mat3f::operator*(int other)
{
	Mat3f v = Mat3f();

	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			v[i][j] = data[i][j]*other;

	return v;
}

Mat3f Mat3f::operator*(float other)
{
	Mat3f v = Mat3f();

	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			v[i][j] = data[i][j] * other;

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

void Mat3f::operator*=(int other)
{
	for(int i=0; i<3; i++)
		for(int j=0; j<3; j++)
			data[i][j] *= other;
}

void Mat3f::operator*=(float other)
{
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			data[i][j] *= other;
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