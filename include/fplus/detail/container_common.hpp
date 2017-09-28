// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <numeric>
#include <type_traits>

#include <fplus/detail/invoke.hpp>

namespace fplus
{
namespace detail
{
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
        acc = detail::invoke(f, acc, x);
        *itOut = acc;
        return acc;
    };

    std::accumulate(begin, end, init, g);
}
}
}
