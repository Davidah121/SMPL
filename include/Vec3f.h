#pragma once
#include "Vec2f.h"
#include "Object.h"

namespace glib
{

	class Vec3f : public Object
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
		Vec3f(Vec2f other, double z=0.0);

		/**
		 * @brief Construct a new Vec3f object
		 * 
		 * @param x 
		 * @param y 
		 * @param z 
		 */
		Vec3f(double x, double y, double z);

		/**
		 * @brief Destroy the Vec3f object
		 * 
		 */
		~Vec3f();

		//Object and RootClass Stuff
		static const RootClass globalClass;

		double x = 0;
		double y = 0;
		double z = 0;

		/**
		 * @brief Gets the X value for the vector
		 * 
		 * @return double 
		 */
		double getX();

		/**
		 * @brief Gets the Y value for the vector
		 * 
		 * @return double 
		 */
		double getY();

		/**
		 * @brief Gets the Z value for the vector
		 * 
		 * @return double 
		 */
		double getZ();

		/**
		 * @brief Sets the X value for the vector
		 * 
		 * @param value 
		 */
		void setX(double value);

		/**
		 * @brief Sets the Y value for the vector
		 * 
		 * @param value 
		 */
		void setY(double value);

		/**
		 * @brief Sets the Z value for the vector
		 * 
		 * @param value 
		 */
		void setZ(double value);

		/**
		 * @brief Copies another Vec3f objects values
		 * 
		 * @param other 
		 */
		void setValues(Vec3f other);

		/**
		 * @brief Copies another Vec2f objects values
		 * 		The z value will not be set.
		 * 
		 * @param other 
		 */
		void setValues(Vec2f other);
		
		/**
		 * @brief Converts the Vec3f object to a Vec2f object by dropping the z value.
		 * 
		 * @return Vec2f 
		 */
		Vec2f toVec2f();

		/**
		 * @brief Get the Length of the vector
		 * 
		 * @return double 
		 */
		double getLength();

		/**
		 * @brief Gets the normalized version of the vector.
		 * 		Returns a vector with the same direction but a length of 1.
		 * 
		 * @return Vec3f 
		 */
		Vec3f normalize();

		Vec3f operator-(Vec3f other);
		Vec3f operator+(Vec3f other);
		Vec3f operator-();

		void operator+=(Vec3f other);
		void operator-=(Vec3f other);

		void operator*=(double other);
		void operator/=(double other);

		bool operator==(Vec3f other);
		bool operator!=(Vec3f other);

		Vec3f operator*(double value);
		Vec3f operator/(double value);

		friend Vec3f operator*(double value, Vec3f other)
		{
			return other*value;
		}

		//Converting into an array
		/**
		 * @brief Fills a float array of at least size 3 with the
		 * 		x, y, and z values.
		 * 		
		 * @param buffer 
		 * 		Must be of size 3 or greater.
		 */
		void fillArray(float* buffer);

		/**
		 * @brief Fills a float array of at least size 3 with the
		 * 		x, y, and z values.
		 * 		
		 * @param buffer 
		 * 		Must be of size 3 or greater.
		 */
		void fillArray(double* buffer);

		/**
		 * @brief Creates a new float array of size 3 and fills it
		 * 		with the x, y, and z values.
		 */
		float* convertToFloatArray();

		/**
		 * @brief Creates a new double array of size 3 and fills it
		 * 		with the x, y, and z values.
		 */
		double* convertToDoubleArray();

		/**
		 * @brief Converts the Vec3f to a GeneralVector object
		 * 
		 * @return GeneralVector 
		 */
		GeneralVector toGeneralVector();
		operator GeneralVector() const;

	};

} //NAMESPACE glib END