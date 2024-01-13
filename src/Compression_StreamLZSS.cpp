#include "InternalCompressionHeader.h"

#ifndef __min
#define __min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef __max
#define __max(a, b) (((a) > (b)) ? (a) : (b))
#endif

namespace glib
{

	StreamCompressionLZSS::StreamCompressionLZSS(bool mode, unsigned int maxBackwardsDistance, unsigned char maxLength)
	{
		this->mode = mode;
		this->maxBackDist = __min(maxBackwardsDistance, 32768);
		this->maxLength = maxLength+3;

        if(mode == TYPE_COMPRESSION)
        {
            map = SimpleHashMap<int, int>( SimpleHashMap<int, int>::MODE_KEEP_ALL, 1<<15 );
            map.setMaxLoadFactor(-1);
        }
        else
        {
            backBuffer = std::vector<unsigned char>(65536);
        }
	}

	StreamCompressionLZSS::~StreamCompressionLZSS()
	{

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
		for(int i=0; i<length; i++)
        {
            lzBuffer.push_back(data[i]);
        }

        size_t index = 0;
        size_t startOffset = offset;
        size_t endOffset = offset + lzBuffer.size();
        if(lzBuffer.size() < 258)
            return;

        while(offset < endOffset - 258)
        {
            int v = ((int)lzBuffer[index+2] << 16) | ((int)lzBuffer[index+1] << 8) | (lzBuffer[index]);
            HashPair<int, int>* k = map.get(v);
            map.add(v, index);

            if(k == nullptr)
            {
                buffer.add(true);
                buffer.add(lzBuffer[index], 8);
                StringTools::println("LIT: %d", lzBuffer[index]);
                offset++;
            }
            else
            {
                //do more
                int lowestPoint = __max(offset-maxBackDist, 0);
                int bestLength = 0;
                int bestLocation = 0;
                unsigned char* startMatch = (lzBuffer.data()+index);
                
                do
                {
                    int locationOfMatch = k->data;
                    if(locationOfMatch < lowestPoint)
                        break;
                    
                    unsigned char* startBase = (lzBuffer.data()+(offset - locationOfMatch));

                    int len = 3;
                    while(len < maxLength && startMatch[len] == startBase[len])
                    {
                        len++;
                    }

                    if(len >= bestLength)
                    {
                        bestLength = len;
                        bestLocation = locationOfMatch;
                    }

                    k = map.getNext();
                } while (k != nullptr);
                
                //always insert
                int tInd = index+1;
                for(int j=1; j<bestLength-2; j++)
                {
                    int nkey = lzBuffer[tInd] + ((int)lzBuffer[tInd+1]<<8) + ((int)lzBuffer[tInd+2]<<16);
                    map.add( nkey, tInd );
                    tInd++;
                }

                if(bestLength>=3)
                {
                    buffer.add(false);
                    buffer.add(bestLength, 8);
                    buffer.add((int)(offset - bestLocation), 15);
                    StringTools::println("REF: (%d, %d)", bestLength, (int)(offset-bestLocation));

                    index += bestLength-1;
                    offset += bestLength;
                }
                else
                {
                    //couldn't find match within max allowed distance
                    buffer.add(true);
                    buffer.add(lzBuffer[index], 8);
                    StringTools::println("LIT: %d", lzBuffer[index]);
                    offset++;
                }
            }

            index++;
        }

        if(offset > startOffset)
        {
            std::vector<unsigned char> newLZBuffer;
            
            for(int i=offset-startOffset; i<lzBuffer.size(); i++)
            {
                newLZBuffer.push_back(lzBuffer[i]);
            }
            lzBuffer = newLZBuffer;
        }
        
	}

