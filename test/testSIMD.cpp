#include <catch_amalgamated.hpp>
#include "SIMD.h"
#include "MathExt.h"

using namespace smpl;

template<typename T>
void testSIMDSSETemplate()
{
    std::vector<T> arrValues = std::vector<T>(SIMD_SSE<T>::SIZE);
    std::vector<T> resultArr = std::vector<T>(SIMD_SSE<T>::SIZE);
	
    for(int i=0; i<SIMD_SSE<T>::SIZE; i++)
    {
        arrValues[i] = i+1;
        resultArr[i] = 0;
    }

	SIMD_SSE<T> baseValues = SIMD_SSE<T>::load(arrValues.data());

	SECTION("ADDITION")
	{
		SIMD_SSE<T> result = baseValues + baseValues;
		result.store(resultArr.data());
		for(int i=0; i<SIMD_SSE<T>::SIZE; i++)
		{
			REQUIRE(resultArr[i] == (arrValues[i]+arrValues[i]));
		}
	}
	SECTION("SUBTRACTION")
	{
		SIMD_SSE<T> result = baseValues - baseValues;
		result.store(resultArr.data());
		for(int i=0; i<SIMD_SSE<T>::SIZE; i++)
		{
			REQUIRE(resultArr[i] == (arrValues[i]-arrValues[i]));
		}
	}
	SECTION("MULTIPLICATION")
	{
		SIMD_SSE<T> result = baseValues * 3;
		result.store(resultArr.data());
		for(int i=0; i<SIMD_SSE<T>::SIZE; i++)
		{
			REQUIRE(resultArr[i] == (arrValues[i]*3));
		}
	}
	SECTION("DIVISION")
	{
		SIMD_SSE<T> result = baseValues / 3;
		result.store(resultArr.data());
		for(int i=0; i<SIMD_SSE<T>::SIZE; i++)
		{
			T err = abs((double)(resultArr[i] - (arrValues[i]/3)));
			REQUIRE(err < EPSILON);
		}
	}
	SECTION("SUMMATION")
	{
		double result = baseValues.sum();
		double actualResult = 0;
		for(int i=0; i<SIMD_SSE<T>::SIZE; i++)
		{
			actualResult += arrValues[i];
		}
		REQUIRE(actualResult == result);
	}
}

TEST_CASE("Test the SIMD_SSE classes" "[SIMD_SSE]")
{
    SECTION("SIMD_SSE_8"){testSIMDSSETemplate<char>();}
    SECTION("SIMD_SSE_U8"){testSIMDSSETemplate<unsigned char>();}
    SECTION("SIMD_SSE_16"){testSIMDSSETemplate<short>();}
    SECTION("SIMD_SSE_U16"){testSIMDSSETemplate<unsigned short>();}
    SECTION("SIMD_SSE_32"){testSIMDSSETemplate<int>();}
    SECTION("SIMD_SSE_U32"){testSIMDSSETemplate<unsigned int>();}
    SECTION("SIMD_SSE_FP32"){testSIMDSSETemplate<float>();}
    SECTION("SIMD_SSE_FP64"){testSIMDSSETemplate<double>();}
}


template<typename T>
void testSIMDAVXTemplate()
{
    std::vector<T> arrValues = std::vector<T>(SIMD_AVX<T>::SIZE);
    std::vector<T> resultArr = std::vector<T>(SIMD_AVX<T>::SIZE);
	
    for(int i=0; i<SIMD_AVX<T>::SIZE; i++)
    {
        arrValues[i] = i+1;
        resultArr[i] = 0;
    }

	SIMD_AVX<T> baseValues = SIMD_AVX<T>::load(arrValues.data());

	SECTION("ADDITION")
	{
		SIMD_AVX<T> result = baseValues + baseValues;
		result.store(resultArr.data());
		for(int i=0; i<SIMD_AVX<T>::SIZE; i++)
		{
			REQUIRE(resultArr[i] == (arrValues[i]+arrValues[i]));
		}
	}
	SECTION("SUBTRACTION")
	{
		SIMD_AVX<T> result = baseValues - baseValues;
		result.store(resultArr.data());
		for(int i=0; i<SIMD_AVX<T>::SIZE; i++)
		{
			REQUIRE(resultArr[i] == (arrValues[i]-arrValues[i]));
		}
	}
	SECTION("MULTIPLICATION")
	{
		SIMD_AVX<T> result = baseValues * 3;
		result.store(resultArr.data());
		for(int i=0; i<SIMD_AVX<T>::SIZE; i++)
		{
			REQUIRE(resultArr[i] == (arrValues[i]*3));
		}
	}
	SECTION("DIVISION")
	{
		SIMD_AVX<T> result = baseValues / 3;
		result.store(resultArr.data());
		for(int i=0; i<SIMD_SSE<T>::SIZE; i++)
		{
			T err = abs((double)(resultArr[i] - (arrValues[i]/3)));
			REQUIRE(err < EPSILON);
		}
	}
	SECTION("SUMMATION")
	{
		double result = baseValues.sum();
		double actualResult = 0;
		for(int i=0; i<SIMD_AVX<T>::SIZE; i++)
		{
			actualResult += arrValues[i];
		}
		REQUIRE(actualResult == result);
	}
}

TEST_CASE("Test the SIMD_AVX classes" "[SIMD_AVX]")
{
    SECTION("SIMD_AVX_8"){testSIMDAVXTemplate<char>();}
    SECTION("SIMD_AVX_U8"){testSIMDAVXTemplate<unsigned char>();}
    SECTION("SIMD_AVX_16"){testSIMDAVXTemplate<short>();}
    SECTION("SIMD_AVX_U16"){testSIMDAVXTemplate<unsigned short>();}
    SECTION("SIMD_AVX_32"){testSIMDAVXTemplate<int>();}
    SECTION("SIMD_AVX_U32"){testSIMDAVXTemplate<unsigned int>();}
    SECTION("SIMD_AVX_FP32"){testSIMDAVXTemplate<float>();}
    SECTION("SIMD_AVX_FP64"){testSIMDAVXTemplate<double>();}
}