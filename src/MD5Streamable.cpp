#include "Cryptography.h"
#include "BitwiseTools.h"

namespace smpl
{
	//Constants used in MD5 hash calculations
	const unsigned int shiftAmountsPerRound[64] = {7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,
												   5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
												   4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
												   6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21};

	//sine of integers
	const unsigned int K[64] = {0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
								0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
								0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
								0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
								0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
								0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
								0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
								0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
								0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
								0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
								0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
								0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
								0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
								0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
								0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
								0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391};

	//Internal functions for MD5 hash taken directly from the specifications: https://www.ietf.org/rfc/rfc1321.txt
	unsigned int MD5_InternalF(unsigned int b, unsigned int c, unsigned int d)
	{
		return (b & c) | ((~b) & d);
	}
	unsigned int MD5_InternalG(unsigned int b, unsigned int c, unsigned int d)
	{
		return (b & d) | (c & (~d));
	}
	unsigned int MD5_InternalH(unsigned int b, unsigned int c, unsigned int d)
	{
		return b ^ c ^ d;
	}
	unsigned int MD5_InternalI(unsigned int b, unsigned int c, unsigned int d)
	{
		return c ^ (b | (~d));
	}

	MD5Streamable::MD5Streamable()
	{
		digest = {0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476};
	}
	MD5Streamable::~MD5Streamable()
	{

	}

	void MD5Streamable::addData(unsigned char* buffer, size_t s)
	{
		if(endFlagSet)
			throw std::runtime_error("End flag set. No longer accepting more data for MD5 HASH");
		
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

	void MD5Streamable::endData()
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
		((size_t*)leftoverBuffer.data())[7] = totalSizeAccumulated*8;
		
		computeChunk((unsigned int*)leftoverBuffer.data());
		if(leftoverBuffer.size() > 64)
			computeChunk((unsigned int*)(leftoverBuffer.data() + 64)); //cheating but correct due to padding.
		leftoverBuffer.clear();
	}

	void MD5Streamable::computeChunk(unsigned int* buffer)
	{
		unsigned int A = digest[0];
		unsigned int B = digest[1];
		unsigned int C = digest[2];
		unsigned int D = digest[3];
		for(int i=0; i<64; i++)
		{
			unsigned int fVal, gVal;
			if(i <= 15)
			{
				fVal = MD5_InternalF(B, C, D);
				gVal = i;
			}
			else if(i <= 31)
			{
				fVal = MD5_InternalG(B, C, D);
				gVal = (5*i + 1) % 16;
			}
			else if(i <= 47)
			{
				fVal = MD5_InternalH(B, C, D);
				gVal = (3*i + 5) % 16;
			}
			else
			{
				fVal = MD5_InternalI(B, C, D);
				gVal = (7*i) % 16;
			}
			fVal += A + K[i] + buffer[gVal];
			A = D;
			D = C;
			C = B;
			B += BitwiseTools::leftRotate<unsigned int>(fVal, shiftAmountsPerRound[i]);
		}

		digest[0] += A;
		digest[1] += B;
		digest[2] += C;
		digest[3] += D;
	}

	std::vector<unsigned int> MD5Streamable::getOutputHash()
	{
		std::vector<unsigned int> copyDigest = digest;
		for(int i=0; i<copyDigest.size(); i++)
			copyDigest[i] = BitwiseTools::byteSwap(copyDigest[i]);
		return copyDigest;
	}
}