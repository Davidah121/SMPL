#include "Cryptography.h"
#include "BitwiseTools.h"

namespace smpl
{
	SHA1Streamable::SHA1Streamable()
	{
		digest = {0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0};
	}
	SHA1Streamable::~SHA1Streamable()
	{

	}

	void SHA1Streamable::addData(unsigned char* buffer, size_t s)
	{
		if(endFlagSet)
			throw std::runtime_error("End flag set. No longer accepting more data for SHA1 HASH");
		
		for(size_t i=0; i<s; i++)
		{
			leftoverBuffer.push_back(buffer[i]);
			if(leftoverBuffer.size() == 64)
			{
				computeChunk((unsigned int*)leftoverBuffer.data());
				leftoverBuffer.clear();
			}
		}

		totalSizeAccumulated += s;
	}

	void SHA1Streamable::endData()
	{
		//pad and then compute chunk one last time. That is the final result
		//always pad to 448 bits even if the message is already 448 bits (add another 512 bits)

		leftoverBuffer.push_back(0x80); //must add this
		while(leftoverBuffer.size() % 64 != 56)
		{
			leftoverBuffer.push_back(0x00);
		}
		for(int i=0; i<8; i++)
			leftoverBuffer.push_back(0);
		
		//add size
		((size_t*)leftoverBuffer.data())[7] = BitwiseTools::byteSwap(totalSizeAccumulated*8);
		

		computeChunk((unsigned int*)leftoverBuffer.data());
		if(leftoverBuffer.size() > 64)
			computeChunk((unsigned int*)(leftoverBuffer.data() + 64)); //cheating but correct due to padding.
		leftoverBuffer.clear();
	}

	void SHA1Streamable::computeChunk(unsigned int* buffer)
	{
		unsigned int A = digest[0];
		unsigned int B = digest[1];
		unsigned int C = digest[2];
		unsigned int D = digest[3];
		unsigned int E = digest[4];
		
		//extend buffer out to 80 integer values instead of 16
		std::vector<unsigned int> extBuffer = std::vector<unsigned int>(80);
		for(int i=0; i<16; i++)
			extBuffer[i] = BitwiseTools::byteSwap(buffer[i]);
		for(int i=16; i<80; i++)
			extBuffer[i] = BitwiseTools::leftRotate(extBuffer[i-3] ^ extBuffer[i-8] ^ extBuffer[i-14] ^ extBuffer[i-16], 1);

		for(int i=0; i<80; i++)
		{
			unsigned int fVal, kVal;
			if(i < 20)
			{
				fVal = (B & C) ^ ((~B) & D);
				kVal = 0x5A827999;
			}
			else if(i < 40)
			{
				fVal = B ^ C ^ D;
				kVal = 0x6ED9EBA1;
			}
			else if(i < 60)
			{
				fVal = (B & C) ^ (B & D) ^ (C & D);
				kVal = 0x8F1BBCDC;
			}
			else
			{
				fVal = B ^ C ^ D;
				kVal = 0xCA62C1D6;
			}
			unsigned int temp = BitwiseTools::leftRotate(A, 5) + fVal + E + kVal + extBuffer[i];
			E = D;
			D = C;
			C = BitwiseTools::leftRotate(B, 30);
			B = A;
			A = temp;
		}

		digest[0] += A;
		digest[1] += B;
		digest[2] += C;
		digest[3] += D;
		digest[4] += E;
	}

	std::vector<unsigned int> SHA1Streamable::getOutputHash()
	{
		return digest;
	}
}