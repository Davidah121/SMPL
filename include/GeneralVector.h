#pragma once
#include "Object.h"

class GeneralVector : public Object
{
public:
	GeneralVector(int size=0);
	GeneralVector(const GeneralVector& o);
	~GeneralVector();

	//Object and Class Stuff
	const Class* getClass();
	static const Class* myClass;

	double getValue(int location);
	void setValue(double value, int location);

	GeneralVector operator*(double value);
	GeneralVector operator*(float value);
	GeneralVector operator*(int value);

	GeneralVector operator/(double value);
	GeneralVector operator/(float value);
	GeneralVector operator/(int value);

	GeneralVector operator+(GeneralVector o);
	GeneralVector operator-(GeneralVector o);

	void operator*=(double value);
	void operator*=(float value);
	void operator*=(int value);

	void operator/=(double value);
	void operator/=(float value);
	void operator/=(int value);

	void operator+=(GeneralVector o);
	void operator-=(GeneralVector o);

	double& operator[](int index);

	int getSize();
private:

	int size = 0;
	double* values = nullptr;
};

