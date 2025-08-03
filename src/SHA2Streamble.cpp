#include "Cryptography.h"
#include "BitwiseTools.h"

namespace smpl
{

	unsigned int K_256[64] = {
		0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
		0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
		0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
		0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
		0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
		0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
		0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
		0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
	};

	uint64_t K_512[80] = {
		0x428a2f98d728ae22, 0x7137449123ef65cd, 0xb5c0fbcfec4d3b2f, 0xe9b5dba58189dbbc, 0x3956c25bf348b538, 
		0x59f111f1b605d019, 0x923f82a4af194f9b, 0xab1c5ed5da6d8118, 0xd807aa98a3030242, 0x12835b0145706fbe, 
		0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2, 0x72be5d74f27b896f, 0x80deb1fe3b1696b1, 0x9bdc06a725c71235, 
		0xc19bf174cf692694, 0xe49b69c19ef14ad2, 0xefbe4786384f25e3, 0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65, 
		0x2de92c6f592b0275, 0x4a7484aa6ea6e483, 0x5cb0a9dcbd41fbd4, 0x76f988da831153b5, 0x983e5152ee66dfab, 
		0xa831c66d2db43210, 0xb00327c898fb213f, 0xbf597fc7beef0ee4, 0xc6e00bf33da88fc2, 0xd5a79147930aa725, 
		0x06ca6351e003826f, 0x142929670a0e6e70, 0x27b70a8546d22ffc, 0x2e1b21385c26c926, 0x4d2c6dfc5ac42aed, 
		0x53380d139d95b3df, 0x650a73548baf63de, 0x766a0abb3c77b2a8, 0x81c2c92e47edaee6, 0x92722c851482353b, 
		0xa2bfe8a14cf10364, 0xa81a664bbc423001, 0xc24b8b70d0f89791, 0xc76c51a30654be30, 0xd192e819d6ef5218, 
		0xd69906245565a910, 0xf40e35855771202a, 0x106aa07032bbd1b8, 0x19a4c116b8d2d0c8, 0x1e376c085141ab53, 
		0x2748774cdf8eeb99, 0x34b0bcb5e19b48a8, 0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb, 0x5b9cca4f7763e373, 
		0x682e6ff3d6b2b8a3, 0x748f82ee5defb2fc, 0x78a5636f43172f60, 0x84c87814a1f0ab72, 0x8cc702081a6439ec, 
		0x90befffa23631e28, 0xa4506cebde82bde9, 0xbef9a3f7b2c67915, 0xc67178f2e372532b, 0xca273eceea26619c, 
		0xd186b8c721c0c207, 0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178, 0x06f067aa72176fba, 0x0a637dc5a2c898a6, 
		0x113f9804bef90dae, 0x1b710b35131c471b, 0x28db77f523047d84, 0x32caab7b40c72493, 0x3c9ebe0a15c9bebc, 
		0x431d67c49c100d4c, 0x4cc5d4becb3e42b6, 0x597f299cfc657e2a, 0x5fcb6fab3ad6faec, 0x6c44198c4a475817
	};

	SHA2Streamable::SHA2Streamable(unsigned char type)
	{
		this->type = type;
		switch(type)
		{
		case SHA2Streamable::SHA_224:
			digest = {0xc1059ed8, 0x367cd507, 0x3070dd17, 0xf70e5939, 0xffc00b31, 0x68581511, 0x64f98fa7, 0xbefa4fa4};
			break;
		case SHA2Streamable::SHA_256:
			digest = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
			break;
		case SHA2Streamable::SHA_384:
			digest64 = {0xcbbb9d5dc1059ed8, 0x629a292a367cd507, 0x9159015a3070dd17, 0x152fecd8f70e5939, 0x67332667ffc00b31, 0x8eb44a8768581511, 0xdb0c2e0d64f98fa7, 0x47b5481dbefa4fa4};
			break;
		case SHA2Streamable::SHA_512:
			digest64 = {0x6a09e667f3bcc908, 0xbb67ae8584caa73b, 0x3c6ef372fe94f82b, 0xa54ff53a5f1d36f1, 0x510e527fade682d1, 0x9b05688c2b3e6c1f, 0x1f83d9abfb41bd6b, 0x5be0cd19137e2179};
			break;
		default:
			digest = {0xc1059ed8, 0x367cd507, 0x3070dd17, 0xf70e5939, 0xffc00b31, 0x68581511, 0x64f98fa7, 0xbefa4fa4};
			type = 0;
			break;
		}
	}
	SHA2Streamable::~SHA2Streamable()
	{

	}

