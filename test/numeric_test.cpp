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

namespace {

    int mod2(int x)
    {
        return x % 2;
    }

    int mod7(int x)
    {
        return x % 7;
    }

    std::size_t string_length(const std::string& s)
    {
        return s.size();
    }

    typedef std::vector<int> Ints;
}

TEST(numeric_test, is_in_range)
{
    EXPECT_TRUE(fplus::is_in_range(1, 3)(1));
    EXPECT_TRUE(fplus::is_in_range(1, 3)(2));
    EXPECT_FALSE(fplus::is_in_range(1, 3)(0));
    EXPECT_FALSE(fplus::is_in_range(1, 3)(3));
    EXPECT_TRUE(fplus::is_in_closed_range(1, 3)(1));
    EXPECT_TRUE(fplus::is_in_closed_range(1, 3)(2));
    EXPECT_TRUE(fplus::is_in_closed_range(1, 3)(3));
    EXPECT_FALSE(fplus::is_in_open_range(1, 3)(1));
    EXPECT_TRUE(fplus::is_in_open_range(1, 3)(2));
    EXPECT_FALSE(fplus::is_in_open_range(1, 3)(3));
    EXPECT_TRUE(fplus::is_in_range(0.09, 0.11)(fplus::abs(-0.1)));
    EXPECT_TRUE(fplus::is_in_range(0.09, 0.11)(fplus::abs( 0.1)));
}

TEST(numeric_test, is_negative)
{
    EXPECT_TRUE(fplus::is_negative(-0.1));
    EXPECT_FALSE(fplus::is_negative(0.1));
}

TEST(numeric_test, is_positive)
{
    EXPECT_TRUE(fplus::is_positive(0.1));
    EXPECT_FALSE(fplus::is_positive(-0.1));
}

TEST(numeric_test, sign)
{
    EXPECT_THAT(fplus::sign(0.1), Eq(1));
    EXPECT_THAT(fplus::sign(-0.1), Eq(-1));
}

TEST(numeric_test, cyclic_value)
{
    using namespace fplus;
    EXPECT_THAT(cyclic_value(8)(3), Eq(3));
    EXPECT_THAT(cyclic_value(8)(11), Eq(3));
    EXPECT_THAT(cyclic_value(8)(19), Eq(3));
    EXPECT_THAT(cyclic_value(8)(-2), Eq(6));
    EXPECT_THAT(cyclic_value(8)(-5), Eq(3));
    EXPECT_THAT(cyclic_value(8)(-13), Eq(3));
    EXPECT_TRUE(is_in_range(3.19, 3.21)(cyclic_value(8.1)(3.2)));
}

TEST(numeric_test, cyclic_difference)
{
    using namespace fplus;
    EXPECT_THAT(cyclic_difference(100)(5, 2), Eq(3));
    EXPECT_THAT(cyclic_difference(100)(2, 5), Eq(97));
    EXPECT_THAT(cyclic_difference(100)(3, -2), Eq(5));
    EXPECT_THAT(cyclic_difference(100)(-2, 3), Eq(95));
    EXPECT_THAT(cyclic_difference(100)(90, 10), Eq(80));
    EXPECT_THAT(cyclic_difference(100)(10, 90), Eq(20));
}

TEST(numeric_test, cyclic_shortest_difference)
{
    using namespace fplus;
    EXPECT_THAT(cyclic_shortest_difference(100)(5, 2), Eq(3));
    EXPECT_THAT(cyclic_shortest_difference(100)(2, 5), Eq(-3));
    EXPECT_THAT(cyclic_shortest_difference(100)(3, -2), Eq(5));
    EXPECT_THAT(cyclic_shortest_difference(100)(-2, 3), Eq(-5));
    EXPECT_THAT(cyclic_shortest_difference(100)(90, 10), Eq(-20));
    EXPECT_THAT(cyclic_shortest_difference(100)(10, 90), Eq(20));
}

TEST(numeric_test, cyclic_distance)
{
    using namespace fplus;
    EXPECT_THAT(cyclic_distance(100)(2, 5), Eq(3));
    EXPECT_THAT(cyclic_distance(100)(5, 2), Eq(3));
    EXPECT_THAT(cyclic_distance(100)(-2, 3), Eq(5));
    EXPECT_THAT(cyclic_distance(100)(3, -2), Eq(5));
    EXPECT_THAT(cyclic_distance(100)(10, 90), Eq(20));
    EXPECT_THAT(cyclic_distance(100)(90, 10), Eq(20));
}

TEST(numeric_test, round)
{
    using namespace fplus;
    EXPECT_THAT(round<int>(1.4), Eq(1));
    EXPECT_THAT(round<int>(1.5), Eq(2));
    EXPECT_THAT(round<int>(1.6), Eq(2));

    EXPECT_THAT(round<unsigned char>(300.0), Eq(255));
    EXPECT_THAT(round<unsigned char>(-5.0), Eq(0));

    EXPECT_THAT(round<int>(-1.4), Eq(-1));
    EXPECT_THAT(round<int>(-1.6), Eq(-2));
}

