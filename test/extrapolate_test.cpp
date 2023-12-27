// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <doctest/doctest.h>
#include <fplus/fplus.hpp>
#include <vector>


namespace {
    typedef std::vector<int> IntVector;
    IntVector xs = {1,2,2,3,2};
}

TEST_CASE("extrapolate_test - elem_at_idx_or_nothing")
{
    using namespace fplus;
    REQUIRE_EQ(elem_at_idx_or_nothing(-4, xs), nothing<int>());
    REQUIRE_EQ(elem_at_idx_or_nothing(14, xs), nothing<int>());
    REQUIRE_EQ(elem_at_idx_or_nothing(3, xs), maybe<int>(3));
}

TEST_CASE("extrapolate_test - elem_at_idx_or_constant")
{
    using namespace fplus;
    REQUIRE_EQ(elem_at_idx_or_constant(4, -4, xs), 4);
    REQUIRE_EQ(elem_at_idx_or_constant(4, 14, xs), 4);
    REQUIRE_EQ(elem_at_idx_or_constant(4, 3, xs), 3);
}

TEST_CASE("extrapolate_test - elem_at_idx_or_replicate")
{
    using namespace fplus;
    REQUIRE_EQ(elem_at_idx_or_replicate(-2, xs), 1);
    REQUIRE_EQ(elem_at_idx_or_replicate(-1, xs), 1);
    REQUIRE_EQ(elem_at_idx_or_replicate(0, xs), 1);
    REQUIRE_EQ(elem_at_idx_or_replicate(4, xs), 2);
    REQUIRE_EQ(elem_at_idx_or_replicate(5, xs), 2);
    REQUIRE_EQ(elem_at_idx_or_replicate(6, xs), 2);
}

TEST_CASE("extrapolate_test - elem_at_idx_or_wrap")
{
    using namespace fplus;
    REQUIRE_EQ(elem_at_idx_or_wrap(-2, xs), 3);
    REQUIRE_EQ(elem_at_idx_or_wrap(-1, xs), 2);
    REQUIRE_EQ(elem_at_idx_or_wrap(0, xs), 1);
    REQUIRE_EQ(elem_at_idx_or_wrap(4, xs), 2);
    REQUIRE_EQ(elem_at_idx_or_wrap(5, xs), 1);
    REQUIRE_EQ(elem_at_idx_or_wrap(6, xs), 2);
}

TEST_CASE("extrapolate_test - elem_at_idx_or_wrap")
{
    using namespace fplus;
    REQUIRE_EQ(elem_at_idx_or_wrap(-2, xs), 3);
    REQUIRE_EQ(elem_at_idx_or_wrap(-1, xs), 2);
    REQUIRE_EQ(elem_at_idx_or_wrap(0, xs), 1);
    REQUIRE_EQ(elem_at_idx_or_wrap(4, xs), 2);
    REQUIRE_EQ(elem_at_idx_or_wrap(5, xs), 1);
    REQUIRE_EQ(elem_at_idx_or_wrap(6, xs), 2);
}

TEST_CASE("extrapolate_test - extrapolate_replicate")
{
    using namespace fplus;
    REQUIRE_EQ(extrapolate_replicate(2, 2, xs), IntVector({1,1,1,2,2,3,2,2,2}));
}

TEST_CASE("extrapolate_test - extrapolate_wrap")
{
    using namespace fplus;
    REQUIRE_EQ(extrapolate_wrap(2, 2, xs), IntVector({3,2,1,2,2,3,2,1,2}));
}
