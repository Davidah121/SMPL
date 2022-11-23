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
		int byteLocation = bitNumber / 8;
		int bitLocation = bitNumber % 8;
		
		if (byteLocation >= this->set.size())
		{
			this->set.push_back(0);
		}

		this->set[byteLocation] += (v << (bitLocation));

		bitNumber++;
	}

	void BinarySet::add(char* v, int size)
	{
		if(v != nullptr)
		{
			for (int i = 0; i < size; i++)
			{
				this->add(v[i]);
			}
		}
	}

	void BinarySet::add(unsigned char* v, int size)
	{
		if(v != nullptr)
		{
			for (int i = 0; i < size; i++)
			{
				this->add(v[i]);
			}
		}
	}

	void BinarySet::add(BinarySet& other)
	{
		for(int i=0; i<other.size(); i++)
		{
			add( other.getBit(i) );
		}
	}

	void BinarySet::setValues(char* v, int size)
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

	void BinarySet::setValues(unsigned char* v, int size)
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

	bool BinarySet::getBit(int index)
	{
		int byteLocation = index / 8;
		int bitLocation = index % 8;

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

	int BinarySet::getBits(int indexStart, int indexEnd, bool lmsb)
	{
		int value = 0;
		if(indexStart >= 0 && indexStart <= bitNumber)
		{
			if(indexEnd >= 0 && indexEnd <= bitNumber)
			{
				int totalSize = indexEnd-indexStart;
				if(lmsb)
				{
					for (int i = 0; i < totalSize; i++)
					{
						value += (int)getBit(indexStart + i) << (totalSize - i - 1);
					}
				}
				else 
				{
					for (int i = 0; i < totalSize; i++)
					{
						value += (int)getBit(indexStart + i) << i;
					}
				}
			}
		}

		return value;
	}

	void BinarySet::setBit(bool value, int index)
	{
		if(index >= bitNumber)
		{
			return;
		}

		int byteLocation = index / 8;
		int bitLocation = index % 8;

		unsigned char byte = 0;
		for(int i=0; i<8; i++)
		{
			if(i!=bitLocation)
			{
				byte += this->set[byteLocation] & ( 1 << i);
			}
			else
			{
				byte += ( value << i);
			}
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

	int BinarySet::size()
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