TEST(numeric_test, ceil)
{
    using namespace fplus;
    EXPECT_THAT(ceil<int>(1.4), Eq(2));
    EXPECT_THAT(ceil<int>(-1.4), Eq(-1));
}

TEST(numeric_test, floor)
{
    using namespace fplus;
    EXPECT_THAT(floor<int>(1.4), Eq(1));
    EXPECT_THAT(floor<int>(-1.4), Eq(-2));
}

TEST(numeric_test, clamp)
{
    using namespace fplus;
    EXPECT_THAT(clamp(2, 6)(5), Eq(5));
    EXPECT_THAT(clamp(2, 6)(1), Eq(2));
    EXPECT_THAT(clamp(2, 6)(8), Eq(6));
}

TEST(numeric_test, int_power)
{
    using namespace fplus;
    EXPECT_THAT(int_power(3, 0), Eq(1));
    EXPECT_THAT(int_power(3, 1), Eq(3));
    EXPECT_THAT(int_power(3, 2), Eq(9));
    EXPECT_THAT(int_power(3, 3), Eq(27));
    EXPECT_THAT(int_power(3, 4), Eq(81));
}

TEST(numeric_test, min_on)
{
    EXPECT_THAT(fplus::min_on(mod2)(4, 4), Eq(4));
    EXPECT_THAT(fplus::min_on(mod2)(4, 3), Eq(4));
    EXPECT_THAT(fplus::min_on(mod2)(4, 3, 7), Eq(4));
    EXPECT_THAT(fplus::min_on(mod2)(5, 3, 7), Eq(5));
    EXPECT_THAT(fplus::min_on(mod2)(5, 3, 7, 9, 2), Eq(2));
    EXPECT_THAT(fplus::min_on(mod2)(5, 3, 7, 13, 19, 4), Eq(4));

    EXPECT_THAT(fplus::min_on(mod7)(4, 4), Eq(4));
    EXPECT_THAT(fplus::min_on(mod7)(4, 3), Eq(3));
    EXPECT_THAT(fplus::min_on(mod7)(4, 3, 7), Eq(7));
    EXPECT_THAT(fplus::min_on(mod7)(5, 3, 7, 9, 9), Eq(7));
    EXPECT_THAT(fplus::min_on(mod7)(5, 3, 7, 9, 9, 6, 6, 6, 6, 6, 6), Eq(7));
    EXPECT_THAT(fplus::min_on(mod7)(70, 3, 7, 9, 9, 6, 6, 6, 6, 6, 6), Eq(70));

    const std::string s1("AAA");
    const std::string s2("AAABB");
    const std::string s3("AAABBCCC");
    EXPECT_THAT(fplus::min_on(string_length)(s1, s2), Eq(s1));
    EXPECT_THAT(fplus::min_on(string_length)(s2, s3), Eq(s2));
    EXPECT_THAT(fplus::min_on(string_length)(s1, s2, s3), Eq(s1));
    EXPECT_THAT(fplus::min_on(string_length)(s1, s3), Eq(s1));

    auto l1_min_on = fplus::min_on(mod7);
    EXPECT_THAT(l1_min_on(1), Eq(1));
    EXPECT_THAT(l1_min_on(1, 2), Eq(1));
    EXPECT_THAT(l1_min_on(1, 2, 3, 7), Eq(7));
    EXPECT_THAT(l1_min_on(1, 2, 3, 6, 77), Eq(77));
    EXPECT_THAT(fplus::min_on(mod7)(1), Eq(1));
    EXPECT_THAT(fplus::min_on(mod7)(1, 2), Eq(1));
    EXPECT_THAT(fplus::min_on(mod7)(1, 2, 3, 7), Eq(7));
    EXPECT_THAT(fplus::min_on(mod7)(1, 2, 3, 6, 77), Eq(77));
}