	void SHA2Streamable::addData(unsigned char* buffer, uint64_t s)
	{
		if(endFlagSet)
			throw std::runtime_error("End flag set. No longer accepting more data for SHA2 HASH");
		
		for(uint64_t i=0; i<s; i++)
		{
			leftoverBuffer.push_back(buffer[i]);

			if(type==SHA_384 || type==SHA_512)
			{
				if(leftoverBuffer.size() == 128)
				{
					computeChunk512((uint64_t*)leftoverBuffer.data());
					leftoverBuffer.clear();
				}
			}
			else
			{
				if(leftoverBuffer.size() == 64)
				{
					computeChunk256((unsigned int*)leftoverBuffer.data());
					leftoverBuffer.clear();
				}
			}
		}

		totalSizeAccumulated += s;
	}

	void SHA2Streamable::endData()
	{
		//pad and then compute chunk one last time. That is the final result
		//always pad to 448 bits even if the message is already 448 bits (add another 512 bits)

		leftoverBuffer.push_back(0x80); //must add this
		if(type == SHA_384 || type == SHA_512)
		{
			while(leftoverBuffer.size() % 128 != 120) //Should actually be 112 but we don't have a size value nor will we need one that is 128 bit
			{
				leftoverBuffer.push_back(0x00);
			}
			for(int i=0; i<8; i++)
				leftoverBuffer.push_back(0);
		
			//add size
			((size_t*)leftoverBuffer.data())[15] = BitwiseTools::byteSwap(totalSizeAccumulated*8);
			
			
			computeChunk512((uint64_t*)leftoverBuffer.data());
			if(leftoverBuffer.size() > 128)
				computeChunk512((uint64_t*)(leftoverBuffer.data() + 128)); //cheating but correct due to padding.
			leftoverBuffer.clear();
		}
		else
		{
			while(leftoverBuffer.size() % 64 != 56)
			{
				leftoverBuffer.push_back(0x00);
			}
			for(int i=0; i<8; i++)
				leftoverBuffer.push_back(0);
		
			//add size
			((size_t*)leftoverBuffer.data())[7] = BitwiseTools::byteSwap(totalSizeAccumulated*8);
			
			computeChunk256((unsigned int*)leftoverBuffer.data());
			if(leftoverBuffer.size() > 64)
				computeChunk256((unsigned int*)(leftoverBuffer.data() + 64)); //cheating but correct due to padding.
			leftoverBuffer.clear();
		}

		if(type == SHA_224)
		{
			digest.pop_back();
		}
		else if(type == SHA_384)
		{
			digest64.pop_back();
			digest64.pop_back();
		}

		if(type == SHA_512 || type == SHA_384)
		{
			//convert 64 bit values into multiple 32 bit values
			digest.clear();
			unsigned int* digest64AsInts = (unsigned int*)digest64.data();
			for(int i=0; i<digest64.size(); i++)
			{
				digest.push_back(digest64AsInts[2*i+1]);
				digest.push_back(digest64AsInts[2*i]);
			}
		}
	}

