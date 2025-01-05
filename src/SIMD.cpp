#include "SIMD.h"

#if (OPTI >= 1)
__m128i sse8HorizontalAdd(__m128i A, __m128i B)
{
    //shuffle then add
    const __m128i shuffle1 = _mm_set_epi8(14, 12, 10, 8, 6, 4, 2, 0, 14, 12, 10, 8, 6, 4, 2, 0);
    const __m128i shuffle2 = _mm_set_epi8(15, 13, 11, 9, 7, 5, 3, 1, 15, 13, 11, 9, 7, 5, 3, 1);
    __m128i temp = _mm_shuffle_epi8(A, shuffle1); //A0, A2, A4, A6, A8, A10, A12, A14, A0, A2, A4, A6, A8, A10, A12, A14
    __m128i temp2 = _mm_shuffle_epi8(B, shuffle2); //A1, A3, A5, A7, A9, A11, A13, A15, A1, A3, A5, A7, A9, A11, A13, A15
    return _mm_add_epi8(temp, temp2);
}
#endif