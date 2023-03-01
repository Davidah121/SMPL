#include "Cryptography.h"
#include "StringTools.h"

namespace glib
{
	std::vector<uint32_t> Cryptography::SHA1(unsigned char* bytes, size_t size)
	{
		std::vector<unsigned char> block = std::vector<unsigned char>(64);
		uint32_t h0 = 0x67452301;
		uint32_t h1 = 0xEFCDAB89;
		uint32_t h2 = 0x98BADCFE;
		uint32_t h3 = 0x10325476;
		uint32_t h4 = 0xC3D2E1F0;
		
		int index = 0;
		size_t ml = size*8;

		int padding = 64 - (size+9) % 64; //1 byte for 0x80 and 8 bytes for the size in bits.
		if(padding == 64)
			padding = 0;

		size_t totalBytesToProc = padding + size;

		bool setOneBit = false;
		while(index < totalBytesToProc)
		{
			memset(block.data(), 0, 64);

			int readSize = (size-index);
			if(readSize > 64)
				readSize = 64;
			if(readSize < 0)
				readSize = 0;

			int addedPadding = 64 - readSize;

			if(index < size)
				memcpy(block.data(), &bytes[index], readSize);
			
			index += 64; //always add 64 since we 'pad' the input to be divisible by 64.
			//fill remaining blocks
			if(addedPadding > 0)
			{
				//bytes after index+readSize = 0x80
				//last 8 bytes are ml
				//may need additional block.
				if(!setOneBit)
				{
					setOneBit = true;
					block[readSize] = 0x80;
					addedPadding--;
				}

				if(addedPadding >= 8)
				{
					((uint64_t*)block.data())[7] = glib::StringTools::byteSwap(ml);
				}
			}
			
			uint32_t* blockAsChunk = (uint32_t*)block.data();

			std::vector<unsigned int> w;
			for(int i=0; i<16; i++)
			{
				unsigned int temp = glib::StringTools::byteSwap(blockAsChunk[i]);
				w.push_back(temp);
			}
			
			//extend w into 80 uint32_t
			for(int i=16; i<80; i++)
			{
				uint32_t v = _rotl(w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16], 1);
				w.push_back(v);
			}

			uint32_t a = h0;
			uint32_t b = h1;
			uint32_t c = h2;
			uint32_t d = h3;
			uint32_t e = h4;

			uint32_t f,k;

			for(int i=0; i<80; i++)
			{
				if(i < 20)
				{
					f = (b & c) ^ ((~b) & d);
					k = 0x5A827999;
				}
				else if(i >=20 && i<40)
				{
					f = b ^ c ^ d;
					k = 0x6ED9EBA1;
				}
				else if(i>=40 && i<60)
				{
					f = (b & c) ^ (b & d) ^ (c & d);
					k = 0x8F1BBCDC;
				}
				else if(i>=60)
				{
					f = b ^ c ^ d;
					k = 0xCA62C1D6;
				}
				uint32_t temp = _rotl(a, 5) + f + e + k + w[i];
				e = d;
				d = c;
				c = _rotl(b, 30);
				b = a;
				a = temp;
			}

			h0 += a;
			h1 += b;
			h2 += c;
			h3 += d;
			h4 += e;
		}

		return {h0, h1, h2, h3, h4};
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