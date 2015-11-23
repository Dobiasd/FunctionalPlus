// Copyright Tobias Hermann 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "ContainerCommon.h"
#include "FunctionTraits.h"

#include <utility>

namespace FunctionalPlus
{

// ZipWith((+), [1, 2, 3], [5, 6]) == [6, 8]
template <typename ContainerIn1, typename ContainerIn2, typename F,
    typename X = typename ContainerIn1::value_type,
    typename Y = typename ContainerIn2::value_type,
    typename TOut = typename utils::function_traits<F>::result_type,
    typename ContainerOut =
        typename SameContNewT<ContainerIn1, TOut>::type>
ContainerOut ZipWith(const F& f,
        const ContainerIn1& xs, const ContainerIn2& ys)
{
    static_assert(utils::function_traits<F>::arity == 2, "Function must take two parameters.");
    typedef typename utils::function_traits<F>::result_type FOut;
    typedef typename utils::function_traits<F>::template arg<0>::type FIn0;
    typedef typename utils::function_traits<F>::template arg<1>::type FIn1;
    typedef typename ContainerIn1::value_type T1;
    typedef typename ContainerIn2::value_type T2;
    static_assert(std::is_convertible<T1, FIn0>::value, "Function does not take elements from first Container as first Parameter.");
    static_assert(std::is_convertible<T2, FIn1>::value, "Function does not take elements from second Container as second Parameter.");
    static_assert(std::is_convertible<FOut, TOut>::value, "Elements produced by this function can not be stored in ContainerOut.");
    static_assert(std::is_same<
        typename SameContNewT<ContainerIn1, void>::type,
        typename SameContNewT<ContainerIn2, void>::type>::value,
        "Both Containers must be of same outer type.");
    ContainerOut result;
    std::size_t resultSize = std::min(Size(xs), Size(ys));
    PrepareContainer(result, resultSize);
    auto itResult = BackInserter(result);
    auto itXs = std::begin(xs);
    auto itYs = std::begin(ys);
    for (std::size_t i = 0; i < resultSize; ++i)
    {
        *itResult = f(*(itXs++), *(itYs)++);
    }
    return result;
}

// Zip([1, 2, 3], [5, 6]) == [(1, 5), (2, 6)]
template <typename ContainerIn1, typename ContainerIn2,
    typename X = typename ContainerIn1::value_type,
    typename Y = typename ContainerIn2::value_type,
    typename TOut = std::pair<X, Y>,
    typename ContainerOut =
        typename SameContNewT<ContainerIn1, TOut>::type>
ContainerOut Zip(const ContainerIn1& xs, const ContainerIn2& ys)
{
    auto MakePair = [](const X& x, const Y& y)
            { return std::make_pair(x, y); };
    return ZipWith(MakePair, xs, ys);
}

// Unzip([(1, 5), (2, 6)]) == ([1, 2], [5, 6])
template <typename ContainerIn,
    typename TIn = typename ContainerIn::value_type,
    typename X = typename TIn::first_type,
    typename Y = typename TIn::second_type,
    typename ContainerOutX = typename SameContNewT<ContainerIn, X>::type,
    typename ContainerOutY = typename SameContNewT<ContainerIn, Y>::type>
std::pair<ContainerOutX, ContainerOutY> Unzip(const ContainerIn& pairs)
{
    ContainerOutX firsts;
    ContainerOutY seconds;
    PrepareContainer(firsts, Size(pairs));
    PrepareContainer(seconds, Size(pairs));
    auto itFirsts = BackInserter(firsts);
    auto itSeconds = BackInserter(seconds);
    for (const auto& pair : pairs)
    {
        *itFirsts = pair.first;
        *itSeconds = pair.second;
    }
    return std::make_pair(firsts, seconds);
}

// Fst((0, 1)) == 0
template <typename X, typename Y>
X Fst(const std::pair<X, Y>& pair)
{
    return pair.first;
}

// Snd((0, 1)) == 1
template <typename X, typename Y>
Y Snd(const std::pair<X, Y>& pair)
{
    return pair.second;
}

// TransformFst(square, (4, 5)) == (16, 5)
template <typename X, typename Y, typename F,
    typename ResultFirst = typename utils::function_traits<F>::result_type>
std::pair<ResultFirst, Y> TransformFst(F f, const std::pair<X, Y>& pair)
{
    return std::make_pair(f(pair.first), pair.second);
}

// TransformSnd(square, (4, 5)) == (4, 25)
template <typename X, typename Y, typename F,
    typename ResultSecond = typename utils::function_traits<F>::result_type>
std::pair<X, ResultSecond> TransformSnd(F f, const std::pair<X, Y>& pair)
{
    return std::make_pair(pair.first, f(pair.second));
}

// SwapPairElems((3,4)) == (4,3)
template <typename X, typename Y>
std::pair<Y, X> SwapPairElems(const std::pair<X, Y>& pair)
{
    return std::make_pair(pair.second, pair.first);
}

// OverlappingPairs([0,1,2,3]) == [(0,1),(1,2),(2,3)]
template <typename ContainerOut, typename Container,
    typename T = typename Container::value_type,
    typename Pair = typename std::pair<T, T>>
ContainerOut OverlappingPairs(const Container& xs)
{
    static_assert(std::is_convertible<Pair, typename ContainerOut::value_type>::value, "ContainerOut can not store pairs of elements from ContainerIn.");
    ContainerOut result;
    if (Size(xs) < 2)
        return result;
    PrepareContainer(result, Size(xs) - 1);
    auto itOut = BackInserter(result);
    auto it1 = std::begin(xs);
    auto it2 = it1;
    std::advance(it2, 1);
    for (; it2 != std::end(xs); ++it1, ++it2)
    {
        *itOut = std::make_pair(*it1, *it2);
    }
    return result;
}

} // namespace FunctionalPlus