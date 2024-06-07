#include "InternalCompressionHeader.h"

namespace smpl
{
	StreamCompressionLZW::StreamCompressionLZW(bool mode, int codeSize)
	{
		this->mode = mode;
		this->baseCodeSize = codeSize;
		buildBaseDictionaryTree(codeSize);

		leftovers.setBitOrder(BinarySet::LMSB);
		buffer.setBitOrder(BinarySet::LMSB);
		started = false;
	}

	StreamCompressionLZW::~StreamCompressionLZW()
	{

	}

	void StreamCompressionLZW::buildBaseDictionaryTree(int codeSize)
	{
		if(mode == TYPE_COMPRESSION)
		{
			int s = 1 << MathExt::clamp(codeSize, 0, 8);
			
			//Note that the tree deletes pointers when cleared or when a node is replaced.
			//Here, it is okay to create new data and not worry about deleting stuff.
			TreeNode<int>* rootNode = new TreeNode<int>();
			rootNode->children = std::vector<TreeNode<int>*>(256);
			rootNode->data = -1;

			this->currentTree.setRootNode( rootNode );

			for(int i=0; i<s; i++)
			{
				TreeNode<int>* childNode = new TreeNode<int>();
				childNode->children = std::vector<TreeNode<int>*>(256);
				childNode->data = i;
				rootNode->children[i] = childNode;
			}
			
			//These are not apart of the tree but are required anyway.
			clearDictionaryCode = s;
			endOfDataLocation = s+1;
			count = s+2;
			lastTreeNode = rootNode;
			
			baseBits = (int)ceil(log2(count));
			currBits = baseBits;
		}
		else if(mode == TYPE_DECOMPRESSION)
		{
			if(baseDictionary.size() == 0)
			{
				//here, code size is taken to be the size of the dictionary. Not the number of bits needed.
				baseDictionary.clear();
				for(int i=0; i<codeSize; i++)
				{
					std::string temp = "";
					temp += (unsigned char)i;
					baseDictionary.push_back(temp);
				}
				baseDictionary.push_back(""); //Empty set because it does not actually have any data
				baseDictionary.push_back(""); //Empty set because it does not actually have any data
				
				clearDictionaryCode = codeSize;
				endOfDataLocation = codeSize+1;
				count = codeSize+2;
			}
			decompressionDictionary = std::vector<std::string>(baseDictionary);
			baseBits = (int)ceil(log2(decompressionDictionary.size()));
			currBits = baseBits;
		}
	}

	void StreamCompressionLZW::addData(unsigned char* data, int length)
	{
		if(mode == TYPE_COMPRESSION)
		{
			addDataCompression(data, length);
		}
		else
		{
			addDataDecompression(data, length);
		}
	}
	
	void StreamCompressionLZW::addDataCompression(unsigned char* data, int length)
	{
		if(data == nullptr || length == 0 || isEndOfData == true)
			return;
		
		if(started == false)
		{
			//add clear dictionary
			buffer.add(clearDictionaryCode, currBits);
			started = true;
		}

		int i=0;

		while(i<length)
		{
			//attempt to traverse tree
			if(lastTreeNode == nullptr)
				break; //ERROR
			
			TreeNode<int>* currentNode = lastTreeNode->children[data[i]];
			if(currentNode != nullptr)
			{
				lastTreeNode = currentNode;
				i++;
			}
			else
			{
				//Does not exists. Add previous index and add new node.
				buffer.add(lastTreeNode->data, currBits);
				TreeNode<int>* newNode = new TreeNode<int>();
				newNode->children = std::vector<TreeNode<int>*>(256);
				
				newNode->data = count;
				lastTreeNode->children[data[i]] = newNode;
				count++;

				currBits = (int)ceil(log2(count)); //Simpler calculations with less code needed

				// previousNode = nullptr;
				lastTreeNode = currentTree.getRoot();
			}

			if(count == 4096)
			{
				//clear dictionary and start over
				//Dictionary size can not be greater than 4096. 
				//While you can keep the dictionary at 4096 and not extend it, it can be a bit difficult to manage.
				buffer.add(clearDictionaryCode, currBits);
				buildBaseDictionaryTree(baseCodeSize);
				// previousNode = nullptr;
				lastTreeNode = currentTree.getRoot();
			}
		}
	}

