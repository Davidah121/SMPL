#include "InternalCompressionHeader.h"

namespace glib
{

#pragma region LZW

	#pragma region THREAD_FUNCTION
	void LZWThreadFunction(BinarySet* outputData, unsigned char* data, size_t size, int codeSize, bool omitEndCode)
	{
		Compression::compressLZW(outputData, data, size, codeSize, omitEndCode);
	}

	#pragma endregion
	std::vector<unsigned char> Compression::compressLZW(std::vector<unsigned char> data, int blocks, int codeSize)
	{
		return Compression::compressLZW(data.data(), data.size(), blocks, codeSize);
	}
	
	std::vector<unsigned char> Compression::compressLZW(unsigned char* data, size_t size, int blocks, int codeSize)
	{
		if(codeSize <= 0)
		{
			return {};
		}

		int totalBlocks = blocks;
		int blockSize = size;
		if(blocks > 1)
		{
			blockSize /= blocks;
			totalBlocks = blocks;
		}
		else
		{
			return compressLZW(data, size, codeSize);
		}

		int threadCount = (int)System::getNumberOfThreads();
		std::vector<BinarySet> blockDataArray = std::vector<BinarySet>(blocks);
		std::vector<std::thread> blockThreads = std::vector<std::thread>(threadCount);

		for(int i=0; i<blocks; i++)
		{
			int threadID = i%threadCount;
			unsigned char* inputData = data + blockSize*i;
			int inputSize = blockSize;
			if(i==blocks-1)
				inputSize = size - blockSize*i;

			if(i==0)
			{
				blockThreads[threadID] = std::thread(LZWThreadFunction, &blockDataArray[i], inputData, inputSize, codeSize, true);
			}
			else
			{
				if(blockThreads[threadID].joinable())
					blockThreads[threadID].join();
				
				bool omitEndCode = true;
				if(i == blocks-1)
				{
					omitEndCode = false;
				}

				blockThreads[threadID] = std::thread(LZWThreadFunction, &blockDataArray[i], inputData, inputSize, codeSize, omitEndCode);
			}
		}

		for(int i=0; i<threadCount; i++)
		{
			int threadID = i%threadCount;

			if(blockThreads[threadID].joinable())
				blockThreads[threadID].join();
		}

		BinarySet finalSet = blockDataArray[0];
		for(int i=1; i<blocks; i++)
		{
			finalSet.add(blockDataArray[i]);
		}

		return finalSet.toBytes();
	}

	std::vector<unsigned char> Compression::compressLZW(std::vector<unsigned char> data, int codeSize)
	{
		return Compression::compressLZW(data.data(), data.size(), codeSize);
	}

	void Compression::compressLZW(BinarySet* outputData, unsigned char* data, size_t size, int codeSize, bool omitEndCode)
	{
		StreamCompressionLZW stream = StreamCompressionLZW(StreamCompressionLZW::TYPE_COMPRESSION, codeSize);
		stream.addData(data, size);
		if(!omitEndCode)
			stream.addEndData();
		else
			stream.addClearDictionary(); //Must add to reset the dictionary for concatenation. Results in 2 clear dictionaries back to back. Should fix.

		outputData->clear();
		outputData->setValues( stream.getBuffer().getByteRef().data(), stream.getBuffer().getByteRef().size() );
		outputData->setNumberOfBits( stream.getBuffer().size() );
	}

	std::vector<unsigned char> Compression::compressLZW(unsigned char* data, size_t size, int codeSize)
	{
		if(data == nullptr)
		{
			#ifdef USE_EXCEPTIONS
			throw InvalidDataError();
			#endif
			return {};
		}
		if(size == 0)
		{
			#ifdef USE_EXCEPTIONS
			throw InvalidSizeError();
			#endif
			return {};
		}

		StreamCompressionLZW stream = StreamCompressionLZW(StreamCompressionLZW::TYPE_COMPRESSION, codeSize);
		stream.addData(data, size);
		stream.addEndData();

		return stream.getBuffer().toBytes();
	}

	std::vector<unsigned char> Compression::decompressLZW(std::vector<unsigned char> data, int dictionarySize, size_t expectedSize)
	{
		return Compression::decompressLZW(data.data(), data.size(), dictionarySize, expectedSize);
	}

