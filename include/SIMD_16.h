#pragma once
#include "SIMD.h"

#if (OPTI >= 1)
//blend stuff
#define SIMD_128_U16_BLEND(x, y, imm) _mm_blend_epi16(x, y, imm)
#define SIMD_128_16_BLEND(x, y, imm) _mm_blend_epi16(x, y, imm)

class SIMD_128_U16
{
public:
	static const int SIZE = 8;
	static unsigned long long getSIMDBound(unsigned long long s);

	SIMD_128_U16();
	SIMD_128_U16(unsigned short byte);
	SIMD_128_U16(__m128i sseValue);
	
	SIMD_128_U16(const SIMD_128_U16& other);
	void operator=(const SIMD_128_U16& other);
	
	SIMD_128_U16(const SIMD_128_U16&& other) noexcept;
	void operator=(const SIMD_128_U16&& other) noexcept;
	
	~SIMD_128_U16();
	
	//load / store
	static SIMD_128_U16 load(unsigned short* pointer);
	void store(unsigned short* pointer);
	
	//arithmetic
	SIMD_128_U16 operator+(const SIMD_128_U16& other) const;
	SIMD_128_U16 operator-(const SIMD_128_U16& other) const;
	SIMD_128_U16 operator*(const SIMD_128_U16& other) const;
	SIMD_128_U16 operator/(const SIMD_128_U16& other) const;
	
	void operator+=(const SIMD_128_U16& other);
	void operator-=(const SIMD_128_U16& other);
	void operator*=(const SIMD_128_U16& other);
	void operator/=(const SIMD_128_U16& other);
	
	//memory access and modification
	
	//bitwise
	SIMD_128_U16 operator>>(const int shift) const;
	SIMD_128_U16 operator<<(const int shift) const;
	SIMD_128_U16 operator>>(const SIMD_128_U16& other) const;
	SIMD_128_U16 operator<<(const SIMD_128_U16& other) const;
	
	void operator>>=(const int shift);
	void operator<<=(const int shift);
	void operator>>=(const SIMD_128_U16& other);
	void operator<<=(const SIMD_128_U16& other);
	
	SIMD_128_U16 operator&(const int v) const;
	SIMD_128_U16 operator&(const SIMD_128_U16& other) const;
	
	void operator&=(const int v);
	void operator&=(const SIMD_128_U16& other);
	
	SIMD_128_U16 bitwiseAndNot(const int v) const;
	SIMD_128_U16 bitwiseAndNot(const SIMD_128_U16& other) const;
	
	void bitwiseAndNot(const int v);
	void bitwiseAndNot(const SIMD_128_U16& other);
	
	//comparison
	SIMD_128_U16 operator>(const unsigned short byte) const;
	SIMD_128_U16 operator>(const SIMD_128_U16& other) const;
	
	SIMD_128_U16 operator<(const unsigned short byte) const;
	SIMD_128_U16 operator<(const SIMD_128_U16& other) const;
	
	SIMD_128_U16 operator==(const unsigned short byte) const;
	SIMD_128_U16 operator==(const SIMD_128_U16& other) const;
	
	SIMD_128_U16 operator!=(const unsigned short byte) const;
	SIMD_128_U16 operator!=(const SIMD_128_U16& other) const;

	//cast / convert operations
	// SIMD_128_U8 convertToU8(int location) const;
	// SIMD_128_U32 convertToU32(int location) const;
	// SIMD_128_FP32 convertToFP32(int location) const;
	
	__m128i values;
};


class SIMD_128_16
{
public:
	static const int SIZE = 8;
	static unsigned long long getSIMDBound(unsigned long long s);

	SIMD_128_16();
	SIMD_128_16(short byte);
	SIMD_128_16(__m128i sseValue);
	
	SIMD_128_16(const SIMD_128_16& other);
	void operator=(const SIMD_128_16& other);
	
	SIMD_128_16(const SIMD_128_16&& other) noexcept;
	void operator=(const SIMD_128_16&& other) noexcept;
	
	~SIMD_128_16();
	
	//load / store
	static SIMD_128_16 load(short* pointer);
	void store(short* pointer);
	
	//arithmetic
	SIMD_128_16 operator+(const SIMD_128_16& other) const;
	SIMD_128_16 operator-(const SIMD_128_16& other) const;
	SIMD_128_16 operator*(const SIMD_128_16& other) const;
	SIMD_128_16 operator/(const SIMD_128_16& other) const;
	
	void operator+=(const SIMD_128_16& other);
	void operator-=(const SIMD_128_16& other);
	void operator*=(const SIMD_128_16& other);
	void operator/=(const SIMD_128_16& other);
	
	//memory access and modification
	
	//bitwise
	SIMD_128_16 operator>>(const int shift) const;
	SIMD_128_16 operator<<(const int shift) const;
	SIMD_128_16 operator>>(const SIMD_128_16& other) const;
	SIMD_128_16 operator<<(const SIMD_128_16& other) const;
	
	void operator>>=(const int shift);
	void operator<<=(const int shift);
	void operator>>=(const SIMD_128_16& other);
	void operator<<=(const SIMD_128_16& other);
	
	SIMD_128_16 operator&(const int v) const;
	SIMD_128_16 operator&(const SIMD_128_16& other) const;
	
	void operator&=(const int v);
	void operator&=(const SIMD_128_16& other);
	
	SIMD_128_16 bitwiseAndNot(const int v) const;
	SIMD_128_16 bitwiseAndNot(const SIMD_128_16& other) const;
	
	void bitwiseAndNot(const int v);
	void bitwiseAndNot(const SIMD_128_16& other);
	
	//comparison
	SIMD_128_16 operator>(const short byte) const;
	SIMD_128_16 operator>(const SIMD_128_16& other) const;
	
	SIMD_128_16 operator<(const short byte) const;
	SIMD_128_16 operator<(const SIMD_128_16& other) const;
	
	SIMD_128_16 operator==(const short byte) const;
	SIMD_128_16 operator==(const SIMD_128_16& other) const;
	
	SIMD_128_16 operator!=(const short byte) const;
	SIMD_128_16 operator!=(const SIMD_128_16& other) const;
	
	//cast / convert operations
	// SIMD_128_U8 convertToU8(int location) const;
	// SIMD_128_U32 convertToU32(int location) const;
	// SIMD_128_FP32 convertToFP32(int location) const;
	
	__m128i values;
};
#endif