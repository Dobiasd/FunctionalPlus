// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <type_traits>

#include <fplus/internal/asserts/functions.hpp>
#include <fplus/internal/invoke.hpp>

namespace fplus
{
namespace internal
{
template <typename Compare>
auto ord_to_impl(Compare comp)
{
    return [comp](auto x, auto y)
    {
        static_assert(std::is_same<decltype(x), decltype(y)>::value,
            "Argument types must be the same");
        using In = decltype(x);
        internal::trigger_static_asserts<internal::binary_predicate_tag, Compare, In, In>();

        using CompareOut = std::decay_t<internal::invoke_result_t<Compare, In, In>>;
        static_assert(std::is_same<CompareOut, bool>::value,
                      "Function must return bool.");
        return std::make_pair(internal::invoke(comp, x, y),
                              internal::invoke(comp, y, x));
    };
}
}
}
