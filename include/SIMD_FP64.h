#pragma once
#include "SIMD.h"

#if (OPTI >= 1)
#define SIMD_128_FP64_BLEND(x, y, imm) _mm_blend_pd(x, y, imm)
#define SIMD_128_FP64_BLEND_MASK(x, y, mask) _mm_blendv_pd(x, y, mask)

class SIMD_128_FP64
{
public:
	static const int SIZE = 2;
	static unsigned long long getSIMDBound(unsigned long long s);

	SIMD_128_FP64();
	SIMD_128_FP64(double val);
	SIMD_128_FP64(__m128d sseValue);
	
	SIMD_128_FP64(const SIMD_128_FP64& other);
	void operator=(const SIMD_128_FP64& other);
	
	SIMD_128_FP64(const SIMD_128_FP64&& other) noexcept;
	void operator=(const SIMD_128_FP64&& other) noexcept;
	
	~SIMD_128_FP64();
	
	//load / store
	static SIMD_128_FP64 load(double* pointer);
	void store(double* pointer);
	
	//arithmetic
	SIMD_128_FP64 operator+(const SIMD_128_FP64& other) const;
	SIMD_128_FP64 operator-(const SIMD_128_FP64& other) const;
	SIMD_128_FP64 operator*(const SIMD_128_FP64& other) const;
	SIMD_128_FP64 operator/(const SIMD_128_FP64& other) const;
	
	void operator+=(const SIMD_128_FP64& other);
	void operator-=(const SIMD_128_FP64& other);
	void operator*=(const SIMD_128_FP64& other);
	void operator/=(const SIMD_128_FP64& other);
	
	//bitwise
	SIMD_128_FP64 operator&(const double v) const;
	SIMD_128_FP64 operator&(const SIMD_128_FP64& other) const;
	
	void operator&=(const double v);
	void operator&=(const SIMD_128_FP64& other);
	
	SIMD_128_FP64 bitwiseAndNot(const double v) const;
	SIMD_128_FP64 bitwiseAndNot(const SIMD_128_FP64& other) const;
	
	void bitwiseAndNot(const double v);
	void bitwiseAndNot(const SIMD_128_FP64& other);
	
	//comparison
	SIMD_128_FP64 operator>(const double byte) const;
	SIMD_128_FP64 operator>(const SIMD_128_FP64& other) const;
	
	SIMD_128_FP64 operator<(const double byte) const;
	SIMD_128_FP64 operator<(const SIMD_128_FP64& other) const;
	
	SIMD_128_FP64 operator==(const double byte) const;
	SIMD_128_FP64 operator==(const SIMD_128_FP64& other) const;
	
	SIMD_128_FP64 operator!=(const double byte) const;
	SIMD_128_FP64 operator!=(const SIMD_128_FP64& other) const;

	//special case functions
	SIMD_128_FP64 horizontalAdd(const SIMD_128_FP64& other) const;
	double sum() const;

	//cast / convert operations
	// SIMD_128_U16 convertToU16(int location) const;
	// SIMD_128_U32 convertToU32(int location) const;
	// SIMD_128_FP64 convertToFP32(int location) const;
	
	__m128d values;
};
#endif