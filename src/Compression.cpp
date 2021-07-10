#include "Compression.h"
#include <iostream>
#include "LinkedList.h"
#include "BinarySet.h"
#include "Algorithms.h"
#include "StringTools.h"
#include "MathExt.h"
#include "System.h"
#include <thread>
#include "SimpleFile.h"

#include "SimpleHashMap.h"

#ifndef min
	#define min(a,b) (a<b)? a:b
#endif

#ifndef max
	#define max(a,b) (a>b)? a:b
#endif

Compression::Compression()
{
}

Compression::~Compression()
{
}



#pragma region RLE

std::vector<unsigned char> Compression::compressRLE(std::vector<unsigned char> data)
{
	return Compression::compressRLE(data.data(), data.size());
}

std::vector<unsigned char> Compression::compressRLE(unsigned char* data, int size)
{
	//Runs that are 1 byte in size and data that is 1 byte in size
	std::vector<unsigned char> output = std::vector<unsigned char>();

	if(data == nullptr)
	{
		#ifdef USE_EXCEPTIONS
		throw InvalidDataError();
		#endif
		return output;
	}

	if (size > 0)
	{
		unsigned char lastValue = data[0];
		unsigned char value = 0;
		int run = 1;

		for (int i = 0; i < size; i++)
		{
			value = data[i];

			if (value == lastValue)
			{
				run++;
			}
			else
			{
				//write
				output.push_back((unsigned char)run);
				output.push_back(lastValue);

				run = 1;
				lastValue = value;
			}

			if (run == 255)
			{
				//write
				output.push_back((unsigned char)run);
				output.push_back(lastValue);
				run = 0;
			}
		}

		output.push_back((unsigned char)run);
		output.push_back(lastValue);
	}
	else
	{
		#ifdef USE_EXCEPTIONS
		throw InvalidSizeError();
		#endif
	}

	return output;
}

std::vector<unsigned char> Compression::decompressRLE(std::vector<unsigned char> data)
{
	return decompressRLE(data.data(), data.size());
}

std::vector<unsigned char> Compression::decompressRLE(unsigned char* data, int size)
{
	std::vector<unsigned char> uncompressedData = std::vector<unsigned char>();

	int i = 0;

	if(size<=0)
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
		return uncompressedData;
	}

	while (i < size)
	{
		int run = data[i];
		int value = data[i + 1];

		for (int k = 0; k < run; k++)
		{
			uncompressedData.push_back(value);
		}

		i += 2;
	}

	return uncompressedData;
}

#pragma endregion

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

#pragma region LZ77

std::vector<unsigned char> Compression::compressLZ77(std::vector<unsigned char> data, int maxBufferSize)
{
	return compressLZ77(data.data(), data.size(), maxBufferSize);
}

std::vector<unsigned char> Compression::compressLZ77(unsigned char* data, int size, int maxBufferSize)
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

	int bytesForRef = ceil(log2(maxBufferSize) / 8.0);

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

std::vector<unsigned char> Compression::decompressLZ77(std::vector<unsigned char> data, int maxBufferSize)
{
	return decompressLZ77(data.data(), data.size(), maxBufferSize);
}

std::vector<unsigned char> Compression::decompressLZ77(unsigned char* data, int size, int maxBufferSize)
{
	std::vector<unsigned char> output = std::vector<unsigned char>();

	int bytesForRef = ceil(log2(maxBufferSize) / 8.0);

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
		//copy stuff
		for (int j = 0; j < t2; j++)
		{
			output.push_back( output[startLoc + j] );
		}

		output.push_back(t3);
	}

	return output;
}

#pragma endregion

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

#pragma region HUFFMAN

struct dataLengthCombo
{
	int data;
	int length;
	int code;
};

std::vector<unsigned char> Compression::compressHuffman(std::vector<unsigned char> data, BinaryTree<HuffmanNode>* tree)
{
	return compressHuffman(data.data(), data.size(), tree);
}

std::vector<unsigned char> Compression::compressHuffman(unsigned char* data, int size, BinaryTree<HuffmanNode>* tree)
{
	if(data == nullptr)
	{
		#ifdef USE_EXCEPTIONS
		throw InvalidDataError();
		#endif
		return std::vector<unsigned char>();
	}

	BinarySet output = BinarySet();
	output.setBitOrder(BinarySet::LMSB);

	//First, build a huffmanTree.
	//We have created a function to do this since it takes up
	//more space than I wanted. It will be changed later for
	//better performance.

	*tree = *Compression::buildHuffmanTree(data, size);

	if(tree == nullptr)
	{
		//error
		return std::vector<unsigned char>();
	}

	//Next, store our values. We need to store the length and code.
	//We will use a table of 256 values and initialize every thing to 0.
	//The first 2 bytes are the length. The next 2 are the code.
	//You shouldn't need more than that. I believe that a short would
	//accomplish the same thing with less storage.

	unsigned int* table = new unsigned int[256];

	if (tree->getRoot()->leftChild == nullptr && tree->getRoot()->rightChild == nullptr)
	{
		//special case
		int finalValue = (1 << 16) + 0;
		table[tree->getRoot()->data.value] = finalValue;
	}
	else
	{
		Compression::fillHuffmanTable(tree->getRoot(), table, 0, 0);
	}

	//Now we just need to replace all of the values with the new codes
	for (int i = 0; i < size; i++)
	{
		int lenCode = table[data[i]];
		int length = (lenCode >> 16) & 0xFFFF;
		int code = lenCode & 0xFFFF;

		output.add(code, length, 32-length);
	}

	return output.toBytes();
}

std::vector<unsigned char> Compression::decompressHuffman(std::vector<unsigned char> data, int messageSize, BinaryTree<HuffmanNode>* tree)
{
	return decompressHuffman(data.data(), data.size(), messageSize, tree);
}

