#pragma once
#include "BuildOptions.h"
#include "Concurrency.h"
#include <math.h>
#include "SimpleSerialization.h"
#include <functional>
#include "GeneralExceptions.h"
#include "SIMD.h"
#include <vector>
#include "NewStringFormatting.h"

#ifdef min
	#undef min
#endif

#ifdef max
	#undef max
#endif

namespace smpl
{
	template<typename T>
	class Matrix;
	
	typedef Matrix<int8_t> MatrixInt8;
	typedef Matrix<uint8_t> MatrixUInt8;
	typedef Matrix<int16_t> MatrixInt16;
	typedef Matrix<uint16_t> MatrixUInt16;
	typedef Matrix<int32_t> MatrixInt32;
	typedef Matrix<uint32_t> MatrixUInt32;
	typedef Matrix<float> MatrixF;
	typedef Matrix<double> MatrixD;
	
	//exceptions
	struct InvalidMatrixSize : public std::exception
	{
		const char* what() const noexcept { return "Matrix/Matrices are an incorrect size to do operation"; }
	};
	
	struct InvalidMatrix : public std::exception
	{
		const char* what() const noexcept { return "Matrix is invalid. It may have been destroyed or not created with any dimensions"; }
	};

	struct InvalidMatrixCreation : public std::exception
	{
		const char* what() const noexcept { return "Matrix dimensions are incorrect to create the matrix. Columns may not be the same across each row."; }
	};
	
	struct InvalidIndex : public std::exception
	{
		const char* what() const noexcept { return "The index provided is not valid."; }
	};
	
	struct InvalidInverse : public std::exception
	{
		const char* what() const noexcept { return "The determinate is 0 or the matrix isn't square and therefore the inverse can't be computed."; }
	};

	template<typename T>
	class Matrix : public SerializedObject
	{
	public:
		static const size_t EXCEPTIONALY_LARGE_MATRIX_AREA = 0xFFFF;
		

		
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
		 * @brief Constructs a new Matrix from an existing 2D vector of data.
		 * 			The data must have the same number of columns per row.
		 */
		Matrix(const std::vector<std::vector<T>>& data);

		/**
		 * @brief Construct a new Matrix object from another Matrix object
		 * 
		 * @param o
		 * 		The matrix to copy.
		 */
		Matrix(const Matrix<T>& o);

		/**
		 * @brief Copies a Matrix object
		 * 
		 * @param o
		 * 		The matrix to copy.
		 */
		void operator=(const Matrix<T>& o);

		/**
		 * @brief Destroy the Matrix object
		 * 
		 */
		virtual ~Matrix();

		/**
		 * @brief Copies a Matrix object
		 * 
		 * @param o
		 * 		The matrix to copy.
		 */
		void copy(const Matrix<T>& o);

		/**
		 * @brief Returns a pointer to the start of the specified row
		 * 		Note that the internal data structure is a 1D array.
		 * @param row 
		 * @return T* 
		 */
		T* operator[](unsigned int row) const;

		Matrix<T> operator*(T value) const;
		Matrix<T> operator/(T value) const;
		Matrix<T> operator*(const Matrix<T>& other) const;
		Matrix<T> operator+(const Matrix<T>& other) const;
		Matrix<T> operator-(const Matrix<T>& other) const;
		Matrix<T> operator-() const;

		bool operator==(const Matrix<T>& other) const;
		bool operator!=(const Matrix<T>& other) const;

		void operator*=(T value);
		void operator/=(T value);
		void operator+=(const Matrix<T>& other);
		void operator-=(const Matrix<T>& other);

		friend Matrix<T> operator*(T value, const Matrix<T>& other)
		{
			return other*value;
		}

		/**
		 * @brief Returns an Identity Matrix which must be a square matrix.
		 * 
		 * @param rows 
		 * @return Matrix<T> 
		 */
		static Matrix<T> getIdentityMatrix(unsigned int rows);

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
		 * @return Matrix<T> 
		 */
		Matrix<T> broadcastAdd(T v) const;

		/**
		 * @brief Attempts to do an element wise addition using a
		 * 		row or column matrix "v". Must be a row or column matrix.
		 * 		Must be an appropriate size as well.
		 * 
		 * @param v 
		 * @return Matrix<T> 
		 */
		Matrix<T> broadcastAdd(const Matrix<T>& v) const;

		
		/**
		 * @brief Performs an element wise subtraction.
		 * 		Specifically does : this-v
		 * 		Equivalent to making the input value "v" into a matrix
		 * 		consisting of just the value "v" and subtraction.
		 * 
		 * @param v 
		 * @return Matrix<T> 
		 */
		Matrix<T> broadcastSubtract(T v) const;

		/**
		 * @brief Performs an element wise subtraction.
		 * 		Specifically does : v-this
		 * 		Equivalent to making the input value "v" into a matrix
		 * 		consisting of just the value "v" and subtraction.
		 * 		
		 * 
		 * @param v 
		 * @return Matrix<T> 
		 */
		Matrix<T> broadcastInverseSubtract(T v) const;
		
		/**
		 * @brief Attempts to do an element wise subtraction using a
		 * 		row or column matrix "v". Must be a row or column matrix.
		 * 		Must be an appropriate size as well.
		 * 
		 * @param v 
		 * @return Matrix<T> 
		 */
		Matrix<T> broadcastSubtract(const Matrix<T>& v) const;

		/**
		 * @brief Attempts to apply the input function to every element.
		 * 		If possible and OPTI has been defined as a value > 0,
		 * 		an SIMD version may be included for additional speed.
		 * 
		 * 		This version allows lambdas that capture a value.
		 * 
		 * @param func 
		 * @param simdFunc 
		 * @return Matrix<T> 
		 */
		Matrix<T> broadcastFunction(std::function<T(T)> func, std::function<SIMD_TEMPLATE<T>(SIMD_TEMPLATE<T>)> simdFunc) const;
		
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
		 * @return Matrix<T> 
		 */
		Matrix<T> broadcastFunction(T (*func)(T), SIMD_TEMPLATE<T> (*simdFunc)(SIMD_TEMPLATE<T>)) const;

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
		unsigned int getColumns() const;

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
		 * @return Matrix<T> 
		 */
		Matrix<T> hadamardProduct(const Matrix<T>& other) const;

		/**
		 * @brief Attempts to broadcast a row or column matrix to the correct size
		 * 		and perform a hadamard product. Must be an appropriate size.
		 * 
		 * @param other 
		 * @return Matrix<T> 
		 */
		Matrix<T> broadcastHadamardProduct(const Matrix<T>& other) const;

		/**
		 * @brief Returns the elementwise division of the 2 matrices.
		 * 		The hadamard product but A * 1.0/B. 
		 * 		Does not check for division by 0
		 * 
		 * @param other 
		 * @return Matrix<T> 
		 */
		Matrix<T> inverseHadamardProduct(const Matrix<T>& other) const;
		
		/**
		 * @brief Attempts to broadcast a row or column matrix to the correct size
		 * 		and perform a inverse hadamard product. Must be an appropriate size.
		 * 			The hadamard product but A * 1.0/B.
		 * 
		 * @param other 
		 * @return Matrix<T> 
		 */
		Matrix<T> broadcastInverseHadamardProduct(const Matrix<T>& other) const;

		/**
		 * @brief Gets the Inverse of the matrix.
		 * 		The inverse only exists if the determinate is not 0 and the matrix is square.
		 * 		An invalid matrix is returned if the function fails.
		 * 
		 * @return Matrix<T> 
		 */
		Matrix<T> getInverse() const;

		/**
		 * @brief Gets the Transpose of the matrix.
		 * 		Swaps the rows with the columns.
		 * 
		 * @return Matrix<T> 
		 */
		Matrix<T> getTranspose() const;

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
		 * @return Matrix<T> 
		 */
		Matrix<T> getMatrixOfMinors(unsigned int row, unsigned int col) const;

		void clear();

		/**
		 * @brief Multiplies 2 matrices.
		 * 		Specifically does A * B
		 * 
		 * @param A 
		 * @param B 
		 * @return Matrix<T> 
		 */
		static Matrix<T> multiply(const Matrix<T>& A, const Matrix<T>& B);
		Matrix<T> multiply(const Matrix<T>& B) const;

