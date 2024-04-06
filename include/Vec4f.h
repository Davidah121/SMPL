#pragma once
#include "Serializable.h"
#include "Vec3f.h"
#include "Vec2f.h"

namespace smpl
{

	class Vec4f : public SerializedObject
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

		Vec4f(const Vec4f& other);
		void operator=(const Vec4f& other);
		
		/**
		 * @brief Construct a new Vec4f object from a Vec3f object and
		 * 		an optional additional value for w.
		 * 
		 * @param other 
		 */
		Vec4f(Vec3f other, double w);

		/**
		 * @brief Construct a new Vec4f object from a Vec3f object and 2 optional
		 * 		values for z and w.
		 * 
		 * @param other 
		 */
		Vec4f(Vec2f other, double z, double w);

		/**
		 * @brief Destroy the Vec4f object
		 * 
		 */
		~Vec4f();

		//Object and RootClass Stuff
		static const RootClass globalClass;
		std::unordered_map<std::string, SerializedData> getSerializedVariables();

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

		bool operator==(Vec4f other);
		bool operator!=(Vec4f other);
		Vec4f operator-();

		void operator+=(Vec4f other);
		void operator-=(Vec4f other);

		void operator*=(double other);
		void operator/=(double other);

		Vec4f operator*(double other);
		Vec4f operator/(double other);

		friend Vec4f operator*(double value, Vec4f other)
		{
			return other*value;
		}

		//Converting into an array
		/**
		 * @brief Fills a float array of at least size 4 with the
		 * 		x, y, z, and w values.
		 * 		
		 * @param buffer 
		 * 		Must be of size 4 or greater.
		 */
		void fillArray(float* buffer);

		/**
		 * @brief Fills a float array of at least size 4 with the
		 * 		x, y, z, and w values.
		 * 		
		 * @param buffer 
		 * 		Must be of size 4 or greater.
		 */
		void fillArray(double* buffer);

		/**
		 * @brief Creates a new float array of size 4 and fills it
		 * 		with the x, y, z, and w values.
		 */
		float* convertToFloatArray();

		/**
		 * @brief Creates a new double array of size 4 and fills it
		 * 		with the x, y, z, and w values.
		 */
		double* convertToDoubleArray();

		/**
		 * @brief Converts the Vec4f object to a GeneralVector object
		 * 
		 * @return GeneralVector 
		 */
		GeneralVector toGeneralVector();
		operator GeneralVector() const;
	};

} //NAMESPACE glib END