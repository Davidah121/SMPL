#pragma once

#ifndef OPTI
#define OPTI 0
#endif


//Note that the following includes are due to intellisense not working properly.
//These will be included by immintrin.h if the proper preprocessors are defined. In
//the case that intellisense properly includes the necessary files, this should be
//moved above immintrin.h and the includes should be removed.

#if (OPTI>=1)

#include <immintrin.h>

#ifndef __SSE3__
    #define __SSE3__ 1
    #include <pmmintrin.h>
#endif

#ifndef __SSE4_2__
    #define __SSE4_2__ 1
    #include <smmintrin.h>
#endif

#endif

#if (OPTI>=2)

#ifndef __AVX__
    #define __AVX__ 1
    #include <avxintrin.h>
#endif

#ifndef __AVX2__
    #define __AVX2__ 1
    #include <avx2intrin.h>
#endif

#endif