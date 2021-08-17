#include "BinarySet.h"
#include <iostream>

namespace glib
{

	const Class BinarySet::myClass = Class("BinarySet", {&Object::myClass});
	const Class* BinarySet::getClass()
	{
		return &BinarySet::myClass;
	}


	BinarySet::BinarySet()
	{
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

	void BinarySet::add(char v, int amountOfBits, int offset)
	{
		int actualAmount = amountOfBits;
		if (amountOfBits > 8)
		{
			actualAmount = 8;
		}

		int end = 8;
		if (offset + actualAmount < 8)
		{
			end = offset+actualAmount;
		}

		for (int i = offset; i < end; i++)
		{
			int h = i;
			if(addBitOrder==RMSB)
			{
				h = (actualAmount-1) - h;
			}
			this->add(((v >> h) & 0x01) == 1);
		}
	}

	void BinarySet::add(unsigned char v, int amountOfBits, int offset)
	{
		int actualAmount = amountOfBits;
		if (amountOfBits > 8)
		{
			actualAmount = 8;
		}

		int end = 8;
		if (offset + actualAmount < 8)
		{
			end = offset+actualAmount;
		}

		for (int i = offset; i < end; i++)
		{
			int h = i;
			if(addBitOrder==RMSB)
			{
				h = (actualAmount-1) - h;
			}
			this->add(((v >> h) & 0x01) == 1);
		}
	}

	void BinarySet::add(short v, int amountOfBits, int offset)
	{
		int actualAmount = amountOfBits;
		if (amountOfBits > 16)
		{
			actualAmount = 16;
		}

		int end = 16;
		if (offset + actualAmount < 16)
		{
			end = offset+actualAmount;
		}

		for (int i = offset; i < end; i++)
		{
			int h = i;
			if(addBitOrder==RMSB)
			{
				h = (actualAmount-1) - h;
			}
			this->add(((v >> h) & 0x01) == 1);
		}
	}

	void BinarySet::add(unsigned short v, int amountOfBits, int offset)
	{
		int actualAmount = amountOfBits;
		if (amountOfBits > 16)
		{
			actualAmount = 16;
		}

		int end = 16;
		if (offset + actualAmount < 16)
		{
			end = offset+actualAmount;
		}

		for (int i = offset; i < end; i++)
		{
			int h = i;
			if(addBitOrder==RMSB)
			{
				h = (actualAmount-1) - h;
			}
			this->add(((v >> h) & 0x01) == 1);
		}
	}

	void BinarySet::add(int v, int amountOfBits, int offset)
	{
		int actualAmount = amountOfBits;
		if (amountOfBits > 32)
		{
			actualAmount = 32;
		}

		int end = 32;
		if (offset + actualAmount < 32)
		{
			end = offset+actualAmount;
		}

		for (int i = offset; i < end; i++)
		{
			int h = i;
			if(addBitOrder==RMSB)
			{
				h = (actualAmount-1) - h;
			}
			this->add(((v >> h) & 0x01) == 1);
		}
	}

	void BinarySet::add(unsigned int v, int amountOfBits, int offset)
	{
		int actualAmount = amountOfBits;
		if (amountOfBits > 32)
		{
			actualAmount = 32;
		}

		int end = 32;
		if (offset + actualAmount < 32)
		{
			end = offset+actualAmount;
		}

		for (int i = offset; i < end; i++)
		{
			int h = i;
			if(addBitOrder==RMSB)
			{
				h = (actualAmount-1) - h;
			}
			this->add(((v >> h) & 0x01) == 1);
		}
	}

	void BinarySet::add(long v, int amountOfBits, int offset)
	{
		int actualAmount = amountOfBits;
		if (amountOfBits > 64)
		{
			actualAmount = 64;
		}

		int end = 64;
		if (offset + actualAmount < 64)
		{
			end = offset+actualAmount;
		}

		for (int i = offset; i < end; i++)
		{
			int h = i;
			if(addBitOrder==RMSB)
			{
				h = (actualAmount-1) - h;
			}
			this->add(((v >> h) & 0x01) == 1);
		}
	}

	void BinarySet::add(unsigned long v, int amountOfBits, int offset)
	{
		int actualAmount = amountOfBits;
		if (amountOfBits > 64)
		{
			actualAmount = 64;
		}

		int end = 64;
		if (offset + actualAmount < 64)
		{
			end = offset+actualAmount;
		}

		for (int i = offset; i < end; i++)
		{
			int h = i;
			if(addBitOrder==RMSB)
			{
				h = (actualAmount-1) - h;
			}
			this->add(((v >> h) & 0x01) == 1);
		}
	}

	void BinarySet::add(char* v, int size)
	{
		for (int i = 0; i < size; i++)
		{
			this->add(v[i]);
		}
	}

	void BinarySet::add(unsigned char* v, int size)
	{
		for (int i = 0; i < size; i++)
		{
			this->add(v[i]);
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
		set.assign((unsigned char*)v, (unsigned char*)v + size);
		bitNumber = size * 8;
	}

	void BinarySet::setValues(unsigned char* v, int size)
	{
		set.assign(v, v + size);
		bitNumber = size * 8;
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

	int BinarySet::getBits(int indexStart, int indexEnd, bool reverse)
	{
		int value = 0;
		if(indexStart >= 0 && indexStart <= bitNumber)
		{
			if(indexEnd >= 0 && indexEnd <= bitNumber)
			{
				int totalSize = indexEnd-indexStart;
				if(!reverse)
				{
					for (int i = 0; i < totalSize; i++)
					{
						value += (int)getBit(indexStart + i) << i;
					}
				}
				else
				{
					for (int i = 0; i < totalSize; i++)
					{
						value += (int)getBit(indexStart + i) << (totalSize - i - 1);
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

	int BinarySet::size()
	{
		return bitNumber;
	}

	void BinarySet::clear()
	{
		set.clear();
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