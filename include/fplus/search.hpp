// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <fplus/container_common.hpp>
#include <fplus/composition.hpp>
#include <fplus/generate.hpp>
#include <fplus/maybe.hpp>

#include <algorithm>

namespace fplus
{

// API search type: find_first_by : ((a -> Bool), [a]) -> Maybe a
// fwd bind count: 1
// Returns the first element fulfilling the predicate.
// find_first_by(is_even, [1, 3, 4, 6, 9]) == Just(4)
// find_first_by(is_even, [1, 3, 5, 7, 9]) == Nothing
template <typename Container, typename UnaryPredicate,
    typename T = typename Container::value_type>
maybe<T> find_first_by(UnaryPredicate pred, const Container& xs)
{
    internal::check_unary_predicate_for_container<UnaryPredicate, Container>();
    auto it = std::find_if(std::begin(xs), std::end(xs), pred);
    if (it == std::end(xs))
        return nothing<T>();
    return just<T>(*it);
}

// API search type: find_last_by : ((a -> Bool), [a]) -> Maybe a
// fwd bind count: 1
// Returns the last element fulfilling the predicate.
// find_last_by(is_even, [1, 3, 4, 6, 9]) == Just(6)
// find_last_by(is_even, [1, 3, 5, 7, 9]) == Nothing
template <typename Container, typename UnaryPredicate,
    typename T = typename Container::value_type>
maybe<T> find_last_by(UnaryPredicate pred, const Container& xs)
{
    internal::check_unary_predicate_for_container<UnaryPredicate, Container>();
    return find_first_by(pred, reverse(xs));
}

// API search type: find_first_idx_by : ((a -> Bool), [a]) -> Maybe Int
// fwd bind count: 1
// Returns the index of the first element fulfilling the predicate.
// find_first_idx_by(is_even, [1, 3, 4, 6, 9]) == Just(2)
// find_first_idx_by(is_even, [1, 3, 5, 7, 9]) == Nothing
template <typename Container, typename UnaryPredicate>
maybe<std::size_t> find_first_idx_by
        (UnaryPredicate pred, const Container& xs)
{
    internal::check_unary_predicate_for_container<UnaryPredicate, Container>();
    auto it = std::find_if(std::begin(xs), std::end(xs), pred);
    if (it == std::end(xs))
        return nothing<std::size_t>();
    return static_cast<std::size_t>(std::distance(std::begin(xs), it));
}

// API search type: find_last_idx_by : ((a -> Bool), [a]) -> Maybe Int
// fwd bind count: 1
// Returns the index of the last element fulfilling the predicate.
// find_last_idx_by(is_even, [1, 3, 4, 6, 9]) == Just(3)
// find_last_idx_by(is_even, [1, 3, 5, 7, 9]) == Nothing
template <typename Container, typename UnaryPredicate>
maybe<std::size_t> find_last_idx_by
        (UnaryPredicate pred, const Container& xs)
{
    internal::check_unary_predicate_for_container<UnaryPredicate, Container>();
    auto calcRevIdx = [&](std::size_t idx)
    {
        return size_of_cont(xs) - (idx + 1);
    };
    return lift_maybe(calcRevIdx, find_first_idx_by(pred, reverse(xs)));
}

// API search type: find_first_idx : (a, [a]) -> Maybe Int
// fwd bind count: 1
// Returns the index of the first element equal to x.
// find_first_idx(4, [1, 3, 4, 4, 9]) == Just(2)
// find_first_idx(4, [1, 3, 5, 7, 9]) == Nothing
template <typename Container>
maybe<std::size_t> find_first_idx
        (const typename Container::value_type& x, const Container& xs)
{
    return find_first_idx_by(is_equal_to(x), xs);
}

// API search type: find_last_idx : (a, [a]) -> Maybe Int
// fwd bind count: 1
// Returns the index of the last element equal to x.
// find_last_idx(4, [1, 3, 4, 4, 9]) == Just(3)
// find_last_idx(4, [1, 3, 5, 7, 9]) == Nothing
template <typename Container>
maybe<std::size_t> find_last_idx
        (const typename Container::value_type& x, const Container& xs)
{
    return find_last_idx_by(is_equal_to(x), xs);
}

// API search type: find_all_idxs_by : ((a -> Bool), [a]) -> [Int]
// fwd bind count: 1
// Returns the indices off all elements fulfilling the predicate.
// find_all_idxs_by(is_even, [1, 3, 4, 6, 9]) == [2, 3]
template <typename ContainerOut = std::vector<std::size_t>,
        typename UnaryPredicate, typename Container>
ContainerOut find_all_idxs_by(UnaryPredicate p, const Container& xs)
{
    internal::check_unary_predicate_for_container<UnaryPredicate, Container>();
    std::size_t idx = 0;
    ContainerOut result;
    auto itOut = internal::get_back_inserter(result);
    for (const auto& x : xs)
    {
        if (internal::invoke(p, x))
            *itOut = idx;
        ++idx;
    }
    return result;
}

// API search type: find_all_idxs_of : (a, [a]) -> [Int]
// fwd bind count: 1
// Returns the indices off all elements equal to x.
// find_all_idxs_of(4, [1, 3, 4, 4, 9]) == [2, 3]
template <typename ContainerOut = std::vector<std::size_t>,
        typename Container,
        typename T = typename Container::value_type>
ContainerOut find_all_idxs_of
        (const T& x, const Container& xs)
{
    return find_all_idxs_by(is_equal_to(x), xs);
}

// API search type: find_all_instances_of_token : ([a], [a]) -> [Int]
// fwd bind count: 1
// Returns the starting indices of all segments matching token.
// find_all_instances_of_token("haha", "oh, hahaha!") == [4, 6]
template <typename ContainerOut =
    std::vector<std::size_t>, typename Container>
ContainerOut find_all_instances_of_token(const Container& token,
        const Container& xs)
{
    if (size_of_cont(token) > size_of_cont(xs))
        return ContainerOut();

    auto itInBegin = std::begin(xs);
    auto itInEnd = itInBegin;
    internal::advance_iterator(itInEnd, size_of_cont(token));
    std::size_t idx = 0;
    ContainerOut result;
    auto outIt = internal::get_back_inserter(result);
    std::size_t last_possible_idx = size_of_cont(xs) - size_of_cont(token);
    auto check_and_push = [&]()
    {
        if (std::equal(itInBegin, itInEnd, std::begin(token)))
        {
            *outIt = idx;
        }
    };
    while (idx != last_possible_idx)
    {
        check_and_push();
        ++itInBegin;
        ++itInEnd;
        ++idx;
    }
    check_and_push();
    return result;
}

// API search type: find_all_instances_of_token_non_overlapping : ([a], [a]) -> [Int]
// fwd bind count: 1
// Returns the starting indices
// of all non-overlapping segments matching token.
// find_all_instances_of_token_non_overlapping("haha", "oh, hahaha!") == [4]
template <typename ContainerOut = std::vector<std::size_t>, typename Container>
ContainerOut find_all_instances_of_token_non_overlapping
        (const Container& token, const Container& xs)
{
    auto overlapping_instances = find_all_instances_of_token<ContainerOut>(
            token, xs);
    ContainerOut result;
    auto outIt = internal::get_back_inserter(result);
    std::size_t token_size = size_of_cont(token);
    for (const auto idx : overlapping_instances)
    {
        if (result.empty() || result.back() + token_size <= idx)
        {
            *outIt = idx;
        }
    }
    return result;
}

// API search type: find_first_instance_of_token : ([a], [a]) -> Maybe Int
// fwd bind count: 1
// Returns the index of the first segment matching token.
// find_first_instance_of_token("haha", "oh, hahaha!") == just 4
template <typename Container>
maybe<std::size_t> find_first_instance_of_token
        (const Container& token, const Container& xs)
{
    if (size_of_cont(token) > size_of_cont(xs))
        return nothing<std::size_t>();

    auto itInBegin = std::begin(xs);
    auto itInEnd = itInBegin;
    internal::advance_iterator(itInEnd, size_of_cont(token));
    std::size_t idx = 0;
    std::size_t last_possible_idx = size_of_cont(xs) - size_of_cont(token);
    while (idx != last_possible_idx)
    {
        if (std::equal(itInBegin, itInEnd, std::begin(token)))
        {
            return just(idx);
        }
        ++itInBegin;
        ++itInEnd;
        ++idx;
    }
    if (std::equal(itInBegin, itInEnd, std::begin(token)))
    {
        return just(idx);
    }
    return nothing<std::size_t>();
}

} // namespace fplus
