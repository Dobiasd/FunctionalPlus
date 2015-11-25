// Copyright Tobias Hermann 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "ContainerCommon.h"
#include "Search.h"
#include "Pairs.h"

namespace FunctionalPlus
{

// ContainerOut is not deduced to
// SameContNewType(ContainerIn, ContainerIn)
// here, since ContainerIn could be a std::string.

// GroupBy((==), [1,2,2,2,3,2,2,4,5,5]) == [[1],[2,2,2],[3],[2,2],4,[5,5]]
template <typename BinaryPredicate, typename ContainerIn,
        typename ContainerOut = typename std::list<ContainerIn>>
ContainerOut GroupBy(BinaryPredicate p, const ContainerIn& xs)
{
    CheckBinaryPredicateForContainer<BinaryPredicate, ContainerIn>();
    static_assert(std::is_same<ContainerIn, typename ContainerOut::value_type>::value, "Containers do not match.");
    ContainerOut result;
    if (IsEmpty(xs))
        return result;
    typedef typename ContainerOut::value_type InnerContainerOut;
    *BackInserter(result) = InnerContainerOut(1, xs.front());
    for (auto it = ++std::begin(xs); it != std::end(xs); ++it)
    {
        if (p(*it, result.back().front()))
            *BackInserter(result.back()) = *it;
        else
            *BackInserter(result) = InnerContainerOut(1, *it);
    }
    return result;
}

// Group([1,2,2,2,3,2,2,4,5,5]) == [[1],[2,2,2],[3],[2,2],4,[5,5]]
template <typename ContainerIn,
        typename ContainerOut = typename std::list<ContainerIn>>
ContainerOut Group(ContainerIn& xs)
{
    static_assert(std::is_same<ContainerIn, typename ContainerOut::value_type>::value, "Containers do not match.");
    typedef typename ContainerIn::value_type T;
    auto pred = [](const T& x, const T& y) { return x == y; };
    return GroupBy(pred, xs);
}

// SplitBy(isEven, true, [1,3,2,2,5,5,3,6,7,9]) == [[1,3],[],[5,5,3],[7,9]]
template <typename UnaryPredicate, typename ContainerIn,
        typename ContainerOut = typename std::list<ContainerIn>>
ContainerOut SplitBy
        (UnaryPredicate pred, bool allowEmpty, const ContainerIn& xs)
{
    typedef typename ContainerIn::value_type T;
    typedef typename ContainerOut::value_type ContainerOutInner;
    CheckUnaryPredicateForContainer<UnaryPredicate, ContainerIn>();
    static_assert(std::is_same<ContainerIn, typename ContainerOut::value_type>::value, "Containers do not match.");
    ContainerOut result;
    auto itOut = BackInserter(result);
    ContainerOutInner current;
    auto itOutCurrent = BackInserter(current);
    for (const T& x : xs)
    {
        if (pred(x))
        {
            if (IsNotEmpty(current) || allowEmpty)
            {
                *itOut = current;
                current.clear();
                itOutCurrent = BackInserter(current);
            }
        }
        else
        {
            *itOutCurrent = x;
        }
    }
    if (IsNotEmpty(current) || allowEmpty)
    {
        *itOut = current;
    }
    return result;
}

// SplitAtIdx(2, [0,1,2,3,4]) == ([0,1],[2,3,4])
template <typename Container>
std::pair<Container, Container> SplitAtIdx
        (std::size_t idx, const Container& xs)
{
    assert(idx <= Size(xs));
    return make_pair(GetRange(0, idx, xs), GetRange(idx, Size(xs), xs));
}

// Partition(isEven, [0,1,1,3,7,2,3,4]) == ([0,2,4],[1,1,3,7,3])
template <typename UnaryPredicate, typename Container>
std::pair<Container, Container> Partition
        (UnaryPredicate pred, const Container& xs)
{
    CheckUnaryPredicateForContainer<UnaryPredicate, Container>();
    Container matching;
    Container notMatching;
    auto itOutMatching = BackInserter(matching);
    auto itOutNotMatching = BackInserter(notMatching);
    for (auto& x : xs)
    {
        if (pred(x))
            *itOutMatching = x;
        else
            *itOutNotMatching = x;
    }
    return make_pair(matching, notMatching);
}

// SplitAtIdxs([2,5], [0,1,2,3,4,5,6,7]) == [[0,1],[2,3,4],[5,6,7]]
template <typename ContainerIdxs, typename ContainerIn,
        typename ContainerOut = std::vector<ContainerIn>>
ContainerOut SplitAtIdxs(const ContainerIdxs& idxsIn, const ContainerIn& xs)
{
    static_assert(std::is_same<ContainerIn, typename ContainerOut::value_type>::value, "Containers do not match.");
    ContainerIdxs idxStartC = {0};
    ContainerIdxs idxEndC = {Size(xs)};
    std::vector<ContainerIdxs> containerIdxss = {idxStartC, idxsIn, idxEndC};
    auto idxs = Concat(containerIdxss);
    auto idxsClean = Unique(Sort(idxs));
    ContainerOut result;
    PrepareContainer(result, Size(idxsClean) + 1);
    auto itOut = BackInserter(result);
    typedef std::vector<std::pair<std::size_t, std::size_t>> PositionPairs;
    auto idxPairs = OverlappingPairs<PositionPairs>(idxsClean);
    for (const auto& idxPair : idxPairs)
    {
        *itOut = GetRange(idxPair.first, idxPair.second, xs);
    }
    return result;
}

// SplitByToken(", ", "foo, bar, baz") == ["foo", "bar", "baz"]
template <typename ContainerIn,
        typename ContainerOut = typename std::list<ContainerIn>>
ContainerOut SplitByToken (const ContainerIn& token,
        bool allowEmpty, const ContainerIn& xs)
{
    static_assert(std::is_same<ContainerIn, typename ContainerOut::value_type>::value, "Containers do not match.");
    auto instances = FindAllInstancesOfNonOverlapping(token, xs);
    *BackInserter(instances) = Size(xs);
    std::list<ContainerIn> result;
    auto itOut = BackInserter(result);
    std::size_t lastEnd = 0;
    for (std::size_t idx : instances)
    {
        if (idx != lastEnd || allowEmpty)
        {
            *itOut = GetRange(lastEnd, idx, xs);
            lastEnd = idx + Size(token);
        }
    }
    return result;
}

// Take(3, [0,1,2,3,4,5,6,7]) == [0,1,2]
template <typename Container>
Container Take(std::size_t amount, const Container& xs)
{
    assert(amount <= Size(xs));
    return GetRange(0, amount, xs);
}

// Drop(3, [0,1,2,3,4,5,6,7]) == [3,4,5,6,7]
template <typename Container>
Container Drop(std::size_t amount, const Container& xs)
{
    assert(amount <= Size(xs));
    return GetRange(amount, Size(xs), xs);
}

// TakeWhile(isEven, [0,2,4,5,6,7,8]) == [0,2,4]
template <typename Container, typename UnaryPredicate>
Container TakeWhile(UnaryPredicate pred, const Container& xs)
{
    auto maybeIdx = FindFirstIdxBy(Not(pred), xs);
    return Take(WithDefault<std::size_t>(Size(xs), maybeIdx), xs);
}

// DropWhile(isEven, [0,2,4,5,6,7,8]) == [5,6,7,8]
template <typename Container, typename UnaryPredicate>
Container DropWhile(UnaryPredicate pred, const Container& xs)
{
    auto maybeIdx = FindFirstIdxBy(Not(pred), xs);
    return Drop(WithDefault<std::size_t>(Size(xs), maybeIdx), xs);
}

} // namespace FunctionalPlus