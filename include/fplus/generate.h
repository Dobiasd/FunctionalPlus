// Copyright Tobias Hermann 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "container_common.h"

namespace fplus
{

// Generate(f, 3) == [f(), f(), f()]
template <typename ContainerOut, typename F>
ContainerOut Generate(F f, std::size_t amount)
{
    static_assert(utils::function_traits<F>::arity == 0, "Wrong arity.");
    ContainerOut ys;
    PrepareContainer(ys, amount);
    auto it = BackInserter<ContainerOut>(ys);
    for (std::size_t i = 0; i < amount; ++i)
    {
        *it = f();
    }
    return ys;
}

// GenerateByIdx(f, 3) == [f(0), f(1), f(2)]
template <typename ContainerOut, typename F>
ContainerOut GenerateByIdx(F f, std::size_t amount)
{
    static_assert(utils::function_traits<F>::arity == 1, "Wrong arity.");
    typedef typename utils::function_traits<F>::template arg<0>::type FIn;
    static_assert(std::is_convertible<std::size_t, FIn>::value, "Function does not take std::size_t or compatible type.");
    ContainerOut ys;
    PrepareContainer(ys, amount);
    auto it = BackInserter<ContainerOut>(ys);
    for (std::size_t i = 0; i < amount; ++i)
    {
        *it = f(i);
    }
    return ys;
}

// GenerateIntegralRangeStep(2, 9, 2) == [2, 4, 6, 8]
template <typename ContainerOut, typename T>
ContainerOut GenerateIntegralRangeStep
        (const T start, const T end, const T step)
{
    ContainerOut result;
    std::size_t size = (end - start) / step;
    PrepareContainer(result, size);
    auto it = BackInserter<ContainerOut>(result);
    for (T x = start; x < end; x+=step)
        *it = x;
    return result;
}

// GenerateIntegralRange(2, 8, 2) == [2, 3, 4, 5, 6, 7, 8]
template <typename ContainerOut, typename T>
ContainerOut GenerateIntegralRange(const T start, const T end)
{
    return GenerateIntegralRangeStep<ContainerOut>(start, end, 1);
}

// Repeat(3, [1, 2]) == [1, 2, 1, 2, 1, 2]
template <typename Container>
Container Repeat(size_t n, const Container& xs)
{
    std::vector<Container> xss(n, xs);
    return Concat(xss);
}

// Replicate(3, [1]) == [1, 1, 1]
template <typename ContainerOut>
ContainerOut Replicate(size_t n, const typename ContainerOut::value_type& x)
{
    return ContainerOut(n, x);
}

// Infixes(3, [1,2,3,4,5,6]) == [[1,2,3], [2,3,4], [3,4,5], [4,5,6]]
template <typename ContainerOut, typename ContainerIn>
ContainerOut Infixes(std::size_t length, ContainerIn& xs)
{
    static_assert(std::is_convertible<ContainerIn, typename ContainerOut::value_type>::value, "ContainerOut can not take values of type ContainerIn as elements.");
    ContainerOut result;
    if (Size(xs) < length)
        return result;
    PrepareContainer(result, Size(xs) - length);
    auto itOut = BackInserter(result);
    std::size_t idx = 0;
    for (;idx < Size(xs) - length; ++idx)
    {
        *itOut = GetRange(idx, idx + length, xs);
    }
    return result;
}

} // namespace fplus