		/**
		 * @brief Multiplies 2 matrices where the transpose of B is used.
		 * 		Specifically does A * B^T
		 * 		Skips the transpose operation which can save on resources.
		 * 
		 * @param A 
		 * @param B 
		 * @return Matrix<T> 
		 */
		static Matrix<T> multiplyTranspose(const Matrix<T>& A, const Matrix<T>& B);
		Matrix<T> multiplyTranspose(const Matrix<T>& B) const;

		/**
		 * @brief Computes A*B + C. It is a bit faster than doing them separately
		 * 		avoiding the need to maintain an additional intermediate value.
		 * 
		 * @param A 
		 * @param B 
		 * @param C 
		 * @return Matrix<T> 
		 */
		static Matrix<T> fusedMultiplyAdd(const Matrix<T>& A, const Matrix<T>& B, const Matrix<T>& C);

		/**
		 * @brief Computes A*B + C. It is a bit faster than doing them separately
		 * 		avoiding the need to maintain an additional intermediate value.
		 * @param B 
		 * @param C 
		 * @return Matrix<T> 
		 */
		Matrix<T> fusedMultiplyAdd(const Matrix<T>& B, const Matrix<T>& C) const;
		
		/**
		 * @brief Computes A*B^T + C. It is a bit faster than doing them separately
		 * 		avoiding the need to maintain an additional intermediate value.
		 * 
		 * @param A 
		 * @param B 
		 * @param C 
		 * @return Matrix<T> 
		 */
		static Matrix<T> fusedMultiplyAddTranspose(const Matrix<T>& A, const Matrix<T>& B, const Matrix<T>& C);

		/**
		 * @brief Computes A*B^T + C. It is a bit faster than doing them separately
		 * 		avoiding the need to maintain an additional intermediate value.
		 * @param B 
		 * @param C 
		 * @return Matrix<T> 
		 */
		Matrix<T> fusedMultiplyAddTranspose(const Matrix<T>& B, const Matrix<T>& C) const;
		
		
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
		 * @return Matrix<T> 
		 */
		Matrix<T> horizontalSum() const;

		/**
		 * @brief Returns a Matrix that store the sum across each column
		 * 		in each column. Returns a row vector as a Matrix
		 * 
		 * @return Matrix<T> 
		 */
		Matrix<T> verticalSum() const;

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
		 * @brief Calculates the minimum across each row and stores as a row vector
		 * 
		 * @return Matrix<T>
		 */
		Matrix<T> minAcrossRows() const;
		
		/**
		 * @brief Calculates the minimum across each column and stores as a column vector
		 * 
		 * @return Matrix<T>
		 */
		Matrix<T> minAcrossCols() const;
		
		/**
		 * @brief Calculates the maximum of all values in the matrix.
		 * 
		 * @return T 
		 */
		T max() const;

		/**
		 * @brief Calculates the maximum across each row and stores as a row vector
		 * 
		 * @return Matrix<T>
		 */
		Matrix<T> maxAcrossRows() const;
		
		/**
		 * @brief Calculates the maximum across each column and stores as a column vector
		 * 
		 * @return Matrix<T>
		 */
		Matrix<T> maxAcrossCols() const;

		/**
		 * @brief Calculates the minimum and maximum of all values in the matrix
		 * 		and returns both of them.
		 * 
		 * @return std::pair<T, T> 
		 */
		std::pair<T, T> minMaxValues() const;

		/**
		 * @brief Calculates the average across the entire matrix.
		 * 
		 * @return T 
		 */
		T mean() const;

		/**
		 * @brief Calculates the average across each column and stores as a column vector
		 * 
		 * @return Matrix<T> 
		 */
		Matrix<T> meanAcrossCols() const;

		/**
		 * @brief Calculate the average across each row and stores as a row vector
		 * 
		 * @return Matrix<T> 
		 */
		Matrix<T> meanAcrossRows() const;

		/**
		 * @brief Attempts to reshape the matrix into a new form.
		 * 		The newRows and newColumns must multiply into the same total size.
		 * 			Internally this changes nothing as the data is already stored as a continuous 1D array.
		 * 			This just changes the interpretation of that data.
		 */
		void reshape(unsigned int newRows, unsigned int newColumns);
		
		/**
		 * @brief Attempts to extract data from the matrix into a new matrix.
		 * 		This can be used to extract a specific row/column/square area
		 * 			Clamped down to the size of the matrix.
		 * 
		 * @param startRow 
		 * @param startColumn 
		 * @param endRow 
		 * @param endColumn 
		 * @return Matrix<T> 
		 */
		Matrix<T> extract(unsigned int startRow, unsigned int startColumn, unsigned int endRow, unsigned int endColumn) const;

		/**
		 * @brief Attempts to pad the matrix with the specified value
		 * 
		 * @param rowPadding 
		 * @param columnPadding 
		 * @return Matrix<T> 
		 */
		Matrix<T> pad(unsigned int rowPadding, unsigned int columnPadding, T paddingValue) const;

		// /**
		//  * @brief Attempts to join another matrix arr onto the end of the current matrix forming a new matrix.
		//  * 
		//  * @param arr 
		//  * @return Matrix<T> 
		//  */
		// Matrix<T> join(const Matrix<T>& arr) const;

		
        template<typename K>
        operator Matrix<K>() const
        {
			Matrix<K> output(rows, columns);
			K* ouptutDataP = output.getData();
			for(size_t i=0; i<rows*columns; i++)
			{
				ouptutDataP[i] = (K)data[i];
			}
            return output;
        }

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

	template<typename T>
	void formatToString(StringStream& stream, const Matrix<T>& mat, const std::string& options)
	{
		if(!mat.getValid())
		{
			stream.write("[INVALID]");
		}
		else
		{
			stream.write('[');
			for(size_t r=0; r<mat.getRows(); r++)
			{
				stream.write('[');
				for(size_t c=0; c<mat.getColumns(); c++)
				{
					formatToString(stream, mat[r][c], options);
					stream.write(" ");
				}
				stream.pop();
				stream.write("]\n");
			}
			stream.pop();
			stream.write(']');
		}
	}


	//inlined code since its a template. LARGE AND NASTY
	template<typename T>
	inline const TypeInfo Matrix<T>::getClass() const
	{
		return TypeInfo::get<Matrix<T>>();
	}
	
	template<typename T>
	inline void Matrix<T>::serialize(SerializedStreamable& output, DataFormatter& formatter)
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
	
	template<typename T>
	inline void Matrix<T>::deserialize(SerializedStreamable& input, DataFormatter& formatter)
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
	
	template<typename T>
	inline std::unordered_map<std::string, SerializedVariable<void>> Matrix<T>::getSerializedVariables()
	{
		return {SERIALIZE_MAP(data), SERIALIZE_MAP(rows), SERIALIZE_MAP(columns)};
	}

	template<typename T>
	void Matrix<T>::createData()
	{
		if(data != nullptr)
			delete[] data;
		
		if (columns > 0 && rows > 0)
		{
			valid = true;
			data = new T[rows*columns];
			for(size_t i=0; i<rows*columns; i++)
				data[i] = T();
		}
		else
		{
			throw InvalidMatrixCreation();
		}
	}
	
	template<typename T>
	void Matrix<T>::deleteData()
	{
		if(data!=nullptr)
			delete[] data;
		data = nullptr;
		valid = false;
	}
	
	template<typename T>
	Matrix<T>::Matrix()
	{
		this->columns = 0;
		this->rows = 0;
		data = nullptr;
		valid = false;
	}
	
	template<typename T>
	Matrix<T>::Matrix(unsigned int rows, unsigned int cols)
	{
		this->columns = cols;
		this->rows = rows;
		createData();
	}

	template<typename T>
	Matrix<T>::Matrix(const std::vector<std::vector<T>>& arr)
	{
		if(arr.size() == 0)
			return;
		
		this->rows = arr.size();
		this->columns = arr.front().size();
		createData();

		for(size_t i=0; i<rows; i++)
		{
			if(arr[i].size() != columns)
			{
				deleteData();
				throw InvalidMatrixCreation();
			}

			for(size_t j=0; j<columns; j++)
			{
				this->data[j + i*columns] = arr[i][j];
			}
		}
	}
	
