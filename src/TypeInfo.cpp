#include "TypeInfo.h"
#include <cxxabi.h>

std::string demangleClassName(std::string name)
{
    // #ifdef _unix_
    std::string finalName = "";
    int status = 0;
    char* realName = abi::__cxa_demangle(name.c_str(), 0, 0, &status);
    finalName = realName;
    std::free(realName);
    return finalName;
    // #endif
    // return name;
}