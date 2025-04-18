#include "InternalCompressionHeader.h"

namespace smpl
{

#pragma region LZW

	#pragma region THREAD_FUNCTION
	void LZWThreadFunction(BinarySet* outputData, unsigned char* data, size_t size, int codeSize, bool omitEndCode)
	{
		Compression::compressLZW(outputData, data, size, codeSize, omitEndCode);
	}

	#pragma endregion
	std::vector<unsigned char> Compression::compressLZW(std::vector<unsigned char> data, int blocks, int codeSize)
	{
		return Compression::compressLZW(data.data(), data.size(), blocks, codeSize);
	}
	
	std::vector<unsigned char> Compression::compressLZW(unsigned char* data, size_t size, int blocks, int codeSize)
	{
		if(codeSize <= 0)
		{
			return {};
		}

		int totalBlocks = blocks;
		int blockSize = size;
		if(blocks > 1)
		{
			blockSize /= blocks;
			totalBlocks = blocks;
		}
		else
		{
			return compressLZW(data, size, codeSize);
		}

		int threadCount = (int)System::getNumberOfThreads();
		std::vector<BinarySet> blockDataArray = std::vector<BinarySet>(blocks);
		std::vector<std::thread> blockThreads = std::vector<std::thread>(threadCount);

		for(int i=0; i<blocks; i++)
		{
			int threadID = i%threadCount;
			unsigned char* inputData = data + blockSize*i;
			int inputSize = blockSize;
			if(i==blocks-1)
				inputSize = size - blockSize*i;

			if(i==0)
			{
				blockThreads[threadID] = std::thread(LZWThreadFunction, &blockDataArray[i], inputData, inputSize, codeSize, true);
			}
			else
			{
				if(blockThreads[threadID].joinable())
					blockThreads[threadID].join();
				
				bool omitEndCode = true;
				if(i == blocks-1)
				{
					omitEndCode = false;
				}

				blockThreads[threadID] = std::thread(LZWThreadFunction, &blockDataArray[i], inputData, inputSize, codeSize, omitEndCode);
			}
		}

		for(int i=0; i<threadCount; i++)
		{
			int threadID = i%threadCount;

			if(blockThreads[threadID].joinable())
				blockThreads[threadID].join();
		}

		BinarySet finalSet = blockDataArray[0];
		for(int i=1; i<blocks; i++)
		{
			finalSet.add(blockDataArray[i]);
		}

		return finalSet.toBytes();
	}

	std::vector<unsigned char> Compression::compressLZW(std::vector<unsigned char> data, int codeSize)
	{
		return Compression::compressLZW(data.data(), data.size(), codeSize);
	}

	void Compression::compressLZW(BinarySet* outputData, unsigned char* data, size_t size, int codeSize, bool omitEndCode)
	{
		StreamCompressionLZW stream = StreamCompressionLZW(StreamCompressionLZW::TYPE_COMPRESSION, codeSize);
		stream.addData(data, size);
		if(!omitEndCode)
			stream.addEndData();
		else
			stream.addClearDictionary(); //Must add to reset the dictionary for concatenation. Results in 2 clear dictionaries back to back. Should fix.

		outputData->clear();
		outputData->setValues( stream.getBuffer().getByteRef().data(), stream.getBuffer().getByteRef().size() );
		outputData->setNumberOfBits( stream.getBuffer().size() );
	}

	std::vector<unsigned char> Compression::compressLZW(unsigned char* data, size_t size, int codeSize)
	{
		if(data == nullptr)
		{
			#ifdef USE_EXCEPTIONS
			throw InvalidDataError();
			#endif
			return {};
		}
		if(size == 0)
		{
			#ifdef USE_EXCEPTIONS
			throw InvalidSizeError();
			#endif
			return {};
		}

		StreamCompressionLZW stream = StreamCompressionLZW(StreamCompressionLZW::TYPE_COMPRESSION, codeSize);
		stream.addData(data, size);
		stream.addEndData();

		return stream.getBuffer().toBytes();
	}

	std::vector<unsigned char> Compression::decompressLZW(std::vector<unsigned char> data, int dictionarySize, size_t expectedSize)
	{
		return Compression::decompressLZW(data.data(), data.size(), dictionarySize, expectedSize);
	}

	std::vector<unsigned char> Compression::decompressLZW(unsigned char* data, size_t size, int dictionarySize, size_t expectedSize)
	{
		StreamCompressionLZW stream = StreamCompressionLZW(StreamCompressionLZW::TYPE_DECOMPRESSION, dictionarySize);
		// stream.addData(data, size);
		for(int i=0; i<size; i+=4096)
		{
			int nSize = ((size - i) > 4096) ? 4096 : (size-i);
			stream.addData(&data[i], nSize);
			if(stream.size() > expectedSize)
			{
				#ifdef USE_EXCEPTIONS
				throw ExceededExpectedSizeError();
				#endif
				return {}; //EXCEEDED EXPECTED SIZE
			}

		}

		if(!stream.getEndData())
		{
			#ifdef USE_EXCEPTIONS
			throw InvalidDataStreamEndError();
			#endif
			return {}; //ERROR IN DATA
		}

		return stream.getBuffer().toBytes();
	}

	#pragma endregion

} //NAMESPACE glib END