	template<typename T>
	Matrix<T>::Matrix(const Matrix<T>& c)
	{
		this->copy(c);
	}

	template<typename T>
	void Matrix<T>::operator=(const Matrix<T>& c)
	{
		this->copy(c);
	}

	template<typename T>
	void Matrix<T>::copy(const Matrix<T>& c)
	{
		deleteData();

		valid = c.valid;
		rows = c.rows;
		columns = c.columns;
		
		if(c.data!=nullptr)
		{
			createData();
			for(size_t i=0; i<rows*columns; i++)
				data[i] = c.data[i];
		}
		else
		{
			throw InvalidMatrix();
		}
	}

	template<typename T>
	Matrix<T>::~Matrix()
	{
		deleteData();
		rows = 0;
		columns = 0;
	}

	template<typename T>
	T* Matrix<T>::operator[](unsigned int row) const
	{
		return &data[row*columns];
	}

	template<typename T>
	Matrix<T> Matrix<T>::operator*(T value) const
	{
		Matrix<T> m = Matrix<T>(rows, columns); //creates a matrix of the same datatype
		size_t simdBound = 0;

		SIMD_TEMPLATE<T> simdValue = SIMD_TEMPLATE<T>(value);
		simdBound = SIMD_TEMPLATE<T>::getSIMDBound(rows*columns);

		LARGE_ENOUGH_CLAUSE(simdBound)
		#pragma omp parallel for
		for (size_t i = 0; i < simdBound; i+=SIMD_TEMPLATE<T>::SIZE)
		{
			SIMD_TEMPLATE<T> dataLoaded = SIMD_TEMPLATE<T>::load(&data[i]);
			dataLoaded *= simdValue;
			dataLoaded.store(&m.data[i]);
		}

		LARGE_ENOUGH_CLAUSE((rows*columns) - simdBound)
		#pragma omp parallel for
		for (size_t i = simdBound; i < rows*columns; i++)
		{
			m.data[i] = value * data[i];
		}

		RESET_LARGE_ENOUGH_CLAUSE()
		return m;
	}

	template<typename T>
	Matrix<T> Matrix<T>::operator/(T value) const
	{
		Matrix<T> m = Matrix<T>(rows, columns); //creates a matrix of the same datatype
		size_t simdBound = 0;

		SIMD_TEMPLATE<T> simdValue = SIMD_TEMPLATE<T>(value);
		simdBound = SIMD_TEMPLATE<T>::getSIMDBound(rows*columns);

		LARGE_ENOUGH_CLAUSE(simdBound)
		#pragma omp parallel for
		for (size_t i = 0; i < simdBound; i+=SIMD_TEMPLATE<T>::SIZE)
		{
			SIMD_TEMPLATE<T> dataLoaded = SIMD_TEMPLATE<T>::load(&data[i]);
			dataLoaded /= simdValue;
			dataLoaded.store(&m.data[i]);
		}

		LARGE_ENOUGH_CLAUSE((rows*columns)-simdBound)
		#pragma omp parallel for
		for (size_t i = simdBound; i < rows*columns; i++)
		{
			m.data[i] = data[i] / value;
		}

		RESET_LARGE_ENOUGH_CLAUSE()
		return m;
	}

	template<typename T>
	Matrix<T> Matrix<T>::operator*(const Matrix<T>& other) const
	{
		return multiply(other);
	}

	template<typename T>
	void Matrix<T>::operator*=(T value)
	{
		size_t simdBound = 0;
		SIMD_TEMPLATE<T> simdValue = SIMD_TEMPLATE<T>(value);
		simdBound = SIMD_TEMPLATE<T>::getSIMDBound(rows*columns);

		LARGE_ENOUGH_CLAUSE(simdBound)
		#pragma omp parallel for
		for (size_t i = 0; i < simdBound; i+=SIMD_TEMPLATE<T>::SIZE)
		{
			SIMD_TEMPLATE<T> dataLoaded = SIMD_TEMPLATE<T>::load(&data[i]);
			dataLoaded *= simdValue;
			dataLoaded.store(&data[i]);
		}

		LARGE_ENOUGH_CLAUSE((rows*columns)-simdBound)
		#pragma omp parallel for
		for (size_t i = simdBound; i < rows*columns; i++)
		{
			data[i] *= value;
		}
		RESET_LARGE_ENOUGH_CLAUSE()
	}

	template<typename T>
	void Matrix<T>::operator/=(T value)
	{
		size_t simdBound = 0;
		SIMD_TEMPLATE<T> simdValue = SIMD_TEMPLATE<T>(value);
		simdBound = SIMD_TEMPLATE<T>::getSIMDBound(rows*columns);

		LARGE_ENOUGH_CLAUSE(simdBound)
		#pragma omp parallel for
		for (size_t i = 0; i < simdBound; i+=SIMD_TEMPLATE<T>::SIZE)
		{
			SIMD_TEMPLATE<T> dataLoaded = SIMD_TEMPLATE<T>::load(&data[i]);
			dataLoaded /= simdValue;
			dataLoaded.store(&data[i]);
		}

		LARGE_ENOUGH_CLAUSE((rows*columns)-simdBound)
		#pragma omp parallel for
		for (size_t i = simdBound; i < rows*columns; i++)
		{
			data[i] /= value;
		}
		RESET_LARGE_ENOUGH_CLAUSE()
	}

	template<typename T>
	Matrix<T> Matrix<T>::operator+(const Matrix<T>& other) const
	{
		if (rows == other.rows && columns == other.columns)
		{
			Matrix<T> m = Matrix<T>(rows, columns);

			size_t simdBound = 0;
			simdBound = SIMD_TEMPLATE<T>::getSIMDBound(rows*columns);

			LARGE_ENOUGH_CLAUSE(simdBound)
			#pragma omp parallel for
			for (size_t i = 0; i < simdBound; i+=SIMD_TEMPLATE<T>::SIZE)
			{
				SIMD_TEMPLATE<T> A = SIMD_TEMPLATE<T>::load(&data[i]);
				SIMD_TEMPLATE<T> B = SIMD_TEMPLATE<T>::load(&other.data[i]);
				A += B;
				A.store(&m.data[i]);
			}
			
			LARGE_ENOUGH_CLAUSE((rows*columns)-simdBound)
			#pragma omp parallel for
			for (size_t i = simdBound; i < rows*columns; i++)
			{
				m.data[i] = data[i] + other.data[i];
			}

			RESET_LARGE_ENOUGH_CLAUSE()
			return m;
		}
		else
		{
			throw InvalidMatrixSize();
		}
	}

	template<typename T>
	void Matrix<T>::operator+=(const Matrix<T>& other)
	{
		if (columns == other.columns && rows == other.rows)
		{
			size_t simdBound = 0;
			simdBound = SIMD_TEMPLATE<T>::getSIMDBound(rows*columns);

			LARGE_ENOUGH_CLAUSE(simdBound)
			#pragma omp parallel for
			for (size_t i = 0; i < simdBound; i+=SIMD_TEMPLATE<T>::SIZE)
			{
				SIMD_TEMPLATE<T> A = SIMD_TEMPLATE<T>::load(&data[i]);
				SIMD_TEMPLATE<T> B = SIMD_TEMPLATE<T>::load(&other.data[i]);
				A += B;
				A.store(&data[i]);
			}
			
			LARGE_ENOUGH_CLAUSE((rows*columns)-simdBound)
			#pragma omp parallel for
			for (size_t i = simdBound; i < rows*columns; i++)
			{
				data[i] += other.data[i];
			}
			RESET_LARGE_ENOUGH_CLAUSE()
		}
		else
		{
			throw InvalidMatrixSize();
		}
	}

