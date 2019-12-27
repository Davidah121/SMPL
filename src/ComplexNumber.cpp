#include "ComplexNumber.h"

#pragma region ClassStuff
const Class* ComplexNumber::myClass = new Class("ComplexNumber", {Object::myClass});
const Class* ComplexNumber::getClass()
{
	return ComplexNumber::myClass;
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

ComplexNumber ComplexNumber::operator+(ComplexNumber other)
{
	ComplexNumber k = *this;
	k.real += other.real;
	k.imaginary += other.imaginary;

	return k;
}

ComplexNumber ComplexNumber::operator-(ComplexNumber other)
{
	ComplexNumber k = *this;
	k.real -= other.real;
	k.imaginary -= other.imaginary;

	return k;
}

ComplexNumber ComplexNumber::operator*(ComplexNumber other)
{
	//(a+bi)*(c+di)
	//(a*c + a*di + bi*c + bi*di)
	//real = a*c - b*d
	//imag = a*d + b*c

	ComplexNumber k = ComplexNumber();
	k.real = real * other.real - imaginary * other.imaginary;
	k.imaginary = real*other.imaginary + imaginary*other.real;

	return k;
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
	double newReal = real * other.real - imaginary * other.imaginary;
	double newImag = real * other.imaginary + imaginary * other.real;

	real = newReal;
	imaginary = newImag;
}