	void StreamCompressionLZSS::addDataDecompression(unsigned char* data, int length)
	{
        size_t index = 0;
        while(true)
        {
            //read bit
            if(leftovers.size() >= 9)
            {
                bool type = leftovers.getBit(0);
                if(type == false)
                {
                    //ref pair - 24 bits
                    while(index < length && leftovers.size() < 24)
                    {
                        leftovers.add(data[index]);
                        index++;
                    }
                    if(leftovers.size() >= 24)
                    {
                        int copyLen = leftovers.getBits(1, 9) + 3;
                        int backDis = leftovers.getBits(10, 24);

                        //do copy into buffer
                        for(int i=0; i<copyLen; i++)
                        {
                            buffer.getByteRef().push_back( backBuffer[backBuffLocation - backDis] );
                            backBuffer[backBuffLocation] = backBuffer[backBuffLocation - backDis];
                            backBuffLocation++;
                        }

                        //remove from leftovers
                        int actualLeftovers = leftovers.size() - 24;
                        BinarySet nLeftovers;
                        for(int i=actualLeftovers; i<leftovers.size(); i++)
                        {
                            nLeftovers.add(leftovers.getBit(i));
                        }
                        leftovers = nLeftovers;
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    //literal - 9 bits
                    int lit = leftovers.getBits(1, 9);

                    //write into buffer
                    buffer.getByteRef().push_back(lit);
                    backBuffer[backBuffLocation] = lit;
                    backBuffLocation++;
                    
                    //remove from leftovers
                    int actualLeftovers = leftovers.size() - 9;
                    BinarySet nLeftovers;
                    for(int i=actualLeftovers; i<leftovers.size(); i++)
                    {
                        nLeftovers.add(leftovers.getBit(i));
                    }
                    leftovers = nLeftovers;
                }
            }
            else
            {
                if(index >= length)
                    break;
                
                leftovers.add(data[index]);
                index++;
            }
        }
	}

    void StreamCompressionLZSS::endData()
    {
        size_t index = 0;
        while(index < lzBuffer.size()-2)
        {
            int v = ((int)lzBuffer[index+2] << 16) | ((int)lzBuffer[index+1] << 8) | (lzBuffer[index]);
            HashPair<int, int>* k = map.get(v);
            map.add(v, index);

            if(k == nullptr)
            {
                buffer.add(true);
                buffer.add(lzBuffer[index], 8);
            }
            else
            {
                //do more
                int lowestPoint = __max(offset-maxBackDist, 0);
                int bestLength = 0;
                int bestLocation = 0;
                unsigned char* startMatch = (lzBuffer.data()+index);
                
                do
                {
                    int locationOfMatch = k->data;
                    if(locationOfMatch < lowestPoint)
                        break;
                    
                    unsigned char* startBase = (lzBuffer.data()+(offset - locationOfMatch));

                    int len = 3;
                    while(len < maxLength && startMatch[len] == startBase[len])
                    {
                        len++;
                    }

                    if(len >= bestLength)
                    {
                        bestLength = len;
                        bestLocation = locationOfMatch;
                    }

                    k = map.getNext();
                } while (k != nullptr);
                
                //always insert
                int tInd = index+1;
                for(int j=1; j<bestLength-2; j++)
                {
                    int nkey = lzBuffer[tInd] + ((int)lzBuffer[tInd+1]<<8) + ((int)lzBuffer[tInd+2]<<16);
                    map.add( nkey, tInd );
                    tInd++;
                }

                if(bestLength>=3)
                {
                    buffer.add(false);
                    buffer.add(bestLength, 8);
                    buffer.add((int)(offset - bestLocation), 15);

                    index += bestLength-1;
                    offset += bestLength-1;
                }
                else
                {
                    //couldn't find match within max allowed distance
                    buffer.add(true);
                    buffer.add(lzBuffer[index], 8);
                }
            }

            index++;
            offset++;
        }
        
		int remainder = lzBuffer.size() - index;
		for(int j=0; j<remainder; j++)
		{
            buffer.add(true);
            buffer.add(lzBuffer[index + j]);
		}

        lzBuffer.clear();
    }

	BinarySet& StreamCompressionLZSS::getBuffer()
	{
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
}