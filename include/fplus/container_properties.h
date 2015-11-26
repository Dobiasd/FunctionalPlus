// Copyright Tobias Hermann 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <algorithm>

#include "container_common.h"
#include "compare.h"
#include "generate.h"
#include "maybe.h"
#include "search.h"
#include "composition.h"

namespace fplus
{

// AllBy(isEven, [2, 4, 6]) == true
// Returns true for empty containers.
template <typename UnaryPredicate, typename Container>
bool AllBy(UnaryPredicate p, const Container& xs)
{
    CheckUnaryPredicateForContainer<UnaryPredicate, Container>();
    return std::all_of(std::begin(xs), std::end(xs), p);
}

// All([true, false, true]) == false
// Returns true for empty containers.
template <typename Container>
bool All(const Container& xs)
{
    typedef typename Container::value_type T;
    return AllBy(Identity<T>, xs);
}

// AnyBy(isOdd, [2, 4, 6]) == false
template <typename UnaryPredicate, typename Container>
bool AnyBy(UnaryPredicate p, const Container& xs)
{
    CheckUnaryPredicateForContainer<UnaryPredicate, Container>();
    return std::any_of(std::begin(xs), std::end(xs), p);
}

// Any([false, true, false]) == true
template <typename Container>
bool Any(const Container& xs)
{
    typedef typename Container::value_type T;
    return AnyBy(Identity<T>, xs);
}

// None(isEven, [3, 4, 5]) == false
template <typename UnaryPredicate, typename Container>
bool NoneBy(UnaryPredicate p, const Container& xs)
{
    CheckUnaryPredicateForContainer<UnaryPredicate, Container>();
    return std::none_of(std::begin(xs), std::end(xs), p);
}

// None([false, true, false]) == false
template <typename Container>
bool None(const Container& xs)
{
    typedef typename Container::value_type T;
    return NoneBy(Identity<T>, xs);
}

// MinimumBy(length, ["123", "12", "1234", "123"]) -> "12"
template <typename Compare, typename Container>
typename Container::value_type MinimumBy(Compare comp,
        const Container& xs)
{
    CheckCompareForContainer<Compare, Container>();
    assert(IsNotEmpty(xs));
    return *std::min_element(std::begin(xs), std::end(xs), comp);
}

// MaximumBy(length, ["123", "12", "1234", "123"]) == "1234"
template <typename Compare, typename Container>
typename Container::value_type MaximumBy(Compare comp,
        const Container& xs)
{
    CheckCompareForContainer<Compare, Container>();
    assert(IsNotEmpty(xs));
    return *std::max_element(std::begin(xs), std::end(xs), comp);
}


// Minimum([3, 1, 4, 2]) == 1
template <typename Container>
typename Container::value_type Minimum(const Container& xs)
{
    assert(IsNotEmpty(xs));
    return *std::min_element(std::begin(xs), std::end(xs));
}

// Maximum([3, 1, 4, 2]) == 4
template <typename Container>
typename Container::value_type Maximum(const Container& xs)
{
    assert(IsNotEmpty(xs));
    return *std::max_element(std::begin(xs), std::end(xs));
}

// Sum([1, 2, 3]) -> 7
template <typename Container>
typename Container::value_type Sum(const Container& xs)
{
    typedef typename Container::value_type X;
    return Foldl([](const X& acc, const X& x) { return acc + x; }, X(), xs);
}

// Mean([1, 4, 4]) == 3
template <typename Result, typename Container>
Result Mean(const Container& xs)
{
    return static_cast<Result>(Sum(xs)) / Size(xs);
}

// Median([5, 6, 4, 3, 2, 6, 7, 9, 3]) == 5
template <typename X>
X Median(std::vector<X> xs)
{
    assert(IsNotEmpty(xs));

    if (Size(xs) == 1)
        return xs.front();

    auto xsSorted = Sort(xs);
    if (Size(xsSorted) % 2 == 1)
    {
        return xsSorted[Size(xsSorted) / 2];
    }
    else
    {
        std::list<X> upperAndLower = {
            xsSorted[Size(xsSorted) / 2 - 1],
            xsSorted[Size(xsSorted) / 2]
        };
        return Mean<X>(upperAndLower);
    }
}

// Returns true for empty containers.
template <typename Container, typename BinaryPredicate>
bool AllTheSameBy(BinaryPredicate p, const Container& xs)
{
    CheckBinaryPredicateForContainer<BinaryPredicate, Container>();
    if (Size(xs) < 2)
        return true;
    auto unaryPredicate = Bind1of2(p, xs.front());
    return AllBy(unaryPredicate, xs);
}

// Returns true for empty containers.
template <typename Container>
bool AllTheSame(const Container& xs)
{
    typedef typename Container::value_type T;
    auto binaryPredicate = std::equal_to<T>();
    return AllTheSameBy(binaryPredicate, xs);
}

// Returns true for empty containers.
// O(n^2)
template <typename Container, typename BinaryPredicate>
bool AllUniqueByEq(BinaryPredicate p, const Container& xs)
{
    CheckBinaryPredicateForContainer<BinaryPredicate, Container>();
    return Size(NubBy(p, xs)) == Size(xs);
}

// Returns true for empty containers.
// O(n^2)
template <typename Container>
bool AllUniqueEq(const Container& xs)
{
    typedef typename Container::value_type T;
    auto comp = std::equal_to<T>();
    return AllUniqueByEq(comp, xs);
}

// Returns true for empty containers.
// O(n*log(n))
template <typename Container, typename Compare>
bool AllUniqueByLess(Compare comp, const Container& xs)
{
    CheckCompareForContainer<Compare, Container>();
    if (Size(xs) < 2)
        return true;
    return Size(Unique(SortBy(comp, xs))) == Size(xs);
}

// Returns true for empty containers.
// O(n*log(n))
template <typename Container>
bool AllUniqueLess(const Container& xs)
{
    typedef typename Container::value_type T;
    auto comp = std::less<T>();
    return AllUniqueByLess(comp, xs);
}

// comp(a, b) must return true only if a < b.
// O(n)
template <typename Container, typename Compare>
bool IsSortedBy(Compare comp, const Container& xs)
{
    CheckCompareForContainer<Compare, Container>();
    if (Size(xs) < 2)
        return true;
    auto it1 = std::begin(xs);
    for (auto it2 = it1 + 1; it2 < std::end(xs); ++it1, ++it2)
        if (!comp(*it1, *it2))
            return false;
    return true;
}

// O(n)
template <typename Container>
bool IsSorted(const Container& xs)
{
    typedef typename Container::value_type T;
    auto comp = std::less_equal<T>();
    return IsSortedBy(comp, xs);
}

// IsInfixOf("ion", "FunctionalPlus") == true
template <typename Container>
bool IsInfixOf(const Container& token, Container& xs)
{
    return Contains(token, Infixes<std::vector<Container>>(Size(token), xs));
}

// IsInfixOf("Fun", "FunctionalPlus") == true
template <typename Container>
bool IsPrefixOf(const Container& token, Container& xs)
{
    if (Size(token) > Size(xs))
        return false;
    return GetRange(0, Size(token), xs) == token;
}

// IsInfixOf("us", "FunctionalPlus") == true
template <typename Container>
bool isSuffixOf(const Container& token, Container& xs)
{
    if (Size(token) > Size(xs))
        return false;
    return GetRange(Size(xs) - Size(token), Size(xs), xs) == token;
}

// IsSubsequenceOf("Final", "FunctionalPlus") == true
template <typename Container>
bool IsSubsequenceOf(const Container& token, Container& xs)
{
    if (IsEmpty(token))
        return true;
    if (Size(token) > Size(xs))
        return false;
    typedef typename Container::value_type T;
    auto remaining = Convert<std::list<T>>(token);
    for (const auto& x : xs)
    {
        if (x == remaining.front())
        {
            remaining.pop_front();
            if (IsEmpty(remaining))
                return true;
        }
    }
    return false;
}

// CountIf(isEven, [1, 2, 3, 5, 7, 8]) == 2
template <typename UnaryPredicate, typename Container>
std::size_t CountIf(UnaryPredicate p, const Container& xs)
{
    CheckUnaryPredicateForContainer<UnaryPredicate, Container>();
    return Size(FindAllIdxsBy(p, xs));
}

// Count(2, [1, 2, 3, 5, 7, 2, 2]) == 3
template <typename Container>
std::size_t Count
        (const typename Container::value_type& x, const Container& xs)
{
    return Size(FindAllIdxsOf(x, xs));
}

} // namespace fplus