// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <fplus/detail/invoke.hpp>
#include <fplus/function_traits.hpp>

namespace fplus
{
namespace detail
{
template <typename T, typename...>
struct function_traits_asserts;

template <
    typename,
    typename F,
    typename... Args,
    typename std::enable_if<is_invocable<F, Args...>::value, int>::type = 0>
constexpr bool trigger_static_asserts()
{
  return true;
}

template <typename Tag,
          typename F,
          typename... Args,
          typename std::enable_if<has_function_traits<F>::value &&
                                      !is_invocable<F, Args...>::value,
                                  int>::type = 0>
constexpr bool trigger_static_asserts()
{
  // don't perform checks if function_traits<F> doesn't exist
  (void)function_traits_asserts<Tag, F, Args...>{};
  return true;
}

template <typename,
          typename F,
          typename... Args,
          typename std::enable_if<!has_function_traits<F>::value &&
                                      !is_invocable<F, Args...>::value,
                                  int>::type = 0>
constexpr bool trigger_static_asserts()
{
  static_assert(sizeof(F) == 0,
                "F is not a Callable, or its definition is ill-formed");
  return true;
}
}
}
