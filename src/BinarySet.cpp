#include "BinarySet.h"
#include "BitwiseTools.h"

namespace smpl
{
	
	BinarySet::BinarySet()
	{
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

	void BinarySet::add(char* v, size_t size, bool addBitOrder)
	{
		if(v != nullptr)
		{
			for (size_t i = 0; i < size; i++)
			{
				this->add(v[i], 8, 0, addBitOrder);
			}
		}
	}

	void BinarySet::add(unsigned char* v, size_t size, bool addBitOrder)
	{
		if(v != nullptr)
		{
			for (size_t i = 0; i < size; i++)
			{
				this->add(v[i], 8, 0, addBitOrder);
			}
		}
	}

	void BinarySet::add(BinarySet& other, bool addBitOrder)
	{
		for(size_t i=0; i<other.size(); i++)
		{
			add( other.getBit(i), 1, 0, addBitOrder);
		}
	}

	void BinarySet::setValues(char* v, size_t size, bool addBitOrder)
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
		if(addBitOrder == RMSB)
		{
			for(unsigned char& c : set)
			{
				c = BitwiseTools::bitSwap(c);
			}
		}
	}

	void BinarySet::setValues(unsigned char* v, size_t size, bool addBitOrder)
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
		if(addBitOrder == RMSB)
		{
			for(unsigned char& c : set)
			{
				c = BitwiseTools::bitSwap(c);
			}
		}
	}

	bool BinarySet::getBit(size_t index)
	{
		size_t byteLocation = index / 8;
		size_t bitLocation = index % 8;
		if (index < bitNumber)
			return ((this->set[byteLocation] >> bitLocation) & 0x01);
		
		return false; //throw exception
	}

	unsigned char reverseBits(unsigned char b)
	{
		unsigned char ret = (b * 0x0202020202ULL & 0x010884422010ULL) % 1023;
		return ret;
	}

	uint64_t BinarySet::getBitsReversed(size_t indexStart, size_t indexEnd, bool byteOrder)
	{
		//lazy way
		uint64_t result = 0;
		size_t totalBits = indexEnd-indexStart;

		for(size_t i=0; i<totalBits; i++)
		{
			result = result << 1 | getBit(indexStart+i);
		}
		if(byteOrder == BigEndian)
		{
			size_t bytesRequired = totalBits/8;
			if(totalBits % 8 != 0)
				bytesRequired++;

			char* rawBytes = (char*)&result;
			for(size_t i=0; i<bytesRequired/2; i++)
			{
				rawBytes[i] = rawBytes[bytesRequired-1-i];
			}
		}
		return result;
	}
	uint64_t BinarySet::getBits(size_t indexStart, size_t indexEnd, bool byteOrder)
	{
		//lazy way
		uint64_t result = 0;
		size_t totalBits = indexEnd-indexStart;

		for(size_t i=0; i<totalBits; i++)
		{
			result |= getBit(indexStart+i)<<i;
		}

		if(byteOrder == BigEndian)
		{
			size_t bytesRequired = totalBits/8;
			if(totalBits % 8 != 0)
				bytesRequired++;

			char* rawBytes = (char*)&result;
			for(size_t i=0; i<bytesRequired/2; i++)
			{
				rawBytes[i] = rawBytes[bytesRequired-1-i];
			}
		}
		return result;
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

} //NAMESPACE glib END