#include <bitset>
#include <typeinfo>
#include <string>
#if __cplusplus >= 201703L
#include <string_view>
#define CONSTEXPR_OPTION constexpr
#else
#define CONSTEXPR_OPTION 
#endif

#ifdef _unix_
#include <cstdlib>
#include <cxxabi.h>
#endif

std::string demangleClassName(std::string name);

template<typename T>
bool constexpr isSerializable();

enum OPTIONS
{
    IS_INTEGER,
    IS_FLOAT,
    IS_SIGNED,
    IS_TRIVIAL,
    IS_POINTER,
    IS_SERIALIZABLE,
    IS_VOID,
    IS_TEMPLATE
};

struct TypeInfo final
{
    
    #if __cplusplus >= 201703L
    std::string_view name;
    #else
    std::string name;
    #endif
    uint32_t size;
    uint64_t hash;
    std::bitset<8> attributes;

    bool operator==(const TypeInfo& t) const
    {
        return hash == t.hash;
    }

    std::string getName() const
    {
        #ifdef _unix_
            return demangleClassName((std::string)name);
        #else
            return (std::string)name;
        #endif
    }
    
    template <typename T>
    static CONSTEXPR_OPTION TypeInfo get();
};


template<typename T>
int constexpr safeSizeOf()
{
    return sizeof(T);
}

template<>
int constexpr safeSizeOf<void>()
{
    return 0;
}

template<typename T>
inline CONSTEXPR_OPTION TypeInfo TypeInfo::get()
{
    TypeInfo info;
    info.name = typeid(T).name();
    info.size = safeSizeOf<T>();
    info.hash = typeid(T).hash_code();
    info.attributes[IS_INTEGER] = std::is_integral<T>();
    info.attributes[IS_FLOAT] = std::is_floating_point<T>();
    info.attributes[IS_SIGNED] = std::is_signed<T>();
    info.attributes[IS_TRIVIAL] = std::is_trivially_copyable<T>();
    info.attributes[IS_POINTER] = std::is_pointer<T>();
    info.attributes[IS_SERIALIZABLE] = isSerializable<T>();
    info.attributes[IS_VOID] = std::is_void<T>();
    info.attributes[IS_TEMPLATE] = info.name.find('<') != SIZE_MAX;
    return info;
}
