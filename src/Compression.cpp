#include "Compression.h"
#include <iostream>
#include "FrequencyTable.h"
#include "LinkedList.h"
#include "BinarySet.h"
#include "Sort.h"
#include "StringTools.h"

#define min(a,b) (a<b)? a:b
#define max(a,b) (a>b)? a:b

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

std::vector<unsigned char> Compression::compressLZW(std::vector<unsigned char> data)
{
	return Compression::compressLZW(data.data(), data.size());
}

std::vector<unsigned char> Compression::compressLZW(unsigned char* data, int size)
{
	std::vector<unsigned char> output = std::vector<unsigned char>();

	//change to limit to max bit read of 12. Currently, no maximum.
	if (size > 0)
	{
		//First make the base dictionary
		std::vector<std::string> baseDictionary = std::vector<std::string>();
		
		for (int i = 0; i < size; i++)
		{
			std::string temp = "";
			temp += data[i];
			bool canInsert = true;

			for (int j = 0; j < baseDictionary.size(); j++)
			{
				if (baseDictionary[j] == temp)
				{
					canInsert = false;
					break;
				}
			}

			if (canInsert)
			{
				baseDictionary.push_back(temp);
			}
		}

		//Add the clearDictionary and endOfData values.
		//We need to store the location so that we don't say that we can exclude it
		//when we compare what data is in the dictionary.

		int clearDictionaryLocation = baseDictionary.size();
		int EndOfDataLocation = baseDictionary.size()+1;

		baseDictionary.push_back("");//For the clearDictionary
		baseDictionary.push_back("");//For the EndOfData

		std::vector<std::string> newDictionary = std::vector<std::string>(baseDictionary);

		//Compress
		//First note how many bits to use
		int baseBits = (int)ceil(log2(newDictionary.size()));
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
			bool exists = false;

			for (int j = 0; j < newDictionary.size(); j++)
			{
				if (newString == newDictionary[j])
				{
					preIndex = j;
					exists = true;
					break;
				}
			}

			if (exists == true)
			{
				lastString = newString;
				i++;
			}
			else
			{
				binData.add(preIndex, currBits);
				
				newDictionary.push_back(newString);
				currBits = (int)ceil(log2(newDictionary.size()));

				preIndex = -1;
				newString = "";
				lastString = "";
			}

			if(newDictionary.size() == 4096)
			{
				//clear Dictionary and start over
				binData.add(clearDictionaryLocation, currBits);
				currBits = baseBits;
				newDictionary = baseDictionary;
			}
			
		}

		if (newString != "")
		{
			binData.add(preIndex, currBits, 0);
		}

		binData.add(EndOfDataLocation, currBits, 0);
		output = binData.toBytes(true);
	}

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
								int tempBits = (int)ceil(log2(newDictionary.size() + 1));
								if(tempBits<=12)
									bitsToRead = tempBits;
								else
									newDictionary.pop_back();
							}
							else
							{
								//StringTools::out << "L317 Error in getting last index, " << lastIndex << ", Index=" << index << " dictionary: " << newDictionary.size() << StringTools::lineBreak;
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
							bitsToRead = (int)ceil(log2(newDictionary.size() + 1));
					
							for (int j = 0; j < newEntry.size(); j++)
							{
								output.push_back((unsigned char)newEntry[j]);
							}
						}
						else
						{
							//There is an error in the data
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
	
	return bin.toBytes(true);
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
				break;
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
	BinarySet output = BinarySet();
	output.setBitOrder(BinarySet::LMSB);

	//First, build a huffmanTree.
	//We have created a function to do this since it takes up
	//more space than I wanted. It will be changed later for
	//better performance.

	*tree = *Compression::buildHuffmanTree(data, size);

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

	return output.toBytes(true);
}

std::vector<unsigned char> Compression::decompressHuffman(std::vector<unsigned char> data, int messageSize, BinaryTree<HuffmanNode>* tree)
{
	return decompressHuffman(data.data(), data.size(), messageSize, tree);
}

std::vector<unsigned char> Compression::decompressHuffman(unsigned char* data, int size, int messageSize, BinaryTree<HuffmanNode>* tree)
{
	std::vector<unsigned char> output = std::vector<unsigned char>();

	BinarySet input = BinarySet();
	input.setBitOrder(BinarySet::LMSB);
	
	//first, fill the BinarySet
	input.setValues(data, size);

	input.printVals(true);

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
		//StringTools::out << "HUFFMAN DECODING, TREE ERROR: ROOT IS INVALID" << StringTools::lineBreak;
	}
	
	return output;
}

