#include "InternalCompressionHeader.h"

namespace glib
{

#pragma region LZW

	std::vector<unsigned char> Compression::compressLZW(std::vector<unsigned char> data, int* codeSizePointer)
	{
		return Compression::compressLZW(data.data(), data.size(), codeSizePointer);
	}

	std::vector<unsigned char> Compression::compressLZW(unsigned char* data, int size, int* codeSizePointer)
	{
		std::vector<unsigned char> output = std::vector<unsigned char>();

		if(size <= 0)
		{
			#ifdef USE_EXCEPTIONS
			throw InvalidSizeError();
			#endif
		}

		if(data == nullptr)
		{
			#ifdef USE_EXCEPTIONS
			throw InvalidDataError();
			#endif
			return output;
		}
		
		//First make the base dictionary
		SimpleHashMap<std::string, int> baseDictionary = SimpleHashMap<std::string, int>();

		int codeSize = -1;
		if(codeSizePointer!=nullptr)
		{
			codeSize = *codeSizePointer;
		}

		if(codeSize <= 0)
		{
			int bSize = 0;
			for (int i = 0; i < size; i++)
			{
				std::string temp = "";
				temp += data[i];

				baseDictionary.add( temp, bSize ); //fix
				bSize++;
			}

			if(codeSizePointer!=nullptr)
				*codeSizePointer = (int)ceil(log2(baseDictionary.size()));
		}
		else
		{
			int s = 1 << codeSize;
			for(int i=0; i < s; i++)
			{
				std::string k = "";
				k += (char)i;
				baseDictionary.add( k, i );
			}
		}
		

		//Add the clearDictionary and endOfData values.
		//We need to store the location so that it can be excluded in compression
		//when we compare what data is in the dictionary.

		int clearDictionaryLocation = baseDictionary.size();
		int EndOfDataLocation = baseDictionary.size()+1;

		baseDictionary.rehash();

		SimpleHashMap<std::string, int> newDictionary = baseDictionary;

		//Compress
		//First note how many bits to use
		int baseBits = (int)ceil(log2(newDictionary.size()+2));
		int currBits = baseBits;

		//Now store the values as a binary string of sorts
		//We are using a custom class BinarySet that does the
		//conversion for us.
		BinarySet binData = BinarySet();
		binData.setBitOrder(BinarySet::LMSB);

		std::string lastString = "";
		std::string newString = "";
		int preIndex = 0;

		binData.add(clearDictionaryLocation, currBits);

		int i = 0;
		while(i<size)
		{
			newString += data[i];

			HashPair<std::string, int>* itr = newDictionary.get(newString);
			
			bool exists = (itr != nullptr);

			if (exists == true)
			{
				preIndex = itr->data;
				lastString = newString;
				i++;
			}
			else
			{
				binData.add(preIndex, currBits);
				
				newDictionary.add( newString, newDictionary.size()+2 );

				int shifts = 0;
				int v = 1;
				while(shifts<32)
				{
					if(newDictionary.size()+2 > v)
					{
						v = v << 1;
						shifts++;
					}
					else
					{
						break;
					}
				}

				currBits = shifts;

				preIndex = -1;
				newString = "";
				lastString = "";
			}

			if(newDictionary.size()+2 == 4096)
			{
				//clear Dictionary and start over
				binData.add(clearDictionaryLocation, currBits);
				currBits = baseBits;
				newDictionary = baseDictionary;
				preIndex = -1;
				newString = "";
				lastString = "";
			}
			
		}

		if (newString != "")
		{
			binData.add(preIndex, currBits, 0);
		}

		binData.add(EndOfDataLocation, currBits, 0);
		output = binData.toBytes();


		return output;
	}

	std::vector<unsigned char> Compression::decompressLZW(std::vector<unsigned char> data, int dictionarySize)
	{
		return Compression::decompressLZW(data.data(), data.size(), dictionarySize);
	}

