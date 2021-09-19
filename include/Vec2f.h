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

		Vec2f operator-(Vec2f other);
		Vec2f operator+(Vec2f other);

		void operator+=(Vec2f other);
		void operator-=(Vec2f other);
		void operator*=(Vec2f other);
		void operator/=(Vec2f other);


		void operator*=(double other);
		void operator/=(double other);

		void operator*=(int other);
		void operator/=(int other);

		void operator*=(long other);
		void operator/=(long other);

		void operator*=(float other);
		void operator/=(float other);

		bool operator==(Vec2f other);
		bool operator!=(Vec2f other);

		Vec2f operator-();

		Vec2f operator*(int value);
		Vec2f operator*(float value);
		Vec2f operator*(double value);
		Vec2f operator*(long value);
		Vec2f operator*(Vec2f other);

		Vec2f operator/(int value);
		Vec2f operator/(float value);
		Vec2f operator/(double value);
		Vec2f operator/(long value);
		Vec2f operator/(Vec2f other);

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