BinaryTree<HuffmanNode>* Compression::buildHuffmanTree(unsigned char* data, int size)
{
	//First pass is to fill the frequency table
	FrequencyTable<unsigned char> freqTable = FrequencyTable<unsigned char>();
	for (int i = 0; i < size; i++)
	{
		freqTable.add(data[i]);
	}

	//Next pass is to build the huffmanTree
	//First sort it
	freqTable.sort();
	int fSize = freqTable.size();
	int maxFrequency = freqTable.sumOfFrequencies();

	LinkedList< BinaryTreeNode<HuffmanNode>* > nodes = LinkedList<BinaryTreeNode<HuffmanNode>*>();

	//Next, make the nodes
	for (int i = 0; i < fSize; i++)
	{
		BinaryTreeNode<HuffmanNode>* test = new BinaryTreeNode<HuffmanNode>;
		test->data = { freqTable.getFrequencyAtLocation(i), freqTable.getValueAtLocation(i) };
		test->leftChild = nullptr;
		test->rightChild = nullptr;

		nodes.addNode(test);
	}

	//Then, group based on min freq.
	//We build these from the bottom up. Eliminating nodes as we go
	//A linked list would be good for thisin that case since we can easily
	//delete things and we still have to check each indiviually anyway.

	BinaryTree<HuffmanNode>* huffmanTree = new BinaryTree<HuffmanNode>();

	while (true)
	{
		//Find the two lowest and group them together as a treeNode.
		LinkNode<BinaryTreeNode<HuffmanNode>*>* min = nodes.getLastNode();
		LinkNode<BinaryTreeNode<HuffmanNode>*>* min2 = min->parentNode;
		
		BinaryTreeNode<HuffmanNode>* newTreeNode = new BinaryTreeNode<HuffmanNode>;

		if (min2->value->leftChild == nullptr && min2->value->rightChild == nullptr)
		{
			newTreeNode->leftChild = min2->value;
			newTreeNode->rightChild = min->value;
		}
		else
		{
			newTreeNode->leftChild = min->value;
			newTreeNode->rightChild = min2->value;
		}

		newTreeNode->data.frequency = min2->value->data.frequency + min->value->data.frequency;
		
		//Next, remove the previous mins from the node since they are now
		//combined in a new tree node. Add the new node to the end.
		//Check if we are done
		nodes.removeNode(min);
		nodes.removeNode(min2);

		nodes.addNode(newTreeNode);

		LinkNode<BinaryTreeNode<HuffmanNode>*>* temp = nodes.getLastNode();
		
		if (newTreeNode->data.frequency == maxFrequency)
		{
			//done
			huffmanTree->setRootNode(newTreeNode);
			break;
		}
		//Next, re-sort the nodes. This is simple since only one node is unsorted
		//and it is the last node that is unsorted.

		LinkNode<BinaryTreeNode<HuffmanNode>*>* currNode = nodes.getLastNode();

		while (true)
		{
			LinkNode<BinaryTreeNode<HuffmanNode>*>* parentNode = currNode->parentNode;
			LinkNode<BinaryTreeNode<HuffmanNode>*>* childNode = currNode->nextNode;
			LinkNode<BinaryTreeNode<HuffmanNode>*>* grandParentNode = nullptr;

			if (parentNode != nullptr)
				LinkNode<BinaryTreeNode<HuffmanNode>*>* grandParentNode = parentNode->parentNode;

			if (parentNode != nullptr)
			{
				if (currNode->value->data.frequency <= parentNode->value->data.frequency)
				{
					//Done sorting
					break;
				}
				else
				{
					if (grandParentNode != nullptr)
					{
						//not the start of the list
						grandParentNode->nextNode = currNode;
					}

					currNode->parentNode = grandParentNode;
					currNode->nextNode = parentNode;
					parentNode->parentNode = currNode;
					parentNode->nextNode = childNode;

					if (childNode != nullptr)
					{
						//Not at the end of the list
						childNode->parentNode = parentNode;
					}
				}
			}
			else
			{
				//start of list
				break;
			}
		}
	}

	return huffmanTree;
}

