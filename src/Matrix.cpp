#include "Matrix.h"
#include "StringTools.h"

namespace glib
{

	const Class Matrix::myClass = Class("Matrix", {&Object::myClass});
	const Class* Matrix::getClass()
	{
		return &Matrix::myClass;
	}

	Matrix::Matrix()
	{
		this->columns = 0;
		this->rows = 0;
		data = nullptr;

		valid = false;
	}

	Matrix::Matrix(int rows, int cols)
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
		if(valid)
		{
			for (int i = 0; i < rows; i++)
			{
				if(data[i]!=nullptr)
					delete[] data[i];
			}

			if(data!=nullptr)
				delete[] data;
		}
		
		valid = false;
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

	Matrix Matrix::operator*(Matrix other)
	{
		if(this->getCols() == other.getRows())
		{
			int nRows = this->getRows();
			int nCols = other.getCols();

			Matrix m = Matrix(nRows, nCols);

			for (int i = 0; i < nRows; i++)
			{
				for (int i2 = 0; i2 < nCols; i2++)
				{
					for(int i3 = 0; i3 < columns; i3++)
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

	Matrix Matrix::hadamardProduct(Matrix other)
	{
		if(rows != other.rows || columns != other.columns)
		{
			return Matrix();
		}

		Matrix m = Matrix(rows, columns);

		for (int i = 0; i < rows; i++)
		{
			for (int i2 = 0; i2 < columns; i2++)
			{
				m[i][i2] = other.data[i][i2] * data[i][i2];
			}
		}

		return m;
	}

	Matrix Matrix::getInverse()
	{
		double det = getDeterminate();

		if(det!=0 && det!=NAN)
		{
			Matrix inverse = Matrix(rows, columns);
			
			if(rows==2 && columns==2)
			{
				inverse[0][0] = data[1][1];
				inverse[1][1] = data[0][0];
				inverse[1][0] = -data[1][0];
				inverse[0][1] = -data[0][1];
				
				return inverse*(1.0/det);
			}
			else
			{
				double multValue = 1;
				for(int i=0; i<rows; i++)
				{
					for(int j=0; j<columns; j++)
					{
						inverse[i][j] = getMatrixOfMinors(i, j).getDeterminate();

						if((i+j) % 2 == 1)
						{
							inverse[i][j] *= -1;
						}
					}
				}

				return inverse.getTranspose() * (1.0/det);
			}
		}

		return Matrix();
	}

	Matrix Matrix::getTranspose()
	{
		Matrix m = Matrix(columns, rows);

		for(int i=0; i<rows; i++)
		{
			for(int j=0; j<columns; j++)
			{
				m[j][i] = data[i][j];
			}
		}

		return m;
	}

	double Matrix::getDeterminate()
	{
		if(rows == columns && rows > 1)
		{
			if(rows == 2)
			{
				return (data[0][0]*data[1][1]) - (data[0][1]*data[1][0]);
			}
			else
			{
				double sumValue = 0;
				for(int i=0; i<rows; i++)
				{
					if(i%2 == 0)
					{
						sumValue += data[i][0] * getMatrixOfMinors(i, 0).getDeterminate();
					}
					else
					{
						sumValue -= data[i][0] * getMatrixOfMinors(i, 0).getDeterminate();
					}
				}
				return sumValue;
			}
		}

		return NAN;
	}

	Matrix Matrix::getMatrixOfMinors(int row, int col)
	{
		Matrix m = Matrix(rows-1, columns-1);
		
		if(m.getValid() && getValid())
		{
			double** arrP = m.data;

			for(int i=0; i<rows; i++)
			{
				if(i != row)
				{
					double* dataP = *arrP;
					for(int j=0; j<columns; j++)
					{
						if(j != col)
						{
							*(dataP) = data[i][j];
							dataP++;
						}
					}
					arrP++;
				}
			}
		}

		return m;
	}

	void Matrix::clear()
	{
		for(int i=0; i<rows; i++)
		{
			for(int j=0; j<columns; j++)
			{
				data[i][j] = 0;
			}
		}
	}

} //NAMESPACE glib END