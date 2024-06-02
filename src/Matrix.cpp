#include "Matrix.h"
#include "StringTools.h"

namespace smpl
{

	const RootClass Matrix::globalClass = CREATE_ROOT_CLASS(Matrix, &Object::globalClass);
	const RootClass* Matrix::getClass()
	{
		return &Matrix::globalClass;
	}

	Matrix::Matrix()
	{
		this->columns = 0;
		this->rows = 0;
		this->size = 0;
		data = nullptr;

		valid = false;
	}

	Matrix::Matrix(int rows, int cols)
	{
		this->columns = cols;
		this->rows = rows;
		this->size = rows*cols;

		if (cols > 0 && rows > 0)
		{
			valid = true;

			data = new double[rows*cols];
			memset(data, 0, sizeof(double)*rows*cols);
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
			if(data!=nullptr)
				delete[] data;
			data = nullptr;
		}

		valid = c.valid;
		rows = c.rows;
		columns = c.columns;
		this->size = rows*columns;
		
		if(c.data!=nullptr)
		{
			data = new double[size];
			for (int i = 0; i < size; i++)
			{
				data[i] = c.data[i];
			}
		}
	}

	Matrix::~Matrix()
	{
		if(valid)
		{
			if(data!=nullptr)
				delete[] data;
			data = nullptr;
		}
		rows = 0;
		columns = 0;
		size = 0;
		valid = false;
	}

	double * Matrix::operator[](int row)
	{
		return &data[row*columns];
	}

	Matrix Matrix::operator*(double value)
	{
		Matrix m = Matrix(rows, columns);

		for (int i = 0; i < size; i++)
		{
			m.data[i] = value * data[i];
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
						m.data[i2 + i*columns] += data[i3 + i*columns] * other.data[i2 + i3*columns];
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
					v[i] += data[i3 + i*columns] * other[i3];
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
		for (int i = 0; i < size; i++)
		{
			data[i] *= value;
		}
	}

	Matrix Matrix::operator+(Matrix other)
	{
		if (rows == other.rows && columns == other.columns)
		{
			Matrix m = Matrix(rows, columns);

			for (int i = 0; i < size; i++)
			{
					m.data[i] = other.data[i] + data[i];
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
			for (int i = 0; i < size; i++)
			{
				data[i] += other.data[i];
			}
		}
	}

	Matrix Matrix::operator-(Matrix other)
	{
		if (rows == other.rows && columns == other.columns)
		{
			Matrix m = Matrix(rows, columns);

			for (int i = 0; i < size; i++)
			{
				m.data[i] = other.data[i] - data[i];
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
			for (int i = 0; i < size; i++)
			{
				data[i] -= other.data[i];
			}
		}
	}

	bool Matrix::operator==(Matrix other)
	{
		if(rows != other.rows || columns != other.columns)
		{
			return false;
		}

		for(int i=0; i<size; i++)
		{
			if(data[i] != other.data[i])
			{
				return false;
			}
		}
		return true;
	}

	bool Matrix::operator!=(Matrix other)
	{
		return !(this->operator==(other));
	}

	double* Matrix::getData()
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
			return data[col + row*columns];
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
			m.data[i] = other.data[i] * data[i];
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
				inverse.data[0] = data[3];
				inverse.data[1] = -data[1];
				inverse.data[2] = -data[2];
				inverse.data[3] = data[0];
				
				return inverse*(1.0/det);
			}
			else
			{
				// double multValue = 1;
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
				m.data[i + j*rows] = data[j + i*columns];
			}
		}

		return m;
	}

	double Matrix::getDeterminate()
	{
		if(rows == columns && rows > 1)
		{
			if(columns == 2)
			{
				return (data[0]*data[3]) - (data[1]*data[2]);
			}
			else
			{
				double sumValue = 0;
				for(int i=0; i<columns; i++)
				{
					if(i%2 == 0)
					{
						sumValue += data[i] * getMatrixOfMinors(0, i).getDeterminate();
					}
					else
					{
						sumValue -= data[i] * getMatrixOfMinors(0, i).getDeterminate();
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
			double* arrP = m.data;

			for(int i=0; i<rows; i++)
			{
				if(i != row)
				{
					double* dataP = arrP;
					for(int j=0; j<columns; j++)
					{
						if(j != col)
						{
							*(dataP) = data[j + i*columns];
							dataP++;
						}
					}
					arrP+=m.columns;
				}
			}
		}

		return m;
	}

	void Matrix::clear()
	{
		memset(data, 0, sizeof(double)*rows*columns);
	}

} //NAMESPACE glib END