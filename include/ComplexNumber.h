#pragma once
#include "Object.h"
#include "Vec2f.h"

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
	ComplexNumber operator-();
	ComplexNumber operator*(ComplexNumber other);

	ComplexNumber operator*(Vec2f other);
	ComplexNumber operator*(int other);
	ComplexNumber operator*(float other);
	ComplexNumber operator*(double other);

	void operator+=(ComplexNumber other);
	void operator-=(ComplexNumber other);
	void operator*=(ComplexNumber other);

	void operator*=(Vec2f other);
	void operator*=(int other);
	void operator*=(float other);
	void operator*=(double other);

	double real = 0;
	double imaginary = 0;
private:
};

