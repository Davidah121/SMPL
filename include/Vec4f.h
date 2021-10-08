#pragma once
#include "Vec3f.h"
#include "Vec2f.h"

namespace glib
{

	class Vec4f : public Object
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
		Vec4f(double x, double y, double z, double w);

		/**
		 * @brief Construct a new Vec4f object from a Vec3f object
		 * 		The w value will not be set.
		 * 
		 * @param other 
		 */
		Vec4f(Vec3f other);

		/**
		 * @brief Construct a new Vec4f object from a Vec3f object
		 * 		The z and w values will not be set.
		 * 
		 * @param other 
		 */
		Vec4f(Vec2f other);

		/**
		 * @brief Destroy the Vec4f object
		 * 
		 */
		~Vec4f();

		//Object and Class Stuff
		const Class* getClass();
		static const Class myClass;

		double x = 0;
		double y = 0;
		double z = 0;
		double w = 0;

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
		 * @brief Gets the W value for the vector
		 * 
		 * @return double 
		 */
		double getW();

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
		 * @brief Sets the W value for the vector
		 * 
		 * @param value 
		 */
		void setW(double value);


		/**
		 * @brief Copies values from a Vec2f.
		 * 		The z and w values will not be set.
		 * 
		 * @param value 
		 */
		void setValues(Vec2f value);

		/**
		 * @brief Copies values from a Vec3f.
		 * 		The w values will not be set.
		 * 
		 * @param value 
		 */
		void setValues(Vec3f value);

		/**
		 * @brief Copies values from a Vec4f.
		 * 
		 * @param value 
		 */
		void setValues(Vec4f value);

		/**
		 * @brief Converts the Vec4f object to a Vec3f object by dropping the w value.
		 * 
		 * @return Vec3f 
		 */
		Vec3f toVec3f();

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
		 * @return Vec4f 
		 */
		Vec4f normalize();

		Vec4f operator-(Vec4f other);
		Vec4f operator+(Vec4f other);

		Vec4f operator*(long other);
		Vec4f operator*(double other);
		Vec4f operator*(float other);
		Vec4f operator*(int other);
		Vec4f operator*(Vec4f other);

		bool operator==(Vec4f other);
		bool operator!=(Vec4f other);
		Vec4f operator-();

		void operator+=(Vec4f other);
		void operator-=(Vec4f other);
		void operator*=(Vec4f other);
		void operator/=(Vec4f other);

		void operator*=(double other);
		void operator/=(double other);

		void operator*=(int other);
		void operator/=(int other);

		void operator*=(long other);
		void operator/=(long other);

		void operator*=(float other);
		void operator/=(float other);

		Vec4f operator/(long other);
		Vec4f operator/(double other);
		Vec4f operator/(float other);
		Vec4f operator/(int other);
		Vec4f operator/(Vec4f other);

		/**
		 * @brief Converts the Vec4f object to a GeneralVector object
		 * 
		 * @return GeneralVector 
		 */
		GeneralVector toGeneralVector();
		operator GeneralVector() const;
	};

} //NAMESPACE glib END