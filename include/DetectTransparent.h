#include <type_traits>

//Thanks to: https://fekir.info/post/detect-member-variables/
//			 https://fekir.info/post/detect-transparent-comparator/
#if (__cplusplus < 201703L)
	template<class>
	struct my_void_t { using type = void; };
	#define VOID_STRUCT_TYPE my_void_t
#else
	#define VOID_STRUCT_TYPE std::void_t
#endif


template<typename T, typename = void>
struct is_transparent : std::false_type{};

template<typename T>
struct is_transparent<T, VOID_STRUCT_TYPE<typename T::is_transparent>> : std::true_type{};

template<typename Type1, typename Type2>
struct both_transparent : std::conditional<is_transparent<Type1>::value && is_transparent<Type2>::value, std::true_type, std::false_type>::type{};
