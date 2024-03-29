// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

namespace fplus {
namespace curry {

    // Currying.
    // Allow to generically bind parameters one by one.

#define fplus_curry_define_fn_0(fplus_curry_define_fn_0_name)                                                   \
    inline auto fplus_curry_define_fn_0_name()                                                                  \
    {                                                                                                           \
        return [](auto&& fplus_curry_p1) {                                                                      \
            return fplus::fplus_curry_define_fn_0_name(std::forward<decltype(fplus_curry_p1)>(fplus_curry_p1)); \
        };                                                                                                      \
    }

#define fplus_curry_define_fn_1(fplus_curry_define_fn_1_name)                                                       \
    template <typename P1>                                                                                          \
    auto fplus_curry_define_fn_1_name(P1 p1)                                                                        \
    {                                                                                                               \
        return [p1](auto&& fplus_curry_p2) {                                                                        \
            return fplus::fplus_curry_define_fn_1_name(p1, std::forward<decltype(fplus_curry_p2)>(fplus_curry_p2)); \
        };                                                                                                          \
    }

#define fplus_curry_define_fn_2(fplus_curry_define_fn_2_name)                                                                           \
    template <typename P1>                                                                                                              \
    auto fplus_curry_define_fn_2_name(P1 p1)                                                                                            \
    {                                                                                                                                   \
        return [p1](const auto& fplus_curry_p2) {                                                                                       \
            return [p1, fplus_curry_p2](auto&& fplus_curry_p3) {                                                                        \
                return fplus::fplus_curry_define_fn_2_name(p1, fplus_curry_p2, std::forward<decltype(fplus_curry_p3)>(fplus_curry_p3)); \
            };                                                                                                                          \
        };                                                                                                                              \
    }

#define fplus_curry_define_fn_3(fplus_curry_define_fn_3_name)                                                                                               \
    template <typename P1>                                                                                                                                  \
    auto fplus_curry_define_fn_3_name(P1 p1)                                                                                                                \
    {                                                                                                                                                       \
        return [p1](const auto& fplus_curry_p2) {                                                                                                           \
            return [p1, fplus_curry_p2](const auto& fplus_curry_p3) {                                                                                       \
                return [p1, fplus_curry_p2, fplus_curry_p3](auto&& fplus_curry_p4) {                                                                        \
                    return fplus::fplus_curry_define_fn_3_name(p1, fplus_curry_p2, fplus_curry_p3, std::forward<decltype(fplus_curry_p4)>(fplus_curry_p4)); \
                };                                                                                                                                          \
            };                                                                                                                                              \
        };                                                                                                                                                  \
    }

#define fplus_curry_define_fn_4(fplus_curry_define_fn_4_name)                                                                                                                   \
    template <typename P1>                                                                                                                                                      \
    auto fplus_curry_define_fn_4_name(P1 p1)                                                                                                                                    \
    {                                                                                                                                                                           \
        return [p1](const auto& fplus_curry_p2) {                                                                                                                               \
            return [p1, fplus_curry_p2](const auto& fplus_curry_p3) {                                                                                                           \
                return [p1, fplus_curry_p2, fplus_curry_p3](const auto& fplus_curry_p4) {                                                                                       \
                    return [p1, fplus_curry_p2, fplus_curry_p3, fplus_curry_p4](auto&& fplus_curry_p5) {                                                                        \
                        return fplus::fplus_curry_define_fn_4_name(p1, fplus_curry_p2, fplus_curry_p3, fplus_curry_p4, std::forward<decltype(fplus_curry_p5)>(fplus_curry_p5)); \
                    };                                                                                                                                                          \
                };                                                                                                                                                              \
            };                                                                                                                                                                  \
        };                                                                                                                                                                      \
    }

#include "curry_instances.autogenerated_defines"

} // namespace curry
} // namespace fplus
