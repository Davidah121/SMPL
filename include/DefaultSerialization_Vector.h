#pragma once
#include "SimpleSerialization.h"
#include <vector>

template<typename T>
void staticSerializeHandler(SerializedStreamable& output, DataFormatter& formatter, const std::string varName, std::vector<T>& var)
{
    formatter.writeStart(output, DataFormatter::FORMAT_ARRAY, TypeInfo::get<T>(), varName, var.size());
    
    for(int64_t i=0; i<var.size(); i++)
    {
        staticSerialize(output, formatter, var[i]);
    }
}

template<typename T>
void staticDeserializeHandler(SerializedStreamable& input, DataFormatter& formatter, const std::string varName, std::vector<T>& var)
{
    int64_t s = formatter.readStart(input, DataFormatter::FORMAT_ARRAY, TypeInfo::get<T>(), varName);
    std::cout << s << std::endl;
    if(s < 0)
        return; //couldn't find it. potentially an error.
        
    
    var = std::vector<T>(s);
    for(int64_t i=0; i<var.size(); i++)
    {
        staticDeserialize(input, formatter, var[i]); //need default constructor
    }
}