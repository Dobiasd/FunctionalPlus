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

// all_by(is_even, [2, 4, 6]) == true
// Returns true for empty containers.
template <typename UnaryPredicate, typename Container>
bool all_by(UnaryPredicate p, const Container& xs)
{
    check_unary_predicate_for_container<UnaryPredicate, Container>();
    return std::all_of(std::begin(xs), std::end(xs), p);
}

// all([true, false, true]) == false
// Returns true for empty containers.
template <typename Container>
bool all(const Container& xs)
{
    typedef typename Container::value_type T;
    return all_by(identity<T>, xs);
}

// any_by(is_odd, [2, 4, 6]) == false
template <typename UnaryPredicate, typename Container>
bool any_by(UnaryPredicate p, const Container& xs)
{
    check_unary_predicate_for_container<UnaryPredicate, Container>();
    return std::any_of(std::begin(xs), std::end(xs), p);
}

// any([false, true, false]) == true
template <typename Container>
bool any(const Container& xs)
{
    typedef typename Container::value_type T;
    return any_by(identity<T>, xs);
}

// none_by(is_even, [3, 4, 5]) == false
template <typename UnaryPredicate, typename Container>
bool none_by(UnaryPredicate p, const Container& xs)
{
    check_unary_predicate_for_container<UnaryPredicate, Container>();
    return std::none_of(std::begin(xs), std::end(xs), p);
}

// none([false, true, false]) == false
template <typename Container>
bool none(const Container& xs)
{
    typedef typename Container::value_type T;
    return none_by(identity<T>, xs);
}

// minimum_by(length, ["123", "12", "1234", "123"]) -> "12"
template <typename Compare, typename Container>
typename Container::value_type minimum_by(Compare comp,
        const Container& xs)
{
    check_compare_for_container<Compare, Container>();
    assert(is_not_empty(xs));
    return *std::min_element(std::begin(xs), std::end(xs), comp);
}

// maximum_by(length, ["123", "12", "1234", "123"]) == "1234"
template <typename Compare, typename Container>
typename Container::value_type maximum_by(Compare comp,
        const Container& xs)
{
    check_compare_for_container<Compare, Container>();
    assert(is_not_empty(xs));
    return *std::max_element(std::begin(xs), std::end(xs), comp);
}


// minimum([3, 1, 4, 2]) == 1
template <typename Container>
typename Container::value_type minimum(const Container& xs)
{
    assert(is_not_empty(xs));
    return *std::min_element(std::begin(xs), std::end(xs));
}

// maximum([3, 1, 4, 2]) == 4
template <typename Container>
typename Container::value_type maximum(const Container& xs)
{
    assert(is_not_empty(xs));
    return *std::max_element(std::begin(xs), std::end(xs));
}

// sum([1, 2, 3]) == 7
template <typename Container>
typename Container::value_type sum(const Container& xs)
{
    typedef typename Container::value_type X;
    return fold_left([](const X& acc, const X& x) { return acc+x; }, X(), xs);
}

// mean([1, 4, 4]) == 3
template <typename Result, typename Container>
Result mean(const Container& xs)
{
    return static_cast<Result>(sum(xs)) / size_of_cont(xs);
}

// median([5, 6, 4, 3, 2, 6, 7, 9, 3]) == 5
template <typename X>
X median(std::vector<X> xs)
{
    assert(is_not_empty(xs));

    if (size_of_cont(xs) == 1)
        return xs.front();

    auto xsSorted = sort(xs);
    if (size_of_cont(xsSorted) % 2 == 1)
    {
        return xsSorted[size_of_cont(xsSorted) / 2];
    }
    else
    {
        std::list<X> upperAndLower = {
            xsSorted[size_of_cont(xsSorted) / 2 - 1],
            xsSorted[size_of_cont(xsSorted) / 2]
        };
        return mean<X>(upperAndLower);
    }
}

// Returns true for empty containers.
template <typename Container, typename BinaryPredicate>
bool all_the_same_by(BinaryPredicate p, const Container& xs)
{
    check_binary_predicate_for_container<BinaryPredicate, Container>();
    if (size_of_cont(xs) < 2)
        return true;
    auto unaryPredicate = bind_1st_of_2(p, xs.front());
    return all_by(unaryPredicate, xs);
}

