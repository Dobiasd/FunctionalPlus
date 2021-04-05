// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <doctest/doctest.h>
#include <fplus/fplus.hpp>

TEST_CASE("queue_test - full")
{
    using namespace fplus;
    using namespace std::chrono_literals;

    queue<int> q;

    std::thread producer([&q] {
        q.push(1);
        q.push(2);
        std::this_thread::sleep_for(400ms);
        q.push(3);
        q.push(4);
        std::this_thread::sleep_for(400ms);
        q.push(5);
        std::this_thread::sleep_for(400ms);
        q.push(6);
    });

    std::thread consumer([&q] {
        std::this_thread::sleep_for(200ms);
        REQUIRE_EQ(q.pop(), fplus::just(1));
        REQUIRE_EQ(q.pop(), fplus::just(2));
        REQUIRE_EQ(q.pop(), fplus::nothing<int>());
        std::this_thread::sleep_for(400ms);
        REQUIRE_EQ(q.pop_all(), std::vector<int>({3, 4}));
        REQUIRE_EQ(q.pop_all(), std::vector<int>({}));
        REQUIRE_EQ(q.wait_and_pop_all(), std::vector<int>({5}));
        REQUIRE_EQ(q.wait_for_and_pop_all(200000), std::vector<int>({}));
        REQUIRE_EQ(q.wait_for_and_pop_all(400000), std::vector<int>({6}));
        REQUIRE_EQ(q.wait_for_and_pop_all(200000), std::vector<int>({}));
    });

    producer.join();
    consumer.join();
}
