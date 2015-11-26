// Copyright Tobias Hermann 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "container_common.h"
#include "compare.h"
#include "split.h"

namespace fplus
{

// ReplaceIf(isEven, 0, [1, 3, 4, 6, 7]) == [1, 3, 0, 0, 7]
template <typename UnaryPredicate, typename Container>
Container ReplaceIf(UnaryPredicate p,
        const typename Container::value_type& dest, const Container& xs)
{
    CheckUnaryPredicateForContainer<UnaryPredicate, Container>();
    Container result;
    PrepareContainer(result, Size(xs));
    auto itOut = BackInserter(result);
    for (const auto& x : xs)
    {
        *itOut = p(x) ? dest : x;
    }
    return result;
}

// ReplaceElems(4, 0, [1, 3, 4, 4, 7]) == [1, 3, 0, 0, 7]
template <typename Container,
        typename T = typename Container::value_type>
Container ReplaceElems(const T& source, const T& dest, const Container& xs)
{
    return ReplaceIf(Bind1of2(IsEqual<T>, source), dest, xs);
}

// ReplaceTokens("haha", "hihi", "oh, hahaha!") == "oh, hihiha!"
template <typename Container>
Container ReplaceTokens
        (const Container& source, const Container& dest, const Container& xs)
{
    auto splitted = SplitByToken(source, true, xs);
    return Join(dest, splitted);
}

} // namespace fplus