#include "InternalCompressionHeader.h"

namespace glib
{

#pragma region LZSS

	std::vector<unsigned char> Compression::compressLZSS(std::vector<unsigned char> data, int maxBufferSize)
	{
		return compressLZSS(data.data(), data.size(), maxBufferSize);
	}

	std::vector<unsigned char> Compression::compressLZSS(unsigned char* data, int size, int maxBufferSize)
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

		int bitsForRef = ceil(log2(maxBufferSize)); //Since we are working in binary

		if(size<=0 || maxBufferSize<=0)
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
		while (i < size)
		{
			int k = i;
			int offset = max(0, i - maxBufferSize);
			int j = 0;

			oldTuplev1 = 0;
			oldTuplev2 = 0;
			tuplev1 = 0;
			tuplev2 = 0;
			while (j < maxBufferSize && j < size)
			{
				if (k < size)
				{
					if (data[j + offset] == data[k])
					{
						if (k == i)
						{
							if (tuplev2 + i >= size)
							{
								//can't work since the last value in our tuple
								//would have to be a null which we can't do
							}
							else if (j + offset >= i)
							{
								//Not allowed to extend past the original
								//start index
								break;
							}
							else if (j + offset == k)
							{
								tuplev1 = 0;
								tuplev2 = 0;
								break;
							}

							tuplev1 = i - j - offset;
						}
						tuplev2 += 1;
						k++;
					}
					else
					{
						k = i;
						if (tuplev2 >= oldTuplev2)
						{
							oldTuplev1 = tuplev1;
							oldTuplev2 = tuplev2;
						}

						tuplev1 = i - j - offset;
						tuplev2 = 0;

						if (j + offset >= i)
						{
							break;
						}
					}
					j++;
				}
				else
				{
					k = i;
					if (tuplev2 + i >= size)
					{
						//can't work since the last value in our tuple
						//would have to be a null which we can't do
					}
					else if (tuplev2 >= oldTuplev2)
					{
						oldTuplev1 = tuplev1;
						oldTuplev2 = tuplev2;
					}

					tuplev1 = i - j - offset;
					tuplev2 = 0;

					if (j + offset >= i)
					{
						break;
					}
				}
			}

			//Now we check if it is optimal to store the additional
			//values instead of just one for a literal

			int lengthNeeded = (int)ceilf((float)(bitsForRef * 2 + 1)/9.0f);

			if (oldTuplev2 >= lengthNeeded)
			{
				//optimal by at least one bit
				bin.add(false);
				bin.add(oldTuplev1,bitsForRef, 32-bitsForRef);
				bin.add(oldTuplev2,bitsForRef, 32-bitsForRef);
				
				i += oldTuplev2;

			}
			else
			{
				//worse so store the literal
				bin.add(true);
				bin.add(data[i]);

				i++;
			}
		}

		//bin.printVals(true);
		
		return bin.toBytes();
	}

	std::vector<unsigned char> Compression::decompressLZSS(std::vector<unsigned char> data, int maxBufferSize)
	{
		return decompressLZSS(data.data(), data.size(), maxBufferSize);
	}

	std::vector<unsigned char> Compression::decompressLZSS(unsigned char* data, int size, int maxBufferSize)
	{
		BinarySet bin = BinarySet();
		bin.setBitOrder(BinarySet::LMSB);

		std::vector<unsigned char> output = std::vector<unsigned char>();

		bin.setValues(data, size);
		
		int bitsForRef = ceil(log2(maxBufferSize)); //Since we are working in binary

		if(size<=0 || maxBufferSize<=0)
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

		int i = bin.size()-1;
		while(i>=0)
		{
			bool isLit = bin.getBit(i) == 1;
			
			if (isLit)
			{
				if (i < 9)
				{
					//can't continue
					break;
				}
				unsigned char nextByte = 0;
				for (int k = 0; k < 8; k++)
				{
					nextByte += bin.getBit(i - k - 1) << (7 - k);
				}
				output.push_back(nextByte);

				i -= 9;
			}
			else
			{
				int refLoc = 0;
				int copyLen = 0;

				if (i < 1+2*bitsForRef)
				{
					//can't continue safetly
					//probably at the end of the data anyway
					break;
				}

				for (int k = 0; k < bitsForRef; k++)
				{
					refLoc += bin.getBit(i - k - 1) << (bitsForRef - 1 - k);
					copyLen += bin.getBit(i - k - 1 - bitsForRef) << (bitsForRef - 1 - k);
				}

				int offsetLoc = max(0, output.size() - refLoc);

				if (offsetLoc < 0)
				{
					//something went wrong
					//StringTools::out << "Error decompressing LZSS on reference: " << StringTools::lineBreak;
					//StringTools::out << "Reference Location goes back farther than the size of your buffer." << StringTools::lineBreak;
					//StringTools::out << "RefLoc: " << (int)refLoc << ", CurrentBufferSize: " << (int)output.size() << ", MessageLen: " << (int)copyLen << StringTools::lineBreak;
						
					#ifdef USE_EXCEPTIONS
					throw Compression::LZSS_ERROR();
					#endif

					return std::vector<unsigned char>();
				}

				for (int k = 0; k < copyLen; k++)
				{
					output.push_back(output[offsetLoc + k]);
				}

				i -= (1 + 2*bitsForRef);
			}
		}
		

		return output;
	}

	#pragma endregion

} //NAMESPACE glib END