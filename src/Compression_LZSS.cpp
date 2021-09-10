#include "InternalCompressionHeader.h"

namespace glib
{

#pragma region LZSS

	std::vector<unsigned char> Compression::compressLZSS(std::vector<unsigned char> data)
	{
		return compressLZSS(data.data(), data.size());
	}

	std::vector<unsigned char> Compression::compressLZSS(unsigned char* data, int size)
	{
		BinarySet bin = BinarySet();
		bin.setBitOrder(BinarySet::LMSB);

		//unlike lz77, we need to deal with raw binary. We will use our BinarySet
		//class since it provides an easy way to deal with binary and std::vector
		//helps deal with properly storing booleans.
		
		//Other than that, it works just like lz77 with the exception that we can't
		//copy past the size of the buffer like in lz77.

		int oldTuplev1 = 0;
		int oldTuplev2 = 0;

		int tuplev1 = 0;//How far to go back
		int tuplev2 = 0;//length of string

		int maxBufferSize = 0x7FFF;

		int bitsForRef = 15;
		int bitsForCopy = 8;

		if(size<=0)
		{
			#ifdef USE_EXCEPTIONS
			throw InvalidSizeError();
			#endif
			return std::vector<unsigned char>();
		}

		if(data == nullptr)
		{
			#ifdef USE_EXCEPTIONS
			throw InvalidDataError();
			#endif
			return std::vector<unsigned char>();
		}

		int i = 0;
		while(i < size)
		{
			int lowestPoint = max(i-maxBufferSize, 0);
			int baseSize = i - lowestPoint;

			int lengthMax = min(size-i, 258);

			unsigned char* startBase = (data+lowestPoint);
			unsigned char* startMatch = (data+i);

			int matchLength = 0;
			int matchStartIndex = 0;

			StringTools::findLongestMatch(startBase, baseSize, startMatch, lengthMax, &matchStartIndex, &matchLength);

			int tempLength = matchLength;
			int tempBackwards = i - (lowestPoint + matchStartIndex);

			if(tempLength<3)
			{
				bin.add(true);
				bin.add(data[i]);
				i++;
			}
			else
			{
				bin.add(false);
				bin.add(tempLength, 15);
				bin.add(tempBackwards, 8);
				i += tempLength;
			}
		}
		
		return bin.toBytes();
	}

	std::vector<unsigned char> Compression::decompressLZSS(std::vector<unsigned char> data, size_t expectedSize)
	{
		return decompressLZSS(data.data(), data.size(), expectedSize);
	}

	std::vector<unsigned char> Compression::decompressLZSS(unsigned char* data, int size, size_t expectedSize)
	{
		BinarySet bin = BinarySet();
		bin.setBitOrder(BinarySet::LMSB);

		std::vector<unsigned char> output = std::vector<unsigned char>();

		bin.setValues(data, size);
		
		int bitsForRef = 15;
		int bitsForCopy = 8;

		if(size<=0)
		{
			#ifdef USE_EXCEPTIONS
			throw InvalidSizeError();
			#endif
			return std::vector<unsigned char>();
		}

		if(data == nullptr)
		{
			#ifdef USE_EXCEPTIONS
			throw InvalidDataError();
			#endif
			return std::vector<unsigned char>();
		}

		int i = 0;
		while(i < bin.size())
		{
			bool lit = bin.getBit(i);
			i++;
			if(lit)
			{
				unsigned char literalValue = bin.getBits(i, i+8, true);
				i += 8;
				output.push_back(literalValue);
			}
			else
			{
				int backwardsDis = bin.getBits(i, i+15, true);
				i += 15;
				int copyLength = bin.getBits(i, i+8, true);
				i += 8;

				int startLoc = output.size() - backwardsDis;
				for(int j=0; j<copyLength; j++)
				{
					output.push_back( output[startLoc+j] );
				}
			}

			if(output.size() > expectedSize)
			{
				#ifdef USE_EXCEPTIONS
				throw ExceededExpectedSizeError();
				#endif
				return std::vector<unsigned char>();
			}
		}

		return output;
	}

	#pragma endregion

} //NAMESPACE glib END