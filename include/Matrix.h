#pragma once
#include "BuildOptions.h"
#include "Opti.h"
#include "Concurrency.h"
#include <math.h>
#include "Object.h"
#include "GeneralVector.h"

#ifndef MATRIX_SHIFT_AMOUNT
	#if (OPTI == 0)
		#define MATRIX_SHIFT_AMOUNT 0
		#define MATRIX_INC_AMOUNT 1
	#else
		#define MATRIX_SHIFT_AMOUNT (1+OPTI)
		#define MATRIX_INC_AMOUNT (1<<MATRIX_SHIFT_AMOUNT)
		#define GET_MATRIX_SIMD_BOUND(x) ((x>>MATRIX_SHIFT_AMOUNT)<<MATRIX_SHIFT_AMOUNT)
		#if (OPTI == 1)
			#define MATRIX_SIMD_DATATYPE __m128
			#define MATRIX_SIMD_LOAD _mm_loadu_ps
			#define MATRIX_SIMD_STORE _mm_storeu_ps
			#define MATRIX_SIMD_MULT _mm_mul_ps
			#define MATRIX_SIMD_DIV _mm_div_ps
			#define MATRIX_SIMD_ADD _mm_add_ps
			#define MATRIX_SIMD_SET_VALUES _mm_set_ps
			#define MATRIX_SIMD_ZERO() _mm_set1_ps(0);

		#elif (OPTI >= 2)
			#define MATRIX_SIMD_DATATYPE __m256
			#define MATRIX_SIMD_LOAD _mm256_loadu_ps
			#define MATRIX_SIMD_STORE _mm256_storeu_ps
			#define MATRIX_SIMD_MULT _mm256_mul_ps
			#define MATRIX_SIMD_DIV _mm256_div_ps
			#define MATRIX_SIMD_ADD _mm256_add_ps
			#define MATRIX_SIMD_SET_VALUES _mm256_set_ps
			#define MATRIX_SIMD_ZERO() _mm256_set1_ps(0);
		#endif
	#endif
#endif

namespace smpl
{

	class DLL_OPTION Matrix : public Object
	{
	public:
		static const size_t EXCEPTIONALY_LARGE_MATRIX_AREA = 0xFFFF;
		/**
		 * @brief Construct a new Matrix object
		 * 		A matrix with rows = 0 or columns = 0 is considered invalid.
		 * 		
		 * 		The internal data structure used to store the matrix is a 1D array.
		 */
		Matrix();

		/**
		 * @brief Construct a new Matrix object
		 * 		A matrix with rows = 0 or columns = 0 is considered invalid.
		 * 
		 * 		The internal data structure used to store the matrix is a 1D array.
		 * @param rows 
		 * @param cols 
		 */
		Matrix(int rows, int cols);

		/**
		 * @brief Construct a new Matrix object from another Matrix object
		 * 
		 * @param o
		 * 		The matrix to copy.
		 */
		Matrix(const Matrix& o);

		/**
		 * @brief Copies a Matrix object
		 * 
		 * @param o
		 * 		The matrix to copy.
		 */
		void operator=(const Matrix& o);

		/**
		 * @brief Destroy the Matrix object
		 * 
		 */
		~Matrix();

		/**
		 * @brief Copies a Matrix object
		 * 
		 * @param o
		 * 		The matrix to copy.
		 */
		void copy(const Matrix& o);

		//Object and RootClass Stuff
		static const RootClass globalClass;
		virtual const RootClass* getClass();

		/**
		 * @brief Returns a pointer to the start of the specified row
		 * 		Note that the internal data structure is a 1D array.
		 * @param row 
		 * @return float* 
		 */
		float* operator[](int row);

		Matrix operator*(float value);
		friend Matrix operator*(float value, Matrix other)
		{
			return other*value;
		}

		Matrix operator*(Matrix other);
		GeneralVector operator*(GeneralVector other);

		void operator*=(float value);

		Matrix operator+(Matrix other);
		void operator+=(Matrix other);

		Matrix operator-(Matrix other);
		void operator-=(Matrix other);

