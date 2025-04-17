#pragma once
#include "SIMD.h"

#if (OPTI >= 1)
#define SIMD_128_U8_BLEND(x, y, mask) _mm_blendv_epi8(x, y, mask)
#define SIMD_128_8_BLEND(x, y, mask) _mm_blendv_epi8(x, y, mask)
__m128i sse8HorizontalAdd(__m128i A, __m128i B);

class SIMD_128_U8
{
public:
	static const int SIZE = 16;
	static unsigned long long getSIMDBound(unsigned long long s);

	SIMD_128_U8();
	SIMD_128_U8(unsigned char byte);
	SIMD_128_U8(__m128i sseValue);
	
	SIMD_128_U8(const SIMD_128_U8& other);
	void operator=(const SIMD_128_U8& other);
	
	SIMD_128_U8(const SIMD_128_U8&& other) noexcept;
	void operator=(const SIMD_128_U8&& other) noexcept;
	
	~SIMD_128_U8();

	//load / store
	static SIMD_128_U8 load(unsigned char* pointer);
	void store(unsigned char* pointer);
	
	//arithmetic
	SIMD_128_U8 operator+(const SIMD_128_U8& other) const;
	SIMD_128_U8 operator-(const SIMD_128_U8& other) const;
	SIMD_128_U8 operator*(const SIMD_128_U8& other) const;
	SIMD_128_U8 operator/(const SIMD_128_U8& other) const;
	
	void operator+=(const SIMD_128_U8& other);
	void operator-=(const SIMD_128_U8& other);
	void operator*=(const SIMD_128_U8& other);
	void operator/=(const SIMD_128_U8& other);
	
	//memory access and modification
	
	//bitwise
	SIMD_128_U8 operator>>(const int shift) const;
	SIMD_128_U8 operator<<(const int shift) const;
	SIMD_128_U8 operator>>(const SIMD_128_U8& other) const;
	SIMD_128_U8 operator<<(const SIMD_128_U8& other) const;
	
	void operator>>=(const int shift);
	void operator<<=(const int shift);
	void operator>>=(const SIMD_128_U8& other);
	void operator<<=(const SIMD_128_U8& other);
	
	SIMD_128_U8 operator&(const int v) const;
	SIMD_128_U8 operator&(const SIMD_128_U8& other) const;
	
	void operator&=(const int v);
	void operator&=(const SIMD_128_U8& other);
	
	SIMD_128_U8 bitwiseAndNot(const int v) const;
	SIMD_128_U8 bitwiseAndNot(const SIMD_128_U8& other) const;
	
	void bitwiseAndNot(const int v);
	void bitwiseAndNot(const SIMD_128_U8& other);
	
	//comparison
	SIMD_128_U8 operator>(const unsigned char byte) const;
	SIMD_128_U8 operator>(const SIMD_128_U8& other) const;
	
	SIMD_128_U8 operator<(const unsigned char byte) const;
	SIMD_128_U8 operator<(const SIMD_128_U8& other) const;
	
	SIMD_128_U8 operator==(const unsigned char byte) const;
	SIMD_128_U8 operator==(const SIMD_128_U8& other) const;
	
	SIMD_128_U8 operator!=(const unsigned char byte) const;
	SIMD_128_U8 operator!=(const SIMD_128_U8& other) const;
	
	//special case functions
	SIMD_128_U8 horizontalAdd(const SIMD_128_U8& other) const;
	unsigned short sum() const;

	//cast / convert operations
	// SIMD_128_U16 convertToU16(int location) const;
	// SIMD_128_U32 convertToU32(int location) const;
	// SIMD_128_U64 convertToU64(int location) const;
	// SIMD_128_FP32 convertToFP32(int location) const;
	// SIMD_128_FP32 convertToFP64(int
	
	__m128i values;
};


class SIMD_128_8
{
public:
	static const int SIZE = 16;
	static unsigned long long getSIMDBound(unsigned long long s);

	SIMD_128_8();
	SIMD_128_8(char byte);
	SIMD_128_8(__m128i sseValue);
	
	SIMD_128_8(const SIMD_128_8& other);
	void operator=(const SIMD_128_8& other);
	
	SIMD_128_8(const SIMD_128_8&& other) noexcept;
	void operator=(const SIMD_128_8&& other) noexcept;
	
	~SIMD_128_8();
	
	//load / store
	static SIMD_128_8 load(char* pointer);
	void store(char* pointer);
	
	//arithmetic
	SIMD_128_8 operator+(const SIMD_128_8& other) const;
	SIMD_128_8 operator-(const SIMD_128_8& other) const;
	SIMD_128_8 operator-() const;
	SIMD_128_8 operator*(const SIMD_128_8& other) const;
	SIMD_128_8 operator/(const SIMD_128_8& other) const;
	
	void operator+=(const SIMD_128_8& other);
	void operator-=(const SIMD_128_8& other);
	void operator*=(const SIMD_128_8& other);
	void operator/=(const SIMD_128_8& other);
	
	//memory access and modification
	
	//bitwise
	SIMD_128_8 operator>>(const int shift) const;
	SIMD_128_8 operator<<(const int shift) const;
	SIMD_128_8 operator>>(const SIMD_128_8& other) const;
	SIMD_128_8 operator<<(const SIMD_128_8& other) const;
	
	void operator>>=(const int shift);
	void operator<<=(const int shift);
	void operator>>=(const SIMD_128_8& other);
	void operator<<=(const SIMD_128_8& other);
	
	SIMD_128_8 operator&(const int v) const;
	SIMD_128_8 operator&(const SIMD_128_8& other) const;
	
	void operator&=(const int v);
	void operator&=(const SIMD_128_8& other);
	
	SIMD_128_8 bitwiseAndNot(const int v) const;
	SIMD_128_8 bitwiseAndNot(const SIMD_128_8& other) const;
	
	void bitwiseAndNot(const int v);
	void bitwiseAndNot(const SIMD_128_8& other);
	
	//comparison
	SIMD_128_8 operator>(const char byte) const;
	SIMD_128_8 operator>(const SIMD_128_8& other) const;
	
	SIMD_128_8 operator<(const char byte) const;
	SIMD_128_8 operator<(const SIMD_128_8& other) const;
	
	SIMD_128_8 operator==(const char byte) const;
	SIMD_128_8 operator==(const SIMD_128_8& other) const;
	
	SIMD_128_8 operator!=(const char byte) const;
	SIMD_128_8 operator!=(const SIMD_128_8& other) const;

	//special case functions
	SIMD_128_8 horizontalAdd(const SIMD_128_8& other) const;
	short sum() const;

	//cast / convert operations
	// SIMD_128_U16 convertToU16(int location) const;
	// SIMD_128_U32 convertToU32(int location) const;
	// SIMD_128_U64 convertToU64(int location) const;
	// SIMD_128_FP32 convertToFP32(int location) const;
	// SIMD_128_FP32 convertToFP64(int location) const;

	__m128i values;
};
#endif