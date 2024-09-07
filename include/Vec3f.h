#pragma once
#include "BuildOptions.h"
#include "Serializable.h"
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
		Vec3f(Vec2f other, float z=0.0);

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

		//Object and RootClass Stuff
		static const RootClass globalClass;
		virtual const RootClass* getClass();
		std::unordered_map<std::string, SerializedData> getSerializedVariables();

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
		 * @return float 
		 */
		float getLength();

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

		void operator*=(float other);
		void operator/=(float other);

		bool operator==(Vec3f other);
		bool operator!=(Vec3f other);

		Vec3f operator*(float value);
		Vec3f operator/(float value);

		friend Vec3f operator*(float value, Vec3f other)
		{
			return other*value;
		}

		/**
		 * @brief Converts the Vec3f to a GeneralVector object
		 * 
		 * @return GeneralVector 
		 */
		GeneralVector toGeneralVector();
		operator GeneralVector() const;

	};

} //NAMESPACE glib END