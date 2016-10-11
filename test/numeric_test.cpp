// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "fplus/fplus.hpp"
#include <vector>

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
    typedef std::vector<double> Floats;
    typedef std::vector<double> Doubles;
}

TEST_CASE("numeric_test, is_in_range")
{
    REQUIRE(fplus::is_in_range(1, 3)(1));
    REQUIRE(fplus::is_in_range(1, 3)(2));
    REQUIRE_FALSE(fplus::is_in_range(1, 3)(0));
    REQUIRE_FALSE(fplus::is_in_range(1, 3)(3));
    REQUIRE(fplus::is_in_closed_range(1, 3)(1));
    REQUIRE(fplus::is_in_closed_range(1, 3)(2));
    REQUIRE(fplus::is_in_closed_range(1, 3)(3));
    REQUIRE_FALSE(fplus::is_in_open_range(1, 3)(1));
    REQUIRE(fplus::is_in_open_range(1, 3)(2));
    REQUIRE_FALSE(fplus::is_in_open_range(1, 3)(3));
    REQUIRE(fplus::is_in_range(0.09, 0.11)(fplus::abs(-0.1)));
    REQUIRE(fplus::is_in_range(0.09, 0.11)(fplus::abs( 0.1)));
}

TEST_CASE("numeric_test, is_in_range_around")
{
    REQUIRE_FALSE(fplus::is_in_range_around(0.1, 2.0)(1.85));
    REQUIRE(fplus::is_in_range_around(0.1, 2.0)(1.95));
    REQUIRE(fplus::is_in_range_around(0.1, 2.0)(2.05));
    REQUIRE_FALSE(fplus::is_in_range_around(0.1, 2.0)(2.15));
}

TEST_CASE("numeric_test, is_negative")
{
    REQUIRE(fplus::is_negative(-0.1));
    REQUIRE_FALSE(fplus::is_negative(0.1));
}

TEST_CASE("numeric_test, is_positive")
{
    REQUIRE(fplus::is_positive(0.1));
    REQUIRE_FALSE(fplus::is_positive(-0.1));
}

TEST_CASE("numeric_test, sign")
{
    REQUIRE_EQ(fplus::sign(0.1), 1);
    REQUIRE_EQ(fplus::sign(-0.1), -1);
}

TEST_CASE("numeric_test, cyclic_value")
{
    using namespace fplus;
    REQUIRE_EQ(cyclic_value(8)(3), 3);
    REQUIRE_EQ(cyclic_value(8)(11), 3);
    REQUIRE_EQ(cyclic_value(8)(19), 3);
    REQUIRE_EQ(cyclic_value(8)(-2), 6);
    REQUIRE_EQ(cyclic_value(8)(-5), 3);
    REQUIRE_EQ(cyclic_value(8)(-13), 3);
    REQUIRE(is_in_range(3.19, 3.21)(cyclic_value(8.1)(3.2)));
}

TEST_CASE("numeric_test, cyclic_difference")
{
    using namespace fplus;
    REQUIRE_EQ(cyclic_difference(100)(5, 2), 3);
    REQUIRE_EQ(cyclic_difference(100)(2, 5), 97);
    REQUIRE_EQ(cyclic_difference(100)(3, -2), 5);
    REQUIRE_EQ(cyclic_difference(100)(-2, 3), 95);
    REQUIRE_EQ(cyclic_difference(100)(90, 10), 80);
    REQUIRE_EQ(cyclic_difference(100)(10, 90), 20);
}

TEST_CASE("numeric_test, cyclic_shortest_difference")
{
    using namespace fplus;
    REQUIRE_EQ(cyclic_shortest_difference(100)(5, 2), 3);
    REQUIRE_EQ(cyclic_shortest_difference(100)(2, 5), -3);
    REQUIRE_EQ(cyclic_shortest_difference(100)(3, -2), 5);
    REQUIRE_EQ(cyclic_shortest_difference(100)(-2, 3), -5);
    REQUIRE_EQ(cyclic_shortest_difference(100)(90, 10), -20);
    REQUIRE_EQ(cyclic_shortest_difference(100)(10, 90), 20);
}

TEST_CASE("numeric_test, cyclic_distance")
{
    using namespace fplus;
    REQUIRE_EQ(cyclic_distance(100)(2, 5), 3);
    REQUIRE_EQ(cyclic_distance(100)(5, 2), 3);
    REQUIRE_EQ(cyclic_distance(100)(-2, 3), 5);
    REQUIRE_EQ(cyclic_distance(100)(3, -2), 5);
    REQUIRE_EQ(cyclic_distance(100)(10, 90), 20);
    REQUIRE_EQ(cyclic_distance(100)(90, 10), 20);
}

