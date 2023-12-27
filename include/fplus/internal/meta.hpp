// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <type_traits>

namespace fplus
{
namespace internal
{
// C++14 compatible void_t (http://en.cppreference.com/w/cpp/types/void_t)
template <typename... Ts>
struct make_void
{
  using type = void;
};

template <typename... Ts>
using void_t = typename make_void<Ts...>::type;

// Sometimes you don't want to use std::decay_t, and the temptation of short
// writing can be huge...
template <typename T>
using uncvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

// disjunction/conjunction/negation, useful to short circuit SFINAE checks
// Use with parsimony, MSVC 2015 can have ICEs quite easily
template <typename...>
struct disjunction : std::false_type
{
};

template <typename B1>
struct disjunction<B1> : B1
{
};

template <typename B1, typename... Bn>
struct disjunction<B1, Bn...>
    : std::conditional<bool(B1::value), B1, disjunction<Bn...>>::type
{
};

template <typename...>
struct conjunction : std::true_type
{
};

template <typename B1>
struct conjunction<B1> : B1
{
};

template <typename B1, typename... Bn>
struct conjunction<B1, Bn...>
    : std::conditional<bool(B1::value), conjunction<Bn...>, B1>::type
{
};

template <typename B>
struct negation : std::integral_constant<bool, !bool(B::value)>
{
};

// non short-circuiting meta functions
// source: https://stackoverflow.com/a/27221517/4116453
template <bool...>
struct bool_pack;

template <bool... Values>
struct all_of
    : std::is_same<bool_pack<Values..., true>, bool_pack<true, Values...>>
{
};

// there seems to be a bug in libc++'s std::is_function
// provide our own (cppreference one)
// (the MSVC implementation seems correct)
#ifndef _MSC_VER
#define PROVIDE_IS_FUNCTION_POLYFILL
#endif

#ifndef PROVIDE_IS_FUNCTION_POLYFILL
template<class... Any>
using is_function = std::is_function<Any...>;
#else //PROVIDE_IS_FUNCTION_POLYFILL
// primary template
template<class>
struct is_function : std::false_type { };

// specialization for regular functions
template<class Ret, class... Args>
struct is_function<Ret(Args...)> : std::true_type {};

// specialization for variadic functions such as std::printf
template<class Ret, class... Args>
struct is_function<Ret(Args...,...)> : std::true_type {};

// specialization for function types that have cv-qualifiers
template<class Ret, class... Args>
struct is_function<Ret(Args...) const> : std::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...) volatile> : std::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...) const volatile> : std::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...,...) const> : std::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...,...) volatile> : std::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...,...) const volatile> : std::true_type {};

// specialization for function types that have ref-qualifiers
template<class Ret, class... Args>
struct is_function<Ret(Args...) &> : std::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...) const &> : std::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...) volatile &> : std::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...) const volatile &> : std::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...,...) &> : std::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...,...) const &> : std::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...,...) volatile &> : std::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...,...) const volatile &> : std::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...) &&> : std::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...) const &&> : std::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...) volatile &&> : std::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...) const volatile &&> : std::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...,...) &&> : std::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...,...) const &&> : std::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...,...) volatile &&> : std::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...,...) const volatile &&> : std::true_type {};
#endif //PROVIDE_IS_FUNCTION_POLYFILL

template <typename>
struct reverse_integer_sequence_impl;

template <typename T>
struct reverse_integer_sequence_impl<std::integer_sequence<T>>
    : std::integer_sequence<T>
{
};

template <typename T, T... Ints>
struct reverse_integer_sequence_impl<std::integer_sequence<T, Ints...>>
    : std::integer_sequence<T, sizeof...(Ints) - 1 - Ints...>
{
};

template <typename Seq>
using reverse_integer_sequence = reverse_integer_sequence_impl<Seq>;

template <typename T, T N>
using make_reverse_integer_sequence =
    reverse_integer_sequence<std::make_integer_sequence<T, N>>;

template <std::size_t... Idx>
using reverse_index_sequence =
    reverse_integer_sequence<std::index_sequence<Idx...>>;

template <std::size_t N>
using make_reverse_index_sequence =
    make_reverse_integer_sequence<std::size_t, N>;
}
}
