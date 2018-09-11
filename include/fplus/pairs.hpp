// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <fplus/container_common.hpp>
#include <fplus/function_traits.hpp>
#include <fplus/internal/invoke.hpp>
#include <fplus/internal/asserts/pairs.hpp>

#include <utility>

namespace fplus
{
// API search type: apply_to_pair : (((a, b) -> c), (a, b)) -> c
// fwd bind count: 1
// Apply binary function to parts of a pair.
template <typename F, typename FIn0, typename FIn1>
auto apply_to_pair(F f, const std::pair<FIn0, FIn1>& p)
{
    internal::trigger_static_asserts<internal::apply_to_pair_tag, F, FIn0, FIn1>();
    return internal::invoke(f, p.first, p.second);
}

// API search type: zip_with : (((a, b) -> c), [a], [b]) -> [c]
// fwd bind count: 2
// Zip two sequences using a binary function.
// zip_with((+), [1, 2, 3], [5, 6]) == [1+5, 2+6] == [6, 8]
template <typename ContainerIn1,
          typename ContainerIn2,
          typename F,
          typename X = typename ContainerIn1::value_type,
          typename Y = typename ContainerIn2::value_type,
          typename TOut = std::decay_t<internal::invoke_result_t<F, X, Y>>,
          typename ContainerOut = std::vector<TOut>>
ContainerOut zip_with(F f, const ContainerIn1& xs, const ContainerIn2& ys)
{
    internal::trigger_static_asserts<internal::zip_with_tag, F, X, Y>();
    ContainerOut result;
    std::size_t resultSize = std::min(size_of_cont(xs), size_of_cont(ys));
    internal::prepare_container(result, resultSize);
    auto itResult = internal::get_back_inserter(result);
    auto itXs = std::begin(xs);
    auto itYs = std::begin(ys);
    for (std::size_t i = 0; i < resultSize; ++i)
    {
        *itResult = internal::invoke(f, *itXs, *itYs);
        ++itXs;
        ++itYs;
    }
  return result;
}

// API search type: zip_with_3 : (((a, b, c) -> d), [a], [b], [c]) -> [c]
// fwd bind count: 3
// Zip three sequences using a ternary function.
// zip_with_3((+), [1, 2, 3], [5, 6], [1, 1]) == [7, 9]
template <
    typename ContainerIn1,
    typename ContainerIn2,
    typename ContainerIn3,
    typename F,
    typename X = typename ContainerIn1::value_type,
    typename Y = typename ContainerIn2::value_type,
    typename Z = typename ContainerIn3::value_type,
    typename TOut = std::decay_t<internal::invoke_result_t<F, X, Y, Z>>,
    typename ContainerOut = typename std::vector<TOut>>
ContainerOut zip_with_3(F f,
                        const ContainerIn1& xs,
                        const ContainerIn2& ys,
                        const ContainerIn3& zs)
{
    internal::trigger_static_asserts<internal::zip_with_3_tag, F, X, Y, Z>();
    static_assert(std::is_same<
        typename internal::same_cont_new_t<ContainerIn1, void>::type,
        typename internal::same_cont_new_t<ContainerIn2, void>::type>::value,
        "All three Containers must be of same outer type.");
    static_assert(std::is_same<
        typename internal::same_cont_new_t<ContainerIn2, void>::type,
        typename internal::same_cont_new_t<ContainerIn3, void>::type>::value,
        "All three Containers must be of same outer type.");
    ContainerOut result;
    std::size_t resultSize = std::min(size_of_cont(xs), size_of_cont(ys));
    internal::prepare_container(result, resultSize);
    auto itResult = internal::get_back_inserter(result);
    auto itXs = std::begin(xs);
    auto itYs = std::begin(ys);
    auto itZs = std::begin(zs);
    for (std::size_t i = 0; i < resultSize; ++i)
    {
        *itResult = internal::invoke(f, *itXs, *itYs, *itZs);
        ++itXs;
        ++itYs;
        ++itZs;
    }
    return result;
}

// API search type: zip_with_defaults : (((a, b) -> c), a, b, [a], [b]) -> [c]
// fwd bind count: 4
// Zip two sequences and using a binary function
// and extrapolate the shorter sequence with a default value.
// zip_with_defaults((+), 6, 7, [1,2,3], [1,2]) == [2,4,10]
// zip_with_defaults((+), 6, 7, [1,2], [1,2,3]) == [2,4,9]
template <
    typename ContainerIn1,
    typename ContainerIn2,
    typename F,
    typename X = typename ContainerIn1::value_type,
    typename Y = typename ContainerIn2::value_type>
auto zip_with_defaults(F f,
    const X& default_x,
    const Y& default_y,
    const ContainerIn1& xs,
    const ContainerIn2& ys)
{
    internal::trigger_static_asserts<internal::zip_with_tag, F, X, Y>();
    const auto size_xs = size_of_cont(xs);
    const auto size_ys = size_of_cont(ys);
    if (size_xs < size_ys)
    {
        const auto extended_xs = append(
            xs,
            replicate<X, ContainerIn1>(size_ys - size_xs, default_x));
        return zip_with(f, extended_xs, ys);
    }
    else if (size_xs > size_ys)
    {
        const auto extended_ys = append(
            ys,
            replicate<Y, ContainerIn2>(size_xs - size_ys, default_y));
        return zip_with(f, xs, extended_ys);
    }
    return zip_with(f, xs, ys);
}

// API search type: zip : ([a], [b]) -> [(a, b)]
// fwd bind count: 1
// Combine two sequences to one sequence of pairs.
// zip([1, 2, 3], [5, 6]) == [(1, 5), (2, 6)]
template <typename ContainerIn1, typename ContainerIn2,
    typename X = typename ContainerIn1::value_type,
    typename Y = typename ContainerIn2::value_type>
auto zip(const ContainerIn1& xs, const ContainerIn2& ys)
{
    auto MakePair = [](const X& x, const Y& y)
        { return std::make_pair(x, y); };
    return zip_with(MakePair, xs, ys);
}

// API search type: unzip : [(a, b)] -> ([a], [b])
// fwd bind count: 0
// Split a sequence of pairs into two sequences.
// unzip([(1, 5), (2, 6)]) == ([1, 2], [5, 6])
template <typename ContainerIn,
    typename TIn = typename ContainerIn::value_type,
    typename X = typename TIn::first_type,
    typename Y = typename TIn::second_type,
    typename ContainerOutX = typename internal::same_cont_new_t<ContainerIn, X>::type,
    typename ContainerOutY = typename internal::same_cont_new_t<ContainerIn, Y>::type>
std::pair<ContainerOutX, ContainerOutY> unzip(const ContainerIn& pairs)
{
    ContainerOutX firsts;
    ContainerOutY seconds;
    internal::prepare_container(firsts, size_of_cont(pairs));
    internal::prepare_container(seconds, size_of_cont(pairs));
    auto itFirsts = internal::get_back_inserter(firsts);
    auto itSeconds = internal::get_back_inserter(seconds);
    for (const auto& pair : pairs)
    {
        *itFirsts = pair.first;
        *itSeconds = pair.second;
    }
    return std::make_pair(firsts, seconds);
}

// API search type: fst : ((a, b)) -> a
// fwd bind count: 0
// Return the first element of a pair.
// fst((0, 1)) == 0
template <typename X, typename Y>
X fst(const std::pair<X, Y>& pair)
{
    return pair.first;
}

// API search type: snd : ((a, b)) -> b
// fwd bind count: 0
// Return the second element of a pair.
// snd((0, 1)) == 1
template <typename X, typename Y>
Y snd(const std::pair<X, Y>& pair)
{
    return pair.second;
}

// API search type: transform_fst : ((a -> c), (a, b)) -> (c, b)
// fwd bind count: 1
// Apply a function to the first element of a pair.
// transform_fst(square, (4, 5)) == (16, 5)
template <typename X, typename Y, typename F,
    typename ResultFirst = std::decay_t<internal::invoke_result_t<F, X>>>
std::pair<ResultFirst, Y> transform_fst(F f, const std::pair<X, Y>& pair)
{
    internal::trigger_static_asserts<internal::transform_fst_tag, F, X>();
    return std::make_pair(internal::invoke(f, pair.first), pair.second);
}

// API search type: transform_snd : ((b -> c), (a, b)) -> (a, c)
// fwd bind count: 1
// Apply a function to the second element of a pair.
// transform_snd(square, (4, 5)) == (4, 25)
template <typename X, typename Y, typename F,
    typename ResultSecond = std::decay_t<internal::invoke_result_t<F, Y>>>
std::pair<X, ResultSecond> transform_snd(F f, const std::pair<X, Y>& pair)
{
    internal::trigger_static_asserts<internal::transform_snd_tag, F, Y>();
    return std::make_pair(pair.first, internal::invoke(f, pair.second));
}

// API search type: transform_pair : ((a -> c), (b -> d), (a, b)) -> (c, d)
// fwd bind count: 2
// Apply functions the both parts of a pair.
// transform_pair(square, square, (4, 5)) == (16, 25)
template <
    typename X,
    typename Y,
    typename F,
    typename G,
    typename ResultFirst = std::decay_t<internal::invoke_result_t<F, X>>,
    typename ResultSecond = std::decay_t<internal::invoke_result_t<G, Y>>>
std::pair<ResultFirst, ResultSecond> transform_pair(F f,
                                                    G g,
                                                    const std::pair<X, Y>& pair)
{
    internal::trigger_static_asserts<internal::transform_fst_tag, F, X>();
    internal::trigger_static_asserts<internal::transform_snd_tag, G, Y>();
    return std::make_pair(internal::invoke(f, pair.first),
                          internal::invoke(g, pair.second));
}

// API search type: swap_pair_elems : (a, b) -> (b, a)
// fwd bind count: 0
// Swap the first and the second element of a pair.
// swap_pair_elems((3,4)) == (4,3)
template <typename X, typename Y>
std::pair<Y, X> swap_pair_elems(const std::pair<X, Y>& pair)
{
    return std::make_pair(pair.second, pair.first);
}

// API search type: swap_pairs_elems : [(a, b)] -> [(b, a)]
// fwd bind count: 0
// Swap the first and the second element of every pair in a sequence.
// swap_pairs_elems([(1,2), (3,4)]) == [(2,1), (4,3)]
template <typename ContainerIn,
    typename X = typename ContainerIn::value_type::first_type,
    typename Y = typename ContainerIn::value_type::second_type>
auto swap_pairs_elems(const ContainerIn& xs)
{
    return fplus::transform(swap_pair_elems<X, Y>, xs);
}

// API search type: adjacent_pairs : [a] -> [(a, a)]
// fwd bind count: 0
// Split a sequence into pairs of elements.
// adjacent_pairs([0,1,2,3,4]) == [(0,1),(2,3)]
template <typename Container,
    typename ContainerOut =
        typename internal::same_cont_new_t<Container,
            std::pair<
                typename Container::value_type,
                    typename Container::value_type>>::type>
ContainerOut adjacent_pairs(const Container& xs)
{
    typedef typename Container::value_type T;
    static_assert(std::is_convertible<
            std::pair<T, T>,
            typename ContainerOut::value_type>::value,
        "ContainerOut can not store pairs of elements from ContainerIn.");
    ContainerOut result;
    if (size_of_cont(xs) < 2)
        return result;
    const std::size_t out_size = size_of_cont(xs) / 2;
    internal::prepare_container(result, out_size);
    auto itOut = internal::get_back_inserter(result);
    auto it1 = std::begin(xs);
    auto it2 = it1;
    internal::advance_iterator(it2, 1);
    const auto it_source_end =
        internal::add_to_iterator(std::begin(xs), out_size + out_size);
    for (;;)
    {
        *itOut = std::make_pair(*it1, *it2);
        internal::advance_iterator(it1, 2);
        if (it1 == it_source_end)
            break;
        internal::advance_iterator(it2, 2);
    }
    return result;
}

// API search type: overlapping_pairs : [a] -> [(a, a)]
// fwd bind count: 0
// Zip a sequence with itself shifted one element.
// overlapping_pairs([0,1,2,3]) == [(0,1),(1,2),(2,3)]
template <typename Container,
    typename ContainerOut =
        typename internal::same_cont_new_t<Container,
            std::pair<
                typename Container::value_type,
                    typename Container::value_type>, -1>::type>
ContainerOut overlapping_pairs(const Container& xs)
{
    typedef typename Container::value_type T;
    static_assert(std::is_convertible<
            std::pair<T, T>,
            typename ContainerOut::value_type>::value,
        "ContainerOut can not store pairs of elements from ContainerIn.");
    ContainerOut result;
    if (size_of_cont(xs) < 2)
        return result;
    internal::prepare_container(result, size_of_cont(xs) - 1);
    auto itOut = internal::get_back_inserter(result);
    auto it1 = std::begin(xs);
    auto it2 = it1;
    internal::advance_iterator(it2, 1);
    for (; it2 != std::end(xs); ++it1, ++it2)
    {
        *itOut = std::make_pair(*it1, *it2);
    }
    return result;
}

// API search type: overlapping_pairs_cyclic : [a] -> [(a, a)]
// fwd bind count: 0
// Zip a sequence with itself shifted one element,
// finally zipping the last element with the first one.
// overlapping_pairs_cyclic([0,1,2,3]) == [(0,1),(1,2),(2,3),(3,0)]
template <typename Container,
    typename ContainerOut =
        typename internal::same_cont_new_t<Container,
            std::pair<
                typename Container::value_type,
                    typename Container::value_type>, 0>::type>
ContainerOut overlapping_pairs_cyclic(const Container& xs)
{
    typedef typename Container::value_type T;
    static_assert(std::is_convertible<
            std::pair<T, T>,
            typename ContainerOut::value_type>::value,
        "ContainerOut can not store pairs of elements from ContainerIn.");
    ContainerOut result;
    if (size_of_cont(xs) < 2)
        return result;
    internal::prepare_container(result, size_of_cont(xs));
    auto itOut = internal::get_back_inserter(result);
    auto it1 = std::begin(xs);
    auto it2 = it1;
    internal::advance_iterator(it2, 1);
    for (; it2 != std::end(xs); ++it1, ++it2)
    {
        *itOut = std::make_pair(*it1, *it2);
    }
    *itOut = std::make_pair(*it1, xs.front());
    return result;
}

// API search type: enumerate : [a] -> [(Int, a)]
// fwd bind count: 0
// Attach its index to every element of a sequence.
// enumerate([6,4,7,6]) == [(0, 6), (1, 4), (2, 7), (3, 6)]
template <typename Container>
auto enumerate(const Container& xs)
{
    return zip(all_idxs(xs), xs);
}

// API search type: inner_product_with : (((a, a) -> b), ((b, b) -> b), b, [a], [a]) -> b
// fwd bind count: 4
// Calculate the inner product of two sequences using custom operations.
// inner_product_with((+), (*), [1, 2, 3], [4, 5, 6]) == [32]
template <
    typename ContainerIn1,
    typename ContainerIn2,
    typename OP1,
    typename OP2,
    typename Acc,
    typename X = typename ContainerIn1::value_type,
    typename Y = typename ContainerIn2::value_type,
    typename OP2Out = internal::invoke_result_t<OP2, X, Y>>
auto inner_product_with(OP1 op1,
                        OP2 op2,
                        const Acc& value,
                        const ContainerIn1& xs,
                        const ContainerIn2& ys)
{
    internal::trigger_static_asserts<internal::inner_product_with_tag, OP2, X, Y>();
    internal::trigger_static_asserts<internal::inner_product_with_tag, OP1, Acc, OP2Out>();
    assert(size_of_cont(xs) == size_of_cont(ys));
    return std::inner_product(
        std::begin(xs), std::end(xs), std::begin(ys), value, op1, op2);
}

// API search type: inner_product : (a, [a], [a]) -> a
// fwd bind count: 2
// Calculate the inner product of two sequences.
// inner_product([1, 2, 3], [4, 5, 6]) == [32]
template <typename ContainerIn1, typename ContainerIn2,
    typename Z>
Z inner_product(const Z& value,
        const ContainerIn1& xs, const ContainerIn2& ys)
{
    assert(size_of_cont(xs) == size_of_cont(ys));

    return std::inner_product(
        std::begin(xs), std::end(xs), std::begin(ys), value);
}

// API search type: first_mismatch_idx_by : (((a, b) -> Bool), [a], [b]) -> Maybe Int
// fwd bind count: 2
// Find the first index at which the two sequences differ
// using a binary predicate.
// first_mismatch_idx_by((==), [1, 2, 3], [1, 4, 3]) == Just 1
// first_mismatch_idx_by((==), [1, 2, 3], [1, 4]) == Just 1
// first_mismatch_idx_by((==), [1, 2, 3], [1, 2]) == Nothing
// first_mismatch_idx_by((==), [], [1, 2]) == Nothing
template <typename ContainerIn1, typename ContainerIn2,
    typename BinaryPredicate>
maybe<std::size_t> first_mismatch_idx_by(BinaryPredicate p,
    const ContainerIn1& xs, const ContainerIn2& ys)
{
    auto itXs = std::begin(xs);
    auto itYs = std::begin(ys);
    std::size_t minSize = std::min(size_of_cont(xs), size_of_cont(ys));
    for (std::size_t i = 0; i < minSize; ++i)
    {
        if (!internal::invoke(p, *itXs, *itYs))
        {
            return just(i);
        }
        ++itXs;
        ++itYs;
    }
    return nothing<std::size_t>();
}

// API search type: first_mismatch_by : (((a, b) -> Bool), [a], [b]) -> Maybe (a, b)
// fwd bind count: 2
// Find the first pair of elements differing in the two sequences
// using a binary predicate.
// first_mismatch_by((==), [1, 2, 3], [1, 4, 3]) == Just (2, 4)
// first_mismatch_by((==), [1, 2, 3], [1, 4]) == Just (2, 4)
// first_mismatch_by((==), [1, 2, 3], [1, 2]) == Nothing
// first_mismatch_by((==), [], [1, 2]) == Nothing
template <typename ContainerIn1, typename ContainerIn2,
    typename BinaryPredicate,
    typename X = typename ContainerIn1::value_type,
    typename Y = typename ContainerIn2::value_type,
    typename TOut = std::pair<X, Y>>
maybe<TOut> first_mismatch_by(BinaryPredicate p,
    const ContainerIn1& xs, const ContainerIn2& ys)
{
    const auto maybe_idx = first_mismatch_idx_by(p, xs, ys);
    if (is_nothing(maybe_idx))
    {
        return nothing<TOut>();
    }
    else
    {
        const auto idx = maybe_idx.unsafe_get_just();
        return just(std::make_pair(
            elem_at_idx(idx, xs),
            elem_at_idx(idx, ys)));
    }
}

// API search type: first_mismatch_idx_on : ((a -> b), [a], [a]) -> Maybe Int
// fwd bind count: 2
// Find the first index of elements differing in the two sequences
// using a transformer.
// first_mismatch_idx_on((mod 2), [1, 2, 3], [3, 5, 3]) == 1
// first_mismatch_idx_on((mod 2), [1, 2, 3], [1, 5]) == 1
// first_mismatch_idx_on((mod 2), [1, 2, 3], [1, 6]) == Nothing
// first_mismatch_idx_on((mod 2), [], [1, 2]) == Nothing
template <typename ContainerIn1, typename ContainerIn2,
    typename F,
    typename X = typename ContainerIn1::value_type,
    typename Y = typename ContainerIn2::value_type,
    typename TOut = std::pair<X, Y>>
maybe<std::size_t> first_mismatch_idx_on(F f,
    const ContainerIn1& xs, const ContainerIn2& ys)
{
    static_assert(std::is_same<X, Y>::value,
        "Both containers must have the same element type.");
    return first_mismatch_idx_by(is_equal_by(f), xs, ys);
}

// API search type: first_mismatch_on : ((a -> b), [a], [a]) -> Maybe (a, a)
// fwd bind count: 2
// Find the first pair of elements differing in the two sequences
// using a transformer.
// first_mismatch_on((mod 2), [1, 2, 3], [3, 5, 3]) == Just (2, 5)
// first_mismatch_on((mod 2), [1, 2, 3], [1, 5]) == Just (2, 5)
// first_mismatch_on((mod 2), [1, 2, 3], [1, 6]) == Nothing
// first_mismatch_on((mod 2), [], [1, 2]) == Nothing
template <typename ContainerIn1, typename ContainerIn2,
    typename F,
    typename X = typename ContainerIn1::value_type,
    typename Y = typename ContainerIn2::value_type,
    typename TOut = std::pair<X, Y>>
maybe<TOut> first_mismatch_on(F f,
    const ContainerIn1& xs, const ContainerIn2& ys)
{
    static_assert(std::is_same<X, Y>::value,
        "Both containers must have the same element type.");
    return first_mismatch_by(is_equal_by(f), xs, ys);
}

// API search type: first_mismatch_idx : ([a], [a]) -> Maybe Int
// fwd bind count: 2
// Find the first index of elements differing in the two sequences.
// first_mismatch_idx((==), [1, 2, 3], [1, 4, 3]) == 1
// first_mismatch_idx((==), [1, 2, 3], [1, 4]) == 1
// first_mismatch_idx((==), [1, 2, 3], [1, 2]) == Nothing
// first_mismatch_idx((==), [], [1, 2]) == Nothing
template <typename ContainerIn1, typename ContainerIn2,
    typename X = typename ContainerIn1::value_type,
    typename Y = typename ContainerIn2::value_type>
maybe<std::size_t> first_mismatch_idx(
    const ContainerIn1& xs, const ContainerIn2& ys)
{
    static_assert(std::is_same<X, Y>::value,
        "Both containers must have the same element type.");
    return first_mismatch_idx_by(std::equal_to<X>(), xs, ys);
}

// API search type: first_mismatch : ([a], [a]) -> Maybe (a, a)
// fwd bind count: 2
// Find the first pair of elements differing in the two sequences
// first_mismatch((==), [1, 2, 3], [1, 4, 3]) == Just (2, 4)
// first_mismatch((==), [1, 2, 3], [1, 4]) == Just (2, 4)
// first_mismatch((==), [1, 2, 3], [1, 2]) == Nothing
// first_mismatch((==), [], [1, 2]) == Nothing
template <typename ContainerIn1, typename ContainerIn2,
    typename X = typename ContainerIn1::value_type,
    typename Y = typename ContainerIn2::value_type,
    typename TOut = std::pair<X, Y>>
maybe<TOut> first_mismatch(const ContainerIn1& xs, const ContainerIn2& ys)
{
    static_assert(std::is_same<X, Y>::value,
        "Both containers must have the same element type.");
    return first_mismatch_by(std::equal_to<X>(), xs, ys);
}

// API search type: first_match_idx_by : (((a, b) -> Bool), [a], [b]) -> Maybe Int
// fwd bind count: 2
// Find the first index at which the two sequences equal
// using a binary predicate.
// first_match_idx_by((==), [1, 2, 3], [3, 2, 3]) == Just 1
// first_match_idx_by((==), [], [1, 2]) == Nothing
template <typename ContainerIn1, typename ContainerIn2,
    typename F,
    typename X = typename ContainerIn1::value_type,
    typename Y = typename ContainerIn2::value_type>
maybe<std::size_t> first_match_idx_by(F f,
    const ContainerIn1& xs, const ContainerIn2& ys)
{
    auto itXs = std::begin(xs);
    auto itYs = std::begin(ys);
    std::size_t minSize = std::min(size_of_cont(xs), size_of_cont(ys));
    for (std::size_t i = 0; i < minSize; ++i)
    {
        if (internal::invoke(f, *itXs, *itYs))
        {
            return just(i);
        }
        ++itXs;
        ++itYs;
    }
    return nothing<std::size_t>();
}

// API search type: first_match_by : (((a, b) -> Bool), [a], [b]) -> Maybe (a, b)
// fwd bind count: 2
// Find the first pair of equal elements in the two sequences
// using a binary predicate.
// first_match_by((==), [1, 2, 3], [3, 2, 3]) == Just (2, 2)
// first_match_by((==), [], [1, 2]) == Nothing
template <typename ContainerIn1, typename ContainerIn2,
    typename F,
    typename X = typename ContainerIn1::value_type,
    typename Y = typename ContainerIn2::value_type,
    typename TOut = std::pair<X, Y>>
maybe<TOut> first_match_by(F f, const ContainerIn1& xs, const ContainerIn2& ys)
{
    const auto maybe_idx = first_match_idx_by(f, xs, ys);
    if (is_nothing(maybe_idx))
    {
        return nothing<TOut>();
    }
    else
    {
        const auto idx = maybe_idx.unsafe_get_just();
        return just(std::make_pair(
            elem_at_idx(idx, xs),
            elem_at_idx(idx, ys)));
    }
}

// API search type: first_match_idx_on : ((a -> b), [a], [a]) -> Maybe Int
// fwd bind count: 2
// Find the first index of equal elements in the two sequences
// using a transformer.
// first_match_idx_on((mod 2), [1, 2, 3], [2, 4, 3]) == 1
// first_match_idx_on((mod 2), [], [1, 2]) == Nothing
template <typename ContainerIn1, typename ContainerIn2,
    typename F,
    typename X = typename ContainerIn1::value_type,
    typename Y = typename ContainerIn2::value_type>
maybe<std::size_t> first_match_idx_on(F f,
    const ContainerIn1& xs, const ContainerIn2& ys)
{
    static_assert(std::is_same<X, Y>::value,
        "Both containers must have the same element type.");
    return first_match_idx_by(is_equal_by(f), xs, ys);
}

// API search type: first_match_on : ((a -> b), [a], [a]) -> Maybe (a, a)
// fwd bind count: 2
// Find the first pair of equal elements in the two sequences
// using a transformer.
// first_match_on((mod 2), [1, 2, 3], [2, 4, 3]) == Just (2, 4)
// first_match_on((mod 2), [], [1, 2]) == Nothing
template <typename ContainerIn1, typename ContainerIn2,
    typename F,
    typename X = typename ContainerIn1::value_type,
    typename Y = typename ContainerIn2::value_type,
    typename TOut = std::pair<X, Y>>
maybe<TOut> first_match_on(F f, const ContainerIn1& xs, const ContainerIn2& ys)
{
    static_assert(std::is_same<X, Y>::value,
        "Both containers must have the same element type.");
    return first_match_by(is_equal_by(f), xs, ys);
}

// API search type: first_match_idx : ([a], [a]) -> Maybe Int
// fwd bind count: 2
// Find the first index of equal elements in the two sequences.
// first_match_idx((==), [1, 2, 3], [5, 2, 3]) == 1
// first_match_idx((==), [], [1, 2]) == Nothing
template <typename ContainerIn1, typename ContainerIn2,
    typename X = typename ContainerIn1::value_type,
    typename Y = typename ContainerIn2::value_type>
maybe<std::size_t> first_match_idx(
    const ContainerIn1& xs, const ContainerIn2& ys)
{
    static_assert(std::is_same<X, Y>::value,
        "Both containers must have the same element type.");
    return first_match_idx_by(std::equal_to<X>(), xs, ys);
}

// API search type: first_match : ([a], [a]) -> Maybe (a, a)
// fwd bind count: 2
// Find the first pair of equal elements in the two sequences.
// first_match((==), [1, 2, 3], [5, 2, 3]) == Just (2, 2)
// first_match((==), [], [1, 2]) == Nothing
template <typename ContainerIn1, typename ContainerIn2,
    typename X = typename ContainerIn1::value_type,
    typename Y = typename ContainerIn2::value_type,
    typename TOut = std::pair<X, Y>>
maybe<TOut> first_match(const ContainerIn1& xs, const ContainerIn2& ys)
{
    static_assert(std::is_same<X, Y>::value,
        "Both containers must have the same element type.");
    return first_match_by(std::equal_to<X>(), xs, ys);
}

} // namespace fplus
