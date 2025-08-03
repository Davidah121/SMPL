#pragma once
#include "TargetVer.h"
#ifndef DLL_OPTION
    #if defined(DLL_EXPORT)
        #if defined(_MSC_VER)
            #define DLL_OPTION __declspec("dllexport")
        #else
            #define DLL_OPTION __attribute__((visibility("default")))
        #endif
    #elif defined(DLL_IMPORT)
        #if defined(_MSC_VER)
            #define DLL_OPTION __declspec("dllimport")
        #else
            #define DLL_OPTION
        #endif
    #else
        #define DLL_OPTION
    #endif
#endif

#ifndef _CRT_SECURE_NO_WARNINGS
    #define _CRT_SECURE_NO_WARNINGS 1
#endif
