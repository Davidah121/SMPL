#pragma once
#include "BuildOptions.h"
#include "StandardTypes.h"
#include "SimpleSerialization.h"
#include "NewStringFormatting.h"
#include "Vec2.h"
#include "Vec3.h"

namespace smpl
{
	
	template<typename T>
	class DLL_OPTION Vec4;

	typedef Vec4<float> Vec4f;
	typedef Vec4<double> Vec4d;
	typedef Vec4<int> Vec4i;

	template<typename T>
	class DLL_OPTION Vec4
	{
	public:
	
		Vec4() = default;
		Vec4(const Vec4<T>& other) = default;
		Vec4<T>& operator=(const Vec4<T>& other) = default;
		Vec4(Vec4<T>&& other) = default;
		Vec4<T>& operator=(Vec4<T>&& other) = default;
		~Vec4() = default;

		/**
		 * @brief Construct a new Vec4 object
		 * 
		 * @param x 
		 * @param y 
		 * @param z
		 */
		Vec4(T x, T y, T z, T w){this->x = x; this->y = y; this->z = z; this->w = w;}
		Vec4(const Vec2<T>& v, T z = 0, T w = 0){this->x = v.x; this->y = v.y; this->z = z; this->w = w;}
		Vec4(const Vec3<T>& v, T w = 0){this->x = v.x; this->y = v.y; this->z = v.z; this->w = w;}
		
		
		/**
		 * @brief Converts the Vec4f object to a Vec3 object by dropping the w value.
		 * 
		 * @return Vec3 
		 */
		Vec3<T> toVec3() const {return Vec3<T>(x, y, z);}

		/**
		 * @brief Get the Length of the vector
		 * 
		 * @return T 
		 */
		T getLength() const {return sqrt(x*x + y*y + z*z + w*w);}

		/**
		 * @brief Gets the normalized version of the vector.
		 * 		Returns a vector with the same direction but a length of 1.
		 * 
		 * @return Vec4<T> 
		 */
		Vec4<T> normalize() const
		{
			T len = getLength();
			return Vec4<T>(x/len, y/len, z/len, w/len);
		}

		/**
		 * @brief Computes the dot product between this vector and another
		 * 
		 * @param other 
		 * @return T 
		 */
		T dot(const Vec4<T>& b) const {return x*b.x + b.y*y + z*b.z + w*b.w;}

		/**
		 * @brief Computes the scalar vector projection between 2 vectors
		 * 
		 * @param other 
		 * @return T 
		 */
		T project(const Vec4<T>& b) const {return dot(b.normalize());}

		/**
		 * @brief Reflects this vector across the vector "b"
		 * 
		 * @param other 
		 * @return Vec4<T> 
		 */
		Vec4<T> reflect(const Vec4<T>& b) const
		{
			Vec4<T> n = b.normalize();
			return this->operator-(n*(dot(n)*2));
		}
		
		Vec4<T> operator-(const Vec4<T>& b) const { return Vec4<T>(x-b.x, y-b.y, z-b.z, w-b.w); }
		Vec4<T> operator+(const Vec4<T>& b) const { return Vec4<T>(x+b.x, y+b.y, z+b.z, w+b.w); }

		void operator+=(const Vec4<T>& b) {x += b.x; y += b.y; z += b.z; w += b.w;}
		void operator-=(const Vec4<T>& b) {x -= b.x; y -= b.y; z -= b.z; w -= b.w;}

		void operator*=(T b) { x *= b; y *= b; z *= b; w *= b;}
		void operator/=(T b) { x /= b; y /= b; z /= b; w /= b;}

		Vec4<T> operator-() const {return Vec4<T>(-x, -y, -z, -w);}

		Vec4<T> operator*(T value) const { return Vec4<T>(x*value, y*value, z*value, w*value); }
		Vec4<T> operator/(T value) const { return Vec4<T>(x/value, y/value, z/value, w/value); }
		
		Vec4<T> operator*(const Vec4<T>& b) const {return Vec4<T>(x*b.x, y*b.y, z*b.z, w*b.w);}
		Vec4<T> operator/(const Vec4<T>& b) const {return Vec4<T>(x/b.x, y/b.y, z/b.z, w/b.w);}

		friend Vec4<T> operator*(T value, const Vec4<T>& other)
		{
			return other*value;
		}

