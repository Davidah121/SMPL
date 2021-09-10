#include "InternalCompressionHeader.h"

namespace glib
{

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

	std::vector<unsigned char> Compression::decompressHuffman(std::vector<unsigned char> data, BinaryTree<HuffmanNode>* tree, size_t expectedSize)
	{
		return decompressHuffman(data.data(), data.size(), tree, expectedSize);
	}

	std::vector<unsigned char> Compression::decompressHuffman(unsigned char* data, int size, BinaryTree<HuffmanNode>* tree, size_t expectedSize)
	{
		std::vector<unsigned char> output = std::vector<unsigned char>();

		if(size <= 0)
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
		BinaryTreeNode<HuffmanNode>* currNode = tree->getRoot();

		if (currNode != nullptr)
		{
			while (i < 0)
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
						return std::vector<unsigned char>();
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
						return std::vector<unsigned char>();
					}
				}

				if (currNode->leftChild == nullptr && currNode->rightChild == nullptr)
				{
					//leaf node
					output.push_back(currNode->data.value);
					currNode = tree->getRoot();

					if(output.size() > expectedSize)
					{
						#ifdef USE_EXCEPTIONS
						throw ExceededExpectedSizeError();
						#endif
						return std::vector<unsigned char>();
					}
				}
			}
		}
		else
		{
			#ifdef USE_EXCEPTIONS
			throw HUFFMAN_TREE_ERROR();
			#endif
			return std::vector<unsigned char>();
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

	BinaryTree<HuffmanNode>* Compression::buildCanonicalHuffmanTree(int* dataValues, int sizeOfData, int* lengths, int sizeOfCodeLengths, bool separateCodeLengths, bool rmsb)
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

				if(rmsb==false)
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

} //NAMESPACE glib END