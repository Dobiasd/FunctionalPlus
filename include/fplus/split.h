// Copyright Tobias Hermann 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "container_common.h"
#include "search.h"
#include "pairs.h"

namespace fplus
{

// ContainerOut is not deduced to
// SameContNewType(ContainerIn, ContainerIn)
// here, since ContainerIn could be a std::string.

// group_by((==), [1,2,2,2,3,2,2,4,5,5]) == [[1],[2,2,2],[3],[2,2],4,[5,5]]
template <typename BinaryPredicate, typename ContainerIn,
        typename ContainerOut = typename std::list<ContainerIn>>
ContainerOut group_by(BinaryPredicate p, const ContainerIn& xs)
{
    check_binary_predicate_for_container<BinaryPredicate, ContainerIn>();
    static_assert(std::is_same<ContainerIn,
        typename ContainerOut::value_type>::value,
        "Containers do not match.");
    ContainerOut result;
    if (is_empty(xs))
        return result;
    typedef typename ContainerOut::value_type InnerContainerOut;
    *get_back_inserter(result) = InnerContainerOut(1, xs.front());
    for (auto it = ++std::begin(xs); it != std::end(xs); ++it)
    {
        if (p(*it, result.back().front()))
            *get_back_inserter(result.back()) = *it;
        else
            *get_back_inserter(result) = InnerContainerOut(1, *it);
    }
    return result;
}

// group([1,2,2,2,3,2,2,4,5,5]) == [[1],[2,2,2],[3],[2,2],4,[5,5]]
template <typename ContainerIn,
        typename ContainerOut = typename std::list<ContainerIn>>
ContainerOut group(ContainerIn& xs)
{
    static_assert(std::is_same<ContainerIn,
        typename ContainerOut::value_type>::value,
        "Containers do not match.");
    typedef typename ContainerIn::value_type T;
    auto pred = [](const T& x, const T& y) { return x == y; };
    return group_by(pred, xs);
}

// split_by(is_even, true, [1,3,2,2,5,5,3,6,7,9]) == [[1,3],[],[5,5,3],[7,9]]
template <typename UnaryPredicate, typename ContainerIn,
        typename ContainerOut = typename std::list<ContainerIn>>
ContainerOut split_by
        (UnaryPredicate pred, bool allowEmpty, const ContainerIn& xs)
{
    typedef typename ContainerIn::value_type T;
    typedef typename ContainerOut::value_type ContainerOutInner;
    check_unary_predicate_for_container<UnaryPredicate, ContainerIn>();
    static_assert(std::is_same<ContainerIn, typename ContainerOut::value_type>::value, "Containers do not match.");
    ContainerOut result;
    auto itOut = get_back_inserter(result);
    ContainerOutInner current;
    auto itOutCurrent = get_back_inserter(current);
    for (const T& x : xs)
    {
        if (pred(x))
        {
            if (is_not_empty(current) || allowEmpty)
            {
                *itOut = current;
                current.clear();
                itOutCurrent = get_back_inserter(current);
            }
        }
        else
        {
            *itOutCurrent = x;
        }
    }
    if (is_not_empty(current) || allowEmpty)
    {
        *itOut = current;
    }
    return result;
}

// split_at_idx(2, [0,1,2,3,4]) == ([0,1],[2,3,4])
template <typename Container>
std::pair<Container, Container> split_at_idx
        (std::size_t idx, const Container& xs)
{
    assert(idx <= size_of_cont(xs));
    return make_pair(get_range(0, idx, xs),
        get_range(idx, size_of_cont(xs), xs));
}

// partition(is_even, [0,1,1,3,7,2,3,4]) == ([0,2,4],[1,1,3,7,3])
template <typename UnaryPredicate, typename Container>
std::pair<Container, Container> partition
        (UnaryPredicate pred, const Container& xs)
{
    check_unary_predicate_for_container<UnaryPredicate, Container>();
    Container matching;
    Container notMatching;
    auto itOutMatching = get_back_inserter(matching);
    auto itOutNotMatching = get_back_inserter(notMatching);
    for (auto& x : xs)
    {
        if (pred(x))
            *itOutMatching = x;
        else
            *itOutNotMatching = x;
    }
    return make_pair(matching, notMatching);
}

// split_at_idxs([2,5], [0,1,2,3,4,5,6,7]) == [[0,1],[2,3,4],[5,6,7]]
template <typename ContainerIdxs, typename ContainerIn,
        typename ContainerOut = std::vector<ContainerIn>>
ContainerOut split_at_idxs(const ContainerIdxs& idxsIn, const ContainerIn& xs)
{
    static_assert(std::is_same<ContainerIn, typename ContainerOut::value_type>::value, "Containers do not match.");
    ContainerIdxs idxStartC = {0};
    ContainerIdxs idxEndC = {size_of_cont(xs)};
    std::vector<ContainerIdxs> containerIdxss = {idxStartC, idxsIn, idxEndC};
    auto idxs = concat(containerIdxss);
    auto idxsClean = unique(sort(idxs));
    ContainerOut result;
    prepare_container(result, size_of_cont(idxsClean) + 1);
    auto itOut = get_back_inserter(result);
    typedef std::vector<std::pair<std::size_t, std::size_t>> PositionPairs;
    auto idxPairs = overlapping_pairs<PositionPairs>(idxsClean);
    for (const auto& idxPair : idxPairs)
    {
        *itOut = get_range(idxPair.first, idxPair.second, xs);
    }
    return result;
}

// split_by_token(", ", "foo, bar, baz") == ["foo", "bar", "baz"]
template <typename ContainerIn,
        typename ContainerOut = typename std::list<ContainerIn>>
ContainerOut split_by_token(const ContainerIn& token,
        bool allowEmpty, const ContainerIn& xs)
{
    static_assert(std::is_same<ContainerIn, typename ContainerOut::value_type>::value, "Containers do not match.");
    auto instances = find_all_instances_of_non_overlapping(token, xs);
    *get_back_inserter(instances) = size_of_cont(xs);
    std::list<ContainerIn> result;
    auto itOut = get_back_inserter(result);
    std::size_t lastEnd = 0;
    for (std::size_t idx : instances)
    {
        if (idx != lastEnd || allowEmpty)
        {
            *itOut = get_range(lastEnd, idx, xs);
            lastEnd = idx + size_of_cont(token);
        }
    }
    return result;
}

// count_occurrences([1,2,2,3,2)) == [(1, 1), (2, 3), (3, 1)]
template <typename ContainerIn,
        typename MapOut = typename std::map<typename ContainerIn::value_type, std::size_t>>
MapOut count_occurrences(const ContainerIn& xs)
{
    MapOut result;
    for (auto& x : xs)
    {
        ++result[x];
    }
    return result;
}

// take(3, [0,1,2,3,4,5,6,7]) == [0,1,2]
template <typename Container>
Container take(std::size_t amount, const Container& xs)
{
    assert(amount <= size_of_cont(xs));
    return get_range(0, amount, xs);
}

// drop(3, [0,1,2,3,4,5,6,7]) == [3,4,5,6,7]
template <typename Container>
Container drop(std::size_t amount, const Container& xs)
{
    assert(amount <= size_of_cont(xs));
    return get_range(amount, size_of_cont(xs), xs);
}

// take_while(is_even, [0,2,4,5,6,7,8]) == [0,2,4]
template <typename Container, typename UnaryPredicate>
Container take_while(UnaryPredicate pred, const Container& xs)
{
    auto maybeIdx = find_first_idx_by(logical_not(pred), xs);
    return take(with_default<std::size_t>(size_of_cont(xs), maybeIdx), xs);
}

// drop_while(is_even, [0,2,4,5,6,7,8]) == [5,6,7,8]
template <typename Container, typename UnaryPredicate>
Container drop_while(UnaryPredicate pred, const Container& xs)
{
    auto maybeIdx = find_first_idx_by(logical_not(pred), xs);
    return drop(with_default<std::size_t>(size_of_cont(xs), maybeIdx), xs);
}

} // namespace fplus