#pragma once
#include "Object.h"
#include "Vec2f.h"

namespace glib
{

	class ComplexNumber : public Object
	{
	public:
		/**
		 * @brief Creates a ComplexNumber object.
		 * 		Allows for mathematical operations using complex numbers.
		 */
		ComplexNumber();

		/**
		 * @brief Creates a ComplexNumber object.
		 * 		Allows for mathematical operations using complex numbers.
		 * @param real
		 * 		Specifies the real component.
		 * @param imaginary
		 * 		Specifies the imaginary component.
		 */
		ComplexNumber(double real, double imaginary);
		/**
		 * @brief Creates a ComplexNumber object from another ComplexNumber.
		 * @param other
		 * 		The ComplexNumber to copy
		 */
		ComplexNumber(const ComplexNumber& other);

		/**
		 * @brief Destroys a ComplexNumber object.
		 */
		~ComplexNumber();

		/**
		 * @brief Converts the Complex number to a Vec2f.
		 * 		x = real component
		 * 		y = imaginary component
		 * 
		 * @return Vec2f 
		 */
		Vec2f toVec2f();

		//Object and RootClass Stuff
		static const RootClass globalClass;
		
		ComplexNumber operator+(ComplexNumber other);
		ComplexNumber operator-(ComplexNumber other);
		ComplexNumber operator-();
		ComplexNumber operator*(ComplexNumber other);

		ComplexNumber operator*(double other);
		ComplexNumber operator/(double other);

		friend ComplexNumber operator*(double value, ComplexNumber other)
		{
			return other*value;
		}

		void operator+=(ComplexNumber other);
		void operator-=(ComplexNumber other);
		void operator*=(ComplexNumber other);

		void operator*=(double other);
		void operator/=(double other);

		//logic operators
		bool operator==(ComplexNumber other);
		bool operator!=(ComplexNumber other);
		
		double real = 0;
		double imaginary = 0;
	private:
	};

}