#pragma once
#include "PreprocessorTricks.h"
#include <unordered_map>
#include "DataFormatter.h"


//
class SerializedObject;
template<typename T>
struct SerializedVariable;

//EXCEPTION STUFF
class SimpleSerializationException : public std::runtime_error
{
public:
    SimpleSerializationException(std::string msg) : std::runtime_error(msg) {}
};

void staticSerialize(SerializedStreamable& output, DataFormatter& preprocessor);
void staticSerializeVar(SerializedStreamable& output, DataFormatter& preprocessor);
void staticDeserialize(SerializedStreamable& input, DataFormatter& preprocessor);
void staticDeserializeVar(SerializedStreamable& input, DataFormatter& preprocessor);


template<typename T, typename... Args>
void staticSerialize(SerializedStreamable& output, DataFormatter& preprocessor, T& var1, Args&... var2);
template<typename T>
void staticSerialize(SerializedStreamable& output, DataFormatter& preprocessor, T& var);

template<typename T, typename... Args>
void staticSerializeVar(SerializedStreamable& output, DataFormatter& preprocessor, const SerializedVariable<T>& var1, const Args&... var2);
template<typename T>
void staticSerializeVar(SerializedStreamable& output, DataFormatter& preprocessor, const SerializedVariable<T>& var);

template<typename T, typename... Args>
void staticDeserialize(SerializedStreamable& input, DataFormatter& preprocessor, T& var1, Args&... var2);
template<typename T>
void staticDeserialize(SerializedStreamable& input, DataFormatter& preprocessor, T& var1);

template<typename T, typename... Args>
void staticDeserializeVar(SerializedStreamable& output, DataFormatter& preprocessor, const SerializedVariable<T>& var1, const Args&... var2);
template<typename T>
void staticDeserializeVar(SerializedStreamable& output, DataFormatter& preprocessor, const SerializedVariable<T>& var);


//OVERLOAD THESE 2
template<typename T>
void staticSerializeHandler(SerializedStreamable& output, DataFormatter& preprocessor, const std::string varName, T& var);
template<typename T>
void staticDeserializeHandler(SerializedStreamable& input, DataFormatter& preprocessor, const std::string varName, T& var);

void mergeSerializedVariableMaps(std::string rootClassName, const std::unordered_map<std::string, SerializedVariable<void>>& rootMap, std::unordered_map<std::string, SerializedVariable<void>>& currMap);

template<typename T>
struct SerializedVariable final
{
    std::string name;
    T* data;
    TypeInfo type;
};

class SerializedObject
{
public:
    virtual const TypeInfo getClass() const = 0;
    virtual void serialize(SerializedStreamable& output, DataFormatter& formatter){} //for overriding
    virtual void deserialize(SerializedStreamable& input, DataFormatter& formatter){} //for overriding
    virtual std::unordered_map<std::string, SerializedVariable<void>> getSerializedVariables() { return {};}

protected:
    void superSerialize(SerializedStreamable& output, DataFormatter& formatter){}
    void superDeserialize(SerializedStreamable& input, DataFormatter& formatter){}
    void superGetSerializedVariables(std::unordered_map<std::string, SerializedVariable<void>>& thisClassVars){}
};

#ifndef SERIALIZE_VOID
    #define SERIALIZE_VOID(var) {#var, &var, TypeInfo::get<decltype(var)>()}
#endif
#ifndef SERIALIZE
    #define SERIALIZE(var) SerializedVariable<decltype(var)>SERIALIZE_VOID(var)
#endif
#ifndef SERIALIZE_MAP
	#define SERIALIZE_MAP(var) {#var, SERIALIZE_VOID(var)}
#endif

#ifndef SERIALIZE_CLASS_GETCLASS
    #define SERIALIZE_CLASS_GETCLASS()\
        virtual const TypeInfo getClass() const {return TypeInfo::get<std::remove_pointer<decltype(this)>::type>();}
#endif


#ifndef SERIALIZE_CLASS_SERIALIZE_VARIABLES
    #define SERIALIZE_CLASS_SERIALIZE_VARIABLES(...)\
        virtual std::unordered_map<std::string, SerializedVariable<void>> getSerializedVariables()\
        {\
            std::unordered_map<std::string, SerializedVariable<void>> currMap = {FOR_EACH_LIST(SERIALIZE_MAP, __VA_ARGS__)};\
            superGetSerializedVariables(currMap);\
            return currMap;\
        }
#endif

