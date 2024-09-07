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

			data = new float[rows*cols];
			memset(data, 0, sizeof(float)*rows*cols);
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
			data = new float[size];
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

	float * Matrix::operator[](int row)
	{
		return &data[row*columns];
	}

	Matrix Matrix::operator*(float value)
	{
		Matrix m = Matrix(rows, columns);

		#pragma omp parallel for
		for (int i = 0; i < size; i++)
		{
			m.data[i] = value * data[i];
		}

		return m;
	}

	Matrix Matrix::operator*(Matrix other)
	{
		return multiply(other);
	}

	//TODO: Make SIMD????
	GeneralVector Matrix::operator*(GeneralVector other)
	{
		if(other.getSize() == this->getRows())
		{
			//treat vector as column matrix
			GeneralVector v = GeneralVector(other.getSize());
			for (int i = 0; i < rows; i++)
			{
				#pragma omp parallel for private(i)
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

	void Matrix::operator*=(float value)
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

			#pragma omp parallel for
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
			#pragma omp parallel for
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

			#pragma omp parallel for
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
			#pragma omp parallel for
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

	
	Matrix Matrix::getIdentityMatrix(int rows)
	{
		Matrix res = Matrix(rows, rows);
		for(int i=0; i<rows; i++)
		{
			res[i][i] = 1;
		}
		return res;
	}

	float* Matrix::getData()
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

	float Matrix::get(int row, int col)
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

		#if (OPTI==0)
		#pragma omp parallel for
		for (int i = 0; i < rows*columns; i++)
		{
			m.data[i] = other.data[i] * data[i];
		}
		#else
		int simdBound = GET_MATRIX_SIMD_BOUND(rows*columns);
		#pragma omp parallel for
		for (int i=0; i < simdBound; i+=MATRIX_INC_AMOUNT)
		{
			MATRIX_SIMD_DATATYPE aValues = MATRIX_SIMD_LOAD(&data[i]);
			MATRIX_SIMD_DATATYPE bValues = MATRIX_SIMD_LOAD(&other.data[i]);
			MATRIX_SIMD_STORE(&m.data[i], MATRIX_SIMD_MULT(aValues, bValues));
		}
		for(int i=simdBound; i<rows*columns; i++)
		{
			m.data[i] = data[i]*other.data[i];
		}
		#endif

		return m;
	}

	
	Matrix Matrix::inverseHadamardProduct(Matrix other)
	{
		if(rows != other.rows || columns != other.columns)
		{
			return Matrix();
		}

		Matrix m = Matrix(rows, columns);

		#if (OPTI==0)
		#pragma omp parallel for
		for (int i = 0; i < rows*columns; i++)
		{
			m.data[i] = other.data[i] * data[i];
		}
		#else
		int simdBound = GET_MATRIX_SIMD_BOUND(rows*columns);
		#pragma omp parallel for
		for (int i=0; i < simdBound; i+=MATRIX_INC_AMOUNT)
		{
			MATRIX_SIMD_DATATYPE aValues = MATRIX_SIMD_LOAD(&data[i]);
			MATRIX_SIMD_DATATYPE bValues = MATRIX_SIMD_LOAD(&other.data[i]);
			MATRIX_SIMD_STORE(&m.data[i], MATRIX_SIMD_DIV(aValues, bValues));
		}
		for(int i=simdBound; i<rows*columns; i++)
		{
			m.data[i] = data[i]*other.data[i];
		}
		#endif

		return m;
	}

	Matrix Matrix::getInverse()
	{
		float det = getDeterminate();

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
				// float multValue = 1;
				
				#pragma omp parallel for
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
		int totalSize = columns*rows;

		#pragma omp parallel for
		for(int i=0; i<totalSize; i++)
		{
			int y = i / columns;
			int x = i % columns;
			m.data[i] = data[y + x*columns];
		}

		return m;
	}

	float Matrix::getDeterminate()
	{
		if(rows == columns && rows > 1)
		{
			if(columns == 2)
			{
				return (data[0]*data[3]) - (data[1]*data[2]);
			}
			else
			{
				float sumValue = 0;
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
			float* arrP = m.data;

			for(int i=0; i<rows; i++)
			{
				if(i != row)
				{
					float* dataP = arrP;
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
		memset(data, 0, sizeof(float)*rows*columns);
	}

	float sumSIMDRegister(__m128 value)
	{
		const __m128 sseZeroes = _mm_set1_ps(0);
		__m128 simdTempSum = _mm_hadd_ps(value, sseZeroes);
		int temp1 = _mm_extract_ps(simdTempSum, 0);
		int temp2 = _mm_extract_ps(simdTempSum, 1);

		return *((float*)&temp1) + *((float*)&temp2);
	}

	float sumSIMDRegister(__m256 value)
	{
		const __m256 sseZeroes = _mm256_set1_ps(0);
		__m256 simdTempSum = _mm256_hadd_ps(value, sseZeroes);
		
		//trickery. avx doesn't really have a shuffle function across 256 bit lanes for floats
		//Should be okay since it is just moving data around.
		__m256i simdSumAsInts = *((__m256i*)&simdTempSum);
		simdSumAsInts = _mm256_permute4x64_epi64(simdSumAsInts, 0b11011000);

		simdTempSum = *((__m256*)&simdSumAsInts);

		return sumSIMDRegister(_mm256_extractf128_ps(simdTempSum, 0));
	}
	
	Matrix Matrix::multiply(Matrix& A, Matrix& B)
	{
		return A.multiply(B);
	}
	Matrix Matrix::multiplyTranspose(Matrix& A, Matrix& B)
	{
		return A.multiplyTranspose(B);
	}

	Matrix Matrix::multiply(Matrix& B)
	{
		//for large size matricies, do transpose then do matrix multiplication.
		if(valid && B.valid && columns == B.rows)
		{
			if(rows * B.columns > EXCEPTIONALY_LARGE_MATRIX_AREA)
			{
				Matrix BT = B.getTranspose(); //O(N^2)
				return Matrix::multiplyTranspose(BT); //O(N^3) but better cache locality
			}

			Matrix C = Matrix(rows, B.columns);
			int totalSize = rows*B.columns;

			//expose more work to make parallel. only 1 join instead of O(N) joins
			#pragma omp parallel for
			for(int i=0; i<totalSize; i++)
			{
				int y = i / B.columns;
				int x = i % B.columns;
				#if (OPTI == 0)
					float sum = 0;
					for(int k=0; k<columns; k++)
					{
						sum += data[k + y*columns] * B.data[x + k*columns];
					}
					C.data[x + y*columns] = sum;
				#else
					MATRIX_SIMD_DATATYPE simdSum = MATRIX_SIMD_ZERO();
					float finalSum = 0;
					float rowFloats[MATRIX_INC_AMOUNT];
					int k=0;
					int simdBound = GET_MATRIX_SIMD_BOUND(columns);
					while(k < simdBound)
					{
						MATRIX_SIMD_DATATYPE aValues = MATRIX_SIMD_LOAD(&data[k + y*columns]);
						for(int count=0; count<MATRIX_INC_AMOUNT; count++)
						{
							rowFloats[count] = B.data[x + (k+count)*columns];
						}
						MATRIX_SIMD_DATATYPE bValues = MATRIX_SIMD_LOAD(rowFloats);

						simdSum = MATRIX_SIMD_ADD(simdSum, MATRIX_SIMD_MULT(aValues, bValues));
						k += MATRIX_INC_AMOUNT;
					}
					while(k < columns)
					{
						finalSum += data[k + y*columns] * B.data[x + k*B.columns];
						k++;
					}

					finalSum += sumSIMDRegister(simdSum);
					C.data[x + y*columns] = finalSum;
				#endif
			}
			return C;
		}
		return Matrix();
	}
	Matrix Matrix::multiplyTranspose(Matrix& B)
	{
		if(valid && B.valid && columns == B.columns)
		{
			Matrix C = Matrix(rows, B.rows);
			int totalSize = rows*B.rows;

			#pragma omp parallel for
			for(int i=0; i<totalSize; i++)
			{
				int y = i / B.rows;
				int x = i % B.rows;
				#if (OPTI == 0)
					float sum = 0;
					for(int k=0; k<columns; k++)
					{
						sum += data[k + y*columns] * B.data[k + x*columns];
					}
					C.data[x + y*columns] = sum;
				#else
					float finalSum = 0;
					MATRIX_SIMD_DATATYPE simdSum = MATRIX_SIMD_ZERO();
					int k = 0;
					int simdBound = GET_MATRIX_SIMD_BOUND(columns);
					while(k < simdBound)
					{
						MATRIX_SIMD_DATATYPE aValues = MATRIX_SIMD_LOAD(&data[k + y*columns]);
						MATRIX_SIMD_DATATYPE bValues = MATRIX_SIMD_LOAD(&B.data[k + x*columns]);
						simdSum = MATRIX_SIMD_ADD(simdSum, MATRIX_SIMD_MULT(aValues, bValues));
						k+=MATRIX_INC_AMOUNT;
					}
					while(k < columns)
					{
						finalSum += data[k + y*columns] * B.data[k + x*B.columns];
						k++;
					}

					finalSum += sumSIMDRegister(simdSum);
					C.data[x + y*columns] = finalSum;
				#endif
			
			}
			return C;
		}
		return Matrix();
	}

	float Matrix::sum()
	{
		float sum = 0;
		for(int i=0; i<rows*columns; i++)
		{
			sum += data[i];
		}
		return sum;
	}

	Matrix Matrix::horizontalSum()
	{
		Matrix m = Matrix(rows, 1);
		#pragma omp parallel for
		for(int i=0; i<rows; i++)
		{
			float sum = 0;
			for(int j=0; j<columns; j++)
			{
				sum += data[j + i*rows];
			}
			m[i][0] = sum;
		}
		return m;
	}

	Matrix Matrix::verticalSum()
	{
		Matrix m = Matrix(1, columns);
		for(int i=0; i<rows; i++)
		{
			for(int j=0; j<columns; j++)
			{
				m[0][j] += data[j + i*rows];
			}
		}
		return m;
	}

	void Matrix::normalize()
	{
		float sValue = sum();
		if(sValue != 0)
		{
			float adjustment = 1.0/sValue;
			operator*=(adjustment);
		}
	}

} //NAMESPACE glib END