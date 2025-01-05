#pragma once
#include "BuildOptions.h"
#include "Matrix.h"
#include "Vec3f.h"

namespace smpl
{

	class DLL_OPTION Mat3f : public MatrixF
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
		Mat3f(float v1, float v2, float v3, float v4, float v5, float v6, float v7, float v8, float v9);

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

		/**
		 * @brief Returns the Identity 3x3 matrix
		 * 
		 * @return Mat3f 
		 */
		static Mat3f getIdentity();

		float* operator[](int row) const;

		Mat3f operator+(const Mat3f& other) const;
		Mat3f operator-(const Mat3f& other) const;
		Mat3f operator*(const Mat3f& other) const;

		Mat3f operator*(float other) const;
		friend Mat3f operator*(float value, const Mat3f& other)
		{
			return other*value;
		}
		
		void operator*=(float other);

		void operator+=(const Mat3f& other);
		void operator-=(const Mat3f& other);

		Vec3f operator*(const Vec3f& other) const;

		bool operator==(const Mat3f& other) const;
		bool operator!=(const Mat3f& other) const;
		
		
		SERIALIZE_SUPER_CLASS(MatrixF)
		SERIALIZE_CLASS() //Override class TypeInfo only
	};

} //NAMESPACE smpl END