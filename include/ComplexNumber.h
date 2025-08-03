#pragma once
#include "BuildOptions.h"
#include "SimpleSerialization.h"
#include "Vec2f.h"
#include "SIMD_Template.h"

namespace smpl
{

	class DLL_OPTION ComplexNumber
	{
	public:
		/**
		 * @brief Creates a ComplexNumber object.
		 * 		Allows for mathematical operations using complex numbers.
		 */
		ComplexNumber();

		/**
		 * @brief Creates a ComplexNumber object.
		 * 		Allows for mathematical operations using complex numbers.
		 * @param real
		 * 		Specifies the real component.
		 * @param imaginary
		 * 		Specifies the imaginary component.
		 */
		ComplexNumber(double real, double imaginary=0);

		/**
		 * @brief Destroys a ComplexNumber object.
		 */
		~ComplexNumber();

		/**
		 * @brief Computes the conjugate which can be utilized in different functions.
		 * 		The complex conjugate of a complex number (A + Bi) = (A - Bi)
		 * 
		 * @return ComplexNumber 
		 */
		ComplexNumber conjugate() const;

		/**
		 * @brief Normalizes the complex number. Behaves in the same way as vector normalization
		 * 		such that A^2 + B^2 = 1
		 * 
		 * @return ComplexNumber 
		 */
		ComplexNumber normalize() const;

		/**
		 * @brief Computes the length of the complex number. Behaves in the same way as vector length
		 * 
		 * @return double 
		 */
		double length() const;

		/**
		 * @brief Converts the Complex number to a Vec2f.
		 * 		x = real component
		 * 		y = imaginary component
		 * 
		 * @return Vec2f 
		 */
		Vec2f toVec2f() const;
		
		ComplexNumber operator+(const ComplexNumber& other) const;
		ComplexNumber operator-(const ComplexNumber& other) const;
		ComplexNumber operator*(const ComplexNumber& other) const;
		ComplexNumber operator-() const;

		ComplexNumber operator*(const double& other) const;
		ComplexNumber operator/(const double& other) const;
		ComplexNumber operator/(const ComplexNumber& other) const;

		friend ComplexNumber operator*(double value, const ComplexNumber& other)
		{
			return other*value;
		}
		friend ComplexNumber operator/(double value, const ComplexNumber& other)
		{
			return ComplexNumber(value, 0) / other;
		}

		void operator+=(const ComplexNumber& other);
		void operator-=(const ComplexNumber& other);
		void operator*=(const ComplexNumber& other);

		void operator*=(double other);
		void operator/=(double other);

		//logic operators
		bool operator==(const ComplexNumber& other) const;
		bool operator!=(const ComplexNumber& other) const;
		
		double real = 0;
		double imaginary = 0;
	private:
		SERIALIZE_CLASS_LIGHT(ComplexNumber, real, imaginary)
	};

	template<>
	template<typename K>
	inline Matrix<ComplexNumber>::operator Matrix<K>() const
	{
		Matrix<K> output(rows, columns);
		K* ouptutDataP = output.getData();
		for(size_t i=0; i<rows*columns; i++)
		{
			ouptutDataP[i] = (K)data[i].real;
		}
		return output;
	}
	void formatToString(StringStream& stream, const ComplexNumber& v, const std::string& options);


	//simd stuff. Identical to loading real and imaginary into 
	#ifdef __SSE4_2__
	template<>
	class SIMD_SSE<ComplexNumber>
	{
	public:
		static const int SIZE;
		static unsigned long long getSIMDBound(unsigned long long s) {return s;}
		SIMD_SSE(){}
		SIMD_SSE(const ComplexNumber& v){values = _mm_set_pd(v.imaginary, v.real);}
		SIMD_SSE(__m128d v){values = v;}
		
		SIMD_SSE(const SIMD_SSE<ComplexNumber>& other) {values = other.values;}
		SIMD_SSE(const SIMD_SSE<ComplexNumber>&& other) noexcept {values = other.values;}
		
		~SIMD_SSE(){}

		static SIMD_SSE<ComplexNumber> load(ComplexNumber* p)
		{
			// return _mm_loadu_pd((double*)p);
			return _mm_set_pd(p->imaginary, p->real);
		}
		void store(ComplexNumber* p)
		{
			// _mm_storeu_pd((double*)p, values);
			double data[2];
			_mm_storeu_pd(data, values);
			p->real = data[0];
			p->imaginary = data[1];
		}
		
