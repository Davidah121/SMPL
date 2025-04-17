#pragma once
#include "SimpleSerialization.h"
#include <array>

template<typename T, size_t S>
void staticSerialize(SerializedStreamable& output, DataFormatter& formatter, const std::string varName, std::array<T, S>& var)
{
    formatter.writeStart(output, DataFormatter::FORMAT_ARRAY, TypeInfo::get<std::array<T, S>>(), varName, S);
    for(size_t i=0; i<S; i++)
    {
        staticSerialize(output, formatter, var[i]);
    }
}

template<typename T, size_t S>
void staticDeserialize(SerializedStreamable& input, DataFormatter& formatter, const std::string varName, std::array<T, S>& var)
{
    int64_t readSize = formatter.readStart(input, DataFormatter::FORMAT_ARRAY, TypeInfo::get<std::array<T, S>>(), varName);
    if(S < 0)
        return; //couldn't find it. probably okay.
    
    if(S != readSize)
        throw std::runtime_error("std::array deserialization failed. Size read does not equal desired size. Read " + std::to_string(readSize) + ", Needs " + std::to_string(S));

    for(size_t i=0; i<S; i++)
    {
        staticDeserialize(input, formatter, var[i]);
    }
}