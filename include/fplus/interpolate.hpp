// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <fplus/container_common.hpp>
#include <fplus/maybe.hpp>

namespace fplus
{

// API search type: elem_at_float_idx : (Float, [a]) -> a
// fwd bind count: 1
// Interpolates linearly between elements.
// xs must be non-empty.
template <typename Container,
    typename T = typename Container::value_type>
T elem_at_float_idx(double idx, const Container& xs)
{
    assert(is_not_empty(xs));
    if (idx <= 0.0)
    {
        return xs.front();
    }
    std::size_t idx_floor = floor<double, std::size_t>(idx);
    std::size_t idx_ceil = ceil<double, std::size_t>(idx);
    if (idx_ceil >= size_of_cont(xs))
    {
        return xs.back();
    }
    double idx_floor_float = static_cast<double>(idx_floor);
    double idx_ceil_float = static_cast<double>(idx_ceil);
    double weight_floor = idx_ceil_float - idx;
    double weight_ceil = idx - idx_floor_float;
    return
        (weight_floor * elem_at_idx(idx_floor, xs) +
            weight_ceil * elem_at_idx(idx_ceil, xs));
}

} // namespace fplus