		SIMD_SSE<ComplexNumber> operator-() const {return SEML::negate(values);}
		SIMD_SSE<ComplexNumber> operator+(const SIMD_SSE<ComplexNumber>& other) const {return _mm_add_pd(values, other.values);}
		SIMD_SSE<ComplexNumber> operator-(const SIMD_SSE<ComplexNumber>& other) const {return _mm_sub_pd(values, other.values);}
		
		SIMD_SSE<ComplexNumber> operator*(const SIMD_SSE<ComplexNumber>& other) const
		{
			//A.real*B.real - A.imaginary*B.imaginary
			//A.imaginary*B.real + A.imaginary*B.real

			__m128d swappedValues = _mm_shuffle_pd(values, values, 0b01);
			__m128d V1 = _mm_mul_pd(values, other.values); //real*real, imagin*imagin
			__m128d V2 = _mm_mul_pd(swappedValues, other.values); //real*imagin, imagin*real

			//Stealing Conjugate Code
			const int blendMask = 0b00000010;
			__m128d negValues = SEML::negate(V1);
			V1 = _mm_blend_pd(V1, negValues, blendMask);

			return SIMD_SSE<ComplexNumber>(_mm_hadd_pd(V1, V2)); //Contains final result
		}
		SIMD_SSE<ComplexNumber> operator/(const SIMD_SSE<ComplexNumber>& other) const
		{
			SIMD_SSE<ComplexNumber> otherCon = other.conjugate();
			SIMD_SSE<ComplexNumber> numerator = operator*(otherCon);
			SIMD_SSE<ComplexNumber> denominator = other*otherCon;

			//extract the first value in the denominator
			__m128d justRealPart = _mm_shuffle_pd(denominator.values, denominator.values, 0b00);
			return SIMD_SSE<ComplexNumber>(_mm_div_pd(numerator.values, justRealPart));
		}

		SIMD_SSE<ComplexNumber> operator*(const SIMD_SSE<double>& other) const
		{
			return _mm_mul_pd(values, other.values);
		}
		SIMD_SSE<ComplexNumber> operator/(const SIMD_SSE<double>& other) const
		{
			return _mm_div_pd(values, other.values);
		}
		
		void operator+=(const SIMD_SSE<ComplexNumber>& other){values = operator+(other).values;}
		void operator-=(const SIMD_SSE<ComplexNumber>& other){values = operator-(other).values;}
		void operator*=(const SIMD_SSE<ComplexNumber>& other){values = operator*(other).values;}
		void operator/=(const SIMD_SSE<ComplexNumber>& other){values = operator/(other).values;}

		SIMD_SSE<double> length() const
		{
			__m128d sqrValue = _mm_mul_pd(values, values);
			__m128d temp = _mm_hadd_pd(sqrValue, sqrValue); //contains A1+B1, ...
			return _mm_sqrt_pd(temp);
		}

		SIMD_SSE<ComplexNumber> normalize() const
		{
			__m128d len = length().values;
			return SIMD_SSE<ComplexNumber>(_mm_div_pd(values, len));
		}
		SIMD_SSE<ComplexNumber> conjugate() const
		{
			const int blendMask = 0b00000010;
			__m128d negValues = SEML::negate(values);
			return SIMD_SSE<ComplexNumber>(_mm_blend_pd(values, negValues, blendMask));
		}

		
		//memory access and modification
		
		//special case functions
		ComplexNumber sum() const
		{
			double data[2];
			_mm_storeu_pd(data, values);
			return ComplexNumber(data[0], data[1]);
		}
		__m128d values;
	};
	
	inline const int SIMD_SSE<ComplexNumber>::SIZE = 1;
	#endif
	
	///TODO: FIX LATER
	#ifdef __AVX2__
	template<>
	class SIMD_AVX<ComplexNumber>
	{
	public:
		static const int SIZE;
		static unsigned long long getSIMDBound(unsigned long long s) {return (s>>1)<<1;}
		SIMD_AVX(){}
		SIMD_AVX(const ComplexNumber& v){values = _mm256_set_pd(v.imaginary, v.real, v.imaginary, v.real);}
		SIMD_AVX(const ComplexNumber& v1, const ComplexNumber& v2){values = _mm256_set_pd(v2.imaginary, v2.real, v1.imaginary, v1.real);}
		SIMD_AVX(__m256d v){values = v;}
		
		SIMD_AVX(const SIMD_AVX<ComplexNumber>& other) {values = other.values;}
		SIMD_AVX(const SIMD_AVX<ComplexNumber>&& other) noexcept {values = other.values;}
		
		~SIMD_AVX(){}