	std::vector<unsigned char> Compression::decompressLZW(unsigned char* data, int size, int dictionarySize)
	{
		//Must know the dictionary size. Do not include the clear dictionary 
		//and end data symbols
		//Will return the index in the dictionary for each value
		//This way, you don't need to provide the dictionary for
		//your data, but just the size.
		std::vector<unsigned char> output = std::vector<unsigned char>();

		if(data == nullptr)
		{
			#ifdef USE_EXCEPTIONS
			throw InvalidDataError();
			#endif
			return output;
		}

		if (size > 0 && dictionarySize > 0)
		{
			//First, create a tempDictionary
			//We will assume that the dictionary's max size is a byte based
			//on our implementation above. It can be expanded however, for
			//many purposes, it is unnecessary.
			std::vector<std::string> baseDictionary = std::vector<std::string>();
			for (int i = 0; i < dictionarySize; i++)
			{
				std::string temp = "";
				temp += (unsigned char)i;

				baseDictionary.push_back(temp);
			}

			baseDictionary.push_back("");
			baseDictionary.push_back("");

			int clearSymLoc = dictionarySize;
			int endSymLoc = dictionarySize + 1;

			int bitsToRead = (int)ceil(log2(baseDictionary.size()));
			int baseBitsToRead = bitsToRead;
			//Next, we load all of our data into a more friendly format
			//for dealing with binary data directly.
			BinarySet binData = BinarySet();
			binData.setBitOrder(BinarySet::LMSB);

			binData.setValues(data, size);

			//Our data is oriented in an odd way. We read bits and add onto
			//end of the value. kinda like a queue.
			std::vector<std::string> newDictionary = std::vector<std::string>(baseDictionary);

			int lastIndex = -1;
			int index = 0;
			int shift = 0;

			//We choose to handle the clear dictionary symbol now since it
			//is simple to do.
			for (int i = 0; i < binData.size(); i++)
			{
				index += ((int)binData.getBit(i) << shift);
				shift++;

				if (shift >= bitsToRead)
				{
					if (index == clearSymLoc)
					{
						//StringTools::out << "ClearSym at " << i << " of " << binData.size() << StringTools::lineBreak;
						
						newDictionary = std::vector<std::string>(baseDictionary);
						lastIndex = -1;
						bitsToRead = baseBitsToRead;
						shift = 0;
						index = 0;
						continue;
					}
					else if (index == endSymLoc)
					{
						//StringTools::out << "EndofSym at " << i << " of " << binData.size() << StringTools::lineBreak;
						break;
					}
					else
					{
						//add the dictionary value to the output
						//Add a new dictionary value if the lastIndex is valid
							//You add the lastValue + newValue[0]
							//This is for when your values go over 1 byte

						//If the index is invalid, add it as the lastIndex + lastIndex[0]
						if (index < newDictionary.size())
						{
							std::string temp = newDictionary[index];
							for (int j = 0; j < temp.size(); j++)
							{
								output.push_back((unsigned char)temp[j]);
							}

							if (lastIndex != -1)
							{
								if (lastIndex < newDictionary.size())
								{
									std::string newEntry = newDictionary[lastIndex] + newDictionary[index][0];
									newDictionary.push_back(newEntry);
									int tempBits = (int)ceil(log2(newDictionary.size()+1));
									if(tempBits<=12)
										bitsToRead = tempBits;
									else
										newDictionary.pop_back();
								}
								else
								{
									#ifdef USE_EXCEPTIONS
									throw Compression::LZW_ERROR_L317();
									#endif
									//StringTools::println("L317 ERROR: %d", newDictionary.size());
									//There is an error in the data
									break;
								}
							}
						}
						else
						{
							if (lastIndex < newDictionary.size())
							{
								std::string lastEntry = newDictionary[lastIndex];
								std::string newEntry = lastEntry + lastEntry[0];
								newDictionary.push_back(newEntry);
								int tempBits = (int)ceil(log2(newDictionary.size()+1));

								if(tempBits<=12)
									bitsToRead = tempBits;
								else
									newDictionary.pop_back();
						
								for (int j = 0; j < newEntry.size(); j++)
								{
									output.push_back((unsigned char)newEntry[j]);
								}
							}
							else
							{
								#ifdef USE_EXCEPTIONS
								throw Compression::LZW_ERROR_L340();
								#endif
								//StringTools::println("L340 ERROR");
								//StringTools::out << "L340 Error in getting last index, " << lastIndex << ", Index=" << index << " dictionary: " << newDictionary.size() << StringTools::lineBreak;
								break;
							}
						}

						lastIndex = index;
						index = 0;
						shift = 0;
					}
				}
			}
		}

		return output;
	}

	#pragma endregion

} //NAMESPACE glib END