TEST(numeric_test, max_on)
{
    EXPECT_THAT(fplus::max_on(mod2)(4, 4), Eq(4));
    EXPECT_THAT(fplus::max_on(mod2)(4, 3), Eq(3));
    EXPECT_THAT(fplus::max_on(mod2)(4, 3, 7), Eq(3));
    EXPECT_THAT(fplus::max_on(mod2)(5, 3, 7), Eq(5));
    EXPECT_THAT(fplus::max_on(mod2)(5, 3, 7, 9, 2), Eq(5));
    EXPECT_THAT(fplus::max_on(mod2)(5, 3, 7, 13, 19, 4), Eq(5));

    EXPECT_THAT(fplus::max_on(mod7)(4, 4), Eq(4));
    EXPECT_THAT(fplus::max_on(mod7)(4, 3), Eq(4));
    EXPECT_THAT(fplus::max_on(mod7)(4, 3, 7), Eq(4));
    EXPECT_THAT(fplus::max_on(mod7)(5, 3, 7, 9, 9), Eq(5));
    EXPECT_THAT(fplus::max_on(mod7)(5, 3, 7, 9, 9, 6, 6, 6, 6, 6, 6), Eq(6));
    EXPECT_THAT(fplus::max_on(mod7)(70, 3, 7, 9, 9, 6, 6, 6, 6, 6, 6), Eq(6));

    const std::string s1("AAA");
    const std::string s2("AAABB");
    const std::string s3("AAABBCCC");
    EXPECT_THAT(fplus::max_on(string_length)(s1, s2), Eq(s2));
    EXPECT_THAT(fplus::max_on(string_length)(s2, s3), Eq(s3));
    EXPECT_THAT(fplus::max_on(string_length)(s1, s2, s3), Eq(s3));
    EXPECT_THAT(fplus::max_on(string_length)(s1, s3), Eq(s3));

    auto l1_max_on = fplus::max_on(mod7);
    EXPECT_THAT(l1_max_on(1), Eq(1));
    EXPECT_THAT(l1_max_on(1, 2), Eq(2));
    EXPECT_THAT(l1_max_on(1, 2, 3, 7), Eq(3));
    EXPECT_THAT(l1_max_on(1, 2, 3, 6, 77), Eq(6));
    EXPECT_THAT(fplus::max_on(mod7)(1), Eq(1));
    EXPECT_THAT(fplus::max_on(mod7)(1, 2), Eq(2));
    EXPECT_THAT(fplus::max_on(mod7)(1, 2, 3, 7), Eq(3));
    EXPECT_THAT(fplus::max_on(mod7)(1, 2, 3, 6, 77), Eq(6));
}

TEST(numeric_test, mean)
{
    using namespace fplus;
    Ints xs = {1,4,4};
    EXPECT_THAT(mean<int>(xs), Eq(3));
}

TEST(numeric_test, mean_obj)
{
    using namespace fplus;
    struct vec_2d
    {
        double x;
        double y;
        vec_2d operator + (const vec_2d& rhs) const
        {
            return {x + rhs.x, y + rhs.y};
        };
        vec_2d operator / (double scalar) const
        {
            double scalar_d = static_cast<double>(scalar);
            return {x / scalar_d, y / scalar_d};
        };
        vec_2d operator / (std::size_t scalar) const
        {
            return *this / static_cast<double>(scalar);
        };
    };

    auto vec_2d_length_squared = [](const vec_2d& v) -> double
    {
        return v.x * v.x + v.y * v.y;
    };
    std::vector<vec_2d> vecs = {{1,1}, {3,3}};
    auto mean_vec_div_double = mean_obj_div_double(vecs);
    auto mean_vec_div_size_t = mean_obj_div_size_t(vecs);
    double mean_vec_length_squared_dest = 2*2 + 2*2;
    EXPECT_THAT(vec_2d_length_squared(mean_vec_div_double),
        DoubleEq(mean_vec_length_squared_dest));
    EXPECT_THAT(vec_2d_length_squared(mean_vec_div_size_t),
        DoubleEq(mean_vec_length_squared_dest));
}

TEST(numeric_test, variadic)
{
    using namespace fplus;
    EXPECT_THAT(min(1,2,3,4,5), Eq(1));
    EXPECT_THAT(min(1.01,1.02,1.03,1.04,1.05), Eq(1.01));
    EXPECT_THAT(min(-54,2,3,54,5), Eq(-54));
    EXPECT_THAT(min(-54.2,2.7,3,54,5), Eq(-54.2));
    EXPECT_THAT(min(123,123,123,124), Eq(123));
    EXPECT_THAT(min(123), Eq(123));
    EXPECT_THAT(min(123,123), Eq(123));
    EXPECT_THAT(min(123,123,123), Eq(123));
    EXPECT_THAT(min(-1), Eq(-1));
    EXPECT_THAT(min(-1,-2), Eq(-2));
    EXPECT_THAT(min(-1,-2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0), Eq(-2));
    EXPECT_THAT(min('a','b','c'), Eq('a'));

    EXPECT_THAT(max(1,2,3,4,5), Eq(5));
    EXPECT_THAT(max(1.01,1.02,1.03,1.04,1.05), Eq(1.05));
    EXPECT_THAT(max(-54,2,3,54,5), Eq(54));
    EXPECT_THAT(max(-54.2,2.7,3,54.85,5), Eq(54.85));
    EXPECT_THAT(max(123,123,123,124), Eq(124));
    EXPECT_THAT(max(123), Eq(123));
    EXPECT_THAT(max(123,123), Eq(123));
    EXPECT_THAT(max(123,123,123), Eq(123));
    EXPECT_THAT(max(123,123,123,123), Eq(123));
    EXPECT_THAT(max(123,123,123,123,123), Eq(123));
    EXPECT_THAT(max(123,123,123,123,123,123), Eq(123));
    EXPECT_THAT(max(-1), Eq(-1));
    EXPECT_THAT(max(-1,-2), Eq(-1));
    EXPECT_THAT(max(-1,-2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0), Eq(0));
    EXPECT_THAT(max('a','b','c'), Eq('c'));
}
