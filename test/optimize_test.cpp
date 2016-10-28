// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "fplus/fplus.hpp"
#include <array>

TEST_CASE("optimize_test, minimize_downhill")
{
    using namespace fplus;
    const auto objective = [](const std::array<double, 1>& x)
    {
        return square(x[0] + 2) + 3;
    };
    {
        const auto result = minimize_downhill<1>(
            objective, 0.0001, 0.01, {123});
        REQUIRE(is_in_closed_range_around(0.0001, -2.0)(result[0]));
    }
    {
        const auto result = minimize_downhill<1>(
            objective, 0.0001, 0.01, {-42});
        REQUIRE(is_in_closed_range_around(0.0001, -2.0)(result[0]));
    }
    {
        const auto result = minimize_downhill<1>(
            objective, 0.0001, 0.01, {-2.000001});
        REQUIRE(is_in_closed_range_around(0.0001, -2.0)(result[0]));
    }
}
