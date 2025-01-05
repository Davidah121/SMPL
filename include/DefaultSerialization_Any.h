#pragma once
#include "SimpleSerialization.h"
#include <any>
#include <functional>
#include <unordered_map>

class SerializeAnyFunctionMapper
{
public:
    static void mapFunction(std::string className, std::function<void(SerializedStreamable&, DataFormatter&, const std::string&, std::any&)> serializeFunction, std::function<void(SerializedStreamable&, DataFormatter&, const std::string&, std::any&)> deserializeFunction);
    static std::pair<std::function<void(SerializedStreamable&, DataFormatter&, const std::string&, std::any&)>, std::function<void(SerializedStreamable&, DataFormatter&, const std::string&, std::any&)>> getFunctions(std::string className);

private:
    static SerializeAnyFunctionMapper& getSingleton();
    SerializeAnyFunctionMapper() {}
    ~SerializeAnyFunctionMapper() {}
    std::unordered_map<std::string, std::pair<std::function<void(SerializedStreamable&, DataFormatter&, const std::string&, std::any&)>, std::function<void(SerializedStreamable&, DataFormatter&, const std::string&, std::any&)>>> functionMappings;
};

inline SerializeAnyFunctionMapper& SerializeAnyFunctionMapper::getSingleton()
{
    static SerializeAnyFunctionMapper singleton;
    return singleton;
}

inline void SerializeAnyFunctionMapper::mapFunction(std::string className, std::function<void(SerializedStreamable&, DataFormatter&, const std::string&, std::any&)> serializeFunction, std::function<void(SerializedStreamable&, DataFormatter&, const std::string&, std::any&)> deserializeFunction)
{
    SerializeAnyFunctionMapper& singleton = getSingleton();
    singleton.functionMappings[className] = {serializeFunction, deserializeFunction};
}

inline std::pair<std::function<void(SerializedStreamable&, DataFormatter&, const std::string&, std::any&)>, std::function<void(SerializedStreamable&, DataFormatter&, const std::string&, std::any&)>> SerializeAnyFunctionMapper::getFunctions(std::string className)
{
    SerializeAnyFunctionMapper& singleton = getSingleton();
    return singleton.functionMappings[className];
}


void staticSerialize(SerializedStreamable& output, DataFormatter& formatter, const std::string& varName, std::any& var)
{
    //attempt to find a function for it in the mapper
    auto mappings = SerializeAnyFunctionMapper::getFunctions(var.type().name());
    if(mappings.first != nullptr)
        mappings.first(output, formatter, varName, var);
    else
        throw std::runtime_error("Serialization Failure of std::any. Unable to find mapping for " + std::string(var.type().name()));
}

void staticDeserialize(SerializedStreamable& input, DataFormatter& formatter, const std::string& varName, std::any& var)
{
    //attempt to find a function for it in the mapper
    auto mappings = SerializeAnyFunctionMapper::getFunctions(var.type().name());
    if(mappings.first != nullptr)
        mappings.second(input, formatter, varName, var);
    else
        throw std::runtime_error("Deserialization Failure of std::any. Unable to find mapping for " + std::string(var.type().name()));
}