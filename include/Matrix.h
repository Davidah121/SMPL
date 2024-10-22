#pragma once
#include "BuildOptions.h"
#include "Opti.h"
#include "Concurrency.h"
#include <math.h>
#include "Object.h"
#include <functional>
#include "GeneralExceptions.h"
#include "SIMD.h"

// #ifndef MATRIX_SHIFT_AMOUNT
// 	#if (OPTI == 0)
// 		#define MATRIX_SHIFT_AMOUNT 0
// 		#define MATRIX_INC_AMOUNT 1
// 		#define MATRIX_SIMD_DATATYPE void
// 	#else
// 		#define MATRIX_SHIFT_AMOUNT (1+OPTI)
// 		#define MATRIX_INC_AMOUNT (1<<MATRIX_SHIFT_AMOUNT)
// 		#define GET_MATRIX_SIMD_BOUND(x) ((x>>MATRIX_SHIFT_AMOUNT)<<MATRIX_SHIFT_AMOUNT)
// 		#if (OPTI == 1)
// 			#define MATRIX_SIMD_DATATYPE __m128
// 			#define MATRIX_SIMD_LOAD _mm_loadu_ps
// 			#define MATRIX_SIMD_STORE _mm_storeu_ps
// 			#define MATRIX_SIMD_MULT _mm_mul_ps
// 			#define MATRIX_SIMD_DIV _mm_div_ps
// 			#define MATRIX_SIMD_ADD _mm_add_ps
// 			#define MATRIX_SIMD_SUB _mm_sub_ps
// 			#define MATRIX_SIMD_SET_VALUES _mm_set_ps
// 			#define MATRIX_SIMD_ZERO() _mm_set1_ps(0);
// 			#define MATRIX_SIMD_SET_SINGLE_VALUE(a) _mm_set1_ps(a);
//             #define MATRIX_SIMD_EXP _mm_exp_ps
//             #define MATRIX_SIMD_RECIPROCAL _mm_rcp_ps
// 			#define MATRIX_SIMD_CMP_GREATER _mm_cmpgt_ps
// 			#define MATRIX_SIMD_CMP_LESS _mm_cmplt_ps
// 			#define MATRIX_SIMD_BLEND _mm_blend_ps
// 			#define MATRIX_SIMD_BLEND_MASK _mm_blend_ps

// 		#elif (OPTI >= 2)
// 			#define MATRIX_SIMD_DATATYPE __m256
// 			#define MATRIX_SIMD_LOAD _mm256_loadu_ps
// 			#define MATRIX_SIMD_STORE _mm256_storeu_ps
// 			#define MATRIX_SIMD_MULT _mm256_mul_ps
// 			#define MATRIX_SIMD_DIV _mm256_div_ps
// 			#define MATRIX_SIMD_ADD _mm256_add_ps
// 			#define MATRIX_SIMD_SUB _mm256_sub_ps
// 			#define MATRIX_SIMD_SET_VALUES _mm256_set_ps
// 			#define MATRIX_SIMD_ZERO() _mm256_set1_ps(0);
// 			#define MATRIX_SIMD_SET_SINGLE_VALUE(a) _mm256_set1_ps(a);
//             #define MATRIX_SIMD_EXP _mm256_exp_ps
//             #define MATRIX_SIMD_RECIPROCAL _mm256_rcp_ps
// 			#define MATRIX_SIMD_CMP_GREATER _mm256_cmpgt_ps
// 			#define MATRIX_SIMD_CMP_LESS _mm256_cmplt_ps
// 			#define MATRIX_SIMD_BLEND _mm256_blend_ps
// 			#define MATRIX_SIMD_BLEND_MASK _mm256_blend_ps
// 		#endif
// 	#endif
// #endif

namespace smpl
{

	class DLL_OPTION Matrix : public Object
	{
	public:
		static const size_t EXCEPTIONALY_LARGE_MATRIX_AREA = 0xFFFF;
		
		struct InvalidMatrixSize : public std::exception
		{
			const char* what() const noexcept { return "Matrices are an incorrect size to do operation"; }
		};

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
		float* operator[](int row) const;

