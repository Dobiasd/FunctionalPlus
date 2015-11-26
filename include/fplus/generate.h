// Copyright Tobias Hermann 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "container_common.h"

namespace fplus
{

// generate(f, 3) == [f(), f(), f()]
template <typename ContainerOut, typename F>
ContainerOut generate(F f, std::size_t amount)
{
    static_assert(utils::function_traits<F>::arity == 0, "Wrong arity.");
    ContainerOut ys;
    prepare_container(ys, amount);
    auto it = get_back_inserter<ContainerOut>(ys);
    for (std::size_t i = 0; i < amount; ++i)
    {
        *it = f();
    }
    return ys;
}

// generate_by_idx(f, 3) == [f(0), f(1), f(2)]
template <typename ContainerOut, typename F>
ContainerOut generate_by_idx(F f, std::size_t amount)
{
    static_assert(utils::function_traits<F>::arity == 1, "Wrong arity.");
    typedef typename utils::function_traits<F>::template arg<0>::type FIn;
    static_assert(std::is_convertible<std::size_t, FIn>::value, "Function does not take std::size_t or compatible type.");
    ContainerOut ys;
    prepare_container(ys, amount);
    auto it = get_back_inserter<ContainerOut>(ys);
    for (std::size_t i = 0; i < amount; ++i)
    {
        *it = f(i);
    }
    return ys;
}

// generate_integral_range_step(2, 9, 2) == [2, 4, 6, 8]
template <typename ContainerOut, typename T>
ContainerOut generate_integral_range_step
        (const T start, const T end, const T step)
{
    ContainerOut result;
    std::size_t size = (end - start) / step;
    prepare_container(result, size);
    auto it = get_back_inserter<ContainerOut>(result);
    for (T x = start; x < end; x+=step)
        *it = x;
    return result;
}

// generate_integral_range(2, 8, 2) == [2, 3, 4, 5, 6, 7, 8]
template <typename ContainerOut, typename T>
ContainerOut generate_integral_range(const T start, const T end)
{
    return generate_integral_range_step<ContainerOut>(start, end, 1);
}

// repeat(3, [1, 2]) == [1, 2, 1, 2, 1, 2]
template <typename Container>
Container repeat(size_t n, const Container& xs)
{
    std::vector<Container> xss(n, xs);
    return concat(xss);
}

// replicate(3, [1]) == [1, 1, 1]
template <typename ContainerOut>
ContainerOut replicate(size_t n, const typename ContainerOut::value_type& x)
{
    return ContainerOut(n, x);
}

// infixes(3, [1,2,3,4,5,6]) == [[1,2,3], [2,3,4], [3,4,5], [4,5,6]]
template <typename ContainerOut, typename ContainerIn>
ContainerOut infixes(std::size_t length, ContainerIn& xs)
{
    static_assert(std::is_convertible<ContainerIn, typename ContainerOut::value_type>::value, "ContainerOut can not take values of type ContainerIn as elements.");
    ContainerOut result;
    if (size_of_cont(xs) < length)
        return result;
    prepare_container(result, size_of_cont(xs) - length);
    auto itOut = get_back_inserter(result);
    std::size_t idx = 0;
    for (;idx < size_of_cont(xs) - length; ++idx)
    {
        *itOut = get_range(idx, idx + length, xs);
    }
    return result;
}

} // namespace fplus