	template<typename T>
	Matrix<T> Matrix<T>::operator-(const Matrix<T>& other) const
	{
		if (rows == other.rows && columns == other.columns)
		{
			Matrix<T> m = Matrix<T>(rows, columns);

			size_t simdBound = 0;
			simdBound = SIMD_TEMPLATE<T>::getSIMDBound(rows*columns);

			LARGE_ENOUGH_CLAUSE(simdBound)
			#pragma omp parallel for
			for (size_t i = 0; i < simdBound; i+=SIMD_TEMPLATE<T>::SIZE)
			{
				SIMD_TEMPLATE<T> A = SIMD_TEMPLATE<T>::load(&data[i]);
				SIMD_TEMPLATE<T> B = SIMD_TEMPLATE<T>::load(&other.data[i]);
				A -= B;
				A.store(&m.data[i]);
			}
			
			LARGE_ENOUGH_CLAUSE((rows*columns)-simdBound)
			#pragma omp parallel for
			for (size_t i = simdBound; i < rows*columns; i++)
			{
				m.data[i] = data[i] - other.data[i];
			}
			
			RESET_LARGE_ENOUGH_CLAUSE()
			return m;
		}
		else
		{
			throw InvalidMatrixSize();
		}
	}

	template<typename T>
	Matrix<T> Matrix<T>::operator-() const
	{
		Matrix<T> m = Matrix<T>(rows, columns);

		size_t simdBound = 0;
		simdBound = SIMD_TEMPLATE<T>::getSIMDBound(rows*columns);

		LARGE_ENOUGH_CLAUSE(simdBound)
		#pragma omp parallel for
		for (size_t i = 0; i < simdBound; i+=SIMD_TEMPLATE<T>::SIZE)
		{
			SIMD_TEMPLATE<T> A = SIMD_TEMPLATE<T>::load(&data[i]);
			A = -A;
			A.store(&m.data[i]);
		}

		LARGE_ENOUGH_CLAUSE((rows*columns)-simdBound)
		#pragma omp parallel for
		for (size_t i = simdBound; i < rows*columns; i++)
		{
			m.data[i] = -data[i];
		}

		RESET_LARGE_ENOUGH_CLAUSE()
		return m;
	}

	template<typename T>
	void Matrix<T>::operator-=(const Matrix<T>& other)
	{
		if (columns == other.columns && rows == other.rows)
		{
			size_t simdBound = 0;
			simdBound = SIMD_TEMPLATE<T>::getSIMDBound(rows*columns);

			LARGE_ENOUGH_CLAUSE(simdBound)
			#pragma omp parallel for
			for (size_t i = 0; i < simdBound; i+=SIMD_TEMPLATE<T>::SIZE)
			{
				SIMD_TEMPLATE<T> A = SIMD_TEMPLATE<T>::load(&data[i]);
				SIMD_TEMPLATE<T> B = SIMD_TEMPLATE<T>::load(&other.data[i]);
				A -= B;
				A.store(&data[i]);
			}
			
			LARGE_ENOUGH_CLAUSE((rows*columns)-simdBound)
			#pragma omp parallel for
			for (size_t i = simdBound; i < rows*columns; i++)
			{
				data[i] -= other.data[i];
			}
			RESET_LARGE_ENOUGH_CLAUSE()
		}
		else
		{
			throw InvalidMatrixSize();
		}
	}

	template<typename T>
	bool Matrix<T>::operator==(const Matrix<T>& other) const
	{
		if(rows != other.rows || columns != other.columns)
			return false;

		return memcmp(data, other.data, rows*columns*sizeof(T)) == 0;
	}

	template<typename T>
	bool Matrix<T>::operator!=(const Matrix<T>& other) const
	{
		return !(this->operator==(other));
	}

	
	template<typename T>
	Matrix<T> Matrix<T>::getIdentityMatrix(unsigned int rows)
	{
		Matrix<T> res = Matrix<T>(rows, rows);
		for(size_t i=0; i<rows; i++)
			res[i][i] = 1;
		
		return res;
	}

	
	template<typename T>
	void Matrix<T>::setAllValues(T v)
	{
		size_t simdBound = 0;
		simdBound = SIMD_TEMPLATE<T>::getSIMDBound(rows*columns);
		SIMD_TEMPLATE<T> setValue = v;

		LARGE_ENOUGH_CLAUSE(simdBound)
		#pragma omp parallel for
		for (size_t i = 0; i < simdBound; i+=SIMD_TEMPLATE<T>::SIZE)
		{
			setValue.store(&data[i]);
		}

		LARGE_ENOUGH_CLAUSE((rows*columns)-simdBound)
		#pragma omp parallel for
		for (size_t i = simdBound; i < rows*columns; i++)
		{
			data[i] = v;
		}
		RESET_LARGE_ENOUGH_CLAUSE()
	}

	
	template<typename T>
	Matrix<T> Matrix<T>::broadcastAdd(T v) const
	{
		Matrix<T> result = Matrix<T>(rows, columns);
		size_t simdBound = 0;
		simdBound = SIMD_TEMPLATE<T>::getSIMDBound(rows*columns);
		SIMD_TEMPLATE<T> addV = v;
		
		LARGE_ENOUGH_CLAUSE(simdBound)
		#pragma omp parallel for
		for(size_t i=0; i<simdBound; i+=SIMD_TEMPLATE<T>::SIZE)
		{
			SIMD_TEMPLATE<T> v = SIMD_TEMPLATE<T>::load(&data[i]);
			v += addV;
			v.store(&result.data[i]);
		}

		LARGE_ENOUGH_CLAUSE((rows*columns)-simdBound)
		#pragma omp parallel for
		for(size_t i=simdBound; i<rows*columns; i++)
			result.data[i] = data[i] + v;
		
		RESET_LARGE_ENOUGH_CLAUSE()
		return result;
	}
	
	template<typename T>
	Matrix<T> Matrix<T>::broadcastAdd(const Matrix<T>& B) const
	{
		if(B.rows != 1 && B.columns != 1)
			throw InvalidMatrixSize();
		Matrix<T> result = Matrix<T>(rows, columns);
		size_t size = rows*columns;
		
		if(B.rows != 1)
		{
			//broadcast about the rows
			LARGE_ENOUGH_CLAUSE(rows) //NOTE: Should it be done this way? Should maybe be done using total area
			#pragma omp parallel for
			for(size_t i=0; i<rows; i++)
			{
				size_t simdSize = SIMD_TEMPLATE<T>::getSIMDBound(columns);
				SIMD_TEMPLATE<T> addV = B.data[i];
				for(size_t j=0; j<simdSize; j+=SIMD_TEMPLATE<T>::SIZE)
				{
					SIMD_TEMPLATE<T> v = SIMD_TEMPLATE<T>::load(&data[j + i*columns]);
					v += addV;
					v.store(&result.data[i]);
				}
				for(size_t j=simdSize; j<columns; j++)
				{
					result.data[j + i*columns] = data[j + i*columns] + B.data[i];
				}
			}
		}
		else
		{
			//broadcast about the columns
			LARGE_ENOUGH_CLAUSE(rows)
			#pragma omp parallel for
			for(size_t i=0; i<rows; i++)
			{
				size_t simdSize = SIMD_TEMPLATE<T>::getSIMDBound(columns);
				for(size_t j=0; j<simdSize; j+=SIMD_TEMPLATE<T>::SIZE)
				{
					SIMD_TEMPLATE<T> v = SIMD_TEMPLATE<T>::load(&data[j + i*columns]);
					SIMD_TEMPLATE<T> addV = SIMD_TEMPLATE<T>::load(&B.data[j]);
					v += addV;
					v.store(&result.data[i]);
				}
				for(size_t j=simdSize; j<columns; j++)
				{
					result.data[j + i*columns] = data[j + i*columns] + B.data[j];
				}
			}
		}

		RESET_LARGE_ENOUGH_CLAUSE()
		return result;
	}

	template<typename T>
	Matrix<T> Matrix<T>::broadcastSubtract(T v) const
	{
		Matrix<T> result = Matrix<T>(rows, columns);
		size_t size = rows*columns;
		size_t simdBound = 0;
		simdBound = SIMD_TEMPLATE<T>::getSIMDBound(size);
		SIMD_TEMPLATE<T> addV = v;
		
		LARGE_ENOUGH_CLAUSE(simdBound)
		#pragma omp parallel for
		for(size_t i=0; i<simdBound; i+=SIMD_TEMPLATE<T>::SIZE)
		{
			SIMD_TEMPLATE<T> v = SIMD_TEMPLATE<T>::load(&data[i]);
			v -= addV;
			v.store(&result.data[i]);
		}
		
		LARGE_ENOUGH_CLAUSE(size-simdBound)
		#pragma omp parallel for
		for(size_t i=simdBound; i<size; i++)
			result.data[i] = data[i] - v;
		
		RESET_LARGE_ENOUGH_CLAUSE()
		return result;
	}

