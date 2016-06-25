// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "fplus/container_common.h"
#include "fplus/compare.h"
#include "fplus/generate.h"
#include "fplus/maybe.h"
#include "fplus/numeric.h"
#include "fplus/search.h"
#include "fplus/composition.h"

#include <algorithm>
#include <type_traits>

namespace fplus
{

// API search type: any_by : ((a -> Bool), [a]) -> Bool
// any_by(is_odd, [2, 4, 6]) == false
template <typename UnaryPredicate, typename Container>
bool any_by(UnaryPredicate p, const Container& xs)
{
    check_unary_predicate_for_container<UnaryPredicate, Container>();
    return std::any_of(std::begin(xs), std::end(xs), p);
}

// API search type: any : [Bool] -> Bool
// any([false, true, false]) == true
template <typename Container>
bool any(const Container& xs)
{
    typedef typename Container::value_type T;
    return any_by(identity<T>, xs);
}

// API search type: none_by : ((a -> Bool), [a]) -> Bool
// none_by(is_even, [3, 4, 5]) == false
template <typename UnaryPredicate, typename Container>
bool none_by(UnaryPredicate p, const Container& xs)
{
    check_unary_predicate_for_container<UnaryPredicate, Container>();
    return std::none_of(std::begin(xs), std::end(xs), p);
}

// API search type: none : [Bool] -> Bool
// none([false, true, false]) == false
template <typename Container>
bool none(const Container& xs)
{
    typedef typename Container::value_type T;
    return none_by(identity<T>, xs);
}


// API search type: minimum_idx_by : [a] -> Int
// minimum_idx_by(lessLength, ["123", "12", "1234", "123"]) -> "1"
// return index of first minimum element
template <typename Compare, typename Container>
typename std::size_t minimum_idx_by(Compare comp,
        const Container& xs)
{
    check_compare_for_container<Compare, Container>();
    assert(is_not_empty(xs));
    return static_cast<std::size_t>(std::distance(std::begin(xs),
        std::min_element(std::begin(xs), std::end(xs), comp)));
}

// API search type: maximum_idx_by : [a] -> Int
// maximum_idx_by(lessLength, ["123", "12", "1234", "123"]) == "2"
// return index of first maximum element
template <typename Compare, typename Container>
typename std::size_t maximum_idx_by(Compare comp,
        const Container& xs)
{
    check_compare_for_container<Compare, Container>();
    assert(is_not_empty(xs));
    return static_cast<std::size_t>(std::distance(std::begin(xs),
        std::max_element(std::begin(xs), std::end(xs), comp)));
}


// API search type: minimum_idx : [a] -> Int
// minimum_idx([3, 1, 4, 2]) == 1
// return index of first minimum element
template <typename Container>
typename std::size_t minimum_idx(const Container& xs)
{
    return minimum_idx_by(is_less<typename Container::value_type>, xs);
}

// API search type: maximum_idx : [a] -> Int
// maximum_idx([3, 1, 4, 2]) == 2
// return index of first maximum element
template <typename Container>
typename std::size_t maximum_idx(const Container& xs)
{
    return maximum_idx_by(is_less<typename Container::value_type>, xs);
}


// API search type: minimum_idx_on : ((a -> b), [a]) -> Int
// minimum_idx_on(length, ["123", "12", "1234", "123"]) -> "1"
// return index of first minimum element
template <typename F, typename Container>
typename std::size_t minimum_idx_on(F f, const Container& xs)
{
    typedef typename utils::function_traits<F>::result_type Result;
    auto transformed = transform_convert<std::vector<Result>>(f, xs);
    return minimum_idx(transformed);
}

// API search type: maximum_idx_on : ((a -> b), [a]) -> Int
// maximum_idx_on(length, ["123", "12", "1234", "123"]) == "2"
// return index of first maximum element
template <typename F, typename Container>
typename std::size_t maximum_idx_on(F f, const Container& xs)
{
    typedef typename utils::function_traits<F>::result_type Result;
    auto transformed = transform_convert<std::vector<Result>>(f, xs);
    return maximum_idx(transformed);
}


// API search type: minimum_by : (((a, a) -> Bool), [a]) -> a
// minimum_by(lessLength, ["123", "12", "1234", "123"]) -> "12"
template <typename Compare, typename Container>
typename Container::value_type minimum_by(Compare comp,
        const Container& xs)
{
    check_compare_for_container<Compare, Container>();
    assert(is_not_empty(xs));
    return *std::min_element(std::begin(xs), std::end(xs), comp);
}

// API search type: maximum_by : (((a, a) -> Bool), [a]) -> a
// maximum_by(lessLength, ["123", "12", "1234", "123"]) == "1234"
template <typename Compare, typename Container>
typename Container::value_type maximum_by(Compare comp,
        const Container& xs)
{
    check_compare_for_container<Compare, Container>();
    assert(is_not_empty(xs));
    return *std::max_element(std::begin(xs), std::end(xs), comp);
}


// API search type: minimum : [a] -> a
// minimum([3, 1, 4, 2]) == 1
template <typename Container>
typename Container::value_type minimum(const Container& xs)
{
    return minimum_by(is_less<typename Container::value_type>, xs);
}

// API search type: maximum : [a] -> a
// maximum([3, 1, 4, 2]) == 4
template <typename Container>
typename Container::value_type maximum(const Container& xs)
{
    return maximum_by(is_less<typename Container::value_type>, xs);
}


// API search type: minimum_on : ((a -> b), [a]) -> a
// minimum_on(length, ["123", "12", "1234", "123"]) -> "12"
template <typename F, typename Container>
typename Container::value_type minimum_on(F f, const Container& xs)
{
    return elem_at_idx(minimum_idx_on(f, xs), xs);
}

// API search type: maximum_on : ((a -> b), [a]) -> a
// maximum_on(length, ["123", "12", "1234", "123"]) == "1234"
template <typename F, typename Container>
typename Container::value_type maximum_on(F f, const Container& xs)
{
    return elem_at_idx(maximum_idx_on(f, xs), xs);
}


// API search type: mean : [a] -> a
// mean([1, 4, 4]) == 3
// unsafe! Make sure sum(xs) does not overflow. see mean_using_doubles
template <typename Result, typename Container>
Result mean(const Container& xs)
{
    assert(size_of_cont(xs) != 0);
    typedef typename Container::value_type T;
    return static_cast<Result>(sum(xs) / static_cast<T>(size_of_cont(xs)));
}

// API search type: mean_obj : [a] -> a
// mean_obj([B 1, B 4, B 4]) == B 3
// The provided objects must support division by a std::size_t.
// unsafe! Make sure sum(xs) does not overflow.
template <typename Container,
    typename T = typename Container::value_type>
T mean_obj(const Container& xs)
{
    assert(size_of_cont(xs) != 0);
    return sum(xs) / size_of_cont(xs);
}

// API search type: mean_using_doubles : [a] -> a
// mean_using_doubles([1, 4, 4]) == 3
// Converts elements to double before calculating the sum to prevent overflows.
template <typename Result, typename Container>
Result mean_using_doubles(const Container& xs)
{
    auto size = size_of_cont(xs);
    assert(size != 0);
    auto xs_as_doubles = convert_elems<double>(xs);
    auto result_as_double = mean<double>(xs_as_doubles);
    if (!std::is_integral<Result>::value)
        return static_cast<Result>(result_as_double);
    else
        return round<Result>(result_as_double);
}

// API search type: median : [a] -> a
// median([5, 6, 4, 3, 2, 6, 7, 9, 3]) == 5
template <typename Container,
        typename Result = typename Container::value_type>
Result median(const Container& xs)
{
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
        return static_cast<Result>(*it1 + *it2) / static_cast<Result>(2);
    }
}

