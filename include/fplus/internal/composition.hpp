// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <tuple>
#include <utility>

#include <fplus/internal/invoke.hpp>

namespace fplus
{
namespace internal
{
// source: https://codereview.stackexchange.com/a/63893
// note: the code in the link above is called with the arguments in reverse order
template <typename... Fs>
class compose_impl
{
    static constexpr std::size_t size = sizeof...(Fs);
    static_assert(size > 1,
                  "Invalid number of functions to compose, minimum is two.");

  public:
    compose_impl(Fs&&... fs) : _functionTuple(std::forward<Fs>(fs)...)
    {
    }

    template <typename... Ts>
    auto operator()(Ts&&... ts) const
    {
        return _apply(std::integral_constant<std::size_t, 0>{},
                      std::forward<Ts>(ts)...);
    }

  private:
    template <std::size_t N, typename... Ts>
    auto _apply(std::integral_constant<std::size_t, N>, Ts&&... ts) const
    {
        return _apply(std::integral_constant<std::size_t, N + 1>{},
                      std::get<N>(_functionTuple)(std::forward<Ts>(ts)...));
    }

    template <typename... Ts>
    auto _apply(std::integral_constant<std::size_t, size - 1>, Ts&&... ts) const
    {
        return internal::invoke(std::get<size - 1>(_functionTuple),
                              std::forward<Ts>(ts)...);
    }

    std::tuple<Fs...> _functionTuple;
};

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

// concentrate asserts in this method. Lambda is provided by the library.
template <typename Lambda, typename F, typename G>
auto logical_binary_op(Lambda op, F f, G g)
{
    // Perfect-forwarding might move twice, if we add a requirement on F and G,
    // that might not be an issue.
    return [op, f, g](auto x) {
        internal::trigger_static_asserts<internal::unary_function_tag,
                                             F,
                                             decltype(x)>();
        internal::trigger_static_asserts<internal::unary_function_tag,
                                             G,
                                             decltype(x)>();
        using FRes = std::decay_t<internal::invoke_result_t<F, decltype(x)>>;
        using GRes = std::decay_t<internal::invoke_result_t<G, decltype(x)>>;
        static_assert(std::is_same<FRes, bool>::value, "Must return bool.");
        static_assert(std::is_same<GRes, bool>::value, "Must return bool.");

        return op(f, g, x);
    };
}
}
}