		Vec4<T> broadcastAdd(T v) const {return Vec4<T>(x+v, y+v, z+v, w+v);}
		Vec4<T> broadcastSubtract(T v) const {return Vec4<T>(x-v, y-v, z-v, w-v);}
		Vec4<T> broadcastFunction(std::function<T(T)> func) const
		{
			if(func == nullptr)
				throw std::bad_function_call();
			return Vec4<T>(func(x), func(y), func(z), func(w));
		}
		Vec4<T> broadcastFunction(T (*func)(T)) const
		{
			if(func == nullptr)
				throw std::bad_function_call();
			return Vec4<T>(func(x), func(y), func(z), func(w));
		}
		
		bool operator==(const Vec4<T>& b) const {return x==x && y==y && z==z && w==w;}
		bool operator!=(const Vec4<T>& b) const {return x!=x || y!=y || z!=z || w!=w;}
		
		T x = 0;
		T y = 0;
		T z = 0;
		T w = 0;
	
		SERIALIZE_CLASS_LIGHT(Vec4<T>, x, y)
	};

	template<typename T>
	Vec4<T> min(const Vec4<T>& a, const Vec4<T>& b)
	{
		return Vec4<T>(min<T>(a.x, b.x), min<T>(a.y, b.y), min<T>(a.z, b.z), min<T>(a.w, b.w));
	}
	template<typename T>
	Vec4<T> max(const Vec4<T>& a, const Vec4<T>& b)
	{
		return Vec4<T>(max<T>(a.x, b.x), max<T>(a.y, b.y), max<T>(a.z, b.z), max<T>(a.w, b.w));
	}

	template<typename T>
	Vec4<T> loadVec4(const T* ptr)
	{
		return Vec4<T>(*ptr, *(ptr+1), *(ptr+2), *(ptr+3));
	}
	template<typename T>
	void storeVec4(const Vec4<T>& v, T* ptr)
	{
		*ptr = v.x;
		*(ptr+1) = v.y;
		*(ptr+2) = v.z;
		*(ptr+3) = v.w;
	}

	template<>
	inline Vec4<SIMD_SSE<float>> loadVec4(const SIMD_SSE<float>* ptr)
	{
		//just a float ptr
		const float* actualPtr = (const float*)ptr;
		SIMD_SSE<float> v1 = SIMD_SSE<float>::load(actualPtr);							//x1,y1,z1,w1
		SIMD_SSE<float> v2 = SIMD_SSE<float>::load(actualPtr+SIMD_SSE<float>::SIZE);	//x2,y2,z2,w2
		SIMD_SSE<float> v3 = SIMD_SSE<float>::load(actualPtr+SIMD_SSE<float>::SIZE*2);	//x3,y3,z3,w3
		SIMD_SSE<float> v4 = SIMD_SSE<float>::load(actualPtr+SIMD_SSE<float>::SIZE*3);	//x4,y4,z4,w4
		
		__m128 unpackedV1 = _mm_unpacklo_ps(v1.values, v2.values); //x1,x2,y1,y2
		__m128 unpackedV2 = _mm_unpackhi_ps(v1.values, v2.values); //z1,z2,w1,w2
		__m128 unpackedV3 = _mm_unpacklo_ps(v3.values, v4.values); //x3,x4,y3,y4
		__m128 unpackedV4 = _mm_unpackhi_ps(v3.values, v4.values); //z3,z4,w3,w4
		
		__m128 x = _mm_unpacklo_ps(unpackedV1, unpackedV2); //x1,x2,x3,x4
		__m128 y = _mm_unpackhi_ps(unpackedV1, unpackedV2); //y1,y2,y3,y4
		__m128 z = _mm_unpacklo_ps(unpackedV3, unpackedV4); //x1,x2,x3,x4
		__m128 w = _mm_unpackhi_ps(unpackedV3, unpackedV4); //y1,y2,y3,y4
		
		return Vec4<SIMD_SSE<float>>(x, y, z, w);
	}
	
	template<>
	inline void storeVec4(const Vec4<SIMD_SSE<float>>& v, SIMD_SSE<float>* ptr)
	{
		//just a float ptr
		float* actualPtr = (float*)ptr;

		__m128 v1 = _mm_unpacklo_ps(v.x.values, v.y.values); //x1,y1,x2,y2
		__m128 v2 = _mm_unpackhi_ps(v.x.values, v.y.values); //x3,y3,x4,y4
		__m128 v3 = _mm_unpacklo_ps(v.z.values, v.w.values); //z1,w1,z2,w2
		__m128 v4 = _mm_unpackhi_ps(v.z.values, v.w.values); //z3,w3,z4,w4

		__m128 storeV1 = _mm_unpacklo_ps(v1, v3);	//x1,y1,z1,w1
		__m128 storeV2 = _mm_unpackhi_ps(v1, v3);	//x2,y2,z2,w2
		__m128 storeV3 = _mm_unpacklo_ps(v2, v4);	//x3,y3,z3,w3
		__m128 storeV4 = _mm_unpackhi_ps(v2, v4);	//x4,y4,z4,w4

		_mm_storeu_ps(actualPtr, storeV1);
		_mm_storeu_ps(actualPtr+4, storeV2);
		_mm_storeu_ps(actualPtr+8, storeV3);
		_mm_storeu_ps(actualPtr+12, storeV4);
	}

