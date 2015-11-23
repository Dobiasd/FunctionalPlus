// Copyright Tobias Hermann 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <algorithm>

#include "ContainerCommon.h"
#include "Composition.h"
#include "Generate.h"
#include "Maybe.h"


namespace FunctionalPlus
{

// FindFirstBy(isEven, [1, 3, 4, 6, 9]) == Just(4)
// FindFirstBy(isEven, [1, 3, 5, 7, 9]) == Nothing
template <typename Container, typename UnaryPredicate,
    typename T = typename Container::value_type>
Maybe<T> FindFirstBy(UnaryPredicate pred, const Container& xs)
{
    CheckUnaryPredicateForContainer<UnaryPredicate, Container>();
    auto it = std::find_if(std::begin(xs), std::end(xs), pred);
    if (it == std::end(xs))
        return Nothing<T>();
    return Just<T>(*it);
}

// FindLastBy(isEven, [1, 3, 4, 6, 9]) == Just(6)
// FindLastBy(isEven, [1, 3, 5, 7, 9]) == Nothing
template <typename Container, typename UnaryPredicate,
    typename T = typename Container::value_type>
Maybe<T> FindLastBy(UnaryPredicate pred, const Container& xs)
{
    CheckUnaryPredicateForContainer<UnaryPredicate, Container>();
    return FindFirstBy(pred, Reverse(xs));
}

// FindFirstIdxBy(isEven, [1, 3, 4, 6, 9]) == Just(2)
// FindFirstIdxBy(isEven, [1, 3, 5, 7, 9]) == Nothing
template <typename Container, typename UnaryPredicate>
Maybe<std::size_t> FindFirstIdxBy
        (UnaryPredicate pred, const Container& xs)
{
    CheckUnaryPredicateForContainer<UnaryPredicate, Container>();
    auto it = std::find_if(std::begin(xs), std::end(xs), pred);
    if (it == std::end(xs))
        return Nothing<std::size_t>();
    return Just<std::size_t>(std::distance(std::begin(xs), it));
}

// FindLastIdxBy(isEven, [1, 3, 4, 6, 9]) == Just(3)
// FindLastIdxBy(isEven, [1, 3, 5, 7, 9]) == Nothing
template <typename Container, typename UnaryPredicate>
Maybe<std::size_t> FindLastIdxBy
        (UnaryPredicate pred, const Container& xs)
{
    CheckUnaryPredicateForContainer<UnaryPredicate, Container>();
    auto calcRevIdx = [&](std::size_t idx) {
        return Size(xs) - (idx + 1);
    };
    return Lift(calcRevIdx)
            (FindFirstIdxBy(pred, Reverse(xs)));
}

// FindFirstIdx(4, [1, 3, 4, 4, 9]) == Just(2)
// FindFirstIdx(4, [1, 3, 5, 7, 9]) == Nothing
template <typename Container>
Maybe<std::size_t> FindFirstIdx
        (const typename Container::value_type& x, const Container& xs)
{
    typedef typename Container::value_type T;
    auto pred = Bind1of2(IsEqual<T>, x);
    return FindFirstIdxBy(pred, xs);
}

// FindFirstIdx(4, [1, 3, 4, 4, 9]) == Just(3)
// FindFirstIdx(4, [1, 3, 5, 7, 9]) == Nothing
template <typename Container>
Maybe<std::size_t> FindLastIdx
        (const typename Container::value_type& x, const Container& xs)
{
    typedef typename Container::value_type T;
    auto pred = Bind1of2(IsEqual<T>, x);
    return FindLastIdxBy(pred, xs);
}

// FindAllIdxsBy(isEven, [1, 3, 4, 6, 9]) == [2, 3]
template <typename ContainerOut = std::list<std::size_t>,
        typename UnaryPredicate, typename Container>
ContainerOut FindAllIdxsBy(UnaryPredicate p, const Container& xs)
{
    CheckUnaryPredicateForContainer<UnaryPredicate, Container>();
    std::size_t idx = 0;
    ContainerOut result;
    auto itOut = BackInserter(result);
    for (const auto& x : xs)
    {
        if (p(x))
            *itOut = idx;
        ++idx;
    }
    return result;
}

// FindAllIdxsOf(4, [1, 3, 4, 4, 9]) == [2, 3]
template <typename ContainerOut = std::list<std::size_t>, typename Container>
ContainerOut FindAllIdxsOf
        (const typename Container::value_type& x, const Container& xs)
{
    typedef typename Container::value_type T;
    auto pred = Bind1of2(IsEqual<T>, x);
    return FindAllIdxsBy(pred, xs);
}

// FindAllInstancesOf("haha", "oh, hahaha!") == [4, 6]
template <typename ContainerOut = std::list<std::size_t>, typename Container>
ContainerOut FindAllInstancesOf(const Container& token, const Container& xs)
{
    typedef std::vector<Container> Containers;
    auto candidates = Infixes<Containers>(Size(token), xs);
    ContainerOut result;
    std::size_t idx = 0;
    auto outIt = BackInserter(result);
    for (const auto& candidate : candidates)
    {
        if (candidate == token)
            *outIt = idx;
        ++idx;
    }
    return result;
}

// FindAllInstancesOfNonOverlapping("haha", "oh, hahaha!") == [4]
template <typename ContainerOut = std::list<std::size_t>, typename Container>
ContainerOut FindAllInstancesOfNonOverlapping
        (const Container& token, const Container& xs)
{
    ContainerOut result;
    if (IsEmpty(token))
        return result;
    typedef typename Container::value_type T;
    auto tokenAsList = Convert<std::list<T>>(token);
    auto tokenRemaining = tokenAsList;
    std::size_t idx = 0;
    auto itOut = BackInserter(result);
    for (const T& x : xs)
    {
        if (x == tokenRemaining.front())
        {
            tokenRemaining.pop_front();
            if (IsEmpty(tokenRemaining))
            {
                *itOut = idx - (Size(token) - 1);
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

} // namespace FunctionalPlus