#pragma once
#include <cstdint>
#include <iostream>
#include <type_traits>
#include <vector>

#include "external/rapidhash.h"


namespace smpl 
{
	//base case for all numbers - NO IDENTITY. Faster if you don't care but in that case, use std::hash
	constexpr inline uint64_t rapidhash_wrapper(const uint64_t& key)
	{
		return rapid_mix(key, UINT64_C(0x9E3779B97F4A7C15));
	}


	//base case for all objects
	template<typename T>
	typename std::enable_if<!std::is_integral<T>::value, uint64_t>::type
	constexpr inline rapidhash_wrapper(const T& key)
	{
		return rapidhash(&key, sizeof(T));
	}

	//for all integer types, you can just zero extend up to 64bits
	template<typename T>
	typename std::enable_if<std::is_integral<T>::value, uint64_t>::type
	constexpr inline rapidhash_wrapper(const T& key)
	{
		return rapidhash_wrapper((uint64_t)key);
	}

	//process each character in the strings for any string type that extends std::basic_string (std::string, std::wstring)
	template<typename T>
	constexpr inline uint64_t rapidhash_wrapper(const std::basic_string<T>& key)
	{
		return rapidhash(key.data(), key.size()*sizeof(T));
	}
	template<typename T>
	constexpr inline uint64_t rapidhash_wrapper(const std::basic_string_view<T>& key)
	{
		return rapidhash(key.data(), key.size()*sizeof(T));
	}

	//for floats and doubles, you want to preserve all the fractional bits too so casting directly is not ideal
	//type pun in order to preserve all the bits
	constexpr inline uint64_t rapidhash_wrapper(const float& key)
	{
		// uint64_t k = *((uint32_t*)&key);
		return rapidhash_wrapper(*((uint32_t*)&key));
	}
	constexpr inline uint64_t rapidhash_wrapper(const double& key)
	{
		// uint64_t k = *((uint64_t*)&key);
		return rapidhash_wrapper(*((uint64_t*)&key));
	}

	template<typename K>
	struct RapidHash
	{
		std::size_t operator()(const K& k) const noexcept
		{
			return rapidhash_wrapper(k);
		}
	};
}