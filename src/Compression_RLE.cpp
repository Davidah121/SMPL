#include "InternalCompressionHeader.h"

namespace glib
{
#pragma region RLE

	std::vector<unsigned char> Compression::compressRLE(std::vector<unsigned char> data)
	{
		return Compression::compressRLE(data.data(), data.size());
	}

	std::vector<unsigned char> Compression::compressRLE(unsigned char* data, size_t size)
	{
		//Runs that are 1 byte in size and data that is 1 byte in size

		if(data == nullptr)
		{
			#ifdef USE_EXCEPTIONS
			throw InvalidDataError();
			#endif
			return {};
		}

		if (size == 0)
		{
			#ifdef USE_EXCEPTIONS
			throw InvalidSizeError();
			#endif
			return {};
		}
		StreamCompressionRLE stream = StreamCompressionRLE(StreamCompressionRLE::TYPE_COMPRESSION);
		stream.addData(data, size);
		return stream.getBuffer();
	}

	std::vector<unsigned char> Compression::decompressRLE(std::vector<unsigned char> data, size_t expectedSize)
	{
		return decompressRLE(data.data(), data.size(), expectedSize);
	}

	std::vector<unsigned char> Compression::decompressRLE(unsigned char* data, size_t size, size_t expectedSize)
	{
		std::vector<unsigned char> uncompressedData = std::vector<unsigned char>();

		int i = 0;

		if(size<=0)
		{
			#ifdef USE_EXCEPTIONS
			throw InvalidSizeError();
			#endif
		}

		if(data == nullptr)
		{
			#ifdef USE_EXCEPTIONS
			throw InvalidDataError();
			#endif
			return uncompressedData;
		}

		StreamCompressionRLE stream = StreamCompressionRLE(StreamCompressionRLE::TYPE_DECOMPRESSION);
		stream.addData(data, size);

		// for(int i=0; i<size; i+=4096)
		// {
		// 	int nSize = ((size - i) > 4096) ? 4096 : (size-i);
		// 	stream.addData(&data[i], nSize);
		// 	if(stream.size() > expectedSize)
		// 	{
		// 		#ifdef USE_EXCEPTIONS
		// 		throw ExceededExpectedSizeError();
		// 		#endif
		// 		return {}; //EXCEEDED EXPECTED SIZE
		// 	}
		// }
		return stream.getBuffer();
	}

#pragma endregion

} //NAMESPACE glib END