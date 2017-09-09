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
struct lift_maybe_tag
{
};

template <typename F, typename ...Args>
struct function_traits_asserts<lift_maybe_tag, F, Args...>
{
    static_assert(utils::function_traits<F>::arity == sizeof...(Args),
                  "Wrong arity");
};
}
}
