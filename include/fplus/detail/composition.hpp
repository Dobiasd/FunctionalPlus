// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <tuple>
#include <utility>

#include <fplus/detail/invoke.hpp>

namespace fplus
{
namespace detail
{
// Is BinaryLift really correct?
template <typename Tuple, typename BinaryLift>
auto compose_binary_lift_impl(std::integral_constant<std::size_t, 1>,
                              const Tuple& tup,
                              const BinaryLift& lifter)
{
    return lifter(std::get<0>(tup), std::get<1>(tup));
}

template <std::size_t N, typename Tuple, typename BinaryLift>
auto compose_binary_lift_impl(std::integral_constant<std::size_t, N>,
                              const Tuple& tup,
                              const BinaryLift& lifter)
{
    return lifter(
        compose_binary_lift_impl(
            std::integral_constant<std::size_t, N - 1>{}, tup, lifter),
        std::get<N>(tup));
}

template <typename BinaryLift, typename... Callables>
auto compose_binary_lift(const BinaryLift& lifter, Callables&&... args)
{
    static_assert(sizeof...(Callables) > 1,
                  "Invalid number of functions to compose, minimum is two.");
    const auto tup = std::forward_as_tuple(std::forward<Callables>(args)...);
    return compose_binary_lift_impl(
        std::integral_constant<std::size_t, sizeof...(Callables) - 1>{},
        tup,
        lifter);
}
}
}
