// Copyright Tobias Hermann 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "ContainerCommon.h"
#include "Filter.h"
#include "Maybe.h"
#include "Composition.h"
#include "FunctionTraits.h"

#include <algorithm>
#include <random>

namespace FunctionalPlus
{

// (a -> b) -> [a] -> [b]
// Transform((*2), [1, 3, 4]) == [2, 6, 8]
// Also known as Map.
template <typename F, typename ContainerIn,
    typename ContainerOut =
        typename SameContNewTFromUnaryF<ContainerIn, F>::type>
ContainerOut Transform(F f, const ContainerIn& xs)
{
    static_assert(utils::function_traits<F>::arity == 1, "Wrong arity.");
    ContainerOut ys;
    PrepareContainer(ys, Size(xs));
    auto it = BackInserter<ContainerOut>(ys);
    std::transform(std::begin(xs), std::end(xs), it, f);
    return ys;
}

// (a -> b) -> [a] -> [b]
// TransformConvert((*2), [1, 3, 4]) == [2, 6, 8]
template <typename ContainerOut, typename F, typename ContainerIn>
ContainerOut TransformConvert(F f, const ContainerIn& xs)
{
    static_assert(utils::function_traits<F>::arity == 1, "Wrong arity.");
    ContainerOut ys;
    PrepareContainer(ys, Size(xs));
    auto it = BackInserter<ContainerOut>(ys);
    std::transform(std::begin(xs), std::end(xs), it, f);
    return ys;
}

// (size_t -> a -> b) -> [a] -> [b]
// TransformWithIdx(f, [6, 4, 7]) == [f(0, 6), f(1, 4), f(2, 7)]
template <typename F, typename ContainerIn,
    typename ContainerOut =
        typename SameContNewTFromBinaryF<
            ContainerIn, F, std::size_t,
            typename ContainerIn::value_type>::type>
ContainerOut TransformWithIdx(F f, const ContainerIn& xs)
{
    static_assert(utils::function_traits<F>::arity == 2, "Wrong arity.");
    ContainerOut ys;
    PrepareContainer(ys, Size(xs));
    auto it = BackInserter<ContainerOut>(ys);
    std::size_t idx = 0;
    for (const auto& x : xs)
    {
        *it = f(idx++, x);
    }
    return ys;
}

// (a -> Maybe b) -> [a] -> [b]
template <typename F, typename ContainerIn,
    typename FOut = typename utils::function_traits<F>::result_type,
    typename ContainerOut =
        typename SameContNewT<ContainerIn, typename FOut::type>::type>
ContainerOut TransformAndKeepJusts(F f, const ContainerIn& xs)
{
    static_assert(utils::function_traits<F>::arity == 1, "Wrong arity.");
    auto transformed = Transform(f, xs);
    return Justs<decltype(transformed), ContainerOut>(transformed);
}

// (a -> [b]) -> [a] -> [b]
template <typename F, typename ContainerIn,
    typename ContainerOut =
        typename SameContNewTFromUnaryF<ContainerIn, F>::type::value_type>
ContainerOut TransformAndConcat(F f, const ContainerIn& xs)
{
    static_assert(utils::function_traits<F>::arity == 1, "Wrong arity.");
    return Concat(Transform(f, xs));
}

// [[1, 2, 3], [4, 5, 6]] -> [[1, 4], [2, 5], [3, 6]]
template <typename Container>
Container Transpose(const Container& grid2d)
{
    std::size_t height = Size(grid2d);
    if (height == 0)
        return grid2d;

    typedef typename Container::value_type Row;
    auto rowLenghts = Transform(Size<Row>, grid2d);
    assert(AllTheSame(rowLenghts));
    std::size_t width = rowLenghts.front();

    Container result;
    PrepareContainer(result, width);

    for (std::size_t x = 0; x < width; ++x)
    {
        *BackInserter(result) = Row();
        auto itOutRow = BackInserter(result.back());
        for (std::size_t y = 0; y < height; ++y)
        {
            *itOutRow = grid2d[y][x];
        }
    }
    return result;
}

// Returns n random elements from xs.
// n has to be smaller than or equal to the number of elements in xs.
template <typename Container>
Container Sample(std::size_t n, const Container& xs)
{
    assert(n <= Size(xs));
    std::random_device rd;
    std::mt19937 gen(rd());
    Container ys = xs;
    std::shuffle(begin(ys), end(ys), gen);
    return GetRange(0, n, ys);
}

} // namespace FunctionalPlus