		Matrix operator*(float value) const;
		Matrix operator*(const Matrix& other) const;
		Matrix operator+(const Matrix& other) const;
		Matrix operator-(const Matrix& other) const;

		bool operator==(const Matrix& other) const;
		bool operator!=(const Matrix& other) const;

		void operator*=(float value);
		void operator+=(const Matrix& other);
		void operator-=(const Matrix& other);

		friend Matrix operator*(float value, const Matrix& other)
		{
			return other*value;
		}

		/**
		 * @brief Returns an Identity Matrix which must be a square matrix.
		 * 
		 * @param rows 
		 * @return Matrix 
		 */
		static Matrix getIdentityMatrix(int rows);

		/**
		 * @brief Sets all of the values in the matrix to a single value.
		 * 
		 * @param v 
		 */
		void setAllValues(float v);
		
		/**
		 * @brief Performs an element wise addition.
		 * 		Equivalent to making the input value "v" into a matrix
		 * 		consisting of just the value "v" and adding.
		 * 
		 * @param v 
		 * @return Matrix 
		 */
		Matrix broadcastAdd(float v);

		/**
		 * @brief Attempts to do an element wise addition using a
		 * 		row or column matrix "v". Must be a row or column matrix.
		 * 		Must be an appropriate size as well.
		 * 
		 * @param v 
		 * @return Matrix 
		 */
		Matrix broadcastAdd(const Matrix& v);

		
		/**
		 * @brief Performs an element wise subtraction.
		 * 		Equivalent to making the input value "v" into a matrix
		 * 		consisting of just the value "v" and subtraction.
		 * 
		 * @param v 
		 * @return Matrix 
		 */
		Matrix broadcastSubtract(float v);
		
		/**
		 * @brief Attempts to do an element wise addition using a
		 * 		row or column matrix "v". Must be a row or column matrix.
		 * 		Must be an appropriate size as well.
		 * 
		 * @param v 
		 * @return Matrix 
		 */
		Matrix broadcastSubtract(const Matrix& v);

		/**
		 * @brief Attempts to apply the input function to every element.
		 * 		If possible and OPTI has been defined as a value > 0,
		 * 		an SIMD version may be included for additional speed.
		 * 
		 * 		This version allows lambdas that capture a value.
		 * 
		 * @param func 
		 * @param simdFunc 
		 * @return Matrix 
		 */
		Matrix broadcastFunction(std::function<float(float)> func, std::function<SIMD_FP32(SIMD_FP32)> simdFunc);

		
		/**
		 * @brief Attempts to apply the input function to every element.
		 * 		If possible and OPTI has been defined as a value > 0,
		 * 		an SIMD version may be included for additional speed.
		 * 
		 * 		This version does not allow lambdas that capture a value.
		 * 			May potentially be faster in practice.
		 * 
		 * @param func 
		 * @param simdFunc 
		 * @return Matrix 
		 */
		Matrix broadcastFunction(float (*func)(float), SIMD_FP32 (*simdFunc)(SIMD_FP32));

		/**
		 * @brief Returns the 1D array representing the data in the matrix.
		 * 
		 * @return float* 
		 */
		float* getData() const;

		/**
		 * @brief Returns the amount of rows.
		 * 
		 * @return int 
		 */
		int getRows() const;

		/**
		 * @brief Returns the amount of columns.
		 * 
		 * @return int 
		 */
		int getCols() const;

		/**
		 * @brief Returns if the matrix is valid
		 * 
		 * @return bool
		 */
		bool getValid() const;

		/**
		 * @brief Returns the value of the matrix at the specified location
		 * 
		 * @param row 
		 * @param col 
		 * @return float 
		 */
		float get(int row, int col) const;

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
		Matrix hadamardProduct(const Matrix& other) const;

		/**
		 * @brief Attempts to broadcast a row or column matrix to the correct size
		 * 		and perform a hadamard product. Must be an appropriate size.
		 * 
		 * @param other 
		 * @return Matrix 
		 */
		Matrix broadcastHadamardProduct(const Matrix& other) const;

		/**
		 * @brief Returns the elementwise division of the 2 matrices.
		 * 		The hadamard product but A * 1.0/B. 
		 * 		Does not check for division by 0
		 * 
		 * @param other 
		 * @return Matrix 
		 */
		Matrix inverseHadamardProduct(const Matrix& other) const;
		
