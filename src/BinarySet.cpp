#include "BinarySet.h"
#include <iostream>

const Class* BinarySet::myClass = new Class("BinarySet", {Object::myClass});
const Class* BinarySet::getClass()
{
	return BinarySet::myClass;
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

	if (this->set.size() <= 0)
	{
		this->set.push_back(0);
	}

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

	if (offset + actualAmount < 8)
	{
		for (int i = offset; i < offset + actualAmount; i++)
		{
			this->add(((v >> i) & 0x01) == 1);
		}
	}
	else
	{
		for (int i = offset; i < 8; i++)
		{
			this->add(((v >> i) & 0x01) == 1);
		}
	}
}

void BinarySet::add(unsigned char v, int amountOfBits, int offset)
{
	int actualAmount = amountOfBits;
	if (amountOfBits > 8)
	{
		actualAmount = 8;
	}

	if (offset + actualAmount < 8)
	{
		for (int i = offset; i < offset + actualAmount; i++)
		{
			this->add(((v >> i) & 0x01) == 1);
		}
	}
	else
	{
		for (int i = offset; i < 8; i++)
		{
			this->add(((v >> i) & 0x01) == 1);
		}
	}
}

void BinarySet::add(short v, int amountOfBits, int offset)
{
	int actualAmount = amountOfBits;
	if (amountOfBits > 16)
	{
		actualAmount = 16;
	}

	if (offset + actualAmount < 16)
	{
		for (int i = offset; i < offset + actualAmount; i++)
		{
			this->add(((v >> i) & 0x01) == 1);
		}
	}
	else
	{
		for (int i = offset; i < 16; i++)
		{
			this->add(((v >> i) & 0x01) == 1);
		}
	}
}

void BinarySet::add(unsigned short v, int amountOfBits, int offset)
{
	int actualAmount = amountOfBits;
	if (amountOfBits > 16)
	{
		actualAmount = 16;
	}

	if (offset + actualAmount < 16)
	{
		for (int i = offset; i < offset + actualAmount; i++)
		{
			this->add(((v >> i) & 0x01) == 1);
		}
	}
	else
	{
		for (int i = offset; i < 16; i++)
		{
			this->add(((v >> i) & 0x01) == 1);
		}
	}
}

void BinarySet::add(int v, int amountOfBits, int offset)
{
	int actualAmount = amountOfBits;
	if (amountOfBits > 32)
	{
		actualAmount = 32;
	}

	if (offset + actualAmount < 32)
	{
		for (int i = offset; i < offset + actualAmount; i++)
		{
			this->add(((v >> i) & 0x01) == 1);
		}
	}
	else
	{
		for (int i = offset; i < 32; i++)
		{
			this->add(((v >> i) & 0x01) == 1);
		}
	}
}

void BinarySet::add(unsigned int v, int amountOfBits, int offset)
{
	int actualAmount = amountOfBits;
	if (amountOfBits > 32)
	{
		actualAmount = 32;
	}

	if (offset + actualAmount < 32)
	{
		for (int i = offset; i < offset + actualAmount; i++)
		{
			this->add(((v >> i) & 0x01) == 1);
		}
	}
	else
	{
		for (int i = offset; i < 32; i++)
		{
			this->add(((v >> i) & 0x01) == 1);
		}
	}
}

void BinarySet::add(long v, int amountOfBits, int offset)
{
	int actualAmount = amountOfBits;
	if (amountOfBits > 32)
	{
		actualAmount = 32;
	}

	if (offset + actualAmount < 32)
	{
		for (int i = offset; i < offset + actualAmount; i++)
		{
			this->add(((v >> i) & 0x01) == 1);
		}
	}
	else
	{
		for (int i = offset; i < 32; i++)
		{
			this->add(((v >> i) & 0x01) == 1);
		}
	}
}

void BinarySet::add(unsigned long v, int amountOfBits, int offset)
{
	int actualAmount = amountOfBits;
	if (amountOfBits > 32)
	{
		actualAmount = 32;
	}

	if (offset + actualAmount < 32)
	{
		for (int i = offset; i < offset + actualAmount; i++)
		{
			this->add(((v >> i) & 0x01) == 1);
		}
	}
	else
	{
		for (int i = offset; i < 32; i++)
		{
			this->add(((v >> i) & 0x01) == 1);
		}
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

std::vector<unsigned char> BinarySet::toBytes()
{
	return set;
}

int BinarySet::size()
{
	return bitNumber;
}

void BinarySet::printVals(bool reverse)
{
	if (reverse)
	{
		int sp = 0;
		for (int i = 0; i < set.size(); i++)
		{
			std::cout << (int)set[i];
			sp++;
			if (sp >= 8)
			{
				sp = 0;
				std::cout << " ";
			}
		}
	}
	else
	{
		int sp = 0;
		for (int i = set.size() - 1; i >= 0; i--)
		{
			std::cout << (int)set[i];
			sp++;
			if (sp >= 8)
			{
				sp = 0;
				std::cout << " ";
			}
		}
	}

	std::cout << std::endl;
}

void BinarySet::clear()
{
	set.clear();
}

void BinarySet::setBitOrder(bool bitOrder)
{
	MSB = bitOrder;
}
