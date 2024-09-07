#include "GeneralVector.h"
#include <math.h>

namespace smpl
{

	const RootClass GeneralVector::globalClass = CREATE_ROOT_CLASS(GeneralVector, &Object::globalClass);
	const RootClass* GeneralVector::getClass()
	{
		return &GeneralVector::globalClass;
	}

	GeneralVector::GeneralVector(int size)
	{
		this->size = size;
		if (size > 0)
		{
			values = new float[size];
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


	GeneralVector::GeneralVector(GeneralVector&& o) noexcept
	{
		this->~GeneralVector();
		this->size = o.size;
		this->values = o.values;
		o.values = nullptr;
		o.size = 0;
	}

	void GeneralVector::operator=(GeneralVector&& o) noexcept
	{
		this->~GeneralVector();
		this->size = o.size;
		this->values = o.values;
		o.values = nullptr;
		o.size = 0;
	}

	void GeneralVector::copy(const GeneralVector& o)
	{
		this->~GeneralVector();
		
		this->size = o.size;
		if(size>0)
		{
			values = new float[size];
			for (int i = 0; i < size; i++)
			{
				values[i] = o.values[i];
			}
		}
	}

	GeneralVector::~GeneralVector()
	{
		if (values != nullptr)
		{
			delete[] values;
		}
		values = nullptr;
		size = 0;
	}

	float GeneralVector::getValue(int location)
	{
		if (location >= 0 && location < size)
		{
			return values[location];
		}
		return 0.0;
	}

	void GeneralVector::setValue(float value, int location)
	{
		if (location >= 0 && location < size)
		{
			values[location] = value;
		}
	}

	float GeneralVector::getLength()
	{
		float sum = 0;
		for(int i=0; i<size; i++)
		{
			sum += values[i]*values[i];
		}

		return sqrt(sum);
	}

	GeneralVector GeneralVector::normalize()
	{
		float l = getLength();
		return operator/(l);
	}

	GeneralVector GeneralVector::operator*(float value)
	{
		GeneralVector k = GeneralVector(size);
		for (int i = 0; i < size; i++)
		{
			k.values[i] = values[i]*value;
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

	void GeneralVector::operator*=(float value)
	{
		for (int i = 0; i < size; i++)
		{
			values[i] *= value;
		}
	}

	void GeneralVector::operator/=(float value)
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


	float& GeneralVector::operator[](int index)
	{
		return values[index];
	}

	int GeneralVector::getSize()
	{
		return size;
	}

} //NAMESPACE glib END