		/**
		 * @brief Attempts to broadcast a row or column matrix to the correct size
		 * 		and perform a inverse hadamard product. Must be an appropriate size.
		 * 			The hadamard product but A * 1.0/B.
		 * 
		 * @param other 
		 * @return Matrix 
		 */
		Matrix broadcastInverseHadamardProduct(const Matrix& other) const;

		/**
		 * @brief Gets the Inverse of the matrix.
		 * 		The inverse only exists if the determinate is not 0 and the matrix is square.
		 * 		An invalid matrix is returned if the function fails.
		 * 
		 * @return Matrix 
		 */
		Matrix getInverse() const;

		/**
		 * @brief Gets the Transpose of the matrix.
		 * 		Swaps the rows with the columns.
		 * 
		 * @return Matrix 
		 */
		Matrix getTranspose() const;

		/**
		 * @brief Gets the Determinate of the matrix.
		 * 		The determinate only exists if the matrix is square.
		 * 
		 * @return float 
		 */
		float getDeterminate() const;

		/**
		 * @brief Gets the Matrix Of Minors from the matrix.
		 * 		Can return an invalid matrix if the original matrix is invalid or the new matrix
		 * 		would have less than 1 row or 1 column.
		 * 
		 * @param row 
		 * @param col 
		 * @return Matrix 
		 */
		Matrix getMatrixOfMinors(int row, int col) const;

		void clear();

		/**
		 * @brief Multiplies 2 matrices.
		 * 		Specifically does A * B
		 * 
		 * @param A 
		 * @param B 
		 * @return Matrix 
		 */
		static Matrix multiply(const Matrix& A, const Matrix& B);
		Matrix multiply(const Matrix& B) const;

		/**
		 * @brief Multiplies 2 matrices where the transpose of B is used.
		 * 		Specifically does A * B^T
		 * 		Skips the transpose operation which can save on resources.
		 * 
		 * @param A 
		 * @param B 
		 * @return Matrix 
		 */
		static Matrix multiplyTranspose(const Matrix& A, const Matrix& B);
		Matrix multiplyTranspose(const Matrix& B) const;

		/**
		 * @brief Computes A*B + C. It is a bit faster than doing them separately
		 * 		avoiding the need to maintain an additional intermediate value.
		 * 
		 * @param A 
		 * @param B 
		 * @param C 
		 * @return Matrix 
		 */
		static Matrix fusedMultiplyAdd(const Matrix& A, const Matrix& B, const Matrix& C);

		/**
		 * @brief Computes A*B + C. It is a bit faster than doing them separately
		 * 		avoiding the need to maintain an additional intermediate value.
		 * @param B 
		 * @param C 
		 * @return Matrix 
		 */
		Matrix fusedMultiplyAdd(const Matrix& B, const Matrix& C) const;
		
		/**
		 * @brief Computes A*B^T + C. It is a bit faster than doing them separately
		 * 		avoiding the need to maintain an additional intermediate value.
		 * 
		 * @param A 
		 * @param B 
		 * @param C 
		 * @return Matrix 
		 */
		static Matrix fusedMultiplyAddTranspose(const Matrix& A, const Matrix& B, const Matrix& C);

		/**
		 * @brief Computes A*B^T + C. It is a bit faster than doing them separately
		 * 		avoiding the need to maintain an additional intermediate value.
		 * @param B 
		 * @param C 
		 * @return Matrix 
		 */
		Matrix fusedMultiplyAddTranspose(const Matrix& B, const Matrix& C) const;
		
		
		/**
		 * @brief Returns the sum of all the elements in the matrix.
		 * 
		 * @return float 
		 */
		float sum() const;

		/**
		 * @brief Returns a Matrix that stores the sum across each row
		 * 		in each row. Returns a column vector as a Matrix
		 * 
		 * @return Matrix 
		 */
		Matrix horizontalSum() const;

		/**
		 * @brief Returns a Matrix that store the sum across each column
		 * 		in each column. Returns a row vector as a Matrix
		 * 
		 * @return Matrix 
		 */
		Matrix verticalSum() const;

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