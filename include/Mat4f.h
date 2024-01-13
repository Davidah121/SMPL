#pragma once
#include "Matrix.h"
#include "Vec4f.h"

namespace glib
{

	class Mat4f : public Matrix
	{
	public:
		/**
		 * @brief Construct a new Mat4f object
		 * 		The class is a subclass of the Matrix class.
		 * 		It is an implementation of a 4x4 Matrix as they are commonly used.
		 */
		Mat4f();

		/**
		 * @brief Construct a new Mat4f object
		 * 		The class is a subclass of the Matrix class.
		 * 		It is an implementation of a 4x4 Matrix as they are commonly used.
		 * 
		 * @param v1 
		 * @param v2 
		 * @param v3 
		 * @param v4 
		 * @param v5 
		 * @param v6 
		 * @param v7 
		 * @param v8 
		 * @param v9 
		 * @param v10 
		 * @param v11 
		 * @param v12 
		 * @param v13 
		 * @param v14 
		 * @param v15 
		 * @param v16 
		 */
		Mat4f(double v1, double v2, double v3, double v4, 
			double v5, double v6, double v7, double v8,
			double v9, double v10, double v11, double v12, 
			double v13, double v14, double v15, double v16);

		/**
		 * @brief Construct a new Mat4f object from another.
		 * 
		 * @param o
		 * 		The Mat4f to copy.
		 */
		Mat4f(const Mat4f& o);

		/**
		 * @brief Copies a Mat4f object
		 * 
		 * @param o
		 * 		The Mat4f to copy.
		 */
		void operator=(const Mat4f& o);

		/**
		 * @brief Destroys the Mat 4f object
		 * 
		 */
		~Mat4f();

		//Object and RootClass Stuff
		static const RootClass globalClass;

		/**
		 * @brief Returns the Identity 4x4 matrix.
		 * 
		 * @return Mat4f 
		 */
		static Mat4f getIdentity();

		/**
		 * @brief Returns the Transpose of the matrix as a Mat4f.
		 * 
		 * @return Mat4f 
		 */
		Mat4f getTranspose();

		double* operator[](int row);

		Mat4f operator+(Mat4f other);
		Mat4f operator-(Mat4f other);
		Mat4f operator*(Mat4f other);

		Mat4f operator*(double other);
		friend Mat4f operator*(double value, Mat4f other)
		{
			return other*value;
		}

		void operator*=(double other);

		void operator+=(Mat4f other);
		void operator-=(Mat4f other);

		Vec4f operator*(Vec4f other);

		bool operator==(Mat4f other);
		bool operator!=(Mat4f other);

		//Converting into an array
		/**
		 * @brief Fills a float array of at least size 16 with the matrix values
		 * 		
		 * @param buffer 
		 * 		Must be of size 16 or greater.
		 */
		void fillArray(float* buffer);

		/**
		 * @brief Fills a float array of at least size 16 with the matrix values
		 * 		
		 * @param buffer 
		 * 		Must be of size 16 or greater.
		 */
		void fillArray(double* buffer);
	};

} //NAMESPACE glib END