// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <fplus/internal/invoke.hpp>
#include <fplus/function_traits.hpp>

namespace fplus
{
namespace internal
{
template <typename T, typename...>
struct function_traits_asserts;

template <
    typename,
    typename F,
    typename... Args,
    typename std::enable_if<is_invocable<F, Args...>::value, int>::type = 0>
constexpr void trigger_static_asserts()
{
}

// Marks a variable as unused. Prevents the compiler warning
// for set but unused variables.
template<class T>
inline void unused(T&&) { }

template <typename Tag,
          typename F,
          typename... Args,
          typename std::enable_if<has_function_traits<F>::value &&
                                      !is_invocable<F, Args...>::value,
                                  int>::type = 0>
constexpr void trigger_static_asserts()
{
    // don't perform checks if function_traits<F> doesn't exist
    unused(function_traits_asserts<Tag, F, Args...>{});
}

template <typename,
          typename F,
          typename... Args,
          typename std::enable_if<!has_function_traits<F>::value &&
                                      !is_invocable<F, Args...>::value,
                                  int>::type = 0>
constexpr void trigger_static_asserts()
{
  static_assert(sizeof(F) == 0,
                "F is not a Callable, or its definition is ill-formed");
}
}
}
