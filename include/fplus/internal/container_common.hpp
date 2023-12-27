// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <numeric>
#include <type_traits>

#include <fplus/internal/invoke.hpp>

namespace fplus
{
namespace internal
{

template<class InputIt, class T>
T accumulate(InputIt first, InputIt last, T init)
{
    for (; first != last; ++first) {
        init = std::move(init) + *first;
    }
    return init;
}

template<class InputIt, class T, class BinaryOperation>
T accumulate(InputIt first, InputIt last, T init,
             BinaryOperation op)
{
    for (; first != last; ++first) {
        init = op(std::move(init), *first);
    }
    return init;
}

template <typename F,
          typename Acc,
          typename InputIterator,
          typename OutputIterator>
void scan_impl(F f,
               const Acc& init,
               OutputIterator itOut,
               InputIterator begin,
               InputIterator end)
{
    *itOut = init;

    auto g = [itOut, f](auto acc, auto x) mutable
    {
        acc = internal::invoke(f, acc, x);
        *itOut = acc;
        return acc;
    };

    internal::accumulate(begin, end, init, g);
}
}
}
