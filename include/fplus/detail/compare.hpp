// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <type_traits>

#include <fplus/detail/asserts/compare.hpp>

#include <fplus/detail/invoke.hpp>

namespace fplus
{
namespace detail
{
template <typename Compare>
auto ord_to_impl(Compare comp)
{
    return [comp](auto x, decltype(x) y)
    {
        using In = decltype(x);
        (void)detail::trigger_static_asserts<detail::ord_tag, Compare, In, In>();

        using CompareOut = std::decay_t<detail::invoke_result_t<Compare, In, In>>;
        static_assert(std::is_same<CompareOut, bool>::value,
                      "Function must return bool.");
        return std::make_pair(detail::invoke(comp, x, y),
                              detail::invoke(comp, y, x));
    };
}
}
}
