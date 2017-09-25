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
struct transform_and_keep_justs_tag
{
};

struct transform_and_keep_oks_tag
{
};

struct apply_functions_tag
{
};

template <typename F, typename X>
struct function_traits_asserts<transform_and_keep_justs_tag, F, X> 
{
    static_assert(utils::function_traits<F>::arity == 1,
                  "Function must take one parameter");
    typedef typename utils::function_traits<F>::template arg<0>::type FIn0;
    static_assert(std::is_convertible<X, FIn0>::value,
                  "Invalid parameter type.");
};

template <typename F, typename X>
struct function_traits_asserts<transform_and_keep_oks_tag, F, X> 
{
    static_assert(utils::function_traits<F>::arity == 1,
                  "Function must take one parameter");
    typedef typename utils::function_traits<F>::template arg<0>::type FIn0;
    static_assert(std::is_convertible<X, FIn0>::value,
                  "Invalid parameter type.");
};

template <typename F, typename X>
struct function_traits_asserts<apply_functions_tag, F, X>
{
    static_assert(utils::function_traits<F>::arity == 1,
                  "Function must take one parameter");
    typedef typename utils::function_traits<F>::template arg<0>::type FIn0;
    static_assert(std::is_convertible<X, FIn0>::value,
                  "Invalid parameter type.");
};
}
}