#ifndef ATTEMPT_SUPER_SERIALIZE_FUNCTIONS
    #define ATTEMPT_SUPER_SERIALIZE_FUNCTIONS()\
        private:\
        template<typename T, typename std::enable_if<std::is_base_of<SerializedObject, T>::value, int>::type = true>\
        void attemptSuperSerialize(SerializedStreamable& output, DataFormatter& formatter){T::serialize(output, formatter);}\
        template<typename T, typename std::enable_if<!std::is_base_of<SerializedObject, T>::value, int>::type = true>\
        void attemptSuperSerialize(SerializedStreamable& output, DataFormatter& formatter){staticSerialize(output, formatter, (T)(*this));}\
        \
        template<typename T, typename std::enable_if<std::is_base_of<SerializedObject, T>::value, int>::type = true>\
        void attemptSuperDeserialize(SerializedStreamable& input, DataFormatter& formatter){T::deserialize(input, formatter);}\
        template<typename T, typename std::enable_if<!std::is_base_of<SerializedObject, T>::value, int>::type = true>\
        void attemptSuperDeserialize(SerializedStreamable& input, DataFormatter& formatter){staticDeserialize(input, formatter, (T)(*this));}
#endif

#ifndef SERIALIZE_CLASS_SERIALIZE_FUNCTIONS
    #define SERIALIZE_HELPER_MACRO(...) staticSerializeVar(output, formatter, FOR_EACH_LIST(SERIALIZE, __VA_ARGS__));
    #define DESERIALIZE_HELPER_MACRO(...) staticDeserializeVar(input, formatter, FOR_EACH_LIST(SERIALIZE, __VA_ARGS__));
    #define SERIALIZE_CLASS_SERIALIZE_FUNCTIONS(...)\
        virtual void serialize(SerializedStreamable& output, DataFormatter& formatter)\
        { superSerialize(output, formatter); \
        EXECUTE_IF_NOT_EMPTY(SERIALIZE_HELPER_MACRO(__VA_ARGS__), __VA_ARGS__)\
        }\
        virtual void deserialize(SerializedStreamable& input, DataFormatter& formatter)\
        { superDeserialize(input, formatter); \
        EXECUTE_IF_NOT_EMPTY(DESERIALIZE_HELPER_MACRO(__VA_ARGS__), __VA_ARGS__)\
        }
#endif

    #define CALL_CLASS_SERIALIZE_HELPER(T) attemptSuperSerialize<T>(output, formatter);
    #define CALL_CLASS_DESERIALIZE_HELPER(T) attemptSuperDeserialize<T>(input, formatter);
    #define CALL_CLASS_SERIALIZED_VARIABLES_HELPER(T) mergeSerializedVariableMaps(#T, T::getSerializedVariables(), currMap);

    #define SERIALIZE_SUPER_SERIALIZE_FUNCTION(...)\
        virtual void superSerialize(SerializedStreamable& output, DataFormatter& formatter)\
        { FOR_EACH(CALL_CLASS_SERIALIZE_HELPER, __VA_ARGS__)}\
        virtual void superDeserialize(SerializedStreamable& input, DataFormatter& formatter)\
        { FOR_EACH(CALL_CLASS_DESERIALIZE_HELPER, __VA_ARGS__)}

    #define SERIALIZE_SUPER_SERIALIZE_VARIABLES(...)\
        virtual void superGetSerializedVariables(std::unordered_map<std::string, SerializedVariable<void>>& currMap)\
        { FOR_EACH(CALL_CLASS_SERIALIZED_VARIABLES_HELPER, __VA_ARGS__) }
    
    #define SERIALIZE_CLASS(...)\
    public:\
        SERIALIZE_CLASS_GETCLASS()\
        SERIALIZE_CLASS_SERIALIZE_FUNCTIONS(__VA_ARGS__)\
        SERIALIZE_CLASS_SERIALIZE_VARIABLES(__VA_ARGS__)
    
    #define SERIALIZE_SUPER_CLASS(...)\
    ATTEMPT_SUPER_SERIALIZE_FUNCTIONS()\
    protected:\
        SERIALIZE_SUPER_SERIALIZE_FUNCTION(__VA_ARGS__)\
        SERIALIZE_SUPER_SERIALIZE_VARIABLES(__VA_ARGS__)


inline void mergeSerializedVariableMaps(std::string rootClassName, const std::unordered_map<std::string, SerializedVariable<void>>& rootMap, std::unordered_map<std::string, SerializedVariable<void>>& currMap)
{
    for(std::pair<std::string, SerializedVariable<void>> entry : rootMap)
    {
        entry.first = rootClassName + "::" + entry.first;
        currMap.insert(entry);
    }
}


template<typename T, typename... Args>
void staticSerialize(SerializedStreamable& output, DataFormatter& formatter, T& var1, Args&... var2)
{
    //print something
    staticSerialize(output, formatter, var1);
    staticSerialize(output, formatter, var2...);
}

