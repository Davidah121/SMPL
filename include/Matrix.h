#pragma once
#include "BuildOptions.h"
#include "Opti.h"
#include "Concurrency.h"
#include <math.h>
#include "SimpleSerialization.h"
#include <functional>
#include "GeneralExceptions.h"
#include "SIMD.h"

#ifdef min
	#undef min
#endif

#ifdef max
	#undef max
#endif

namespace smpl
{
	template<typename T, typename SIMD_TYPE>
	class DLL_OPTION Matrix;
	
	typedef Matrix<int8_t, SIMD_8> MatrixInt8;
	typedef Matrix<uint8_t, SIMD_U8> MatrixUInt8;
	typedef Matrix<int16_t, SIMD_16> MatrixInt16;
	typedef Matrix<uint16_t, SIMD_U16> MatrixUInt16;
	typedef Matrix<int32_t, SIMD_32> MatrixInt32;
	typedef Matrix<uint32_t, SIMD_U32> MatrixUInt32;
	typedef Matrix<float, SIMD_FP32> MatrixF;
	typedef Matrix<double, SIMD_FP64> MatrixD;
	
	template<typename T, typename SIMD_TYPE>
	class DLL_OPTION Matrix : public SerializedObject
	{
	public:
		static const size_t EXCEPTIONALY_LARGE_MATRIX_AREA = 0xFFFF;
		
		struct InvalidMatrixSize : public std::exception
		{
			const char* what() const noexcept { return "Matrices are an incorrect size to do operation"; }
		};

		struct InvalidMatrixDataType : public std::exception
		{
			const char* what() const noexcept { return "Matrices do not share the same datatype and must be explicitly casted first"; }
		};
		
		/**
		 * @brief Construct a new Empty Matrix object
		 * 
		 */
		Matrix();
		
		/**
		 * @brief Construct a new Base Matrix object
		 * 		Can only be constructed from sub classes and type must be specified.
		 * 
		 * @param rows 
		 * @param cols 
		 * @param type 
		 */
		Matrix(unsigned int rows, unsigned int cols);

		/**
		 * @brief Construct a new Matrix object from another Matrix object
		 * 
		 * @param o
		 * 		The matrix to copy.
		 */
		Matrix(const Matrix<T, SIMD_TYPE>& o);

		/**
		 * @brief Copies a Matrix object
		 * 
		 * @param o
		 * 		The matrix to copy.
		 */
		void operator=(const Matrix<T, SIMD_TYPE>& o);

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
		void copy(const Matrix<T, SIMD_TYPE>& o);

		/**
		 * @brief Returns a pointer to the start of the specified row
		 * 		Note that the internal data structure is a 1D array.
		 * @param row 
		 * @return T* 
		 */
		T* operator[](unsigned int row) const;

		Matrix<T, SIMD_TYPE> operator*(T value) const;
		Matrix<T, SIMD_TYPE> operator/(T value) const;
		Matrix<T, SIMD_TYPE> operator*(const Matrix<T, SIMD_TYPE>& other) const;
		Matrix<T, SIMD_TYPE> operator+(const Matrix<T, SIMD_TYPE>& other) const;
		Matrix<T, SIMD_TYPE> operator-(const Matrix<T, SIMD_TYPE>& other) const;

		bool operator==(const Matrix<T, SIMD_TYPE>& other) const;
		bool operator!=(const Matrix<T, SIMD_TYPE>& other) const;

		void operator*=(T value);
		void operator/=(T value);
		void operator+=(const Matrix<T, SIMD_TYPE>& other);
		void operator-=(const Matrix<T, SIMD_TYPE>& other);

		friend Matrix<T, SIMD_TYPE> operator*(T value, const Matrix<T, SIMD_TYPE>& other)
		{
			return other*value;
		}

		/**
		 * @brief Returns an Identity Matrix which must be a square matrix.
		 * 
		 * @param rows 
		 * @return Matrix<T, SIMD_TYPE> 
		 */
		static Matrix<T, SIMD_TYPE> getIdentityMatrix(unsigned int rows);

		/**
		 * @brief Sets all of the values in the matrix to a single value.
		 * 
		 * @param v 
		 */
		void setAllValues(T v);
		
		/**
		 * @brief Performs an element wise addition.
		 * 		Equivalent to making the input value "v" into a matrix
		 * 		consisting of just the value "v" and adding.
		 * 
		 * @param v 
		 * @return Matrix<T, SIMD_TYPE> 
		 */
		Matrix<T, SIMD_TYPE> broadcastAdd(T v) const;

		/**
		 * @brief Attempts to do an element wise addition using a
		 * 		row or column matrix "v". Must be a row or column matrix.
		 * 		Must be an appropriate size as well.
		 * 
		 * @param v 
		 * @return Matrix<T, SIMD_TYPE> 
		 */
		Matrix<T, SIMD_TYPE> broadcastAdd(const Matrix<T, SIMD_TYPE>& v) const;

		
		/**
		 * @brief Performs an element wise subtraction.
		 * 		Equivalent to making the input value "v" into a matrix
		 * 		consisting of just the value "v" and subtraction.
		 * 
		 * @param v 
		 * @return Matrix<T, SIMD_TYPE> 
		 */
		Matrix<T, SIMD_TYPE> broadcastSubtract(T v) const;
		
		/**
		 * @brief Attempts to do an element wise addition using a
		 * 		row or column matrix "v". Must be a row or column matrix.
		 * 		Must be an appropriate size as well.
		 * 
		 * @param v 
		 * @return Matrix<T, SIMD_TYPE> 
		 */
		Matrix<T, SIMD_TYPE> broadcastSubtract(const Matrix<T, SIMD_TYPE>& v) const;

		/**
		 * @brief Attempts to apply the input function to every element.
		 * 		If possible and OPTI has been defined as a value > 0,
		 * 		an SIMD version may be included for additional speed.
		 * 
		 * 		This version allows lambdas that capture a value.
		 * 
		 * @param func 
		 * @param simdFunc 
		 * @return Matrix<T, SIMD_TYPE> 
		 */
		#if(SIMD_TYPE != void)
		Matrix<T, SIMD_TYPE> broadcastFunction(std::function<T(T)> func, std::function<SIMD_TYPE(SIMD_TYPE)> simdFunc) const;
		#else
		Matrix<T, SIMD_TYPE> broadcastFunction(std::function<T(T)> func, std::function<void()> simdFunc) const;
		#endif

		
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
		 * @return Matrix<T, SIMD_TYPE> 
		 */
		
		#if(SIMD_TYPE != void)
		Matrix<T, SIMD_TYPE> broadcastFunction(T (*func)(T), SIMD_TYPE (*simdFunc)(SIMD_TYPE)) const;
		#else
		Matrix<T, SIMD_TYPE> broadcastFunction(T (*func)(T), void (*simdFunc)()) const;
		#endif

		/**
		 * @brief Returns the 1D array representing the data in the matrix.
		 * 
		 * @return T* 
		 */
		T* getData() const;

		/**
		 * @brief Returns the amount of rows.
		 * 
		 * @return unsigned int 
		 */
		unsigned int getRows() const;

