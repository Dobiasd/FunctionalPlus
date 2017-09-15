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
struct compose_tag
{
};

struct bind_1st_of_2_tag
{
};

struct logical_unary_op_tag
{
};

template <typename F>
struct function_traits_asserts<compose_tag, F>
{
    static_assert(utils::function_traits<F>::arity == 1,
        "Function must take one parameter.");
};

template <typename F, typename X, typename Y>
struct function_traits_asserts<bind_1st_of_2_tag, F, X, Y>
{
    static_assert(utils::function_traits<F>::arity == 2,
                  "Function must take two parameters.");
    typedef typename utils::function_traits<F>::template arg<0>::type FIn0;
    typedef typename utils::function_traits<F>::template arg<1>::type FIn1;
    static_assert(std::is_convertible<X, FIn0>::value,
                  "Function can not take bound parameter type");
    static_assert(std::is_convertible<Y, FIn1>::value,
                  "Function can not take provided parameter type");
};

template <typename F, typename X>
struct function_traits_asserts<logical_unary_op_tag, F, X>
{
    static_assert(utils::function_traits<F>::arity == 1,
                  "Function must take one parameter.");
};
}
}
