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
			v[i][j] = data[i][j] * other[j][i];

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
