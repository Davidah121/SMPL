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

		//Object and Class Stuff
		const Class* getClass();
		static const Class myClass;
		
		ComplexNumber operator+(ComplexNumber other);
		ComplexNumber operator-(ComplexNumber other);
		ComplexNumber operator-();
		ComplexNumber operator*(ComplexNumber other);

		/**
		 * @brief Does component to component multiplication.
		 * 		Ex:
		 * 			real*other.x
		 * 			imaginary*other.y
		 * @param other
		 * 		A Vec2f to use for the component multiplication
		 * @return ComplexNumber
		 */
		ComplexNumber operator*(Vec2f other);

		/**
		 * @brief Does component to component division.
		 * 		Ex:
		 * 			real/other.x
		 * 			imaginary/other.y
		 * @param other
		 * 		A Vec2f to use for the component division
		 * @return ComplexNumber
		 */
		ComplexNumber operator/(Vec2f other);

		ComplexNumber operator*(int other);
		ComplexNumber operator*(float other);
		ComplexNumber operator*(double other);

		ComplexNumber operator/(int other);
		ComplexNumber operator/(float other);
		ComplexNumber operator/(double other);

		void operator+=(ComplexNumber other);
		void operator-=(ComplexNumber other);
		void operator*=(ComplexNumber other);

		/**
		 * @brief Does component to component multiplication.
		 * 		Ex:
		 * 			real*other.x
		 * 			imaginary*other.y
		 */
		void operator*=(Vec2f other);

		/**
		 * @brief Does component to component division.
		 * 		Ex:
		 * 			real*other.x
		 * 			imaginary*other.y
		 */
		void operator/=(Vec2f other);

		void operator*=(int other);
		void operator*=(float other);
		void operator*=(double other);

		void operator/=(int other);
		void operator/=(float other);
		void operator/=(double other);

		//logic operators
		bool operator==(ComplexNumber other);
		bool operator!=(ComplexNumber other);
		
		double real = 0;
		double imaginary = 0;
	private:
	};

}