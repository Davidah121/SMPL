// #include "InternalCompressionHeader.h"

// namespace glib
// {

// 	StreamCompressionLZSS::StreamCompressionLZSS(bool mode, unsigned int maxBackwardsDistance, unsigned char maxLength)
// 	{
// 		this->mode = mode;
// 		this->maxBackDist = maxBackwardsDistance;
// 		this->maxLength = maxLength;
// 	}

// 	StreamCompressionLZSS::~StreamCompressionLZSS()
// 	{

// 	}

// 	void StreamCompressionLZSS::addData(unsigned char* data, int length)
// 	{
// 		if(mode == TYPE_COMPRESSION)
// 		{
// 			addDataCompression(data, length);
// 		}
// 		else
// 		{
// 			addDataDecompression(data, length);
// 		}
// 	}

// 	void StreamCompressionLZSS::addDataCompression(unsigned char* data, int length)
// 	{
// 		for(int i=0; i<length; i++)
// 		{
// 			BRS<RBNode<uint32_t>> newRange = {};
// 			int addType = 0; //0 = delayed, 1 = add literals, 2 = add length pair

// 			if(sufTree.getBuffer().size() >= 3)
// 			{
// 				newRange = sufTree.binaryRangeSearch(data[i], offset, lastKnownRange);

// 				if(newRange.commonAncestor == nullptr)
// 				{
// 					if(lastKnownRange.commonAncestor == nullptr || offset < 3)
// 					{
// 						addType = 1;
// 					}
// 					else if(offset >= 3 && lastKnownRange.commonAncestor != nullptr)
// 					{
// 						addType = 2;
// 					}
// 				}
// 				else
// 				{
// 					if(offset >= maxLength)
// 					{
// 						addType = 2;
// 					}
// 				}
// 			}
// 			else
// 			{
// 				// not enough data to form a length pair.
// 				addType = 1;
// 			}


// 			if(addType == 1)
// 			{
// 				//add last couple of literals
// 				for(int j=offset; j>=0; j--)
// 				{
// 					//add literal
// 					unsigned char c;
// 					buffer.add(true);

// 					//extra work for handling incoming stream data and dealing with the previous buffer
// 					if(i-j >= 0)
// 					{
// 						c = data[i-j];
// 					}
// 					else
// 					{
// 						int extraOffset = i-j; //starts at -1
// 						c = sufTree.getBuffer()[ sufTree.getBuffer().size()+extraOffset ];
// 					}

// 					buffer.add(c, 8);
// 					// StringTools::println("Literal: %c", c);
// 				}
// 				lastKnownRange = {};
// 				offset = 0;
// 			}
// 			else if(addType == 2)
// 			{
// 				//add length pair
// 				unsigned short dist;
// 				unsigned char len = offset-3;

// 				if(newRange.commonAncestor != nullptr)
// 					dist = sufTree.getBuffer().size() - newRange.commonAncestor->data.data;
// 				else
// 					dist = sufTree.getBuffer().size() - lastKnownRange.commonAncestor->data.data;
				
// 				buffer.add(false);
// 				buffer.add(dist-offset, 15);
// 				buffer.add(len, 8);
				
// 				// StringTools::println("Length Pair: %d, %d", dist-offset, len+3);

// 				lastKnownRange = {};
// 				offset = 0;

// 				//add literal that did not get added
// 				if(newRange.commonAncestor == nullptr)
// 				{
// 					buffer.add(true);
// 					buffer.add(data[i], 8);

// 					// StringTools::println("Literal: %c", data[i]);
// 				}
// 			}
// 			else
// 			{
// 				offset++;
// 				lastKnownRange = newRange;
// 			}

// 			//add to tree and add that node to a linked list
// 			BinaryTreeNode<RBNode<uint32_t>>* newNode = sufTree.push(data[i]);
// 			queueOfNodes.addNode(newNode);
// 			queueSize++;

// 			if(addType > 0)
// 			{
// 				//safe to delete stuff
// 				//at worst, O(maxLength) which is a constant. So 255 deletions at most
// 				while(queueSize > maxBackDist)
// 				{
// 					LinkNode<BinaryTreeNode<RBNode<uint32_t>>*>* lNode = queueOfNodes.getRootNode();
// 					if(lNode != nullptr)
// 					{
// 						// StringTools::println("Deleting: %p", lNode->value);
// 						sufTree.remove( lNode->value );
// 						queueOfNodes.removeNode(lNode);
// 						queueSize--;
// 					}
// 					else
// 					{
// 						break;
// 					}
// 				}
// 			}

// 			// if(sufTree.getBuffer().size() > 0xFFFF)
// 			// {
// 			// 	return;
// 			// }
// 		}
// 	}

// 	void StreamCompressionLZSS::addDataDecompression(unsigned char* data, int length)
// 	{

// 	}

// 	BinarySet& StreamCompressionLZSS::getBuffer()
// 	{
// 		return buffer;
// 	}

// 	void StreamCompressionLZSS::clearBuffer()
// 	{
// 		buffer.clear();
// 	}

// 	size_t StreamCompressionLZSS::size()
// 	{
// 		return buffer.getByteRef().size();
// 	}
// }