// API search type: all_unique_by_less : (((a, a) -> Bool), [a]) -> Bool
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

// API search type: all_unique_less : [a] -> Bool
// Returns true for empty containers.
// O(n*log(n))
template <typename Container>
bool all_unique_less(const Container& xs)
{
    typedef typename Container::value_type T;
    auto comp = std::less<T>();
    return all_unique_by_less(comp, xs);
}

// API search type: is_infix_of : ([a], [a]) -> Bool
// is_infix_of("ion", "FunctionalPlus") == true
template <typename Container>
bool is_infix_of(const Container& token, const Container& xs)
{
    return is_just(find_first_instance_of_token(token, xs));
}

// API search type: is_subsequence_of : ([a], [a]) -> Bool
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

// API search type: count_if : ((a -> Bool), [a]) -> Int
// count_if(is_even, [1, 2, 3, 5, 7, 8]) == 2
template <typename UnaryPredicate, typename Container>
std::size_t count_if(UnaryPredicate p, const Container& xs)
{
    check_unary_predicate_for_container<UnaryPredicate, Container>();
    return size_of_cont(find_all_idxs_by(p, xs));
}

// API search type: count : (a, [a]) -> Int
// count(2, [1, 2, 3, 5, 7, 2, 2]) == 3
template <typename Container>
std::size_t count
        (const typename Container::value_type& x, const Container& xs)
{
    return size_of_cont(find_all_idxs_of(x, xs));
}

// API search type: is_permutation_of : ([a], [a]) -> Bool
// Checks if one container is a permuation of the other one.
// is_permutation_of([2,3,1], [1,2,3]) == true
// O(log(n))
template <typename Container>
bool is_permutation_of(const Container& xs, const Container& ys)
{
    return size_of_cont(xs) == size_of_cont(ys) &&
        sort(xs) == sort(ys);
}

} // namespace fplus
