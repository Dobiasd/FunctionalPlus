// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <doctest/doctest.h>
#include <fplus/fplus.hpp>
#include <array>

TEST_CASE("optimize_test - minimize_downhill")
{
    using namespace fplus;
    const auto objective = [](const std::array<double, 1>& x)
    {
        return square(x[0] + 2) + 3;
    };
    {
        const auto result = minimize_downhill<1>(
            objective, 0.0001, {{123}});
        REQUIRE(is_in_closed_interval_around(0.0001, -2.0, result[0]));
    }
    {
        const auto result = minimize_downhill<1>(
            objective, 0.0001, {{-42}}, maybe<double>(0.01));
        REQUIRE(is_in_closed_interval_around(0.0001, -2.0, result[0]));
    }
    {
        const auto result = minimize_downhill<1>(
            objective, 0.0001, {{-2.000001}});
        REQUIRE(is_in_closed_interval_around(0.0001, -2.0, result[0]));
    }

    const auto objective_2d = [](const std::array<double, 2>& p)
    {
        const auto x = p[0];
        const auto y = p[1];
        return
            abs(2*cube(x-3)) + 4*square(x+1) + 2*x +
            abs(1*cube(y-7)) + 7*square(y-4) + 6*y;
    };
    {
        const auto result1 = minimize_downhill<2>(
            objective_2d, 0.0001, {{0,0}});
        REQUIRE(is_in_closed_interval_around(0.001, 1.1946, result1[0]));
        REQUIRE(is_in_closed_interval_around(0.001, 4.7025, result1[1]));

        const auto result2 = minimize_downhill<2>(
            objective_2d, 0.0001, {{0,0}}, 0.123, 0.234);
        REQUIRE(is_in_closed_interval_around(0.001, 1.1946, result2[0]));
        REQUIRE(is_in_closed_interval_around(0.001, 4.7025, result2[1]));
    }
}
