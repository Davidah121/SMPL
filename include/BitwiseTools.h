#pragma once
#include <iostream>
#include "BuildOptions.h"
namespace smpl
{
	class DLL_OPTION BitwiseTools
	{
	public:
		/**
		 * @brief Performs a bitwise left rotate on the data type.
		 * 
		 * @tparam T 
		 * @param n 
		 * @param d 
		 * @return T 
		 */
		template<typename T>
		static T leftRotate(T n, unsigned int d)
		{
			return (n << d) | (n >> ((sizeof(T)*8) - d));
		}

		/**
		 * @brief Performs a bitwise right rotate on the data type.
		 * 
		 * @tparam T 
		 * @param n 
		 * @param d 
		 * @return T 
		 */
		template<typename T>
		static T rightRotate(T n, unsigned int d)
		{
			return (n >> d) | (n << ((sizeof(T)*8) - d));
		}

		/**
		 * @brief Swaps the order of the bytes in a short (16 bits).
		 * 		It is equivalent to a left or right rotate by 8 bits.
		 * @param v 
		 * @return short 
		 */
		static short byteSwap(short v);

		/**
		 * @brief Swaps the order of the bytes in an integer (32 bits).
		 * 		Example:
		 * 			0x11223344 -> 0x44332211
		 * @param v 
		 * @return int 
		 */
		static int byteSwap(int v);

		/**
		 * @brief Swaps the order of the bytes in a size_t or long (64 bits).
		 * 
		 * @param v 
		 * @return size_t 
		 */
		static size_t byteSwap(size_t v);

		/**
		 * @brief Swaps the order of the bytes where the size is 8bit, 16bit, 32bit, or 64bit.
		 * 		All other sizes return the same value. Assumes that the value can be cast into a short, int, and size_t
		 * 
		 * @tparam T 
		 * @param v 
		 * @return T 
		 */
		template<typename T>
		static T byteSwap(T v)
		{
			switch(sizeof(T))
			{
				case 1:
					return v;
				case 2:
					return (T)byteSwap((short)v);
				case 4:
					return (T)byteSwap((int)v);
				case 8:
					return (T)byteSwap((size_t)v);
				default:
					return v;
			}
			return v;
		}
	private:
	};
}