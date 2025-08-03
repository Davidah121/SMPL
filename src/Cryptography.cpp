#include "Cryptography.h"
#include "StringTools.h"
#include "SimpleFile.h"
#include "Concurrency.h"

namespace smpl
{
	std::vector<uint32_t> Cryptography::MD5(unsigned char* bytes, size_t size)
	{
		MD5Streamable streamableBuffer = MD5Streamable();
		streamableBuffer.addData(bytes, size);
		streamableBuffer.endData();
		return streamableBuffer.getOutputHash();
	}

	std::vector<uint32_t> Cryptography::MD5File(File filename)
	{
		unsigned char tempBuffer[4096];
		SimpleFile f = SimpleFile(filename, SimpleFile::READ);
		if(f.isOpen())
		{
			MD5Streamable streamableBuffer = MD5Streamable();
			while(f.isOpen())
			{
				size_t readSize = f.readBytes((char*)tempBuffer, 4096);
				if(f.isEndOfFile())
					break;
				streamableBuffer.addData(tempBuffer, readSize);
			}
			streamableBuffer.endData();
			return streamableBuffer.getOutputHash();
		}

		return {};
	}

	std::vector<uint32_t> Cryptography::SHA1(unsigned char* bytes, size_t size)
	{
		SHA1Streamable streamableBuffer = SHA1Streamable();
		streamableBuffer.addData(bytes, size);
		streamableBuffer.endData();
		return streamableBuffer.getOutputHash();
	}

	std::vector<uint32_t> Cryptography::SHA1File(File filename)
	{
		unsigned char tempBuffer[4096];
		SimpleFile f = SimpleFile(filename, SimpleFile::READ);
		if(f.isOpen())
		{
			SHA1Streamable streamableBuffer = SHA1Streamable();
			while(f.isOpen())
			{
				size_t readSize = f.readBytes((char*)tempBuffer, 4096);
				if(f.isEndOfFile())
					break;
				streamableBuffer.addData(tempBuffer, readSize);
			}
			streamableBuffer.endData();
			return streamableBuffer.getOutputHash();
		}

		return {};
	}

	std::vector<uint32_t> Cryptography::SHA2(unsigned char type, unsigned char* bytes, size_t size)
	{
		SHA2Streamable streamableBuffer = SHA2Streamable(type);
		streamableBuffer.addData(bytes, size);
		streamableBuffer.endData();
		return streamableBuffer.getOutputHash();
	}

	std::vector<uint32_t> Cryptography::SHA2File(unsigned char type, File filename)
	{
		unsigned char tempBuffer[4096];
		SimpleFile f = SimpleFile(filename, SimpleFile::READ);
		if(f.isOpen())
		{
			SHA2Streamable streamableBuffer = SHA2Streamable(type);
			while(f.isOpen())
			{
				size_t readSize = f.readBytes((char*)tempBuffer, 4096);
				if(f.isEndOfFile())
					break;
				streamableBuffer.addData(tempBuffer, readSize);
			}
			streamableBuffer.endData();
			return streamableBuffer.getOutputHash();
		}

		return {};
	}
	
	unsigned int Cryptography::adler32(std::vector<unsigned char> data)
	{
		return Cryptography::adler32(data.data(), data.size());
	}

	unsigned int Cryptography::adler32(unsigned char* data, size_t size)
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
		for(size_t i=0; i<size; i++)
		{
			sumA = (sumA + data[i]) % 65521;
			sumB = (sumB + sumA) % 65521;
		}

		return sumB*65536 + sumA;
	}

	unsigned int Cryptography::crc(std::vector<unsigned char> data, unsigned char type)
	{
		return Cryptography::crc(data.data(), data.size(), type);
	}

	unsigned int Cryptography::crc(unsigned char* data, size_t size, unsigned char type)
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
}