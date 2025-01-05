#pragma once
#include "SimpleSerialization.h"
#include <map>
#include <unordered_map>

#pragma region MAP

template<typename key, typename T>
void staticSerializeHandler(SerializedStreamable& output, DataFormatter& formatter, const std::string varName, std::map<key, T>& var)
{
    formatter.writeStart(output, DataFormatter::FORMAT_ARRAY, TypeInfo::get<std::map<key, T>>(), varName, var.size());
    for(std::pair<key, T>& pair : var)
    {
        staticSerialize(output, formatter, pair);
    }
}

template<typename key, typename T>
void staticDeserializeHandler(SerializedStreamable& input, DataFormatter& formatter, const std::string varName, std::map<key, T>& var)
{
    int64_t size = formatter.readStart(input, DataFormatter::FORMAT_ARRAY, TypeInfo::get<std::map<key, T>>(), varName);
    if(size < 0)
        return; //couldn't find it potentially
    
    var.clear();
    for(int64_t i=0; i<size; i++)
    {
        std::pair<key, T> p;
        staticDeserialize(input, formatter, p);
        var.insert(p);
    }
}

template<typename key, typename T>
void staticSerializeHandler(SerializedStreamable& output, DataFormatter& formatter, const std::string varName, std::multimap<key, T>& var)
{
    formatter.writeStart(output, DataFormatter::FORMAT_ARRAY, TypeInfo::get<std::multimap<key, T>>(), varName, var.size());
    for(std::pair<key, T>& pair : var)
    {
        staticSerialize(output, formatter, pair);
    }
}

template<typename key, typename T>
void staticDeserializeHandler(SerializedStreamable& input, DataFormatter& formatter, const std::string varName, std::multimap<key, T>& var)
{
    int64_t size = formatter.readStart(input, DataFormatter::FORMAT_ARRAY, TypeInfo::get<std::multimap<key, T>>(), varName);
    if(size < 0)
        return; //couldn't find it potentially
    
    var.clear();
    for(int64_t i=0; i<size; i++)
    {
        std::pair<key, T> p;
        staticDeserialize(input, formatter, p);
        var.insert(p);
    }
}

#pragma endregion

#pragma region UNORDERED_MAP
template<typename key, typename T>
void staticSerializeHandler(SerializedStreamable& output, DataFormatter& formatter, const std::string varName, std::unordered_map<key, T>& var)
{
    formatter.writeStart(output, DataFormatter::FORMAT_ARRAY, TypeInfo::get<std::unordered_map<key, T>>(), varName, var.size());
    for(std::pair<key, T>& pair : var)
    {
        staticSerialize(output, formatter, pair);
    }
}

template<typename key, typename T>
void staticDeserializeHandler(SerializedStreamable& input, DataFormatter& formatter, const std::string varName, std::unordered_map<key, T>& var)
{
    int64_t size = formatter.readStart(input, DataFormatter::FORMAT_ARRAY, TypeInfo::get<std::unordered_map<key, T>>(), varName);
    if(size < 0)
        return; //couldn't find it potentially
    
    var.clear();
    for(int64_t i=0; i<size; i++)
    {
        std::pair<key, T> p;
        staticDeserialize(input, formatter, p);
        var.insert(p);
    }
}

template<typename key, typename T>
void staticSerializeHandler(SerializedStreamable& output, DataFormatter& formatter, const std::string varName, std::unordered_multimap<key, T>& var)
{
    formatter.writeStart(output, DataFormatter::FORMAT_ARRAY, TypeInfo::get<std::unordered_multimap<key, T>>(), varName, var.size());
    for(std::pair<key, T>& pair : var)
    {
        staticSerialize(output, formatter, pair);
    }
}

template<typename key, typename T>
void staticDeserializeHandler(SerializedStreamable& input, DataFormatter& formatter, const std::string varName, std::unordered_multimap<key, T>& var)
{
    int64_t size = formatter.readStart(input, DataFormatter::FORMAT_ARRAY, TypeInfo::get<std::unordered_multimap<key, T>>(), varName);
    if(size < 0)
        return; //couldn't find it potentially
    
    var.clear();
    for(int64_t i=0; i<size; i++)
    {
        std::pair<key, T> p;
        staticDeserialize(input, formatter, p);
        var.insert(p);
    }
}

#pragma endregion