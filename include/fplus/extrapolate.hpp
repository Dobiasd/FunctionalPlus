// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "fplus/container_common.hpp"
#include "fplus/maybe.hpp"

namespace fplus
{

// API search type: elem_at_idx_or_reflect101 : (Int, [a]) -> Maybe a
template <typename Container,
    typename T = typename Container::value_type>
maybe<T> elem_at_idx_or_nothing(signed int idx, const Container& xs)
{
    if (idx < 0 || idx >= static_cast<signed int>(size_of_cont(xs)))
    {
        return {};
    }
    auto it = std::begin(xs);
    std::advance(it, idx);
    return *it;
}

// API search type: elem_at_idx_or_constant : (a, Int, [a]) -> a
// iiiiii|abcdefgh|iiiiiii
template <typename Container,
    typename T = typename Container::value_type>
T elem_at_idx_or_constant(const T& c, signed int idx, const Container& xs)
{
    if (idx < 0 || idx >= static_cast<signed int>(size_of_cont(xs)))
    {
        return c;
    }
    auto it = std::begin(xs);
    std::advance(it, idx);
    return *it;
}

// API search type: elem_at_idx_or_replicate : (Int, [a]) -> a
// aaaaaa|abcdefgh|hhhhhhh
// xs must not be empty.
template <typename Container,
    typename T = typename Container::value_type>
T elem_at_idx_or_replicate(signed int idx, const Container& xs)
{
    assert(is_not_empty(xs));
    if (idx < 0)
    {
        return xs.front();
    }
    if (idx >= static_cast<signed int>(size_of_cont(xs)))
    {
        return xs.back();
    }
    auto it = std::begin(xs);
    std::advance(it, idx);
    return *it;
}

// API search type: elem_at_idx_or_wrap : (Int, [a]) -> a
// For cyclic element access
// cdefgh|abcdefgh|abcdefg
// xs must not be empty.
template <typename Container,
    typename T = typename Container::value_type>
T elem_at_idx_or_wrap(signed int idx, const Container& xs)
{
    assert(is_not_empty(xs));
    const signed int cont_size = static_cast<signed int>(size_of_cont(xs));
    if (idx < 0)
        idx = cont_size - (std::abs(idx) % cont_size);
    else
        idx = idx % cont_size;
    auto it = std::begin(xs);
    std::advance(it, idx);
    return *it;
}

// API search type: extrapolate_replicate : (Int, Int, [a]) -> [a]
// aaaaaa|abcdefgh|hhhhhhh
// xs must not be empty.
template <typename Container,
    typename T = typename Container::value_type>
Container extrapolate_replicate(std::size_t count_begin, std::size_t count_end,
        const Container& xs)
{
    assert(is_not_empty(xs));
    Container ys;
    const auto xs_size = size_of_cont(xs);
    internal::prepare_container(ys, xs_size + count_begin + count_end);
    auto it = internal::get_back_inserter<Container>(ys);
    const signed int idx_end = static_cast<signed int>(xs_size + count_end);
    const signed int idx_start = -static_cast<signed int>(count_begin);
    for (signed int idx = idx_start; idx < idx_end; ++idx)
    {
        *it = elem_at_idx_or_replicate(idx, xs);
    }
    return ys;
}

// API search type: extrapolate_wrap : (Int, Int, [a]) -> [a]
// cdefgh|abcdefgh|abcdefg
// xs must not be empty.
template <typename Container,
    typename T = typename Container::value_type>
Container extrapolate_wrap(std::size_t count_begin, std::size_t count_end,
        const Container& xs)
{
    assert(is_not_empty(xs));
    Container ys;
    const auto xs_size = size_of_cont(xs);
    internal::prepare_container(ys, xs_size + count_begin + count_end);
    auto it = internal::get_back_inserter<Container>(ys);
    const signed int idx_end = static_cast<signed int>(xs_size + count_end);
    const signed int idx_start = -static_cast<signed int>(count_begin);
    for (signed int idx = idx_start; idx < idx_end; ++idx)
    {
        *it = elem_at_idx_or_wrap(idx, xs);
    }
    return ys;
}

} // namespace fplus
