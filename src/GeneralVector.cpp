#include "GeneralVector.h"
#include <math.h>

namespace glib
{

	const Class GeneralVector::myClass = Class("GeneralVector", {&Object::myClass});
	const Class* GeneralVector::getClass()
	{
		return &GeneralVector::myClass;
	}

	GeneralVector::GeneralVector(int size)
	{
		this->size = size;
		if (size > 0)
		{
			values = new double[size];
			for(int i=0; i<size; i++)
			{
				values[i] = 0;
			}
		}
	}

	GeneralVector::GeneralVector(const GeneralVector& o)
	{
		copy(o);
	}

	void GeneralVector::operator=(const GeneralVector& o)
	{
		copy(o);
	}

	void GeneralVector::copy(const GeneralVector& o)
	{
		this->~GeneralVector();

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
		{
			delete[] values;
			values = nullptr;
		}
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

	double GeneralVector::getLength()
	{
		double sum = 0;
		for(int i=0; i<size; i++)
		{
			sum += values[i]*values[i];
		}

		return sqrt(sum);
	}

	GeneralVector GeneralVector::normalize()
	{
		double l = getLength();
		return operator/(l);
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

	GeneralVector GeneralVector::operator/(double value)
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

	GeneralVector GeneralVector::operator-()
	{
		GeneralVector k = GeneralVector(size);
		for (int i = 0; i < size; i++)
		{
			k.values[i] = -values[i];
		}
		return k;
	}

	void GeneralVector::operator*=(double value)
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

	bool GeneralVector::operator!=(GeneralVector other)
	{
		if(other.getSize() == size)
		{
			bool notSame = false;
			for(int i=0; i<size; i++)
			{
				if(other.values[i] != values[i])
				{
					notSame = true;
					break;
				}
			}
			return notSame;
		}
		return true;
	}

	bool GeneralVector::operator==(GeneralVector other)
	{
		if(other.getSize() == size)
		{
			bool same = true;
			for(int i=0; i<size; i++)
			{
				if(other.values[i] != values[i])
				{
					same = false;
					break;
				}
			}
			return same;
		}
		return false;
	}


	double& GeneralVector::operator[](int index)
	{
		return values[index];
	}

	int GeneralVector::getSize()
	{
		return size;
	}

} //NAMESPACE glib END