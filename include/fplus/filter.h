// Copyright Tobias Hermann 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "maybe.h"

#include <algorithm>

namespace fplus
{

// keep_if(is_even, [1, 2, 3, 2, 4, 5]) == [2, 2, 4]
// Also known as Filter.
template <typename Pred, typename Container>
Container keep_if(Pred pred, const Container& xs)
{
    check_unary_predicate_for_container<Pred, Container>();
    Container result;
    auto it = get_back_inserter<Container>(result);
    std::copy_if(std::begin(xs), std::end(xs), it, pred);
    return result;
}

// drop_if(is_even, [1, 2, 3, 2, 4, 5]) == [1, 3, 5]
// Also known as Reject.
template <typename Pred, typename Container>
Container drop_if(Pred pred, const Container& xs)
{
    check_unary_predicate_for_container<Pred, Container>();
    return keep_if(logical_not(pred), xs);
}

// without(0, [1, 0, 0, 5, 3, 0, 1]) == [1, 5, 3, 1]
template <typename Container,
    typename T = typename Container::value_type>
Container without(T elem, const Container& xs)
{
    auto pred = bind_1_of_2(is_equal<T>, elem);
    return drop_if(pred, xs);
}

// Predicate takes index and value.
// All elements fulfilling the predicate are kept.
template <typename Pred, typename Container>
Container keep_if_with_idx(Pred pred, const Container& xs)
{
    check_index_with_type_predicate_for_container<Pred, Container>();
    Container ys;
    auto it = get_back_inserter<Container>(ys);
    std::size_t idx = 0;
    for (const auto& x : xs)
    {
        if (pred(idx++, x))
            *it = x;
    }
    return ys;
}

// Predicate takes index and value.
// All elements fulfilling the predicate are skipped.
template <typename Pred, typename Container>
Container drop_if_with_idx(Pred pred, const Container& xs)
{
    check_index_with_type_predicate_for_container<Pred, Container>();
    return keep_if_with_idx(logical_not(pred), xs);
}

// Predicate takes an index and decides if an element is kept.
template <typename UnaryPredicate, typename Container>
Container keep_by_idx(UnaryPredicate pred, const Container& xs)
{
    check_unary_predicate_for_type<UnaryPredicate, std::size_t>();
    Container ys;
    auto it = get_back_inserter<Container>(ys);
    std::size_t idx = 0;
    for (const auto& x : xs)
    {
        if (pred(idx++))
            *it = x;
    }
    return ys;
}

// Predicate takes an index and decides if an element is dropped.
template <typename UnaryPredicate, typename Container>
Container drop_by_idx(UnaryPredicate pred, const Container& xs)
{
    check_unary_predicate_for_type<UnaryPredicate, std::size_t>();
    return keep_by_idx(logical_not(pred), xs);
}

// From a Container filled with Maybe<T> the nothings are dropped
// and the values inside the justs are returned in a new container.
template <typename ContainerIn,
    typename ContainerOut =
        typename same_cont_new_t<ContainerIn, typename ContainerIn::value_type::type>::type>
ContainerOut justs(const ContainerIn& xs)
{
    typedef typename ContainerIn::value_type::type T;
    auto justsInMaybes = keep_if(is_just<T>, xs);
    ContainerOut ys;
    prepare_container(ys, fplus::size_of_cont(justsInMaybes));
    auto itOut = get_back_inserter<ContainerOut>(ys);
    std::transform(std::begin(justsInMaybes), std::end(justsInMaybes),
        itOut, unsafe_get_just<T>);
    return ys;
}

// trim_left(is_even, [0,2,4,5,6,7,8,6,4]) == [5,6,7,8,6,4]
template <typename Container, typename UnaryPredicate>
Container trim_left(UnaryPredicate p, const Container& xs)
{
    check_unary_predicate_for_container<UnaryPredicate, Container>();
    auto itFirstNot = std::find_if_not(std::begin(xs), std::end(xs), p);
    if (itFirstNot == std::end(xs))
        return Container();
    return Container(itFirstNot, std::end(xs));
}

// trim_right(is_even, [0,2,4,5,6,7,8,6,4]) == [0,2,4,5,6,7]
template <typename Container, typename UnaryPredicate>
Container trim_right(UnaryPredicate p, const Container& xs)
{
    check_unary_predicate_for_container<UnaryPredicate, Container>();
    return reverse(trim_left(p, reverse(xs)));
}

// trim(is_even, [0,2,4,5,6,7,8,6,4]) == [5,6,7]
template <typename Container, typename UnaryPredicate>
Container trim(UnaryPredicate p, const Container& xs)
{
    check_unary_predicate_for_container<UnaryPredicate, Container>();
    return trim_right(p, trim_left(p, xs));
}

} // namespace fplus