TEST_CASE("numeric_test, round")
{
    using namespace fplus;
    REQUIRE_EQ(round<int>(1.4), 1);
    REQUIRE_EQ(round<int>(1.5), 2);
    REQUIRE_EQ(round<int>(1.6), 2);

    REQUIRE_EQ(round<unsigned char>(300.0), 255);
    REQUIRE_EQ(round<unsigned char>(-5.0), 0);

    REQUIRE_EQ(round<int>(-1.4), -1);
    REQUIRE_EQ(round<int>(-1.6), -2);
}

TEST_CASE("numeric_test, ceil")
{
    using namespace fplus;
    REQUIRE_EQ(ceil<int>(1.4), 2);
    REQUIRE_EQ(ceil<int>(-1.4), -1);
}

TEST_CASE("numeric_test, floor")
{
    using namespace fplus;
    REQUIRE_EQ(floor<int>(1.4), 1);
    REQUIRE_EQ(floor<int>(-1.4), -2);
}

TEST_CASE("numeric_test, clamp")
{
    using namespace fplus;
    REQUIRE_EQ(clamp(2, 6)(5), 5);
    REQUIRE_EQ(clamp(2, 6)(1), 2);
    REQUIRE_EQ(clamp(2, 6)(8), 6);
}

TEST_CASE("numeric_test, int_power")
{
    using namespace fplus;
    REQUIRE_EQ(int_power(3, 0), 1);
    REQUIRE_EQ(int_power(3, 1), 3);
    REQUIRE_EQ(int_power(3, 2), 9);
    REQUIRE_EQ(int_power(3, 3), 27);
    REQUIRE_EQ(int_power(3, 4), 81);
}

TEST_CASE("numeric_test, min_on")
{
    REQUIRE_EQ(fplus::min_on(mod2)(4, 4), 4);
    REQUIRE_EQ(fplus::min_on(mod2)(4, 3), 4);
    REQUIRE_EQ(fplus::min_on(mod2)(4, 3, 7), 4);
    REQUIRE_EQ(fplus::min_on(mod2)(5, 3, 7), 5);
    REQUIRE_EQ(fplus::min_on(mod2)(5, 3, 7, 9, 2), 2);
    REQUIRE_EQ(fplus::min_on(mod2)(5, 3, 7, 13, 19, 4), 4);

    REQUIRE_EQ(fplus::min_on(mod7)(4, 4), 4);
    REQUIRE_EQ(fplus::min_on(mod7)(4, 3), 3);
    REQUIRE_EQ(fplus::min_on(mod7)(4, 3, 7), 7);
    REQUIRE_EQ(fplus::min_on(mod7)(5, 3, 7, 9, 9), 7);
    REQUIRE_EQ(fplus::min_on(mod7)(5, 3, 7, 9, 9, 6, 6, 6, 6, 6, 6), 7);
    REQUIRE_EQ(fplus::min_on(mod7)(70, 3, 7, 9, 9, 6, 6, 6, 6, 6, 6), 70);

    const std::string s1("AAA");
    const std::string s2("AAABB");
    const std::string s3("AAABBCCC");
    REQUIRE_EQ(fplus::min_on(string_length)(s1, s2), s1);
    REQUIRE_EQ(fplus::min_on(string_length)(s2, s3), s2);
    REQUIRE_EQ(fplus::min_on(string_length)(s1, s2, s3), s1);
    REQUIRE_EQ(fplus::min_on(string_length)(s1, s3), s1);

    auto l1_min_on = fplus::min_on(mod7);
    REQUIRE_EQ(l1_min_on(1), 1);
    REQUIRE_EQ(l1_min_on(1, 2), 1);
    REQUIRE_EQ(l1_min_on(1, 2, 3, 7), 7);
    REQUIRE_EQ(l1_min_on(1, 2, 3, 6, 77), 77);
    REQUIRE_EQ(fplus::min_on(mod7)(1), 1);
    REQUIRE_EQ(fplus::min_on(mod7)(1, 2), 1);
    REQUIRE_EQ(fplus::min_on(mod7)(1, 2, 3, 7), 7);
    REQUIRE_EQ(fplus::min_on(mod7)(1, 2, 3, 6, 77), 77);
}

