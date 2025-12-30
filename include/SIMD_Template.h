#pragma once
#include "StandardTypes.h"
#include "SEML.h"
#include "SEML_256.h"

namespace smpl
{
	class SIMD_Singleton
	{
	private:
		SIMD_Singleton()
		{
			//Default the rounding to round down which is the default for non SIMD functions
			SEML::setRoundingMode(SEML_ROUND_DOWN);
		}
		~SIMD_Singleton()
		{

		}
		static SIMD_Singleton singleton;
	};
	inline SIMD_Singleton SIMD_Singleton::singleton = SIMD_Singleton();

	template<typename T>
	class SIMD_RAW;
	template<typename T>
	class SIMD_SSE;
	template<typename T>
	class SIMD_AVX;

	#ifdef __SSE4_2__
	template<typename T>
	__m128i gatherSSE(T* pointer, __m128i indices)
	{
		//unsure if you can do this
		const int maxSize = 16/sizeof(T);
		T indexArr[maxSize];
		_mm_storeu_si128((__m128i*)indexArr, indices);
		for(int i=0; i<maxSize; i++)
			indexArr[i] = ((T*)pointer)[indexArr[i]];
		return _mm_loadu_si128((__m128i*)indexArr);
	}
	
	template<typename T>
	void scatterSSE(T* pointer, __m128i indices, __m128i values)
	{
		const int maxSize = 16/sizeof(T);
		T indexArr[maxSize];
		_mm_storeu_si128((__m128i*)indexArr, indices);
		
		T valuesAsArr[16];
		_mm_storeu_si128((__m128i*)indexArr, values);
		
		for(int i=0; i<maxSize; i++)
			pointer[indexArr[i]] = valuesAsArr[i];
	}
	#endif


	#ifdef __AVX2__
	//GATHER FUNCTIONS
	template<>
	inline __m128i gatherSSE<int>(int* pointer, __m128i indices)
	{
		return _mm_i32gather_epi32(pointer, indices, 1);
	}
	template<>
	inline __m128i gatherSSE<unsigned int>(unsigned int* pointer, __m128i indices)
	{
		return _mm_i32gather_epi32((int*)pointer, indices, 1);
	}

	template<>
	inline __m128i gatherSSE<int64_t>(int64_t* pointer, __m128i indices)
	{
		return _mm_i64gather_epi64(pointer, indices, 1);
	}
	template<>
	inline __m128i gatherSSE<uint64_t>(uint64_t* pointer, __m128i indices)
	{
		return _mm_i64gather_epi64((int64_t*)pointer, indices, 1);
	}

	inline __m128 gatherSSE(float* pointer, __m128i indices)
	{
		return _mm_castsi128_ps(_mm_i32gather_epi32((int*)pointer, indices, 1));
	}
	inline __m128d gatherSSE(double* pointer, __m128i indices)
	{
		return _mm_castsi128_pd(_mm_i64gather_epi64((double*)pointer, indices, 1));
	}
	#endif


	#ifdef __AVX2__
	template<typename T>
	__m256i gatherAVX(T* pointer, __m256i indices)
	{
		//unsure if you can do this
		const int maxSize = 32/sizeof(T);
		T indexArr[maxSize];
		_mm256_storeu_si256((__m256i*)indexArr, indices);
		for(int i=0; i<maxSize; i++)
			indexArr[i] = ((T*)pointer)[indexArr[i]];
		return _mm256_loadu_si256((__m256i*)indexArr);
	}

	template<typename T>
	void scatterSSE(T* pointer, __m256i indices, __m256i values)
	{
		const int maxSize = 32/sizeof(T);
		T indexArr[maxSize];
		_mm256_storeu_si256((__m256i*)indexArr, indices);
		
		T valuesAsArr[32];
		_mm256_storeu_si256((__m256i*)indexArr, values);
		
		for(int i=0; i<maxSize; i++)
			pointer[indexArr[i]] = valuesAsArr[i];
	}

	template<>
	inline __m256i gatherAVX<int>(int* pointer, __m256i indices)
	{
		return _mm256_i32gather_epi32(pointer, indices, 1);
	}
	template<>
	inline __m256i gatherAVX<unsigned int>(unsigned int* pointer, __m256i indices)
	{
		return _mm256_i32gather_epi32((int*)pointer, indices, 1);
	}

	template<>
	inline __m256i gatherAVX<int64_t>(int64_t* pointer, __m256i indices)
	{
		return _mm256_i64gather_epi64(pointer, indices, 1);
	}
	template<>
	inline __m256i gatherAVX<uint64_t>(uint64_t* pointer, __m256i indices)
	{
		return _mm256_i64gather_epi64((int64_t*)pointer, indices, 1);
	}

	inline __m256 gatherAVX(float* pointer, __m256i indices)
	{
		return _mm256_castsi256_ps(_mm256_i32gather_epi32((int*)pointer, indices, 1));
	}
	inline __m256d gatherAVX(double* pointer, __m256i indices)
	{
		return _mm256_castsi256_pd(_mm256_i64gather_epi64((double*)pointer, indices, 1));
	}
	#endif

	//Ideally, this should compile to inlined functions bypassing all the extra overhead. 
	template<typename T>
	class SIMD_RAW
	{
	public:
		static const int SIZE;
		static unsigned long long getSIMDBound(unsigned long long s) {return s;}

		SIMD_RAW(){}
		SIMD_RAW(const T& byte){values = byte;}
		
		~SIMD_RAW(){}

		//load / store
		static SIMD_RAW<T>load(const T* pointer){return *pointer;}
		void store(T* pointer){*pointer = values;}
		
