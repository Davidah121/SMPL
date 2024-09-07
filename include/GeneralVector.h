#pragma once
#include "BuildOptions.h"
#include "Object.h"

namespace smpl
{

	class DLL_OPTION GeneralVector : public Object
	{
	public:
		/**
		 * @brief Creates a GeneralVector object.
		 * 		This is a Mathematical vector used for different math operations.
		 * 		A GeneralVector can be of any size to the maximum of an signed integer.
		 * @param size
		 * 		The dimension of the vector.
		 * 		Default is 0.
		 */
		GeneralVector(int size=0);

		/**
		 * @brief Copies a GeneralVector object from another GeneralVector object
		 * @param o
		 * 		The other valid GeneralVector object to copy.
		 */
		GeneralVector(const GeneralVector& o);

		/**
		 * @brief Copies a GeneralVector object from another GeneralVector object
		 * @param o
		 * 		The other valid GeneralVector object to copy.
		 */
		void operator=(const GeneralVector& o);

		/**
		 * @brief Moves a GeneralVector object from another GeneralVector object
		 * @param o
		 * 		The other valid GeneralVector object to copy.
		 */
		GeneralVector(GeneralVector&& o) noexcept;

		/**
		 * @brief Moves a GeneralVector object from another GeneralVector object
		 * @param o
		 * 		The other valid GeneralVector object to copy.
		 */
		void operator=(GeneralVector&& o) noexcept;

		/**
		 * @brief Destroys a GeneralVector object and clears the memory used.
		 */
		~GeneralVector();

		//Object and RootClass Stuff
		static const RootClass globalClass;
		virtual const RootClass* getClass();

		/**
		 * @brief Returns the value at the specified location or dimension.
		 * @param location
		 * 		The dimension or location to get.
		 * @return float
		 */
		float getValue(int location);

		/**
		 * @brief Sets the value at the specified location or dimension.
		 * @param value
		 * 		The value to set.
		 * @param location
		 * 		The dimension or location to get.
		 */
		void setValue(float value, int location);

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
		 * @return GeneralVector 
		 */
		GeneralVector normalize();

		GeneralVector operator*(float value);
		GeneralVector operator/(float value);

		friend GeneralVector operator*(float value, GeneralVector other)
		{
			return other*value;
		}

		GeneralVector operator+(GeneralVector o);
		GeneralVector operator-(GeneralVector o);

		void operator*=(float value);
		void operator/=(float value);

		void operator+=(GeneralVector o);
		void operator-=(GeneralVector o);

		GeneralVector operator-();

		float& operator[](int index);

		bool operator==(GeneralVector other);
		bool operator!=(GeneralVector other);

		int getSize();
	private:
		
		void copy(const GeneralVector& o);
		int size = 0;
		float* values = nullptr;
	};

} //NAMESPACE smpl END