#pragma once
#include <math.h>
#include "Object.h"
#include "GeneralVector.h"

namespace glib
{

	class Matrix : public Object
	{
	public:
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

		//Object and Class Stuff
		const Class* getClass();
		static const Class myClass;

		/**
		 * @brief Returns a pointer to the start of the specified row
		 * 		Note that the internal data structure is a 1D array.
		 * @param row 
		 * @return double* 
		 */
		double* operator[](int row);

		Matrix operator*(double value);
		friend Matrix operator*(double value, Matrix other)
		{
			return other*value;
		}

		Matrix operator*(Matrix other);
		GeneralVector operator*(GeneralVector other);

		void operator*=(double value);

		Matrix operator+(Matrix other);
		void operator+=(Matrix other);

		Matrix operator-(Matrix other);
		void operator-=(Matrix other);

		bool operator==(Matrix other);
		bool operator!=(Matrix other);

		/**
		 * @brief Returns the 1D array representing the data in the matrix.
		 * 
		 * @return double* 
		 */
		double* getData();

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
		 * @return double 
		 */
		double get(int row, int col);

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
		 * @return double 
		 */
		double getDeterminate();

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

	protected:
		double* data = nullptr;
		int rows = 0;
		int columns = 0;
		int size = 0;

		bool valid = false;
	};

} //NAMESPACE glib END