TEST_CASE("numeric_test, max_on")
{
    REQUIRE_EQ(fplus::max_on(mod2)(4, 4), 4);
    REQUIRE_EQ(fplus::max_on(mod2)(4, 3), 3);
    REQUIRE_EQ(fplus::max_on(mod2)(4, 3, 7), 3);
    REQUIRE_EQ(fplus::max_on(mod2)(5, 3, 7), 5);
    REQUIRE_EQ(fplus::max_on(mod2)(5, 3, 7, 9, 2), 5);
    REQUIRE_EQ(fplus::max_on(mod2)(5, 3, 7, 13, 19, 4), 5);

    REQUIRE_EQ(fplus::max_on(mod7)(4, 4), 4);
    REQUIRE_EQ(fplus::max_on(mod7)(4, 3), 4);
    REQUIRE_EQ(fplus::max_on(mod7)(4, 3, 7), 4);
    REQUIRE_EQ(fplus::max_on(mod7)(5, 3, 7, 9, 9), 5);
    REQUIRE_EQ(fplus::max_on(mod7)(5, 3, 7, 9, 9, 6, 6, 6, 6, 6, 6), 6);
    REQUIRE_EQ(fplus::max_on(mod7)(70, 3, 7, 9, 9, 6, 6, 6, 6, 6, 6), 6);

    const std::string s1("AAA");
    const std::string s2("AAABB");
    const std::string s3("AAABBCCC");
    REQUIRE_EQ(fplus::max_on(string_length)(s1, s2), s2);
    REQUIRE_EQ(fplus::max_on(string_length)(s2, s3), s3);
    REQUIRE_EQ(fplus::max_on(string_length)(s1, s2, s3), s3);
    REQUIRE_EQ(fplus::max_on(string_length)(s1, s3), s3);

    auto l1_max_on = fplus::max_on(mod7);
    REQUIRE_EQ(l1_max_on(1), 1);
    REQUIRE_EQ(l1_max_on(1, 2), 2);
    REQUIRE_EQ(l1_max_on(1, 2, 3, 7), 3);
    REQUIRE_EQ(l1_max_on(1, 2, 3, 6, 77), 6);
    REQUIRE_EQ(fplus::max_on(mod7)(1), 1);
    REQUIRE_EQ(fplus::max_on(mod7)(1, 2), 2);
    REQUIRE_EQ(fplus::max_on(mod7)(1, 2, 3, 7), 3);
    REQUIRE_EQ(fplus::max_on(mod7)(1, 2, 3, 6, 77), 6);
}

TEST_CASE("numeric_test, mean")
{
    using namespace fplus;
    Ints xs = {1,4,4};
    REQUIRE_EQ(mean<int>(xs), 3);
}

TEST_CASE("numeric_test, mean_obj")
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
    REQUIRE(is_in_range_around(0.001, mean_vec_length_squared_dest)(
        vec_2d_length_squared(mean_vec_div_double)));
    REQUIRE(is_in_range_around(0.001, mean_vec_length_squared_dest)(
        vec_2d_length_squared(mean_vec_div_size_t)));
}

TEST_CASE("numeric_test, variadic")
{
    using namespace fplus;
    REQUIRE_EQ(min(1,2,3,4,5), 1);
    REQUIRE_EQ(min(1.01,1.02,1.03,1.04,1.05), 1.01);
    REQUIRE_EQ(min(-54,2,3,54,5), -54);
    REQUIRE_EQ(min(-54.2,2.7,3,54,5), -54.2);
    REQUIRE_EQ(min(123,123,123,124), 123);
    REQUIRE_EQ(min(123), 123);
    REQUIRE_EQ(min(123,123), 123);
    REQUIRE_EQ(min(123,123,123), 123);
    REQUIRE_EQ(min(-1), -1);
    REQUIRE_EQ(min(-1,-2), -2);
    REQUIRE_EQ(min(-1,-2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0), -2);
    REQUIRE_EQ(min('a','b','c'), 'a');

    REQUIRE_EQ(max(1,2,3,4,5), 5);
    REQUIRE_EQ(max(1.01,1.02,1.03,1.04,1.05), 1.05);
    REQUIRE_EQ(max(-54,2,3,54,5), 54);
    REQUIRE_EQ(max(-54.2,2.7,3,54.85,5), 54.85);
    REQUIRE_EQ(max(123,123,123,124), 124);
    REQUIRE_EQ(max(123), 123);
    REQUIRE_EQ(max(123,123), 123);
    REQUIRE_EQ(max(123,123,123), 123);
    REQUIRE_EQ(max(123,123,123,123), 123);
    REQUIRE_EQ(max(123,123,123,123,123), 123);
    REQUIRE_EQ(max(123,123,123,123,123,123), 123);
    REQUIRE_EQ(max(-1), -1);
    REQUIRE_EQ(max(-1,-2), -1);
    REQUIRE_EQ(max(-1,-2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0), 0);
    REQUIRE_EQ(max('a','b','c'), 'c');
}

TEST_CASE("numeric_test, normalize")
{
    using namespace fplus;

    REQUIRE_EQ(normalize_min_max(0, 10, Doubles({1, 3, 6})), Doubles({0, 4, 10}));
    REQUIRE_EQ(normalize_mean_stddev(3, 2, Doubles({7, 8})), Doubles({1, 5}));
    REQUIRE_EQ(standardize(Doubles({2.0, 6.0})), Doubles({-1, 1}));

    REQUIRE_EQ(normalize_min_max(0, 10, Floats({1, 3, 6})), Floats({0, 4, 10}));
    REQUIRE_EQ(normalize_mean_stddev(3, 2, Floats({7, 8})), Floats({1, 5}));
    REQUIRE_EQ(standardize(Floats({2.0, 6.0})), Floats({-1, 1}));
}
