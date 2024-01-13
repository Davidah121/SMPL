#include "BinarySet.h"
#include <iostream>

namespace glib
{

	const RootClass BinarySet::globalClass = RootClass("BinarySet", {"Object"});

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

	unsigned char reverseBits(unsigned char b)
	{
		unsigned char ret = (b * 0x0202020202ULL & 0x010884422010ULL) % 1023;
		return ret;
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
					// for (size_t i = 0; i < totalSize; i++)
					// {
					// 	value += (int)getBit(indexStart + i) << (totalSize - i - 1);
					// }
					// return value;
					size_t sByteLocation = indexStart / 8;
					size_t sBitLocation = indexStart % 8;
					
					size_t eByteLocation = indexEnd / 8;
					size_t eBitLocation = indexEnd % 8;

					if(MSB == BinarySet::LMSB)
					{
						for (size_t i = indexStart; i < indexEnd; i++)
						{
							size_t byteLocation = i / 8;
							size_t bitLocation = i % 8;
							value = (value<<1) | ((set[byteLocation] >> bitLocation) & 0x01);
						}
						return value;
						
						// //case 2
						// unsigned char revB1 = set[sByteLocation];
						// value = revB1 & ((1<<(8-sBitLocation))-1);

						// int startI = sByteLocation+1;
						// int endI = eByteLocation-1;
						// if(endI < startI)
						// 	return value;
						
						// for(int i=startI; i<endI; i++)
						// {
						// 	value = (value << 8) | set[i];
						// }
						
						// unsigned char revB2 = set[eByteLocation];
						// value = (value<<eBitLocation) | ((revB2>>(8-eBitLocation)) & ((1<<eBitLocation)-1));
					}
					else
					{
						for (size_t i = indexStart; i < indexEnd; i++)
						{
							size_t byteLocation = i / 8;
							size_t bitLocation = i % 8;
							value = (value<<1) | ((set[byteLocation] >> (7-bitLocation)) & 0x01);
						}
						return value;

					// 	//case 4
					// 	unsigned char revB1 = reverseBits(set[sByteLocation]);
					// 	value = revB1 & ((1<<(8-sBitLocation))-1);

					// 	int startI = sByteLocation+1;
					// 	int endI = eByteLocation-1;
					// 	if(endI < startI)
					// 		return value;
						
					// 	for(int i=startI; i<endI; i++)
					// 	{
					// 		value = (value << 8) | reverseBits(set[i]);
					// 	}
						
					// 	unsigned char revB2 = reverseBits(set[eByteLocation]);
					// 	value = (value<<eBitLocation) | ((revB2>>(8-eBitLocation)) & ((1<<eBitLocation)-1));
					}
					
				}
				else 
				{
					for (size_t i = 0; i < totalSize; i++)
					{
						value += (int)getBit(indexStart + i) << i;
					}
					return value;
					
					// int firstBitsToGrab = 8-(indexStart % 8);
					// int firstByteLoc = indexStart / 8;
					// int lastBitsToGrab = (indexEnd % 8);
					// int lastByteLoc = indexEnd / 8;
					
					// int firstShiftAmount = (indexStart % 8);
					// int firstBitAndAmount = ((1<<firstBitsToGrab)-1);
					// int lastBitAndAmount = ((1<<lastBitsToGrab)-1);

					// if(MSB == BinarySet::LMSB)
					// {
					// 	//case 1
						
					// 	//grab the bits from the first byte. May not be 8
					// 	value = set[lastByteLoc] & lastBitAndAmount;
						
					// 	size_t startI = lastByteLoc-1;
					// 	size_t endI = firstByteLoc;
					// 	if(endI > startI)
					// 		return value;
						
					// 	//grab the bytes between first and last byte. Always 8 bits
					// 	for (size_t i=startI; i>endI; i--)
					// 	{
					// 		value = (value << 8) | set[i];
					// 	}
						
					// 	//grab the last bits. May not be 8
					// 	value = (value << firstBitsToGrab) | ((set[firstByteLoc]>>firstShiftAmount) & firstBitAndAmount);
					// }
					// else
					// {
					// 	//case 3

					// 	//grab the bits from the first byte. May not be 8
					// 	value = reverseBits(set[lastByteLoc]) & lastBitAndAmount;
						
					// 	size_t startI = lastByteLoc-1;
					// 	size_t endI = firstByteLoc;
					// 	if(endI > startI)
					// 		return value;
						
					// 	//grab the bytes between first and last byte. Always 8 bits
					// 	for (size_t i=startI; i>endI; i--)
					// 	{
					// 		value = (value << 8) | reverseBits(set[i]);
					// 	}
						
					// 	//grab the last bits. May not be 8
					// 	value = (value << firstBitsToGrab) | ((reverseBits(set[firstByteLoc])>>firstShiftAmount) & firstBitAndAmount);
					// }
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
			unsigned char v = 1 << bitLocation;
			byte |= v;
		}
		else
		{
			unsigned char nByte = ~byte;
			unsigned char v = 1 << bitLocation;
			nByte |= v;
			byte = ~nByte;
		}

		this->set[byteLocation] = byte;
	}

	void BinarySet::setNumberOfBits(size_t n)
	{
		if(n > set.size()*8)
			bitNumber = set.size()*8;
		else
			bitNumber = n;
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