		/**
		 * @brief Returns the amount of columns.
		 * 
		 * @return unsigned int 
		 */
		unsigned int getCols() const;

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
		 * @return T 
		 */
		T get(unsigned int row, unsigned int col) const;

		/**
		 * @brief Returns the Hadamard product of the 2 matrices.
		 * 		The Hadamard product requires that the matricies are the same size.
		 * 			rows = other.rows, columns = other.columns
		 * 		
		 * 		It does component to component multiplication.
		 * 
		 * @param other 
		 * @return Matrix<T, SIMD_TYPE> 
		 */
		Matrix<T, SIMD_TYPE> hadamardProduct(const Matrix<T, SIMD_TYPE>& other) const;

		/**
		 * @brief Attempts to broadcast a row or column matrix to the correct size
		 * 		and perform a hadamard product. Must be an appropriate size.
		 * 
		 * @param other 
		 * @return Matrix<T, SIMD_TYPE> 
		 */
		Matrix<T, SIMD_TYPE> broadcastHadamardProduct(const Matrix<T, SIMD_TYPE>& other) const;

		/**
		 * @brief Returns the elementwise division of the 2 matrices.
		 * 		The hadamard product but A * 1.0/B. 
		 * 		Does not check for division by 0
		 * 
		 * @param other 
		 * @return Matrix<T, SIMD_TYPE> 
		 */
		Matrix<T, SIMD_TYPE> inverseHadamardProduct(const Matrix<T, SIMD_TYPE>& other) const;
		
		/**
		 * @brief Attempts to broadcast a row or column matrix to the correct size
		 * 		and perform a inverse hadamard product. Must be an appropriate size.
		 * 			The hadamard product but A * 1.0/B.
		 * 
		 * @param other 
		 * @return Matrix<T, SIMD_TYPE> 
		 */
		Matrix<T, SIMD_TYPE> broadcastInverseHadamardProduct(const Matrix<T, SIMD_TYPE>& other) const;

		/**
		 * @brief Gets the Inverse of the matrix.
		 * 		The inverse only exists if the determinate is not 0 and the matrix is square.
		 * 		An invalid matrix is returned if the function fails.
		 * 
		 * @return Matrix<T, SIMD_TYPE> 
		 */
		Matrix<T, SIMD_TYPE> getInverse() const;

		/**
		 * @brief Gets the Transpose of the matrix.
		 * 		Swaps the rows with the columns.
		 * 
		 * @return Matrix<T, SIMD_TYPE> 
		 */
		Matrix<T, SIMD_TYPE> getTranspose() const;

		/**
		 * @brief Gets the Determinate of the matrix.
		 * 		The determinate only exists if the matrix is square.
		 * 
		 * @return T 
		 */
		T getDeterminate() const;

		/**
		 * @brief Gets the Matrix Of Minors from the matrix.
		 * 		Can return an invalid matrix if the original matrix is invalid or the new matrix
		 * 		would have less than 1 row or 1 column.
		 * 
		 * @param row 
		 * @param col 
		 * @return Matrix<T, SIMD_TYPE> 
		 */
		Matrix<T, SIMD_TYPE> getMatrixOfMinors(unsigned int row, unsigned int col) const;

		void clear();

		/**
		 * @brief Multiplies 2 matrices.
		 * 		Specifically does A * B
		 * 
		 * @param A 
		 * @param B 
		 * @return Matrix<T, SIMD_TYPE> 
		 */
		static Matrix<T, SIMD_TYPE> multiply(const Matrix<T, SIMD_TYPE>& A, const Matrix<T, SIMD_TYPE>& B);
		Matrix<T, SIMD_TYPE> multiply(const Matrix<T, SIMD_TYPE>& B) const;

		/**
		 * @brief Multiplies 2 matrices where the transpose of B is used.
		 * 		Specifically does A * B^T
		 * 		Skips the transpose operation which can save on resources.
		 * 
		 * @param A 
		 * @param B 
		 * @return Matrix<T, SIMD_TYPE> 
		 */
		static Matrix<T, SIMD_TYPE> multiplyTranspose(const Matrix<T, SIMD_TYPE>& A, const Matrix<T, SIMD_TYPE>& B);
		Matrix<T, SIMD_TYPE> multiplyTranspose(const Matrix<T, SIMD_TYPE>& B) const;

		/**
		 * @brief Computes A*B + C. It is a bit faster than doing them separately
		 * 		avoiding the need to maintain an additional intermediate value.
		 * 
		 * @param A 
		 * @param B 
		 * @param C 
		 * @return Matrix<T, SIMD_TYPE> 
		 */
		static Matrix<T, SIMD_TYPE> fusedMultiplyAdd(const Matrix<T, SIMD_TYPE>& A, const Matrix<T, SIMD_TYPE>& B, const Matrix<T, SIMD_TYPE>& C);

		/**
		 * @brief Computes A*B + C. It is a bit faster than doing them separately
		 * 		avoiding the need to maintain an additional intermediate value.
		 * @param B 
		 * @param C 
		 * @return Matrix<T, SIMD_TYPE> 
		 */
		Matrix<T, SIMD_TYPE> fusedMultiplyAdd(const Matrix<T, SIMD_TYPE>& B, const Matrix<T, SIMD_TYPE>& C) const;
		
		/**
		 * @brief Computes A*B^T + C. It is a bit faster than doing them separately
		 * 		avoiding the need to maintain an additional intermediate value.
		 * 
		 * @param A 
		 * @param B 
		 * @param C 
		 * @return Matrix<T, SIMD_TYPE> 
		 */
		static Matrix<T, SIMD_TYPE> fusedMultiplyAddTranspose(const Matrix<T, SIMD_TYPE>& A, const Matrix<T, SIMD_TYPE>& B, const Matrix<T, SIMD_TYPE>& C);

		/**
		 * @brief Computes A*B^T + C. It is a bit faster than doing them separately
		 * 		avoiding the need to maintain an additional intermediate value.
		 * @param B 
		 * @param C 
		 * @return Matrix<T, SIMD_TYPE> 
		 */
		Matrix<T, SIMD_TYPE> fusedMultiplyAddTranspose(const Matrix<T, SIMD_TYPE>& B, const Matrix<T, SIMD_TYPE>& C) const;
		
		
		/**
		 * @brief Returns the sum of all the elements in the matrix.
		 * 
		 * @return T 
		 */
		T sum() const;

		/**
		 * @brief Returns a Matrix that stores the sum across each row
		 * 		in each row. Returns a column vector as a Matrix
		 * 
		 * @return Matrix<T, SIMD_TYPE> 
		 */
		Matrix<T, SIMD_TYPE> horizontalSum() const;

		/**
		 * @brief Returns a Matrix that store the sum across each column
		 * 		in each column. Returns a row vector as a Matrix
		 * 
		 * @return Matrix<T, SIMD_TYPE> 
		 */
		Matrix<T, SIMD_TYPE> verticalSum() const;

		/**
		 * @brief Normalizes the matrix such that the sum of all elements is
		 * 		equal to 1.0. It maintains the relavant distance or ratio
		 * 		between elements.
		 * 
		 */
		void normalize();