template<typename T>
void staticSerialize(SerializedStreamable& output, DataFormatter& formatter, T& var)
{
    //special clause for keeping things a bit simpler
    TypeInfo t = TypeInfo::get<T>();
    if(t.attributes[IS_SERIALIZABLE])
    {
        //MARK START
        formatter.writeStart(output, DataFormatter::FORMAT_OBJECT, t, "", 1); //no known name
        ((SerializedObject&)var).serialize(output, formatter);
    }
    else
    {
        //skip marking start for custom marking. NO VARIABLE NAME KNOWN
        staticSerializeHandler(output, formatter, "", var);
    }

    //MARK END
    formatter.writeEnd(output);
}

template<typename T, typename... Args>
void staticSerializeVar(SerializedStreamable& output, DataFormatter& formatter, const SerializedVariable<T>& var1, const Args&... var2)
{
    //print something
    staticSerializeVar(output, formatter, var1);
    staticSerializeVar(output, formatter, var2...);
}

template<typename T>
void staticSerializeVar(SerializedStreamable& output, DataFormatter& formatter, const SerializedVariable<T>& var)
{
    //special clause for keeping things a bit simpler
    if(var.type.attributes[IS_SERIALIZABLE])
    {
        //MARK START
        formatter.writeStart(output, DataFormatter::FORMAT_OBJECT, var.type, var.name, 1);
        ((SerializedObject*)var.data)->serialize(output, formatter);
    }
    else
    {
        //skip marking start to allow for custom marking
        staticSerializeHandler(output, formatter, var.name, *var.data);
    }

    //MARK END
    formatter.writeEnd(output);
}

template<typename T>
void staticSerializeHandler(SerializedStreamable& output, DataFormatter& formatter, const std::string varName, T& var)
{
    TypeInfo t = TypeInfo::get<T>();
    formatter.writeStart(output, DataFormatter::FORMAT_DATA, t, varName, 1);
    if(t.attributes[IS_FLOAT] || t.attributes[IS_INTEGER])
        formatter.writeNumber(output, t, (void*)&var);
    else
        formatter.writeRaw(output, t, (void*)&var);
}

template<typename T, typename... Args>
void staticDeserialize(SerializedStreamable& input, DataFormatter& formatter, T& var1, Args&... var2)
{
    //print something
    staticDeserialize(input, formatter, var1);
    staticDeserialize(input, formatter, var2...);
}
template<typename T>
void staticDeserialize(SerializedStreamable& input, DataFormatter& formatter, T& var)
{
    //special clause for keeping things a bit simpler
    TypeInfo t = TypeInfo::get<T>();
    if(t.attributes[IS_SERIALIZABLE])
    {
        //READ START
        int64_t elements = formatter.readStart(input, DataFormatter::FORMAT_OBJECT, t, "");
        if(elements >= 1)
            ((SerializedObject&)var).deserialize(input, formatter);
        
    }
    else
    {
        //skip marking start to allow custom marks
        staticDeserializeHandler(input, formatter, "", var);
    }
    formatter.readEnd(input);
}

template<typename T, typename... Args>
void staticDeserializeVar(SerializedStreamable& input, DataFormatter& formatter, const SerializedVariable<T>& var1, const Args&... var2)
{
    //print something
    staticDeserializeVar(input, formatter, var1);
    staticDeserializeVar(input, formatter, var2...);
}
template<typename T>
void staticDeserializeVar(SerializedStreamable& input, DataFormatter& formatter, const SerializedVariable<T>& var)
{
    //special clause for keeping things a bit simpler
    if(var.type.attributes[IS_SERIALIZABLE])
    {
        //MARK START
        int64_t elements = formatter.readStart(input, DataFormatter::FORMAT_OBJECT, var.type, var.name);
        if(elements >= 1)
            ((SerializedObject*)var.data)->deserialize(input, formatter);
    }
    else
    {
        //skip marking start to allow custom marks
        staticDeserializeHandler(input, formatter, var.name, *var.data);
    }

    //MARK END
    formatter.readEnd(input);
}

template<typename T>
void staticDeserializeHandler(SerializedStreamable& input, DataFormatter& formatter, const std::string varName, T& var)
{
    //MARK START
    TypeInfo t = TypeInfo::get<T>();
    int64_t elements = formatter.readStart(input, DataFormatter::FORMAT_DATA, t, varName);
    if(elements >= 1)
    {
        if(t.attributes[IS_FLOAT] || t.attributes[IS_INTEGER])
            formatter.readNumber(input, t, (void*)&var);
        else
            formatter.readRaw(input, t, (void*)&var);
    }
}

template<typename T>
bool constexpr isSerializable()
{
    return std::is_base_of<SerializedObject, T>();
}