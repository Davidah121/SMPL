#pragma once
#include "Object.h"
#include "GeneralVector.h"

namespace glib
{
		
	class Vec2f : public Object
	{
	public:
		/**
		 * @brief Construct a new Vec2f object
		 * 
		 * @param x 
		 * @param y 
		 */
		Vec2f(double x, double y);

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

		//Object and Class Stuff
		const Class* getClass();
		static const Class myClass;

		double x = 0;
		double y = 0;

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
		 * @brief Copies the values of another Vec2f
		 * 
		 * @param other 
		 */
		void setValues(Vec2f other);

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
		 * @return Vec2f 
		 */
		Vec2f normalize();
		
		Vec2f operator-(Vec2f other);
		Vec2f operator+(Vec2f other);

		void operator+=(Vec2f other);
		void operator-=(Vec2f other);

		void operator*=(double other);
		void operator/=(double other);

		bool operator==(Vec2f other);
		bool operator!=(Vec2f other);

		Vec2f operator-();

		Vec2f operator*(double value);
		Vec2f operator/(double value);

		friend Vec2f operator*(double value, Vec2f other)
		{
			return other*value;
		}

		//Converting into an array
		/**
		 * @brief Fills a float array of at least size 2 with the
		 * 		x and y values.
		 * 		
		 * @param buffer 
		 * 		Must be of size 2 or greater.
		 */
		void fillArray(float* buffer);

		/**
		 * @brief Fills a float array of at least size 2 with the
		 * 		x and y values.
		 * 		
		 * @param buffer 
		 * 		Must be of size 2 or greater.
		 */
		void fillArray(double* buffer);

		/**
		 * @brief Creates a new float array of size 2 and fills it
		 * 		with the x and y values.
		 */
		float* convertToFloatArray();

		/**
		 * @brief Creates a new double array of size 2 and fills it
		 * 		with the x and y values.
		 */
		double* convertToDoubleArray();

		//casting to generic vector
		/**
		 * @brief Converts the Vec2f to a GeneralVector Object
		 * 
		 * @return GeneralVector 
		 */
		GeneralVector toGeneralVector();
		operator GeneralVector() const;

	};

} //NAMESPACE glib END