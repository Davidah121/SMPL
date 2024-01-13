#pragma once
#include "Object.h"

namespace glib
{

	class GeneralVector : public Object
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
		 * @brief Destroys a GeneralVector object and clears the memory used.
		 */
		~GeneralVector();

		//Object and RootClass Stuff
		static const RootClass globalClass;

		/**
		 * @brief Returns the value at the specified location or dimension.
		 * @param location
		 * 		The dimension or location to get.
		 * @return double
		 */
		double getValue(int location);

		/**
		 * @brief Sets the value at the specified location or dimension.
		 * @param value
		 * 		The value to set.
		 * @param location
		 * 		The dimension or location to get.
		 */
		void setValue(double value, int location);

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
		 * @return GeneralVector 
		 */
		GeneralVector normalize();

		GeneralVector operator*(double value);
		GeneralVector operator/(double value);

		friend GeneralVector operator*(double value, GeneralVector other)
		{
			return other*value;
		}

		GeneralVector operator+(GeneralVector o);
		GeneralVector operator-(GeneralVector o);

		void operator*=(double value);
		void operator/=(double value);

		void operator+=(GeneralVector o);
		void operator-=(GeneralVector o);

		GeneralVector operator-();

		double& operator[](int index);

		bool operator==(GeneralVector other);
		bool operator!=(GeneralVector other);

		int getSize();
	private:
		
		void copy(const GeneralVector& o);
		int size = 0;
		double* values = nullptr;
	};

} //NAMESPACE glib END