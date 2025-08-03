#pragma once
#include <immintrin.h>
struct Float16
{
    Float16(){}
    Float16(const float& data)
    {
        operator=(data);
    }
    void operator=(const float& data)
    {
        rawData = _cvtss_sh(data, 0);
    }
    
    unsigned short& getRawData()
    {
        return rawData;
    }

    operator float() const
    {
        return _cvtsh_ss(rawData);
    }

    Float16 operator-() const
    {
        Float16 testData;
        testData.rawData = rawData ^ 0x8000;
        return testData;
    }

    static Float16 abs(const Float16& v)
    {
        Float16 testData;
        testData.rawData = v.rawData ^ 0x7FFF;
        return testData;
    }

    unsigned short rawData = 0;
};

struct BFloat16
{
    void operator=(const float& data)
    {
        int p = *(int*)&data;
        rawData = (p & 0x80000000) >> 16;
        unsigned short expValue = ((p & 0x7F800000)>>16);
        rawData |= expValue;
        rawData |= (p>>16) & 0x7F;
    }
    unsigned short& getRawData()
    {
        return rawData;
    }

    operator float() const
    {
        int temp = 0;
        temp = (rawData&0x8000) << 16;
        unsigned int expValue = ((rawData&0x7F80)<<16);
        temp |= expValue;
        temp |= (rawData&0x7F) << 16;
        return *(float*)&temp;
    }


    BFloat16 operator-() const
    {
        return BFloat16{(unsigned short)(rawData ^ 0x8000)};
    }

    static BFloat16 abs(const BFloat16& v)
    {
        return BFloat16{(unsigned short)(v.rawData ^ 0x7FFF)};
    }

    unsigned short rawData = 0;
};