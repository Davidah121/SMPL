#pragma once
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <cstring>


//All datatypes that you'd expect to exist
#ifndef ALWAYS_INLINE
#define ALWAYS_INLINE __attribute__((__always_inline__,__nodebug__))
#endif

namespace smpl
{
	template<typename T>
	T min(const T& a, const T& b)
	{
		return (a < b) ? a : b;
	}

	template<typename T>
	T max(const T& a, const T& b)
	{
		return (a > b) ? a : b;
	}
}