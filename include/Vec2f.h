#pragma once
#include "BuildOptions.h"
#include "SimpleSerialization.h"
#include "Matrix.h"

namespace smpl
{
		
	class DLL_OPTION Vec2f
	{
	public:
		/**
		 * @brief Construct a new Vec2f object
		 * 
		 * @param x 
		 * @param y 
		 */
		Vec2f(float x, float y);

		/**
		 * @brief Construct a new Vec2f object
		 * 
		 */
		Vec2f();

		/**
		 * @brief Destroy the Vec2f object
		 * 
		 */
		~Vec2f();
		
		float x = 0;
		float y = 0;

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
		 * @brief Copies the values of another Vec2f
		 * 
		 * @param other 
		 */
		void setValues(const Vec2f& other);

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
		 * @return Vec2f 
		 */
		Vec2f normalize() const;

		/**
		 * @brief Computes the piecewise multiplication between
		 * 		the 2 vectors
		 * 
		 * @param other 
		 * @return Vec2f 
		 */
		Vec2f hadamardProduct(const Vec2f& b) const;

		/**
		 * @brief Computes the dot product between this vector and another
		 * 
		 * @param other 
		 * @return float 
		 */
		float dot(const Vec2f& b) const;

		/**
		 * @brief Computes the scalar vector projection between 2 vectors
		 * 
		 * @param other 
		 * @return float 
		 */
		float project(const Vec2f& b) const;

		/**
		 * @brief Reflects this vector across the vector "b"
		 * 
		 * @param other 
		 * @return Vec2f 
		 */
		Vec2f reflect(const Vec2f& b) const;

		/**
		 * @brief Returns a new vector that is the inverse.
		 * 		Returns this : Vec2f( -f.y, f.x )
		 * 
		 * @return Vec2f 
		 */
		Vec2f inverse() const;
		
		Vec2f operator-(const Vec2f& b) const;
		Vec2f operator+(const Vec2f& b) const;

		void operator+=(const Vec2f& b);
		void operator-=(const Vec2f& b);

		void operator*=(float b);
		void operator/=(float b);

		bool operator==(const Vec2f& b) const;
		bool operator!=(const Vec2f& b) const;

		Vec2f operator-() const;

		Vec2f operator*(float value) const;
		Vec2f operator/(float value) const;

		friend Vec2f operator*(float value, const Vec2f& other)
		{
			return other*value;
		}

		Vec2f broadcastAdd(float v);
		Vec2f broadcastSubtract(float v);
		Vec2f broadcastFunction(std::function<float(float)> func);
		Vec2f broadcastFunction(float (*func)(float));

		//casting to generic vector
		/**
		 * @brief Converts the Vec2f to a Column Matrix
		 * 
		 * @return Matrix 
		 */
		MatrixF toMatrix() const;
		operator MatrixF() const;

	SERIALIZE_CLASS_LIGHT(Vec2f, x, y)
	};

	void formatToString(StringStream& stream, const Vec2f& v, const std::string& options);
} //NAMESPACE glib END