		/**
		 * @brief Calculates the minimum of all values in the matrix.
		 * 
		 * @return T 
		 */
		T min() const;
		
		/**
		 * @brief Calculates the maximum of all values in the matrix.
		 * 
		 * @return T 
		 */
		T max() const;

		/**
		 * @brief Calculates the minimum and maximum of all values in the matrix
		 * 		and returns both of them.
		 * 
		 * @return std::pair<T, T> 
		 */
		std::pair<T, T> minMaxValues() const;

	protected:
		T* data = nullptr;

		unsigned int rows = 0;
		unsigned int columns = 0;

		bool valid = false;
	private:
		void createData();
		void deleteData();
	
	public:
		virtual const TypeInfo getClass() const;
		virtual void serialize(SerializedStreamable& output, DataFormatter& formatter); //for overriding
		virtual void deserialize(SerializedStreamable& input, DataFormatter& formatter); //for overriding
		virtual std::unordered_map<std::string, SerializedVariable<void>> getSerializedVariables();

	};


	//inlined code since its a template. LARGE AND NASTY
	template<typename T, typename SIMD_TYPE>
	inline const TypeInfo Matrix<T, SIMD_TYPE>::getClass() const
	{
		return TypeInfo::get<Matrix<T, SIMD_TYPE>>();
	}
	
	template<typename T, typename SIMD_TYPE>
	inline void Matrix<T, SIMD_TYPE>::serialize(SerializedStreamable& output, DataFormatter& formatter)
	{
		//custom serialization
		//write out rows, columns, then data (not as pointer)
		staticSerialize(output, formatter, rows);
		staticSerialize(output, formatter, columns);

		formatter.writeStart(output, DataFormatter::FORMAT_ARRAY, TypeInfo::get<T>(), "", rows*columns);
		for(size_t i=0; i<rows*columns; i++)
		{
			staticSerialize(output, formatter, data[i]);
		}
		formatter.writeEnd(output);//write end of array
	}
	
	template<typename T, typename SIMD_TYPE>
	inline void Matrix<T, SIMD_TYPE>::deserialize(SerializedStreamable& input, DataFormatter& formatter)
	{
		//custom deserialization
		//read in rows, columns, then data (not as pointer)
		staticDeserialize(input, formatter, rows);
		staticDeserialize(input, formatter, columns);
		if(data != nullptr)
			delete[] data;
		data = new T[rows*columns];

		size_t elements = formatter.readStart(input, DataFormatter::FORMAT_ARRAY, TypeInfo::get<T>(), "");
		if(elements != rows*columns)
			throw SimpleSerializationException("Failed to load Matrix. Array size is not the same as the Rows*Columns.");
		
		for(size_t i=0; i<rows*columns; i++)
		{
			staticDeserialize(input, formatter, data[i]);
		}
		formatter.readEnd(input);
	}
	
	template<typename T, typename SIMD_TYPE>
	inline std::unordered_map<std::string, SerializedVariable<void>> Matrix<T, SIMD_TYPE>::getSerializedVariables()
	{
		return {SERIALIZE_MAP(data), SERIALIZE_MAP(rows), SERIALIZE_MAP(columns)};
	}

	template<typename T, typename SIMD_TYPE>
	void Matrix<T, SIMD_TYPE>::createData()
	{
		if(data != nullptr)
			delete[] data;
		
		if (columns > 0 && rows > 0)
		{
			valid = true;
			data = new T[rows*columns];
			memset(data, 0, sizeof(T)*rows*columns);
		}
	}
	
	template<typename T, typename SIMD_TYPE>
	void Matrix<T, SIMD_TYPE>::deleteData()
	{
		if(data!=nullptr)
			delete[] data;
		
		valid = false;
	}
	
	template<typename T, typename SIMD_TYPE>
	Matrix<T, SIMD_TYPE>::Matrix()
	{
		this->columns = 0;
		this->rows = 0;
		data = nullptr;

		valid = false;
	}
	
	template<typename T, typename SIMD_TYPE>
	Matrix<T, SIMD_TYPE>::Matrix(unsigned int rows, unsigned int cols)
	{
		this->columns = cols;
		this->rows = rows;
		createData();
	}
	
	template<typename T, typename SIMD_TYPE>
	Matrix<T, SIMD_TYPE>::Matrix(const Matrix<T, SIMD_TYPE>& c)
	{
		this->copy(c);
	}

	template<typename T, typename SIMD_TYPE>
	void Matrix<T, SIMD_TYPE>::operator=(const Matrix<T, SIMD_TYPE>& c)
	{
		this->copy(c);
	}

	template<typename T, typename SIMD_TYPE>
	void Matrix<T, SIMD_TYPE>::copy(const Matrix<T, SIMD_TYPE>& c)
	{
		deleteData();

		valid = c.valid;
		rows = c.rows;
		columns = c.columns;
		
		if(c.data!=nullptr)
		{
			createData();
			memcpy(data, c.data, rows*columns*sizeof(T));
		}
	}

	template<typename T, typename SIMD_TYPE>
	Matrix<T, SIMD_TYPE>::~Matrix()
	{
		deleteData();
		rows = 0;
		columns = 0;
	}

	template<typename T, typename SIMD_TYPE>
	T* Matrix<T, SIMD_TYPE>::operator[](unsigned int row) const
	{
		return &data[row*columns];
	}

	template<typename T, typename SIMD_TYPE>
	Matrix<T, SIMD_TYPE> Matrix<T, SIMD_TYPE>::operator*(T value) const
	{
		Matrix<T, SIMD_TYPE> m = Matrix<T, SIMD_TYPE>(rows, columns); //creates a matrix of the same datatype
		size_t simdBound = 0;

		#if (SIMD_TYPE != void)
		SIMD_TYPE simdValue = SIMD_TYPE(value);
		simdBound = SIMD_TYPE::getSimdBound(rows*columns);

		#pragma omp parallel for
		for (size_t i = 0; i < simdBound; i+=SIMD_TYPE::SIZE)
		{
			SIMD_TYPE dataLoaded = SIMD_TYPE(&data[i]);
			dataLoaded *= simdValue;
			dataLoaded.store(&m.data[i]);
		}
		#endif

		#pragma omp parallel for
		for (size_t i = simdBound; i < rows*columns; i++)
		{
			m.data[i] = value * data[i];
		}

		return m;
	}

	template<typename T, typename SIMD_TYPE>
	Matrix<T, SIMD_TYPE> Matrix<T, SIMD_TYPE>::operator/(T value) const
	{
		Matrix<T, SIMD_TYPE> m = Matrix<T, SIMD_TYPE>(rows, columns); //creates a matrix of the same datatype
		size_t simdBound = 0;

		#if (SIMD_TYPE != void)
		SIMD_TYPE simdValue = SIMD_TYPE(value);
		simdBound = SIMD_TYPE::getSimdBound(rows*columns);

		#pragma omp parallel for
		for (size_t i = 0; i < simdBound; i+=SIMD_TYPE::SIZE)
		{
			SIMD_TYPE dataLoaded = SIMD_TYPE(&data[i]);
			dataLoaded /= simdValue;
			dataLoaded.store(&m.data[i]);
		}
		#endif

		#pragma omp parallel for
		for (size_t i = simdBound; i < rows*columns; i++)
		{
			m.data[i] = data[i] / value;
		}

		return m;
	}