	std::vector<unsigned char> Compression::decompressLZW(unsigned char* data, size_t size, int dictionarySize, size_t expectedSize)
	{
		StreamCompressionLZW stream = StreamCompressionLZW(StreamCompressionLZW::TYPE_DECOMPRESSION, dictionarySize);
		// stream.addData(data, size);
		for(int i=0; i<size; i+=4096)
		{
			int nSize = ((size - i) > 4096) ? 4096 : (size-i);
			stream.addData(&data[i], nSize);
			if(stream.size() > expectedSize)
			{
				#ifdef USE_EXCEPTIONS
				throw ExceededExpectedSizeError();
				#endif
				return {}; //EXCEEDED EXPECTED SIZE
			}

		}

		if(!stream.getEndData())
		{
			#ifdef USE_EXCEPTIONS
			throw InvalidDataStreamEndError();
			#endif
			return {}; //ERROR IN DATA
		}

		return stream.getBuffer().toBytes();
	}

	// std::vector<unsigned char> Compression::decompressLZW(unsigned char* data, size_t size, int dictionarySize, size_t expectedSize)
	// {
	// 	//Must know the dictionary size. Do not include the clear dictionary 
	// 	//and end data symbols
	// 	//Will return the index in the dictionary for each value
	// 	//This way, you don't need to provide the dictionary for
	// 	//your data, but just the size.
	// 	std::vector<unsigned char> output = std::vector<unsigned char>();

	// 	if(data == nullptr)
	// 	{
	// 		#ifdef USE_EXCEPTIONS
	// 		throw InvalidDataError();
	// 		#endif
	// 		return output;
	// 	}

	// 	if (size > 0 && dictionarySize > 0)
	// 	{
	// 		//First, create a tempDictionary
	// 		//We will assume that the dictionary's max size is a byte based
	// 		//on our implementation above. It can be expanded however, for
	// 		//many purposes, it is unnecessary.
	// 		std::vector<std::string> baseDictionary = std::vector<std::string>();
	// 		for (int i = 0; i < dictionarySize; i++)
	// 		{
	// 			std::string temp = "";
	// 			temp += (unsigned char)i;

	// 			baseDictionary.push_back(temp);
	// 		}

	// 		baseDictionary.push_back("");
	// 		baseDictionary.push_back("");

	// 		int clearSymLoc = dictionarySize;
	// 		int endSymLoc = dictionarySize + 1;

	// 		int bitsToRead = (int)ceil(log2(baseDictionary.size()));
	// 		int baseBitsToRead = bitsToRead;
	// 		//Next, we load all of our data into a more friendly format
	// 		//for dealing with binary data directly.
	// 		BinarySet binData = BinarySet();
	// 		binData.setBitOrder(BinarySet::LMSB);

	// 		binData.setValues(data, size);

	// 		//Our data is oriented in an odd way. We read bits and add onto
	// 		//end of the value. kinda like a queue.
	// 		std::vector<std::string> newDictionary = std::vector<std::string>(baseDictionary);

	// 		int lastIndex = -1;
	// 		size_t index = 0;
	// 		int shift = 0;

	// 		size_t t1,t2;
	// 		size_t add1Time, add2Time;

	// 		add1Time = 0;
	// 		add2Time = 0;
	// 		int count = 0;

	// 		//We choose to handle the clear dictionary symbol now since it
	// 		//is simple to do.
	// 		for (size_t i = 0; i < binData.size(); i++)
	// 		{
	// 			index += ((size_t)binData.getBit(i) << shift);
	// 			shift++;

	// 			if (shift >= bitsToRead)
	// 			{
	// 				count++;
	// 				if(count == 2966 || count == 5931)
	// 					StringTools::println("Count = %d, Index = %d", count, index);
					
	// 				if (index == (size_t)clearSymLoc)
	// 				{
	// 					// StringTools::out << "ClearSym at " << i << " of " << binData.size() << StringTools::lineBreak;
	// 					// StringTools::println("CLEAR at %d", counter);
						