	template<typename T>
	Matrix<T> Matrix<T>::broadcastInverseSubtract(T v) const
	{
		Matrix<T> result = Matrix<T>(rows, columns);
		size_t size = rows*columns;
		size_t simdBound = 0;
		simdBound = SIMD_TEMPLATE<T>::getSIMDBound(size);
		SIMD_TEMPLATE<T> addV = v;
		
		LARGE_ENOUGH_CLAUSE(simdBound)
		#pragma omp parallel for
		for(size_t i=0; i<simdBound; i+=SIMD_TEMPLATE<T>::SIZE)
		{
			SIMD_TEMPLATE<T> v = SIMD_TEMPLATE<T>::load(&data[i]);
			v -= addV;
			v.store(&result.data[i]);
		}
		
		LARGE_ENOUGH_CLAUSE(size-simdBound)
		#pragma omp parallel for
		for(size_t i=simdBound; i<size; i++)
			result.data[i] = data[i] - v;
		
		RESET_LARGE_ENOUGH_CLAUSE()
		return result;
	}
	
	template<typename T>
	Matrix<T> Matrix<T>::broadcastSubtract(const Matrix<T>& B) const
	{
		if(B.rows != 1 && B.columns != 1)
			throw InvalidMatrixSize();
		Matrix<T> result = Matrix<T>(rows, columns);
		size_t size = rows*columns;
		
		if(B.rows != 1)
		{
			//broadcast about the rows
			LARGE_ENOUGH_CLAUSE(rows)
			#pragma omp parallel for
			for(size_t i=0; i<rows; i++)
			{
				size_t simdSize = SIMD_TEMPLATE<T>::getSIMDBound(columns);
				SIMD_TEMPLATE<T> subV = B.data[i];
				for(size_t j=0; j<simdSize; j+=SIMD_TEMPLATE<T>::SIZE)
				{
					SIMD_TEMPLATE<T> v = SIMD_TEMPLATE<T>::load(&data[j + i*columns]);
					v = subV - v;
					v.store(&result.data[i]);
				}
				for(size_t j=simdSize; j<columns; j++)
				{
					result.data[j + i*columns] = B.data[i] - data[j + i*columns];
				}
			}
		}
		else
		{
			//broadcast about the columns
			LARGE_ENOUGH_CLAUSE(rows)
			#pragma omp parallel for
			for(size_t i=0; i<rows; i++)
			{
				size_t simdSize = SIMD_TEMPLATE<T>::getSIMDBound(columns);
				for(size_t j=0; j<simdSize; j+=SIMD_TEMPLATE<T>::SIZE)
				{
					SIMD_TEMPLATE<T> v = SIMD_TEMPLATE<T>::load(&data[j + i*columns]);
					SIMD_TEMPLATE<T> subV = SIMD_TEMPLATE<T>::load(&B.data[j]);
					v = subV - v;
					v.store(&result.data[i]);
				}
				for(size_t j=simdSize; j<columns; j++)
				{
					result.data[j + i*columns] = B.data[j] - data[j + i*columns];
				}
			}
		}
		RESET_LARGE_ENOUGH_CLAUSE()
		return result;
	}

	
	template<typename T>
	Matrix<T> Matrix<T>::broadcastFunction(std::function<T(T)> func, std::function<SIMD_TEMPLATE<T>(SIMD_TEMPLATE<T>)> simdFunc) const
	{
		if(func == nullptr)
			throw std::bad_function_call();
		
		Matrix<T> result = Matrix<T>(rows, columns);
		size_t size = rows*columns;
		size_t simdBound = 0;
		simdBound = SIMD_TEMPLATE<T>::getSIMDBound(size);
		if(simdFunc == nullptr)
			simdBound = 0; //Don't do any SIMD work
		
		LARGE_ENOUGH_CLAUSE(simdBound)
		#pragma omp parallel for
		for(size_t i=0; i<simdBound; i+=SIMD_TEMPLATE<T>::SIZE)
		{
			SIMD_TEMPLATE<T> v = SIMD_TEMPLATE<T>::load(&data[i]);
			v = simdFunc(v);
			v.store(&result.data[i]);
		}

		LARGE_ENOUGH_CLAUSE(size-simdBound)
		#pragma omp parallel for
		for(size_t i=simdBound; i<size; i++)
			result.data[i] = func(data[i]);

		RESET_LARGE_ENOUGH_CLAUSE()
		return result;
	}
	
	template<typename T>
	Matrix<T> Matrix<T>::broadcastFunction(T (*func)(T), SIMD_TEMPLATE<T> (*simdFunc)(SIMD_TEMPLATE<T>)) const
	{
		if(func == nullptr)
			throw std::bad_function_call();
		
		Matrix<T> result = Matrix<T>(rows, columns);
		size_t size = rows*columns;
		size_t simdBound = 0;
		simdBound = SIMD_TEMPLATE<T>::getSIMDBound(size);
		if(simdFunc == nullptr)
			simdBound = 0; //Don't do any SIMD work
			
		LARGE_ENOUGH_CLAUSE(simdBound)
		#pragma omp parallel for
		for(size_t i=0; i<simdBound; i+=SIMD_TEMPLATE<T>::SIZE)
		{
			SIMD_TEMPLATE<T> v = SIMD_TEMPLATE<T>::load(&data[i]);
			v = simdFunc(v);
			v.store(&result.data[i]);
		}

		LARGE_ENOUGH_CLAUSE(size-simdBound)
		#pragma omp parallel for
		for(size_t i=simdBound; i<size; i++)
			result.data[i] = func(data[i]);

		RESET_LARGE_ENOUGH_CLAUSE()
		return result;
	}

	template<typename T>
	T* Matrix<T>::getData() const
	{
		return data;
	}

	template<typename T>
	unsigned int Matrix<T>::getRows() const
	{
		return rows;
	}

	template<typename T>
	unsigned int Matrix<T>::getColumns() const
	{
		return columns;
	}

	template<typename T>
	bool Matrix<T>::getValid() const
	{
		return valid;
	}

	template<typename T>
	T Matrix<T>::get(unsigned int row, unsigned int col) const
	{
		if (row < rows && row >= 0 && col < columns && col >= 0)
			return data[col + row*columns];
		throw InvalidIndex();
	}

	template<typename T>
	Matrix<T> Matrix<T>::hadamardProduct(const Matrix<T>& other) const
	{
		if(rows != other.rows || columns != other.columns)
			throw InvalidMatrixSize();

		Matrix<T> m = Matrix<T>(rows, columns);
		size_t simdBound = 0;
		simdBound = SIMD_TEMPLATE<T>::getSIMDBound(rows*columns);
		
		LARGE_ENOUGH_CLAUSE(simdBound)
		#pragma omp parallel for
		for (size_t i=0; i < simdBound; i+=SIMD_TEMPLATE<T>::SIZE)
		{
			SIMD_TEMPLATE<T> aValues = SIMD_TEMPLATE<T>::load(&data[i]);
			SIMD_TEMPLATE<T> bValues = SIMD_TEMPLATE<T>::load(&other.data[i]);
			aValues*=bValues;
			aValues.store(&m.data[i]);
		}
		
		LARGE_ENOUGH_CLAUSE((rows*columns)-simdBound)
		#pragma omp parallel for
		for(size_t i=simdBound; i<rows*columns; i++)
		{
			m.data[i] = data[i]*other.data[i];
		}

		return m;
	}
	
