#include "BinarySet.h"
#include <iostream>

namespace glib
{

	const Class BinarySet::globalClass = Class("BinarySet", {&Object::globalClass});

	BinarySet::BinarySet()
	{
		setClass(globalClass);
	}

	BinarySet::~BinarySet()
	{
	}

	void BinarySet::add(bool v)
	{
		size_t byteLocation = bitNumber / 8;
		size_t bitLocation = bitNumber % 8;
		
		if (byteLocation >= this->set.size())
		{
			this->set.push_back(0);
		}

		this->set[byteLocation] += (v << (bitLocation));

		bitNumber++;
	}

	void BinarySet::add(char* v, size_t size)
	{
		if(v != nullptr)
		{
			for (size_t i = 0; i < size; i++)
			{
				this->add(v[i]);
			}
		}
	}

	void BinarySet::add(unsigned char* v, size_t size)
	{
		if(v != nullptr)
		{
			for (size_t i = 0; i < size; i++)
			{
				this->add(v[i]);
			}
		}
	}

	void BinarySet::add(BinarySet& other)
	{
		for(size_t i=0; i<other.size(); i++)
		{
			add( other.getBit(i) );
		}
	}

	void BinarySet::setValues(char* v, size_t size)
	{
		if(v != nullptr)
		{
			set.assign((unsigned char*)v, (unsigned char*)v + size);
			bitNumber = size * 8;
		}
		else
		{
			set.clear();
			bitNumber = 0;
		}
	}

	void BinarySet::setValues(unsigned char* v, size_t size)
	{
		if(v != nullptr)
		{
			set.assign(v, v + size);
			bitNumber = size * 8;
		}
		else
		{
			set.clear();
			bitNumber = 0;
		}
	}

	bool BinarySet::getBit(size_t index)
	{
		size_t byteLocation = index / 8;
		size_t bitLocation = index % 8;

		if (MSB == BinarySet::LMSB)
		{
			if (index < bitNumber)
			{
				return ((this->set[byteLocation] >> bitLocation) & 0x01)==1;
			}
		}
		else
		{
			if (index < bitNumber)
			{
				return ((this->set[byteLocation] >> (7-bitLocation)) & 0x01)==1;
			}
		}
		
		return false;
	}

	int BinarySet::getBits(size_t indexStart, size_t indexEnd, bool lmsb)
	{
		int value = 0;
		if(indexStart >= 0 && indexStart <= bitNumber)
		{
			if(indexEnd >= 0 && indexEnd <= bitNumber)
			{
				size_t totalSize = indexEnd-indexStart;
				if(lmsb)
				{
					for (size_t i = 0; i < totalSize; i++)
					{
						value += (int)getBit(indexStart + i) << (totalSize - i - 1);
					}
				}
				else 
				{
					for (size_t i = 0; i < totalSize; i++)
					{
						value += (int)getBit(indexStart + i) << i;
					}
				}
			}
		}

		return value;
	}

	void BinarySet::setBit(bool value, size_t index)
	{
		if(index >= bitNumber)
		{
			return;
		}

		size_t byteLocation = index / 8;
		size_t bitLocation = index % 8;

		unsigned char byte = this->set[byteLocation];

		if(value == true)
		{
			unsigned char v = value << bitLocation;
			byte |= v;
		}
		else
		{
			unsigned char nByte = ~byte;
			unsigned char v = value << bitLocation;
			nByte |= v;
			byte = ~nByte;
		}

		this->set[byteLocation] = byte;
	}

	std::vector<unsigned char> BinarySet::toBytes()
	{
		return set;
	}
	
	std::vector<unsigned char>& BinarySet::getByteRef()
	{
		return set;
	}

	size_t BinarySet::size()
	{
		return bitNumber;
	}

	void BinarySet::clear()
	{
		set.clear();
		bitNumber = 0;
	}

	void BinarySet::setBitOrder(bool bitOrder)
	{
		MSB = bitOrder;
	}

	void BinarySet::setAddBitOrder(bool bitOrder)
	{
		addBitOrder = bitOrder;
	}

} //NAMESPACE glib END