	template<typename T, typename SIMD_TYPE>
	Matrix<T, SIMD_TYPE> Matrix<T, SIMD_TYPE>::operator*(const Matrix<T, SIMD_TYPE>& other) const
	{
		return multiply(other);
	}

	template<typename T, typename SIMD_TYPE>
	void Matrix<T, SIMD_TYPE>::operator*=(T value)
	{
		size_t simdBound = 0;
		#if (SIMD_TYPE != void)
		SIMD_TYPE simdValue = SIMD_TYPE(value);
		simdBound = SIMD_TYPE::getSimdBound(rows*columns);

		#pragma omp parallel for
		for (size_t i = 0; i < simdBound; i+=SIMD_TYPE::SIZE)
		{
			SIMD_TYPE dataLoaded = SIMD_TYPE(&data[i]);
			dataLoaded *= simdValue;
			dataLoaded.store(&data[i]);
		}
		#endif

		#pragma omp parallel for
		for (size_t i = simdBound; i < rows*columns; i++)
		{
			data[i] *= value;
		}
	}

	template<typename T, typename SIMD_TYPE>
	void Matrix<T, SIMD_TYPE>::operator/=(T value)
	{
		size_t simdBound = 0;
		#if (SIMD_TYPE != void)
		SIMD_TYPE simdValue = SIMD_TYPE(value);
		simdBound = SIMD_TYPE::getSimdBound(rows*columns);

		#pragma omp parallel for
		for (size_t i = 0; i < simdBound; i+=SIMD_TYPE::SIZE)
		{
			SIMD_TYPE dataLoaded = SIMD_TYPE(&data[i]);
			dataLoaded /= simdValue;
			dataLoaded.store(&data[i]);
		}
		#endif

		#pragma omp parallel for
		for (size_t i = simdBound; i < rows*columns; i++)
		{
			data[i] /= value;
		}
	}

	template<typename T, typename SIMD_TYPE>
	Matrix<T, SIMD_TYPE> Matrix<T, SIMD_TYPE>::operator+(const Matrix<T, SIMD_TYPE>& other) const
	{
		if (rows == other.rows && columns == other.columns)
		{
			Matrix<T, SIMD_TYPE> m = Matrix<T, SIMD_TYPE>(rows, columns);

			size_t simdBound = 0;
			#if (SIMD_TYPE != void)
			simdBound = SIMD_TYPE::getSimdBound(rows*columns);

			#pragma omp parallel for
			for (size_t i = 0; i < simdBound; i+=SIMD_TYPE::SIZE)
			{
				SIMD_TYPE A = SIMD_TYPE(&data[i]);
				SIMD_TYPE B = SIMD_TYPE(&other.data[i]);
				A += B;
				A.store(&m.data[i]);
			}
			#endif
			#pragma omp parallel for
			for (size_t i = simdBound; i < rows*columns; i++)
			{
				m.data[i] = data[i] + other.data[i];
			}

			return m;
		}
		else
		{
			return Matrix<T, SIMD_TYPE>(0, 0); //TODO: THROW EXCEPTION. 
		}
	}

	template<typename T, typename SIMD_TYPE>
	void Matrix<T, SIMD_TYPE>::operator+=(const Matrix<T, SIMD_TYPE>& other)
	{
		if (columns == other.columns && rows == other.rows)
		{
			size_t simdBound = 0;
			#if (SIMD_TYPE != void)
			simdBound = SIMD_TYPE::getSimdBound(rows*columns);

			#pragma omp parallel for
			for (size_t i = 0; i < simdBound; i+=SIMD_TYPE::SIZE)
			{
				SIMD_TYPE A = SIMD_TYPE(&data[i]);
				SIMD_TYPE B = SIMD_TYPE(&other.data[i]);
				A += B;
				A.store(&data[i]);
			}
			#endif
			#pragma omp parallel for
			for (size_t i = simdBound; i < rows*columns; i++)
			{
				data[i] += other.data[i];
			}
		}
	}

	template<typename T, typename SIMD_TYPE>
	Matrix<T, SIMD_TYPE> Matrix<T, SIMD_TYPE>::operator-(const Matrix<T, SIMD_TYPE>& other) const
	{
		if (rows == other.rows && columns == other.columns)
		{
			Matrix<T, SIMD_TYPE> m = Matrix<T, SIMD_TYPE>(rows, columns);

			size_t simdBound = 0;
			#if (SIMD_TYPE != void)
			simdBound = SIMD_TYPE::getSimdBound(rows*columns);

			#pragma omp parallel for
			for (size_t i = 0; i < simdBound; i+=SIMD_TYPE::SIZE)
			{
				SIMD_TYPE A = SIMD_TYPE(&data[i]);
				SIMD_TYPE B = SIMD_TYPE(&other.data[i]);
				A -= B;
				A.store(&m.data[i]);
			}
			#endif
			#pragma omp parallel for
			for (size_t i = simdBound; i < rows*columns; i++)
			{
				m.data[i] = data[i] - other.data[i];
			}

			return m;
		}
		else
		{
			return Matrix<T, SIMD_TYPE>(0, 0); //TODO: THROW EXCEPTION. 
		}
	}

	template<typename T, typename SIMD_TYPE>
	void Matrix<T, SIMD_TYPE>::operator-=(const Matrix<T, SIMD_TYPE>& other)
	{
		if (columns == other.columns && rows == other.rows)
		{
			size_t simdBound = 0;
			#if (SIMD_TYPE != void)
			simdBound = SIMD_TYPE::getSimdBound(rows*columns);

			#pragma omp parallel for
			for (size_t i = 0; i < simdBound; i+=SIMD_TYPE::SIZE)
			{
				SIMD_TYPE A = SIMD_TYPE(&data[i]);
				SIMD_TYPE B = SIMD_TYPE(&other.data[i]);
				A -= B;
				A.store(&data[i]);
			}
			#endif
			#pragma omp parallel for
			for (size_t i = simdBound; i < rows*columns; i++)
			{
				data[i] -= other.data[i];
			}
		}
	}

	template<typename T, typename SIMD_TYPE>
	bool Matrix<T, SIMD_TYPE>::operator==(const Matrix<T, SIMD_TYPE>& other) const
	{
		if(rows != other.rows || columns != other.columns)
			return false;

		return memcmp(data, other.data, rows*columns*sizeof(T)) == 0;
	}

