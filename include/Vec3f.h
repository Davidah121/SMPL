#pragma once
#include "BuildOptions.h"
#include "SimpleSerialization.h"
#include "Vec2f.h"

namespace smpl
{

	class DLL_OPTION Vec3f : public SerializedObject
	{
	public:
		/**
		 * @brief Construct a new Vec3f object
		 * 
		 */
		Vec3f();

		/**
		 * @brief Construct a new Vec3f object using a Vec2f and an
		 * 		optional additional z value.
		 * 
		 * @param other 
		 */
		Vec3f(const Vec2f& other, float z=0.0);

		/**
		 * @brief Construct a new Vec3f object
		 * 
		 * @param x 
		 * @param y 
		 * @param z 
		 */
		Vec3f(float x, float y, float z);

		/**
		 * @brief Destroy the Vec3f object
		 * 
		 */
		~Vec3f();

		float x = 0;
		float y = 0;
		float z = 0;

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
		 * @brief Copies another Vec3f objects values
		 * 
		 * @param other 
		 */
		void setValues(const Vec3f& other);

		/**
		 * @brief Copies another Vec2f objects values
		 * 		The z value will not be set.
		 * 
		 * @param other 
		 */
		void setValues(const Vec2f& other);
		
		/**
		 * @brief Converts the Vec3f object to a Vec2f object by dropping the z value.
		 * 
		 * @return Vec2f 
		 */
		Vec2f toVec2f() const;

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
		 * @return Vec3f 
		 */
		Vec3f normalize() const;

		/**
		 * @brief Computes the piecewise multiplication between
		 * 		the 2 vectors
		 * 
		 * @param other 
		 * @return Vec3f 
		 */
		Vec3f hadamardProduct(const Vec3f& b) const;

		/**
		 * @brief Computes the dot product between this vector and another
		 * 
		 * @param other 
		 * @return float 
		 */
		float dot(const Vec3f& b) const;

		/**
		 * @brief Computes the scalar vector projection between 2 vectors
		 * 
		 * @param other 
		 * @return float 
		 */
		float project(const Vec3f& b) const;

		/**
		 * @brief Reflects this vector across the vector "b"
		 * 
		 * @param other 
		 * @return Vec3f 
		 */
		Vec3f reflect(const Vec3f& b) const;

		/**
		 * @brief Computes the cross product between 2 vectors
		 * 
		 * @param v2 
		 * @return Vec3f 
		 */
		Vec3f crossProduct(const Vec3f& v2) const;

		Vec3f operator-(const Vec3f& other) const;
		Vec3f operator+(const Vec3f& other) const;
		Vec3f operator-() const;

		void operator+=(const Vec3f& other);
		void operator-=(const Vec3f& other);

		void operator*=(float other);
		void operator/=(float other);

		bool operator==(const Vec3f& other) const;
		bool operator!=(const Vec3f& other) const;

		Vec3f operator*(float value) const;
		Vec3f operator/(float value) const;

		friend Vec3f operator*(float value, const Vec3f& other)
		{
			return other*value;
		}
		
		Vec3f broadcastAdd(float v);
		Vec3f broadcastSubtract(float v);
		Vec3f broadcastFunction(std::function<float(float)> func);
		Vec3f broadcastFunction(float (*func)(float));

		/**
		 * @brief Converts the Vec3f to a Column Matrix object
		 * 
		 * @return MatrixF 
		 */
		MatrixF toMatrix() const;
		operator MatrixF() const;

	SERIALIZE_CLASS(x, y, z)
	};

} //NAMESPACE glib END