	template<>
	inline Vec4<SIMD_AVX<float>> loadVec4(const SIMD_AVX<float>* ptr)
	{
		//just a float ptr
		const float* actualPtr = (const float*)ptr;
		SIMD_AVX<float> v1 = SIMD_AVX<float>::load(actualPtr);
		SIMD_AVX<float> v2 = SIMD_AVX<float>::load(actualPtr+SIMD_AVX<float>::SIZE);
		SIMD_AVX<float> v3 = SIMD_AVX<float>::load(actualPtr+SIMD_AVX<float>::SIZE*2);
		SIMD_AVX<float> v4 = SIMD_AVX<float>::load(actualPtr+SIMD_AVX<float>::SIZE*3);
		
		__m256 unpackedV1 = _mm256_unpacklo_ps(v1.values, v2.values);
		__m256 unpackedV2 = _mm256_unpackhi_ps(v1.values, v2.values);
		__m256 unpackedV3 = _mm256_unpacklo_ps(v3.values, v4.values);
		__m256 unpackedV4 = _mm256_unpackhi_ps(v3.values, v4.values);
		
		__m256 unpacked2V1 = _mm256_unpacklo_ps(unpackedV1, unpackedV2);
		__m256 unpacked2V2 = _mm256_unpackhi_ps(unpackedV1, unpackedV2);
		__m256 unpacked2V3 = _mm256_unpacklo_ps(unpackedV3, unpackedV4);
		__m256 unpacked2V4 = _mm256_unpackhi_ps(unpackedV3, unpackedV4);

		__m256 x = _mm256_unpacklo_ps(unpacked2V1, unpacked2V2);
		__m256 y = _mm256_unpackhi_ps(unpacked2V1, unpacked2V2);
		__m256 z = _mm256_unpacklo_ps(unpacked2V3, unpacked2V4);
		__m256 w = _mm256_unpackhi_ps(unpacked2V3, unpacked2V4);
		
		return Vec4<SIMD_AVX<float>>(x, y, z, w);
	}
	
	template<>
	inline void storeVec4(const Vec4<SIMD_AVX<float>>& v, SIMD_AVX<float>* ptr)
	{
		//just a float ptr
		float* actualPtr = (float*)ptr;

		__m256 v1 = _mm256_unpacklo_ps(v.x.values, v.y.values); //x1,y1,x2,y2
		__m256 v2 = _mm256_unpackhi_ps(v.x.values, v.y.values); //x3,y3,x4,y4
		__m256 v3 = _mm256_unpacklo_ps(v.z.values, v.w.values); //z1,w1,z2,w2
		__m256 v4 = _mm256_unpackhi_ps(v.z.values, v.w.values); //z3,w3,z4,w4

		__m256 storeV1 = _mm256_unpacklo_ps(v1, v3);	//x1,y1,z1,w1
		__m256 storeV2 = _mm256_unpackhi_ps(v1, v3);	//x2,y2,z2,w2
		__m256 storeV3 = _mm256_unpacklo_ps(v2, v4);	//x3,y3,z3,w3
		__m256 storeV4 = _mm256_unpackhi_ps(v2, v4);	//x4,y4,z4,w4

		_mm256_storeu_ps(actualPtr, storeV1);
		_mm256_storeu_ps(actualPtr+8, storeV2);
		_mm256_storeu_ps(actualPtr+16, storeV3);
		_mm256_storeu_ps(actualPtr+24, storeV4);
	}

	template<typename T>
	void formatToString(StringStream& stream, const Vec4<T>& v, const std::string& options)
	{
		stream.write('<');
		formatToString(stream, v.x, options);
		stream.write(", ");
		formatToString(stream, v.y, options);
		stream.write(", ");
		formatToString(stream, v.z, options);
		stream.write(", ");
		formatToString(stream, v.w, options);
		stream.write('>');
	}
} //NAMESPACE glib END