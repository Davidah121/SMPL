#pragma once
#include "BuildOptions.h"
#include "Matrix.h"
#include "Vec4f.h"

namespace smpl
{

	class DLL_OPTION Mat4f : public Matrix
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
		Mat4f(float v1, float v2, float v3, float v4, 
			float v5, float v6, float v7, float v8,
			float v9, float v10, float v11, float v12, 
			float v13, float v14, float v15, float v16);

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
		virtual const RootClass* getClass();

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

		float* operator[](int row);

		Mat4f operator+(Mat4f other);
		Mat4f operator-(Mat4f other);
		Mat4f operator*(Mat4f other);

		Mat4f operator*(float other);
		friend Mat4f operator*(float value, Mat4f other)
		{
			return other*value;
		}

		void operator*=(float other);

		void operator+=(Mat4f other);
		void operator-=(Mat4f other);

		Vec4f operator*(Vec4f other);

		bool operator==(Mat4f other);
		bool operator!=(Mat4f other);
	};

} //NAMESPACE smpl END