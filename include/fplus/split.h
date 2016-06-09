// Copyright Tobias Hermann 2015.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "container_common.h"
#include "search.h"
#include "pairs.h"

namespace fplus
{

// API search type: group_by : (((a, a) -> Bool), [a]) -> [[a]]
// group_by((==), [1,2,2,2,3,2,2,4,5,5]) == [[1],[2,2,2],[3],[2,2],[4],[5,5]]
// ContainerOut is not deduced to
// SameContNewType(ContainerIn, ContainerIn)
// here, since ContainerIn could be a std::string.
// BinaryPredicate p is a (not neccessarily assoziative) connectivity check.
// O(n)
template <typename BinaryPredicate, typename ContainerIn,
        typename ContainerOut = typename std::vector<ContainerIn>>
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
        if (p(*it, result.back().back()))
            *get_back_inserter(result.back()) = *it;
        else
            *get_back_inserter(result) = InnerContainerOut(1, *it);
    }
    return result;
}

// API search type: group_on : ((a -> b), [a]) -> [[a]]
// group_on((mod 10), [12,22,34]) == [[12,22],[34]]
// O(n)
template <typename F, typename ContainerIn,
        typename ContainerOut = typename std::vector<ContainerIn>>
ContainerOut group_on(F f, const ContainerIn& xs)
{
    return group_by(is_equal_by(f), xs);
}

// API search type: group : [a] -> [[a]]
// group([1,2,2,2,3,2,2,4,5,5]) == [[1],[2,2,2],[3],[2,2],[4],[5,5]]
// O(n)
template <typename ContainerIn,
        typename ContainerOut = typename std::vector<ContainerIn>>
ContainerOut group(const ContainerIn& xs)
{
    static_assert(std::is_same<ContainerIn,
        typename ContainerOut::value_type>::value,
        "Containers do not match.");
    typedef typename ContainerIn::value_type T;
    auto pred = [](const T& x, const T& y) { return x == y; };
    return group_by<decltype(pred), ContainerIn, ContainerOut>(pred, xs);
}

// API search type: group_globally_by : (((a, a) -> Bool), [a]) -> [[a]]
// group_globally_by((==), [1,2,2,2,3,2,2,4,5,5])
// == [[1],[2,2,2,2,2],[3],[4],[5,5]]
// BinaryPredicate p is an assoziative equality check.
// O(n^2)
template <typename BinaryPredicate, typename ContainerIn,
        typename ContainerOut = typename std::vector<ContainerIn>>
ContainerOut group_globally_by(BinaryPredicate p, const ContainerIn& xs)
{
    check_binary_predicate_for_container<BinaryPredicate, ContainerIn>();
    static_assert(std::is_same<ContainerIn,
        typename ContainerOut::value_type>::value,
        "Containers do not match.");
    typedef typename ContainerOut::value_type InnerContainerOut;
    ContainerOut result;
    for (const auto& x : xs)
    {
        bool found = false;
        for (auto& ys : result)
        {
            if (p(x, ys.front()))
            {
                *get_back_inserter(ys) = x;
                found = true;
                break;
            }
        }
        if (!found)
        {
            *get_back_inserter(result) = InnerContainerOut(1, x);
        }
    }
    return result;
}

// API search type: group_globally_on : ((a -> b), [a]) -> [[a]]
// group_globally_on((mod 10), [12,34,22]) == [[12,34],[22]]
// O(n^2)
template <typename F, typename ContainerIn,
        typename ContainerOut = typename std::vector<ContainerIn>>
ContainerOut group_globally_on(F f, const ContainerIn& xs)
{
    return group_globally_by(is_equal_by(f), xs);
}

// API search type: group_globally : [a] -> [[a]]
// group_globally([1,2,2,2,3,2,2,4,5,5]) == [[1],[2,2,2,2,2],[3],[4],[5,5]]
// O(n^2)
template <typename ContainerIn,
        typename ContainerOut = typename std::vector<ContainerIn>>
ContainerOut group_globally(const ContainerIn& xs)
{
    static_assert(std::is_same<ContainerIn,
        typename ContainerOut::value_type>::value,
        "Containers do not match.");
    typedef typename ContainerIn::value_type T;
    auto pred = [](const T& x, const T& y) { return x == y; };
    return group_globally_by(pred, xs);
}

// API search type: split_by : ((a -> Bool), Bool, [a]) -> [[a]]
// split_by(is_even, true, [1,3,2,2,5,5,3,6,7,9]) == [[1,3],[],[5,5,3],[7,9]]
// also known as split_when
// O(n)
template <typename UnaryPredicate, typename ContainerIn,
        typename ContainerOut = typename std::vector<ContainerIn>>
