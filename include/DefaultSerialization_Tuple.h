#pragma once
#include "SimpleSerialization.h"
#include <tuple>

template<size_t I = 0, typename... Types>
typename std::enable_if<I == sizeof...(Types), void>::type
staticSerializeHandlerTemp(SerializedStreamable& output, DataFormatter& formatter, std::tuple<Types...>& var)
{
}

template<size_t I = 0, typename... Types>
typename std::enable_if<I < sizeof...(Types), void>::type
staticSerializeHandlerTemp(SerializedStreamable& output, DataFormatter& formatter, std::tuple<Types...>& var)
{
    staticSerialize(output, formatter, std::get<I>(var));
    staticSerializeHandlerTemp<I+1, Types...>(output, formatter, var);
}


template<size_t I = 0, typename... Types>
typename std::enable_if<I < sizeof...(Types), void>::type
staticSerializeHandler(SerializedStreamable& output, DataFormatter& formatter, const std::string& varName, std::tuple<Types...>& var)
{
    formatter.writeStart(output, DataFormatter::FORMAT_OBJECT, TypeInfo::get<std::tuple<Types...>>(), varName, 1);
    staticSerializeHandlerTemp(output, formatter, var);
}

template<size_t I = 0, typename... Types>
typename std::enable_if<I == sizeof...(Types), void>::type
staticDeserializeHandlerTemp(SerializedStreamable& input, DataFormatter& formatter, std::tuple<Types...>& var)
{
}

template<size_t I = 0, typename... Types>
typename std::enable_if<I < sizeof...(Types), void>::type
staticDeserializeHandlerTemp(SerializedStreamable& input, DataFormatter& formatter, std::tuple<Types...>& var)
{
    staticDeserialize(input, formatter, std::get<I>(var));
    staticDeserializeHandlerTemp<I+1, Types...>(input, formatter, var);
}

template<size_t I = 0, typename... Types>
typename std::enable_if<I < sizeof...(Types), void>::type
staticDeserializeHandler(SerializedStreamable& input, DataFormatter& formatter, const std::string& varName, std::tuple<Types...>& var)
{
    int64_t elements = formatter.readStart(input, DataFormatter::FORMAT_OBJECT, TypeInfo::get<std::tuple<Types...>>(), varName);
    if(elements >= 1)
        staticDeserializeHandlerTemp(input, formatter, var);
}