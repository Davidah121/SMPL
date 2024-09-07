#pragma once
#include "BuildOptions.h"
#include "Serializable.h"
#include "GeneralVector.h"

namespace smpl
{
		
	class DLL_OPTION Vec2f : public SerializedObject
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

		//Object and RootClass Stuff
		static const RootClass globalClass;
		virtual const RootClass* getClass();
		virtual std::unordered_map<std::string, SerializedData> getSerializedVariables();

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
		void setValues(Vec2f other);

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
		 * @return Vec2f 
		 */
		Vec2f normalize();

		/**
		 * @brief Computes the piecewise multiplication between
		 * 		the 2 vectors
		 * 
		 * @param other 
		 * @return Vec2f 
		 */
		Vec2f hadamardProduct(Vec2f other);
		
		Vec2f operator-(Vec2f other);
		Vec2f operator+(Vec2f other);

		void operator+=(Vec2f other);
		void operator-=(Vec2f other);

		void operator*=(float other);
		void operator/=(float other);

		bool operator==(Vec2f other);
		bool operator!=(Vec2f other);

		Vec2f operator-();

		Vec2f operator*(float value);
		Vec2f operator/(float value);

		friend Vec2f operator*(float value, Vec2f other)
		{
			return other*value;
		}

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