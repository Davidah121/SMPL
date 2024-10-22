#pragma once
#include "SIMD.h"

#if (OPTI >= 1)
#define SIMD_128_U32_BLEND(x, y, imm) (*(__m128i*)&_mm_blend_ps(*(__m128*)&(x), *(__m128*)&(y), imm))
#define SIMD_128_U32_BLEND_MASK(x, y, mask) (*(__m128i*)&_mm_blend_ps(*(__m128*)&(x), *(__m128*)&(y), mask))
#define SIMD_128_32_BLEND(x, y, imm) (*(__m128i*)&_mm_blend_ps(*(__m128*)&(x), *(__m128*)&(y), imm))
#define SIMD_128_32_BLEND_MASK(x, y, mask) (*(__m128i*)&_mm_blend_ps(*(__m128*)&(x), *(__m128*)&(y), mask))

class SIMD_128_U32
{
public:
	static const int SIZE = 4;
	static unsigned long long getSIMDBound(unsigned long long s);

	SIMD_128_U32();
	SIMD_128_U32(const unsigned int byte);
	SIMD_128_U32(__m128i sseValue);
	
	SIMD_128_U32(const SIMD_128_U32& other);
	void operator=(const SIMD_128_U32& other);
	
	SIMD_128_U32(const SIMD_128_U32&& other) noexcept;
	void operator=(const SIMD_128_U32&& other) noexcept;
	
	~SIMD_128_U32();
	
	//load / store
	static SIMD_128_U32 load(unsigned int* pointer);
	void store(unsigned int* pointer);
	
	//arithmetic
	SIMD_128_U32 operator+(const SIMD_128_U32& other) const;
	SIMD_128_U32 operator-(const SIMD_128_U32& other) const;
	SIMD_128_U32 operator*(const SIMD_128_U32& other) const;
	SIMD_128_U32 operator/(const SIMD_128_U32& other) const;
	
	void operator+=(const SIMD_128_U32& other);
	void operator-=(const SIMD_128_U32& other);
	void operator*=(const SIMD_128_U32& other);
	void operator/=(const SIMD_128_U32& other);
	
	//memory access and modification
	
	//bitwise
	SIMD_128_U32 operator>>(const int shift) const;
	SIMD_128_U32 operator<<(const int shift) const;
	SIMD_128_U32 operator>>(const SIMD_128_U32& other) const;
	SIMD_128_U32 operator<<(const SIMD_128_U32& other) const;
	
	void operator>>=(const int shift);
	void operator<<=(const int shift);
	void operator>>=(const SIMD_128_U32& other);
	void operator<<=(const SIMD_128_U32& other);
	
	SIMD_128_U32 operator&(const int v) const;
	SIMD_128_U32 operator&(const SIMD_128_U32& other) const;
	
	void operator&=(const int v);
	void operator&=(const SIMD_128_U32& other);
	
	SIMD_128_U32 bitwiseAndNot(const int v) const;
	SIMD_128_U32 bitwiseAndNot(const SIMD_128_U32& other) const;
	
	void bitwiseAndNot(const int v);
	void bitwiseAndNot(const SIMD_128_U32& other);
	
	//comparison
	SIMD_128_U32 operator>(const unsigned int byte) const;
	SIMD_128_U32 operator>(const SIMD_128_U32& other) const;
	
	SIMD_128_U32 operator<(const unsigned int byte) const;
	SIMD_128_U32 operator<(const SIMD_128_U32& other) const;
	
	SIMD_128_U32 operator==(const unsigned int byte) const;
	SIMD_128_U32 operator==(const SIMD_128_U32& other) const;
	
	SIMD_128_U32 operator!=(const unsigned int byte) const;
	SIMD_128_U32 operator!=(const SIMD_128_U32& other) const;

	//cast / convert operations
	// SIMD_128_U16 convertToU16(int location) const;
	// SIMD_128_U32 convertToU32(int location) const;
	// SIMD_128_U64 convertToU64(int location) const;
	// SIMD_128_FP32 convertToFP32(int location) const;
	// SIMD_128_FP32 convertToFP64(int location) const;
	
	__m128i values;
};


class SIMD_128_32
{
public:
	static const int SIZE = 4;
	static unsigned long long getSIMDBound(unsigned long long s);

	SIMD_128_32();
	SIMD_128_32(int byte);
	SIMD_128_32(__m128i sseValue);
	
	SIMD_128_32(const SIMD_128_32& other);
	void operator=(const SIMD_128_32& other);
	
	SIMD_128_32(const SIMD_128_32&& other) noexcept;
	void operator=(const SIMD_128_32&& other) noexcept;
	
	~SIMD_128_32();
	
	//load / store
	static SIMD_128_32 load(int* pointer);
	void store(int* pointer);
	
	//arithmetic
	SIMD_128_32 operator+(const SIMD_128_32& other) const;
	SIMD_128_32 operator-(const SIMD_128_32& other) const;
	SIMD_128_32 operator*(const SIMD_128_32& other) const;
	SIMD_128_32 operator/(const SIMD_128_32& other) const;
	
	void operator+=(const SIMD_128_32& other);
	void operator-=(const SIMD_128_32& other);
	void operator*=(const SIMD_128_32& other);
	void operator/=(const SIMD_128_32& other);
	
	//memory access and modification
	
	//bitwise
	SIMD_128_32 operator>>(const int shift) const;
	SIMD_128_32 operator<<(const int shift) const;
	SIMD_128_32 operator>>(const SIMD_128_32& other) const;
	SIMD_128_32 operator<<(const SIMD_128_32& other) const;
	
	void operator>>=(const int shift);
	void operator<<=(const int shift);
	void operator>>=(const SIMD_128_32& other);
	void operator<<=(const SIMD_128_32& other);
	
	SIMD_128_32 operator&(const int v) const;
	SIMD_128_32 operator&(const SIMD_128_32& other) const;
	
	void operator&=(const int v);
	void operator&=(const SIMD_128_32& other);
	
	SIMD_128_32 bitwiseAndNot(const int v) const;
	SIMD_128_32 bitwiseAndNot(const SIMD_128_32& other) const;
	
	void bitwiseAndNot(const int v);
	void bitwiseAndNot(const SIMD_128_32& other);
	
	//comparison
	SIMD_128_32 operator>(const int byte) const;
	SIMD_128_32 operator>(const SIMD_128_32& other) const;
	
	SIMD_128_32 operator<(const int byte) const;
	SIMD_128_32 operator<(const SIMD_128_32& other) const;
	
	SIMD_128_32 operator==(const int byte) const;
	SIMD_128_32 operator==(const SIMD_128_32& other) const;
	
	SIMD_128_32 operator!=(const int byte) const;
	SIMD_128_32 operator!=(const SIMD_128_32& other) const;

	//cast / convert operations
	// SIMD_128_U16 convertToU16(int location) const;
	// SIMD_128_32 convertToU32(int location) const;
	// SIMD_128_U64 convertToU64(int location) const;
	// SIMD_128_FP32 convertToFP32(int location) const;
	// SIMD_128_FP32 convertToFP64(int location) const;
	
	__m128i values;
};
#endif