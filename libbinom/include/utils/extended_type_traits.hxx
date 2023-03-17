#ifndef EXTENDED_TYPE_TRAITS_HXX
#define EXTENDED_TYPE_TRAITS_HXX

#include <type_traits>
#include <concepts>

//! Extendet type traits and concepts
namespace extended_type_traits {

template <typename T>
concept is_char_v =
    std::same_as<T, char> ||
    std::same_as<T, signed char> ||
    std::same_as<T, unsigned char> ||
    std::same_as<T, wchar_t> ||
    std::same_as<T, char8_t> ||
    std::same_as<T, char16_t> ||
    std::same_as<T, char32_t>;

template <class T>
struct remove_cvref : std::remove_cv<std::remove_reference_t<T>> {};

template <class T>
using remove_cvref_t = typename remove_cvref<T>::type;

template <class T, class U>
struct is_base_of : std::is_base_of<remove_cvref_t<T>, remove_cvref_t<U>> {};

template <class T, class U>
concept is_base_of_v = is_base_of<T, U>::value;

template <template<typename T> class CRTP_Base, class CRTP_Driven>
struct is_crtp_base_of : std::is_base_of<remove_cvref_t<CRTP_Base<remove_cvref_t<CRTP_Driven>>>, remove_cvref_t<CRTP_Driven>> {};

template <template<typename T> class CRTP_Base, class CRTP_Driven>
concept is_crtp_base_of_v = is_crtp_base_of<CRTP_Base, CRTP_Driven>::value;

template <class T>
struct is_integral_without_cvref : std::is_integral<remove_cvref_t<T>> {};

template <class T>
concept is_integral_without_cvref_v = is_integral_without_cvref<T>::value;

template <class T>
struct is_floating_point_without_cvref : std::is_floating_point<remove_cvref_t<T>> {};

template <class T>
concept is_floating_point_without_cvref_v = is_floating_point_without_cvref<T>::value;

template <class T>
struct is_arithmetic_without_cvref : std::is_arithmetic<remove_cvref_t<T>> {};

template <class T>
concept is_arithmetic_without_cvref_v = is_arithmetic_without_cvref<T>::value;

template <class T>
struct is_signed_without_cvref : std::is_signed<remove_cvref_t<T>> {};

template <class T>
concept is_signed_without_cvref_v = is_signed_without_cvref<T>::value;

template <class T>
struct is_unsigned_without_cvref : std::is_unsigned<remove_cvref_t<T>> {};

template <class T>
concept is_unsigned_without_cvref_v = is_unsigned_without_cvref<T>::value;

template <class T>
struct is_float_without_cvref : std::is_floating_point<remove_cvref_t<T>> {};

template <class T>
concept is_float_without_cvref_v = is_float_without_cvref<T>::value;

template <class T, class U>
struct is_same_without_cvref : std::is_same<remove_cvref_t<T>, remove_cvref_t<U>> {};

template <class T, class U>
concept is_same_without_cvref_v = is_same_without_cvref<T, U>::value;

template<template<typename...> class, typename...>
struct is_instantiation : public std::false_type {};

template<template<typename...> class U, typename... T>
struct is_instantiation<U, U<T...>> : public std::true_type {};

template<template<typename...> class U, typename... T>
concept is_instantiation_v = is_instantiation<U, T...>::value;

template<template<typename...> class U, typename... T>
struct is_instantiation_without_cvref : is_instantiation<U, remove_cvref_t<T>...> {};

template<template<typename...> class U, typename... T>
concept is_instantiation_without_cvref_v = is_instantiation_without_cvref<U, T...>::value;

template<typename T, typename ... U>
concept is_any_of = (std::same_as<T, U> || ...);

//typename iterator_traits<_Iter>::iterator_category

}

#endif // EXTENDED_TYPE_TRAITS_HXX
