// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <doctest/doctest.h>
#include <fplus/fplus.hpp>

TEST_CASE("raii_test - make_raii")
{
    std::string log = "nothing";
    const auto init = [&log]() { log = "init"; };
    const auto quit = [&log]() { log = "quit"; };

    REQUIRE_EQ(log, "nothing");
    {
        REQUIRE_EQ(log, "nothing");
        const auto ressource = fplus::make_raii(init, quit);
        REQUIRE_EQ(log, "init");
    }
    REQUIRE_EQ(log, "quit");
}