ContainerOut split_by
        (UnaryPredicate pred, bool allowEmpty, const ContainerIn& xs)
{
    check_unary_predicate_for_container<UnaryPredicate, ContainerIn>();
    static_assert(std::is_same<ContainerIn,
        typename ContainerOut::value_type>::value,
        "Containers do not match.");

    ContainerOut result;
    auto itOut = get_back_inserter(result);
    auto start = std::begin(xs);
    while (start != std::end(xs))
    {
        const auto stop = std::find_if(start, std::end(xs), pred);
        if (start != stop || allowEmpty)
        {
            *itOut = { start, stop };
        }
        if (stop == std::end(xs))
        {
            break;
        }
        start = std::next(stop);
        if (allowEmpty && start == std::end(xs))
        {
            *itOut = {};
        }
    }
    return result;
}

// API search type: split_by_keep_separators : ((a -> Bool), [a]) -> [[a]]
// split_by_keep_separators(is_even, true, [1,3,2,2,5,5,3,6,7,9])
// == [[1,3],[2],[2,5,5,3],[6,7,9]]
// O(n)
template <typename UnaryPredicate, typename ContainerIn,
        typename ContainerOut = typename std::vector<ContainerIn>>
ContainerOut split_by_keep_separators
        (UnaryPredicate pred, const ContainerIn& xs)
{
    check_unary_predicate_for_container<UnaryPredicate, ContainerIn>();
    static_assert(std::is_same<ContainerIn,
        typename ContainerOut::value_type>::value,
        "Containers do not match.");
    ContainerOut result;
    if (is_empty(xs))
        return result;
    auto itOut = get_back_inserter(result);
    auto start = std::begin(xs);
    while (start != std::end(xs))
    {
        const auto stop = std::find_if(std::next(start), std::end(xs), pred);
        *itOut = { start, stop };
        if (stop == std::end(xs))
        {
            break;
        }
        start = stop;
    }
    return result;
}

// API search type: split : (a, [a]) -> [[a]]
// split(0, [1,3,2,0,0,6,0,7,5]) == [[1,3,2],[],[6],[7,5]]
template <typename ContainerIn,
        typename T = typename ContainerIn::value_type,
        typename ContainerOut = typename std::vector<ContainerIn>>
// O(n)
ContainerOut split(const T& x, bool allowEmpty, const ContainerIn& xs)
{
    return split_by(is_equal_to(x), allowEmpty, xs);
}

// API search type: split_at_idx : (Int, [a]) -> ([a], [a])
// split_at_idx(2, [0,1,2,3,4]) == ([0,1],[2,3,4])
template <typename Container>
std::pair<Container, Container> split_at_idx
        (std::size_t idx, const Container& xs)
{
    assert(idx <= size_of_cont(xs));
    return make_pair(get_range(0, idx, xs),
        get_range(idx, size_of_cont(xs), xs));
}

// API search type: partition : ((a -> Bool), [a]) -> ([a], [a])
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
    for (const auto& x : xs)
    {
        if (pred(x))
            *itOutMatching = x;
        else
            *itOutNotMatching = x;
    }
    return make_pair(matching, notMatching);
}

// API search type: split_at_idxs : ([Int], [a]) -> [[a]]
// split_at_idxs([2,5], [0,1,2,3,4,5,6,7]) == [[0,1],[2,3,4],[5,6,7]]
template <typename ContainerIdxs, typename ContainerIn,
        typename ContainerOut = std::vector<ContainerIn>>
