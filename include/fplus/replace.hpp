// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "fplus/container_common.hpp"
#include "fplus/compare.hpp"
#include "fplus/split.hpp"

namespace fplus
{

// API search type: replace_if : ((a -> Bool), a, [a]) -> [a]
// fwd bind count: 2
// replace_if(is_even, 0, [1, 3, 4, 6, 7]) == [1, 3, 0, 0, 7]
template <typename UnaryPredicate, typename Container>
Container replace_if(UnaryPredicate p,
        const typename Container::value_type& dest, const Container& xs)
{
    internal::check_unary_predicate_for_container<UnaryPredicate, Container>();
    Container result;
    internal::prepare_container(result, size_of_cont(xs));
    auto itOut = internal::get_back_inserter(result);
    for (const auto& x : xs)
    {
        *itOut = p(x) ? dest : x;
    }
    return result;
}

// API search type: replace_elem_at_idx : (Int, a, [a]) -> [a]
// fwd bind count: 2
// replace_elem_at_idx(2, 0, [1, 3, 4, 4, 7]) == [1, 3, 0, 4, 7]
template <typename Container,
        typename T = typename Container::value_type>
Container replace_elem_at_idx(std::size_t idx, const T& dest, const Container& xs)
{
    Container result;
    internal::prepare_container(result, size_of_cont(xs));
    auto itOut = internal::get_back_inserter(result);
    std::size_t i = 0;
    for (const auto& x : xs)
    {
        *itOut = i == idx ? dest : x;
        ++i;
    }
    return result;
}

// API search type: replace_elems : (a, a, [a]) -> [a]
// fwd bind count: 2
// replace_elems(4, 0, [1, 3, 4, 4, 7]) == [1, 3, 0, 0, 7]
template <typename Container,
        typename T = typename Container::value_type>
Container replace_elems(const T& source, const T& dest, const Container& xs)
{
    return replace_if(bind_1st_of_2(is_equal<T>, source), dest, xs);
}

// API search type: replace_tokens : ([a], [a], [a]) -> [a]
// fwd bind count: 2
// replace_tokens("haha", "hihi", "oh, hahaha!") == "oh, hihiha!"
template <typename Container>
Container replace_tokens
        (const Container& source, const Container& dest, const Container& xs)
{
    auto splitted = split_by_token(source, true, xs);
    return join(dest, splitted);
}

} // namespace fplus
