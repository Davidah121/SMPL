#include "InternalCompressionHeader.h"

namespace smpl
{
	StreamCompressionRLE::StreamCompressionRLE(bool mode)
	{
		this->mode = mode;
	}

	StreamCompressionRLE::~StreamCompressionRLE()
	{
		clearBuffer();
	}
	
	void StreamCompressionRLE::addData(unsigned char* data, int length)
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

	void StreamCompressionRLE::addDataCompression(unsigned char* data, int length)
	{
		//go over each character and compare to the last byte known
		//update run or add new run pair if required.
		if(data == nullptr || length == 0)
		{
			return;
		}

		for(int i=0; i<length; i++)
		{
			if(run == 0)
			{
				byte = data[i];
				run = 1;
				//add new run
				buffer.push_back(run);
				buffer.push_back(byte);
			}
			else
			{
				if(data[i] == byte)
				{
					if(run < 0xFF)
					{
						run++;
						//update run info
						buffer[buffer.size()-2] = run;
					}
					else
					{
						//update run info
						buffer[buffer.size()-2] = run;
						byte = 0;
						run = 0;
					}
				}
			}
		}
	}

	void StreamCompressionRLE::addDataDecompression(unsigned char* data, int length)
	{
		//Deal with leftovers if necessary. If there are leftovers from the previous time,
		//there will be a run of at least 1.
		if(data == nullptr || length == 0)
		{
			return;
		}

		for(int i=0; i<length; i++)
		{
			if(run == 0)
			{
				run = data[i];
			}
			else
			{
				byte = data[i];

				for(int k=0; k<run; k++)
				{
					buffer.push_back(byte);
				}
				byte = 0;
				run = 0;
			}
		}
	}

	std::vector<unsigned char>& StreamCompressionRLE::getBuffer()
	{
		return buffer;
	}

	void StreamCompressionRLE::clearBuffer()
	{
		buffer.clear();
		run = 0;
		byte = 0;
	}

	size_t StreamCompressionRLE::size()
	{
		return buffer.size();
	}
};