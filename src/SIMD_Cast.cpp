#include "SIMD.h"

//implement cast operators here so that they use the correct constructor
namespace smpl
{
	#ifdef __SSE4_2__
    SIMD_SSE<char>::operator SIMD_SSE<unsigned char>() const
    {
        return SIMD_SSE<unsigned char>(values);
    }
    SIMD_SSE<short>::operator SIMD_SSE<unsigned short>() const
    {
        return SIMD_SSE<unsigned short>(values);
    }
    SIMD_SSE<int>::operator SIMD_SSE<unsigned int>() const
    {
        return SIMD_SSE<unsigned int>(values);
    }
    SIMD_SSE<int>::operator SIMD_SSE<float>() const
    {
        return SIMD_SSE<float>(SEML::int32ToFloat(values));
    }
    SIMD_SSE<unsigned int>::operator SIMD_SSE<float>() const
    {
        return SIMD_SSE<float>(SEML::uint32ToFloat(values));
    }
    
    SIMD_SSE<int64_t>::operator SIMD_SSE<uint64_t>() const
    {
        return SIMD_SSE<uint64_t>(values);
    }
    SIMD_SSE<int64_t>::operator SIMD_SSE<double>() const
    {
        return SIMD_SSE<double>(SEML::fastInt64ToDouble(values));
    }
    SIMD_SSE<uint64_t>::operator SIMD_SSE<double>() const
    {
        return SIMD_SSE<double>(SEML::fastUInt64ToDouble(values));
    }

    SIMD_SSE<float>::operator SIMD_SSE<int>() const
    {
        return SIMD_SSE<int>(SEML::floatToInt32(values));
    }
    SIMD_SSE<float>::operator SIMD_SSE<unsigned int>() const
    {
        return SIMD_SSE<unsigned int>(SEML::floatToUInt32(values));
    }
    SIMD_SSE<double>::operator SIMD_SSE<int64_t>() const
    {
        return SIMD_SSE<int64_t>(SEML::fastDoubleToInt64(values));
    }
    SIMD_SSE<double>::operator SIMD_SSE<uint64_t>() const
    {
        return SIMD_SSE<uint64_t>(SEML::fastDoubleToUInt64(values));
    }
    #endif

    
	#ifdef __AVX2__
    SIMD_AVX<char>::operator SIMD_AVX<unsigned char>() const
    {
        return SIMD_AVX<unsigned char>(values);
    }
    SIMD_AVX<short>::operator SIMD_AVX<unsigned short>() const
    {
        return SIMD_AVX<unsigned short>(values);
    }
    SIMD_AVX<int>::operator SIMD_AVX<unsigned int>() const
    {
        return SIMD_AVX<unsigned int>(values);
    }
    SIMD_AVX<int>::operator SIMD_AVX<float>() const
    {
        return SIMD_AVX<float>(SEML::int32ToFloat(values));
    }
    SIMD_AVX<unsigned int>::operator SIMD_AVX<float>() const
    {
        return SIMD_AVX<float>(SEML::uint32ToFloat(values));
    }
    
    SIMD_AVX<int64_t>::operator SIMD_AVX<uint64_t>() const
    {
        return SIMD_AVX<uint64_t>(values);
    }
    SIMD_AVX<int64_t>::operator SIMD_AVX<double>() const
    {
        return SIMD_AVX<double>(SEML::fastInt64ToDouble(values));
    }
    SIMD_AVX<uint64_t>::operator SIMD_AVX<double>() const
    {
        return SIMD_AVX<double>(SEML::fastUInt64ToDouble(values));
    }

    SIMD_AVX<float>::operator SIMD_AVX<int>() const
    {
        return SIMD_AVX<int>(SEML::floatToInt32(values));
    }
    SIMD_AVX<float>::operator SIMD_AVX<unsigned int>() const
    {
        return SIMD_AVX<unsigned int>(SEML::floatToUInt32(values));
    }
    SIMD_AVX<double>::operator SIMD_AVX<int64_t>() const
    {
        return SIMD_AVX<int64_t>(SEML::fastDoubleToInt64(values));
    }
    SIMD_AVX<double>::operator SIMD_AVX<uint64_t>() const
    {
        return SIMD_AVX<uint64_t>(SEML::fastDoubleToUInt64(values));
    }
    #endif
}