	template<typename T, typename SIMD_TYPE>
	bool Matrix<T, SIMD_TYPE>::operator!=(const Matrix<T, SIMD_TYPE>& other) const
	{
		return !(this->operator==(other));
	}

	
	template<typename T, typename SIMD_TYPE>
	Matrix<T, SIMD_TYPE> Matrix<T, SIMD_TYPE>::getIdentityMatrix(unsigned int rows)
	{
		Matrix<T, SIMD_TYPE> res = Matrix<T, SIMD_TYPE>(rows, rows);
		for(size_t i=0; i<rows; i++)
			res[i][i] = 1;
		
		return res;
	}

	
	template<typename T, typename SIMD_TYPE>
	void Matrix<T, SIMD_TYPE>::setAllValues(T v)
	{
		size_t simdBound = 0;
		#if (SIMD_TYPE != void)
		simdBound = SIMD_TYPE::getSIMDBound(rows*columns);
		SIMD_TYPE setValue = v;
		#pragma omp parallel for
		for (size_t i = 0; i < simdBound; i+=SIMD_TYPE::SIZE)
		{
			setValue.store(&data[i]);
		}
		#endif

		#pragma omp parallel for
		for (size_t i = simdBound; i < rows*columns; i++)
		{
			data[i] = v;
		}
	}

	
	template<typename T, typename SIMD_TYPE>
	Matrix<T, SIMD_TYPE> Matrix<T, SIMD_TYPE>::broadcastAdd(T v) const
	{
		Matrix<T, SIMD_TYPE> result = Matrix<T, SIMD_TYPE>(rows, columns);
		size_t simdBound = 0;
		#if (SIMD_TYPE != void)
			simdBound = SIMD_TYPE::getSIMDBound(rows*columns);
			SIMD_TYPE addV = v;
			#pragma omp parallel for
			for(size_t i=0; i<simdSize; i+=SIMD_TYPE::SIZE)
			{
				SIMD_TYPE v = SIMD_TYPE::load(&data[i]);
				v += addV;
				v.store(&result.data[i]);
			}
		#endif
		for(size_t i=simdBound; i<rows*columns; i++)
			result.data[i] = data[i] + v;
		return result;
	}
	
