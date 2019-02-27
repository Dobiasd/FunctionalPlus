// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include <fplus/fplus.hpp>

TEST_CASE("queue_test, full")
{
    using namespace fplus;

    queue<int> q;
    q.push(1);
    q.push(2);

    const std::vector<int> content = {1,2};

    REQUIRE_EQ(q.pop_all(), content);
}
