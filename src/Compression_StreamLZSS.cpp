#include "InternalCompressionHeader.h"

#ifndef __min
#define __min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef __max
#define __max(a, b) (((a) > (b)) ? (a) : (b))
#endif

namespace smpl
{

	StreamCompressionLZSS::StreamCompressionLZSS(bool mode, unsigned int maxBackwardsDistance, unsigned char maxLength)
	{
		this->mode = mode;
		this->maxBackDist = __min(maxBackwardsDistance, 32768);
		this->maxLength = maxLength+3;

        buffer.setBitOrder(BinarySet::LMSB);

        if(mode == TYPE_COMPRESSION)
        {
            csa = new ChainedSuffixAutomaton(this->maxBackDist, 3, this->maxLength);
            csa->resetSearch();
        }
        else
        {
            backBuffer = std::vector<unsigned char>(65536);
        }
	}

	StreamCompressionLZSS::~StreamCompressionLZSS()
	{
        if(csa != nullptr)
            delete csa;
        csa = nullptr;
	}

	void StreamCompressionLZSS::addData(unsigned char* data, int length)
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
    
	void StreamCompressionLZSS::addDataCompression(unsigned char* data, int length)
	{
        size_t index = 0;

        while(index < length)
        {
            if (delayedBufferSize < 3)
			{
				int endOfQueue = (delayedBufferSize + startOfQueue) % 8;

				delayedBuffer[endOfQueue] = data[index];
				delayedBufferSize++;
				index++;
			}
            unsigned char c = delayedBuffer[currQueueIndex];
			currQueueIndex = (currQueueIndex + 1) % 8;

			bool foundMatch = csa->searchNext(c);
			searchState = csa->extractSearch();

            if(foundMatch && searchState.length != maxLength)
            {
                if (searchState.length >= 3)
				{
					while (delayedBufferSize > 0)
					{
						csa->extend(delayedBuffer[startOfQueue]);
						startOfQueue = (startOfQueue + 1) % 8;
						delayedBufferSize--;
					}
					currQueueIndex = startOfQueue;
				}
            }
            else
            {
                csa->resetSearch();
                int backwardsDis = startLoc - searchState.start;

                if(searchState.length >= 3)
                {
                    buffer.add(false);
                    buffer.add(backwardsDis, 15);
                    buffer.add(searchState.length-3, 8);
                    startLoc += searchState.length;
                    
                    if(foundMatch && searchState.length == maxLength)
                    {
						csa->extend(delayedBuffer[startOfQueue]);
						startOfQueue = (startOfQueue + 1) % 8;
						delayedBufferSize--;
                    }
                }
                else
                {
					//only add 1
                    buffer.add(true);
                    buffer.add(delayedBuffer[startOfQueue]);
					startLoc++;

					csa->extend(delayedBuffer[startOfQueue]);
					startOfQueue = (startOfQueue + 1) % 8;
					delayedBufferSize--;
                }
                
				currQueueIndex = startOfQueue;
            }
        }
	}

    void StreamCompressionLZSS::addDataDecompression(unsigned char* data, int length)
	{
        size_t index = 0;
        while(true)
        {
            if(leftoversSize > 0)
            {
                if(getBitFromLeftovers())
                {
                    //literal. read 8 more bits
                    if(leftoversSize < 9)
                    {
                        if(index >= length)
                            break;
                        addByteToLeftovers(data[index]);
                        index++;
                    }
                    if(leftoversSize < 9) //need more data
                        break;
                    
                    //read the next 8 bits
                    removeFlagBitLeftovers();
                    unsigned char lit = getBitsFromLeftoversAndRemove(8);
                    buffer.getByteRef().push_back(lit);

                    backBuffer[backBuffLocation] = lit;
                    backBuffLocation++;
                }
                else
                {
                    //reference read 15+8 more bits
                    while(leftoversSize < 24)
                    {
                        if(index >= length)
                            break;
                        addByteToLeftovers(data[index]);
                        index++;
                    }
                    if(leftoversSize < 24) //need more data
                        break;
                    
                    //read the next 24 bits
                    removeFlagBitLeftovers();
                    unsigned short backwards = getBitsFromLeftoversAndRemove(15);
                    int copyLength = getBitsFromLeftoversAndRemove(8) + 3;
                    unsigned short copyLoc = backBuffLocation - backwards;

                    for(int i=0; i<copyLength; i++)
                    {
                        buffer.getByteRef().push_back(backBuffer[copyLoc]);
                        backBuffer[backBuffLocation] = backBuffer[copyLoc];
                        copyLoc++;
                        backBuffLocation++;
                    }
                    
                }
            }
            else
            {
                if(index >= length)
                    break; //need more data to continue
                addByteToLeftovers(data[index]);
                index++;
            }
        }
	}

    void StreamCompressionLZSS::endData()
    {
        //here we just need to do the very last bit which is get the remaining match
        //and add anything left in the delayed buffer
		searchState = csa->extractSearch();
		int backwardsDis = startLoc - searchState.start;
		if (searchState.length >= 3)
		{
            buffer.add(false);
            buffer.add(backwardsDis, 15);
            buffer.add(searchState.length-3, 8);
		}
		while (delayedBufferSize > 0)
		{
            buffer.add(true);
            buffer.add(delayedBuffer[startOfQueue]);

			startOfQueue = (startOfQueue + 1) % 8;
			delayedBufferSize--;
		}
    }

	BinarySet& StreamCompressionLZSS::getBuffer()
	{
        if(mode == TYPE_DECOMPRESSION)
            buffer.setNumberOfBits( buffer.getByteRef().size() * 8 ); //decompressed data is byte aligned
		return buffer;
	}

	void StreamCompressionLZSS::clearBuffer()
	{
		buffer.clear();
	}

	size_t StreamCompressionLZSS::size()
	{
		return buffer.getByteRef().size();
	}
    
    bool StreamCompressionLZSS::getBitFromLeftovers()
    {
        bool bit = leftoversInt & 0x01;
        return bit;
    }
    int StreamCompressionLZSS::getBitsFromLeftoversAndRemove(int size)
    {
        int mask = ((1<<size)-1);
        int v = leftoversInt & mask;
        
        leftoversInt = leftoversInt >> size;
        leftoversSize -= size;
        return v;
    }
    void StreamCompressionLZSS::addByteToLeftovers(unsigned char v)
    {
        int mask = ((1<<leftoversSize)-1);
        leftoversInt = (v << leftoversSize) + (leftoversInt & mask);
        leftoversSize += 8;
    }
    
    void StreamCompressionLZSS::removeFlagBitLeftovers()
    {
        leftoversInt = leftoversInt >> 1;
        leftoversSize -= 1;
    }
}