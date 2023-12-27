// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <doctest/doctest.h>
#include <fplus/fplus.hpp>
#include <vector>
#include <string>

TEST_CASE("replace_test - replace_if")
{
    auto is_even = [](int value) { return value % 2 == 0; };
    std::vector<int> v = { 1, 3, 4, 6, 7 };
    auto result = fplus::replace_if(is_even, 0, v);
    REQUIRE_EQ(result, std::vector<int>({1, 3, 0, 0, 7}));

    auto result_rvalue = fplus::replace_if(is_even, 0, std::vector<int>({ 1, 3, 4, 6, 7 }));
    REQUIRE_EQ(result_rvalue, std::vector<int>({1, 3, 0, 0, 7}));
}

TEST_CASE("replace_test - replace_elem_at_idx")
{
    std::vector<int> v = { 1, 3, 4, 4, 7 };
    auto result = fplus::replace_elem_at_idx(2, 0, v);
    REQUIRE_EQ(result, std::vector<int>({1, 3, 0, 4, 7}));

    auto result_rvalue = fplus::replace_elem_at_idx(2, 0, std::vector<int>({ 1, 3, 4, 4, 7 }));
    REQUIRE_EQ(result_rvalue, std::vector<int>({1, 3, 0, 4, 7}));
}

TEST_CASE("replace_test - replace_elems")
{
    std::vector<int> v = { 1, 3, 4, 4, 7 };
    auto result = fplus::replace_elems(4, 0, v);
    REQUIRE_EQ(result, std::vector<int>({1, 3, 0, 0, 7}));

    auto result_rvalue = fplus::replace_elems(4, 0, std::vector<int>({1, 3, 0, 0, 7}));
    REQUIRE_EQ(result_rvalue, std::vector<int>({1, 3, 0, 0, 7}));
}

TEST_CASE("replace_test - replace_tokens")
{
    const std::string source = "haha";
    const std::string dest = "hihi";
    const std::string input = "oh, hahaha!";
    auto result = fplus::replace_tokens(source, dest, input);
    REQUIRE_EQ(result, std::string("oh, hihiha!"));
}
