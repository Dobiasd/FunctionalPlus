// Copyright Tobias Hermann 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "Maybe.h"

#include <algorithm>

namespace FunctionalPlus
{

// KeepIf(isEven, [1, 2, 3, 2, 4, 5]) == [2, 2, 4]
// Also known as Filter.
template <typename Pred, typename Container>
Container KeepIf(Pred pred, const Container& xs)
{
    CheckUnaryPredicateForContainer<Pred, Container>();
    Container result;
    auto it = BackInserter<Container>(result);
    std::copy_if(std::begin(xs), std::end(xs), it, pred);
    return result;
}

// DropIf(isEven, [1, 2, 3, 2, 4, 5]) == [1, 3, 5]
// Also known as Reject.
template <typename Pred, typename Container>
Container DropIf(Pred pred, const Container& xs)
{
    CheckUnaryPredicateForContainer<Pred, Container>();
    return KeepIf(Not(pred), xs);
}

// Without(0, [1, 0, 0, 5, 3, 0, 1]) == [1, 5, 3, 1]
template <typename Container,
    typename T = typename Container::value_type>
Container Without(T elem, const Container& xs)
{
    auto pred = Bind1of2(IsEqual<T>, elem);
    return DropIf(pred, xs);
}

// Predicate takes index and value.
// All elements fulfilling the predicate are kept.
template <typename Pred, typename Container>
Container KeepIfWithIdx(Pred pred, const Container& xs)
{
    CheckIndexWithTypePredicateForContainer<Pred, Container>();
    Container ys;
    auto it = BackInserter<Container>(ys);
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
Container DropIfWithIdx(Pred pred, const Container& xs)
{
    CheckIndexWithTypePredicateForContainer<Pred, Container>();
    return KeepIfWithIdx(Not(pred), xs);
}

// Predicate takes an index and decides if an element is kept.
template <typename UnaryPredicate, typename Container>
Container KeepByIdx(UnaryPredicate pred, const Container& xs)
{
    CheckUnaryPredicateForType<UnaryPredicate, std::size_t>();
    Container ys;
    auto it = BackInserter<Container>(ys);
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
Container DropByIdx(UnaryPredicate pred, const Container& xs)
{
    CheckUnaryPredicateForType<UnaryPredicate, std::size_t>();
    return KeepByIdx(Not(pred), xs);
}

// From a Container filled with Maybe<T> the nothings are dropped
// and the values inside the justs are returned in a new container.
template <typename ContainerIn,
    typename ContainerOut =
        typename SameContNewT<ContainerIn, typename ContainerIn::value_type::type>::type>
ContainerOut Justs(const ContainerIn& xs)
{
    typedef typename ContainerIn::value_type::type T;
    auto justsInMaybes = KeepIf(IsJust<T>, xs);
    ContainerOut ys;
    PrepareContainer(ys, Size(justsInMaybes));
    auto itOut = BackInserter<ContainerOut>(ys);
    std::transform(std::begin(justsInMaybes), std::end(justsInMaybes),
        itOut, UnsafeGetJust<T>);
    return ys;
}

// TrimLeft(isEven, [0,2,4,5,6,7,8,6,4]) == [5,6,7,8,6,4]
template <typename Container, typename UnaryPredicate>
Container TrimLeft(UnaryPredicate p, const Container& xs)
{
    CheckUnaryPredicateForContainer<UnaryPredicate, Container>();
    auto itFirstNot = std::find_if_not(std::begin(xs), std::end(xs), p);
    if (itFirstNot == std::end(xs))
        return Container();
    return Container(itFirstNot, std::end(xs));
}

// TrimRight(isEven, [0,2,4,5,6,7,8,6,4]) == [0,2,4,5,6,7]
template <typename Container, typename UnaryPredicate>
Container TrimRight(UnaryPredicate p, const Container& xs)
{
    CheckUnaryPredicateForContainer<UnaryPredicate, Container>();
    return Reverse(TrimLeft(p, Reverse(xs)));
}

// Trim(isEven, [0,2,4,5,6,7,8,6,4]) == [5,6,7]
template <typename Container, typename UnaryPredicate>
Container Trim(UnaryPredicate p, const Container& xs)
{
    CheckUnaryPredicateForContainer<UnaryPredicate, Container>();
    return TrimRight(p, TrimLeft(p, xs));
}

} // namespace FunctionalPlus