std::vector<unsigned char> Compression::decompressHuffman(unsigned char* data, int size, int messageSize, BinaryTree<HuffmanNode>* tree)
{
	std::vector<unsigned char> output = std::vector<unsigned char>();

	if(size <= 0 || messageSize <= 0)
	{
		#ifdef USE_EXCEPTIONS
		throw InvalidSizeError();
		#endif
		return output;
	}

	if(tree == nullptr)
	{
		#ifdef USE_EXCEPTIONS
		throw HUFFMAN_TREE_ERROR();
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

	BinarySet input = BinarySet();
	input.setBitOrder(BinarySet::LMSB);
	
	//first, fill the BinarySet
	input.setValues(data, size);

	//input.printVals(true);

	int i = input.size() - 1;
	int m = 0;
	BinaryTreeNode<HuffmanNode>* currNode = tree->getRoot();

	if (currNode != nullptr)
	{
		while (m < messageSize || i < 0)
		{
			if (input.getBit(i) == 0)
			{
				//go left
				i--;
				if (currNode->leftChild != nullptr)
				{
					currNode = currNode->leftChild;
				}
				else
				{
					//StringTools::out << "HUFFMAN DECODING ERROR ON TREE: CAN'T GO LEFT AT CURRENT POSITION" << StringTools::lineBreak;
					//StringTools::out << "index: " << (input.size() - i) << StringTools::lineBreak;
					#ifdef USE_EXCEPTIONS
					throw HUFFMAN_TREE_ERROR();
					#endif
					break;
				}
			}
			else
			{
				//go right
				i--;
				if (currNode->rightChild != nullptr)
				{
					currNode = currNode->rightChild;
				}
				else
				{
					//StringTools::out << "HUFFMAN DECODING ERROR ON TREE: CAN'T GO RIGHT AT CURRENT POSITION" << StringTools::lineBreak;
					//StringTools::out << "index: " << (input.size() - i) << StringTools::lineBreak;
					#ifdef USE_EXCEPTIONS
					throw HUFFMAN_TREE_ERROR();
					#endif
					break;
				}
			}

			if (currNode->leftChild == nullptr && currNode->rightChild == nullptr)
			{
				//leaf node
				m++;
				output.push_back(currNode->data.value);
				currNode = tree->getRoot();
			}
		}
	}
	else
	{
		#ifdef USE_EXCEPTIONS
		throw HUFFMAN_TREE_ERROR();
		#endif
		return output;
		//StringTools::out << "HUFFMAN DECODING, TREE ERROR: ROOT IS INVALID" << StringTools::lineBreak;
	}
	
	return output;
}


BinaryTree<HuffmanNode>* Compression::buildHuffmanTreeSubFunc(FrequencyTable<int>* freqTable)
{
	//Next pass is to build the huffmanTree
	//First sort it
	freqTable->sort();
	int fSize = freqTable->size();
	int maxFrequency = freqTable->sumOfFrequencies();

	std::vector<BinaryTreeNode<HuffmanNode>*> nodes = std::vector<BinaryTreeNode<HuffmanNode>*>();

	//LinkedList< BinaryTreeNode<HuffmanNode>* > nodes = LinkedList<BinaryTreeNode<HuffmanNode>*>();

	//Next, make the nodes
	for (int i = 0; i < fSize; i++)
	{
		BinaryTreeNode<HuffmanNode>* test = new BinaryTreeNode<HuffmanNode>();
		test->data = { freqTable->getFrequencyAtLocation(i), freqTable->getValueAtLocation(i) };
		test->leftChild = nullptr;
		test->rightChild = nullptr;

		nodes.push_back(test);
	}

	//Then, group based on min freq.
	//We build these from the bottom up. Eliminating nodes as we go
	//A linked list would be good for this since we can easily
	//delete things and we still have to check each indiviually anyway.

	BinaryTree<HuffmanNode>* huffmanTree = new BinaryTree<HuffmanNode>();

	if(fSize<=1)
	{
		//must be have at least 2 nodes
		BinaryTreeNode<HuffmanNode>* rNode = new BinaryTreeNode<HuffmanNode>();

		rNode->data.frequency = maxFrequency;
		rNode->leftChild = nodes[0];
		rNode->rightChild = nullptr;

		huffmanTree->setRootNode( rNode );
		return huffmanTree;
	}

	while (true)
	{
		//Find the two lowest and group them together as a treeNode.
		BinaryTreeNode<HuffmanNode>* min = nodes[ nodes.size()-1 ];
		BinaryTreeNode<HuffmanNode>* min2 = nodes[ nodes.size()-2 ];
		
		BinaryTreeNode<HuffmanNode>* newTreeNode = new BinaryTreeNode<HuffmanNode>;

		if (min2->leftChild == nullptr && min2->rightChild == nullptr)
		{
			newTreeNode->leftChild = min2;
			newTreeNode->rightChild = min;
		}
		else
		{
			newTreeNode->leftChild = min;
			newTreeNode->rightChild = min2;
		}

		newTreeNode->data.value = 0xFFFFFFFF;
		newTreeNode->data.frequency = min2->data.frequency + min->data.frequency;
		
		//Next, remove the previous mins from the node since they are now
		//combined in a new tree node. Add the new node to the end.
		//Check if we are done
		nodes.pop_back();
		nodes.pop_back();

		nodes.push_back(newTreeNode);
		
		if (newTreeNode->data.frequency == maxFrequency)
		{
			//done
			huffmanTree->setRootNode(newTreeNode);
			break;
		}
		//Next, re-sort the nodes. This is simple since only one node is unsorted
		//and it is the last node that is unsorted.

		Sort::insertionSort<BinaryTreeNode<HuffmanNode>*>(nodes.data(), nodes.size(), [](BinaryTreeNode<HuffmanNode>* a, BinaryTreeNode<HuffmanNode>* b) ->bool
		{
			return (a->data.frequency > b->data.frequency);
		});
	}

	return huffmanTree;
}


BinaryTree<HuffmanNode>* Compression::buildLimitedHuffmanTreeSubFunc(FrequencyTable<int>* freqTable, int maxCodeLength)
{
	//setup
	int totalRepresentable = 1 << maxCodeLength;
	if(freqTable->size() > totalRepresentable)
	{
		//Error. Max Code Length not usable to represent the entire set of values.
		//Throw error if enabled
		return nullptr;
	}

	//Use Package merge with adjusted weights and widths. Done on a different computer.

	std::vector<int> dataValues = std::vector<int>(freqTable->size());
	std::vector<int> codeLengths = std::vector<int>(freqTable->size());

	for(int i=0; i<freqTable->size(); i++)
	{
		dataValues[i] = freqTable->getValueAtLocation(i);
	}

	std::vector<Grouping> nList;
	for(int j=0; j<maxCodeLength; j++)
	{
		for(int i=0; i<freqTable->size(); i++)
		{
			Grouping k;
			k.valid = true;
			k.ids = { i };
			k.weight = freqTable->getFrequencyAtLocation(i);
			k.width = 1;
			k.width <<= (31-j);

			nList.push_back(k);
		}
	}

	size_t val1 = 0x100000000;

	std::vector<Grouping> results = Algorithms::packageMergeAlgorithm(nList, val1 * (freqTable->size()-1)); //copying the results and deleting the list grouping list causes slowness

	for(Grouping& g : results)
	{
		for(int& id : g.ids)
		{
			codeLengths[id]++;
		}
	}
	
	return Compression::buildCanonicalHuffmanTree(dataValues.data(), dataValues.size(), codeLengths.data(), codeLengths.size(), false, true);
}

void Compression::fillHuffmanTable(BinaryTreeNode<HuffmanNode>* treeNode, unsigned int* table, int length, int code)
{
	if (treeNode != nullptr)
	{
		if (treeNode->leftChild != nullptr)
		{
			int newCode = (code << 1);
			//not leaf node
			Compression::fillHuffmanTable(treeNode->leftChild, table, length + 1, newCode);
		}

		if (treeNode->rightChild != nullptr)
		{
			int newCode = (code << 1)+1;
			//not leaf node
			Compression::fillHuffmanTable(treeNode->rightChild, table, length + 1, newCode);
		}

		if (treeNode->leftChild == nullptr && treeNode->rightChild == nullptr)
		{
			//leaf node
			int finalValue = (length << 16) + code;
			table[treeNode->data.value] = finalValue;

			//StringTools::out << treeNode->data.value << ", " << length << ", " << code << StringTools::lineBreak;
			//StringTools::out << table[treeNode->data.value] << StringTools::lineBreak;
		}
	}
}

BinaryTree<HuffmanNode>* Compression::buildCanonicalHuffmanTree(int* dataValues, int sizeOfData, int* lengths, int sizeOfCodeLengths, bool separateCodeLengths, bool reversed)
{
	//We must sort first before building the huffman code
	//Since this is C++ and we can use structures, we can use MergeSort while keeping the
	//correct location for each length and dataValue.

	if(sizeOfData <= 0 || sizeOfCodeLengths <= 0)
	{
		#ifdef USE_EXCEPTIONS
		throw InvalidSizeError();
		#endif
	}

	if(dataValues == nullptr || lengths == nullptr)
	{
		#ifdef USE_EXCEPTIONS
		throw InvalidDataError();
		#endif
	}

	//first, convert
	dataLengthCombo* arr = new dataLengthCombo[sizeOfData];

	if(!separateCodeLengths)
	{
		//There is a one to one pairing for data and lengths.
		//each length refers to how many bits the data at the same spot takes up.
		if(sizeOfData==sizeOfCodeLengths)
		{
			for(int i=0; i<sizeOfData; i++)
			{
				arr[i] = {dataValues[i], lengths[i], 0};
			}
		}
		else
		{
			//invalid size data
			delete[] arr;

			#ifdef USE_EXCEPTIONS
			throw InvalidSizeError();
			#endif

			return nullptr;
		}
	}
	else
	{
		//There is not a one to one pairing of lengths to data.
		//Length specifies how many data values have this length and the order of
		//data values matters in this case. Length is usually less than 16 values.
		int j = 0;
		int leng = 0;
		int i = 0;
		int* actualCodeLengths = new int[sizeOfData];
		int count = 0;
		while(i < sizeOfCodeLengths)
		{
			if(leng == 0)
			{
				leng = lengths[i];
			}

			if(leng>0)
			{
				if(j<sizeOfData)
				{
					count++;
					actualCodeLengths[j] = i;
				}
				else
				{
					//error has occured
					delete[] actualCodeLengths;
					delete[] arr;

					#ifdef USE_EXCEPTIONS
					throw HUFFMAN_CANONICAL_ERROR();
					#endif

					return nullptr;
				}
				j++;
				leng--;
			}
			if(leng<=0)
			{
				i++;
			}
		}
		for(i=0; i<sizeOfData; i++)
		{
			arr[i] = {dataValues[i], actualCodeLengths[i], 0};
		}

		delete[] actualCodeLengths;
	}

	//now sort
	//We use lambda expressions here so we only compare the length values. We could have overriden the comparison
	//operator in dataLengthCombo, but I wanted to use lambda expressions.

	std::function<bool(dataLengthCombo, dataLengthCombo)> compareFunc = [](const dataLengthCombo & a, const dataLengthCombo & b) -> bool { 
		return a.length < b.length;
	};

	Sort::insertionSort<dataLengthCombo>(arr, sizeOfData, compareFunc);
	
	//now, we can start building the codes.
	int startCode = 0;
	int preLength = 0;
	for(int i=0; i<sizeOfData; i++)
	{
		if(arr[i].length>0)
		{
			if(preLength==0)
			{
				preLength = arr[i].length;
			}
			else
			{
				//add 1 to the value of the code.
				startCode++;

				int shiftAmount = arr[i].length-preLength;
				//add zeros to the end of the code
				startCode = startCode << shiftAmount;

				preLength = arr[i].length;
			}

			arr[i].code = startCode;
		}
		
	}

	//now, we build a tree with this information.
	BinaryTree<HuffmanNode>* tree = new BinaryTree<HuffmanNode>();
	tree->setRootNode( new BinaryTreeNode<HuffmanNode>() );
	BinaryTreeNode<HuffmanNode>* currNode = tree->getRoot();

	for(int i=0; i<sizeOfData; i++)
	{

		for(int i2=0; i2<arr[i].length; i2++)
		{
			int v = 0;

			if(reversed==false)
				v = arr[i].code >> (i2) & 0x01;
			else
				v = arr[i].code >> (arr[i].length-1 - i2) & 0x01;
			
			if(v==0)
			{
				//left
				if(currNode->leftChild == nullptr)
					currNode->leftChild = new BinaryTreeNode<HuffmanNode>();

				currNode = currNode->leftChild;
			}
			else
			{
				//right
				if(currNode->rightChild == nullptr)
					currNode->rightChild = new BinaryTreeNode<HuffmanNode>();

				currNode = currNode->rightChild;
			}
			
		}

		currNode->data.frequency=1;
		currNode->data.value = arr[i].data;

		currNode = tree->getRoot();
	}

	delete[] arr;

	return tree;
}


#pragma endregion

#pragma region DEFLATE

//Note that Literals are from 0-255
//Reference Pairs are from 257-285
//EndOfBlock is 256
//286 and 287 never occur

void Compression::compressDeflateSubFunction(unsigned char* data, int size, std::vector<lengthPair>* outputData, int compressionLevel)
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

	//Method 5 - SIMPLE_HASH_MAP : Best Performance and Good Size. Memory Hog. Could potentially crash due to memory constraints.
	//The rehash function should be reworked to use less memory as it basically doubles the memory used. It is also the slowest part I believe.
	
	SimpleHashMap<int, int> map = SimpleHashMap<int, int>( SimpleHashMap<int, int>::MODE_REMOVE_LAST_FROM_BUCKET, 0xFFFF );
	map.setMaxOfSameKey(15);
	
	int i = 0;
	while(i < size-2)
	{
		int key = data[i] + ((int)data[i+1]<<8) + ((int)data[i+2]<<16);
		
		std::vector<HashPair<int, int>*> k = map.getAll(key);

		if(k.size()==0)
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

			for(HashPair<int, int>* it : k)
			{
				int locationOfMatch = it->data;

				if(locationOfMatch < lowestPoint)
				{
					//maximum backwards distance reached
					continue;
				}

				int lengthMax = min(size-i, 258);
				
				unsigned char* startBase = (data+locationOfMatch);
				unsigned char* startMatch = (data+i);

				int len = 3;
				
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

				if(bestLength>=lengthMax)
				{
					break;
				}
			}

			//always insert
			map.add( key, i );

			int backwardsDis = i - bestLocation;
			
			if(bestLength>0)
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

	map.clear();

	#pragma region OLD_CODE
	//Method 4 - HASHED_LINKED_LIST : same as Method 3 but slow delete time causing slower overall time
	
	// //deleting the map is the slowest part.
	// //It for whatever reason takes around 90% of the time just to delete.
	// std::unordered_multimap<int, int> map = std::unordered_multimap<int, int>();

	// time_t findTime = 0;
	// time_t searchTime = 0;
	// time_t insertDNE = 0;
	// time_t insertExists = 0;
	
	// int i = 0;
	// while(i < size-2)
	// {
	// 	int key = data[i] + ((int)data[i+1]<<8) + ((int)data[i+2]<<16);
		
	// 	time_t t1 = System::getCurrentTimeMicro();
	// 	//adjust the find part because it is the second slowest part
	// 	auto k = map.equal_range(key);
	// 	time_t t2 = System::getCurrentTimeMicro();

	// 	findTime += t2-t1;

	// 	if(k.first == k.second)
	// 	{
	// 		//not found

	// 		//always insert
	// 		t1 = System::getCurrentTimeMicro();
	// 		map.insert( {key, i } );
	// 		t2 = System::getCurrentTimeMicro();

	// 		insertDNE += t2-t1;

	// 		outputData->push_back( {true, data[i], 0} );
	// 		i++;
	// 	}
	// 	else
	// 	{
	// 		int lowestPoint = max(i-maxDistance, 0);
	// 		int bestLength = 0;
	// 		int bestLocation = 0;

	// 		t1 = System::getCurrentTimeMicro();

	// 		for(auto it = k.first; it != k.second; it++)
	// 		{
	// 			int locationOfMatch = it->second;

	// 			if(locationOfMatch < lowestPoint)
	// 			{
	// 				//maximum backwards distance reached
	// 				continue;
	// 			}

	// 			int lengthMax = min(size-i, 258);
				
	// 			unsigned char* startBase = (data+locationOfMatch);
	// 			unsigned char* startMatch = (data+i);

	// 			int len = 3;
				
	// 			for(len=3; len<lengthMax; len++)
	// 			{
	// 				if(startMatch[len] != startBase[len])
	// 				{
	// 					break;
	// 				}
	// 			}

	// 			if(len>bestLength)
	// 			{
	// 				bestLength = len;
	// 				bestLocation = locationOfMatch;
	// 			}

	// 			if(bestLength>=lengthMax)
	// 			{
	// 				break;
	// 			}
	// 		}
	// 		t2 = System::getCurrentTimeMicro();

	// 		searchTime += t2-t1;

	// 		//always insert
	// 		t1 = System::getCurrentTimeMicro();
	// 		map.insert( {key, i } );
	// 		t2 = System::getCurrentTimeMicro();

	// 		insertExists += t2-t1;

	// 		int backwardsDis = i - bestLocation;
			
	// 		if(bestLength>0)
	// 		{
	// 			outputData->push_back( {false, bestLength, backwardsDis} );
	// 			i += bestLength;
	// 		}
	// 		else
	// 		{
	// 			//couldn't find match within max allowed distance
	// 			outputData->push_back( {true, data[i], 0} );
	// 			i++;
	// 		}
	// 	}

	// }

	// int remainder = size - i;
	// for(int j=0; j<remainder; j++)
	// {
	// 	outputData->push_back( {true, data[i+j], 0} );
	// }

	// StringTools::println("FindTime: %llu", findTime);
	// StringTools::println("SearchTime: %llu", searchTime);
	// StringTools::println("InsertDNE: %llu", insertDNE);
	// StringTools::println("InsertExists: %llu", insertExists);

	// StringTools::println("SizeOfContainer: %lu", map.size());

	// time_t t1 = System::getCurrentTimeMicro();
	// map.clear();
	// time_t t2 = System::getCurrentTimeMicro();

	// StringTools::println("Time to clear: %lu", t2-t1);

	//Method 2 - Fast, near smallest size
	// std::unordered_map<std::string, int> map = std::unordered_map<std::string, int>();
	// int i = 0;
	// while(i < size)
	// {
	// 	std::string key = "";
	// 	key += data[i]; key += data[i+1]; key += data[i+2];
		
	// 	auto k = map.find(key);

	// 	//always insert / set new data.
		
	// 	map.insert( {key, i} );
	// 	//map[key] = i;

	// 	if(k == map.end())
	// 	{
	// 		//not found
	// 		outputData->push_back( {true, data[i], 0} );
	// 		i++;
	// 		//map.insert( {key, i} );
	// 	}
	// 	else
	// 	{
	// 		//found potential match
	// 		//map[key] = i;

	// 		//lazy matching. If location is outside the bounds allowed by the maximum distance, it no exist
	// 		int locationOfMatch = k->second;
			
	// 		int lowestPoint = max(i-maxDistance, 0);

	// 		if(locationOfMatch < lowestPoint)
	// 		{
	// 			//no exist
	// 			// outputData->push_back( {true, data[i], 0} );
	// 			// i++;
	// 			// continue;
	// 		}
	// 		else
	// 		{
	// 			lowestPoint = locationOfMatch;
	// 		}

	// 		int baseSize = i - lowestPoint;

	// 		int lengthMax = min(size-i, 258);

	// 		unsigned char* startBase = (data+lowestPoint);
	// 		unsigned char* startMatch = (data+i);

	// 		int matchLength = 0;
	// 		int matchStartIndex = 0;

	// 		StringTools::findLongestMatch(startBase, baseSize, startMatch, lengthMax, &matchStartIndex, &matchLength);

	// 		int tempLength = matchLength;
	// 		int tempBackwards = i - (lowestPoint + matchStartIndex);

	// 		if(tempLength<3)
	// 		{
	// 			outputData->push_back( {true, data[i], 0} );
	// 			i++;
	// 		}
	// 		else
	// 		{
	// 			outputData->push_back( {false, tempLength, tempBackwards} );
	// 			i += tempLength;
	// 		}
	// 	}

	// }
	
	//Old Method - smallest size
	// int i = 0;
	// while(i < size)
	// {
	// 	int lowestPoint = max(i-maxDistance, 0);
	// 	int baseSize = i - lowestPoint;

	// 	int lengthMax = min(size-i, 258);

	// 	unsigned char* startBase = (data+lowestPoint);
	// 	unsigned char* startMatch = (data+i);

	// 	int matchLength = 0;
	// 	int matchStartIndex = 0;

	// 	StringTools::findLongestMatch(startBase, baseSize, startMatch, lengthMax, &matchStartIndex, &matchLength);

	// 	int tempLength = matchLength;
	// 	int tempBackwards = i - (lowestPoint + matchStartIndex);

	// 	if(tempLength<3)
	// 	{
	// 		outputData->push_back( {true, data[i], 0} );
	// 		i++;
	// 	}
	// 	else
	// 	{
	// 		outputData->push_back( {false, tempLength, tempBackwards} );
	// 		i += tempLength;
	// 	}
	// }
	#pragma endregion
	
}

void Compression::compressDeflateSubFunction2(std::vector<lengthPair>* block, BinarySet* output, bool dynamic, bool lastBlock)
{
	output->setAddBitOrder(BinarySet::LMSB);
	output->add(lastBlock);

	if(!dynamic)
	{
		output->add(1, 2);

		output->setAddBitOrder(BinarySet::RMSB);
		//should incorporate block ends
		for(int i=0; i<block->size(); i++)
		{
			lengthPair lp = block->at(i);
			if(lp.literal)
			{
				if(lp.v1 <= 143)
				{
					output->add(48 + lp.v1, 8);
				}
				else
				{
					output->add(256 + lp.v1, 9);
				}
			}
			else
			{
				int codeVal = 0;
				int addVal = 0;
				int additionalBits = 0;
				int bitsRequired = 7;
				
				if(lp.v1 <= 10)
				{
					codeVal = lp.v1-2; //codeVal 0 is reserved so start at 1
					additionalBits = 0;
				}
				else if(lp.v1 < 258)
				{
					additionalBits = MathExt::ceil(log2(lp.v1-2)) - 3;
					int expV = (1 << additionalBits);
					double incVal = 1.0 / expV;
					int tempV = 3 + expV*4;
					int add1 = (int) (incVal * (lp.v1 - tempV));

					codeVal = (261 + (4*additionalBits)) + add1;
					addVal = (lp.v1-tempV) % expV;

					if(codeVal < 280)
					{
						codeVal = codeVal - 256;
						bitsRequired = 7;
					}
					else
					{
						codeVal = codeVal - 280 + 0b11000000;
						bitsRequired = 8;
					}
				}
				else if(lp.v1 == 258)
				{
					codeVal = 0b11000101;
					additionalBits = 0;
					bitsRequired = 8;
				}
				
				output->add(codeVal, bitsRequired);
				if(additionalBits>0)
				{
					output->setAddBitOrder(BinarySet::LMSB);
					output->add(addVal, additionalBits);
					output->setAddBitOrder(BinarySet::RMSB);
				}

				//now add the extrabits for the backwards distance.
				//Just like the length stuff above.
				codeVal = 0;
				addVal = 0;
				bitsRequired = 5;
				additionalBits = 0;

				if(lp.v2 <= 4)
				{
					codeVal = lp.v2 - 1; //codeVal 0 is reserved so start at 1
					additionalBits = 0;
				}
				else
				{
					additionalBits = MathExt::ceil(log2(lp.v2)) - 2;
					int expV = (1 << additionalBits);
					double incVal = 1.0 / expV;
					int tempV = 1 + expV*2;
					int add1 = (int) (incVal * (lp.v2 - tempV));

					codeVal = (2 + (2*additionalBits)) + add1;

					addVal = (lp.v2-tempV) % expV;
				}
				
				output->add(codeVal, bitsRequired);
				if(additionalBits>0)
				{
					output->setAddBitOrder(BinarySet::LMSB);
					output->add(addVal, additionalBits);
					output->setAddBitOrder(BinarySet::RMSB);
				}
			}
		}
		
		output->add(0, 7); //end of block
	}
	else
	{
		output->add(2, 2);

		output->setAddBitOrder(BinarySet::RMSB);
		//should incorporate block ends

		//Helpful structure for sorting
		struct HCStruct
		{
			int value;
			int length;
		};

		//generate canonical huffman tables for the literals
		#pragma region GENERATE_CANONICAL_TABLES_FOR_DATA

		std::vector<int> dataToBuildHuffTree = std::vector<int>(block->size()+1);
		std::vector<int> dataToBuildDistHuffTree = std::vector<int>();

		for(int index=0; index<block->size(); index++)
		{
			lengthPair lp = block->at(index);
			if(lp.literal)
			{
				dataToBuildHuffTree[index] = lp.v1;
			}
			else
			{
				if(lp.v1<=10)
					dataToBuildHuffTree[index] = 254 + lp.v1;
				else if(lp.v1<=18)
					dataToBuildHuffTree[index] = 265 + ((lp.v1-11)>>1);
				else if(lp.v1<=34)
					dataToBuildHuffTree[index] = 269 + ((lp.v1-19)>>2);
				else if(lp.v1<=66)
					dataToBuildHuffTree[index] = 273 + ((lp.v1-35)>>3);
				else if(lp.v1<=130)
					dataToBuildHuffTree[index] = 277 + ((lp.v1-67)>>4);
				else if(lp.v1<=257)
					dataToBuildHuffTree[index] = 281 + ((lp.v1-131)>>5);
				else if(lp.v1==258)
					dataToBuildHuffTree[index] = 285;
				
				if(lp.v2<=4)
					dataToBuildDistHuffTree.push_back( lp.v2-1);
				else if(lp.v2<=8)
					dataToBuildDistHuffTree.push_back( 4 + ((lp.v2-5)>>1) );
				else if(lp.v2<=16)
					dataToBuildDistHuffTree.push_back( 6 + ((lp.v2-9)>>2) );
				else if(lp.v2<=32)
					dataToBuildDistHuffTree.push_back( 8 + ((lp.v2-17)>>3) );
				else if(lp.v2<=64)
					dataToBuildDistHuffTree.push_back( 10 + ((lp.v2-33)>>4) );
				else if(lp.v2<=128)
					dataToBuildDistHuffTree.push_back( 12 + ((lp.v2-65)>>5) );
				else if(lp.v2<=256)
					dataToBuildDistHuffTree.push_back( 14 + ((lp.v2-129)>>6) );
				else if(lp.v2<=512)
					dataToBuildDistHuffTree.push_back( 16 + ((lp.v2-257)>>7) );
				else if(lp.v2<=1024)
					dataToBuildDistHuffTree.push_back( 18 + ((lp.v2-513)>>8) );
				else if(lp.v2<=2048)
					dataToBuildDistHuffTree.push_back( 20 + ((lp.v2-1025)>>9) );
				else if(lp.v2<=4096)
					dataToBuildDistHuffTree.push_back( 22 + ((lp.v2-2049)>>10) );
				else if(lp.v2<=8192)
					dataToBuildDistHuffTree.push_back( 24 + ((lp.v2-4097)>>11) );
				else if(lp.v2<=16384)
					dataToBuildDistHuffTree.push_back( 26 + ((lp.v2-8193)>>12) );
				else if(lp.v2<=32768)
					dataToBuildDistHuffTree.push_back( 28 + ((lp.v2-16385)>>13) );
			}
		}
		dataToBuildHuffTree.back() = 256;

		//Check later if can eliminate stuff below to generate canonical version since this is the canonical version.
		BinaryTree<HuffmanNode>* huffTreeLit = Compression::buildHuffmanTree(dataToBuildHuffTree.data(), dataToBuildHuffTree.size(), 15);
		BinaryTree<HuffmanNode>* huffTreeDist = Compression::buildHuffmanTree(dataToBuildDistHuffTree.data(), dataToBuildDistHuffTree.size(), 15);

		int HLIT = 29;
		int HDIST = 29;
		int HCLEN = 15;

		int amountOfLitCodes = 257+HLIT;
		int amountOfDistCodes = 1+HDIST;
		int totalCodes = amountOfLitCodes+amountOfDistCodes;

		HCStruct* litStuff = new HCStruct[amountOfLitCodes];
		int* litDataValues = new int[amountOfLitCodes];
		int* litLengths = new int[amountOfLitCodes];

		HCStruct* distStuff = new HCStruct[amountOfDistCodes];
		int* distDataValues = new int[amountOfDistCodes];
		int* distLengths = new int[amountOfDistCodes];

		memset(litStuff, 0, sizeof(HCStruct)*amountOfLitCodes);
		memset(litDataValues, 0, sizeof(int)*amountOfLitCodes);
		memset(litLengths, 0, sizeof(int)*amountOfLitCodes);

		memset(distStuff, 0, sizeof(HCStruct)*amountOfDistCodes);
		memset(distDataValues, 0, sizeof(int)*amountOfDistCodes);
		memset(distLengths, 0, sizeof(int)*amountOfDistCodes);


		std::vector<TraverseInfo<HuffmanNode>> litTreeTraversal = huffTreeLit->getTraversalInformation(true);
		std::vector<TraverseInfo<HuffmanNode>> distTreeTraversal = huffTreeDist->getTraversalInformation(true);

		for(TraverseInfo<HuffmanNode>& tInfo : litTreeTraversal)
		{
			litStuff[tInfo.node->data.value] = {tInfo.node->data.value, tInfo.path.size()};
		}
	
		Sort::quickSort<HCStruct>(litStuff, amountOfLitCodes, [](HCStruct a, HCStruct b) -> bool{
			if(a.length!=b.length)
				return a.length < b.length;
			else
				return a.value < b.value;
		});

		for(int i=0; i<amountOfLitCodes; i++)
		{
			litDataValues[i] = litStuff[i].value;
			litLengths[i] = litStuff[i].length;
		}

		for(TraverseInfo<HuffmanNode>& tInfo : distTreeTraversal)
		{
			distStuff[tInfo.node->data.value] = {tInfo.node->data.value, tInfo.path.size()};
		}

		Sort::quickSort<HCStruct>(distStuff, amountOfDistCodes, [](HCStruct a, HCStruct b) -> bool{
			if(a.length!=b.length)
				return a.length < b.length;
			else
				return a.value < b.value;
		});

		for(int i=0; i<amountOfDistCodes; i++)
		{
			distDataValues[i] = distStuff[i].value;
			distLengths[i] = distStuff[i].length;
		}

		
		//generate canonical huffman tables for the distance values
		BinaryTree<HuffmanNode>* litCanonicalTree = Compression::buildCanonicalHuffmanTree(litDataValues, amountOfLitCodes, litLengths, amountOfLitCodes, false, true);
		BinaryTree<HuffmanNode>* distCanonicalTree = Compression::buildCanonicalHuffmanTree(distDataValues, amountOfDistCodes, distLengths, amountOfDistCodes, false, true);

		//may not need litData and the other arrays
		delete[] litStuff;
		delete[] litDataValues;
		delete[] litLengths;

		delete[] distStuff;
		delete[] distDataValues;
		delete[] distLengths;

		delete huffTreeLit;
		delete huffTreeDist;
		
		#pragma endregion

		#pragma region GENERATE_CANONICAL_TABLE_FOR_HC_RECREATION

		std::vector<TraverseInfo<HuffmanNode>> nLitCanonicalTraversal = litCanonicalTree->getTraversalInformation(true);
		std::vector<TraverseInfo<HuffmanNode>> nDistCanonicalTraversal = distCanonicalTree->getTraversalInformation(true);

		int* allCodeLengths = new int[totalCodes];
		memset(allCodeLengths, 0, sizeof(int)*totalCodes);

		//lazy method of putting all literals
		for(TraverseInfo<HuffmanNode>& tInfo : nLitCanonicalTraversal)
		{
			allCodeLengths[ tInfo.node->data.value ] = tInfo.path.size();
		}

		for(TraverseInfo<HuffmanNode>& tInfo : nDistCanonicalTraversal)
		{
			allCodeLengths[ amountOfLitCodes + tInfo.node->data.value ] = tInfo.path.size();
		}

		//Now we have an array of all the possible values that can appear with their code lengths
		//Next, we generate a huffman tree with this data, and record the code lengths, then put them into a special array,
		//sort, and generate a canonical huffman tree.

		BinaryTree<HuffmanNode>* codeRecreateTree = Compression::buildHuffmanTree(allCodeLengths, totalCodes, 7);

		//Get code Lengths
		std::vector<TraverseInfo<HuffmanNode>> codeRecreateTreeInfo = codeRecreateTree->getTraversalInformation(true);

		//now form the HCLength stuff. Lazy method of not using the codes 16,17,18
		HCStruct* hcStuff = new HCStruct[19]{ {16,0}, {17,0}, {18,0}, {0,0}, {8,0}, {7,0}, {9,0}, {6,0}, {10,0}, {5,0}, {11,0}, {4,0}, {12,0}, {3,0}, {13,0}, {2,0}, {14,0}, {1,0}, {15,0} };
		int indexingArray[19] = {3,17,15,13,11,9,7,5,4,6,8,10,12,14,16,18,0,1,2};
		int hcValues[19];
		int hcLengths[19];
		for(TraverseInfo<HuffmanNode>& tInfo : codeRecreateTreeInfo)
		{
			int codeLengthForBigTree = tInfo.node->data.value;
			int codeLengthForHCTree = tInfo.path.size();
			
			int index = indexingArray[codeLengthForBigTree];
			hcStuff[index].length = codeLengthForHCTree;
		}

		Sort::quickSort<HCStruct>(hcStuff, 19, [](HCStruct a, HCStruct b) -> bool{
			if(a.length!=b.length)
				return a.length < b.length;
			else
				return a.value < b.value;
		});

		for(int i=0; i<19; i++)
		{
			hcValues[i] = hcStuff[i].value;
			hcLengths[i] = hcStuff[i].length;
		}

		BinaryTree<HuffmanNode>* tempTree = Compression::buildCanonicalHuffmanTree(hcValues, 19, hcLengths, 19, false, true);

		std::vector<TraverseInfo<HuffmanNode>> hcTraversalInfo = tempTree->getTraversalInformation(true);

		//write HLIT, HDIST, HCLEN
		output->setAddBitOrder(BinarySet::LMSB);

		output->add(HLIT, 5);
		output->add(HDIST, 5);
		output->add(HCLEN, 4);
		
		
		//write hcStuff. 3 bits per value. In the original order without sorting
		BinarySet* hcLengthsUnSorted = new BinarySet[19];
		for(TraverseInfo<HuffmanNode>& tInfo : hcTraversalInfo)
		{
			hcLengthsUnSorted[ indexingArray[tInfo.node->data.value] ] = tInfo.path;
		}

		for(int i=0; i<19; i++)
		{
			output->add(hcLengthsUnSorted[i].size(), 3); //ERROR HERE. TRYING TO RECORD VALUE OF 8 WHICH IS IMPOSSIBLE
		}
		
		for(int i=0; i<totalCodes; i++)
		{
			int index = indexingArray[allCodeLengths[i]];
			output->add( hcLengthsUnSorted[index] );
		}
		
		output->setAddBitOrder(BinarySet::RMSB);
		
		delete[] allCodeLengths;
		delete[] hcLengthsUnSorted;
		delete codeRecreateTree;

		delete[] hcStuff;
		delete tempTree;


		#pragma endregion
		
		#pragma region ADD_DATA_USING_TREES

		//add data
		for(int i=0; i<block->size(); i++)
		{
			lengthPair lp = block->at(i);

			if(lp.literal)
			{
				//find value in tree info
				bool found = false;
				for(TraverseInfo<HuffmanNode>& n : nLitCanonicalTraversal)
				{
					if(lp.v1 == n.node->data.value)
					{
						output->add( n.path );
						found = true;
						break;
					}
				}
				
				if(found==false)
				{
					//error
					StringTools::println("Error finding literal %d in traversal information.", lp.v1);
				}
			}
			else
			{
				int codeWord = -1;
				int additionalBits = 0;
				int addVal = 0;
				int adjustedVal = 0;

				if(lp.v1<=10)
				{
					adjustedVal = lp.v1;
					additionalBits = 0;
					addVal = 0;
					codeWord = 254 + adjustedVal;
				}
				else if(lp.v1<=18)
				{
					additionalBits = 1;
					adjustedVal = (lp.v1-11) >> additionalBits;
					addVal = (lp.v1-11) % 2;
					codeWord = 265 + adjustedVal;
				}
				else if(lp.v1<=34)
				{
					additionalBits = 2;
					adjustedVal = (lp.v1-19) >> additionalBits;
					addVal = (lp.v1-19) % 4;
					codeWord = 269 + adjustedVal;
				}
				else if(lp.v1<=66)
				{
					additionalBits = 3;
					adjustedVal = (lp.v1-35) >> additionalBits;
					addVal = (lp.v1-35) % 8;
					codeWord = 273 + adjustedVal;
				}
				else if(lp.v1<=130)
				{
					additionalBits = 4;
					adjustedVal = (lp.v1-67) >> additionalBits;
					addVal = (lp.v1-67) % 16;
					codeWord = 277 + adjustedVal;
				}
				else if(lp.v1<=257)
				{
					additionalBits = 5;
					adjustedVal = (lp.v1-131) >> additionalBits;
					addVal = (lp.v1-131) % 32;
					codeWord = 281 + adjustedVal;
				}
				else if(lp.v1==258)
				{
					additionalBits = 0;
					addVal = 0;
					codeWord = 285;
				}
				else
					StringTools::println("ERROR: Length is too long");

				bool found = false;
				for(TraverseInfo<HuffmanNode>& n : nLitCanonicalTraversal)
				{
					if(codeWord == n.node->data.value)
					{
						// StringTools::println("Writing Literal %d with value %s", codeWord, StringTools::toBinaryString(n.path.toBytes()[0], n.path.size()));
						output->add( n.path );
						found = true;
						break;
					}
				}

				if(additionalBits>0)
				{
					// StringTools::println("Writing additional bits with value %s", StringTools::toBinaryString(addVal, additionalBits));
						
					output->setAddBitOrder(BinarySet::LMSB);
					output->add(addVal, additionalBits);
					output->setAddBitOrder(BinarySet::RMSB);
				}
				
				if(found==false)
				{
					//error
					StringTools::println("Error finding Length %d with code %d in traversal information.", lp.v1, codeWord);
				}
				
				found = false;
				codeWord = -1;
				
				if(lp.v2<=4)
				{
					additionalBits = 0;
					addVal = 0;
					codeWord = lp.v2-1;
				}
				else if(lp.v2<=32768)
				{
					additionalBits = (int)MathExt::ceil(MathExt::log(lp.v2, 2.0)-2);
					int subVal = 1 + (1<<(additionalBits+1));
					int expVal = 1 << additionalBits;

					adjustedVal = (lp.v2-subVal) >> additionalBits;
					addVal = (lp.v2-subVal) % expVal;
					codeWord = (1+additionalBits)*2 + adjustedVal;
				}
				else
					StringTools::println("ERROR: backwards distance too long");

				for(TraverseInfo<HuffmanNode>& n : nDistCanonicalTraversal)
				{
					if(codeWord == n.node->data.value)
					{
						// StringTools::println("Writing Distance %d with value %s", codeWord, StringTools::toBinaryString(n.path.toBytes()[0], n.path.size()));
						output->add( n.path );
						found = true;
						break;
					}
				}

				if(additionalBits>0)
				{
					// StringTools::println("Writing additional bits with value %s", StringTools::toBinaryString(addVal, additionalBits));
					output->setAddBitOrder(BinarySet::LMSB);
					output->add(addVal, additionalBits);
					output->setAddBitOrder(BinarySet::RMSB);
				}
				
				if(found==false)
				{
					//error
					StringTools::println("Error finding Distance %d with code %d in traversal information.", lp.v2, codeWord);
				}
			}
		}

		//add end of block.
		bool found = false;
		for(TraverseInfo<HuffmanNode>& n : nLitCanonicalTraversal)
		{
			if(256 == n.node->data.value)
			{
				// StringTools::println("Writing %d with value %s", 256, StringTools::toBinaryString(n.path.toBytes()[0], n.path.size()));
				output->add( n.path );
				found = true;
				break;
			}
		}

		if(found == false)
		{
			//error
			StringTools::println("Error finding value 256 to end the block. Invalid deflate stream.");
		}

		delete litCanonicalTree;
		delete distCanonicalTree;

		#pragma endregion

	}
	output->setAddBitOrder(BinarySet::LMSB);
}


std::vector<unsigned char> Compression::compressDeflate(std::vector<unsigned char> data, int blocks, int compressionLevel, bool customTable)
{
	return Compression::compressDeflate(data.data(), data.size(), blocks, compressionLevel, customTable);
}

std::vector<unsigned char> Compression::compressDeflate(unsigned char* data, int size, int blocks, int compressionLevel, bool customTable)
{
	//probably will only use default trees and stuff

	if(data == nullptr)
	{
		#ifdef USE_EXCEPTIONS
		throw InvalidDataError();
		#endif
		return std::vector<unsigned char>();
	}

	if(size <=0 || blocks <=0 )
	{
		#ifdef USE_EXCEPTIONS
		throw InvalidSizeError();
		#endif
		return std::vector<unsigned char>();
	}
	
	int tSize = System::getNumberOfThreads();
	std::vector<std::vector<lengthPair>> info = std::vector<std::vector<lengthPair>>(blocks);
	std::vector<std::thread> threads = std::vector<std::thread>( tSize );

	BinarySet bin = BinarySet();

	bin.setBitOrder(BinarySet::LMSB);
	bin.setAddBitOrder(BinarySet::RMSB);

	int sizeOfBlock = size/blocks;

	for(int block=0; block<blocks; block++)
	{
		int nSize = sizeOfBlock;
		unsigned char* nData = data + block*sizeOfBlock;
		if(block == blocks-1)
		{
			nSize = size - (block*sizeOfBlock);
		}

		if(block<tSize)
		{
			info[block].clear();
			threads[ block%tSize ] = std::thread(compressDeflateSubFunction, nData, nSize, &info[block], compressionLevel);
		}
		else
		{
			if(threads[block%tSize].joinable())
				threads[block%tSize].join();

			info[block].clear();
			threads[ block%tSize ] = std::thread(compressDeflateSubFunction, nData, nSize, &info[block], compressionLevel);
		}
	}

	for(int t=0; t<tSize; t++)
	{
		if(threads[t].joinable())
			threads[t].join();
	}

	std::vector<BinarySet> threadBinData = std::vector<BinarySet>(blocks);

	for(int block=0; block<blocks; block++)
	{
		if(threads[block%tSize].joinable())
			threads[block%tSize].join();
			
		threads[ block%tSize ] = std::thread(compressDeflateSubFunction2, &info[block], &threadBinData[block], customTable, block == (blocks-1) );
	}
	for(int t=0; t<tSize; t++)
	{
		if(threads[t].joinable())
			threads[t].join();
	}

	// for(int block=0; block<blocks; block++)
	// {
	// 	StringTools::println("BLOCK = %d", block);
	// 	compressDeflateSubFunction2(&info[block], &threadBinData[block], customTable, block == (blocks-1)); //for debugging purposes
	// 	StringTools::println("");
	// }

	//Construct
	for(int block=0; block<blocks; block++)
	{
		bin.add( threadBinData[block] );
	}

	bin.setAddBitOrder(BinarySet::LMSB);
	
	return bin.toBytes();
}

std::vector<unsigned char> Compression::decompressDeflate(std::vector<unsigned char> data)
{
	return Compression::decompressDeflate(data.data(), data.size());
}

std::vector<unsigned char> Compression::decompressDeflate(unsigned char* data, int size)
{
	//determine if it is case 0, 1, or 2
	//if case 0, or 1, we can deal with it
	//case 2 will come later
	//case 3 is not valid

	//general rule, go down huffmanTree till you hit a valid value
	//if value is < 256, it is a literal
	//if value is == 256, stop
	//if value is > 256, it is a reference
		//if reference, refer to the table on how to
		//deal with references
	
	//first, get everything into a BinarySet
	//load it in reverse order so we may read with no
	//jumping around

	if(data == nullptr)
	{
		#ifdef USE_EXCEPTIONS
		throw InvalidDataError();
		#endif
		return std::vector<unsigned char>();
	}

	if(size <= 0)
	{
		#ifdef USE_EXCEPTIONS
		throw InvalidSizeError();
		#endif
		return std::vector<unsigned char>();
	}

	std::vector<unsigned char> finalData = std::vector<unsigned char>();

	BinarySet binData = BinarySet();
	
	//set data in binarySet
	binData.setBitOrder(BinarySet::LMSB);
	binData.setValues(data, size);

	//StringTools::out << "BinSize " << binData.size() << StringTools::lineBreak;

	int currLoc = 0;

	//always create default tree
	BinaryTree<HuffmanNode>* defaultTree = buildDeflateDefaultTree();

	while(currLoc<binData.size())
	{
		bool lastBlock = false;
		bool blockEnded = false;
		int mode = -1;

		//first bit of a block tells if this is the last block
		if(binData.getBit(currLoc)==0)
		{
			//not last block
			//StringTools::out << "Not Last block" << StringTools::lineBreak;
		}
		else
		{
			//last block
			lastBlock = true;
			//StringTools::out << "Last block" << StringTools::lineBreak;
		}
		currLoc++;

		if(binData.getBit(currLoc)==0)
		{
			if(binData.getBit(currLoc+1)==0)
			{
				//no compression on this block
				mode = 0;
			}
			else
			{
				//custom compression on this block
				//StringTools::out << "Custom Compression block" << StringTools::lineBreak;
				mode = 2;
			}
		}
		else
		{
			if(binData.getBit(currLoc+1)==0)
			{
				//default compression on this block
				mode = 1;
			}
			else
			{
				//invalid option
				mode = 3;

				#ifdef USE_EXCEPTIONS
				throw Compression::DEFLATE_INVALID_MODE();
				#endif

				return finalData;
			}
		}
		currLoc += 2;
		
		if(mode==0)
		{
			while(blockEnded==false)
			{
				//just literals
				//skip bits to next byte
				//2 bytes for length
				//2 bytes for length complement
				//data
				int amountToSkip = 8 - (currLoc % 8);
				if(amountToSkip!=8)
				{
					currLoc+=amountToSkip;
				}
				
				//find the byte location of the currentLocation
				//remember that currLoc represent the current bit and not byte.
				//also, the bytes have been loaded in reverse order for the binary set,
				//but that doesn't affect much.
				int byteLocation = (currLoc/8);
				
				//next, read 2 bytes for the length
				//then 2 bytes for the complement of the length
				//remember to adjust the location for the bits as well.
				int length = (int)data[byteLocation] + ((int)data[byteLocation+1]<<8);
				int nlength = (int)data[byteLocation+2] + ((int)data[byteLocation+3]<<8);
				byteLocation+=4;
				currLoc+=8*4;
				
				//now add the non-compressed literal data to the final data
				//and adjust the location for the bits.
				for(int i=0; i<length; i++)
				{
					finalData.push_back(data[byteLocation + i]);
				}
				
				currLoc+=8*length;

				blockEnded=true;
				break;
			}
		}
		else
		{
			BinaryTree<HuffmanNode>* mTree = nullptr;
			BinaryTree<HuffmanNode>* backTree = nullptr;
			BinaryTree<HuffmanNode>** dynTrees = nullptr;

			if(mode==1)
			{
				//make the default tree
				//StringTools::out << "Making Default tree" << StringTools::lineBreak;
				mTree = defaultTree;
			}
			else
			{
				//make the dynamic tree
				//StringTools::out << "Making dynamic tree" << StringTools::lineBreak;
				
				dynTrees = Compression::buildDynamicDeflateTree(&binData, &currLoc);
				if(dynTrees!=nullptr)
				{
					mTree = dynTrees[0];
					backTree = dynTrees[1];
				}
				else
				{
					//StringTools::out << "Error creating dynamic tree" << StringTools::lineBreak;
					break;
				}
				//StringTools::out << "Finished Tree" << StringTools::lineBreak;
			}
			
			BinaryTreeNode<HuffmanNode>* currNode = mTree->getRoot();
			bool hitGood = false;

			while(blockEnded==false)
			{
				if(hitGood!=true)
				{
					//traverse per bit

					if(binData.getBit(currLoc) == 0)
					{
						// StringTools::print("0");
						currNode = currNode->leftChild;
					}
					else
					{
						// StringTools::print("1");
						currNode = currNode->rightChild;
					}

					if (currNode != nullptr)
					{
						if (currNode->leftChild == nullptr && currNode->rightChild == nullptr)
						{
							//must be the correct value
							//StringTools::out << "Hit value with value - " << currNode->data.value << StringTools::lineBreak;
							/*
							StringTools::out << "Hit value with value - " << currNode->data.value << ", and bitvalue - ";
							test.printVals();
							*/

							hitGood = true;

						}
						currLoc += 1;
					}
					else
					{
						//error

						if(mTree!=nullptr)
							delete mTree;
						if(backTree!=nullptr)
							delete backTree;
						
						if(dynTrees!=nullptr)
							delete[] dynTrees;

						#ifdef USE_EXCEPTIONS
						throw Compression::HUFFMAN_TREE_ERROR();
						#endif
						
						return finalData;
					}
				}
				else
				{
					if(currNode->data.value <= 255)
					{
						//literal
						// StringTools::println(" Got literal with value %d", currNode->data.value);
						//StringTools::out << "Literal" << StringTools::lineBreak;
						finalData.push_back(currNode->data.value);
					}
					else if(currNode->data.value == 256)
					{
						//end of block
						//StringTools::out << "End of block" << StringTools::lineBreak;
						blockEnded = true;
						break;
					}
					else
					{
						//StringTools::out << "Reference Pair" << StringTools::lineBreak;
						
						//reference pair
						int baseCopyLength = 0;
						int copyExtraBits = 0;
						Compression::getCopyLengthInformation(currNode->data.value, &baseCopyLength, &copyExtraBits);

						
						//get the value of the extra bits
						int copyExtraVal = binData.getBits(currLoc, currLoc+copyExtraBits);
						currLoc+=copyExtraBits;

						int finalCopyLength = baseCopyLength + copyExtraVal;

						
						// StringTools::println(" Got length with code %d and final length %d", currNode->data.value, finalCopyLength);

						int backRefCode = 0;
						int baseBackLength = 0;
						int backRefExtraBits = 0;

						//read next 5 bits to get back ref
						//Note that this only works for default compression

						if(backTree==nullptr)
						{
							backRefCode = binData.getBits(currLoc, currLoc+5, true);
							currLoc+=5;
						}
						else
						{
							//traverse the back tree to get the backRefCode
							BinaryTreeNode<HuffmanNode>* backNode = backTree->getRoot();

							while(true)
							{
								if(binData.getBit(currLoc)==false)
								{
									backNode = backNode->leftChild;
								}
								else
								{
									backNode = backNode->rightChild;
								}
								currLoc+=1;

								if(backNode->leftChild==nullptr && backNode->rightChild==nullptr)
								{
									//hit a value;
									backRefCode = backNode->data.value;
									break;
								}
							}
						}
						
						Compression::getBackDistanceInformation(backRefCode, &baseBackLength, &backRefExtraBits);

						//get the value of the extra bits
						int backExtraVal = binData.getBits(currLoc, currLoc+backRefExtraBits);
						currLoc+=backRefExtraBits;
						int finalBackRef = baseBackLength + backExtraVal;

						// StringTools::println(" Got Distance with code %d and final length %d", backRefCode, finalBackRef);

						
						//StringTools::out << "Length is: " << finalCopyLength << ", Dist is: " << finalBackRef << StringTools::lineBreak;
						//with finalCopyLength and finalBackRef, copy into the finalData
						int startPos = finalData.size()-finalBackRef;

						for(int i=0; i<finalCopyLength; i++)
						{
							finalData.push_back( finalData[startPos+i] );
						}
						
					}
					
					hitGood = false;
					currNode = mTree->getRoot();
				}
				
			}

			if(mode == 1)
			{
				mTree = nullptr;
			}

			if(mTree!=nullptr)
				delete mTree;
			if(backTree!=nullptr)
				delete backTree;
			
			if(dynTrees!=nullptr)
				delete[] dynTrees;

			mTree = nullptr;
			backTree = nullptr;
			dynTrees = nullptr;
				
			if(lastBlock)
			{
				break;
			}
		}
		
	}

	delete defaultTree;
	defaultTree = nullptr;

	return finalData;
}

BinaryTree<HuffmanNode>* Compression::buildDeflateDefaultTree()
{
	BinaryTree<HuffmanNode>* tree = new BinaryTree<HuffmanNode>();
	
	tree->setRootNode( new BinaryTreeNode<HuffmanNode>());

	//fill tree to each point making sure to only
	//add what is necessary

	//values from 0-143 : 8bits : starts at 00110000
	int startVal = 0b00110000;

	for(int values = 0; values<=143; values++)
	{
		BinaryTreeNode<HuffmanNode>* currNode = tree->getRoot();

		for(int startBit=0; startBit<8; startBit++)
		{
			bool side = ((startVal >> (7-startBit)) & 0x01)==1;
			if(side == false)
			{
				//left
				if(currNode->leftChild == nullptr)
					currNode->leftChild = new BinaryTreeNode<HuffmanNode>();
				
				currNode = currNode->leftChild;
			}
			else
			{
				//right
				if(currNode->rightChild == nullptr)
					currNode->rightChild = new BinaryTreeNode<HuffmanNode>();
				
				currNode = currNode->rightChild;
			}
		}
		currNode->data.value = values;
		
		startVal++;
	}

	//values from 144-255 : 9bits : starts at 110010000
	startVal = 0b110010000;

	for(int values = 144; values<=255; values++)
	{
		BinaryTreeNode<HuffmanNode>* currNode = tree->getRoot();

		for(int startBit=0; startBit<9; startBit++)
		{
			bool side = ((startVal >> (8-startBit)) & 0x01)==1;
			if(side == false)
			{
				//left
				if(currNode->leftChild == nullptr)
					currNode->leftChild = new BinaryTreeNode<HuffmanNode>();
				
				currNode = currNode->leftChild;
			}
			else
			{
				//right
				if(currNode->rightChild == nullptr)
					currNode->rightChild = new BinaryTreeNode<HuffmanNode>();
				
				currNode = currNode->rightChild;
			}
		}
		currNode->data.value = values;
		startVal++;
	}

	//values from 256-279 : 7bits : starts at 0000000
	startVal = 0b0000000;

	for(int values = 256; values<=279; values++)
	{
		BinaryTreeNode<HuffmanNode>* currNode = tree->getRoot();

		for(int startBit=0; startBit<7; startBit++)
		{
			bool side = ((startVal >> (6-startBit)) & 0x01)==1;
			if(side == false)
			{
				//left
				if(currNode->leftChild == nullptr)
					currNode->leftChild = new BinaryTreeNode<HuffmanNode>();
				
				currNode = currNode->leftChild;
			}
			else
			{
				//right
				if(currNode->rightChild == nullptr)
					currNode->rightChild = new BinaryTreeNode<HuffmanNode>();
				
				currNode = currNode->rightChild;
			}
		}
		currNode->data.value = values;
		startVal++;
	}

	//values from 280-287 : 8bits : starts at 11000000
	startVal = 0b11000000;

	for(int values = 280; values<=287; values++)
	{
		BinaryTreeNode<HuffmanNode>* currNode = tree->getRoot();

		for(int startBit=0; startBit<8; startBit++)
		{
			bool side = ((startVal >> (7-startBit)) & 0x01)==1;
			if(side == false)
			{
				//left
				if(currNode->leftChild == nullptr)
					currNode->leftChild = new BinaryTreeNode<HuffmanNode>();
				
				currNode = currNode->leftChild;
			}
			else
			{
				//right
				if(currNode->rightChild == nullptr)
					currNode->rightChild = new BinaryTreeNode<HuffmanNode>();
				
				currNode = currNode->rightChild;
			}
		}
		currNode->data.value = values;
		startVal++;
	}

	return tree;
}

BinaryTree<HuffmanNode>** Compression::buildDynamicDeflateTree(BinarySet* data, int* location)
{
	if(data == nullptr || location == nullptr)
	{
		#ifdef USE_EXCEPTIONS
		throw InvalidDataError();
		#endif
		return nullptr;
	}

	BinaryTree<HuffmanNode>** trees = new BinaryTree<HuffmanNode> * [2]{};

	int HLIT = data->getBits(*location, *location+5);
	*location+=5;
	int HDIST = data->getBits(*location, *location+5);
	*location+=5;
	int HCLEN = data->getBits(*location, *location+4);
	*location+=4;

	int amountOfLitCodes = 257+HLIT;
	int amountOfDistCodes = 1+HDIST;
	int amountOfCodeLengthCodes = 4+HCLEN;
	int totalCodes = amountOfDistCodes+amountOfLitCodes;

	//we must build up the codes to find the codes
	//The order of the values are defined in the deflate specs
	//Do some canonical huffman coding stuff.

	struct HCStruct
	{
		int value;
		int length;
	};
	
	HCStruct* hcStuff = new HCStruct[19]{ {16,0}, {17,0}, {18,0}, {0,0}, {8,0}, {7,0}, {9,0}, {6,0}, {10,0}, {5,0}, {11,0}, {4,0}, {12,0}, {3,0}, {13,0}, {2,0}, {14,0}, {1,0}, {15,0} };

	int hcValues[19];
	int hcLengths[19];

	for(int i=0; i<amountOfCodeLengthCodes; i++)
	{
		hcStuff[i].length = data->getBits(*location, *location+3);
		*location+=3;
	}
	
	Sort::insertionSort<HCStruct>(hcStuff, amountOfCodeLengthCodes, [](HCStruct a, HCStruct b) -> bool{
		if(a.length!=b.length)
			return a.length < b.length;
		else
			return a.value < b.value;
	});

	for(int i=0; i<amountOfCodeLengthCodes; i++)
	{
		hcValues[i] = hcStuff[i].value;
		hcLengths[i] = hcStuff[i].length;
	}
	
	BinaryTree<HuffmanNode>* tempTree = Compression::buildCanonicalHuffmanTree(hcValues, amountOfCodeLengthCodes, hcLengths, amountOfCodeLengthCodes, false, true);

	//next, we use this tree to find the code lengths for our data set.
	//Due to how we get the codes, we store them in a single array at first.
	//Since this is c++, we don't have to explicitly separate our array. Just set
	//the start point and the length.

	delete[] hcStuff;
	hcStuff = new HCStruct[totalCodes];
	std::memset(hcStuff, 0, totalCodes*sizeof(HCStruct));

	BinaryTreeNode<HuffmanNode>* currNode = tempTree->getRoot();

	int codes = 0;
	int tempCode = 0;
	
	while(codes < totalCodes)
	{
		tempCode = (data->getBit(*location)==true) ? 1 : 0;
		*location+=1;

		if(tempCode==0)
		{
			currNode = currNode->leftChild;
		}
		else
		{
			currNode = currNode->rightChild;
		}

		if(currNode->leftChild == nullptr && currNode->rightChild == nullptr)
		{
			//hit a value
			int actualValue = currNode->data.value;
			
			if(actualValue < 16)
			{
				hcStuff[codes].length = actualValue;
				codes++;
			}
			else if(actualValue==16)
			{
				//copy previous value x times
				int extra = data->getBits(*location, *location+2);
				*location+=2;

				int copyLen = extra + 3;

				int preVal = hcStuff[codes-1].length;
				for(int i=0; i<copyLen; i++)
				{
					hcStuff[codes+i].length = preVal;
				}
				codes+=copyLen;
			}
			else if(actualValue==17)
			{
				//copy 0 x times
				int extra = data->getBits(*location, *location+3);
				*location+=3;

				int copyLen = extra + 3;

				for(int i=0; i<copyLen; i++)
				{
					hcStuff[codes+i].length = 0;
				}
				codes+=copyLen;
			}
			else if(actualValue==18)
			{
				//copy 0 x times
				int extra = data->getBits(*location, *location+7);
				*location+=7;

				int copyLen = extra + 11;

				for(int i=0; i<copyLen; i++)
				{
					if(codes+i < totalCodes)
						hcStuff[codes+i].length = 0;
				}
				codes+=copyLen;
			}

			currNode = tempTree->getRoot();
		}
	}

	//requirements
	//the value 256 must have a valid code
	if(hcStuff[256].length == 0)
	{
		if(hcStuff!=nullptr)
			delete[] hcStuff;
		if(tempTree!=nullptr)
			delete tempTree;
		if(trees!=nullptr)
		{
			if(trees[0]!=nullptr)
				delete trees[0];
			if(trees[1]!=nullptr)
				delete trees[1];
			
			delete[] trees;
		}
			
		return nullptr;
	}

	for(int i=0; i<amountOfLitCodes; i++)
	{
		hcStuff[i].value = i;
	}
	for(int i=0; i<amountOfDistCodes; i++)
	{
		hcStuff[i+amountOfLitCodes].value = i;
	}

	Sort::insertionSort<HCStruct>(&hcStuff[0], amountOfLitCodes, [](HCStruct a, HCStruct b) -> bool{
		if(a.length!=b.length)
			return a.length < b.length;
		else
			return a.value < b.value;
	});

	Sort::insertionSort<HCStruct>(&hcStuff[amountOfLitCodes], amountOfDistCodes, [](HCStruct a, HCStruct b) -> bool{
		if(a.length!=b.length)
			return a.length < b.length;
		else
			return a.value < b.value;
	});

	int* allCodes = new int[totalCodes];
	int* lenLengthValues = new int[amountOfLitCodes];
	int* distValues = new int[amountOfDistCodes];

	for(int i=0; i<totalCodes; i++)
	{
		allCodes[i] = hcStuff[i].length;
	}
	for(int i=0; i<amountOfLitCodes; i++)
	{
		lenLengthValues[i] = hcStuff[i].value;
	}
	for(int i=0; i<amountOfDistCodes; i++)
	{
		distValues[i] = hcStuff[i+amountOfLitCodes].value;
	}

	delete[] hcStuff;

	trees[0] = Compression::buildCanonicalHuffmanTree(lenLengthValues, amountOfLitCodes, &allCodes[0], amountOfLitCodes, false, true);
	trees[1] = Compression::buildCanonicalHuffmanTree(distValues, amountOfDistCodes, &allCodes[amountOfLitCodes], amountOfDistCodes, false, true);

	if(lenLengthValues!=nullptr)
		delete[] lenLengthValues;
	if(distValues!=nullptr)
		delete[] distValues;
	if(allCodes!=nullptr)
		delete[] allCodes;

	if(tempTree!=nullptr)
		delete tempTree;

	return trees;
}

void Compression::getCopyLengthInformation(int code, int* baseValue, int* extraBits)
{
	if(baseValue == nullptr || extraBits == nullptr)
	{
		#ifdef USE_EXCEPTIONS
		throw InvalidDataError();
		#endif
		return;
	}

	int loc = code-257;
	if(loc<=7)
	{
		*extraBits = 0;
		*baseValue = 3+loc;
	}
	else if(loc<=11)
	{
		*extraBits = 1;
		*baseValue = 11 + (loc-8)*(std::pow(2, *extraBits));
	}
	else if(loc<=15)
	{
		*extraBits = 2;
		*baseValue = 19 + (loc-12)*(std::pow(2, *extraBits));
	}
	else if(loc<=19)
	{
		*extraBits = 3;
		*baseValue = 35 + (loc-16)*(std::pow(2, *extraBits));
	}
	else if(loc<=23)
	{
		*extraBits = 4;
		*baseValue = 67 + (loc-20)*(std::pow(2, *extraBits));
	}
	else if(loc<=27)
	{
		*extraBits = 5;
		*baseValue = 131 + (loc-24)*(std::pow(2, *extraBits));
	}
	else if(loc==28)
	{
		*extraBits = 0;
		*baseValue = 258;
	}
}

void Compression::getBackDistanceInformation(int code, int* baseValue, int* extraBits)
{
	if(baseValue == nullptr || extraBits == nullptr)
	{
		#ifdef USE_EXCEPTIONS
		throw InvalidDataError();
		#endif
		return;
	}

	if(code<=3)
	{
		*extraBits = 0;
		*baseValue = 1+code;
	}
	else if(code<=5)
	{
		*extraBits = 1;
		*baseValue = 5 + (code-4)*(std::pow(2, *extraBits));
	}
	else if(code<=7)
	{
		*extraBits = 2;
		*baseValue = 9 + (code-6)*(std::pow(2, *extraBits));
	}
	else if(code<=9)
	{
		*extraBits = 3;
		*baseValue = 17 + (code-8)*(std::pow(2, *extraBits));
	}
	else if(code<=11)
	{
		*extraBits = 4;
		*baseValue = 33 + (code-10)*(std::pow(2, *extraBits));
	}
	else if(code<=13)
	{
		*extraBits = 5;
		*baseValue = 65 + (code-12)*(std::pow(2, *extraBits));
	}
	else if(code<=15)
	{
		*extraBits = 6;
		*baseValue = 129 + (code-14)*(std::pow(2, *extraBits));
	}
	else if(code<=17)
	{
		*extraBits = 7;
		*baseValue = 257 + (code-16)*(std::pow(2, *extraBits));
	}
	else if(code<=19)
	{
		*extraBits = 8;
		*baseValue = 513 + (code-18)*(std::pow(2, *extraBits));
	}
	else if(code<=21)
	{
		*extraBits = 9;
		*baseValue = 1025 + (code-20)*(std::pow(2, *extraBits));
	}
	else if(code<=23)
	{
		*extraBits = 10;
		*baseValue = 2049 + (code-22)*(std::pow(2, *extraBits));
	}
	else if(code<=25)
	{
		*extraBits = 11;
		*baseValue = 4097 + (code-24)*(std::pow(2, *extraBits));
	}
	else if(code<=27)
	{
		*extraBits = 12;
		*baseValue = 8193 + (code-26)*(std::pow(2, *extraBits));
	}
	else if(code<=29)
	{
		*extraBits = 13;
		*baseValue = 16385 + (code-28)*(std::pow(2, *extraBits));
	}
}

#pragma endregion

#pragma region ARITHMETIC

double Compression::compressArithmetic(std::vector<unsigned char> data, std::vector<double>& percentages)
{
	//generate percentages
	percentages.clear();
	percentages = std::vector<double>(256);

	int total = data.size();

	std::vector<double> percentageRangeVals = std::vector<double>();
	
	for(int i=0; i<data.size(); i++)
	{
		percentages[data[i]] += 1;
	}

	percentageRangeVals.push_back(0);
	for(int i=0; i<256; i++)
	{
		percentages[i] /= total;
		percentageRangeVals.push_back(percentages[i] + percentageRangeVals.back());
	}

	double oldMinPer = 0.0;
	double oldMaxPer = 1.0;
	
	for(int i=0; i<data.size(); i++)
	{
		int index = data[i];
		double minPercentage = percentageRangeVals[index];
		double maxPercentage = percentageRangeVals[index+1];

		double nMinPer = oldMinPer + (oldMaxPer-oldMinPer) * minPercentage;
		double nMaxPer = oldMinPer + (oldMaxPer-oldMinPer) * maxPercentage;

		StringTools::println("%c : range %f to %f", data[i], minPercentage, maxPercentage);
		StringTools::println("%c : new range %f to %f", data[i], nMinPer, nMaxPer);

		oldMinPer = nMinPer;
		oldMaxPer = nMaxPer;
	}

	double finalValue = (oldMinPer + oldMaxPer)/2.0;

	StringTools::println("FinalValue: %f", finalValue);
	return finalValue;
}

std::vector<unsigned char> Compression::decompressArithmetic(double data, int messageSize, std::vector<double> percentages)
{
	//generate percentageRanges

	std::vector<double> percentageRangeVals = std::vector<double>();
	percentageRangeVals.push_back(0);
	for(int i=0; i<256; i++)
	{
		percentageRangeVals.push_back(percentages[i] + percentageRangeVals.back());
	}

	double oldMinPer = 0.0;
	double oldMaxPer = 1.0;

	std::vector<unsigned char> message = std::vector<unsigned char>();
	while(message.size() < messageSize)
	{
		bool valid = false;
		for(int i=1; i<percentageRangeVals.size(); i++)
		{
			double minPercentage = percentageRangeVals[i-1];
			double maxPercentage = percentageRangeVals[i];

			double nMinPer = oldMinPer + (oldMaxPer-oldMinPer) * minPercentage;
			double nMaxPer = oldMinPer + (oldMaxPer-oldMinPer) * maxPercentage;
			
			if(data >= nMinPer && data < nMaxPer)
			{
				oldMinPer = nMinPer;
				oldMaxPer = nMaxPer;
				message.push_back((unsigned char)(i-1));
				valid = true;
				break;
			}
		}

		if(valid==false)
		{
			StringTools::println("Error finding data");
			break;
		}
	}
	
	return message;
}

#pragma endregion

#pragma region CHECKSUMS

unsigned int Compression::adler32(std::vector<unsigned char> data)
{
	return Compression::adler32(data.data(), data.size());
}

unsigned int Compression::adler32(unsigned char* data, int size)
{
	if(size <= 0)
	{
		#ifdef USE_EXCEPTIONS
		throw InvalidSizeError();
		#endif
		return 0;
	}

	if(data == nullptr)
	{
		#ifdef USE_EXCEPTIONS
		throw InvalidDataError();
		#endif
		return 0;
	}

	unsigned long sumA = 1;
	unsigned long sumB = 0;

	for(int i=0; i<size; i++)
	{
		sumA = (sumA + data[i]) % 65521;
		sumB = (sumB + sumA) % 65521;
	}

	return sumB*65536 + sumA;
}

unsigned int Compression::crc(std::vector<unsigned char> data, unsigned char type)
{
	return Compression::crc(data.data(), data.size(), type);
}

unsigned int Compression::crc(unsigned char* data, int size, unsigned char type)
{
	if(size <= 0)
	{
		#ifdef USE_EXCEPTIONS
		throw InvalidSizeError();
		#endif
		return 0;
	}

	if(data == nullptr)
	{
		#ifdef USE_EXCEPTIONS
		throw InvalidDataError();
		#endif
		return 0;
	}
	
	unsigned long crcTable[256];
	unsigned long polynomial = 0;
	unsigned int crcValue = 0xFFFFFFFF;

	switch(type)
	{
		case CRC_8:
			polynomial = 0xE0;
			crcValue = crcValue & 0xFF;
			break;
		case CRC_16:
			polynomial = 0xA001;
			crcValue = crcValue & 0xFFFF;
			break;
		case CRC_32:
			polynomial = 0xEDB88320;
			crcValue = crcValue & 0xFFFFFFFF;
			break;
		default:
			polynomial = 0xEDB88320;
			crcValue = crcValue & 0xFFFFFFFF;
			break;
	}

	//fill table
	for(int i=0; i<256; i++)
	{
		unsigned long remainder = i;
		for(int b=0; b<8; b++)
		{
			if(remainder & 0x01)
			{
				remainder = (remainder >> 1) ^ polynomial;
			}
			else
			{
				remainder = (remainder >> 1);
			}
		}

		crcTable[i] = remainder;
	}

	for(int i=0; i<size; i++)
	{
		crcValue = crcTable[ (data[i] ^ crcValue)&0xFF] ^ (crcValue>>8);
	}

	return ~crcValue;
}



#pragma endregion