#pragma once
#include "SIMD.h"

#if (OPTI >= 1)
#define SIMD_128_FP32_BLEND(x, y, imm) _mm_blend_ps(x, y, imm)
#define SIMD_128_FP32_BLEND_MASK(x, y, mask) _mm_blendv_ps(x, y, mask)

class SIMD_128_FP32
{
public:
	static const int SIZE = 4;
	static unsigned long long getSIMDBound(unsigned long long s);

	SIMD_128_FP32();
	SIMD_128_FP32(float val);
	SIMD_128_FP32(__m128 sseValue);
	
	SIMD_128_FP32(const SIMD_128_FP32& other);
	void operator=(const SIMD_128_FP32& other);
	
	SIMD_128_FP32(const SIMD_128_FP32&& other) noexcept;
	void operator=(const SIMD_128_FP32&& other) noexcept;
	
	~SIMD_128_FP32();
	
	//load / store
	static SIMD_128_FP32 load(float* pointer);
	void store(float* pointer);
	
	//arithmetic
	SIMD_128_FP32 operator+(const SIMD_128_FP32& other) const;
	SIMD_128_FP32 operator-(const SIMD_128_FP32& other) const;
	SIMD_128_FP32 operator-() const;
	SIMD_128_FP32 operator*(const SIMD_128_FP32& other) const;
	SIMD_128_FP32 operator/(const SIMD_128_FP32& other) const;
	
	void operator+=(const SIMD_128_FP32& other);
	void operator-=(const SIMD_128_FP32& other);
	void operator*=(const SIMD_128_FP32& other);
	void operator/=(const SIMD_128_FP32& other);
	
	//bitwise
	SIMD_128_FP32 operator&(const float v) const;
	SIMD_128_FP32 operator&(const SIMD_128_FP32& other) const;
	
	void operator&=(const float v);
	void operator&=(const SIMD_128_FP32& other);
	
	SIMD_128_FP32 bitwiseAndNot(const float v) const;
	SIMD_128_FP32 bitwiseAndNot(const SIMD_128_FP32& other) const;
	
	void bitwiseAndNot(const float v);
	void bitwiseAndNot(const SIMD_128_FP32& other);
	
	//comparison
	SIMD_128_FP32 operator>(const float byte) const;
	SIMD_128_FP32 operator>(const SIMD_128_FP32& other) const;
	
	SIMD_128_FP32 operator<(const float byte) const;
	SIMD_128_FP32 operator<(const SIMD_128_FP32& other) const;
	
	SIMD_128_FP32 operator==(const float byte) const;
	SIMD_128_FP32 operator==(const SIMD_128_FP32& other) const;
	
	SIMD_128_FP32 operator!=(const float byte) const;
	SIMD_128_FP32 operator!=(const SIMD_128_FP32& other) const;

	//special case functions
	SIMD_128_FP32 horizontalAdd(const SIMD_128_FP32& other) const;
	float sum() const;

	// //cast / convert operations
	// SIMD_128_U16 convertToU16(int location) const;
	// SIMD_128_U32 convertToU32(int location) const;
	// SIMD_128_FP32 convertToFP32(int location) const;
	
	__m128 values;
};
#endif