void Compression::fillHuffmanTable(BinaryTreeNode<HuffmanNode>* treeNode, unsigned int* table, int length, int code)
{
	//StringTools::out << treeNode << StringTools::lineBreak;
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

BinaryTree<HuffmanNode>* Compression::buildCanonicalHuffmanTree(int* dataValues, int* lengths, int size, bool reversed=false)
{
	//We must sort first before building the huffman code
	//Since this is C++ and we can use structures, we can use MergeSort while keeping the
	//correct location for each length and dataValue.

	//first, convert
	
	dataLengthCombo* arr = new dataLengthCombo[size];

	for(int i=0; i<size; i++)
	{
		arr[i] = {dataValues[i], lengths[i], 0};
	}

	//now sort
	//We use lambda expressions here so we only compare the length values. We could have overriden the comparison
	//operator in dataLengthCombo, but I wanted to use lambda expressions.

	std::function<bool(dataLengthCombo, dataLengthCombo)> compareFunc = [](const dataLengthCombo & a, const dataLengthCombo & b) -> bool { if(a.length == b.length)
																										return a.data < b.data;
																									else
																										return a.length < b.length; };
	Sort::mergeSort<dataLengthCombo>(arr, size, compareFunc);

	//now, we can start building the codes.
	int startCode = 0;
	int preLength = 0;
	for(int i=0; i<size; i++)
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

	for(int i=0; i<size; i++)
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

	return tree;
}

#pragma endregion

#pragma region DEFLATE

//Note that Literals are from 0-255
//Reference Pairs are from 257-285
//EndOfBlock is 256
//286 and 287 never occur

std::vector<unsigned char> Compression::compressDeflate(std::vector<unsigned char> data)
{
	return Compression::compressDeflate(data.data(), data.size());
}

std::vector<unsigned char> Compression::compressDeflate(unsigned char* data, int size)
{
	//probably will only use default trees and stuff
	return std::vector<unsigned char>();
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
	std::vector<unsigned char> finalData = std::vector<unsigned char>();

	BinarySet binData = BinarySet();
	
	//set data in binarySet
	binData.setBitOrder(BinarySet::LMSB);
	binData.setValues(data, size);

	StringTools::out << "BinSize " << binData.size() << StringTools::lineBreak;

	int currLoc = 0;
	bool invalid = false;

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
				break;
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
				mTree = Compression::buildDeflateDefaultTree();
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
						currNode = currNode->leftChild;
					}
					else
					{
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
						invalid = true;
						break;
					}
				}
				else
				{
					if(currNode->data.value <= 255)
					{
						//literal
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

						int backRefCode = 0;
						int baseBackLength = 0;
						int backRefExtraBits = 0;

						//read next 5 bits to get back ref
						//Note that this only works for default compression

						if(backTree==nullptr)
						{
							for(int i=4; i>=0; i--)
							{
								backRefCode += ((int)binData.getBit(currLoc) << i);
								currLoc+=1;
							}
						}
						else
						{
							//traverse the back tree to get the backRefCode
							BinaryTreeNode<HuffmanNode>* backNode = backTree->getRoot();

							while(true)
							{
								backNode = backTree->traverse(backNode, (int)binData.getBit(currLoc), 1, true);
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

			if(mTree!=nullptr)
				delete mTree;
			if(backTree!=nullptr)
				delete backTree;
			
			if(dynTrees!=nullptr)
				delete[] dynTrees;
				
			if(lastBlock)
			{
				break;
			}

			if (invalid)
			{
				break;
			}
		}
		
	}
	

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
	
	int hcValues[19] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
	int hcLengths[19];

	for(int i=0; i<19; i++)
	{
		hcLengths[i] = 0;
	}

	for(int i=0; i<amountOfCodeLengthCodes; i++)
	{
		hcLengths[i] = data->getBits(*location, *location+3);
		*location+=3;
	}
	
	
	BinaryTree<HuffmanNode>* tempTree = Compression::buildCanonicalHuffmanTree(hcValues, hcLengths, amountOfCodeLengthCodes, true);

	//next, we use this tree to find the code lengths for our data set.
	//Due to how we get the codes, we store them in a single array at first.
	//Since this is c++, we don't have to explicitly separate our array. Just set
	//the start point and the length.

	int* allCodes = new int[totalCodes];
	std::memset(allCodes, 0, totalCodes);

	BinaryTreeNode<HuffmanNode>* currNode = tempTree->getRoot();

	int codes = 0;
	int tempCode = 0;

	while(codes < totalCodes)
	{
		tempCode = (data->getBit(*location)==true) ? 1 : 0;
		*location+=1;		
		currNode = tempTree->traverse(currNode, tempCode, 1, true);

		if(currNode->leftChild == nullptr && currNode->rightChild == nullptr)
		{
			//hit a value
			int actualValue = currNode->data.value;
			
			if(actualValue < 16)
			{
				allCodes[codes]=actualValue;
				codes++;
			}
			else if(actualValue==16)
			{
				//copy previous value x times
				int extra = data->getBits(*location, *location+2);
				*location+=2;

				int copyLen = extra + 3;

				int preVal = allCodes[codes-1];
				for(int i=0; i<copyLen; i++)
				{
					allCodes[codes+i] = preVal;
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
					allCodes[codes+i] = 0;
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
						allCodes[codes+i] = 0;
				}
				codes+=copyLen;
			}

			currNode = tempTree->getRoot();
		}
	}

	//requirements
	//the value 256 must have a valid code
	if(allCodes[256] == 0)
	{
		if(allCodes!=nullptr)
			delete[] allCodes;
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

	int* lenLengthValues = new int[amountOfLitCodes];
	int* distValues = new int[amountOfDistCodes];

	for(int i=0; i<amountOfLitCodes; i++)
	{
		lenLengthValues[i] = i;
	}
	for(int i=0; i<amountOfDistCodes; i++)
	{
		distValues[i] = i;
	}

	trees[0] = Compression::buildCanonicalHuffmanTree(lenLengthValues, &allCodes[0], amountOfLitCodes, true);
	trees[1] = Compression::buildCanonicalHuffmanTree(distValues, &allCodes[amountOfLitCodes], amountOfDistCodes, true);

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

