#include "InternalCompressionHeader.h"

#ifndef __min
#define __min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef __max
#define __max(a, b) (((a) > (b)) ? (a) : (b))
#endif

namespace smpl
{

	StreamCompressionLZ77::StreamCompressionLZ77(bool mode, unsigned int maxBackwardsDistance, unsigned char maxLength)
	{
		this->mode = mode;
		this->maxBackDist = maxBackwardsDistance;
		this->maxLength = maxLength;

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

	StreamCompressionLZ77::~StreamCompressionLZ77()
	{
        if(csa != nullptr)
            delete csa;
        csa = nullptr;
	}

	void StreamCompressionLZ77::addData(unsigned char* data, int length)
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
    
	void StreamCompressionLZ77::addDataCompression(unsigned char* data, int length)
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
                unsigned int backwardsDis = startLoc - searchState.start;

                if(searchState.length >= 3)
                {
                    buffer.push_back({false, (unsigned int)searchState.length, backwardsDis});
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
                    buffer.push_back({true, delayedBuffer[startOfQueue], 0});
					startLoc++;

					csa->extend(delayedBuffer[startOfQueue]);
					startOfQueue = (startOfQueue + 1) % 8;
					delayedBufferSize--;
                }
                
				currQueueIndex = startOfQueue;
            }
        }
	}

    void StreamCompressionLZ77::addDataDecompression(unsigned char* data, int length)
	{
        size_t index = 0;
        int trueLength = length;
        if(length <= 0)
            return;
        
        if(backBuffer.size() > 0)
        {
            if(backBuffer[0] == true)
            {
                //it was a literal we failed on. Read 1 byte from data
                buffer.push_back({true, data[0], 0});
                trueLength--;
            }
            else
            {
                //it was a reference we failed on. Read upto 8 bytes. May fail
                for(int i=0; i<length; i++)
                {
                    backBuffer.push_back(data[i]);
                    trueLength--;
                    if(backBuffer.size() >= 9)
                        break;
                }

                if(backBuffer.size() != 9)
                {
                    lengthPair p;
                    p.literal = false;
                    p.v1 = *((int*)&backBuffer[1]);
                    p.v2 = *((int*)&backBuffer[5]);
                    buffer.push_back(p);
                }
                else
                {
                    return; //not enough data
                }
            }
            backBuffer.clear();
        }

        while(index < trueLength)
        {
            //read byte
            if(data[index] == 0)
            {
                if(trueLength-index < 9)
                {
                    //not enough data. add to back buffer
                    for(int i=index; i<trueLength; i++)
                    {
                        backBuffer.push_back(data[index]);
                    }
                    break;
                }
                //ref data.
                //Read 4 bytes for backwards distance
                //Read 4 bytes for length
                lengthPair p;
                p.literal = false;
                p.v1 = *((int*)&data[index+1]);
                p.v2 = *((int*)&data[index+5]);
                buffer.push_back(p);
                index += 9;
            }
            else
            {
                //literal data.
                //Read 1 byte for data
                if(trueLength-index < 2)
                {
                    //not enough data. add to back buffer
                    for(int i=index; i<trueLength; i++)
                    {
                        backBuffer.push_back(data[index]);
                    }
                    break;
                }
                buffer.push_back({true, data[index+1], 0});
                index += 2;
            }
        }
	}

    void StreamCompressionLZ77::endData()
    {
        //here we just need to do the very last bit which is get the remaining match
        //and add anything left in the delayed buffer
		searchState = csa->extractSearch();
		unsigned int backwardsDis = startLoc - searchState.start;
		if (searchState.length >= 3)
		{
            buffer.push_back({false, (unsigned int)searchState.length, backwardsDis});
		}
		while (delayedBufferSize > 0)
		{
            buffer.push_back({true, delayedBuffer[startOfQueue], 0});

			startOfQueue = (startOfQueue + 1) % 8;
			delayedBufferSize--;
		}
    }

	std::vector<lengthPair>& StreamCompressionLZ77::getBuffer()
	{
		return buffer;
	}

	void StreamCompressionLZ77::clearBuffer()
	{
		buffer.clear();
	}

	size_t StreamCompressionLZ77::size()
	{
		return buffer.size();
	}
}