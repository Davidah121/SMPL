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
		std::vector<unsigned char> output = std::vector<unsigned char>();

		if(data == nullptr)
		{
			#ifdef USE_EXCEPTIONS
			throw InvalidDataError();
			#endif
			return output;
		}

		if (size > 0)
		{
			unsigned char lastValue = data[0];
			unsigned char value = 0;
			int run = 1;

			for (int i = 0; i < size; i++)
			{
				value = data[i];

				if (value == lastValue)
				{
					run++;
				}
				else
				{
					//write
					output.push_back((unsigned char)run);
					output.push_back(lastValue);

					run = 1;
					lastValue = value;
				}

				if (run == 255)
				{
					//write
					output.push_back((unsigned char)run);
					output.push_back(lastValue);
					run = 0;
				}
			}

			output.push_back((unsigned char)run);
			output.push_back(lastValue);
		}
		else
		{
			#ifdef USE_EXCEPTIONS
			throw InvalidSizeError();
			#endif
		}

		return output;
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

		while (i < size)
		{
			int run = data[i];
			int value = data[i + 1];

			for (int k = 0; k < run; k++)
			{
				uncompressedData.push_back(value);
			}

			i += 2;

			if(uncompressedData.size() > expectedSize)
			{
				#ifdef USE_EXCEPTIONS
				throw ExceededExpectedSizeError();
				#endif
				return std::vector<unsigned char>();
			}
		}

		return uncompressedData;
	}

#pragma endregion

} //NAMESPACE glib END