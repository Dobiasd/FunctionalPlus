// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "fplus/fplus.h"
#include <vector>

using namespace testing;

TEST(read_test, read_value)
{
    using namespace fplus;
    EXPECT_EQ(read_value<std::size_t>("42"), just<std::size_t>(42));
    EXPECT_EQ(read_value<unsigned long>("42"), just<unsigned long>(42));
    EXPECT_EQ(read_value<unsigned long long>("42"), just<unsigned long long>(42));
    EXPECT_EQ(read_value<int>("42"), just<int>(42));
    EXPECT_EQ(read_value<long>("42"), just<long>(42));
    EXPECT_EQ(read_value<long long>("42"), just<long long>(42));
    EXPECT_EQ(read_value<int>("-3"), just<int>(-3));
    EXPECT_EQ(read_value<int>("twenty"), nothing<int>());
    EXPECT_EQ(read_value<int>("3 thousand"), nothing<int>());

    EXPECT_TRUE(is_in_range(-42.4f, -42.2f)(unsafe_get_just(read_value<float>("-42.3"))));
    EXPECT_TRUE(is_in_range(-42.4 , -42.2 )(unsafe_get_just(read_value<double>("-42.3"))));
    EXPECT_TRUE(is_in_range(-42.4L, -42.2L )(unsafe_get_just(read_value<long double>("-42.3"))));
}

TEST(read_test, read_value_result)
{
    using namespace fplus;
    EXPECT_EQ(read_value_result<int>("42"), (ok<int, std::string>(42)));
    EXPECT_EQ(read_value_result<int>("-3"), (ok<int, std::string>(-3)));
    EXPECT_TRUE(is_error(read_value_result<int>("twenty")));
    EXPECT_TRUE(is_error(read_value_result<int>("3 thousand")));
}