		static SIMD_AVX<ComplexNumber> load(ComplexNumber* p)
		{
			// return _mm256_loadu_pd((double*)p);
			return _mm256_set_pd(p[1].imaginary, p[1].real, p[0].imaginary, p[0].real);
		}
		void store(ComplexNumber* p)
		{
			// _mm256_storeu_pd((double*)p, values);

			double data[4];
			_mm256_storeu_pd(data, values);
			p[0].real = data[0];
			p[0].imaginary = data[1];
			p[1].real = data[2];
			p[1].imaginary = data[3];
		}
		
		SIMD_AVX<ComplexNumber> operator-() const {return SEML::negate(values);}
		SIMD_AVX<ComplexNumber> operator+(const SIMD_AVX<ComplexNumber>& other) const {return _mm256_add_pd(values, other.values);}
		SIMD_AVX<ComplexNumber> operator-(const SIMD_AVX<ComplexNumber>& other) const {return _mm256_sub_pd(values, other.values);}
		
		SIMD_AVX<ComplexNumber> operator*(const SIMD_AVX<ComplexNumber>& other) const
		{
			
			//A.real*B.real - A.imaginary*B.imaginary
			//A.imaginary*B.real + A.imaginary*B.real

  			const __m256d neg = _mm256_setr_pd(1.0, -1.0, 1.0, -1.0);

			__m256d swappedValues = _mm256_permute_pd(other.values, 0b0101);
			swappedValues = _mm256_mul_pd(swappedValues, neg);
			
			__m256d V1 = _mm256_mul_pd(values, other.values); //real*real, imagin*imagin
			__m256d V2 = _mm256_mul_pd(values, swappedValues); //real*imagin, imagin*real

			return SIMD_AVX<ComplexNumber>(_mm256_hsub_pd(V1, V2)); //Contains final result
		}
		SIMD_AVX<ComplexNumber> operator/(const SIMD_AVX<ComplexNumber>& other) const
		{
			SIMD_AVX<ComplexNumber> otherCon = other.conjugate();
			SIMD_AVX<ComplexNumber> numerator = operator*(otherCon);
			SIMD_AVX<ComplexNumber> denominator = other*otherCon;

			//extract the first value in the denominator
			__m256d justRealPart = _mm256_shuffle_pd(denominator.values, denominator.values, 0b0000);
			return SIMD_AVX<ComplexNumber>(_mm256_div_pd(numerator.values, justRealPart));
		}
		
		SIMD_AVX<ComplexNumber> operator*(const SIMD_AVX<double>& other) const
		{
			return _mm256_mul_pd(values, other.values);
		}
		SIMD_AVX<ComplexNumber> operator/(const SIMD_AVX<double>& other) const
		{
			return _mm256_div_pd(values, other.values);
		}
		
		void operator+=(const SIMD_AVX<ComplexNumber>& other){values = operator+(other).values;}
		void operator-=(const SIMD_AVX<ComplexNumber>& other){values = operator-(other).values;}
		void operator*=(const SIMD_AVX<ComplexNumber>& other){values = operator*(other).values;}
		void operator/=(const SIMD_AVX<ComplexNumber>& other){values = operator/(other).values;}

		SIMD_AVX<double> length() const
		{
			const int controlMask = 0b10011000;
			__m256d sqrValue = _mm256_mul_pd(values, values);
			__m256d temp = _mm256_hadd_pd(sqrValue, sqrValue); //contains A1+B1, A2+B2, ..., ...
			temp = _mm256_sqrt_pd(temp);
			//rearrange such that sqrt(A1+B1), sqrt(A1+B1), sqrt(A2+B2), sqrt(A2+B2)
			return _mm256_permute4x64_pd(temp, controlMask);
		}

		SIMD_AVX<ComplexNumber> normalize() const
		{
			__m256d len = length().values;
			return SIMD_AVX<ComplexNumber>(_mm256_div_pd(values, len));
		}
		SIMD_AVX<ComplexNumber> conjugate() const
		{
			const int blendMask = 0b1010;
			__m256d negValues = SEML::negate(values);
			return SIMD_AVX<ComplexNumber>(_mm256_blend_pd(values, negValues, blendMask));
		}

		
		//memory access and modification
		
		//special case functions
		ComplexNumber sum() const
		{
			double data[4];
			_mm256_storeu_pd(data, values);
			return ComplexNumber(data[0]+data[2], data[1]+data[3]); //lazy approach
		}
		__m256d values;
	};
	inline const int SIMD_AVX<ComplexNumber>::SIZE = 2;
	#endif
}