	template<typename T>
	Matrix<T> Matrix<T>::broadcastHadamardProduct(const Matrix<T>& B) const
	{
		if(B.rows != 1 && B.columns != 1)
			throw InvalidMatrixSize();
		
		Matrix<T> result = Matrix<T>(rows, columns);
		size_t size = rows*columns;

		if(B.columns != 1)
		{
			size_t simdBound = 0;
			simdBound = SIMD_TEMPLATE<T>::getSIMDBound(columns*rows);
			//make a temporary array that duplicates data for better memory access.
			T* duplicateColumns = new T[B.columns*SIMD_TEMPLATE<T>::SIZE];
			memcpy(duplicateColumns, B.data, B.columns*sizeof(T));
			memcpy(duplicateColumns+B.columns*sizeof(T), B.data, SIMD_TEMPLATE<T>::SIZE*sizeof(T));

			LARGE_ENOUGH_CLAUSE(simdBound)
			#pragma omp parallel for
			for (size_t i=0; i < simdBound; i+=SIMD_TEMPLATE<T>::SIZE)
			{
				SIMD_TEMPLATE<T> aValues = SIMD_TEMPLATE<T>::load(&data[i]);
				SIMD_TEMPLATE<T> bValues = SIMD_TEMPLATE<T>::load(&duplicateColumns[i%B.columns]);
				aValues*=bValues;
				aValues.store(&result.data[i]);
			}

			delete[] duplicateColumns;
			
			LARGE_ENOUGH_CLAUSE(size-simdBound)
			#pragma omp parallel for
			for(size_t i=simdBound; i<size; i++)
			{
				result.data[i] = data[i] * B.data[i%B.columns];
			}
		}
		else
		{
			size_t simdBound = 0;
			simdBound = SIMD_TEMPLATE<T>::getSIMDBound(rows*columns);
			
			//make a temporary array that duplicates data for better memory access.
			T* duplicateRows = new T[B.rows*SIMD_TEMPLATE<T>::SIZE];
			T* startDupRows = duplicateRows;
			for(size_t i=0; i<B.rows; i++)
			{
				SIMD_TEMPLATE<T> val = SIMD_TEMPLATE<T>(B.data[i]);
				val.store(&startDupRows);
				startDupRows += SIMD_TEMPLATE<T>::SIZE;
			}

			LARGE_ENOUGH_CLAUSE(simdBound)
			#pragma omp parallel for
			for (size_t i=0; i < simdBound; i+=SIMD_TEMPLATE<T>::SIZE)
			{
				size_t left = columns - i%columns;
				size_t offset = (left < SIMD_TEMPLATE<T>::SIZE) ? left : 0;

				SIMD_TEMPLATE<T> aValues = SIMD_TEMPLATE<T>::load(&data[i]);
				SIMD_TEMPLATE<T> bValues = SIMD_TEMPLATE<T>::load(&duplicateRows[offset + (i/B.rows)]);
				aValues*=bValues;
				aValues.store(&result.data[i]);
			}

			delete[] duplicateRows;
			
			LARGE_ENOUGH_CLAUSE(size-simdBound)
			#pragma omp parallel for
			for(size_t i=simdBound; i<size; i++)
			{
				result.data[i] = data[i] * B.data[i/B.rows];
			}
		}
		RESET_LARGE_ENOUGH_CLAUSE()
		return result;
	}
	
	template<typename T>
	Matrix<T> Matrix<T>::inverseHadamardProduct(const Matrix<T>& other) const
	{
		if(rows != other.rows || columns != other.columns)
			throw InvalidMatrixSize();

		Matrix<T> m = Matrix<T>(rows, columns);
		size_t simdBound = 0;
		simdBound = SIMD_TEMPLATE<T>::getSIMDBound(rows*columns);
		
		LARGE_ENOUGH_CLAUSE(simdBound)
		#pragma omp parallel for
		for (size_t i=0; i < simdBound; i+=SIMD_TEMPLATE<T>::SIZE)
		{
			SIMD_TEMPLATE<T> aValues = SIMD_TEMPLATE<T>::load(&data[i]);
			SIMD_TEMPLATE<T> bValues = SIMD_TEMPLATE<T>::load(&other.data[i]);
			aValues /= bValues;
			aValues.store(&m.data[i]);
		}
		
		LARGE_ENOUGH_CLAUSE((rows*columns)-simdBound)
		#pragma omp parallel for
		for(size_t i=simdBound; i<rows*columns; i++)
		{
			m.data[i] = data[i]/other.data[i];
		}

		RESET_LARGE_ENOUGH_CLAUSE()
		return m;
	}

	template<typename T>
	Matrix<T> Matrix<T>::broadcastInverseHadamardProduct(const Matrix<T>& B) const
	{
		if(B.rows != 1 && B.columns != 1)
			throw InvalidMatrixSize();
		
		Matrix<T> result = Matrix<T>(rows, columns);
		size_t size = rows*columns;

		if(B.columns != 1)
		{
			size_t simdBound = 0;
			simdBound = SIMD_TEMPLATE<T>::getSIMDBound(columns*rows);
			//make a temporary array that duplicates data for better memory access.
			T* duplicateColumns = new T[B.columns*SIMD_TEMPLATE<T>::SIZE];
			memcpy(duplicateColumns, B.data, B.columns*sizeof(T));
			memcpy(duplicateColumns+B.columns*sizeof(T), B.data, SIMD_TEMPLATE<T>::SIZE*sizeof(T));

			LARGE_ENOUGH_CLAUSE(simdBound)
			#pragma omp parallel for
			for (size_t i=0; i < simdBound; i+=SIMD_TEMPLATE<T>::SIZE)
			{
				SIMD_TEMPLATE<T> aValues = SIMD_TEMPLATE<T>::load(&data[i]);
				SIMD_TEMPLATE<T> bValues = SIMD_TEMPLATE<T>::load(&duplicateColumns[i%B.columns]);
				aValues/=bValues;
				aValues.store(&result.data[i]);
			}

			delete[] duplicateColumns;
			
			LARGE_ENOUGH_CLAUSE(size-simdBound)
			#pragma omp parallel for
			for(size_t i=simdBound; i<size; i++)
			{
				result.data[i] = data[i] / B.data[i%B.columns];
			}
		}
		else
		{
			size_t simdBound = 0;
			simdBound = SIMD_TEMPLATE<T>::getSIMDBound(rows*columns);
			
			//make a temporary array that duplicates data for better memory access.
			T* duplicateRows = new T[B.rows*SIMD_TEMPLATE<T>::SIZE];
			T* startDupRows = duplicateRows;
			for(size_t i=0; i<B.rows; i++)
			{
				SIMD_TEMPLATE<T> val = SIMD_TEMPLATE<T>(B.data[i]);
				val.store(startDupRows);
				startDupRows += SIMD_TEMPLATE<T>::SIZE;
			}

			LARGE_ENOUGH_CLAUSE(simdBound)
			#pragma omp parallel for
			for (size_t i=0; i < simdBound; i+=SIMD_TEMPLATE<T>::SIZE)
			{
				size_t left = columns - i%columns;
				size_t offset = (left < SIMD_TEMPLATE<T>::SIZE) ? left : 0;

				SIMD_TEMPLATE<T> aValues = SIMD_TEMPLATE<T>::load(&data[i]);
				SIMD_TEMPLATE<T> bValues = SIMD_TEMPLATE<T>::load(&duplicateRows[offset + (i/B.rows)]);
				aValues/=bValues;
				aValues.store(&result.data[i]);
			}

			delete[] duplicateRows;
			
			LARGE_ENOUGH_CLAUSE(size-simdBound)
			#pragma omp parallel for
			for(size_t i=simdBound; i<size; i++)
			{
				result.data[i] = data[i] / B.data[i/B.rows];
			}
		}

		RESET_LARGE_ENOUGH_CLAUSE()
		return result;
	}

