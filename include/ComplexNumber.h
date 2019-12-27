#pragma once
#include "Object.h"

class ComplexNumber : public Object
{
public:
	ComplexNumber();
	ComplexNumber(double real, double imaginary);
	ComplexNumber(const ComplexNumber& other);
	~ComplexNumber();

	//Object and Class Stuff
	const Class* getClass();
	static const Class* myClass;
	
	ComplexNumber operator+(ComplexNumber other);
	ComplexNumber operator-(ComplexNumber other);
	ComplexNumber operator*(ComplexNumber other);

	void operator+=(ComplexNumber other);
	void operator-=(ComplexNumber other);
	void operator*=(ComplexNumber other);

	double real = 0;
	double imaginary = 0;
private:
};

