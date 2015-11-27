// Copyright Tobias Hermann 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <algorithm>

#include "container_common.h"
#include "composition.h"
#include "generate.h"
#include "maybe.h"


namespace fplus
{

// find_first_by(is_even, [1, 3, 4, 6, 9]) == Just(4)
// find_first_by(is_even, [1, 3, 5, 7, 9]) == Nothing
template <typename Container, typename UnaryPredicate,
    typename T = typename Container::value_type>
maybe<T> find_first_by(UnaryPredicate pred, const Container& xs)
{
    check_unary_predicate_for_container<UnaryPredicate, Container>();
    auto it = std::find_if(std::begin(xs), std::end(xs), pred);
    if (it == std::end(xs))
        return nothing<T>();
    return just<T>(*it);
}

// find_last_by(is_even, [1, 3, 4, 6, 9]) == Just(6)
// find_last_by(is_even, [1, 3, 5, 7, 9]) == Nothing
template <typename Container, typename UnaryPredicate,
    typename T = typename Container::value_type>
maybe<T> find_last_by(UnaryPredicate pred, const Container& xs)
{
    check_unary_predicate_for_container<UnaryPredicate, Container>();
    return find_first_by(pred, reverse(xs));
}

// find_first_idx_by(is_even, [1, 3, 4, 6, 9]) == Just(2)
// find_first_idx_by(is_even, [1, 3, 5, 7, 9]) == Nothing
template <typename Container, typename UnaryPredicate>
maybe<std::size_t> find_first_idx_by
        (UnaryPredicate pred, const Container& xs)
{
    check_unary_predicate_for_container<UnaryPredicate, Container>();
    auto it = std::find_if(std::begin(xs), std::end(xs), pred);
    if (it == std::end(xs))
        return nothing<std::size_t>();
    return just<std::size_t>(std::distance(std::begin(xs), it));
}

// find_last_idx_by(is_even, [1, 3, 4, 6, 9]) == Just(3)
// find_last_idx_by(is_even, [1, 3, 5, 7, 9]) == Nothing
template <typename Container, typename UnaryPredicate>
maybe<std::size_t> find_last_idx_by
        (UnaryPredicate pred, const Container& xs)
{
    check_unary_predicate_for_container<UnaryPredicate, Container>();
    auto calcRevIdx = [&](std::size_t idx) {
        return size_of_cont(xs) - (idx + 1);
    };
    return lift(calcRevIdx)
            (find_first_idx_by(pred, reverse(xs)));
}

// find_first_idx(4, [1, 3, 4, 4, 9]) == Just(2)
// find_first_idx(4, [1, 3, 5, 7, 9]) == Nothing
template <typename Container>
maybe<std::size_t> find_first_idx
        (const typename Container::value_type& x, const Container& xs)
{
    typedef typename Container::value_type T;
    auto pred = bind_1st_of_2(is_equal<T>, x);
    return find_first_idx_by(pred, xs);
}

// find_last_idx(4, [1, 3, 4, 4, 9]) == Just(3)
// find_last_idx(4, [1, 3, 5, 7, 9]) == Nothing
template <typename Container>
maybe<std::size_t> find_last_idx
        (const typename Container::value_type& x, const Container& xs)
{
    typedef typename Container::value_type T;
    auto pred = bind_1st_of_2(is_equal<T>, x);
    return find_last_idx_by(pred, xs);
}

// find_all_idxs_by(is_even, [1, 3, 4, 6, 9]) == [2, 3]
template <typename ContainerOut = std::list<std::size_t>,
        typename UnaryPredicate, typename Container>
ContainerOut find_all_idxs_by(UnaryPredicate p, const Container& xs)
{
    check_unary_predicate_for_container<UnaryPredicate, Container>();
    std::size_t idx = 0;
    ContainerOut result;
    auto itOut = get_back_inserter(result);
    for (const auto& x : xs)
    {
        if (p(x))
            *itOut = idx;
        ++idx;
    }
    return result;
}

// find_all_idxs_of(4, [1, 3, 4, 4, 9]) == [2, 3]
template <typename ContainerOut = std::list<std::size_t>, typename Container>
ContainerOut find_all_idxs_of
        (const typename Container::value_type& x, const Container& xs)
{
    typedef typename Container::value_type T;
    auto pred = bind_1st_of_2(is_equal<T>, x);
    return find_all_idxs_by(pred, xs);
}

// find_all_instances_of("haha", "oh, hahaha!") == [4, 6]
template <typename ContainerOut = std::list<std::size_t>, typename Container>
ContainerOut find_all_instances_of(const Container& token, const Container& xs)
{
    typedef std::vector<Container> Containers;
    auto candidates = infixes<Containers>(size_of_cont(token), xs);
    ContainerOut result;
    std::size_t idx = 0;
    auto outIt = get_back_inserter(result);
    for (const auto& candidate : candidates)
    {
        if (candidate == token)
            *outIt = idx;
        ++idx;
    }
    return result;
}

// find_all_instances_of_non_overlapping("haha", "oh, hahaha!") == [4]
template <typename ContainerOut = std::list<std::size_t>, typename Container>
ContainerOut find_all_instances_of_non_overlapping
        (const Container& token, const Container& xs)
{
    ContainerOut result;
    if (is_empty(token))
        return result;
    typedef typename Container::value_type T;
    auto tokenAsList = convert<std::list<T>>(token);
    auto tokenRemaining = tokenAsList;
    std::size_t idx = 0;
    auto itOut = get_back_inserter(result);
    for (const T& x : xs)
    {
        if (x == tokenRemaining.front())
        {
            tokenRemaining.pop_front();
            if (is_empty(tokenRemaining))
            {
                *itOut = idx - (size_of_cont(token) - 1);
                tokenRemaining = tokenAsList;
            }
        }
        else
        {
            tokenRemaining = tokenAsList;
        }
        ++idx;
    }
    return result;
}

} // namespace fplus