#include "SIMD.h"

#ifdef __SSE4_2__
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


#ifdef __AVX2__
__m256i sse8HorizontalAdd(__m256i A, __m256i B)
{
    //shuffle then add
    const __m256i shuffle1 = _mm256_set_epi8(30, 28, 26, 24, 22, 20, 18, 16, 14, 12, 10, 8, 6, 4, 2, 0, 30, 28, 26, 24, 22, 20, 18, 16, 14, 12, 10, 8, 6, 4, 2, 0);
    const __m256i shuffle2 = _mm256_set_epi8(31, 29, 27, 25, 23, 21, 19, 17, 15, 13, 11, 9, 7, 5, 3, 1, 31, 29, 27, 25, 23, 21, 19, 17, 15, 13, 11, 9, 7, 5, 3, 1);
    __m256i temp = _mm256_shuffle_epi8(A, shuffle1); //A0, A2, A4, A6, A8, A10, A12, A14, A0, A2, A4, A6, A8, A10, A12, A14
    __m256i temp2 = _mm256_shuffle_epi8(B, shuffle2); //A1, A3, A5, A7, A9, A11, A13, A15, A1, A3, A5, A7, A9, A11, A13, A15
    return _mm256_add_epi8(temp, temp2);
}
#endif