	template<typename T>
	Matrix<T> Matrix<T>::getInverse() const
	{
		//TODO: SPEED UP. IS SLOW
		T det = getDeterminate();

		if(det!=0 && det!=NAN)
		{
			Matrix<T> inverse = Matrix<T>(rows, columns);
			
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
				// LARGE_ENOUGH_CLAUSE(totalSize)
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
		else
		{
			throw InvalidInverse();
		}
	}

	template<typename T>
	Matrix<T> Matrix<T>::getTranspose() const
	{
		Matrix<T> m = Matrix<T>(columns, rows);
		unsigned int totalSize = columns*rows;

		LARGE_ENOUGH_CLAUSE(totalSize)
		#pragma omp parallel for
		for(size_t i=0; i<totalSize; i++)
		{
			unsigned int y = i / columns;
			unsigned int x = i % columns;
			m.data[i] = data[y + x*columns];
		}

		return m;
	}

	template<typename T>
	T Matrix<T>::getDeterminate() const
	{
		if(rows == columns && rows > 1)
		{
			if(columns == 2)
			{
				return (data[0]*data[3]) - (data[1]*data[2]);
			}
			else
			{
				T sumValue = T();
				#pragma omp parallel for
				for(size_t i=0; i<columns; i++)
				{
					T addV = data[i] * getMatrixOfMinors(0, i).getDeterminate();
					#pragma omp critical
					{
						if(i%2 == 0)
							sumValue += addV;
						else
							sumValue -= addV;
					}
				}
				return sumValue;
			}
		}
		else
		{
			throw InvalidMatrixSize();
		}

		return T();
	}

	template<typename T>
	Matrix<T> Matrix<T>::getMatrixOfMinors(unsigned int row, unsigned int col) const
	{
		Matrix<T> m = Matrix<T>(rows-1, columns-1);
		
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

	template<typename T>
	void Matrix<T>::clear()
	{
		T* startData=data;
		T* endData = data+rows*columns;
		while(startData < endData)
		{
			*startData = T();
			startData++;
		}
	}
	
	template<typename T>
	Matrix<T> Matrix<T>::multiply(const Matrix<T>& A, const Matrix<T>& B)
	{
		return A.multiply(B);
	}
	
	template<typename T>
	Matrix<T> Matrix<T>::multiplyTranspose(const Matrix<T>& A, const Matrix<T>& B)
	{
		return A.multiplyTranspose(B);
	}
	
	template<typename T>
	Matrix<T> Matrix<T>::multiply(const Matrix<T>& B) const
	{
		//for large size matricies, do transpose then do matrix multiplication.
		if(!valid || !B.valid || columns != B.rows)
			throw InvalidMatrixSize();
		
		if(rows * B.columns > EXCEPTIONALY_LARGE_MATRIX_AREA)
		{
			Matrix<T> BT = B.getTranspose(); //O(N^2)
			return Matrix<T>::multiplyTranspose(BT); //O(N^3) but better cache locality
		}

		Matrix<T> C = Matrix<T>(rows, B.columns);
		size_t columnsAdjusted, inc;
		if(SIMD_TEMPLATE<T>::SIZE == 1)
		{
			columnsAdjusted = (C.columns>>2)<<2;
			inc = 4;
		}
		else
		{
			columnsAdjusted = SIMD_TEMPLATE<T>::getSIMDBound(C.columns);
			inc = SIMD_TEMPLATE<T>::SIZE;
		}
		
		LARGE_ENOUGH_CLAUSE(C.rows)
		#pragma omp parallel for
		for(size_t i=0; i<C.rows; i++)
		{
			LARGE_ENOUGH_CLAUSE(columnsAdjusted)
			if(SIMD_TEMPLATE<T>::SIZE == 1)
			{
				#pragma omp parallel for
				for(size_t j=0; j<columnsAdjusted; j+=inc) //compute 4 at once
				{
					T sums[4] = {T(), T(), T(), T()}; //defaults to 0 for all primitive types or the equivalent 0 in other types.
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
				}
			}
			else
			{
				#pragma omp parallel for
				for(size_t j=0; j<columnsAdjusted; j+=inc) //compute 4 at once
				{
					SIMD_TEMPLATE<T> sums = T();
					for(size_t k=0; k<columns; k++)
					{
						SIMD_TEMPLATE<T> aValue = data[k + i*columns];
						SIMD_TEMPLATE<T> bValue;
						bValue.load(&B.data[j + k*B.columns]);
						sums += aValue * bValue;
					}
					sums.store(&C[i][j]);
				}
			}
			
			LARGE_ENOUGH_CLAUSE(columnsAdjusted - C.columns)
			#pragma omp parallel for
			for(size_t j=columnsAdjusted; j<C.columns; j++)
			{
				T sum = T();
				for(size_t k=0; k<columns; k++)
				{
					sum += data[k + i*columns] * B.data[j + k*B.columns];
				}
				C[i][j] = sum;
			}
		}

		RESET_LARGE_ENOUGH_CLAUSE()
		return C;
	}

	template<typename T>
	Matrix<T> Matrix<T>::multiplyTranspose(const Matrix<T>& B) const
	{
		if(!valid || !B.valid || columns != B.columns)
			throw InvalidMatrixSize();
		
		Matrix<T> C = Matrix<T>(rows, B.rows);
		unsigned int totalSize = rows*B.rows;

		LARGE_ENOUGH_CLAUSE(totalSize)
		#pragma omp parallel for
		for(size_t i=0; i<totalSize; i++)
		{
			unsigned int y = i / B.rows;
			unsigned int x = i % B.rows;
			T finalSum = T();
			SIMD_TEMPLATE<T> simdSum = T();
			unsigned int k = 0;
			unsigned int simdBound = SIMD_TEMPLATE<T>::getSIMDBound(columns);
			while(k < simdBound)
			{
				SIMD_TEMPLATE<T> aValues = SIMD_TEMPLATE<T>::load(&data[k + y*columns]);
				SIMD_TEMPLATE<T> bValues = SIMD_TEMPLATE<T>::load(&B.data[k + x*B.columns]);
				simdSum += aValues*bValues;
				k+=SIMD_TEMPLATE<T>::SIZE;
			}
			while(k < columns)
			{
				finalSum += data[k + y*columns] * B.data[k + x*B.columns];
				k++;
			}

			finalSum += simdSum.sum();
			C.data[x + y*B.rows] = finalSum;
		}

		RESET_LARGE_ENOUGH_CLAUSE()
		return C;
	}
	
	template<typename T>
	Matrix<T> Matrix<T>::fusedMultiplyAdd(const Matrix<T>& A, const Matrix<T>& B, const Matrix<T>& C)
	{
		return A.fusedMultiplyAdd(B, C);
	}
	
	template<typename T>
	Matrix<T> Matrix<T>::fusedMultiplyAddTranspose(const Matrix<T>& A, const Matrix<T>& B, const Matrix<T>& C)
	{
		return A.fusedMultiplyAddTranspose(B, C);
	}

	template<typename T>
	Matrix<T> Matrix<T>::fusedMultiplyAdd(const Matrix<T>& B, const Matrix<T>& C) const
	{
		//for large size matricies, do transpose then do matrix multiplication.
		if(!valid || !B.valid || columns != B.rows)
			throw InvalidMatrixSize();
		
		if(C.rows != rows || C.columns != B.columns)
			throw InvalidMatrixSize();
			
		if(rows * B.columns > EXCEPTIONALY_LARGE_MATRIX_AREA)
		{
			Matrix<T> BT = B.getTranspose(); //O(N^2)
			return Matrix<T>::fusedMultiplyAddTranspose(BT, C); //O(N^3) but better cache locality
		}

		Matrix<T> result = Matrix<T>(rows, B.columns);
		size_t columnsAdjusted, inc;
		if(SIMD_TEMPLATE<T>::SIZE == 1)
		{
			columnsAdjusted = (result.columns>>2)<<2;
			inc = 4;
		}
		else
		{
			columnsAdjusted = SIMD_TEMPLATE<T>::getSIMDBound(result.columns);
			inc = SIMD_TEMPLATE<T>::SIZE;
		}

		LARGE_ENOUGH_CLAUSE(result.rows)
		#pragma omp parallel for
		for(size_t i=0; i<result.rows; i++)
		{
			LARGE_ENOUGH_CLAUSE(columnsAdjusted)
			if(SIMD_TEMPLATE<T>::SIZE == 1)
			{
				#pragma omp parallel for
				for(size_t j=0; j<columnsAdjusted; j+=inc) //compute 4 at once
				{
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
				}
			}
			else
			{
				#pragma omp parallel for
				for(size_t j=0; j<columnsAdjusted; j+=inc) //compute 4 at once
				{
					SIMD_TEMPLATE<T> sums = T();
					sums.load(&C.data[i + j*result.columns]);
					
					for(size_t k=0; k<columns; k++)
					{
						SIMD_TEMPLATE<T> aValue = data[k + i*columns];
						SIMD_TEMPLATE<T> bValue;
						bValue.load(&B.data[j + k*B.columns]);
						sums += aValue * bValue;
					}
					sums.store(&result[i][j]);
				}
			}
			
			LARGE_ENOUGH_CLAUSE(result.columns - columnsAdjusted)
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

		RESET_LARGE_ENOUGH_CLAUSE()
		return result;
	}
	
	template<typename T>
	Matrix<T> Matrix<T>::fusedMultiplyAddTranspose(const Matrix<T>& B, const Matrix<T>& C) const
	{
		if(!valid || !B.valid || columns != B.columns)
			throw InvalidMatrixSize();
		
		if(C.rows != rows || C.columns != B.rows)
			throw InvalidMatrixSize();
		
		Matrix<T> result = Matrix<T>(rows, B.rows);
		unsigned int totalSize = rows*B.rows;

		LARGE_ENOUGH_CLAUSE(totalSize)
		#pragma omp parallel for
		for(size_t i=0; i<totalSize; i++)
		{
			unsigned int y = i / B.rows;
			unsigned int x = i % B.rows;
			T finalSum = C[y][x];
			SIMD_TEMPLATE<T> simdSum = T();
			unsigned int k = 0;
			unsigned int simdBound = SIMD_TEMPLATE<T>::getSIMDBound(columns);
			while(k < simdBound)
			{
				SIMD_TEMPLATE<T> aValues = SIMD_TEMPLATE<T>::load(&data[k + y*columns]);
				SIMD_TEMPLATE<T> bValues = SIMD_TEMPLATE<T>::load(&B.data[k + x*B.columns]);
				simdSum += aValues*bValues;
				k+=SIMD_TEMPLATE<T>::SIZE;
			}
			while(k < columns)
			{
				finalSum += data[k + y*columns] * B.data[k + x*B.columns];
				k++;
			}

			finalSum += simdSum.sum();
			result.data[x + y*B.rows] = finalSum;
		}

		RESET_LARGE_ENOUGH_CLAUSE()
		return result;
	}

	template<typename T>
	T Matrix<T>::sum() const
	{
		T sum = T();
		for(size_t i=0; i<rows*columns; i++)
		{
			sum += data[i];
		}
		return sum;
	}

	template<typename T>
	Matrix<T> Matrix<T>::horizontalSum() const
	{
		Matrix<T> m = Matrix<T>(rows, 1);
		
		LARGE_ENOUGH_CLAUSE(rows)
		#pragma omp parallel for
		for(size_t i=0; i<rows; i++)
		{
			T sum = T();
			for(size_t j=0; j<columns; j++)
			{
				sum += data[j + i*rows];
			}
			m[i][0] = sum;
		}
		RESET_LARGE_ENOUGH_CLAUSE()
		return m;
	}

	template<typename T>
	Matrix<T> Matrix<T>::verticalSum() const
	{
		Matrix<T> m = Matrix<T>(1, columns);
		for(size_t i=0; i<rows; i++)
		{
			for(size_t j=0; j<columns; j++)
			{
				m[0][j] += data[j + i*rows];
			}
		}
		return m;
	}

	template<typename T>
	void Matrix<T>::normalize()
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
				operator/=(sValue);
			}
		}
	}
	
