#include "InternalCompressionHeader.h"
#include "SuffixAutomaton.h"

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

	void Compression::getLZ77RefPairsCSA(unsigned char* data, int size, std::vector<lengthPair>* outputData, int compressionLevel)
	{
		int maxDistance = 1<<15;

		size_t index = 0;
    	size_t endIndex = size;

		ChainedSuffixAutomaton SA = ChainedSuffixAutomaton(32768, 4096, 258);
		int startLoc = 0;
		
		SA.resetSearch();
		while(index < endIndex)
		{
			bool newMatch = SA.searchNext(data[index]);
			if(!newMatch || index == endIndex-1)
			{
				bool includedCurrByte = true;
				std::pair<int, int> pairStuff = SA.extractSearch();
				SA.resetSearch();

				if(pairStuff.second >= 3)
				{
					int backwardsDis = startLoc - pairStuff.first;
					outputData->push_back({false, pairStuff.second, backwardsDis});
					if(pairStuff.second != 258)
						includedCurrByte = false;
					
					startLoc += pairStuff.second;
					
					if(index == endIndex-1)
						break;
				}
				else
				{
					includedCurrByte = false;
					for(int i=pairStuff.second; i>0; i--)
					{
						outputData->push_back({true, data[index-i], 0});
						startLoc++;
					}
					
					if(pairStuff.second == 0)
					{
						includedCurrByte = true;
						outputData->push_back({true, data[index], 0});
						startLoc++;
					}
				}

				if(includedCurrByte)
				{
					SA.extend(data[index]);
					index++;
				}
			}
			else
			{
				SA.extend(data[index]);
				index++;
			}
		}
		
	}

	void Compression::getLZ77RefPairsCHash(unsigned char* data, int size, std::vector<lengthPair>* outputData, int compressionLevel)
	{
		int maxDistance = 1<<15;
		switch(compressionLevel)
		{
			case 7:
				maxDistance = 1<<15;
				break;
			case 6:
				maxDistance = 1<<14;
				break;
			case 5:
				maxDistance = 1<<13;
				break;
			case 4:
				maxDistance = 1<<12;
				break;
			case 3:
				maxDistance = 1<<11;
				break;
			case 2:
				maxDistance = 1<<10;
				break;
			case 1:
				maxDistance = 1<<9;
				break;
			case 0:
				maxDistance = 1<<8;
				break;
			default:
				maxDistance = 1<<15;
				break;
		}

		if(data == nullptr || outputData == nullptr)
		{
			#ifdef USE_EXCEPTIONS
			throw InvalidDataError();
			#endif
			return;
		}
		
		if(size <= 0)
		{
			#ifdef USE_EXCEPTIONS
			throw InvalidSizeError();
			#endif
			return;
		}
		
		
		//for all bytes, try to match it in the hashmap.
		//Get 3 bytes and try to find it in the hashmap. If found, try and find match.
		//If not found, add the 3 values to the hashmap and write the first byte to the output

		//Method 5 - SIMPLE_HASH_MAP : Best Performance and Good Size.
		SimpleHashMap<int, int> map = SimpleHashMap<int, int>( SimpleHashMap<int, int>::MODE_KEEP_ALL, 1<<15 );
		map.setMaxLoadFactor(-1);

		int i = 0;
		while(i < size-2)
		{
			int key = data[i] + ((int)data[i+1]<<8) + ((int)data[i+2]<<16);
			
			HashPair<int, int>* k = map.get(key);

			if(k == nullptr)
			{
				//not found or size too small
				//always insert
				map.add(key, i);

				outputData->push_back( {true, data[i], 0} );
				i++;
			}
			else
			{
				int lowestPoint = max(i-maxDistance, 0);
				int bestLength = 0;
				int bestLocation = 0;

				do
				{
					int locationOfMatch = k->data;

					if(locationOfMatch < lowestPoint)
					{
						//maximum backwards distance reached
						break;
					}

					int lengthMax = min(size-i, 258);
					
					unsigned char* startBase = (data+locationOfMatch);
					unsigned char* startMatch = (data+i);

					int len;
					
					for(len=3; len<lengthMax; len++)
					{
						if(startMatch[len] != startBase[len])
						{
							break;
						}
					}

					if(len>=bestLength)
					{
						bestLength = len;
						bestLocation = locationOfMatch;
					}
					
					k = map.getNext();
				} while(k != nullptr);

				//always insert
				map.add( key, i );
				for(int j=1; j<bestLength-2; j++)
				{
					int nkey = data[i+j] + ((int)data[i+j+1]<<8) + ((int)data[i+j+2]<<16);
					map.add( nkey, i+j );
				}

				int backwardsDis = i - bestLocation;
				
				if(bestLength>=3)
				{
					outputData->push_back( {false, bestLength, backwardsDis} );
					i += bestLength;
				}
				else
				{
					//couldn't find match within max allowed distance
					outputData->push_back( {true, data[i], 0} );
					i++;
				}
			}

		}

		int remainder = size - i;
		for(int j=0; j<remainder; j++)
		{
			outputData->push_back( {true, data[i+j], 0} );
		}
	}

	void Compression::getLZ77RefPairsKMP(unsigned char* data, int size, std::vector<lengthPair>* outputData, int compressionLevel)
	{
		//unlike lz77, we need to deal with raw binary. We will use our BinarySet
		//class since it provides an easy way to deal with binary and std::vector
		//helps deal with properly storing booleans.
		
		//Other than that, it works just like lz77 with the exception that we can't
		//copy past the size of the buffer like in lz77.

		int maxDistance = 1<<15;
		switch(compressionLevel)
		{
			case 7:
				maxDistance = 1<<15;
				break;
			case 6:
				maxDistance = 1<<14;
				break;
			case 5:
				maxDistance = 1<<13;
				break;
			case 4:
				maxDistance = 1<<12;
				break;
			case 3:
				maxDistance = 1<<11;
				break;
			case 2:
				maxDistance = 1<<10;
				break;
			case 1:
				maxDistance = 1<<9;
				break;
			case 0:
				maxDistance = 1<<8;
				break;
			default:
				maxDistance = 1<<15;
				break;
		}

		if(data == nullptr || outputData == nullptr)
		{
			#ifdef USE_EXCEPTIONS
			throw InvalidDataError();
			#endif
			return;
		}
		
		if(size <= 0)
		{
			#ifdef USE_EXCEPTIONS
			throw InvalidSizeError();
			#endif
			return;
		}

		int i = 0;
		while(i < size)
		{
			int lowestPoint = max(i-maxDistance, 0);
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
				outputData->push_back({true, data[i], 0});
				i++;
			}
			else
			{
				outputData->push_back({false, tempLength, tempBackwards});
				i += tempLength;
			}
		}
	}

	#pragma endregion
    
} //NAMESPACE glib END