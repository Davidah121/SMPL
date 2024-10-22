#pragma once
#include "BuildOptions.h"
#include "Matrix.h"
#include "Vec2f.h"

namespace smpl
{

	///A special Matrix that is a 2x2 matrix. Commonly used
	class DLL_OPTION Mat2f : public Matrix
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
		Mat2f(float v1, float v2, float v3, float v4);

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

		float* operator[](int row) const;

		Mat2f operator+(const Mat2f& other) const;
		Mat2f operator-(const Mat2f& other) const;
		Mat2f operator*(const Mat2f& other) const;
		Mat2f operator*(float value) const;
		Vec2f operator*(const Vec2f& other) const;
		bool operator==(const Mat2f& other) const;
		bool operator!=(const Mat2f& other) const;

		void operator*=(float value);
		void operator+=(const Mat2f& other);
		void operator-=(const Mat2f& other);

		friend Mat2f operator*(float value, const Mat2f& other)
		{
			return other*value;
		}
	};

} //NAMESPACE smpl END