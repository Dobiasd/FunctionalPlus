// Copyright Tobias Hermann 2015.
// https://github.com/Dobiasd/FunctionalPlus
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
    return minimum_by(is_less<typename Container::value_type>, xs);
}

// maximum([3, 1, 4, 2]) == 4
template <typename Container>
typename Container::value_type maximum(const Container& xs)
{
    return maximum_by(is_less<typename Container::value_type>, xs);
}


// minimum_idx_by(length, ["123", "12", "1234", "123"]) -> "1"
// return index of first minimum element
template <typename Compare, typename Container>
typename Container::value_type minimum_idx_by(Compare comp,
        const Container& xs)
{
    check_compare_for_container<Compare, Container>();
    assert(is_not_empty(xs));
    return std::distance(std::begin(xs),
        std::min_element(std::begin(xs), std::end(xs), comp));
}

// maximum_idx_by(length, ["123", "12", "1234", "123"]) == "2"
// return index of first maximum element
template <typename Compare, typename Container>
typename Container::value_type maximum_idx_by(Compare comp,
        const Container& xs)
{
    check_compare_for_container<Compare, Container>();
    assert(is_not_empty(xs));
    return std::distance(std::begin(xs),
        std::max_element(std::begin(xs), std::end(xs), comp));
}

// minimum_idx([3, 1, 4, 2]) == 1
// return index of first minimum element
template <typename Container>
typename Container::value_type minimum_idx(const Container& xs)
{
    return minimum_idx_by(is_less<typename Container::value_type>, xs);
}

// maximum_idx([3, 1, 4, 2]) == 2
// return index of first maximum element
template <typename Container>
typename Container::value_type maximum_idx(const Container& xs)
{
    return maximum_idx_by(is_less<typename Container::value_type>, xs);
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
template <typename Container,
        typename Result = typename Container::value_type>
Result median(const Container& xs)
{
    typedef typename Container::value_type T;
    assert(is_not_empty(xs));

    if (size_of_cont(xs) == 1)
        return static_cast<Result>(xs.front());

    auto xsSorted = sort(xs);
    if (size_of_cont(xsSorted) % 2 == 1)
    {
        auto it = std::begin(xsSorted);
        std::advance(it, size_of_cont(xsSorted) / 2);
        return static_cast<Result>(*it);
    }
    else
    {
        auto it1 = std::begin(xsSorted);
        std::advance(it1, size_of_cont(xsSorted) / 2 - 1);
        auto it2 = it1;
        ++it2;
        std::vector<T> upperAndLower = { *it1, *it2 };
        return mean<Result, Container>(upperAndLower);
    }
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

// is_infix_of("ion", "FunctionalPlus") == true
template <typename Container>
bool is_infix_of(const Container& token, const Container& xs)
{
    return is_just(find_first_instance_of_token(token, xs));
}

// is_subsequence_of("Final", "FunctionalPlus") == true
template <typename Container>
bool is_subsequence_of(const Container& seq, const Container& xs)
{
    if (is_empty(seq))
        return true;
    if (size_of_cont(seq) > size_of_cont(xs))
        return false;
    typedef typename Container::value_type T;
    auto remaining = convert_container_and_elems<std::list<T>>(seq);
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
