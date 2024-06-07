#pragma once
#include "Matrix.h"
#include "Vec2f.h"

namespace smpl
{

	///A special Matrix that is a 2x2 matrix. Commonly used
	class Mat2f : public Matrix
	{
	public:
		/**
		 * @brief Construct a new Mat2f object
		 * 		The class is a subclass of the Matrix class.
		 * 		It is an implementation of a 2x2 Matrix as they are commonly used.
		 */
		Mat2f();

		/**
		 * @brief Construct a new Mat2f object
		 * 		The class is a subclass of the Matrix class.
		 * 		It is an implementation of a 2x2 Matrix as they are commonly used.
		 * 
		 * @param v1 
		 * @param v2 
		 * @param v3 
		 * @param v4 
		 */
		Mat2f(double v1, double v2, double v3, double v4);

		/**
		 * @brief Construct a new Mat2f object from another.
		 * 
		 * @param o 
		 * 		The other Mat2f to copy.
		 */
		Mat2f(const Mat2f& o);

		/**
		 * @brief Copies a Mat2f object.
		 * 
		 * @param o 
		 * 		The other Mat2f to copy.
		 */
		void operator=(const Mat2f& o);

		/**
		 * @brief Destroys the Mat2f object
		 * 
		 */
		~Mat2f();
		
		//Object and RootClass Stuff
		static const RootClass globalClass;
		virtual const RootClass* getClass();

		/**
		 * @brief Returns the Identity 2x2 matrix
		 * 
		 * @return Mat2f 
		 */
		static Mat2f getIdentity();

		double* operator[](int row);

		Mat2f operator*(double value);

		friend Mat2f operator*(double value, Mat2f other)
		{
			return other * value;
		}

		void operator*=(double value);

		Mat2f operator+(Mat2f other);
		void operator+=(Mat2f other);

		Mat2f operator-(Mat2f other);
		void operator-=(Mat2f other);

		Mat2f operator*(Mat2f other);

		Vec2f operator*(Vec2f other);

		bool operator==(Mat2f other);
		bool operator!=(Mat2f other);

		//Converting into an array
		/**
		 * @brief Fills a float array of at least size 4 with the matrix values
		 * 		
		 * @param buffer 
		 * 		Must be of size 4 or greater.
		 */
		void fillArray(float* buffer);

		/**
		 * @brief Fills a float array of at least size 4 with the matrix values
		 * 		
		 * @param buffer 
		 * 		Must be of size 4 or greater.
		 */
		void fillArray(double* buffer);
	};

} //NAMESPACE smpl END