		bool operator==(Matrix other);
		bool operator!=(Matrix other);

		static Matrix getIdentityMatrix(int rows);
		/**
		 * @brief Returns the 1D array representing the data in the matrix.
		 * 
		 * @return float* 
		 */
		float* getData();

		/**
		 * @brief Returns the amount of rows.
		 * 
		 * @return int 
		 */
		int getRows();

		/**
		 * @brief Returns the amount of columns.
		 * 
		 * @return int 
		 */
		int getCols();

		/**
		 * @brief Returns if the matrix is valid
		 * 
		 * @return bool
		 */
		bool getValid();

		/**
		 * @brief Returns the value of the matrix at the specified location
		 * 
		 * @param row 
		 * @param col 
		 * @return float 
		 */
		float get(int row, int col);

		/**
		 * @brief Returns the Hadamard product of the 2 matrices.
		 * 		The Hadamard product requires that the matricies are the same size.
		 * 			rows = other.rows, columns = other.columns
		 * 		
		 * 		It does component to component multiplication.
		 * 
		 * @param other 
		 * @return Matrix 
		 */
		Matrix hadamardProduct(Matrix other);

		/**
		 * @brief Returns the elementwise division of the 2 matrices.
		 * 		The hadamard product but A * 1.0/B. 
		 * 		Does not check for division by 0
		 * 
		 * @param other 
		 * @return Matrix 
		 */
		Matrix inverseHadamardProduct(Matrix other);

		/**
		 * @brief Gets the Inverse of the matrix.
		 * 		The inverse only exists if the determinate is not 0 and the matrix is square.
		 * 		An invalid matrix is returned if the function fails.
		 * 
		 * @return Matrix 
		 */
		Matrix getInverse();

		/**
		 * @brief Gets the Transpose of the matrix.
		 * 		Swaps the rows with the columns.
		 * 
		 * @return Matrix 
		 */
		Matrix getTranspose();

		/**
		 * @brief Gets the Determinate of the matrix.
		 * 		The determinate only exists if the matrix is square.
		 * 
		 * @return float 
		 */
		float getDeterminate();

		/**
		 * @brief Gets the Matrix Of Minors from the matrix.
		 * 		Can return an invalid matrix if the original matrix is invalid or the new matrix
		 * 		would have less than 1 row or 1 column.
		 * 
		 * @param row 
		 * @param col 
		 * @return Matrix 
		 */
		Matrix getMatrixOfMinors(int row, int col);

		void clear();

		/**
		 * @brief Multiplies 2 matrices.
		 * 		Specifically does A * B
		 * 
		 * @param A 
		 * @param B 
		 * @return Matrix 
		 */
		static Matrix multiply(Matrix& A, Matrix& B);
		Matrix multiply(Matrix& B);

		/**
		 * @brief Multiplies 2 matrices where the transpose of B is used.
		 * 		Specifically does A * B^T
		 * 		Skips the transpose operation which can save on resources.
		 * 
		 * @param A 
		 * @param B 
		 * @return Matrix 
		 */
		static Matrix multiplyTranspose(Matrix& A, Matrix& B);
		Matrix multiplyTranspose(Matrix& B);

		/**
		 * @brief Returns the sum of all the elements in the matrix.
		 * 
		 * @return float 
		 */
		float sum();

		/**
		 * @brief Returns a Matrix that stores the sum across each row
		 * 		in each row. Returns a column vector as a Matrix
		 * 
		 * @return Matrix 
		 */
		Matrix horizontalSum();

		/**
		 * @brief Returns a Matrix that store the sum across each column
		 * 		in each column. Returns a row vector as a Matrix
		 * 
		 * @return Matrix 
		 */
		Matrix verticalSum();

		/**
		 * @brief Normalizes the matrix such that the sum of all elements is
		 * 		equal to 1.0. It maintains the relavant distance or ratio
		 * 		between elements.
		 * 
		 */
		void normalize();

	protected:

		float* data = nullptr;
		int rows = 0;
		int columns = 0;
		int size = 0;

		bool valid = false;
	
	};

} //NAMESPACE smpl END