	void SHA2Streamable::computeChunk256(unsigned int* buffer)
	{
		unsigned int A = digest[0];
		unsigned int B = digest[1];
		unsigned int C = digest[2];
		unsigned int D = digest[3];
		unsigned int E = digest[4];
		unsigned int F = digest[5];
		unsigned int G = digest[6];
		unsigned int H = digest[7];

		//extend buffer out to 64 integer values instead of 16
		std::vector<unsigned int> extBuffer = std::vector<unsigned int>(64);
		for(int i=0; i<16; i++)
			extBuffer[i] = BitwiseTools::byteSwap(buffer[i]);
			
		for(int i=16; i<64; i++)
		{
			unsigned int s0 = BitwiseTools::rightRotate(extBuffer[i-15], 7) ^ BitwiseTools::rightRotate(extBuffer[i-15], 18) ^ (extBuffer[i-15] >> 3);
			unsigned int s1 = BitwiseTools::rightRotate(extBuffer[i-2], 17) ^ BitwiseTools::rightRotate(extBuffer[i-2], 19) ^ (extBuffer[i-2] >> 10);
			extBuffer[i] = extBuffer[i-16] + s0 + extBuffer[i-7] + s1;
		}

		for(int i=0; i<64; i++)
		{
			unsigned int s0 = BitwiseTools::rightRotate(A, 2) ^ BitwiseTools::rightRotate(A, 13) ^ BitwiseTools::rightRotate(A, 22);
			unsigned int s1 = BitwiseTools::rightRotate(E, 6) ^ BitwiseTools::rightRotate(E, 11) ^ BitwiseTools::rightRotate(E, 25);
			unsigned int ch = (E & F) ^ ((~E) & G);
			unsigned int temp1 = H + s1 + ch + K_256[i] + extBuffer[i];
			unsigned int maj = (A & B) ^ (A & C) ^ (B & C);
			unsigned int temp2 = s0 + maj;

			H = G;
			G = F;
			F = E;
			E = D + temp1;
			D = C;
			C = B;
			B = A;
			A = temp1 + temp2;
		}

		digest[0] += A;
		digest[1] += B;
		digest[2] += C;
		digest[3] += D;
		digest[4] += E;
		digest[5] += F;
		digest[6] += G;
		digest[7] += H;
	}

	void SHA2Streamable::computeChunk512(uint64_t* buffer)
	{
		uint64_t A = digest64[0];
		uint64_t B = digest64[1];
		uint64_t C = digest64[2];
		uint64_t D = digest64[3];
		uint64_t E = digest64[4];
		uint64_t F = digest64[5];
		uint64_t G = digest64[6];
		uint64_t H = digest64[7];

		//extend buffer out to 64 integer values instead of 16
		std::vector<uint64_t> extBuffer = std::vector<uint64_t>(80);
		for(int i=0; i<16; i++)
			extBuffer[i] = BitwiseTools::byteSwap(buffer[i]);

		for(int i=16; i<80; i++)
		{
			uint64_t s0 = BitwiseTools::rightRotate(extBuffer[i-15], 1) ^ BitwiseTools::rightRotate(extBuffer[i-15], 8) ^ (extBuffer[i-15] >> 7);
			uint64_t s1 = BitwiseTools::rightRotate(extBuffer[i-2], 19) ^ BitwiseTools::rightRotate(extBuffer[i-2], 61) ^ (extBuffer[i-2] >> 6);
			extBuffer[i] = extBuffer[i-16] + s0 + extBuffer[i-7] + s1;
		}

		for(int i=0; i<80; i++)
		{
			uint64_t s0 = BitwiseTools::rightRotate(A, 28) ^ BitwiseTools::rightRotate(A, 34) ^ BitwiseTools::rightRotate(A, 39);
			uint64_t s1 = BitwiseTools::rightRotate(E, 14) ^ BitwiseTools::rightRotate(E, 18) ^ BitwiseTools::rightRotate(E, 41);
			uint64_t ch = (E & F) ^ (~E & G);
			uint64_t temp1 = H + s1 + ch + K_512[i] + extBuffer[i];
			uint64_t maj = (A & B) ^ (A & C) ^ (B & C);
			uint64_t temp2 = s0 + maj;

			H = G;
			G = F;
			F = E;
			E = D + temp1;
			D = C;
			C = B;
			B = A;
			A = temp1 + temp2;
		}

		digest64[0] += A;
		digest64[1] += B;
		digest64[2] += C;
		digest64[3] += D;
		digest64[4] += E;
		digest64[5] += F;
		digest64[6] += G;
		digest64[7] += H;
	}

	std::vector<unsigned int> SHA2Streamable::getOutputHash()
	{
		return digest;
	}
}