// Copyright Tobias Hermann 2015.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "maybe.h"
#include "result.h"

#include "container_common.h"

#include <algorithm>

namespace fplus
{

// API search type: keep_if : (a -> Bool) -> [a] -> [a]
// keep_if(is_even, [1, 2, 3, 2, 4, 5]) == [2, 2, 4]
// Also known as filter.
template <typename Pred, typename Container>
Container keep_if(Pred pred, const Container& xs)
{
    check_unary_predicate_for_container<Pred, Container>();
    Container result;
    auto it = get_back_inserter<Container>(result);
    std::copy_if(std::begin(xs), std::end(xs), it, pred);
    return result;
}

// API search type: drop_if : (a -> Bool) -> [a] -> [a]
// drop_if(is_even, [1, 2, 3, 2, 4, 5]) == [1, 3, 5]
// Also known as reject.
template <typename Pred, typename Container>
Container drop_if(Pred pred, const Container& xs)
{
    check_unary_predicate_for_container<Pred, Container>();
    return keep_if(logical_not(pred), xs);
}

// API search type: without : a -> [a] -> [a]
// without(0, [1, 0, 0, 5, 3, 0, 1]) == [1, 5, 3, 1]
template <typename Container,
    typename T = typename Container::value_type>
Container without(T elem, const Container& xs)
{
    return drop_if(is_equal_to(elem), xs);
}

// API search type: keep_if_with_idx : (Int -> a -> Bool) -> [a] -> [a]
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

// API search type: drop_if_with_idx : (Int -> a -> Bool) -> [a] -> [a]
// Predicate takes index and value.
// All elements fulfilling the predicate are skipped.
template <typename Pred, typename Container>
Container drop_if_with_idx(Pred pred, const Container& xs)
{
    check_index_with_type_predicate_for_container<Pred, Container>();
    return keep_if_with_idx(logical_not(pred), xs);
}

// API search type: keep_by_idx : (Int -> Bool) -> [a] -> [a]
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

// API search type: drop_by_idx : (Int -> Bool) -> [a] -> [a]
// Predicate takes an index and decides if an element is dropped.
template <typename UnaryPredicate, typename Container>
Container drop_by_idx(UnaryPredicate pred, const Container& xs)
{
    check_unary_predicate_for_type<UnaryPredicate, std::size_t>();
    return keep_by_idx(logical_not(pred), xs);
}

// API search type: keep_idxs : [Int] -> [a] -> [a]
// keep_idxs([2,5], [1,2,3,4,5,6,7]) == [3,6]
template <typename ContainerIdxs, typename Container>
Container keep_idxs(const ContainerIdxs& idxs_to_keep, const Container& xs)
{
    auto idxs_left = convert_container<std::list<std::size_t>>(
        unique(sort(idxs_to_keep)));
    Container ys;
    auto it = get_back_inserter<Container>(ys);
    std::size_t idx = 0;
    for (const auto& x : xs)
    {
        if (!idxs_left.empty() && idxs_left.front() == idx)
        {
            idxs_left.pop_front();
            *it = x;
        }
        ++idx;
    }
    return ys;
}

// API search type: drop_idxs : [Int] -> [a] -> [a]
// drop_idxs([2,5], [1,2,3,4,5,6,7]) == [1,2,4,5,7]
template <typename ContainerIdxs, typename Container>
Container drop_idxs(const ContainerIdxs& idxs_to_drop, const Container& xs)
{
    auto idxs_left = convert_container<std::list<std::size_t>>(
        unique(sort(idxs_to_drop)));
    Container ys;
    auto it = get_back_inserter<Container>(ys);
    std::size_t idx = 0;
    for (const auto& x : xs)
    {
        if (idxs_left.empty() || idxs_left.front() != idx)
        {
            *it = x;
        }
        else
        {
            if (!idxs_left.empty())
                idxs_left.pop_front();
        }
        ++idx;
    }
    return ys;
}

// API search type: justs : [Maybe a] -> [a]
// From a Container filled with Maybe<T> the nothings are dropped
// and the values inside the justs are returned in a new container.
template <typename ContainerIn,
    typename ContainerOut =
        typename same_cont_new_t<ContainerIn,
            typename ContainerIn::value_type::type>::type>
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

// API search type: oks : [Result a b] -> [a]
// From a Container filled with Result<Ok, Error> the errors are dropped
// and the values inside the ok are returned in a new container.
template <typename ContainerIn,
    typename ContainerOut =
        typename same_cont_new_t<ContainerIn,
            typename ContainerIn::value_type::ok_t>::type>
ContainerOut oks(const ContainerIn& xs)
{
    typedef typename ContainerIn::value_type::ok_t Ok;
    typedef typename ContainerIn::value_type::error_t Error;
    auto oksInResults = keep_if(is_ok<Ok, Error>, xs);
    ContainerOut ys;
    prepare_container(ys, fplus::size_of_cont(oksInResults));
    auto itOut = get_back_inserter<ContainerOut>(ys);
    std::transform(std::begin(oksInResults), std::end(oksInResults),
        itOut, unsafe_get_ok<Ok, Error>);
    return ys;
}

// API search type: errors : [Result a b] -> [b]
// From a Container filled with Result<Ok, Error> the oks are dropped
// and the values inside the errors are returned in a new container.
template <typename ContainerIn,
    typename ContainerOut =
        typename same_cont_new_t<ContainerIn,
            typename ContainerIn::value_type::error_t>::type>
ContainerOut errors(const ContainerIn& xs)
{
    typedef typename ContainerIn::value_type::ok_t Ok;
    typedef typename ContainerIn::value_type::error_t Error;
    auto errorsInResults = keep_if(is_error<Ok, Error>, xs);
    ContainerOut ys;
    prepare_container(ys, fplus::size_of_cont(errorsInResults));
    auto itOut = get_back_inserter<ContainerOut>(ys);
    std::transform(std::begin(errorsInResults), std::end(errorsInResults),
        itOut, unsafe_get_error<Ok, Error>);
    return ys;
}

// API search type: trim_left_by : (a -> Bool) -> [a] -> [a]
// trim_left_by(is_even, [0,2,4,5,6,7,8,6,4]) == [5,6,7,8,6,4]
template <typename Container, typename UnaryPredicate>
Container trim_left_by(UnaryPredicate p, const Container& xs)
{
    check_unary_predicate_for_container<UnaryPredicate, Container>();
    auto itFirstNot = std::find_if_not(std::begin(xs), std::end(xs), p);
    if (itFirstNot == std::end(xs))
        return Container();
    return Container(itFirstNot, std::end(xs));
}

// API search type: trim_left : a -> [a] -> [a]
// trim_left('_', "___abc__") == "abc__"
// trim_left(0, [0,0,0,5,6,7,8,6,4]) == [5,6,7,8,6,4]
template <typename Container,
        typename T = typename Container::value_type>
Container trim_left(const T& x, const Container& xs)
{
    return trim_left_by(is_equal_to(x), xs);
}

// API search type: trim_token_left : [a] -> [a] -> [a]
// trim_token_left([0,1,2], [0,1,2,0,1,2,7,5,9]) == [7,5,9]
template <typename Container>
Container trim_token_left(const Container& token, const Container& xs)
{
    auto result = xs;
    while (is_prefix_of(token, result))
    {
        result = get_range(size_of_cont(token), size_of_cont(result), result);
    }
    return result;
}

// API search type: trim_right_by : (a -> Bool) -> [a] -> [a]
// trim_right_by(is_even, [0,2,4,5,6,7,8,6,4]) == [0,2,4,5,6,7]
template <typename Container, typename UnaryPredicate>
Container trim_right_by(UnaryPredicate p, const Container& xs)
{
    check_unary_predicate_for_container<UnaryPredicate, Container>();
    return reverse(trim_left_by(p, reverse(xs)));
}

// API search type: trim_right : a -> [a] -> [a]
// trim_right('_', "___abc__") == "___abc"
// trim_right(4, [0,2,4,5,6,7,8,4,4]) == [0,2,4,5,6,7,8]
template <typename Container,
        typename T = typename Container::value_type>
Container trim_right(const T& x, const Container& xs)
{
    return trim_right_by(is_equal_to(x), xs);
}

// API search type: trim_token_right : [a] -> [a] -> [a]
// trim_token_right([0,1,2], [7,5,9,0,1,2,0,1,2]) == [7,5,9]
template <typename Container>
Container trim_token_right(const Container& token, const Container& xs)
{
    return reverse(trim_token_left(reverse(token), reverse(xs)));
}

// API search type: trim_by : (a -> Bool) -> [a] -> [a]
// trim_by(is_even, [0,2,4,5,6,7,8,6,4]) == [5,6,7]
template <typename Container, typename UnaryPredicate>
Container trim_by(UnaryPredicate p, const Container& xs)
{
    check_unary_predicate_for_container<UnaryPredicate, Container>();
    return trim_right_by(p, trim_left_by(p, xs));
}

// API search type: trim : a -> [a] -> [a]
// trim('_', "___abc__") == "abc"
// trim(0, [0,2,4,5,6,7,8,0,0]) == [2,4,5,6,7,8]
template <typename Container,
        typename T = typename Container::value_type>
Container trim(const T& x, const Container& xs)
{
    return trim_right(x, trim_left(x, xs));
}

// API search type: trim_token : [a] -> [a] -> [a]
// trim_token([0,1], [0,1,7,8,9,0,1]) == [7,8,9]
template <typename Container>
Container trim_token(const Container& token, const Container& xs)
{
    return trim_token_right(token, trim_token_left(token, xs));
}

} // namespace fplus
