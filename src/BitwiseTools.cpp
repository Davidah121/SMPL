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
}