#pragma once
#include "Matrix.h"

namespace smpl
{
    template<typename T>
    class TensorMatrix;
    template<typename T>
    class Tensor;
    
    template<typename T>
    class TensorMatrix : public Matrix<T>
    {
    public:
        virtual ~TensorMatrix()
        {
            this->valid = false;
            this->rows=0;
            this->columns=0;
            this->data = nullptr;
        }
        TensorMatrix(const Matrix<T>& other)
        {
            this->rows = other.getRows();
            this->columns = other.getColumns();
            copy(other);
        }
        void operator=(const Matrix<T>& other)
        {
            copy(other);
        }
    private:
        friend Tensor<T>;
        void copy(const Matrix<T>& other)
        {
            this->valid = false;
            if(other.getValid() && this->rows==other.getRows() && this->columns==other.getColumns())
            {
                memcpy(this->data, other.getData(), sizeof(T)*this->rows*this->columns);
                this->valid = true;
            }
            else
            {
                throw InvalidMatrixCreation();
            }
        }
        
        /**
         * @brief Constructs a special Matrix for Tensors. It does not maintain its data and just refers back
         *      to the original tensor. It therefore does not delete any data as well but has all of the functionality
         *      of a standard matrix.
         * 
         * @param rows 
         * @param columns 
         * @param tempPointer 
         */
        TensorMatrix(unsigned int rows, unsigned int columns, T* tempPointer) : Matrix<T>()
        {
            this->valid = true;
            this->rows = rows;
            this->columns = columns;
            this->data = tempPointer;
            if(rows == 0 || columns == 0 || tempPointer == nullptr)
                this->valid = false;
        }

    };

    template<typename T>
    class Tensor
    {
    public:
        Tensor(unsigned int depth, unsigned int rows, unsigned int cols);
        Tensor(const std::vector<std::vector<std::vector<T>>>& arr);
        Tensor(const Tensor<T>& other);
        Tensor(Tensor<T>&& other) noexcept;
        void operator=(const Tensor<T>& other);
        void operator=(Tensor<T>&& other) noexcept;
        ~Tensor();

        TensorMatrix<T> operator[](size_t index) const;

        Tensor<T> operator*(const Matrix<T>& other) const;
        Tensor<T> operator*(const Tensor<T>& other) const;
        
        Tensor<T> operator+(const Matrix<T>& other) const;
        Tensor<T> operator+(const Tensor<T>& other) const;
        
        Tensor<T> operator-(const Matrix<T>& other) const;
        Tensor<T> operator-(const Tensor<T>& other) const;
        
        Tensor<T> operator*(const T& other) const;
        Tensor<T> operator/(const T& other) const;
        
        void operator+=(const Matrix<T>& other);
        void operator+=(const Tensor<T>& other);
        
        void operator-=(const Matrix<T>& other);
        void operator-=(const Tensor<T>& other);
        
        void operator*=(const T& other);
        void operator/=(const T& other);

        // Tensor<T> hadamardProduct(const Matrix<T>& other) const;
        // Tensor<T> hadamardProduct(const Tensor<T>& other) const;

        
        /**
         * @brief Get the Transpose of the entire Tensor.
         *      Goes from DxRxC -> CxRxD swaping the values accordingly
         * 
         * @return Tensor<T> 
         */
        Tensor<T> getTranspose() const;

        /**
         * @brief Get the Transpose of each matrix in the tensor separately.
         *      Goes from DxRxC -> DxCxR swaping the values accordingly.
         *          This may be more desired.
         * 
         * @return Tensor<T> 
         */
        Tensor<T> getMatrixTranspose() const;
        
        void reshape(int newDepth, int newRows, int newColumns);

        unsigned int getRows() const;
        unsigned int getColumns() const;
        unsigned int getDepth() const;

        bool getValid() const;
        
    private:
        void copy(const Tensor<T>& other);
        // void move(Tensor<T>&& other) noexcept;
        void dispose();
        unsigned int depth=0;
        unsigned int rows=0;
        unsigned int columns=0;
        bool valid = false;
        T* data;
    };

