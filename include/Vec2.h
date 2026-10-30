#pragma once
#include <cmath>
#include "BuildOptions.h"
#include "StandardTypes.h"
#include "SimpleSerialization.h"
#include "NewStringFormatting.h"
#include "SIMD.h"

namespace smpl
{
	template<typename T>
	class DLL_OPTION Vec2;

	typedef Vec2<float> Vec2f;
	typedef Vec2<double> Vec2d;
	typedef Vec2<int> Vec2i;

	template<typename T>
	class DLL_OPTION Vec2
	{
	public:
	
		Vec2() = default;
		Vec2(const Vec2<T>& other) = default;
		Vec2<T>& operator=(const Vec2<T>& other) = default;
		Vec2(Vec2<T>&& other) = default;
		Vec2<T>& operator=(Vec2<T>&& other) = default;
		~Vec2() = default;

		/**
		 * @brief Construct a new Vec2 object
		 * 
		 * @param x 
		 * @param y 
		 */
		Vec2(T x, T y){this->x = x; this->y = y;}
		
		/**
		 * @brief Get the Length of the vector
		 * 
		 * @return T 
		 */
		T getLength() const {return sqrt(x*x + y*y);}

		/**
		 * @brief Gets the normalized version of the vector.
		 * 		Returns a vector with the same direction but a length of 1.
		 * 
		 * @return Vec2<T> 
		 */
		Vec2<T> normalize() const
		{
			T len = getLength();
			return Vec2<T>(x/len, y/len);
		}

		/**
		 * @brief Computes the dot product between this vector and another
		 * 
		 * @param other 
		 * @return T 
		 */
		T dot(const Vec2<T>& b) const {return x*b.x + b.y*y;}

		/**
		 * @brief Computes the scalar vector projection between 2 vectors
		 * 
		 * @param other 
		 * @return T 
		 */
		T project(const Vec2<T>& b) const {return dot(b.normalize());}

		/**
		 * @brief Reflects this vector across the vector "b"
		 * 
		 * @param other 
		 * @return Vec2<T> 
		 */
		Vec2<T> reflect(const Vec2<T>& b) const
		{
			Vec2<T> n = b.normalize();
			return this->operator-(n*(dot(n)*2));
		}

		/**
		 * @brief Returns a new vector that is the inverse.
		 * 		Returns this : Vec2<T>( -f.y, f.x )
		 * 
		 * @return Vec2 
		 */
		Vec2<T> inverse() const { return Vec2<T>(-y, x); }
		
		Vec2<T> operator-(const Vec2<T>& b) const { return Vec2<T>(x-b.x, y-b.y); }
		Vec2<T> operator+(const Vec2<T>& b) const { return Vec2<T>(x+b.x, y+b.y); }

		void operator+=(const Vec2<T>& b) {x += b.x; y += b.y;}
		void operator-=(const Vec2<T>& b) {x -= b.x; y -= b.y;}

		void operator*=(T b) { x *= b; y *= b;}
		void operator/=(T b) { x /= b; y /= b;}

		Vec2<T> operator-() const {return Vec2<T>(-x, -y);}

		Vec2<T> operator*(T value) const { return Vec2<T>(x*value, y*value); }
		Vec2<T> operator/(T value) const { return Vec2<T>(x/value, y/value); }
		
		Vec2<T> operator*(const Vec2<T>& b) const {return Vec2<T>(x*b.x, y*b.y);}
		Vec2<T> operator/(const Vec2<T>& b) const {return Vec2<T>(x/b.x, y/b.y);}

		friend Vec2<T> operator*(T value, const Vec2<T>& other)
		{
			return other*value;
		}

		Vec2<T> broadcastAdd(T v) const {return Vec2<T>(x+v, y+v);}
		Vec2<T> broadcastSubtract(T v) const {return Vec2<T>(x-v, y-v);}
		Vec2<T> broadcastFunction(std::function<T(T)> func) const
		{
			if(func == nullptr)
				throw std::bad_function_call();
			return Vec2<T>(func(x), func(y));
		}
		Vec2<T> broadcastFunction(T (*func)(T)) const
		{
			if(func == nullptr)
				throw std::bad_function_call();
			return Vec2<T>(func(x), func(y));
		}
		
		bool operator==(const Vec2<T>& b) const {return x==b.x && y==b.y;}
		bool operator!=(const Vec2<T>& b) const {return x!=b.x || y!=b.y;}

		
		T x = 0;
		T y = 0;
	
		SERIALIZE_CLASS_LIGHT(Vec2<T>, x, y)
	};
	