		//arithmetic
		SIMD_RAW<T> operator-() const {return -values;}
		SIMD_RAW<T> operator+(const SIMD_RAW<T>& other) const {return values+other.values;}
		SIMD_RAW<T> operator-(const SIMD_RAW<T>& other) const {return values-other.values;}
		SIMD_RAW<T> operator*(const SIMD_RAW<T>& other) const {return values*other.values;}
		SIMD_RAW<T> operator/(const SIMD_RAW<T>& other) const {return values/other.values;}
		
		void operator+=(const SIMD_RAW<T>& other) {values+=other.values;}
		void operator-=(const SIMD_RAW<T>& other) {values-=other.values;}
		void operator*=(const SIMD_RAW<T>& other) {values*=other.values;}
		void operator/=(const SIMD_RAW<T>& other) {values/=other.values;}
		
		//memory access and modification
		
		//bitwise
		SIMD_RAW<T> operator>>(const int shift) const {return values << shift;}
		SIMD_RAW<T> operator<<(const int shift) const {return values >> shift;}
		
		void operator>>=(const int shift) {values <<= shift;}
		void operator<<=(const int shift) {values >>= shift;}
		
		SIMD_RAW<T> operator&(const SIMD_RAW<T>& other) const {return values & other.values;}
		void operator&=(const SIMD_RAW<T>& other) {values &= other.values;}

		SIMD_RAW<T> bitwiseAndNot(const SIMD_RAW<T>& other) const {return ~(values & other.values);}
		
		//comparison
		SIMD_RAW<T> operator>(const SIMD_RAW<T>& other) const {return values > other.values;}
		SIMD_RAW<T> operator<(const SIMD_RAW<T>& other) const {return values < other.values;}
		SIMD_RAW<T> operator==(const SIMD_RAW<T>& other) const {return values == other.values;}
		SIMD_RAW<T> operator!=(const SIMD_RAW<T>& other) const {return values != other.values;}
		
		SIMD_RAW<T> blend(const SIMD_RAW<T>& other, const SIMD_RAW<T>& blendFactor) const
		{
			if(blendFactor.values != 0) 
				return other; 
			return values;
		}
		//special case functions
		SIMD_RAW<T> horizontalAdd(const SIMD_RAW<T>& other) const {return values;}
		T sum() const {return values;}

		//cast function
		template<typename K>
		operator SIMD_RAW<K>() const
		{
			return SIMD_RAW<K>((K)values);
		}
		T values;
	};

	template<typename T>
	class SIMD_SSE : public SIMD_RAW<T>
	{
	public:
		static const int SIZE;
		SIMD_SSE() : SIMD_RAW<T>(){}
		SIMD_SSE(const T& byte) : SIMD_RAW<T>(byte){}
		SIMD_SSE(const SIMD_RAW<T>& other) : SIMD_RAW<T>(other){}
		SIMD_SSE(const SIMD_SSE<T>& other) : SIMD_RAW<T>(other){}
		
		SIMD_SSE(const SIMD_SSE<T>&& other) noexcept : SIMD_RAW<T>(other){}
		SIMD_SSE(const SIMD_RAW<T>&& other) noexcept : SIMD_RAW<T>(other){}
		
		~SIMD_SSE(){}

		// template<typename K>
		// operator SIMD_SSE<K>() const
		// {
		// 	return SIMD_RAW<K>((K)values);
		// }
	};

	template<typename T>
	class SIMD_AVX : public SIMD_RAW<T>
	{
	public:
		static const int SIZE;
		SIMD_AVX() : SIMD_RAW<T>(){}
		SIMD_AVX(const T& byte) : SIMD_RAW<T>(byte){}
		SIMD_AVX(const SIMD_RAW<T>& other) : SIMD_RAW<T>(other){}
		SIMD_AVX(const SIMD_AVX<T>& other) : SIMD_RAW<T>(other){}
		
		SIMD_AVX(const SIMD_AVX<T>&& other) noexcept : SIMD_RAW<T>(other){}
		SIMD_AVX(const SIMD_RAW<T>&& other) noexcept : SIMD_RAW<T>(other){}
		
		~SIMD_AVX(){}

		// template<typename K>
		// operator SIMD_AVX<K>() const
		// {
		// 	return SIMD_RAW<K>((K)values);
		// }
	};

	template<typename T>
	const int SIMD_RAW<T>::SIZE = 1;

	template<typename T>
	const int SIMD_SSE<T>::SIZE = 1;

	template<typename T>
	const int SIMD_AVX<T>::SIZE = 1;





	//DEFAULT EXPECTED TEMPLATE SPECIALIZATIONS
	
	template<> class SIMD_SSE<char>;
	template<> class SIMD_SSE<unsigned char>;
	template<> class SIMD_SSE<short>;
	template<> class SIMD_SSE<unsigned short>;
	template<> class SIMD_SSE<int>;
	template<> class SIMD_SSE<unsigned int>;
	template<> class SIMD_SSE<int64_t>;
	template<> class SIMD_SSE<uint64_t>;
	template<> class SIMD_SSE<float>;
	template<> class SIMD_SSE<double>;
	
	template<> class SIMD_AVX<char>;
	template<> class SIMD_AVX<unsigned char>;
	template<> class SIMD_AVX<short>;
	template<> class SIMD_AVX<unsigned short>;
	template<> class SIMD_AVX<int>;
	template<> class SIMD_AVX<unsigned int>;
	template<> class SIMD_AVX<int64_t>;
	template<> class SIMD_AVX<uint64_t>;
	template<> class SIMD_AVX<float>;
	template<> class SIMD_AVX<double>;
};