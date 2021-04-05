// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <doctest/doctest.h>
#include <fplus/fplus.hpp>
#include <vector>

TEST_CASE("side_effects_test - execute")
{
    using namespace fplus;
    std::vector<int> buffer;
    auto push_one_return_true = [&]() -> bool { buffer.push_back(1); return true; };
    execute_effect(push_one_return_true);
    REQUIRE_EQ(execute_effect(push_one_return_true), true);
    REQUIRE_EQ(buffer, std::vector<int>({1,1}));
}

TEST_CASE("side_effects_test - execute_serially")
{
    using namespace fplus;
    std::vector<int> buffer;
    auto push_one_return_true = [&]() -> bool { buffer.push_back(1); return true; };
    auto push_3_ones = execute_serially(replicate(3, push_one_return_true))();
    REQUIRE_EQ(buffer, std::vector<int>({1,1,1}));
}

TEST_CASE("side_effects_test - execute_parallelly")
{
    using namespace fplus;
    auto return_one = [&](){ return 1; };
    REQUIRE_EQ(execute_parallelly(replicate(4, return_one))(), std::vector<int>({1,1,1,1}));
}

TEST_CASE("side_effects_test - execute_max_n_times_until_success")
{
    using namespace fplus;
    std::vector<int> buffer;
    auto push_one_return_false = [&]() -> bool { buffer.push_back(1); return false; };
    execute_max_n_times_until_success(5, push_one_return_false)();
    execute_max_n_times_until_success(3, push_one_return_false, 1)();
    REQUIRE_EQ(buffer, std::vector<int>({1,1,1,1,1,1,1,1}));
}

TEST_CASE("side_effects_test - execute_serially_until_success")
{
    using namespace fplus;
    std::vector<int> buffer;
    auto push_one_return_true = [&]() -> bool { buffer.push_back(1); return true; };
    auto push_one_return_false = [&]() -> bool { buffer.push_back(1); return false; };
    typedef std::function<bool()> BoolReturningFunction;
    typedef std::vector<BoolReturningFunction> BoolReturningFunctions;
    execute_serially_until_success(BoolReturningFunctions({push_one_return_false, push_one_return_false, push_one_return_true, push_one_return_true}))();
    REQUIRE_EQ(buffer, std::vector<int>({1,1,1}));
}

TEST_CASE("side_effects_test - execute_serially_until_failure")
{
    using namespace fplus;
    std::vector<int> buffer;
    auto push_one_return_true = [&]() -> bool { buffer.push_back(1); return true; };
    auto push_one_return_false = [&]() -> bool { buffer.push_back(1); return false; };
    typedef std::function<bool()> BoolReturningFunction;
    typedef std::vector<BoolReturningFunction> BoolReturningFunctions;
    execute_serially_until_failure(BoolReturningFunctions({push_one_return_true, push_one_return_false, push_one_return_false}))();
    REQUIRE_EQ(buffer, std::vector<int>({1,1}));
}

TEST_CASE("side_effects_test - execute_parallelly_atomic")
{
    using namespace fplus;
    std::atomic<int> atomic_int(0);
    auto inc_atomic_int_return_true = [&]() -> bool { ++atomic_int; return true;};
    execute_parallelly(replicate(4, inc_atomic_int_return_true))();
    REQUIRE_EQ(atomic_int.load(), 4);
}
