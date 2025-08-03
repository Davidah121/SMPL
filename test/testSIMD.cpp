#include <catch_amalgamated.hpp>
#include "SIMD.h"

template<typename T, typename V>
void testSIMDTemplate()
{
    std::vector<V> arrValues = std::vector<V>(T::SIZE);
    std::vector<V> resultArr = std::vector<V>(T::SIZE);
	
    for(int i=0; i<T::SIZE; i++)
    {
        arrValues[i] = i+1;
        resultArr[i] = 0;
    }

	T baseValues = T::load(arrValues.data());

	SECTION("ADDITION")
	{
		T result = baseValues + baseValues;
		result.store(resultArr.data());
		for(int i=0; i<T::SIZE; i++)
		{
			REQUIRE(resultArr[i] == (arrValues[i]+arrValues[i]));
		}
	}
	SECTION("SUBTRACTION")
	{
		T result = baseValues - baseValues;
		result.store(resultArr.data());
		for(int i=0; i<T::SIZE; i++)
		{
			REQUIRE(resultArr[i] == (arrValues[i]-arrValues[i]));
		}
	}
	SECTION("MULTIPLICATION")
	{
		T result = baseValues * 3;
		result.store(resultArr.data());
		for(int i=0; i<T::SIZE; i++)
		{
			REQUIRE(resultArr[i] == (arrValues[i]*3));
		}
	}
	SECTION("DIVISION")
	{
		T result = baseValues / 3;
		result.store(resultArr.data());
		for(int i=0; i<T::SIZE; i++)
		{
			REQUIRE(resultArr[i] == (arrValues[i]/3));
		}
	}
	SECTION("SUMMATION")
	{
		double result = baseValues.sum();
		double actualResult = 0;
		for(int i=0; i<T::SIZE; i++)
		{
			actualResult += arrValues[i];
		}
		REQUIRE(actualResult == result);
	}
}

TEST_CASE("Test the SIMD_128 classes" "[SIMD_128]")
{
    // SECTION("SIMD_128_8"){testSIMDTemplate<smpl::SIMD_128_8, char>();}
    // SECTION("SIMD_128_U8"){testSIMDTemplate<smpl::SIMD_128_U8, unsigned char>();}
    // SECTION("SIMD_128_16"){testSIMDTemplate<smpl::SIMD_128_16, short>();}
    // SECTION("SIMD_128_U16"){testSIMDTemplate<smpl::SIMD_128_U16, unsigned short>();}
    // SECTION("SIMD_128_32"){testSIMDTemplate<smpl::SIMD_128_32, int>();}
    // SECTION("SIMD_128_U32"){testSIMDTemplate<smpl::SIMD_128_U32, unsigned int>();}
    // SECTION("SIMD_128_FP32"){testSIMDTemplate<smpl::SIMD_128_FP32, float>();}
    // SECTION("SIMD_128_FP64"){testSIMDTemplate<smpl::SIMD_128_FP64, double>();}
}

//TODO: NO WORK. FIX. UNSURE WHY AS IT WORKS NORMALLY