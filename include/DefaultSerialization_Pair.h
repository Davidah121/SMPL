#pragma once
#include "SimpleSerialization.h"
#include <map>
#include <unordered_map>

template<typename T1, typename T2>
void staticSerializeHandler(SerializedStreamable& output, DataFormatter& formatter, const std::string varName, std::pair<T1, T2>& var)
{
    formatter.writeStart(output, DataFormatter::FORMAT_OBJECT, TypeInfo::get<std::pair<T1, T2>>(), varName, 1);
    staticSerialize(output, var.first); //write first
    staticSerialize(output, var.second); //write second
}

template<typename T1, typename T2>
void staticDeserializeHandler(SerializedStreamable& input, DataFormatter& formatter, const std::string varName, std::pair<T1, T2>& var)
{
    int64_t elements = formatter.readStart(input, DataFormatter::FORMAT_OBJECT, TypeInfo::get<std::pair<T1, T2>>(), varName, 1);
    if(elements < 1)
        return;
    
    staticDeserialize(input, var.first); //read first
    staticDeserialize(input, var.second); //read second
}