// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <doctest/doctest.h>
#include <fplus/fplus.hpp>

TEST_CASE("container_traits_test - static_asserts")
{
    using namespace fplus;

    const auto unary_f = [](int x) -> double
    {
        return static_cast<double>(x);
    };

    const auto binary_f = [](int x, int y) -> double
    {
        return static_cast<double>(x + y);
    };

    static_assert(std::is_same<
            internal::same_cont_new_t<
                std::vector<int>,
                double>::type,
        std::vector<double>>::value,
        "No.");

    static_assert(std::is_same<
            internal::same_cont_new_t_from_unary_f<
                std::vector<int>,
                decltype(unary_f)>::type,
        std::vector<double>>::value,
        "No.");

    static_assert(std::is_same<
            internal::same_cont_new_t_from_binary_f<
                std::vector<int>,
                decltype(binary_f),
                int,
                int>::type,
        std::vector<double>>::value,
        "No.");
}
