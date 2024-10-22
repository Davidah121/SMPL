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
			for (size_t i = 0; i < size; i++)
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

	float * Matrix::operator[](int row) const
	{
		return &data[row*columns];
	}

	Matrix Matrix::operator*(float value) const
	{
		Matrix m = Matrix(rows, columns);

		#pragma omp parallel for
		for (size_t i = 0; i < size; i++)
		{
			m.data[i] = value * data[i];
		}

		return m;
	}

	Matrix Matrix::operator*(const Matrix& other) const
	{
		return multiply(other);
	}

	void Matrix::operator*=(float value)
	{
		for (size_t i = 0; i < size; i++)
		{
			data[i] *= value;
		}
	}

	Matrix Matrix::operator+(const Matrix& other) const
	{
		if (rows == other.rows && columns == other.columns)
		{
			Matrix m = Matrix(rows, columns);

			#pragma omp parallel for
			for (size_t i = 0; i < size; i++)
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

	void Matrix::operator+=(const Matrix& other)
	{
		if (columns == other.columns && rows == other.rows)
		{
			#pragma omp parallel for
			for (size_t i = 0; i < size; i++)
			{
				data[i] += other.data[i];
			}
		}
	}

	Matrix Matrix::operator-(const Matrix& other) const
	{
		if (rows == other.rows && columns == other.columns)
		{
			Matrix m = Matrix(rows, columns);

			#pragma omp parallel for
			for (size_t i = 0; i < size; i++)
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

	void Matrix::operator-=(const Matrix& other)
	{
		if (columns == other.columns && rows == other.rows)
		{
			#pragma omp parallel for
			for (size_t i = 0; i < size; i++)
			{
				data[i] -= other.data[i];
			}
		}
	}

	bool Matrix::operator==(const Matrix& other) const
	{
		if(rows != other.rows || columns != other.columns)
		{
			return false;
		}

		for(size_t i=0; i<size; i++)
		{
			if(data[i] != other.data[i])
			{
				return false;
			}
		}
		return true;
	}

	bool Matrix::operator!=(const Matrix& other) const
	{
		return !(this->operator==(other));
	}

	
	Matrix Matrix::getIdentityMatrix(int rows)
	{
		Matrix res = Matrix(rows, rows);
		for(size_t i=0; i<rows; i++)
		{
			res[i][i] = 1;
		}
		return res;
	}

	
	void Matrix::setAllValues(float v)
	{
		#if (OPTI==0)
		#pragma omp parallel for
		for (size_t i = 0; i < rows*columns; i++)
		{
			data[i] = v;
		}
		#else
		size_t simdBound = SIMD_FP32::getSIMDBound(rows*columns);
		SIMD_FP32 setValues = v;
		#pragma omp parallel for
		for (size_t i=0; i < simdBound; i+=SIMD_FP32::SIZE)
		{
			setValues.store(&data[i]);
		}
		for(size_t i=simdBound; i<rows*columns; i++)
		{
			data[i] = v;
		}
		#endif
	}

	
	Matrix Matrix::broadcastAdd(float v)
	{
		Matrix result = Matrix(rows, columns);
		size_t size = rows*columns;
		#if (OPTI == 0)
			#pragma omp parallel for
			for(size_t i=0; i<size; i++)
				result.data[i] = data[i] + v;
		#else
			size_t simdSize = SIMD_FP32::getSIMDBound(size);
			SIMD_FP32 addV = v;
			#pragma omp parallel for
			for(size_t i=0; i<simdSize; i+=SIMD_FP32::SIZE)
			{
				SIMD_FP32 v = SIMD_FP32::load(&data[i]);
				v += addV;
				v.store(&result.data[i]);
			}
			for(size_t i=simdSize; i<size; i++)
				result.data[i] = data[i] + v;
		#endif
		return result;
	}
	
	Matrix Matrix::broadcastAdd(const Matrix& B)
	{
		if(B.rows != 1 && B.columns != 1)
			return Matrix(); //throw an exception lol
		Matrix result = Matrix(rows, columns);
		size_t size = rows*columns;
		
		if(B.rows != 1)
		{
			//broadcast about the rows
			#if (OPTI == 0)
				#pragma omp parallel for
				for(size_t i=0; i<size; i++)
				{
					result.data[i] = data[i] + B.data[i/rows];
				}
			#else
				#pragma omp parallel for
				for(size_t i=0; i<rows; i++)
				{
					size_t simdSize = SIMD_FP32::getSIMDBound(columns);
					SIMD_FP32 addV = B.data[i];
					for(size_t j=0; j<simdSize; j+=SIMD_FP32::SIZE)
					{
						SIMD_FP32 v = SIMD_FP32::load(&data[j + i*columns]);
						v += addV;
						v.store(&result.data[i]);
					}
					for(size_t j=simdSize; j<columns; j++)
					{
						result.data[j + i*columns] = data[j + i*columns] + B.data[i];
					}
				}
			#endif
		}
		else
		{
			//broadcast about the columns
			#if (OPTI == 0)
				#pragma omp parallel for
				for(size_t i=0; i<size; i++)
				{
					result.data[i] = data[i] + B.data[i%rows];
				}
			#else
				#pragma omp parallel for
				for(size_t i=0; i<rows; i++)
				{
					size_t simdSize = SIMD_FP32::getSIMDBound(columns);
					for(size_t j=0; j<simdSize; j+=SIMD_FP32::SIZE)
					{
						SIMD_FP32 v = SIMD_FP32::load(&data[j + i*columns]);
						SIMD_FP32 addV = SIMD_FP32::load(&B.data[j]);
						v += addV;
						v.store(&result.data[i]);
					}
					for(size_t j=simdSize; j<columns; j++)
					{
						result.data[j + i*columns] = data[j + i*columns] + B.data[j];
					}
				}
			#endif
		}
		return result;
	}

	Matrix Matrix::broadcastSubtract(float v)
	{
		Matrix result = Matrix(rows, columns);
		size_t size = rows*columns;
		#if (OPTI == 0)
			#pragma omp parallel for
			for(size_t i=0; i<size; i++)
				result.data[i] = data[i] - v;
		#else
			size_t simdSize = SIMD_FP32::getSIMDBound(size);
			SIMD_FP32 addV = v;
			#pragma omp parallel for
			for(size_t i=0; i<simdSize; i+=SIMD_FP32::SIZE)
			{
				SIMD_FP32 v = SIMD_FP32::load(&data[i]);
				v -= addV;
				v.store(&result.data[i]);
			}
			for(size_t i=simdSize; i<size; i++)
				result.data[i] = data[i] - v;
		#endif
		return result;
	}
	Matrix Matrix::broadcastSubtract(const Matrix& B)
	{
		if(B.rows != 1 && B.columns != 1)
			return Matrix(); //throw an exception lol
		Matrix result = Matrix(rows, columns);
		size_t size = rows*columns;
		
		if(B.rows != 1)
		{
			//broadcast about the rows
			#if (OPTI == 0)
				#pragma omp parallel for
				for(size_t i=0; i<size; i++)
				{
					result.data[i] = data[i] - B.data[i/rows];
				}
			#else
				#pragma omp parallel for
				for(size_t i=0; i<rows; i++)
				{
					size_t simdSize = SIMD_FP32::getSIMDBound(columns);
					SIMD_FP32 subV = B.data[i];
					for(size_t j=0; j<simdSize; j+=SIMD_FP32::SIZE)
					{
						SIMD_FP32 v = SIMD_FP32::load(&data[j + i*columns]);
						v -= subV;
						v.store(&result.data[i]);
					}
					for(size_t j=simdSize; j<columns; j++)
					{
						result.data[j + i*columns] = data[j + i*columns] - B.data[i];
					}
				}
			#endif
		}
		else
		{
			//broadcast about the columns
			#if (OPTI == 0)
				#pragma omp parallel for
				for(size_t i=0; i<size; i++)
				{
					result.data[i] = data[i] - B.data[i%rows];
				}
			#else
				#pragma omp parallel for
				for(size_t i=0; i<rows; i++)
				{
					size_t simdSize = SIMD_FP32::getSIMDBound(columns);
					for(size_t j=0; j<simdSize; j+=SIMD_FP32::SIZE)
					{
						SIMD_FP32 v = SIMD_FP32::load(&data[j + i*columns]);
						SIMD_FP32 subV = SIMD_FP32::load(&B.data[j]);
						v -= subV;
						v.store(&result.data[i]);
					}
					for(size_t j=simdSize; j<columns; j++)
					{
						result.data[j + i*columns] = data[j + i*columns] - B.data[j];
					}
				}
			#endif
		}
		return result;
	}

	Matrix Matrix::broadcastFunction(std::function<float(float)> func, std::function<SIMD_FP32(SIMD_FP32)> simdFunc)
	{
		if(func == nullptr)
			throw std::bad_function_call();
		
		Matrix result = Matrix(rows, columns);
		size_t size = rows*columns;
		#if (OPTI == 0)
			#pragma omp parallel for
			for(size_t i=0; i<size; i++)
				result.data[i] = func(data[i]);
		#else
			size_t simdSize = SIMD_FP32::getSIMDBound(size);
			if(simdFunc == nullptr)
				simdSize = 0; //Don't do any SIMD work
			#pragma omp parallel for
			for(size_t i=0; i<simdSize; i+=SIMD_FP32::SIZE)
			{
				SIMD_FP32 v = SIMD_FP32::load(&data[i]);
				v = simdFunc(v);
				v.store(&result.data[i]);
			}
			for(size_t i=simdSize; i<size; i++)
				result.data[i] = func(data[i]);
		#endif
		return result;
	}
	Matrix Matrix::broadcastFunction(float (*func)(float), SIMD_FP32 (*simdFunc)(SIMD_FP32))
	{
		if(func == nullptr)
			throw std::bad_function_call();
		
		Matrix result = Matrix(rows, columns);
		size_t size = rows*columns;
		#if (OPTI == 0)
			#pragma omp parallel for
			for(size_t i=0; i<size; i++)
				result.data[i] = func(data[i]);
		#else
			size_t simdSize = SIMD_FP32::getSIMDBound(size);
			if(simdFunc == nullptr)
				simdSize = 0; //Don't do any SIMD work
			#pragma omp parallel for
			for(size_t i=0; i<simdSize; i+=SIMD_FP32::SIZE)
			{
				SIMD_FP32 v = SIMD_FP32::load(&data[i]);
				v = simdFunc(v);
				v.store(&result.data[i]);
			}
			for(size_t i=simdSize; i<size; i++)
				result.data[i] = func(data[i]);
		#endif
		return result;
	}

	float* Matrix::getData() const
	{
		return data;
	}

	int Matrix::getRows() const
	{
		return rows;
	}

	int Matrix::getCols() const
	{
		return columns;
	}

	bool Matrix::getValid() const
	{
		return valid;
	}

	float Matrix::get(int row, int col) const
	{
		if (row < rows && row >= 0 && col < columns && col >= 0)
			return data[col + row*columns];
		else
			return 0;
	}

	Matrix Matrix::hadamardProduct(const Matrix& other) const
	{
		if(rows != other.rows || columns != other.columns)
		{
			return Matrix();
		}

		Matrix m = Matrix(rows, columns);

		#if (OPTI==0)
		#pragma omp parallel for
		for (size_t i = 0; i < rows*columns; i++)
		{
			m.data[i] = other.data[i] * data[i];
		}
		#else
		int simdBound = SIMD_FP32::getSIMDBound(rows*columns);
		#pragma omp parallel for
		for (size_t i=0; i < simdBound; i+=SIMD_FP32::SIZE)
		{
			SIMD_FP32 aValues = SIMD_FP32::load(&data[i]);
			SIMD_FP32 bValues = SIMD_FP32::load(&other.data[i]);
			aValues*=bValues;
			aValues.store(&m.data[i]);
		}
		for(size_t i=simdBound; i<rows*columns; i++)
		{
			m.data[i] = data[i]*other.data[i];
		}
		#endif

		return m;
	}

	
	Matrix Matrix::inverseHadamardProduct(const Matrix& other) const
	{
		if(rows != other.rows || columns != other.columns)
		{
			return Matrix();
		}

		Matrix m = Matrix(rows, columns);

		#if (OPTI==0)
		#pragma omp parallel for
		for (size_t i = 0; i < rows*columns; i++)
		{
			m.data[i] = other.data[i] * data[i];
		}
		#else
		int simdBound = SIMD_FP32::getSIMDBound(rows*columns);
		#pragma omp parallel for
		for (size_t i=0; i < simdBound; i+=SIMD_FP32::SIZE)
		{
			SIMD_FP32 aValues = SIMD_FP32::load(&data[i]);
			SIMD_FP32 bValues = SIMD_FP32::load(&other.data[i]);
			aValues *= bValues;
			aValues.store(&m.data[i]);
		}
		for(size_t i=simdBound; i<rows*columns; i++)
		{
			m.data[i] = data[i]*other.data[i];
		}
		#endif

		return m;
	}

	Matrix Matrix::getInverse() const
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
				
				size_t totalSize = rows*columns;
				#pragma omp parallel for
				for(size_t index = 0; index<totalSize; index++)
				{
					size_t i = index/columns;
					size_t j = index%columns;
					double mult = ((i+j)%2) ? -1 : 1;

					inverse[i][j] = mult * getMatrixOfMinors(i, j).getDeterminate();
				}
				// for(size_t i=0; i<rows; i++)
				// {
				// 	for(size_t j=0; j<columns; j++)
				// 	{
				// 		inverse[i][j] = getMatrixOfMinors(i, j).getDeterminate();

				// 		if((i+j) % 2 == 1)
				// 		{
				// 			inverse[i][j] *= -1;
				// 		}
				// 	}
				// }

				return inverse.getTranspose() * (1.0/det);
			}
		}

		return Matrix();
	}

	Matrix Matrix::getTranspose() const
	{
		Matrix m = Matrix(columns, rows);
		int totalSize = columns*rows;

		#pragma omp parallel for
		for(size_t i=0; i<totalSize; i++)
		{
			int y = i / columns;
			int x = i % columns;
			m.data[i] = data[y + x*columns];
		}

		return m;
	}

	float Matrix::getDeterminate() const
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
				for(size_t i=0; i<columns; i++)
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

	Matrix Matrix::getMatrixOfMinors(int row, int col) const
	{
		Matrix m = Matrix(rows-1, columns-1);
		
		if(!m.getValid() || !getValid())
			throw InvalidMatrixSize();
	
		float* arrP = m.data;
		for(size_t i=0; i<rows; i++)
		{
			if(i != row)
			{
				float* dataP = arrP;
				for(size_t j=0; j<columns; j++)
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
	
	Matrix Matrix::multiply(const Matrix& A, const Matrix& B)
	{
		return A.multiply(B);
	}
	Matrix Matrix::multiplyTranspose(const Matrix& A, const Matrix& B)
	{
		return A.multiplyTranspose(B);
	}
	
	Matrix Matrix::multiply(const Matrix& B) const
	{
		//for large size matricies, do transpose then do matrix multiplication.
		if(!valid || !B.valid || columns != B.rows)
			throw InvalidMatrixSize();
		
		// //quick fix
		// Matrix C = Matrix(rows, B.columns);
		// int totalSize = rows*B.columns;
		// for(size_t i=0; i<totalSize; i++)
		// {
		// 	int y = i / B.columns;
		// 	int x = i % B.columns;
		// 	float sum=0;
		// 	for(int k=0; k<columns; k++)
		// 	{
		// 		sum += data[k + y*columns] * B.data[x + k*B.columns];
		// 	}
		// 	C.data[x + y*B.columns] = sum;
		// }
		// return C;
		
		if(rows * B.columns > EXCEPTIONALY_LARGE_MATRIX_AREA)
		{
			Matrix BT = B.getTranspose(); //O(N^2)
			return Matrix::multiplyTranspose(BT); //O(N^3) but better cache locality
		}

		Matrix C = Matrix(rows, B.columns);
		int totalSize = rows*B.columns;

		//expose more work to make parallel. only 1 join instead of O(N) joins
		#pragma omp parallel for
		for(size_t i=0; i<totalSize; i++)
		{
			int y = i / B.columns;
			int x = i % B.columns;
			#if (OPTI == 0)
				float sum = 0;
				for(int k=0; k<columns; k++)
				{
					sum += data[k + y*columns] * B.data[x + k*B.columns];
				}
				C.data[x + y*B.columns] = sum;
			#else
				SIMD_FP32 simdSum = 0;
				float finalSum = 0;
				float rowFloats[SIMD_FP32::SIZE];
				int k=0;
				int simdBound = SIMD_FP32::getSIMDBound(columns);
				while(k < simdBound)
				{
					SIMD_FP32 aValues = SIMD_FP32::load(&data[k + y*columns]);
					for(int count=0; count<SIMD_FP32::SIZE; count++)
					{
						rowFloats[count] = B.data[x + (k+count)*B.columns];
					}
					SIMD_FP32 bValues = SIMD_FP32::load(rowFloats);
					simdSum += aValues*bValues;
					k += SIMD_FP32::SIZE;
				}
				while(k < columns)
				{
					finalSum += data[k + y*columns] * B.data[x + k*B.columns];
					k++;
				}

				finalSum += sumSIMDRegister(simdSum.values);
				C.data[x + y*B.columns] = finalSum;
			#endif
		}
		return C;
	}
	Matrix Matrix::multiplyTranspose(const Matrix& B) const
	{
		if(!valid || !B.valid || columns != B.columns)
			throw InvalidMatrixSize();
		
		Matrix C = Matrix(rows, B.rows);
		int totalSize = rows*B.rows;

		#pragma omp parallel for
		for(size_t i=0; i<totalSize; i++)
		{
			int y = i / B.rows;
			int x = i % B.rows;
			#if (OPTI == 0)
				float sum = 0;
				for(int k=0; k<columns; k++)
				{
					sum += data[k + y*columns] * B.data[k + x*B.columns];
				}
				C.data[x + y*B.rows] = sum;
			#else
				float finalSum = 0;
				SIMD_FP32 simdSum = 0;
				int k = 0;
				int simdBound = SIMD_FP32::getSIMDBound(columns);
				while(k < simdBound)
				{
					SIMD_FP32 aValues = SIMD_FP32::load(&data[k + y*columns]);
					SIMD_FP32 bValues = SIMD_FP32::load(&B.data[k + x*B.columns]);
					simdSum += aValues*bValues;
					k+=SIMD_FP32::SIZE;
				}
				while(k < columns)
				{
					finalSum += data[k + y*columns] * B.data[k + x*B.columns];
					k++;
				}

				finalSum += sumSIMDRegister(simdSum.values);
				C.data[x + y*B.rows] = finalSum;
			#endif
		
		}
		return C;
	}
	
	Matrix Matrix::fusedMultiplyAdd(const Matrix& A, const Matrix& B, const Matrix& C)
	{
		return A.fusedMultiplyAdd(B, C);
	}
	Matrix Matrix::fusedMultiplyAddTranspose(const Matrix& A, const Matrix& B, const Matrix& C)
	{
		return A.fusedMultiplyAddTranspose(B, C);
	}

	Matrix Matrix::fusedMultiplyAdd(const Matrix& B, const Matrix& C) const
	{
		//for large size matricies, do transpose then do matrix multiplication.
		if(!valid || !B.valid || columns != B.rows)
			throw InvalidMatrixSize();
		
		if(C.rows != rows || C.columns != B.columns)
			throw InvalidMatrixSize();
			
		if(rows * B.columns > EXCEPTIONALY_LARGE_MATRIX_AREA)
		{
			Matrix BT = B.getTranspose(); //O(N^2)
			return Matrix::fusedMultiplyAddTranspose(BT, C); //O(N^3) but better cache locality
		}

		Matrix result = Matrix(rows, B.columns);
		int totalSize = rows*B.columns;

		//expose more work to make parallel. only 1 join instead of O(N) joins
		#pragma omp parallel for
		for(size_t i=0; i<totalSize; i++)
		{
			int y = i / B.columns;
			int x = i % B.columns;
			#if (OPTI == 0)
				float sum = C[y][x];
				for(int k=0; k<columns; k++)
				{
					sum += data[k + y*columns] * B.data[x + k*B.columns];
				}
				result.data[x + y*B.columns] = sum;
			#else
				SIMD_FP32 simdSum = 0;
				float finalSum = C[y][x];
				float rowFloats[SIMD_FP32::SIZE];
				int k=0;
				int simdBound = SIMD_FP32::getSIMDBound(columns);
				while(k < simdBound)
				{
					SIMD_FP32 aValues = SIMD_FP32::load(&data[k + y*columns]);
					for(int count=0; count<SIMD_FP32::SIZE; count++)
					{
						rowFloats[count] = B.data[x + (k+count)*B.columns];
					}
					SIMD_FP32 bValues = SIMD_FP32::load(rowFloats);

					simdSum += aValues*bValues;
					k += SIMD_FP32::SIZE;
				}
				while(k < columns)
				{
					finalSum += data[k + y*columns] * B.data[x + k*B.columns];
					k++;
				}

				finalSum += sumSIMDRegister(simdSum.values);
				result.data[x + y*B.columns] = finalSum;
			#endif
		}
		return result;
	}
	Matrix Matrix::fusedMultiplyAddTranspose(const Matrix& B, const Matrix& C) const
	{
		if(!valid || !B.valid || columns != B.columns)
			throw InvalidMatrixSize();
		
		if(C.rows != rows || C.columns != B.rows)
			throw InvalidMatrixSize();
		
		Matrix result = Matrix(rows, B.rows);
		int totalSize = rows*B.rows;

		#pragma omp parallel for
		for(size_t i=0; i<totalSize; i++)
		{
			int y = i / B.rows;
			int x = i % B.rows;
			#if (OPTI == 0)
				float sum = C[y][x];
				for(int k=0; k<columns; k++)
				{
					sum += data[k + y*columns] * B.data[k + x*B.columns];
				}
				result.data[x + y*B.rows] = sum;
			#else
				float finalSum = C[y][x];
				SIMD_FP32 simdSum = 0;
				int k = 0;
				int simdBound = SIMD_FP32::getSIMDBound(columns);
				while(k < simdBound)
				{
					SIMD_FP32 aValues = SIMD_FP32::load(&data[k + y*columns]);
					SIMD_FP32 bValues = SIMD_FP32::load(&B.data[k + x*B.columns]);
					simdSum += aValues*bValues;
					k+=SIMD_FP32::SIZE;
				}
				while(k < columns)
				{
					finalSum += data[k + y*columns] * B.data[k + x*B.columns];
					k++;
				}

				finalSum += sumSIMDRegister(simdSum.values);
				result.data[x + y*B.rows] = finalSum;
			#endif
		
		}
		return result;
	}

	float Matrix::sum() const
	{
		float sum = 0;
		for(size_t i=0; i<rows*columns; i++)
		{
			sum += data[i];
		}
		return sum;
	}

	Matrix Matrix::horizontalSum() const
	{
		Matrix m = Matrix(rows, 1);
		#pragma omp parallel for
		for(size_t i=0; i<rows; i++)
		{
			float sum = 0;
			for(size_t j=0; j<columns; j++)
			{
				sum += data[j + i*rows];
			}
			m[i][0] = sum;
		}
		return m;
	}

	Matrix Matrix::verticalSum() const
	{
		Matrix m = Matrix(1, columns);
		for(size_t i=0; i<rows; i++)
		{
			for(size_t j=0; j<columns; j++)
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