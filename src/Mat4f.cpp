#include "Mat4f.h"

const Class* Mat4f::myClass = new Class("Mat4f", {Matrix::myClass});
const Class* Mat4f::getClass()
{
	return Mat4f::myClass;
}

Mat4f::Mat4f() : Matrix(4, 4)
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
			v[i][j] = data[i][j] * other[j][i];

	return v;
}

Mat4f Mat4f::operator*(int other)
{
	Mat4f v = Mat4f();

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			v[i][j] = data[i][j] * other;

	return v;
}

Mat4f Mat4f::operator*(float other)
{
	Mat4f v = Mat4f();

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			v[i][j] = data[i][j] * other;

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

void Mat4f::operator*=(int other)
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			data[i][j] *= other;
}

void Mat4f::operator*=(float other)
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			data[i][j] *= other;
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
