// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <fplus/detail/function_traits_asserts.hpp>
#include <fplus/detail/invoke.hpp>

namespace fplus
{
namespace detail
{
struct zip_with_tag
{
};

template <typename F, typename X, typename Y>
struct function_traits_asserts<zip_with_tag, F, X, Y>
{
    static_assert(utils::function_traits<F>::arity == 2,
        "Function must take two parameters.");
    typedef typename utils::function_traits<F>::template arg<0>::type FIn0;
    typedef typename utils::function_traits<F>::template arg<1>::type FIn1;
    static_assert(std::is_convertible<X, FIn0>::value,
        "Function does not take elements from first Container as first Parameter.");
    static_assert(std::is_convertible<Y, FIn1>::value,
        "Function does not take elements from second Container as second Parameter.");
};
}
}
