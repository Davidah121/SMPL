#include "InternalCompressionHeader.h"

namespace glib
{

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
		
		size_t t1,t2;

		t1 = System::getCurrentTimeMicro();
		SimpleHashMap<int, int> map = SimpleHashMap<int, int>( SimpleHashMap<int, int>::MODE_KEEP_ALL, 1<<15 );
		map.setMaxLoadFactor(-1);
		t2 = System::getCurrentTimeMicro();

		// StringTools::println("TIME TO CREATE %llu", t2-t1);

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
						map.remove(it);
						continue;
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

					if(bestLength>=lengthMax)
					{
						break;
					}
				}

				//always insert
				map.add( key, i );
				for(int j=1; j<bestLength; j++)
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

		t1 = System::getCurrentTimeMicro();
		map.clear();
		t2 = System::getCurrentTimeMicro();

		// StringTools::println("TIME TO DELETE %llu", t2-t1);

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
			return {};
		}

		if(size <=0 || blocks <=0 )
		{
			#ifdef USE_EXCEPTIONS
			throw InvalidSizeError();
			#endif
			return {};
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
			info[block].clear();

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
		
		//Construct
		for(int block=0; block<blocks; block++)
		{
			bin.add( threadBinData[block] );
		}

		bin.setAddBitOrder(BinarySet::LMSB);
		
		return bin.toBytes();
	}

	void Compression::compressDeflate(BinarySet* outputData, unsigned char* data, int size, int blocks, int compressionLevel, bool customTable)
	{
		//probably will only use default trees and stuff
		outputData->clear();
		if(data == nullptr)
		{
			#ifdef USE_EXCEPTIONS
			throw InvalidDataError();
			#endif
			return;
		}

		if(size <=0 || blocks <=0 )
		{
			#ifdef USE_EXCEPTIONS
			throw InvalidSizeError();
			#endif
			return;
		}
		
		int tSize = System::getNumberOfThreads();
		std::vector<std::vector<lengthPair>> info = std::vector<std::vector<lengthPair>>(blocks);
		std::vector<std::thread> threads = std::vector<std::thread>( tSize );

		BinarySet bin = BinarySet();

		outputData->setBitOrder(BinarySet::LMSB);
		outputData->setAddBitOrder(BinarySet::RMSB);

		int sizeOfBlock = size/blocks;

		for(int block=0; block<blocks; block++)
		{
			int nSize = sizeOfBlock;
			unsigned char* nData = data + block*sizeOfBlock;
			if(block == blocks-1)
			{
				nSize = size - (block*sizeOfBlock);
			}
			info[block].clear();

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

		//Construct
		for(int block=0; block<blocks; block++)
		{
			outputData->add( threadBinData[block] );
		}

		outputData->setAddBitOrder(BinarySet::LMSB);
	}

	std::vector<unsigned char> Compression::decompressDeflate(std::vector<unsigned char> data, size_t expectedSize)
	{
		return Compression::decompressDeflate(data.data(), data.size(), expectedSize);
	}

	std::vector<unsigned char> Compression::decompressDeflate(unsigned char* data, int size, size_t expectedSize)
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

					//Size check for security
					if(finalData.size()+length > expectedSize)
					{
						return std::vector<unsigned char>();
					}

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

							//Size check for security
							if(finalData.size() > expectedSize)
							{
								return std::vector<unsigned char>();
							}
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

							//Size check for security
							if(finalData.size()+finalCopyLength > expectedSize)
							{
								return std::vector<unsigned char>();
							}

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

} //NAMESPACE glib END