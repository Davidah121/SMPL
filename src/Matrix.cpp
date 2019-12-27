#include "Matrix.h"

const Class* Matrix::myClass = new Class("Matrix", {Object::myClass});
const Class* Matrix::getClass()
{
	return Matrix::myClass;
}

Matrix::Matrix()
{
	this->columns = 0;
	this->rows = 0;

	valid = false;
}

Matrix::Matrix(int cols, int rows)
{
	this->columns = cols;
	this->rows = rows;

	if (cols > 0 && rows > 0)
	{
		valid = true;

		data = new double*[rows];
		for (int i = 0; i < rows; i++)
		{
			data[i] = new double[cols];
		}
	}
}

Matrix::Matrix(const Matrix & c)
{
	rows = c.rows;
	columns = c.columns;

	data = new double*[rows];
	for (int i = 0; i < rows; i++)
	{
		data[i] = new double[columns];
	}

	for (int i = 0; i < rows; i++)
	{
		for (int i2 = 0; i2 < columns; i2++)
		{
			data[i][i2] = c.data[i][i2];
		}
	}
}

Matrix::~Matrix()
{
	for (int i = 0; i < rows; i++)
	{
		delete[] data[i];
	}
	delete[] data;
}

double * Matrix::operator[](int row)
{
	return data[row];
}

Matrix Matrix::operator*(double value)
{
	Matrix m = Matrix(rows, columns);

	for (int i = 0; i < rows; i++)
	{
		for (int i2 = 0; i2 < columns; i2++)
		{
			m[i][i2] = value * data[i][i2];
		}
	}

	return m;
}

Matrix Matrix::operator*(int value)
{
	Matrix m = Matrix(rows, columns);

	for (int i = 0; i < rows; i++)
	{
		for (int i2 = 0; i2 < columns; i2++)
		{
			m[i][i2] = value * data[i][i2];
		}
	}

	return m;
}

Matrix Matrix::operator*(float value)
{
	Matrix m = Matrix(rows, columns);

	for (int i = 0; i < rows; i++)
	{
		for (int i2 = 0; i2 < columns; i2++)
		{
			m[i][i2] = value * data[i][i2];
		}
	}

	return m;
}

void Matrix::operator*=(double value)
{
	for (int i = 0; i < rows; i++)
	{
		for (int i2 = 0; i2 < columns; i2++)
		{
			data[i][i2] *= value;
		}
	}
}

void Matrix::operator*=(int value)
{
	for (int i = 0; i < rows; i++)
	{
		for (int i2 = 0; i2 < columns; i2++)
		{
			data[i][i2] *= value;
		}
	}
}

void Matrix::operator*=(float value)
{
	for (int i = 0; i < rows; i++)
	{
		for (int i2 = 0; i2 < columns; i2++)
		{
			data[i][i2] *= value;
		}
	}
}

Matrix Matrix::operator+(Matrix other)
{
	if (rows == other.rows && columns == other.columns)
	{
		Matrix m = Matrix(rows, columns);

		for (int i = 0; i < rows; i++)
		{
			for (int i2 = 0; i2 < columns; i2++)
			{
				m[i][i2] = other[i][i2] + data[i][i2];
			}
		}

		return m;
	}
	else
	{
		return Matrix(0, 0);
	}
}

void Matrix::operator+=(Matrix other)
{
	if (columns == other.columns && rows == other.rows)
	{
		for (int i = 0; i < rows; i++)
		{
			for (int i2 = 0; i2 < columns; i2++)
			{
				data[i][i2] += other[i][i2];
			}
		}
	}
}

Matrix Matrix::operator-(Matrix other)
{
	if (rows == other.rows && columns == other.columns)
	{
		Matrix m = Matrix(rows, columns);

		for (int i = 0; i < rows; i++)
		{
			for (int i2 = 0; i2 < columns; i2++)
			{
				m[i][i2] = other[i][i2] - data[i][i2];
			}
		}

		return m;
	}
	else
	{
		return Matrix(0, 0);
	}
}

void Matrix::operator-=(Matrix other)
{
	if (columns == other.columns && rows == other.rows)
	{
		for (int i = 0; i < rows; i++)
		{
			for (int i2 = 0; i2 < columns; i2++)
			{
				data[i][i2] -= other[i][i2];
			}
		}
	}
}

double** Matrix::getData()
{
	return data;
}

int Matrix::getRows()
{
	return rows;
}

int Matrix::getCols()
{
	return columns;
}

bool Matrix::getValid()
{
	return valid;
}

double Matrix::get(int col, int row)
{
	if (row < rows && col < columns)
		return data[row][col];
	else
		return 0;
}
