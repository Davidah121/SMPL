#pragma once
#include "Matrix.h"
#include "Vec3f.h"

namespace glib
{

	class Mat3f : public Matrix
	{
	public:
		/**
		 * @brief Construct a new Mat3f object
		 * 		The class is a subclass of the Matrix class.
		 * 		It is an implementation of a 3x3 Matrix as they are commonly used.
		 */
		Mat3f();

		/**
		 * @brief Construct a new Mat3f object
		 * 		The class is a subclass of the Matrix class.
		 * 		It is an implementation of a 3x3 Matrix as they are commonly used.
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
		 */
		Mat3f(double v1, double v2, double v3, double v4, double v5, double v6, double v7, double v8, double v9);

		/**
		 * @brief Destroys the Mat3f object
		 * 
		 */
		~Mat3f();

		/**
		 * @brief Construct a new Mat3f object from another.
		 * 
		 * @param o 
		 * 		The other Mat3f to copy.
		 */
		Mat3f(const Mat3f& o);

		/**
		 * @brief Copies a Mat3f object.
		 * 
		 * @param o 
		 * 		The other Mat3f to copy.
		 */
		void operator=(const Mat3f& o);

		//Object and Class Stuff
		const Class* getClass();
		static const Class myClass;

		/**
		 * @brief Returns the Identity 3x3 matrix
		 * 
		 * @return Mat3f 
		 */
		static Mat3f getIdentity();

		double* operator[](int row);

		Mat3f operator+(Mat3f other);
		Mat3f operator-(Mat3f other);
		Mat3f operator*(Mat3f other);

		Mat3f operator*(double other);
		friend Mat3f operator*(double value, Mat3f other)
		{
			return other*value;
		}
		
		void operator*=(double other);

		void operator+=(Mat3f other);
		void operator-=(Mat3f other);

		Vec3f operator*(Vec3f other);

		bool operator==(Mat3f other);
		bool operator!=(Mat3f other);

		//Converting into an array
		/**
		 * @brief Fills a float array of at least size 9 with the matrix values
		 * 		
		 * @param buffer 
		 * 		Must be of size 9 or greater.
		 */
		void fillArray(float* buffer);

		/**
		 * @brief Fills a float array of at least size 9 with the matrix values
		 * 		
		 * @param buffer 
		 * 		Must be of size 9 or greater.
		 */
		void fillArray(double* buffer);

		/**
		 * @brief Creates a new float array of size 9 and fills it
		 * 		with the matrix values.
		 */
		float* convertToFloatArray();

		/**
		 * @brief Creates a new double array of size 9 and fills it
		 * 		with the matrix values.
		 */
		double* convertToDoubleArray();
	};

} //NAMESPACE glib END