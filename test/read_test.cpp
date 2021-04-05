// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <doctest/doctest.h>
#include <fplus/fplus.hpp>
#include <vector>

TEST_CASE("read_test - read_value")
{
    using namespace fplus;
    REQUIRE_EQ(read_value<std::size_t>("42"), just<std::size_t>(42));
    REQUIRE_EQ(read_value<std::string>("foo"), just<std::string>("foo"));
    REQUIRE_EQ(read_value<unsigned long>("42"), just<unsigned long>(42));
    REQUIRE_EQ(read_value<unsigned long long>("42"), just<unsigned long long>(42));
    REQUIRE_EQ(read_value<int>("42"), just<int>(42));
    REQUIRE_EQ(read_value<long>("42"), just<long>(42));
    REQUIRE_EQ(read_value<long long>("42"), just<long long>(42));
    REQUIRE_EQ(read_value<int>("-3"), just<int>(-3));
    REQUIRE_EQ(read_value<int>("twenty"), nothing<int>());
    REQUIRE_EQ(read_value<int>("3 thousand"), nothing<int>());
    REQUIRE_EQ(read_value_with_default<int>(3, "42"), 42);
    REQUIRE_EQ(read_value_with_default<int>(3, "foo"), 3);
    REQUIRE_EQ(read_value_with_default<int>(3, ""), 3);
    REQUIRE_EQ(read_value_unsafe<int>("42"), 42);

    REQUIRE(is_in_interval(-42.4f, -42.2f, unsafe_get_just(read_value<float>("-42.3"))));
    REQUIRE(is_in_interval(-42.4 , -42.2, unsafe_get_just(read_value<double>("-42.3"))));
    REQUIRE(is_in_interval(-42.4L, -42.2L, unsafe_get_just(read_value<long double>("-42.3"))));
}

TEST_CASE("read_test - read_value_result")
{
    using namespace fplus;
    REQUIRE_EQ(read_value_result<int>("42"), (ok<int, std::string>(42)));
    REQUIRE_EQ(read_value_result<int>("-3"), (ok<int, std::string>(-3)));
    REQUIRE(is_error(read_value_result<int>("twenty")));
    REQUIRE(is_error(read_value_result<int>("3 thousand")));
}
