// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <fplus/detail/function_traits_asserts.hpp>

namespace fplus
{
namespace detail
{

struct unary_function_tag
{
};

struct binary_function_tag
{
};

struct binary_predicate_tag
{
};

struct check_arity_tag
{
};

template <typename F, typename X>
struct function_traits_asserts<unary_function_tag, F, X>
{
    static_assert(utils::function_traits<F>::arity == 1,
                  "Function must take one parameter.");
    typedef typename utils::function_traits<F>::template arg<0>::type FIn0;
    static_assert(std::is_convertible<X, FIn0>::value,
                  "Invalid argument type for function");
};

template <typename F>
struct function_traits_asserts<unary_function_tag, F>
{
    static_assert(utils::function_traits<F>::arity == 1,
        "Function must take one parameter.");
};

template <typename F>
struct function_traits_asserts<binary_function_tag, F>
{
    static_assert(utils::function_traits<F>::arity == 2,
        "Function must take two parameters.");
};

template <typename F>
struct function_traits_asserts<binary_predicate_tag, F>
{
    static_assert(utils::function_traits<F>::arity == 2,
        "Function must take two parameters.");
    typedef typename utils::function_traits<F>::template arg<0>::type FIn0;
    typedef typename utils::function_traits<F>::template arg<1>::type FIn1;
    static_assert(std::is_same<FIn0, FIn1>::value,
                  "Both parameters must have the same type.");
};

template <typename F, typename... Args>
struct function_traits_asserts<check_arity_tag, F, Args...>
{
    static_assert(utils::function_traits<F>::arity == sizeof...(Args),
                  "Wrong arity.");
};

}
}
