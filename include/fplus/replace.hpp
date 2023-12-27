// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <fplus/container_common.hpp>
#include <fplus/compare.hpp>
#include <fplus/split.hpp>

namespace fplus
{

namespace internal
{

template <typename UnaryPredicate, typename T, typename Container>
Container replace_if(internal::reuse_container_t,
    UnaryPredicate p, const T& dest, Container&& xs)
{
    std::replace_if(std::begin(xs), std::end(xs), p, dest);
    return std::forward<Container>(xs);
}

template <typename UnaryPredicate, typename T, typename Container>
Container replace_if(internal::create_new_container_t,
    UnaryPredicate p, const T& dest, const Container& xs)
{
    Container ys = xs;
    return replace_if(internal::reuse_container_t(),
        p, dest, std::move(ys));
}

} // namespace internal

// API search type: replace_if : ((a -> Bool), a, [a]) -> [a]
// fwd bind count: 2
// Replace every element fulfilling a predicate with a specific value.
// replace_if(is_even, 0, [1, 3, 4, 6, 7]) == [1, 3, 0, 0, 7]
template <typename UnaryPredicate, typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut replace_if(UnaryPredicate p,
    const typename ContainerOut::value_type& dest, Container&& xs)
{
    return internal::replace_if(internal::can_reuse_v<Container>{},
        p, dest, std::forward<Container>(xs));
}

namespace internal
{

template <typename Container,
        typename T = typename Container::value_type>
Container replace_elem_at_idx(internal::reuse_container_t,
    std::size_t idx, const T& dest, Container&& xs)
{
    assert(idx < xs.size());
    auto it = std::begin(xs);
    advance_iterator(it, idx);
    *it = dest;
    return std::forward<Container>(xs);
}

template <typename Container,
        typename T = typename Container::value_type>
Container replace_elem_at_idx(internal::create_new_container_t,
    std::size_t idx, const T& dest, const Container& xs)
{
    Container ys = xs;
    return replace_elem_at_idx(internal::reuse_container_t(),
        idx, dest, std::move(ys));
}

} // namespace internal

// API search type: replace_elem_at_idx : (Int, a, [a]) -> [a]
// fwd bind count: 2
// Replace the element at a specific index.
// replace_elem_at_idx(2, 0, [1, 3, 4, 4, 7]) == [1, 3, 0, 4, 7]
template <typename Container,
        typename ContainerOut = internal::remove_const_and_ref_t<Container>,
        typename T = typename ContainerOut::value_type>
ContainerOut replace_elem_at_idx(std::size_t idx, const T& dest,
    Container&& xs)
{
    return internal::replace_elem_at_idx(internal::can_reuse_v<Container>{},
        idx, dest, std::forward<Container>(xs));
}

// API search type: replace_elems : (a, a, [a]) -> [a]
// fwd bind count: 2
// Replace all elements matching source with dest.
// replace_elems(4, 0, [1, 3, 4, 4, 7]) == [1, 3, 0, 0, 7]
template <typename Container,
        typename ContainerOut = internal::remove_const_and_ref_t<Container>,
        typename T = typename ContainerOut::value_type>
ContainerOut replace_elems(const T& source, const T& dest, Container&& xs)
{
    return replace_if(bind_1st_of_2(is_equal<T>, source), dest, xs);
}

// API search type: replace_tokens : ([a], [a], [a]) -> [a]
// fwd bind count: 2
// Replace all segments matching source with dest.
// replace_tokens("haha", "hihi", "oh, hahaha!") == "oh, hihiha!"
// replace_tokens("haha", "o", "oh, hahaha!") == "oh, oha!"
template <typename Container>
Container replace_tokens
        (const Container& source, const Container& dest, const Container& xs)
{
    auto splitted = split_by_token(source, true, xs);
    return join(dest, splitted);
}

} // namespace fplus