    template<typename T>
    void formatToString(StringStream& stream, const TensorMatrix<T>& mat, const std::string& format)
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
					formatToString(stream, mat[r][c], format);
					stream.write(" ");
				}
				stream.pop();
				stream.write("]\n");
			}
			stream.pop();
			stream.write(']');
		}
    }

    template<typename T>
    void formatToString(StringStream& stream, const Tensor<T>& tensor, const std::string& format)
    {
        if(!tensor.getValid())
        {
            stream.write("[INVALID]");
        }
        else
        {
            stream.write('[');
            for(size_t d=0; d<tensor.getDepth(); d++)
			{
				formatToString(stream, tensor[d], format);
			    stream.write('\n');
			}
			stream.pop();
            stream.write(']');
        }
    }

    template<typename T>
    Tensor<T>::Tensor(unsigned int depth, unsigned int rows, unsigned int cols)
    {
        valid = false;
        this->depth = depth;
        this->rows = rows;
        this->columns = cols;
        if(depth > 0 && rows > 0 && cols > 0)
        {
            valid = true;
            data = new T[depth*rows*cols];
            memset(data, 0, depth*rows*cols*sizeof(T));
        }
        else
        {
            throw InvalidMatrixCreation();
        }
    }

    template<typename T>
    void Tensor<T>::dispose()
    {
        if(data != nullptr)
            delete[] data;
        
        columns = 0;
        depth = 0;
        rows = 0;
        valid = false;
    }

    template<typename T>
    void Tensor<T>::copy(const Tensor<T>& other)
    {
        dispose();
        this->depth = other.depth;
        this->rows = other.rows;
        this->columns = other.columns;
        this->valid = other.valid;

        if(depth > 0 && rows > 0 && columns > 0)
        {
            data = new T[depth*rows*columns];
            memcpy(data, other.data, depth*rows*columns*sizeof(T));
        }    
        else
        {
            throw InvalidMatrixCreation();
        }
    }

    // template<typename T>
    // void Tensor<T>::move(Tensor<T>&& other) noexcept
    // {
    //     this->depth = other.depth;
    //     this->rows = other.rows;
    //     this->columns = other.columns;
    //     this->valid = other.valid;
    //     this->data = other.data;
    //     other.data = nullptr;
    // }


    template<typename T>
    Tensor<T>::Tensor(const std::vector<std::vector<std::vector<T>>>& arr)
    {
        this->depth = arr.size();
        this->rows = arr.front().size();
        this->columns = arr.front().front().size();
        this->valid = true;
        data = new T[depth*rows*columns];
        T* tempData = data;

        for(size_t d=0; d<arr.size(); d++)
        {
            for(size_t r=0; r<arr[d].size(); r++)
            {
                for(size_t c=0; c<arr[d][r].size(); c++)
                {
                    *tempData = arr[d][r][c];
                    tempData++;
                }
            }
        }
    }

    template<typename T>
    Tensor<T>::Tensor(const Tensor<T>& other)
    {
        copy(other);
    }

    template<typename T>
    Tensor<T>::Tensor(Tensor<T>&& other) noexcept
    {
        this->depth = other.depth;
        this->rows = other.rows;
        this->columns = other.columns;
        this->valid = other.valid;
        this->data = other.data;
        other.data = nullptr;
    }

    template<typename T>
    void Tensor<T>::operator=(const Tensor<T>& other)
    {
        copy(other);
    }

    template<typename T>
    void Tensor<T>::operator=(Tensor<T>&& other) noexcept
    {
        this->depth = other.depth;
        this->rows = other.rows;
        this->columns = other.columns;
        this->valid = other.valid;
        this->data = other.data;
        other.data = nullptr;
    }

    template<typename T>
    Tensor<T>::~Tensor()
    {
        if(data != nullptr)
            delete[] data;
        data = nullptr;
        depth = 0;
        rows = 0;
        columns = 0;
        valid = false;
    }


    template<typename T>
    TensorMatrix<T> Tensor<T>::operator[](size_t index) const
    {
        return TensorMatrix<T>(rows, columns, &data[index * (rows*columns)]);
    }

    template<typename T>
    Tensor<T> Tensor<T>::operator*(const Matrix<T>& other) const
    {
        Tensor<T> outputTensor = Tensor<T>(depth, rows, other.getColumns());
        for(int d=0; d<depth; d++)
        {
            outputTensor[d] = operator[](d) * other;
        }
        return outputTensor;
    }
    template<typename T>
    Tensor<T> Tensor<T>::operator*(const Tensor<T>& other) const
    {
        if(depth != other.depth)
            throw InvalidMatrixSize();
        
        Tensor<T> outputTensor = Tensor<T>(depth, rows, other.getColumns());
        for(int d=0; d<depth; d++)
        {
            outputTensor[d] = operator[](d) * other[d];
        }
        return outputTensor;
    }
    
    template<typename T>
    Tensor<T> Tensor<T>::operator+(const Matrix<T>& other) const
    {
        if(rows != other.rows || columns != other.columns)
            throw InvalidMatrixSize();
        
        Tensor<T> outputTensor = Tensor<T>(depth, rows, columns);
        for(int d=0; d<depth; d++)
        {
            outputTensor[d] = operator[](d) + other;
        }
        return outputTensor;
    }
    template<typename T>
    Tensor<T> Tensor<T>::operator+(const Tensor<T>& other) const
    {
        if(depth != other.depth || rows != other.rows || columns != other.columns)
            throw InvalidMatrixSize();
        
        Tensor<T> outputTensor = Tensor<T>(depth, rows, columns);
        for(int d=0; d<depth; d++)
        {
            outputTensor[d] = operator[](d) + other[d];
        }
        return outputTensor;
    }
    
    template<typename T>
    Tensor<T> Tensor<T>::operator-(const Matrix<T>& other) const
    {
        if(rows != other.rows || columns != other.columns)
            throw InvalidMatrixSize();
        
        Tensor<T> outputTensor = Tensor<T>(depth, rows, columns);
        for(int d=0; d<depth; d++)
        {
            outputTensor[d] = operator[](d) - other;
        }
        return outputTensor;
    }
    template<typename T>
    Tensor<T> Tensor<T>::operator-(const Tensor<T>& other) const
    {
        if(depth != other.depth || rows != other.rows || columns != other.columns)
            throw InvalidMatrixSize();
        
        Tensor<T> outputTensor = Tensor<T>(depth, rows, columns);
        for(int d=0; d<depth; d++)
        {
            outputTensor[d] = operator[](d) - other[d];
        }
        return outputTensor;
    }
    
    template<typename T>
    Tensor<T> Tensor<T>::operator*(const T& other) const
    {
        Tensor<T> outputTensor = Tensor<T>(depth, rows, columns);
        for(int d=0; d<depth; d++)
        {
            outputTensor[d] = operator[](d) * other;
        }
        return outputTensor;
    }
    template<typename T>
    Tensor<T> Tensor<T>::operator/(const T& other) const
    {
        Tensor<T> outputTensor = Tensor<T>(depth, rows, columns);
        for(int d=0; d<depth; d++)
        {
            outputTensor[d] = operator[](d) / other;
        }
        return outputTensor;
    }

    template<typename T>
    void Tensor<T>::operator+=(const Matrix<T>& other)
    {
        if(rows != other.rows || columns != other.columns)
            throw InvalidMatrixSize();
        
        for(int d=0; d<depth; d++)
        {
            operator[](d) += other;
        }
    }
    template<typename T>
    void Tensor<T>::operator+=(const Tensor<T>& other)
    {
        if(depth != other.depth || rows != other.rows || columns != other.columns)
            throw InvalidMatrixSize();
        
        for(int d=0; d<depth; d++)
        {
            operator[](d) += other[d];
        }
    }
    
    template<typename T>
    void Tensor<T>::operator-=(const Matrix<T>& other)
    {
        if(rows != other.rows || columns != other.columns)
            throw InvalidMatrixSize();
        
        for(int d=0; d<depth; d++)
        {
            operator[](d) -= other;
        }
    }
    template<typename T>
    void Tensor<T>::operator-=(const Tensor<T>& other)
    {
        if(depth != other.depth || rows != other.rows || columns != other.columns)
            throw InvalidMatrixSize();
        
        for(int d=0; d<depth; d++)
        {
            operator[](d) -= other[d];
        }
    }
    
    template<typename T>
    void Tensor<T>::operator*=(const T& other)
    {
        for(int d=0; d<depth; d++)
        {
            operator[](d) *= other;
        }
    }
    template<typename T>
    void Tensor<T>::operator/=(const T& other)
    {
        for(int d=0; d<depth; d++)
        {
            operator[](d) /= other;
        }
    }

    template<typename T>
    Tensor<T> Tensor<T>::getTranspose() const
    {
        Tensor<T> output = Tensor<T>(columns, rows, depth);
        for(size_t d=0; d<depth; d++)
        {
            for(size_t r=0; r<rows; r++)
            {
                for(size_t c=0; c<columns; c++)
                {
                    output.data[c*depth*rows + r*depth + d] = data[d*columns*rows + r*columns + c];
                }
            }
        }
        return output;
    }

    template<typename T>
    Tensor<T> Tensor<T>::getMatrixTranspose() const
    {
        Tensor<T> output = Tensor<T>(depth, columns, rows);
        for(size_t d=0; d<depth; d++)
        {
            for(size_t r=0; r<rows; r++)
            {
                for(size_t c=0; c<columns; c++)
                {
                    output.data[d*columns*rows + c*rows + r] = data[d*columns*rows + r*columns + c];
                }
            }
        }
        return output;
    }
    
    template<typename T>
    void Tensor<T>::reshape(int newDepth, int newRows, int newColumns)
    {
        if(newDepth*newRows*newColumns != rows*columns*depth)
		    throw InvalidMatrixSize();
        
        depth = newDepth;
        rows = newRows;
        columns = newColumns;
    }

    
    template<typename T>
    unsigned int Tensor<T>::getRows() const
    {
        return rows;
    }
    template<typename T>
    unsigned int Tensor<T>::getColumns() const
    {
        return columns;
    }
    template<typename T>
    unsigned int Tensor<T>::getDepth() const
    {
        return depth;
    }
    template<typename T>
    bool Tensor<T>::getValid() const
    {
        return valid;
    }
}