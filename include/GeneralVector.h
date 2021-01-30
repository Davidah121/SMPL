#pragma once
#include "Object.h"

class GeneralVector : public Object
{
public:
	GeneralVector(int size=0);
	GeneralVector(const GeneralVector& o);
	void operator=(const GeneralVector& o);
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

	bool operator==(GeneralVector other);
	bool operator!=(GeneralVector other);

	int getSize();
private:
	
	void copy(const GeneralVector& o);
	unsigned char size = 0;
	double* values = nullptr;
};

