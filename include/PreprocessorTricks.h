#pragma once

/*
    References:
        https://github.com/pfultz2/Cloak/wiki/C-Preprocessor-tricks,-tips,-and-idioms
        https://www.scs.stanford.edu/~dm/blog/va-opt.html
    
    Note that EVAL is reduced compared to common references. allows ~65 expansions which is more than enough.
    C++20 is required to use __VA_OPT__ but some compilers may support it on earlier versions of C++
*/
#ifndef USE_PREPROCESSOR_TRICKS
    #define USE_PREPROCESSOR_TRICKS 0
#endif

#if USE_PREPROCESSOR_TRICKS

    #define PASSTHROUGH_FUNCTION(x) x
    #define PARENS ()
    #define COMMA ,
    #define EVAL(...)  EVAL1(EVAL1(__VA_ARGS__))
    #define EVAL1(...) EVAL2(EVAL2(__VA_ARGS__))
    #define EVAL2(...) EVAL3(EVAL3(__VA_ARGS__))
    #define EVAL3(...) EVAL4(EVAL4(__VA_ARGS__))
    #define EVAL4(...) EVAL5(EVAL5(__VA_ARGS__))
    #define EVAL5(...) __VA_ARGS__

    #if __cplusplus <= 201703L
        #define USING_NEW_PREPROCESSOR_METHOD 0
        #define CAT(a, ...) PRIMITIVE_CAT(a, __VA_ARGS__)
        #define PRIMITIVE_CAT(a, ...) a ## __VA_ARGS__

        #define IIF(c) PRIMITIVE_CAT(IIF_, c)
        #define IIF_0(t, ...) __VA_ARGS__
        #define IIF_1(t, ...) t

        #define COMPLEMENT(b) PRIMITIVE_CAT(COMPLEMENT_, b)
        #define COMPLEMENT_0 1
        #define COMPLEMENT_1 0

        #define CHECK_N(x, n, ...) n
        #define CHECK(...) CHECK_N(__VA_ARGS__, 0,)
        #define PROBE(x) x, 1,

        #define IS_EMPTY(x, ...) CHECK(IS_EMPTY_PROBE x ())
        #define IS_EMPTY_PROBE(...) PROBE(~)

        #define NOT(x) CHECK(PRIMITIVE_CAT(NOT_, x))
        #define NOT_0 PROBE(~)

        #define BOOL_FUNCTION(x) COMPLEMENT(NOT(x))
        #define IF(c) IIF(BOOL_FUNCTION(c))

        #define EMPTY()
        #define DEFER(x) x EMPTY()
        #define OBSTRUCT(...) __VA_ARGS__ DEFER(EMPTY)()
        #define EXPAND(...) __VA_ARGS__

        #define EXECUTE_IF_NOT_EMPTY(macro, ...)\
        IF(NOT(IS_EMPTY(__VA_ARGS__)))(macro)
        

        #define FOR_EACH_HELPER(macro, x, ...)\
            macro(x)\
            IF(NOT(IS_EMPTY(__VA_ARGS__)))\
            (\
                OBSTRUCT(FOR_EACH_INDIRECT) () (macro, __VA_ARGS__)\
            )
        #define FOR_EACH(macro, ...)\
            IF(NOT(IS_EMPTY(__VA_ARGS__)))\
            (\
                EVAL(FOR_EACH_HELPER(macro, __VA_ARGS__))\
            )
        #define FOR_EACH_INDIRECT() FOR_EACH_HELPER

        #define FOR_EACH_FUNCTION_HELPER(macro, x, ...)\
            macro(x);\
            IF(NOT(IS_EMPTY(__VA_ARGS__)))\
            (\
                OBSTRUCT(FOR_EACH_FUNCTION_INDIRECT) () (macro, __VA_ARGS__)\
            )
        #define FOR_EACH_FUNCTION(macro, ...)\
            IF(NOT(IS_EMPTY(__VA_ARGS__)))\
            (\
                EVAL(FOR_EACH_FUNCTION_HELPER(macro, __VA_ARGS__))\
            )
        #define FOR_EACH_FUNCTION_INDIRECT() FOR_EACH_FUNCTION_HELPER

        #define FOR_EACH_LIST_HELPER(macro, x, ...)\
            macro(x)\
            IF(NOT(IS_EMPTY(__VA_ARGS__)))\
            (\
                OBSTRUCT(FOR_EACH_LIST_INDIRECT) () (macro, __VA_ARGS__)\
            )
        #define FOR_EACH_LIST(macro, ...)\
            IF(NOT(IS_EMPTY(__VA_ARGS__)))\
            (\
                EVAL(FOR_EACH_LIST_HELPER(macro, __VA_ARGS__))\
            )
        #define FOR_EACH_LIST_INDIRECT() , FOR_EACH_LIST_HELPER
    #else
        #define USING_NEW_PREPROCESSOR_METHOD 1
        #define EXECUTE_IF_NOT_EMPTY(macro, ...)\
        __VA_OPT__(macro)

        #define FOR_EACH_FUNCTION(macro, ...)                                    \
        __VA_OPT__(EVAL(FOR_EACH_FUNCTION_HELPER(macro, __VA_ARGS__)))
        #define FOR_EACH_FUNCTION_HELPER(macro, a1, ...)                         \
        macro(a1);                                                     \
        __VA_OPT__(FOR_EACH_FUNCTION_AGAIN PARENS (macro, __VA_ARGS__))
        #define FOR_EACH_FUNCTION_AGAIN() FOR_EACH_FUNCTION_HELPER

        #define FOR_EACH_LIST(macro, ...)                                    \
        __VA_OPT__(EVAL(FOR_EACH_LIST_HELPER(macro, __VA_ARGS__)))
        #define FOR_EACH_LIST_HELPER(macro, a1, ...)                         \
        macro(a1)                                                     \
        __VA_OPT__(, FOR_EACH_LIST_AGAIN PARENS (macro, __VA_ARGS__))
        #define FOR_EACH_LIST_AGAIN() FOR_EACH_LIST_HELPER

        #define FOR_EACH(macro, ...)                                    \
        __VA_OPT__(EVAL(FOR_EACH_HELPER(macro, __VA_ARGS__)))
        #define FOR_EACH_HELPER(macro, a1, ...)                         \
        macro(a1)                                                     \
        __VA_OPT__(FOR_EACH_AGAIN PARENS (macro, __VA_ARGS__))
        #define FOR_EACH_AGAIN() FOR_EACH_HELPER
    #endif
#else
    #define EXECUTE_IF_NOT_EMPTY(macro, ...)
    #define FOR_EACH_FUNCTION(macro, ...)
    #define FOR_EACH_LIST(macro, ...)
    #define FOR_EACH(macro, ...)
#endif