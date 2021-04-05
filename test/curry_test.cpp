// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <doctest/doctest.h>
#include <fplus/fplus.hpp>

TEST_CASE("curry_test - show_fill_left")
{
    using namespace fplus;
    REQUIRE_EQ(show_fill_left<int>(' ', 4, 3), "   3");
    const auto result_old_style = show_fill_left<int>(' ', 4, 3);
    const auto result_new_style = curry::show_fill_left(' ')(std::size_t(4))(3);
    REQUIRE_EQ(result_old_style, result_new_style);
}
