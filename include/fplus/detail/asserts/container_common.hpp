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
struct transform_tag
{
};

template <typename F, typename T>
struct function_traits_asserts<transform_tag, F, T>
{
    static_assert(utils::function_traits<F>::arity == 1,
                  "Function must take one parameter.");
};
}
}
