#include "InternalCompressionHeader.h"

namespace glib
{
	#pragma region CHECKSUMS

	unsigned int Compression::adler32(std::vector<unsigned char> data)
	{
		return Compression::adler32(data.data(), data.size());
	}

	unsigned int Compression::adler32(unsigned char* data, int size)
	{
		if(size <= 0)
		{
			#ifdef USE_EXCEPTIONS
			throw InvalidSizeError();
			#endif
			return 0;
		}

		if(data == nullptr)
		{
			#ifdef USE_EXCEPTIONS
			throw InvalidDataError();
			#endif
			return 0;
		}

		unsigned long sumA = 1;
		unsigned long sumB = 0;

		for(int i=0; i<size; i++)
		{
			sumA = (sumA + data[i]) % 65521;
			sumB = (sumB + sumA) % 65521;
		}

		return sumB*65536 + sumA;
	}

	unsigned int Compression::crc(std::vector<unsigned char> data, unsigned char type)
	{
		return Compression::crc(data.data(), data.size(), type);
	}

	unsigned int Compression::crc(unsigned char* data, int size, unsigned char type)
	{
		if(size <= 0)
		{
			#ifdef USE_EXCEPTIONS
			throw InvalidSizeError();
			#endif
			return 0;
		}

		if(data == nullptr)
		{
			#ifdef USE_EXCEPTIONS
			throw InvalidDataError();
			#endif
			return 0;
		}
		
		unsigned long crcTable[256];
		unsigned long polynomial = 0;
		unsigned int crcValue = 0xFFFFFFFF;

		switch(type)
		{
			case CRC_8:
				polynomial = 0xE0;
				crcValue = crcValue & 0xFF;
				break;
			case CRC_16:
				polynomial = 0xA001;
				crcValue = crcValue & 0xFFFF;
				break;
			case CRC_32:
				polynomial = 0xEDB88320;
				crcValue = crcValue & 0xFFFFFFFF;
				break;
			default:
				polynomial = 0xEDB88320;
				crcValue = crcValue & 0xFFFFFFFF;
				break;
		}

		//fill table
		for(int i=0; i<256; i++)
		{
			unsigned long remainder = i;
			for(int b=0; b<8; b++)
			{
				if(remainder & 0x01)
				{
					remainder = (remainder >> 1) ^ polynomial;
				}
				else
				{
					remainder = (remainder >> 1);
				}
			}

			crcTable[i] = remainder;
		}

		for(int i=0; i<size; i++)
		{
			crcValue = crcTable[ (data[i] ^ crcValue)&0xFF] ^ (crcValue>>8);
		}

		return ~crcValue;
	}



	#pragma endregion

} //NAMESPACE glib END