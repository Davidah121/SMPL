#include "ComplexNumber.h"

namespace glib
{

	#pragma region ClassStuff
	const Class ComplexNumber::myClass = Class("ComplexNumber", {&Object::myClass});
	const Class* ComplexNumber::getClass()
	{
		return &ComplexNumber::myClass;
	}
	#pragma endregion

	ComplexNumber::ComplexNumber()
	{
	}

	ComplexNumber::ComplexNumber(double real, double imaginary)
	{
		this->real = real;
		this->imaginary = imaginary;
	}

	ComplexNumber::ComplexNumber(const ComplexNumber& other)
	{
		real = other.real;
		imaginary = other.imaginary;
	}

	ComplexNumber::~ComplexNumber()
	{
	}

	Vec2f ComplexNumber::toVec2f()
	{
		return Vec2f(real, imaginary);
	}

	ComplexNumber ComplexNumber::operator+(ComplexNumber other)
	{
		return ComplexNumber(real+other.real, imaginary+other.imaginary);
	}

	ComplexNumber ComplexNumber::operator-(ComplexNumber other)
	{
		return ComplexNumber(real-other.real, imaginary-other.imaginary);
	}

	ComplexNumber ComplexNumber::operator-()
	{
		return ComplexNumber(-real, -imaginary);
	}

	ComplexNumber ComplexNumber::operator*(ComplexNumber other)
	{
		//(a+bi)*(c+di)
		//(a*c + a*di + bi*c + bi*di)
		//real = a*c - b*d
		//imag = a*d + b*c

		ComplexNumber k = ComplexNumber();
		k.real = (real*other.real) - (imaginary*other.imaginary);
		k.imaginary = (real*other.imaginary) + (imaginary*other.real);

		return k;
	}

	ComplexNumber ComplexNumber::operator*(double other)
	{
		return ComplexNumber(real*other,imaginary*other);
	}

	ComplexNumber ComplexNumber::operator/(double other)
	{
		return ComplexNumber(real/other,imaginary/other);
	}

	void ComplexNumber::operator+=(ComplexNumber other)
	{
		real += other.real;
		imaginary += other.imaginary;
	}

	void ComplexNumber::operator-=(ComplexNumber other)
	{
		real -= other.real;
		imaginary -= other.imaginary;
	}

	void ComplexNumber::operator*=(ComplexNumber other)
	{
		double newReal = (real*other.real) - (imaginary*other.imaginary);
		double newImag = (real*other.imaginary) + (imaginary*other.real);

		real = newReal;
		imaginary = newImag;
	}

	void ComplexNumber::operator*=(double other)
	{
		real*=other;
		imaginary*=other;
	}

	void ComplexNumber::operator/=(double other)
	{
		real/=other;
		imaginary/=other;
	}

	bool ComplexNumber::operator==(ComplexNumber other)
	{
		return (real==other.real && imaginary==other.imaginary);
	}

	bool ComplexNumber::operator!=(ComplexNumber other)
	{
		return (real!=other.real || imaginary!=other.imaginary);
	}

} //NAMESPACE glib END