// Copyright Tobias Hermann 2015.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "container_common.h"
#include "filter.h"
#include "maybe.h"
#include "result.h"
#include "composition.h"
#include "function_traits.h"

#include <algorithm>
#include <random>

namespace fplus
{

// API search type: transform_convert : (a -> b) -> [a] -> [b]
// transform_convert((*2), [1, 3, 4]) == [2, 6, 8]
template <typename ContainerOut, typename F, typename ContainerIn>
ContainerOut transform_convert(F f, const ContainerIn& xs)
{
    static_assert(utils::function_traits<F>::arity == 1, "Wrong arity.");
    ContainerOut ys;
    prepare_container(ys, size_of_cont(xs));
    auto it = get_back_inserter<ContainerOut>(ys);
    std::transform(std::begin(xs), std::end(xs), it, f);
    return ys;
}

// API search type: transform_with_idx : (int -> a -> b) -> [a] -> [b]
// transform_with_idx(f, [6, 4, 7]) == [f(0, 6), f(1, 4), f(2, 7)]
template <typename F, typename ContainerIn,
    typename ContainerOut = typename same_cont_new_t_from_binary_f<
        ContainerIn, F, std::size_t, typename ContainerIn::value_type>::type>
ContainerOut transform_with_idx(F f, const ContainerIn& xs)
{
    static_assert(utils::function_traits<F>::arity == 2, "Wrong arity.");
    ContainerOut ys;
    prepare_container(ys, size_of_cont(xs));
    auto it = get_back_inserter<ContainerOut>(ys);
    std::size_t idx = 0;
    for (const auto& x : xs)
    {
        *it = f(idx++, x);
    }
    return ys;
}

// API search type: transform_and_keep_justs : (a -> maybe b) -> [a] -> [b]
// Map function over values and drop resulting nothings.
template <typename F, typename ContainerIn,
    typename FOut = typename utils::function_traits<F>::result_type,
    typename ContainerOut = typename same_cont_new_t<ContainerIn,
        typename FOut::type>::type>
ContainerOut transform_and_keep_justs(F f, const ContainerIn& xs)
{
    static_assert(utils::function_traits<F>::arity == 1, "Wrong arity.");
    auto transformed = transform(f, xs);
    return justs<decltype(transformed), ContainerOut>(transformed);
}

// API search type: transform_and_keep_oks : (a -> Result b) -> [a] -> [b]
// Map function over values and drop resulting errors.
template <typename F, typename ContainerIn,
    typename FOut = typename utils::function_traits<F>::result_type,
    typename ContainerOut = typename same_cont_new_t<
        ContainerIn, typename FOut::ok_t>::type>
ContainerOut transform_and_keep_oks(F f, const ContainerIn& xs)
{
    static_assert(utils::function_traits<F>::arity == 1, "Wrong arity.");
    auto transformed = transform(f, xs);
    return oks<decltype(transformed), ContainerOut>(transformed);
}

// API search type: transform_and_concat : (a -> [b]) -> [a] -> [b]
// Map function over values and concat results.
template <typename F, typename ContainerIn,
    typename ContainerOut = typename same_cont_new_t_from_unary_f<
        ContainerIn, F>::type::value_type>
ContainerOut transform_and_concat(F f, const ContainerIn& xs)
{
    static_assert(utils::function_traits<F>::arity == 1, "Wrong arity.");
    return concat(transform(f, xs));
}

// API search type: transpose : [[a]] -> [[a]]
// [[1, 2, 3], [4, 5, 6]] -> [[1, 4], [2, 5], [3, 6]]
template <typename Container>
Container transpose(const Container& grid2d)
{
    std::size_t height = size_of_cont(grid2d);
    if (height == 0)
        return grid2d;

    typedef typename Container::value_type Row;
    auto rowLenghts = transform(size_of_cont<Row>, grid2d);
    assert(all_the_same(rowLenghts));
    std::size_t width = rowLenghts.front();

    Container result;
    prepare_container(result, width);

    for (std::size_t x = 0; x < width; ++x)
    {
        *get_back_inserter(result) = Row();
        auto itOutRow = get_back_inserter(result.back());
        for (std::size_t y = 0; y < height; ++y)
        {
            *itOutRow = grid2d[y][x];
        }
    }
    return result;
}

// API search type: sample : int -> [a] -> [a]
// Returns n random elements from xs.
// n has to be smaller than or equal to the number of elements in xs.
template <typename Container>
Container sample(std::size_t n, const Container& xs)
{
    assert(n <= size_of_cont(xs));
    std::random_device rd;
    std::mt19937 gen(rd());
    Container ys = xs;
    std::shuffle(begin(ys), end(ys), gen);
    return get_range(0, n, ys);
}

} // namespace fplus