// Returns true for empty containers.
template <typename Container>
bool all_the_same(const Container& xs)
{
    typedef typename Container::value_type T;
    auto binaryPredicate = std::equal_to<T>();
    return all_the_same_by(binaryPredicate, xs);
}

// Returns true for empty containers.
// O(n^2)
template <typename Container, typename BinaryPredicate>
bool all_unique_by_eq(BinaryPredicate p, const Container& xs)
{
    check_binary_predicate_for_container<BinaryPredicate, Container>();
    return size_of_cont(nub_by(p, xs)) == size_of_cont(xs);
}

// Returns true for empty containers.
// O(n^2)
template <typename Container>
bool all_unique(const Container& xs)
{
    typedef typename Container::value_type T;
    auto comp = std::equal_to<T>();
    return all_unique_by_eq(comp, xs);
}

// Returns true for empty containers.
// O(n*log(n))
template <typename Container, typename Compare>
bool all_unique_by_less(Compare comp, const Container& xs)
{
    check_compare_for_container<Compare, Container>();
    if (size_of_cont(xs) < 2)
        return true;
    return size_of_cont(unique(sort_by(comp, xs))) == size_of_cont(xs);
}

// Returns true for empty containers.
// O(n*log(n))
template <typename Container>
bool all_unique_less(const Container& xs)
{
    typedef typename Container::value_type T;
    auto comp = std::less<T>();
    return all_unique_by_less(comp, xs);
}

// comp(a, b) must return true only if a < b.
// O(n)
template <typename Container, typename Compare>
bool is_sorted_by(Compare comp, const Container& xs)
{
    check_compare_for_container<Compare, Container>();
    if (size_of_cont(xs) < 2)
        return true;
    auto it1 = std::begin(xs);
    for (auto it2 = it1 + 1; it2 < std::end(xs); ++it1, ++it2)
        if (!comp(*it1, *it2))
            return false;
    return true;
}

// O(n)
template <typename Container>
bool is_sorted(const Container& xs)
{
    typedef typename Container::value_type T;
    auto comp = std::less_equal<T>();
    return is_sorted_by(comp, xs);
}

// is_infix_of("ion", "FunctionalPlus") == true
template <typename Container>
bool is_infix_of(const Container& token, Container& xs)
{
    return is_not_empty(find_all_instances_of(token, xs));
}

// is_prefix_of("Fun", "FunctionalPlus") == true
template <typename Container>
bool is_prefix_of(const Container& token, Container& xs)
{
    if (size_of_cont(token) > size_of_cont(xs))
        return false;
    return get_range(0, size_of_cont(token), xs) == token;
}

// is_suffix_of("us", "FunctionalPlus") == true
template <typename Container>
bool is_suffix_of(const Container& token, Container& xs)
{
    if (size_of_cont(token) > size_of_cont(xs))
        return false;
    return get_range(size_of_cont(xs) - size_of_cont(token),
        size_of_cont(xs), xs) == token;
}

// is_subsequence_of("Final", "FunctionalPlus") == true
template <typename Container>
bool is_subsequence_of(const Container& token, Container& xs)
{
    if (is_empty(token))
        return true;
    if (size_of_cont(token) > size_of_cont(xs))
        return false;
    typedef typename Container::value_type T;
    auto remaining = convert<std::list<T>>(token);
    for (const auto& x : xs)
    {
        if (x == remaining.front())
        {
            remaining.pop_front();
            if (is_empty(remaining))
                return true;
        }
    }
    return false;
}

// count_if(is_even, [1, 2, 3, 5, 7, 8]) == 2
template <typename UnaryPredicate, typename Container>
std::size_t count_if(UnaryPredicate p, const Container& xs)
{
    check_unary_predicate_for_container<UnaryPredicate, Container>();
    return size_of_cont(find_all_idxs_by(p, xs));
}

// count(2, [1, 2, 3, 5, 7, 2, 2]) == 3
template <typename Container>
std::size_t count
        (const typename Container::value_type& x, const Container& xs)
{
    return size_of_cont(find_all_idxs_of(x, xs));
}

} // namespace fplus