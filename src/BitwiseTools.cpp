#include "BitwiseTools.h"
#ifdef _WIN32	
	#include<Windows.h>
#endif
#ifdef __unix__
	#include <byteswap.h>
#endif

namespace smpl
{
	short BitwiseTools::byteSwap(short v)
	{
		#ifdef __unix__
			return BitwiseTools::leftRotate(v, 8);
		#else
			return _byteswap_ushort(v);
		#endif
	}

	int BitwiseTools::byteSwap(int v)
	{
		#ifdef __unix__
			return bswap_32(v);
		#endif

		#ifdef _WIN32
			return _byteswap_ulong(v);
		#endif
	}

	size_t BitwiseTools::byteSwap(size_t v)
	{
		#ifdef __unix__
			return bswap_64(v);
		#endif

		#ifdef _WIN32
			return _byteswap_uint64(v);
		#endif
	}

	static const unsigned char lookup[16] = {
	0x0, 0x8, 0x4, 0xC, 0x2, 0xA, 0x6, 0xE,
	0x1, 0x9, 0x5, 0xD, 0x3, 0xB, 0x7, 0xF, };

	char BitwiseTools::bitSwap(char v)
	{
		return (lookup[v&0b1111]<<4) | lookup[v>>4];
	}
	unsigned char BitwiseTools::bitSwap(unsigned char v)
	{
		return (lookup[v&0b1111]<<4) | lookup[v>>4];
	}
}