	template<typename T>
	T Matrix<T>::min() const
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
		throw InvalidMatrixSize();
		return T();
	}
	
	template<typename T>
	T Matrix<T>::max() const
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
		throw InvalidMatrixSize();
		return T();
	}

	template<typename T>
	Matrix<T> Matrix<T>::minAcrossRows() const
	{
		if(rows*columns > 0)
		{
			Matrix<T> output = Matrix<T>(rows, 1);
			for(size_t r=0; r<rows; r++)
			{
				T minValue = this->operator[](r)[0];
				for(size_t c=1; c<columns; c++)
				{
					minValue = (minValue < this->operator[](r)[c]) ? minValue : this->operator[](r)[c];
				}
				output[r][0] = minValue;
			}
			return output;
		}
		throw InvalidMatrixSize();
		return T();
	}
	
	template<typename T>
	Matrix<T> Matrix<T>::minAcrossCols() const
	{
		if(rows*columns > 0)
		{
			Matrix<T> output = Matrix<T>(1, columns);
			for(size_t c=1; c<columns; c++)
			{
				output[0][c] = this->operator[](0)[c];
			}
			for(size_t r=1; r<rows; r++)
			{
				for(size_t c=0; c<columns; c++)
				{
					output[0][c] = (output[0][c] < this->operator[](r)[c]) ? output[0][c] : this->operator[](r)[c];
				}
			}
			return output;
		}
		throw InvalidMatrixSize();
		return T();
	}
	
	
	template<typename T>
	Matrix<T> Matrix<T>::maxAcrossRows() const
	{
		if(rows*columns > 0)
		{
			Matrix<T> output = Matrix<T>(rows, 1);
			for(size_t r=0; r<rows; r++)
			{
				T maxValue = this->operator[](r)[0];
				for(size_t c=1; c<columns; c++)
				{
					maxValue = (maxValue > this->operator[](r)[c]) ? maxValue : this->operator[](r)[c];
				}
				output[r][0] = maxValue;
			}
			return output;
		}
		throw InvalidMatrixSize();
		return T();
	}
	
	template<typename T>
	Matrix<T> Matrix<T>::maxAcrossCols() const
	{
		if(rows*columns > 0)
		{
			Matrix<T> output = Matrix<T>(1, columns);
			for(size_t c=1; c<columns; c++)
			{
				output[0][c] = this->operator[](0)[c];
			}
			for(size_t r=1; r<rows; r++)
			{
				for(size_t c=0; c<columns; c++)
				{
					output[0][c] = (output[0][c] > this->operator[](r)[c]) ? output[0][c] : this->operator[](r)[c];
				}
			}
			return output;
		}
		throw InvalidMatrixSize();
		return T();
	}

	template<typename T>
	std::pair<T, T> Matrix<T>::minMaxValues() const
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
		throw InvalidMatrixSize();
		return {T(), T()};
	}

	template<typename T>
	T Matrix<T>::mean() const
	{
		return sum() / (rows*columns);
	}
	template<typename T>
	Matrix<T> Matrix<T>::meanAcrossCols() const
	{
		return horizontalSum() / rows*columns;
	}
	template<typename T>
	Matrix<T> Matrix<T>::meanAcrossRows() const
	{
		return verticalSum() / rows*columns;
	}
	

	template<typename T>
	void Matrix<T>::reshape(unsigned int newRows, unsigned int newColumns)
	{
		if(newRows*newColumns == rows*columns)
		{
			rows = newRows;
			columns = newColumns;
		}
		throw InvalidMatrixSize();
	}
	

	template<typename T>
	Matrix<T> Matrix<T>::extract(unsigned int startRow, unsigned int startColumn, unsigned int endRow, unsigned int endColumn) const
	{
		Matrix<T> newMatrix = Matrix<T>(endRow-startRow+1, endColumn-startColumn+1);
		T* newMatrixDataPointer = newMatrix.data;
		for(size_t i=0; i<newMatrix.rows; i++)
		{
			T* currMatrixDataPointer = data + startColumn + (i+startRow)*columns;
			for(size_t j=0; j<newMatrix.columns; j++)
			{
				*newMatrixDataPointer = *currMatrixDataPointer;
				newMatrixDataPointer++;
				currMatrixDataPointer++;
			}
		}
		return newMatrix;
	}

	template<typename T>
	Matrix<T> Matrix<T>::pad(unsigned int rowPadding, unsigned int columnPadding, T paddingValue) const
	{
		Matrix<T> newMatrix = Matrix<T>(rows + rowPadding, columns + columnPadding);
		T* newMatrixDataPointer = newMatrix.data;
		T* currMatrixDataPointer = data;

		for(size_t i=0; i<newMatrix.rows; i++)
		{
			if(i < rows)
			{
				size_t j=0;
				for(; j<columns; j++)
				{
					*newMatrixDataPointer = *currMatrixDataPointer;
					newMatrixDataPointer++;
					currMatrixDataPointer++;
				}
				for(; j<newMatrix.columns; j++)
				{
					*newMatrixDataPointer = paddingValue;
					newMatrixDataPointer++;
				}
			}
			else
			{
				for(size_t j=0; j<newMatrix.columns; j++)
				{
					*newMatrixDataPointer = paddingValue;
					newMatrixDataPointer++;
				}
			}
		}
		return newMatrix;
	}
	
} //NAMESPACE smpl END