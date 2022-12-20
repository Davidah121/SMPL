#include "InternalCompressionHeader.h"

namespace glib
{

#pragma region LZ77

	std::vector<unsigned char> Compression::compressLZ77(std::vector<unsigned char> data, int maxBufferSize)
	{
		return compressLZ77(data.data(), data.size(), maxBufferSize);
	}

	std::vector<unsigned char> Compression::compressLZ77(unsigned char* data, size_t size, int maxBufferSize)
	{
		std::vector<unsigned char> output = std::vector<unsigned char>();

		int oldTuplev1 = 0;
		int oldTuplev2 = 0;

		int tuplev1 = 0;//How far to go back
		int tuplev2 = 0;//length of string

		if(size<=0 || maxBufferSize<=0)
		{
			#ifdef USE_EXCEPTIONS
			throw InvalidSizeError();
			#endif
			return output;
		}

		if(data == nullptr)
		{
			#ifdef USE_EXCEPTIONS
			throw InvalidDataError();
			#endif
			return output;
		}

		//has to be at least 2 repeated values. We are using a greedy version
		//meaning that we search for the best possible match not regarding performance.
		//If the length is greater than the previous, it is better. This also 
		//means that we search through everything.

		int bytesForRef = (int)ceil(log2(maxBufferSize) / 8.0);

		int i = 0;
		while(i<size)
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

						if (j+offset >= i)
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
			
			i += oldTuplev2 + 1;

			
			if (i <= size)
			{
				//StringTools::out << oldTuplev1 << ',' << oldTuplev2 << ',' << (int)data[i - 1] << StringTools::lineBreak;
				for(int b=0; b<bytesForRef; b++)
				{
					int shiftVal = (8 * (bytesForRef - b - 1));
					output.push_back((unsigned char) ((oldTuplev1>>shiftVal) & 0xFF));
				}
				for (int b = 0; b < bytesForRef; b++)
				{
					int shiftVal = (8 * (bytesForRef - b - 1));
					output.push_back((unsigned char)((oldTuplev2 >> shiftVal) & 0xFF));
				}
				output.push_back((unsigned char)data[i-1]);
			}
			else
			{
				//StringTools::out << oldTuplev1 << ',' << oldTuplev2 << ',' << (int)data[i - 2] << StringTools::lineBreak;
				for (int b = 0; b < bytesForRef; b++)
				{
					int shiftVal = (8 * (bytesForRef - b - 1));
					output.push_back((unsigned char)((oldTuplev1 >> shiftVal) & 0xFF));
				}
				for (int b = 0; b < bytesForRef; b++)
				{
					int shiftVal = (8 * (bytesForRef - b - 1));
					output.push_back((unsigned char)((oldTuplev2 >> shiftVal) & 0xFF));
				}
				output.push_back((unsigned char)data[i-2]);
			}
		}

		return output;
	}

	std::vector<unsigned char> Compression::decompressLZ77(std::vector<unsigned char> data, int maxBufferSize, size_t expectedSize)
	{
		return decompressLZ77(data.data(), data.size(), maxBufferSize, expectedSize);
	}

	std::vector<unsigned char> Compression::decompressLZ77(unsigned char* data, size_t size, int maxBufferSize, size_t expectedSize)
	{
		std::vector<unsigned char> output = std::vector<unsigned char>();

		int bytesForRef = (int)ceil(log2(maxBufferSize) / 8.0);

		int t1 = 0;
		int t2 = 0;

		if(size<=0 || maxBufferSize<=0)
		{
			#ifdef USE_EXCEPTIONS
			throw InvalidSizeError();
			#endif
			return output;
		}

		if(data == nullptr)
		{
			#ifdef USE_EXCEPTIONS
			throw InvalidDataError();
			#endif
			return output;
		}

		//readTuple then put into buffer
		for (int i = 0; i < size; i+=3)
		{
			t1 = 0;
			t2 = 0;
			for (int b = 0; b < bytesForRef; b++)
			{
				t1 += data[i + b] << (8 * b); //Referece offset
				t2 += data[i + b + bytesForRef] << (8 * b); //Length of message
			}
			//unsigned char t1 = data[i]; //Reference offset
			//unsigned char t2 = data[i + 1]; //Length of message

			unsigned char t3 = data[i + 2*bytesForRef]; //value to add on afterwards

			int startLoc = output.size() - t1;

			if(output.size()+t2 > expectedSize)
			{
				#ifdef USE_EXCEPTIONS
				throw ExceededExpectedSizeError();
				#endif
				return std::vector<unsigned char>();
			}

			//copy stuff
			for (int j = 0; j < t2; j++)
			{
				output.push_back( output[startLoc + j] );
			}

			output.push_back(t3);

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