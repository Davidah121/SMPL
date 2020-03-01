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
	this->set.push_back(v);
}

void BinarySet::add(char v, int amountOfBits, int offset, bool reverse)
{
	if (amountOfBits >= 8)
	{
		for (int i = 0; i < 8; i++)
		{
			if (reverse)
			{
				this->set.push_back((v >> (i+offset) ) & 1);
			}
			else
			{
				this->set.push_back((v >> (7 - (i+offset) )) & 1);
			}
		}
	}
	else
	{
		for (int i = 0; i < amountOfBits; i++)
		{
			if (reverse)
			{
				this->set.push_back((v >> (i + offset)) & 1);
			}
			else
			{
				this->set.push_back((v >> (7 - (i + offset))) & 1);
			}
		}
	}
}

void BinarySet::add(unsigned char v, int amountOfBits, int offset, bool reverse)
{
	if (amountOfBits >= 8)
	{
		for (int i = 0; i < 8; i++)
		{
			if (reverse)
			{
				this->set.push_back((v >> (i + offset)) & 1);
			}
			else
			{
				this->set.push_back((v >> (7 - (i + offset))) & 1);
			}
		}
	}
	else
	{
		for (int i = 0; i < amountOfBits; i++)
		{
			if (reverse)
			{
				this->set.push_back((v >> (i + offset)) & 1);
			}
			else
			{
				this->set.push_back((v >> (7 - (i + offset))) & 1);
			}
		}
	}
}

void BinarySet::add(short v, int amountOfBits, int offset, bool reverse)
{
	if (amountOfBits >= 16)
	{
		for (int i = 0; i < 16; i++)
		{
			if (reverse)
			{
				this->set.push_back((v >> (i + offset)) & 1);
			}
			else
			{
				this->set.push_back((v >> (15 - (i + offset))) & 1);
			}
		}
	}
	else
	{
		for (int i = 0; i < amountOfBits; i++)
		{
			if (reverse)
			{
				this->set.push_back((v >> (i + offset)) & 1);
			}
			else
			{
				this->set.push_back((v >> (15 - (i + offset))) & 1);
			}
		}
	}
}

void BinarySet::add(unsigned short v, int amountOfBits, int offset, bool reverse)
{
	if (amountOfBits >= 16)
	{
		for (int i = 0; i < 16; i++)
		{
			if (reverse)
			{
				this->set.push_back((v >> (i + offset)) & 1);
			}
			else
			{
				this->set.push_back((v >> (15 - (i + offset))) & 1);
			}
		}
	}
	else
	{
		for (int i = 0; i < amountOfBits; i++)
		{
			if (reverse)
			{
				this->set.push_back((v >> (i + offset)) & 1);
			}
			else
			{
				this->set.push_back((v >> (15 - (i + offset))) & 1);
			}
		}
	}
}

void BinarySet::add(int v, int amountOfBits, int offset, bool reverse)
{
	if (amountOfBits >= 32)
	{
		for (int i = 0; i < 32; i++)
		{
			if (reverse)
			{
				this->set.push_back((v >> (i + offset)) & 1);
			}
			else
			{
				this->set.push_back((v >> (31 - (i + offset))) & 1);
			}
		}
	}
	else
	{
		for (int i = 0; i < amountOfBits; i++)
		{
			if (reverse)
			{
				this->set.push_back((v >> (i + offset)) & 1);
			}
			else
			{
				this->set.push_back((v >> (31 - (i + offset))) & 1);
			}
		}
	}
}

void BinarySet::add(unsigned int v, int amountOfBits, int offset, bool reverse)
{
	if (amountOfBits >= 32)
	{
		for (int i = 0; i < 32; i++)
		{
			if (reverse)
			{
				this->set.push_back((v >> (i + offset)) & 1);
			}
			else
			{
				this->set.push_back((v >> (31 - (i + offset))) & 1);
			}
		}
	}
	else
	{
		for (int i = 0; i < amountOfBits; i++)
		{
			if (reverse)
			{
				this->set.push_back((v >> (i + offset)) & 1);
			}
			else
			{
				this->set.push_back((v >> (31 - (i + offset))) & 1);
			}
		}
	}
}

void BinarySet::add(long v, int amountOfBits, int offset, bool reverse)
{
	if (amountOfBits >= 64)
	{
		for (int i = 0; i < 64; i++)
		{
			if (reverse)
			{
				this->set.push_back((v >> (i + offset)) & 1);
			}
			else
			{
				this->set.push_back((v >> (63 - (i + offset))) & 1);
			}
		}
	}
	else
	{
		for (int i = 0; i < amountOfBits; i++)
		{
			if (reverse)
			{
				this->set.push_back((v >> (i + offset)) & 1);
			}
			else
			{
				this->set.push_back((v >> (63 - (i + offset))) & 1);
			}
		}
	}
}

void BinarySet::add(unsigned long v, int amountOfBits, int offset, bool reverse)
{
	if (amountOfBits >= 64)
	{
		for (int i = 0; i < 64; i++)
		{
			if (reverse)
			{
				this->set.push_back((v >> (i + offset)) & 1);
			}
			else
			{
				this->set.push_back((v >> (63 - (i + offset))) & 1);
			}
		}
	}
	else
	{
		for (int i = 0; i < amountOfBits; i++)
		{
			if (reverse)
			{
				this->set.push_back((v >> (i + offset)) & 1);
			}
			else
			{
				this->set.push_back((v >> (63 - (i + offset))) & 1);
			}
		}
	}
}

bool BinarySet::getBit(int index)
{
	if (index < set.size())
	{
		return this->set[set.size() - index - 1];
	}
	return false;
}

int BinarySet::getBits(int indexStart, int indexEnd, bool reverse)
{
	int value = 0;
	if(indexStart > 0 && indexStart < set.size())
	{
		if(indexEnd > 0 && indexEnd < set.size())
		{
			int totalSize = indexEnd-indexStart;

			if(reverse==false)
			{
				for(int i=0; i<totalSize; i++)
				{
					value += (int)getBit(indexStart+i) << i;
				}
			}
			else
			{
				for(int i=0; i<totalSize; i++)
				{
					value += (int)getBit(indexEnd-i) << i;
				}
			}
			
		}
	}

	return value;
}

std::vector<unsigned char> BinarySet::toBytes(bool reverse)
{
	bool extraByte = ((set.size() % 8) != 0);
	int amountOfBytes = (set.size() + (set.size()%8)) / 8; //Rounds up to the next byte

	std::vector<unsigned char> output = std::vector<unsigned char>();

	unsigned char curByte = 0;
	int shift = 0;

	if (reverse == false)
	{
		for (int i = 0; i < set.size(); i++)
		{
			curByte += ((int)set[i]) << shift;

			shift++;
			if (shift >= 8)
			{
				output.push_back(curByte);
				shift = 0;
				curByte = 0;
			}
		}
	}
	else
	{
		int shift = 7;
		for (int i = 0; i < set.size(); i++)
		{
			curByte += ((int)set[i]) << shift;

			shift--;
			if (shift <0)
			{
				output.push_back(curByte);
				shift = 7;
				curByte = 0;
			}
		}
	}

	if (extraByte)
	{
		output.push_back(curByte);
	}

	return output;
}

int BinarySet::size()
{
	return set.size();
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
