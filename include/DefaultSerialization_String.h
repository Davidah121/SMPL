#pragma once
#include "SimpleSerialization.h"
#include <string>

template<typename T>
void staticSerializeHandler(SerializedStreamable& output, DataFormatter& formatter, const std::string varName, std::basic_string<T>& var)
{
    //Generic possibly. Can't write out as a simple string like others.
    //null character not needed
    formatter.writeStart(output, DataFormatter::FORMAT_ARRAY, TypeInfo::get<std::basic_string<T>>(), varName, var.size());
    for(T& c : var)
    {
        staticSerialize(output, c); //must be trivial but that doesn't mean simple
    }
}

template<typename T>
void staticDeserializeHandler(SerializedStreamable& input, DataFormatter& formatter, const std::string varName, std::basic_string<T>& var)
{
    //Generic possibly. Can't write out as a simple string like others.
    //null character not needed
    int64_t s = formatter.readStart(input, DataFormatter::FORMAT_ARRAY, TypeInfo::get<std::basic_string<T>>(), varName);
    if(s < 0)
        return; //couldn't find it.

    var.reserve(s); //reserve data
    for(int64_t i=0; i<s; i++)
    {
        staticDeserialize(input, var[i]); //must be trivial but that doesn't mean simple
    }
}

void staticSerializeHandler(SerializedStreamable& output, DataFormatter& formatter, const std::string varName, std::string& var)
{
    formatter.writeStart(output, DataFormatter::FORMAT_DATA, TypeInfo::get<std::string>(), varName, 1);
    formatter.writeString(output, TypeInfo::get<char>(), var.data(), var.size());
}

void staticDeserializeHandler(SerializedStreamable& input, DataFormatter& formatter, const std::string varName, std::string& var)
{
    std::vector<char> buffer;
    int64_t elements = formatter.readStart(input, DataFormatter::FORMAT_DATA, TypeInfo::get<std::string>(), varName);
    if(elements < 1)
        return; //couldn't find it.
    
    formatter.readString(input, TypeInfo::get<char>(), buffer);
    var.assign(buffer.data());
}

void staticSerializeHandler(SerializedStreamable& output, DataFormatter& formatter, const std::string varName, std::wstring& var)
{
    formatter.writeStart(output, DataFormatter::FORMAT_DATA, TypeInfo::get<std::wstring>(), varName, 1);
    formatter.writeString(output, TypeInfo::get<wchar_t>(), var.data(), var.size());
}

void staticDeserializeHandler(SerializedStreamable& input, DataFormatter& formatter, const std::string varName, std::wstring& var)
{
    std::vector<char> buffer;
    int64_t elements = formatter.readStart(input, DataFormatter::FORMAT_DATA, TypeInfo::get<std::wstring>(), varName);
    if(elements < 1)
        return; //couldn't find it.
    
    formatter.readString(input, TypeInfo::get<wchar_t>(), buffer);
    var.assign((wchar_t*)buffer.data());
}