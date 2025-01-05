#pragma once
#include "BuildOptions.h"
#include "SimpleSerialization.h"
#include "Vec3f.h"
#include "Vec2f.h"

namespace smpl
{

	class DLL_OPTION Vec4f : public SerializedObject
	{
	public:

		/**
		 * @brief Construct a new Vec4f object
		 * 
		 */
		Vec4f();

		/**
		 * @brief Construct a new Vec4f object
		 * 
		 * @param x 
		 * @param y 
		 * @param z 
		 * @param w 
		 */
		Vec4f(float x, float y, float z, float w);
		
		/**
		 * @brief Construct a new Vec4f object from a Vec3f object and
		 * 		an optional additional value for w.
		 * 
		 * @param other 
		 */
		Vec4f(const Vec3f& other, float w);

		/**
		 * @brief Construct a new Vec4f object from a Vec3f object and 2 optional
		 * 		values for z and w.
		 * 
		 * @param other 
		 */
		Vec4f(const Vec2f& other, float z, float w);

		/**
		 * @brief Destroy the Vec4f object
		 * 
		 */
		~Vec4f();

		float x = 0;
		float y = 0;
		float z = 0;
		float w = 0;

		/**
		 * @brief Gets the X value for the vector
		 * 
		 * @return float 
		 */
		float getX();

		/**
		 * @brief Gets the Y value for the vector
		 * 
		 * @return float 
		 */
		float getY();

		/**
		 * @brief Gets the Z value for the vector
		 * 
		 * @return float 
		 */
		float getZ();

		/**
		 * @brief Gets the W value for the vector
		 * 
		 * @return float 
		 */
		float getW();

		/**
		 * @brief Sets the X value for the vector
		 * 
		 * @param value 
		 */
		void setX(float value);

		/**
		 * @brief Sets the Y value for the vector
		 * 
		 * @param value 
		 */
		void setY(float value);

		/**
		 * @brief Sets the Z value for the vector
		 * 
		 * @param value 
		 */
		void setZ(float value);

		/**
		 * @brief Sets the W value for the vector
		 * 
		 * @param value 
		 */
		void setW(float value);


		/**
		 * @brief Copies values from a Vec2f.
		 * 		The z and w values will not be set.
		 * 
		 * @param value 
		 */
		void setValues(const Vec2f& value);

		/**
		 * @brief Copies values from a Vec3f.
		 * 		The w values will not be set.
		 * 
		 * @param value 
		 */
		void setValues(const Vec3f& value);

		/**
		 * @brief Copies values from a Vec4f.
		 * 
		 * @param value 
		 */
		void setValues(const Vec4f& value);

		/**
		 * @brief Converts the Vec4f object to a Vec3f object by dropping the w value.
		 * 
		 * @return Vec3f 
		 */
		Vec3f toVec3f() const;

		/**
		 * @brief Get the Length of the vector
		 * 
		 * @return float 
		 */
		float getLength() const;

		/**
		 * @brief Gets the normalized version of the vector.
		 * 		Returns a vector with the same direction but a length of 1.
		 * 
		 * @return Vec4f 
		 */
		Vec4f normalize() const;

		/**
		 * @brief Computes the piecewise multiplication between
		 * 		the 2 vectors
		 * 
		 * @param other 
		 * @return Vec4f 
		 */
		Vec4f hadamardProduct(const Vec4f& b) const;

		/**
		 * @brief Computes the dot product between this vector and another
		 * 
		 * @param other 
		 * @return float 
		 */
		float dot(const Vec4f& b) const;

		/**
		 * @brief Computes the scalar vector projection between 2 vectors
		 * 
		 * @param other 
		 * @return float 
		 */
		float project(const Vec4f& b) const;

		/**
		 * @brief Reflects this vector across the vector "b"
		 * 
		 * @param other 
		 * @return Vec4f 
		 */
		Vec4f reflect(const Vec4f& b) const;

		Vec4f operator-(const Vec4f& other) const;
		Vec4f operator+(const Vec4f& other) const;

		bool operator==(const Vec4f& other) const;
		bool operator!=(const Vec4f& other) const;
		Vec4f operator-() const;

		void operator+=(const Vec4f& other);
		void operator-=(const Vec4f& other);

		void operator*=(float other);
		void operator/=(float other);

		Vec4f operator*(float other) const;
		Vec4f operator/(float other) const;

		friend Vec4f operator*(float value, const Vec4f& other)
		{
			return other*value;
		}

		
		Vec4f broadcastAdd(float v);
		Vec4f broadcastSubtract(float v);
		Vec4f broadcastFunction(std::function<float(float)> func);
		Vec4f broadcastFunction(float (*func)(float));

		/**
		 * @brief Converts the Vec4f object to a Column Matrix object
		 * 
		 * @return MatrixF 
		 */
		MatrixF toMatrix() const;
		operator MatrixF() const;

	SERIALIZE_CLASS(x, y, z, w)
	};

} //NAMESPACE glib END