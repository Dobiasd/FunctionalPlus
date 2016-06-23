// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "fplus/fplus.h"
#include <vector>
#include <string>

using namespace testing;

TEST(replace_test, replace_if)
{
    auto is_even = [](int value) { return value % 2 == 0; };
    std::vector<int> v = { 1, 3, 4, 6, 7 };
    auto result = fplus::replace_if(is_even, 0, v);
    EXPECT_THAT(result, ElementsAre(1, 3, 0, 0, 7));
}

TEST(replace_test, replace_elems)
{
    std::vector<int> v = { 1, 3, 4, 4, 7 };
    auto result = fplus::replace_elems(4, 0, v);
    EXPECT_THAT(result, ElementsAre(1, 3, 0, 0, 7));
}

TEST(replace_test, replace_tokens)
{
    const std::string source = "haha";
    const std::string dest = "hihi";
    const std::string input = "oh, hahaha!";
    auto result = fplus::replace_tokens(source, dest, input);
    EXPECT_THAT(result, Eq("oh, hihiha!"));
}
