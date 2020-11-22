#include "GeneralVector.h"

const Class* GeneralVector::myClass = new Class("GeneralVector", {Object::myClass});
const Class* GeneralVector::getClass()
{
	return GeneralVector::myClass;
}

GeneralVector::GeneralVector(int size)
{
	this->size = size;
	if (size > 0)
		values = new double[size];
}

GeneralVector::GeneralVector(const GeneralVector& o)
{
	this->size = o.size;
	if(size>0)
	{
		values = new double[size];
		for (int i = 0; i < size; i++)
		{
			values[i] = o.values[i];
		}
	}
}

GeneralVector::~GeneralVector()
{
	if (size > 0)
		delete[] values;
}

double GeneralVector::getValue(int location)
{
	if (location >= 0 && location < size)
	{
		return values[location];
	}
	return 0.0;
}

void GeneralVector::setValue(double value, int location)
{
	if (location >= 0 && location < size)
	{
		values[location] = value;
	}
}

GeneralVector GeneralVector::operator*(double value)
{
	GeneralVector k = GeneralVector(size);
	for (int i = 0; i < size; i++)
	{
		k.values[i] = values[i]*value;
	}
	return k;
}

GeneralVector GeneralVector::operator*(float value)
{
	GeneralVector k = GeneralVector(size);
	for (int i = 0; i < size; i++)
	{
		k.values[i] = values[i] * value;
	}
	return k;
}

GeneralVector GeneralVector::operator*(int value)
{
	GeneralVector k = GeneralVector(size);
	for (int i = 0; i < size; i++)
	{
		k.values[i] = values[i] * value;
	}
	return k;
}

GeneralVector GeneralVector::operator/(double value)
{
	GeneralVector k = GeneralVector(size);
	for (int i = 0; i < size; i++)
	{
		k.values[i] = values[i] / value;
	}
	return k;
}

GeneralVector GeneralVector::operator/(float value)
{
	GeneralVector k = GeneralVector(size);
	for (int i = 0; i < size; i++)
	{
		k.values[i] = values[i] / value;
	}
	return k;
}

GeneralVector GeneralVector::operator/(int value)
{
	GeneralVector k = GeneralVector(size);
	for (int i = 0; i < size; i++)
	{
		k.values[i] = values[i] / value;
	}
	return k;
}

GeneralVector GeneralVector::operator+(GeneralVector o)
{
	if (size == o.size)
	{
		GeneralVector k = GeneralVector(size);
		for (int i = 0; i < size; i++)
		{
			k.values[i] = values[i] + o.values[i];
		}
		return k;
	}
	
	return GeneralVector();
}

GeneralVector GeneralVector::operator-(GeneralVector o)
{
	if (size == o.size)
	{
		GeneralVector k = GeneralVector(size);
		for (int i = 0; i < size; i++)
		{
			k.values[i] = values[i] - o.values[i];
		}
		return k;
	}

	return GeneralVector();
}

void GeneralVector::operator*=(double value)
{
	for (int i = 0; i < size; i++)
	{
		values[i] *= value;
	}
}

void GeneralVector::operator*=(float value)
{
	for (int i = 0; i < size; i++)
	{
		values[i] *= value;
	}
}

void GeneralVector::operator*=(int value)
{
	for (int i = 0; i < size; i++)
	{
		values[i] *= value;
	}
}

void GeneralVector::operator/=(double value)
{
	for (int i = 0; i < size; i++)
	{
		values[i] /= value;
	}
}

void GeneralVector::operator/=(float value)
{
	for (int i = 0; i < size; i++)
	{
		values[i] /= value;
	}
}

void GeneralVector::operator/=(int value)
{
	for (int i = 0; i < size; i++)
	{
		values[i] /= value;
	}
}

void GeneralVector::operator+=(GeneralVector o)
{
	if (size == o.size)
	{
		for (int i = 0; i < size; i++)
		{
			values[i] += o.values[i];
		}
	}
}

void GeneralVector::operator-=(GeneralVector o)
{
	if (size == o.size)
	{
		for (int i = 0; i < size; i++)
		{
			values[i] -= o.values[i];
		}
	}
}

double& GeneralVector::operator[](int index)
{
	return values[index];
}

int GeneralVector::getSize()
{
	return size;
}
