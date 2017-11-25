// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <iterator>
#include <utility>

#include <fplus/compare.hpp>
#include <fplus/container_common.hpp>

#include <fplus/internal/invoke.hpp>

namespace fplus
{
namespace internal
{
template <typename GroupByCallable, typename F, typename ContainerIn>
auto group_on_labeled_impl(GroupByCallable group, F f, const ContainerIn& xs)
{
    const auto grouped = group(is_equal_by(f), xs);
    const auto attach_label = [f](const auto& g)
    {
        using std::begin;
        return std::make_pair(internal::invoke(f, *begin(g)), g);
    };
    return fplus::transform(attach_label, grouped);
}
}
}