	template<typename T, typename SIMD_TYPE>
	Matrix<T, SIMD_TYPE> Matrix<T, SIMD_TYPE>::broadcastAdd(const Matrix<T, SIMD_TYPE>& B) const
	{
		if(B.rows != 1 && B.columns != 1)
			return Matrix<T, SIMD_TYPE>(); //throw an exception lol
		Matrix<T, SIMD_TYPE> result = Matrix<T, SIMD_TYPE>(rows, columns);
		size_t size = rows*columns;
		
		if(B.rows != 1)
		{
			//broadcast about the rows
			#if (SIMD_TYPE == void)
				#pragma omp parallel for
				for(size_t i=0; i<size; i++)
				{
					result.data[i] = data[i] + B.data[i/rows];
				}
			#else
				#pragma omp parallel for
				for(size_t i=0; i<rows; i++)
				{
					size_t simdSize = SIMD_TYPE::getSIMDBound(columns);
					SIMD_TYPE addV = B.data[i];
					for(size_t j=0; j<simdSize; j+=SIMD_TYPE::SIZE)
					{
						SIMD_TYPE v = SIMD_TYPE::load(&data[j + i*columns]);
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
			#if (SIMD_TYPE == void)
				#pragma omp parallel for
				for(size_t i=0; i<size; i++)
				{
					result.data[i] = data[i] + B.data[i%rows];
				}
			#else
				#pragma omp parallel for
				for(size_t i=0; i<rows; i++)
				{
					size_t simdSize = SIMD_TYPE::getSIMDBound(columns);
					for(size_t j=0; j<simdSize; j+=SIMD_TYPE::SIZE)
					{
						SIMD_TYPE v = SIMD_TYPE::load(&data[j + i*columns]);
						SIMD_TYPE addV = SIMD_TYPE::load(&B.data[j]);
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

	template<typename T, typename SIMD_TYPE>
	Matrix<T, SIMD_TYPE> Matrix<T, SIMD_TYPE>::broadcastSubtract(T v) const
	{
		Matrix<T, SIMD_TYPE> result = Matrix<T, SIMD_TYPE>(rows, columns);
		size_t size = rows*columns;
		size_t simdBound = 0;
		#if (SIMD_TYPE != void)
			simdBound = SIMD_TYPE::getSIMDBound(size);
			SIMD_TYPE addV = v;
			#pragma omp parallel for
			for(size_t i=0; i<simdBound; i+=SIMD_TYPE::SIZE)
			{
				SIMD_TYPE v = SIMD_TYPE::load(&data[i]);
				v -= addV;
				v.store(&result.data[i]);
			}
		#endif
		
		#pragma omp parallel for
		for(size_t i=simdBound; i<size; i++)
			result.data[i] = data[i] - v;
		
		return result;
	}
	
	template<typename T, typename SIMD_TYPE>
	Matrix<T, SIMD_TYPE> Matrix<T, SIMD_TYPE>::broadcastSubtract(const Matrix<T, SIMD_TYPE>& B) const
	{
		if(B.rows != 1 && B.columns != 1)
			return Matrix<T, SIMD_TYPE>(); //throw an exception lol
		Matrix<T, SIMD_TYPE> result = Matrix<T, SIMD_TYPE>(rows, columns);
		size_t size = rows*columns;
		
		if(B.rows != 1)
		{
			//broadcast about the rows
			#if (SIMD_TYPE == void)
				#pragma omp parallel for
				for(size_t i=0; i<size; i++)
				{
					result.data[i] = data[i] - B.data[i/rows];
				}
			#else
				#pragma omp parallel for
				for(size_t i=0; i<rows; i++)
				{
					size_t simdSize = SIMD_TYPE::getSIMDBound(columns);
					SIMD_TYPE subV = B.data[i];
					for(size_t j=0; j<simdSize; j+=SIMD_TYPE::SIZE)
					{
						SIMD_TYPE v = SIMD_TYPE::load(&data[j + i*columns]);
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
			#if (SIMD_TYPE == void)
				#pragma omp parallel for
				for(size_t i=0; i<size; i++)
				{
					result.data[i] = data[i] - B.data[i%rows];
				}
			#else
				#pragma omp parallel for
				for(size_t i=0; i<rows; i++)
				{
					size_t simdSize = SIMD_TYPE::getSIMDBound(columns);
					for(size_t j=0; j<simdSize; j+=SIMD_TYPE::SIZE)
					{
						SIMD_TYPE v = SIMD_TYPE::load(&data[j + i*columns]);
						SIMD_TYPE subV = SIMD_TYPE::load(&B.data[j]);
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

	
	template<typename T, typename SIMD_TYPE>
	#if(SIMD_TYPE != void)
	Matrix<T, SIMD_TYPE> Matrix<T, SIMD_TYPE>::broadcastFunction(std::function<T(T)> func, std::function<SIMD_TYPE(SIMD_TYPE)> simdFunc) const
	#else
	Matrix<T, SIMD_TYPE> Matrix<T, SIMD_TYPE>::broadcastFunction(std::function<T(T)> func, std::function<void()> simdFunc) const
	#endif
	{
		if(func == nullptr)
			throw std::bad_function_call();
		
		Matrix<T, SIMD_TYPE> result = Matrix<T, SIMD_TYPE>(rows, columns);
		size_t size = rows*columns;
		size_t simdBound = 0;
		#if (SIMD_TYPE != void)
			simdBound = SIMD_TYPE::getSIMDBound(size);
			if(simdFunc == nullptr)
				simdBound = 0; //Don't do any SIMD work
			#pragma omp parallel for
			for(size_t i=0; i<simdBound; i+=SIMD_TYPE::SIZE)
			{
				SIMD_TYPE v = SIMD_TYPE::load(&data[i]);
				v = simdFunc(v);
				v.store(&result.data[i]);
			}
		#endif

		#pragma omp parallel for
		for(size_t i=simdBound; i<size; i++)
			result.data[i] = func(data[i]);

		return result;
	}
	
	template<typename T, typename SIMD_TYPE>
	#if(SIMD_TYPE != void)
	Matrix<T, SIMD_TYPE> Matrix<T, SIMD_TYPE>::broadcastFunction(T (*func)(T), SIMD_TYPE (*simdFunc)(SIMD_TYPE)) const
	#else
	Matrix<T, SIMD_TYPE> Matrix<T, SIMD_TYPE>::broadcastFunction(T (*func)(T), void (*simdFunc)()) const
	#endif
	{
		if(func == nullptr)
			throw std::bad_function_call();
		
		Matrix<T, SIMD_TYPE> result = Matrix<T, SIMD_TYPE>(rows, columns);
		size_t size = rows*columns;
		size_t simdBound = 0;
		#if (SIMD_TYPE != void)
			simdBound = SIMD_TYPE::getSIMDBound(size);
			if(simdFunc == nullptr)
				simdBound = 0; //Don't do any SIMD work
			#pragma omp parallel for
			for(size_t i=0; i<simdBound; i+=SIMD_TYPE::SIZE)
			{
				SIMD_TYPE v = SIMD_TYPE::load(&data[i]);
				v = simdFunc(v);
				v.store(&result.data[i]);
			}
		#endif

		#pragma omp parallel for
		for(size_t i=simdBound; i<size; i++)
			result.data[i] = func(data[i]);

		return result;
	}

	template<typename T, typename SIMD_TYPE>
	T* Matrix<T, SIMD_TYPE>::getData() const
	{
		return data;
	}

	template<typename T, typename SIMD_TYPE>
	unsigned int Matrix<T, SIMD_TYPE>::getRows() const
	{
		return rows;
	}

	template<typename T, typename SIMD_TYPE>
	unsigned int Matrix<T, SIMD_TYPE>::getCols() const
	{
		return columns;
	}

	template<typename T, typename SIMD_TYPE>
	bool Matrix<T, SIMD_TYPE>::getValid() const
	{
		return valid;
	}

	template<typename T, typename SIMD_TYPE>
	T Matrix<T, SIMD_TYPE>::get(unsigned int row, unsigned int col) const
	{
		if (row < rows && row >= 0 && col < columns && col >= 0)
			return data[col + row*columns];
		else
			return T(); //should be 0 in most cases
	}

	template<typename T, typename SIMD_TYPE>
	Matrix<T, SIMD_TYPE> Matrix<T, SIMD_TYPE>::hadamardProduct(const Matrix<T, SIMD_TYPE>& other) const
	{
		if(rows != other.rows || columns != other.columns)
			return Matrix<T, SIMD_TYPE>();

		Matrix<T, SIMD_TYPE> m = Matrix<T, SIMD_TYPE>(rows, columns);
		size_t simdBound = 0;
		#if(SIMD_TYPE != void)
		simdBound = SIMD_TYPE::getSIMDBound(rows*columns);
		#pragma omp parallel for
		for (size_t i=0; i < simdBound; i+=SIMD_TYPE::SIZE)
		{
			SIMD_TYPE aValues = SIMD_TYPE::load(&data[i]);
			SIMD_TYPE bValues = SIMD_TYPE::load(&other.data[i]);
			aValues*=bValues;
			aValues.store(&m.data[i]);
		}
		#endif
		#pragma omp parallel for
		for(size_t i=simdBound; i<rows*columns; i++)
		{
			m.data[i] = data[i]*other.data[i];
		}

		return m;
	}
	
	template<typename T, typename SIMD_TYPE>
	Matrix<T, SIMD_TYPE> Matrix<T, SIMD_TYPE>::inverseHadamardProduct(const Matrix<T, SIMD_TYPE>& other) const
	{
		if(rows != other.rows || columns != other.columns)
			return Matrix<T, SIMD_TYPE>();

		Matrix<T, SIMD_TYPE> m = Matrix<T, SIMD_TYPE>(rows, columns);
		size_t simdBound = 0;
		#if (SIMD_TYPE != void)
		simdBound = SIMD_TYPE::getSIMDBound(rows*columns);
		#pragma omp parallel for
		for (size_t i=0; i < simdBound; i+=SIMD_TYPE::SIZE)
		{
			SIMD_TYPE aValues = SIMD_TYPE::load(&data[i]);
			SIMD_TYPE bValues = SIMD_TYPE::load(&other.data[i]);
			aValues *= bValues;
			aValues.store(&m.data[i]);
		}
		#endif
		#pragma omp parallel for
		for(size_t i=simdBound; i<rows*columns; i++)
		{
			m.data[i] = data[i]*other.data[i];
		}

		return m;
	}

	template<typename T, typename SIMD_TYPE>
	Matrix<T, SIMD_TYPE> Matrix<T, SIMD_TYPE>::getInverse() const
	{
		//TODO: SPEED UP. IS SLOW
		T det = getDeterminate();

		if(det!=0 && det!=NAN)
		{
			Matrix<T, SIMD_TYPE> inverse = Matrix<T, SIMD_TYPE>(rows, columns);
			
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
				size_t totalSize = rows*columns;
				#pragma omp parallel for
				for(size_t index = 0; index<totalSize; index++)
				{
					size_t i = index/columns;
					size_t j = index%columns;
					double mult = ((i+j)%2) ? -1 : 1;

					inverse[i][j] = mult * getMatrixOfMinors(i, j).getDeterminate();
				}
				if(typeid(T) == typeid(float) || typeid(T) == typeid(double))
					return inverse.getTranspose() * (1.0/det);
				else
					return inverse.getTranspose() / det;
			}
		}

		return Matrix<T, SIMD_TYPE>();
	}

	template<typename T, typename SIMD_TYPE>
	Matrix<T, SIMD_TYPE> Matrix<T, SIMD_TYPE>::getTranspose() const
	{
		Matrix<T, SIMD_TYPE> m = Matrix<T, SIMD_TYPE>(columns, rows);
		unsigned int totalSize = columns*rows;

		#pragma omp parallel for
		for(size_t i=0; i<totalSize; i++)
		{
			unsigned int y = i / columns;
			unsigned int x = i % columns;
			m.data[i] = data[y + x*columns];
		}

		return m;
	}

	template<typename T, typename SIMD_TYPE>
	T Matrix<T, SIMD_TYPE>::getDeterminate() const
	{
		if(rows == columns && rows > 1)
		{
			if(columns == 2)
			{
				return (data[0]*data[3]) - (data[1]*data[2]);
			}
			else
			{
				T sumValue = 0;
				#pragma omp parallel for
				for(size_t i=0; i<columns; i++)
				{
					T addV = data[i] * getMatrixOfMinors(0, i).getDeterminate();
					#pragma omp critical
					if(i%2 == 0)
						sumValue += addV;
					else
						sumValue -= addV;
				}
				return sumValue;
			}
		}

		return T();
	}

	template<typename T, typename SIMD_TYPE>
	Matrix<T, SIMD_TYPE> Matrix<T, SIMD_TYPE>::getMatrixOfMinors(unsigned int row, unsigned int col) const
	{
		Matrix<T, SIMD_TYPE> m = Matrix<T, SIMD_TYPE>(rows-1, columns-1);
		
		if(!m.getValid() || !getValid())
			throw InvalidMatrixSize();
	
		T* arrP = m.data;
		for(size_t i=0; i<rows; i++)
		{
			if(i != row)
			{
				T* dataP = arrP;
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

	template<typename T, typename SIMD_TYPE>
	void Matrix<T, SIMD_TYPE>::clear()
	{
		memset(data, 0, sizeof(T)*rows*columns);
	}
	
	template<typename T, typename SIMD_TYPE>
	Matrix<T, SIMD_TYPE> Matrix<T, SIMD_TYPE>::multiply(const Matrix<T, SIMD_TYPE>& A, const Matrix<T, SIMD_TYPE>& B)
	{
		return A.multiply(B);
	}
	
	template<typename T, typename SIMD_TYPE>
	Matrix<T, SIMD_TYPE> Matrix<T, SIMD_TYPE>::multiplyTranspose(const Matrix<T, SIMD_TYPE>& A, const Matrix<T, SIMD_TYPE>& B)
	{
		return A.multiplyTranspose(B);
	}
	
	template<typename T, typename SIMD_TYPE>
	Matrix<T, SIMD_TYPE> Matrix<T, SIMD_TYPE>::multiply(const Matrix<T, SIMD_TYPE>& B) const
	{
		//for large size matricies, do transpose then do matrix multiplication.
		if(!valid || !B.valid || columns != B.rows)
			throw InvalidMatrixSize();
		
		if(rows * B.columns > EXCEPTIONALY_LARGE_MATRIX_AREA)
		{
			Matrix<T, SIMD_TYPE> BT = B.getTranspose(); //O(N^2)
			return Matrix<T, SIMD_TYPE>::multiplyTranspose(BT); //O(N^3) but better cache locality
		}

		Matrix<T, SIMD_TYPE> C = Matrix<T, SIMD_TYPE>(rows, B.columns);
		#if (SIMD_TYPE == void)
		size_t columnsAdjusted = (C.columns>>2)<<2;
		size_t inc = 4;
		#else
		size_t columnsAdjusted = SIMD_TYPE::getSIMDBound(C.columns);
		size_t inc = SIMD_TYPE::SIZE;
		#endif

		#pragma omp parallel for
		for(size_t i=0; i<C.rows; i++)
		{
			#pragma omp parallel for
			for(size_t j=0; j<columnsAdjusted; j+=inc) //compute 4 at once
			{
				#if (SIMD_TYPE == void)
				T sums[4] = {0, 0, 0, 0};
				for(size_t k=0; k<columns; k++)
				{
					T aValue = data[k + i*columns];
					for(int block=0; block<4; block++)
					{
						sums[block] += aValue * B.data[j+block + k*B.columns];
					}
				}
				for(int block=0; block<4; block++)
				{
					C[i][j+block] = sums[block];
				}
				#else
				SIMD_TYPE sums = 0;
				for(size_t k=0; k<columns; k++)
				{
					SIMD_TYPE aValue = data[k + i*columns];
					SIMD_TYPE bValue;
					bValue.load(&B.data[j + k*B.columns]);
					sums += aValue * bValue;
				}
				sums.store(&C[i][j]);
				#endif
			}
			
			#pragma omp parallel for
			for(size_t j=columnsAdjusted; j<C.columns; j++)
			{
				T sum = 0;
				for(size_t k=0; k<columns; k++)
				{
					sum += data[k + i*columns] * B.data[j + k*B.columns];
				}
				C[i][j] = sum;
			}
		}
		return C;
	}

	template<typename T, typename SIMD_TYPE>
	Matrix<T, SIMD_TYPE> Matrix<T, SIMD_TYPE>::multiplyTranspose(const Matrix<T, SIMD_TYPE>& B) const
	{
		if(!valid || !B.valid || columns != B.columns)
			throw InvalidMatrixSize();
		
		Matrix<T, SIMD_TYPE> C = Matrix<T, SIMD_TYPE>(rows, B.rows);
		unsigned int totalSize = rows*B.rows;

		#pragma omp parallel for
		for(size_t i=0; i<totalSize; i++)
		{
			unsigned int y = i / B.rows;
			unsigned int x = i % B.rows;
			#if (SIMD_TYPE == void)
				T sum = 0;
				for(unsigned int k=0; k<columns; k++)
				{
					sum += data[k + y*columns] * B.data[k + x*B.columns];
				}
				C.data[x + y*B.rows] = sum;
			#else
				T finalSum = 0;
				SIMD_TYPE simdSum = 0;
				unsigned int k = 0;
				unsigned int simdBound = SIMD_TYPE::getSIMDBound(columns);
				while(k < simdBound)
				{
					SIMD_TYPE aValues = SIMD_TYPE::load(&data[k + y*columns]);
					SIMD_TYPE bValues = SIMD_TYPE::load(&B.data[k + x*B.columns]);
					simdSum += aValues*bValues;
					k+=SIMD_TYPE::SIZE;
				}
				while(k < columns)
				{
					finalSum += data[k + y*columns] * B.data[k + x*B.columns];
					k++;
				}

				finalSum += simdSum.sum();
				C.data[x + y*B.rows] = finalSum;
			#endif
		
		}
		return C;
	}
	
	template<typename T, typename SIMD_TYPE>
	Matrix<T, SIMD_TYPE> Matrix<T, SIMD_TYPE>::fusedMultiplyAdd(const Matrix<T, SIMD_TYPE>& A, const Matrix<T, SIMD_TYPE>& B, const Matrix<T, SIMD_TYPE>& C)
	{
		return A.fusedMultiplyAdd(B, C);
	}
	
	template<typename T, typename SIMD_TYPE>
	Matrix<T, SIMD_TYPE> Matrix<T, SIMD_TYPE>::fusedMultiplyAddTranspose(const Matrix<T, SIMD_TYPE>& A, const Matrix<T, SIMD_TYPE>& B, const Matrix<T, SIMD_TYPE>& C)
	{
		return A.fusedMultiplyAddTranspose(B, C);
	}

	template<typename T, typename SIMD_TYPE>
	Matrix<T, SIMD_TYPE> Matrix<T, SIMD_TYPE>::fusedMultiplyAdd(const Matrix<T, SIMD_TYPE>& B, const Matrix<T, SIMD_TYPE>& C) const
	{
		//for large size matricies, do transpose then do matrix multiplication.
		if(!valid || !B.valid || columns != B.rows)
			throw InvalidMatrixSize();
		
		if(C.rows != rows || C.columns != B.columns)
			throw InvalidMatrixSize();
			
		if(rows * B.columns > EXCEPTIONALY_LARGE_MATRIX_AREA)
		{
			Matrix<T, SIMD_TYPE> BT = B.getTranspose(); //O(N^2)
			return Matrix<T, SIMD_TYPE>::fusedMultiplyAddTranspose(BT, C); //O(N^3) but better cache locality
		}

		Matrix<T, SIMD_TYPE> result = Matrix<T, SIMD_TYPE>(rows, B.columns);
		#if (SIMD_TYPE == void)
		size_t columnsAdjusted = (result.columns>>2)<<2;
		size_t inc = 4;
		#else
		size_t columnsAdjusted = SIMD_TYPE::getSIMDBound(result.columns);
		size_t inc = SIMD_TYPE::SIZE;
		#endif

		#pragma omp parallel for
		for(size_t i=0; i<result.rows; i++)
		{
			#pragma omp parallel for
			for(size_t j=0; j<columnsAdjusted; j+=inc) //compute 4 at once
			{
				#if (SIMD_TYPE == void)
				T sums[4] = {C[i][j], C[i][j+1], C[i][j+2], C[i][j+3]};
				for(size_t k=0; k<columns; k++)
				{
					T aValue = data[k + i*columns];
					for(int block=0; block<4; block++)
					{
						sums[block] += aValue * B.data[j+block + k*B.columns];
					}
				}
				for(int block=0; block<4; block++)
				{
					result[i][j+block] = sums[block];
				}
				#else
				SIMD_TYPE sums;
				sums.load(&C.data[i + j*result.columns]);
				
				for(size_t k=0; k<columns; k++)
				{
					SIMD_TYPE aValue = data[k + i*columns];
					SIMD_TYPE bValue;
					bValue.load(&B.data[j + k*B.columns]);
					sums += aValue * bValue;
				}
				sums.store(&result[i][j]);
				#endif
			}
			
			#pragma omp parallel for
			for(size_t j=columnsAdjusted; j<result.columns; j++)
			{
				T sum = C[i][j];
				for(size_t k=0; k<columns; k++)
				{
					sum += data[k + i*columns] * B.data[j + k*B.columns];
				}
				result[i][j] = sum;
			}
		}
		return result;
	}
	
	template<typename T, typename SIMD_TYPE>
	Matrix<T, SIMD_TYPE> Matrix<T, SIMD_TYPE>::fusedMultiplyAddTranspose(const Matrix<T, SIMD_TYPE>& B, const Matrix<T, SIMD_TYPE>& C) const
	{
		if(!valid || !B.valid || columns != B.columns)
			throw InvalidMatrixSize();
		
		if(C.rows != rows || C.columns != B.rows)
			throw InvalidMatrixSize();
		
		Matrix<T, SIMD_TYPE> result = Matrix<T, SIMD_TYPE>(rows, B.rows);
		unsigned int totalSize = rows*B.rows;

		#pragma omp parallel for
		for(size_t i=0; i<totalSize; i++)
		{
			unsigned int y = i / B.rows;
			unsigned int x = i % B.rows;
			#if (SIMD_TYPE == void)
				T sum = C[y][x];
				for(unsigned int k=0; k<columns; k++)
				{
					sum += data[k + y*columns] * B.data[k + x*B.columns];
				}
				result.data[x + y*B.rows] = sum;
			#else
				T finalSum = C[y][x];
				SIMD_TYPE simdSum = 0;
				unsigned int k = 0;
				unsigned int simdBound = SIMD_TYPE::getSIMDBound(columns);
				while(k < simdBound)
				{
					SIMD_TYPE aValues = SIMD_TYPE::load(&data[k + y*columns]);
					SIMD_TYPE bValues = SIMD_TYPE::load(&B.data[k + x*B.columns]);
					simdSum += aValues*bValues;
					k+=SIMD_TYPE::SIZE;
				}
				while(k < columns)
				{
					finalSum += data[k + y*columns] * B.data[k + x*B.columns];
					k++;
				}

				finalSum += simdSum.sum();
				result.data[x + y*B.rows] = finalSum;
			#endif
		
		}
		return result;
	}

	template<typename T, typename SIMD_TYPE>
	T Matrix<T, SIMD_TYPE>::sum() const
	{
		T sum = 0;
		for(size_t i=0; i<rows*columns; i++)
		{
			sum += data[i];
		}
		return sum;
	}

	template<typename T, typename SIMD_TYPE>
	Matrix<T, SIMD_TYPE> Matrix<T, SIMD_TYPE>::horizontalSum() const
	{
		Matrix<T, SIMD_TYPE> m = Matrix<T, SIMD_TYPE>(rows, 1);
		#pragma omp parallel for
		for(size_t i=0; i<rows; i++)
		{
			T sum = 0;
			for(size_t j=0; j<columns; j++)
			{
				sum += data[j + i*rows];
			}
			m[i][0] = sum;
		}
		return m;
	}

	template<typename T, typename SIMD_TYPE>
	Matrix<T, SIMD_TYPE> Matrix<T, SIMD_TYPE>::verticalSum() const
	{
		Matrix<T, SIMD_TYPE> m = Matrix<T, SIMD_TYPE>(1, columns);
		for(size_t i=0; i<rows; i++)
		{
			for(size_t j=0; j<columns; j++)
			{
				m[0][j] += data[j + i*rows];
			}
		}
		return m;
	}

	template<typename T, typename SIMD_TYPE>
	void Matrix<T, SIMD_TYPE>::normalize()
	{
		T sValue = sum();
		if(sValue != 0)
		{
			if(typeid(T) == typeid(float) || typeid(T) == typeid(double))
			{
				T adjustment = 1.0/sValue;
				operator*=(adjustment);
			}
			else
			{
				operator/=(adjustment);
			}
		}
	}
	
	template<typename T, typename SIMD_TYPE>
	T Matrix<T, SIMD_TYPE>::min() const
	{
		if(rows*columns > 0)
		{
			T minValue = data[0];
			for(size_t i=1; i<rows*columns; i++)
			{
				minValue = (minValue < data[i]) ? minValue : data[i];
			}
			return minValue;
		}
		return 0;
	}
	
	template<typename T, typename SIMD_TYPE>
	T Matrix<T, SIMD_TYPE>::max() const
	{
		if(rows*columns > 0)
		{
			T maxValue = data[0];
			for(size_t i=1; i<rows*columns; i++)
			{
				maxValue = (maxValue > data[i]) ? maxValue : data[i];
			}
			return maxValue;
		}
		return 0;
	}
	
	template<typename T, typename SIMD_TYPE>
	std::pair<T, T> Matrix<T, SIMD_TYPE>::minMaxValues() const
	{
		if(rows*columns > 0)
		{
			std::pair<T, T> pValues = {data[0], data[0]};
			for(size_t i=1; i<rows*columns; i++)
			{
				pValues.first = (pValues.first < data[i]) ? pValues.first : data[i];
				pValues.second = (pValues.second > data[i]) ? pValues.second : data[i];
			}
			return pValues;
		}
		return {0, 0};
	}


	
} //NAMESPACE smpl END