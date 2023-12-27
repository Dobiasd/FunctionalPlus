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

TEST_CASE("side_effects_test - for_each (serial and parallel")
{
    using namespace fplus;
    constexpr size_t nb_elems = 100;

    for (auto i : fplus::numbers(0, 3))
    {
        std::vector<int> xs(nb_elems, 0);
        auto idxs = fplus::numbers<size_t>(0, nb_elems);
        auto fill_one_cell = [&](size_t idx) { xs[idx] = 1; };
        if (i == 0)
            fplus::for_each(fill_one_cell, idxs);
        else if (i==1)
            fplus::parallel_for_each(fill_one_cell, idxs);
        else if (i==2)
            fplus::parallel_for_each_n_threads(size_t(4), fill_one_cell, idxs);
        size_t nb_filled = fplus::count_if([](int v) { return v== 1;}, xs);
        REQUIRE_EQ(nb_filled, nb_elems);
    }
}
