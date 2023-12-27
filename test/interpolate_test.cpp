// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <doctest/doctest.h>
#include <fplus/fplus.hpp>
#include <vector>


namespace {
    typedef std::vector<double> FloatVector;
    FloatVector xs = {1.0, 2.0, 6.0};
}

TEST_CASE("interpolate_test - interpolate_linear")
{
    using namespace fplus;
    REQUIRE(is_in_interval_around(0.0001, 1.0, elem_at_float_idx(-1.7, xs)));
    REQUIRE(is_in_interval_around(0.0001, 1.0, elem_at_float_idx(-0.2, xs)));
    REQUIRE(is_in_interval_around(0.0001, 1.0, elem_at_float_idx( 0.0, xs)));
    REQUIRE(is_in_interval_around(0.0001, 1.2, elem_at_float_idx( 0.2, xs)));
    REQUIRE(is_in_interval_around(0.0001, 1.7, elem_at_float_idx( 0.7, xs)));
    REQUIRE(is_in_interval_around(0.0001, 2.0, elem_at_float_idx( 1.0, xs)));
    REQUIRE(is_in_interval_around(0.0001, 4.0, elem_at_float_idx( 1.5, xs)));
    REQUIRE(is_in_interval_around(0.0001, 5.6, elem_at_float_idx( 1.9, xs)));
    REQUIRE(is_in_interval_around(0.0001, 6.0, elem_at_float_idx( 2.0, xs)));
    REQUIRE(is_in_interval_around(0.0001, 6.0, elem_at_float_idx( 2.1, xs)));
    REQUIRE(is_in_interval_around(0.0001, 6.0, elem_at_float_idx( 2.8, xs)));
}