ContainerOut split_at_idxs(const ContainerIdxs& idxsIn, const ContainerIn& xs)
{
    static_assert(std::is_same<ContainerIn,
        typename ContainerOut::value_type>::value,
        "Containers do not match.");
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

// API search type: split_every : (Int, [a]) -> [[a]]
// split_every(3, [0,1,2,3,4,5,6,7]) == [[0,1,2],[3,4,5],[6,7]]
// Also known as chunk.
template <typename ContainerIn,
        typename ContainerOut = std::vector<ContainerIn>>
ContainerOut split_every(std::size_t n, const ContainerIn& xs)
{
    return split_at_idxs(
        generate_range_step<std::vector<std::size_t>, std::size_t>(
            0, size_of_cont(xs), n),
        xs);
}

// API search type: split_by_token : ([a], Bool, [a]) -> [[a]]
// split_by_token(", ", "foo, bar, baz") == ["foo", "bar", "baz"]
template <typename ContainerIn,
        typename ContainerOut = typename std::vector<ContainerIn>>
ContainerOut split_by_token(const ContainerIn& token,
        bool allowEmpty, const ContainerIn& xs)
{
    static_assert(std::is_same<ContainerIn,
        typename ContainerOut::value_type>::value,
        "Containers do not match.");
    auto instances = find_all_instances_of_token_non_overlapping(token, xs);
    *get_back_inserter(instances) = size_of_cont(xs);
    ContainerOut result;
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

// API search type: count_occurrences : [a] -> Map a Int
// count_occurrences([1,2,2,3,2)) == [(1, 1), (2, 3), (3, 1)]
// O(n)
template <typename ContainerIn,
        typename MapOut = typename std::map<
            typename ContainerIn::value_type, std::size_t>>
MapOut count_occurrences(const ContainerIn& xs)
{
    MapOut result;
    for (const auto& x : xs)
    {
        ++result[x];
    }
    return result;
}

// API search type: run_length_encode_by : (((a, a) -> Bool), [a]) -> [(Int, a)]
// run_length_encode_by((==),[1,2,2,2,2,3,3,2)) == [(1,1),(4,2),(2,3),(1,2)]
template <typename BinaryPredicate,
        typename ContainerIn,
        typename T = typename ContainerIn::value_type,
        typename ContainerOut =
            typename std::vector<std::pair<std::size_t, T>>>
ContainerOut run_length_encode_by(BinaryPredicate pred, const ContainerIn& xs)
{
    check_binary_predicate_for_container<BinaryPredicate, ContainerIn>();
    ContainerOut result;
    auto groups = group_by(pred, xs);
    auto group_to_pair = [](const ContainerIn& group )
    {
        return std::make_pair(size_of_cont(group), group.front());
    };
    return transform(group_to_pair, groups);
}

// API search type: run_length_encode : [a] -> [(Int, a)]
// run_length_encode([1,2,2,2,2,3,3,2)) == [(1,1),(4,2),(2,3),(1,2)]
template <typename ContainerIn,
        typename T = typename ContainerIn::value_type,
        typename ContainerOut =
            typename std::vector<std::pair<std::size_t, T>>>
ContainerOut run_length_encode(const ContainerIn& xs)
{
    return run_length_encode_by(is_equal_by(identity<T>), xs);
}

// API search type: run_length_decode : [(Int, a)] -> [a]
// run_length_decode([(1,1),(4,2),(2,3),(1,2)]) == [1,2,2,2,2,3,3,2)
template <typename ContainerIn,
        typename Pair = typename ContainerIn::value_type,
        typename Cnt = typename Pair::first_type,
        typename T = typename Pair::second_type,
        typename ContainerOut = typename std::vector<T>>
ContainerOut run_length_decode(const ContainerIn& pairs)
{
    static_assert(std::is_convertible<Cnt, std::size_t>::value,
        "Count type must be convertible to std::size_t.");
    auto pair_to_vec = apply_to_pair(replicate<T>);
    return concat(transform(pair_to_vec, pairs));
}

// API search type: take_while : ((a -> Bool), [a]) -> [a]
// take_while(is_even, [0,2,4,5,6,7,8]) == [0,2,4]
template <typename Container, typename UnaryPredicate>
Container take_while(UnaryPredicate pred, const Container& xs)
{
    auto maybeIdx = find_first_idx_by(logical_not(pred), xs);
    return take(just_with_default<std::size_t>(
        size_of_cont(xs), maybeIdx), xs);
}

// API search type: drop_while : ((a -> Bool), [a]) -> [a]
// drop_while(is_even, [0,2,4,5,6,7,8]) == [5,6,7,8]
template <typename Container, typename UnaryPredicate>
Container drop_while(UnaryPredicate pred, const Container& xs)
{
    auto maybeIdx = find_first_idx_by(logical_not(pred), xs);
    return drop(just_with_default<std::size_t>(
        size_of_cont(xs), maybeIdx), xs);
}

// API search type: span : ((a -> Bool), [a]) -> ([a], [a])
// span, applied to a predicate p and a list xs,
// returns a tuple where first element is longest prefix (possibly empty)
// of xs of elements that satisfy p
// and second element is the remainder of the list.
// span(is_even, [0,2,4,5,6,7,8]) == ([0,2,4], [5,6,7,8])
template <typename Container, typename UnaryPredicate>
std::pair<Container, Container> span(UnaryPredicate pred, const Container& xs)
{
    auto maybeIdx = find_first_idx_by(logical_not(pred), xs);
    return {
        take(just_with_default<std::size_t>(size_of_cont(xs), maybeIdx), xs),
        drop(just_with_default<std::size_t>(size_of_cont(xs), maybeIdx), xs)
    };
}

} // namespace fplus