	// 					newDictionary = std::vector<std::string>(baseDictionary);
	// 					lastIndex = -1;
	// 					bitsToRead = baseBitsToRead;
	// 					shift = 0;
	// 					index = 0;
						
	// 					continue;
	// 				}
	// 				else if (index == (size_t)endSymLoc)
	// 				{
	// 					//StringTools::out << "EndofSym at " << i << " of " << binData.size() << StringTools::lineBreak;
	// 					break;
	// 				}
	// 				else
	// 				{
	// 					//add the dictionary value to the output
	// 					//Add a new dictionary value if the lastIndex is valid
	// 						//You add the lastValue + newValue[0]
	// 						//This is for when your values go over 1 byte

	// 					//If the index is invalid, add it as the lastIndex + lastIndex[0]
	// 					if (index < newDictionary.size())
	// 					{
	// 						t1 = System::getCurrentTimeNano();
	// 						std::string temp = newDictionary[index];
							
	// 						// if(count < 128)
	// 						// 	StringTools::println("Size: %llu", temp.size());
	// 						// count++;

	// 						for (size_t j = 0; j < temp.size(); j++)
	// 						{
	// 							output.push_back((unsigned char)temp[j]);
	// 						}
	// 						t2 = System::getCurrentTimeNano();
	// 						add1Time += t2-t1;

							
	// 						t1 = System::getCurrentTimeNano();
	// 						if (lastIndex != -1)
	// 						{
	// 							if (lastIndex < (int)newDictionary.size())
	// 							{
	// 								std::string newEntry = newDictionary[lastIndex] + newDictionary[index][0];
	// 								newDictionary.push_back(newEntry);
	// 								int tempBits = (int)ceil(log2(newDictionary.size()+1));
	// 								if(tempBits<=12)
	// 									bitsToRead = tempBits;
	// 								else
	// 									newDictionary.pop_back();
									
	// 								if(newDictionary.size() == 4095)
	// 									StringTools::println("%llu", newDictionary.size());
	// 							}
	// 							else
	// 							{
	// 								#ifdef USE_EXCEPTIONS
	// 								throw Compression::LZW_ERROR_L317();
	// 								#endif
	// 								//StringTools::println("L317 ERROR: %d", newDictionary.size());
	// 								//There is an error in the data
	// 								break;
	// 							}
	// 						}
							
	// 						t2 = System::getCurrentTimeNano();
	// 						add2Time += t2-t1;
	// 					}
	// 					else
	// 					{
	// 						if (lastIndex < (int)newDictionary.size())
	// 						{
	// 							std::string lastEntry = newDictionary[lastIndex];
	// 							std::string newEntry = lastEntry + lastEntry[0];
	// 							newDictionary.push_back(newEntry);
	// 							int tempBits = (int)ceil(log2(newDictionary.size()+1));

	// 							if(tempBits<=12)
	// 								bitsToRead = tempBits;
	// 							else
	// 								newDictionary.pop_back();
						
	// 							for (size_t j = 0; j < newEntry.size(); j++)
	// 							{
	// 								output.push_back((unsigned char)newEntry[j]);
	// 							}
	// 						}
	// 						else
	// 						{
	// 							#ifdef USE_EXCEPTIONS
	// 							throw Compression::LZW_ERROR_L340();
	// 							#endif
	// 							//StringTools::println("L340 ERROR");
	// 							//StringTools::out << "L340 Error in getting last index, " << lastIndex << ", Index=" << index << " dictionary: " << newDictionary.size() << StringTools::lineBreak;
	// 							break;
	// 						}
	// 					}

	// 					lastIndex = (int)index;
	// 					index = 0;
	// 					shift = 0;
	// 				}
	// 			}

	// 			if(output.size() > expectedSize)
	// 			{
	// 				#ifdef USE_EXCEPTIONS
	// 				throw ExceededExpectedSizeError();
	// 				#endif
	// 				return std::vector<unsigned char>();
	// 			}
	// 		}

	// 	StringTools::println("Time to add 1: %llu", add1Time);
	// 	StringTools::println("Time to add 2: %llu", add2Time);
	// 	}
		
	// 	StringTools::println("Size: %llu", output.size());
	// 	return output;
	// }

	#pragma endregion

} //NAMESPACE glib END