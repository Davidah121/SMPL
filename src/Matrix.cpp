#include "Matrix.h"
#include "StringTools.h"

const Class Matrix::myClass = Class("Matrix", {&Object::myClass});
const Class* Matrix::getClass()
{
	return &Matrix::myClass;
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
			memset(data[i], 0, sizeof(double)*cols);
		}
	}
}

Matrix::Matrix(const Matrix& c)
{
	this->copy(c);
}

void Matrix::operator=(const Matrix& c)
{
	this->copy(c);
}

void Matrix::copy(const Matrix& c)
{
	if(data!=nullptr)
	{
		//clear first
		for (int i = 0; i < rows; i++)
		{
			if(data[i]!=nullptr)
				delete[] data[i];
		}

		if(data!=nullptr)
			delete[] data;
	}

	valid = c.valid;
	rows = c.rows;
	columns = c.columns;
	
	if(c.data!=nullptr)
	{
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
}

Matrix::~Matrix()
{
	for (int i = 0; i < rows; i++)
	{
		if(data[i]!=nullptr)
			delete[] data[i];
	}

	if(data!=nullptr)
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

Matrix Matrix::operator*(Matrix other)
{
	if(this->getRows() == other.getCols())
	{
		Matrix m = Matrix(this->getCols(), other.getRows());

		for (int i = 0; i < rows; i++)
		{
			for (int i2 = 0; i2 < columns; i2++)
			{
				for(int i3=0; i3<rows; i3++)
				{
					m[i][i2] += data[i][i3] * other[i3][i2];
				}
			}
		}

		return m;
	}

	return Matrix();
}

GeneralVector Matrix::operator*(GeneralVector other)
{
	if(other.getSize() == this->getRows())
	{
		//treat vector as column matrix
		GeneralVector v = GeneralVector(other.getSize());
		for (int i = 0; i < rows; i++)
		{
			for(int i3=0; i3 < rows; i3++)
			{
				v[i] += data[i][i3] * other[i3];
			}
		}

		return v;
	}
	else
	{
		return GeneralVector();
	}
	
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

bool Matrix::operator==(Matrix other)
{
	if(rows != other.rows || columns != other.columns)
	{
		return false;
	}

	bool same = true;
	for(int y=0; y<other.rows; y++)
	{
		for(int x=0; x<other.columns; x++)
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

bool Matrix::operator!=(Matrix other)
{
	if(rows != other.rows || columns != other.columns)
	{
		return true;
	}
	
	bool notSame = true;
	for(int y=0; y<2; y++)
	{
		for(int x=0; x<2; x++)
		{
			if(data[y][x] == other.data[y][x])
			{
				notSame = false;
				break;
			}
		}
		if(notSame==false)
		{
			break;
		}
	}
	return notSame;
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
