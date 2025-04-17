#pragma once
#include "SimpleSerialization.h"
#include <vector>

void staticSerializeHandler(SerializedStreamable& output, DataFormatter& formatter, const std::string varName, SerializedStreamableVector& var)
{
    formatter.writeStart(output, DataFormatter::FORMAT_OBJECT, TypeInfo::get<SerializedStreamableVector>(), varName, 1);
    formatter.writeString(output, TypeInfo::get<char>(), var.getBuffer().data(), var.getBuffer().size());
}

void staticDeserializeHandler(SerializedStreamable& input, DataFormatter& formatter, const std::string varName, SerializedStreamableVector& var)
{
    int64_t s = formatter.readStart(input, DataFormatter::FORMAT_OBJECT, TypeInfo::get<SerializedStreamableVector>(), varName);
    if(s != 1)
        return; //couldn't find it. potentially an error.
    formatter.readString(input, TypeInfo::get<char>(), *((std::vector<char>*)&var.getBuffer()));
}