	template<typename T>
	Vec2<T> min(const Vec2<T>& a, const Vec2<T>& b)
	{
		return Vec2<T>(min<T>(a.x, b.x), min<T>(a.y, b.y));
	}
	template<typename T>
	Vec2<T> max(const Vec2<T>& a, const Vec2<T>& b)
	{
		return Vec2<T>(max<T>(a.x, b.x), max<T>(a.y, b.y));
	}

	template<typename T>
	Vec2<T> loadVec2(const T* ptr)
	{
		return Vec2<T>(*ptr, *(ptr+1));
	}
	template<typename T>
	void storeVec2(const Vec2<T>& v, T* ptr)
	{
		*ptr = v.x;
		*(ptr+1) = v.y;
	}

	template<>
	inline Vec2<SIMD_SSE<float>> loadVec2(const SIMD_SSE<float>* ptr)
	{
		//just a float ptr
		const float* actualPtr = (const float*)ptr;
		SIMD_SSE<float> v1 = SIMD_SSE<float>::load(actualPtr);							//x1,y1,x2,y2
		SIMD_SSE<float> v2 = SIMD_SSE<float>::load(actualPtr+SIMD_SSE<float>::SIZE);	//x3,y3,x4,y4
		
		__m128 unpackedV1 = _mm_unpacklo_ps(v1.values, v2.values); //x1,x3,y1,y3
		__m128 unpackedV2 = _mm_unpackhi_ps(v1.values, v2.values); //x2,x4,y2,y4
		
		__m128 x = _mm_unpacklo_ps(unpackedV1, unpackedV2); //x1,x2,x3,x4
		__m128 y = _mm_unpackhi_ps(unpackedV1, unpackedV2); //y1,y2,y3,y4
		
		return Vec2<SIMD_SSE<float>>(x, y);
	}
	
	template<>
	inline void storeVec2(const Vec2<SIMD_SSE<float>>& v, SIMD_SSE<float>* ptr)
	{
		//just a float ptr
		float* actualPtr = (float*)ptr;

		__m128 v1 = _mm_unpacklo_ps(v.x.values, v.y.values); //x1,y1,x2,y2
		__m128 v2 = _mm_unpackhi_ps(v.x.values, v.y.values); //x3,y3,x4,y4

		_mm_storeu_ps(actualPtr, v1);
		_mm_storeu_ps(actualPtr+4, v2);
	}

	template<>
	inline Vec2<SIMD_AVX<float>> loadVec2(const SIMD_AVX<float>* ptr)
	{
		//just a float ptr
		const float* actualPtr = (const float*)ptr;
		SIMD_AVX<float> v1 = SIMD_AVX<float>::load(actualPtr);							//x1,y1,x2,y2,x3,y3,x4,y4
		SIMD_AVX<float> v2 = SIMD_AVX<float>::load(actualPtr+SIMD_AVX<float>::SIZE);	//x5,y5,x6,y6,x7,y7,x8,y8
		
		__m256 unpackedV1 = _mm256_unpacklo_ps(v1.values, v2.values); //x1,x5,y1,y5,x2,x6,y2,y6
		__m256 unpackedV2 = _mm256_unpackhi_ps(v1.values, v2.values); //x3,x7,y3,y7,x4,x8,y4,y8
		
		__m256 unpacked2V1 = _mm256_unpacklo_ps(unpackedV1, unpackedV2); //x1,x3,x5,x7,y1,y3,y5,y7
		__m256 unpacked2V2 = _mm256_unpackhi_ps(unpackedV1, unpackedV2); //x2,x4,x6,x8,y2,y4,y6,y8

		__m256 x = _mm256_unpacklo_ps(unpacked2V1, unpacked2V2); //x1,x3,x5,x7,y1,y3,y5,y7
		__m256 y = _mm256_unpackhi_ps(unpacked2V1, unpacked2V2); //x2,x4,x6,x8,y2,y4,y6,y8
		
		return Vec2<SIMD_AVX<float>>(x, y);
	}
	
	template<>
	inline void storeVec2(const Vec2<SIMD_AVX<float>>& v, SIMD_AVX<float>* ptr)
	{
		//just a float ptr
		float* actualPtr = (float*)ptr;

		__m256 v1 = _mm256_unpacklo_ps(v.x.values, v.y.values); //x1,y1,x2,y2,x3,y3,x4,y4
		__m256 v2 = _mm256_unpackhi_ps(v.x.values, v.y.values); //x5,y5,x6,y6,x7,y7,x8,y8

		_mm256_storeu_ps(actualPtr, v1);
		_mm256_storeu_ps(actualPtr+8, v2);
	}

	template<typename T>
	void formatToString(StringStream& stream, const Vec2<T>& v, const std::string& options)
	{
		stream.write('<');
		formatToString(stream, v.x, options);
		stream.write(", ");
		formatToString(stream, v.y, options);
		stream.write('>');
	}
} //NAMESPACE glib END