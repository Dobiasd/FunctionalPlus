// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <tuple>
#include <type_traits>
#include <utility>

#include <fplus/internal/invoke.hpp>

namespace fplus
{
namespace internal
{
// C++17 std::apply (http://en.cppreference.com/w/cpp/utility/apply)
template <typename F, typename Tuple, std::size_t... I>
constexpr decltype(auto) apply_impl(F&& f, Tuple&& t, std::index_sequence<I...>)
{
    return internal::invoke(std::forward<F>(f),
                          std::get<I>(std::forward<Tuple>(t))...);
}

template <typename F, typename Tuple>
constexpr decltype(auto) apply(F&& f, Tuple&& t)
{
    return internal::apply_impl(
        std::forward<F>(f),
        std::forward<Tuple>(t),
        std::make_index_sequence<
            std::tuple_size<std::decay_t<Tuple>>::value>{});
}
}
}
