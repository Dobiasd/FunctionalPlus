// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "fplus/container_common.hpp"
#include "fplus/function_traits.hpp"

#include <utility>

namespace fplus
{

// API search type: apply_to_pair : ((a, b) -> c) -> (((a, b)) -> c)
// Apply binary function to parts of a pair.
template <typename F,
    typename FIn0 = typename utils::function_traits<F>::template arg<0>::type,
    typename FIn1 = typename utils::function_traits<F>::template arg<1>::type,
    typename FuncRes = typename utils::function_traits<F>::result_type,
    typename ResFunc = typename std::function<FuncRes(const std::pair<FIn0, FIn1>&)>>
ResFunc apply_to_pair(F f)
{
    internal::check_arity<2, F>();
    return [f](const std::pair<FIn0, FIn1>& pair)
    {
        return f(pair.first, pair.second);
    };
}

// API search type: zip_with : (((a, b) -> c), [a], [b]) -> [c]
// zip_with((+), [1, 2, 3], [5, 6]) == [6, 8]
template <typename ContainerIn1, typename ContainerIn2, typename F,
    typename X = typename ContainerIn1::value_type,
    typename Y = typename ContainerIn2::value_type,
    typename TOut = typename utils::function_traits<F>::result_type,
    typename ContainerOut = typename std::vector<TOut>>
ContainerOut zip_with(F f,
        const ContainerIn1& xs, const ContainerIn2& ys)
{
    static_assert(utils::function_traits<F>::arity == 2,
        "Function must take two parameters.");
    typedef typename utils::function_traits<F>::result_type FOut;
    typedef typename utils::function_traits<F>::template arg<0>::type FIn0;
    typedef typename utils::function_traits<F>::template arg<1>::type FIn1;
    typedef typename ContainerIn1::value_type T1;
    typedef typename ContainerIn2::value_type T2;
    static_assert(std::is_convertible<T1, FIn0>::value,
        "Function does not take elements from first Container as first Parameter.");
    static_assert(std::is_convertible<T2, FIn1>::value,
        "Function does not take elements from second Container as second Parameter.");
    static_assert(std::is_convertible<FOut, TOut>::value,
        "Elements produced by this function can not be stored in ContainerOut.");
    static_assert(std::is_same<
        typename internal::same_cont_new_t<ContainerIn1, void>::type,
        typename internal::same_cont_new_t<ContainerIn2, void>::type>::value,
        "Both Containers must be of same outer type.");
    ContainerOut result;
    std::size_t resultSize = std::min(size_of_cont(xs), size_of_cont(ys));
    internal::prepare_container(result, resultSize);
    auto itResult = internal::get_back_inserter(result);
    auto itXs = std::begin(xs);
    auto itYs = std::begin(ys);
    for (std::size_t i = 0; i < resultSize; ++i)
    {
        *itResult = f(*itXs, *itYs);
        ++itXs;
        ++itYs;
    }
    return result;
}

// API search type: zip_with_defaults : (((a, b) -> c), a, b, [a], [b]) -> [c]
// zip_with_defaults((+), 6, 7, [1,2,3], [1,2]) == [2,4,10]
// zip_with_defaults((+), 6, 7, [1,2], [1,2,3]) == [2,4,9]
template <typename ContainerIn1, typename ContainerIn2, typename F,
    typename X = typename ContainerIn1::value_type,
    typename Y = typename ContainerIn2::value_type,
    typename TOut = typename utils::function_traits<F>::result_type,
    typename ContainerOut = typename std::vector<TOut>>
ContainerOut zip_with_defaults(F f,
        const X& default_x, const Y& default_y,
        const ContainerIn1& xs, const ContainerIn2& ys)
{
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
// zip([1, 2, 3], [5, 6]) == [(1, 5), (2, 6)]
template <typename ContainerIn1, typename ContainerIn2,
    typename X = typename ContainerIn1::value_type,
    typename Y = typename ContainerIn2::value_type,
    typename TOut = std::pair<X, Y>,
    typename ContainerOut = typename std::vector<TOut>>
ContainerOut zip(const ContainerIn1& xs, const ContainerIn2& ys)
{
    auto MakePair = [](const X& x, const Y& y)
            { return std::make_pair(x, y); };
    return zip_with(MakePair, xs, ys);
}

// API search type: unzip : [(a, b)] -> ([a], [b])
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
// fst((0, 1)) == 0
template <typename X, typename Y>
X fst(const std::pair<X, Y>& pair)
{
    return pair.first;
}

// API search type: snd : ((a, b)) -> b
// snd((0, 1)) == 1
template <typename X, typename Y>
Y snd(const std::pair<X, Y>& pair)
{
    return pair.second;
}

// API search type: transform_fst : ((a -> c), (a, b)) -> (c, b)
// transform_fst(square, (4, 5)) == (16, 5)
template <typename X, typename Y, typename F,
    typename ResultFirst = typename utils::function_traits<F>::result_type>
std::pair<ResultFirst, Y> transform_fst(F f, const std::pair<X, Y>& pair)
{
    return std::make_pair(f(pair.first), pair.second);
}

// API search type: transform_snd : ((b -> c), (a, b)) -> (a, c)
// transform_snd(square, (4, 5)) == (4, 25)
template <typename X, typename Y, typename F,
    typename ResultSecond = typename utils::function_traits<F>::result_type>
std::pair<X, ResultSecond> transform_snd(F f, const std::pair<X, Y>& pair)
{
    return std::make_pair(pair.first, f(pair.second));
}

// API search type: transform_pair : ((a -> c), (b -> d), (a, b)) -> (c, d)
// transform_pair(square, square, (4, 5)) == (16, 25)
template <typename X, typename Y, typename F, typename G,
    typename ResultFirst = typename utils::function_traits<F>::result_type,
    typename ResultSecond = typename utils::function_traits<G>::result_type>
std::pair<ResultFirst, ResultSecond> transform_pair(
    F f, G g, const std::pair<X, Y>& pair)
{
    return std::make_pair(f(pair.first), g(pair.second));
}

// API search type: swap_pair_elems : (a, b) -> (b, a)
// swap_pair_elems((3,4)) == (4,3)
template <typename X, typename Y>
std::pair<Y, X> swap_pair_elems(const std::pair<X, Y>& pair)
{
    return std::make_pair(pair.second, pair.first);
}

// API search type: swap_pairs_elems : [(a, b)] -> [(b, a)]
// swap_pairs_elems([(1,2), (3,4)]) == [(2,1), (4,3)]
template <typename ContainerIn,
    typename X = typename ContainerIn::value_type::first_type,
    typename Y = typename ContainerIn::value_type::second_type,
    typename ContainerOut =
        typename internal::same_cont_new_t<ContainerIn, std::pair<Y, X>>::type>
ContainerOut swap_pairs_elems(const ContainerIn& xs)
{
    return fplus::transform(swap_pair_elems<X, Y>, xs);
}

// API search type: overlapping_pairs : [a] -> [(a, a)]
// overlapping_pairs([0,1,2,3]) == [(0,1),(1,2),(2,3)]
template <typename Container,
    typename ContainerOut =
        typename internal::same_cont_new_t<Container,
            std::pair<
                typename Container::value_type,
                    typename Container::value_type>>::type>
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
    std::advance(it2, 1);
    for (; it2 != std::end(xs); ++it1, ++it2)
    {
        *itOut = std::make_pair(*it1, *it2);
    }
    return result;
}

// API search type: enumerate : [a] -> [(Int, a)]
// enumerate([6,4,7,6]) == [(0, 6), (1, 4), (2, 7), (3, 6)]
template <typename Container,
    typename T = typename Container::value_type>
std::vector<std::pair<std::size_t, T>> enumerate(const Container& xs)
{
    return zip(all_idxs(xs), xs);
}

// API search type: inner_product_with : (((a, a) -> b), ((b, b) -> b), b, [a], [a]) -> b
// inner_product_with((+), (*), [1, 2, 3], [4, 5, 6]) == [32]
template <typename ContainerIn1, typename ContainerIn2,
    typename OP1, typename OP2,
    typename Z,
    typename TOut = typename utils::function_traits<OP2>::result_type>
TOut inner_product_with(OP1 op1, OP2 op2, const Z& value,
        const ContainerIn1& xs, const ContainerIn2& ys)
{
    assert(size_of_cont(xs) == size_of_cont(ys));
    return std::inner_product(
        std::begin(xs), std::end(xs), std::begin(ys), value, op1, op2);
}

// API search type: inner_product : (a, [a], [a]) -> a
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

} // namespace fplus