	void StreamCompressionLZW::addDataDecompression(unsigned char* data, int length)
	{
		//here, we will be modifying the binary set in an unconventional way.
		if(data == nullptr || length == 0 || isEndOfData == true)
			return;

		BinarySet bits = BinarySet();
		bits.setValues(data, length);

		size_t bitNum = 0;
		size_t i = 0;

		while(true)
		{
			//get code size if possible
			if(leftovers.size() + bits.size() - bitNum >= currBits)
			{
				int index = 0;
				if(leftovers.size() > 0)
				{
					//special case.
					for(int j=0; j<leftovers.size(); j++)
					{
						index |= (int)leftovers.getBit(j) << j;
					}

					for(int j=0; j<currBits-leftovers.size(); j++)
					{
						index |= (int)bits.getBit(bitNum + j) << (leftovers.size() + j);
					}
					
					bitNum += currBits - leftovers.size();
					leftovers.clear();
				}
				else
				{
					index = bits.getBits(bitNum, bitNum+currBits, false);
					bitNum += currBits;
				}

				if(index == clearDictionaryCode)
				{
					//clear dictionary
					buildBaseDictionaryTree(baseCodeSize);
					previousIndex = -1;
					index = -1;
					continue;
				}
				else if(index == endOfDataLocation)
				{
					addEndData();
					break;
				}
				else
				{
					//Check if it already exists.
					if(index < decompressionDictionary.size())
					{
						//exists, add to buffer
						std::string dataToAdd = decompressionDictionary[index];

						for(size_t j = 0; j<dataToAdd.size(); j++)
						{
							buffer.getByteRef().push_back((unsigned char)dataToAdd[j]);
						}
						buffer.setNumberOfBits( buffer.getByteRef().size() * 8 );
						
						if(previousIndex != -1)
						{
							if(previousIndex < (int)decompressionDictionary.size())
							{
								//add a new entry using previous + current[0]
								//Using strings since it is faster for some reason
								std::string newEntry = decompressionDictionary[previousIndex] + decompressionDictionary[index][0];
								
								//Determine if it is safe to add it.
								int tempNewBitSize = (int)ceil(log2(decompressionDictionary.size()+2));
								if(tempNewBitSize <= 12)
								{
									//Dictionary Size < 8192
									currBits = tempNewBitSize;
									decompressionDictionary.push_back(newEntry);
								}
							}
							else
							{
								#ifdef USE_EXCEPTIONS
								throw Compression::LZW_ERROR_L317();
								#endif
								// StringTools::println("L317 ERROR");
								// StringTools::println("%d, %d", index, previousIndex);
								//There is an error in the data
								break;
							}
						}
						
					}
					else
					{
						if(previousIndex < (int)decompressionDictionary.size())
						{
							//add a new entry using previous + previous[0]
							//Using strings since it is faster for some reason
							std::string newEntry = decompressionDictionary[previousIndex] + decompressionDictionary[previousIndex][0];
							
							//Determine if it is safe to add it.
							int tempNewBitSize = (int)ceil(log2(decompressionDictionary.size()+2));
							if(tempNewBitSize <= 12)
							{
								//Dictionary Size < 8192
								currBits = tempNewBitSize;
								decompressionDictionary.push_back(newEntry);
							}

							for(size_t j=0; j<newEntry.size(); j++)
							{
								buffer.getByteRef().push_back((unsigned char)newEntry[j]);
							}
							buffer.setNumberOfBits( buffer.getByteRef().size() * 8 );
						}
						else
						{
							#ifdef USE_EXCEPTIONS
							throw Compression::LZW_ERROR_L340();
							#endif
							// StringTools::println("L340 ERROR");
							//StringTools::out << "L340 Error in getting last index, " << lastIndex << ", Index=" << index << " dictionary: " << newDictionary.size() << StringTools::lineBreak;
							break;
						}
						
					}

					previousIndex = index;
					index = 0;
				}
			}
			else
			{
				//add remainder to the leftovers
				for(size_t b = bitNum; b<bits.size(); b++)
				{
					leftovers.add( bits.getBit(b) );
				}
				break;
			}
			
		}
	}
	
	BinarySet& StreamCompressionLZW::getBuffer()
	{
		return buffer;
	}

	void StreamCompressionLZW::addEndData()
	{
		if(mode == TYPE_COMPRESSION)
		{
			//add end data code.
			if(lastTreeNode != nullptr)
			{
				buffer.add(lastTreeNode->data, currBits);
			}
			buffer.add(endOfDataLocation, currBits);
		}

		lastTreeNode = nullptr;
		isEndOfData = true;
	}

	void StreamCompressionLZW::addClearDictionary()
	{
		if(mode == TYPE_COMPRESSION)
		{
			//clear dictionary and start over
			if(lastTreeNode != nullptr)
			{
				buffer.add(lastTreeNode->data, currBits);
			}
			buffer.add(clearDictionaryCode, currBits);
			buildBaseDictionaryTree(baseCodeSize);
			lastTreeNode = currentTree.getRoot();
		}
	}

	bool StreamCompressionLZW::getEndData()
	{
		return isEndOfData;
	}

	void StreamCompressionLZW::clearBuffer()
	{
		buffer.clear();
	}

	size_t StreamCompressionLZW::size()
	{
		return buffer.getByteRef().size();
	}
};