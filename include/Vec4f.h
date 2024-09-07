#pragma once
#include "BuildOptions.h"
#include "Serializable.h"
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
		Vec4f(Vec3f other, float w);

		/**
		 * @brief Construct a new Vec4f object from a Vec3f object and 2 optional
		 * 		values for z and w.
		 * 
		 * @param other 
		 */
		Vec4f(Vec2f other, float z, float w);

		/**
		 * @brief Destroy the Vec4f object
		 * 
		 */
		~Vec4f();

		//Object and RootClass Stuff
		static const RootClass globalClass;
		virtual const RootClass* getClass();
		std::unordered_map<std::string, SerializedData> getSerializedVariables();

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
		 * @return float 
		 */
		float getLength();

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

		void operator*=(float other);
		void operator/=(float other);

		Vec4f operator*(float other);
		Vec4f operator/(float other);

		friend Vec4f operator*(float value, Vec4f other)
		{
			return other*value;
		}

		/**
		 * @brief Converts the Vec4f object to a GeneralVector object
		 * 
		 * @return GeneralVector 
		 */
		GeneralVector toGeneralVector();
		operator GeneralVector() const;
	};

} //NAMESPACE glib END