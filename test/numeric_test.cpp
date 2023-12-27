// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <doctest/doctest.h>
#include <fplus/fplus.hpp>
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
    typedef std::vector<float> Floats;
    typedef std::vector<double> Doubles;
}

TEST_CASE("numeric_test - is_in_interval")
{
    REQUIRE(fplus::is_in_interval(1, 3, 1));
    REQUIRE(fplus::is_in_interval(1, 3, 2));
    REQUIRE_FALSE(fplus::is_in_interval(1, 3, 0));
    REQUIRE_FALSE(fplus::is_in_interval(1, 3, 3));
    REQUIRE(fplus::is_in_closed_interval(1, 3, 1));
    REQUIRE(fplus::is_in_closed_interval(1, 3, 2));
    REQUIRE(fplus::is_in_closed_interval(1, 3, 3));
    REQUIRE_FALSE(fplus::is_in_open_interval(1, 3, 1));
    REQUIRE(fplus::is_in_open_interval(1, 3, 2));
    REQUIRE_FALSE(fplus::is_in_open_interval(1, 3, 3));
    REQUIRE(fplus::is_in_interval(0.09, 0.11, fplus::abs(-0.1)));
    REQUIRE(fplus::is_in_interval(0.09, 0.11, fplus::abs( 0.1)));
}

TEST_CASE("numeric_test - is_in_interval_around")
{
    REQUIRE_FALSE(fplus::is_in_interval_around(0.1, 2.0, 1.85));
    REQUIRE(fplus::is_in_interval_around(0.1, 2.0, 1.95));
    REQUIRE(fplus::is_in_interval_around(0.1, 2.0, 2.05));
    REQUIRE_FALSE(fplus::is_in_interval_around(0.1, 2.0, 2.15));

    REQUIRE(fplus::is_in_interval_around(1, 2, 1));
    REQUIRE_FALSE(fplus::is_in_interval_around(1, 2, 3));
}

TEST_CASE("numeric_test - is_in_open_interval_around")
{
    REQUIRE_FALSE(fplus::is_in_open_interval_around(0.1, 2.0, 1.85));
    REQUIRE(fplus::is_in_open_interval_around(0.1, 2.0, 1.95));
    REQUIRE(fplus::is_in_open_interval_around(0.1, 2.0, 2.05));
    REQUIRE_FALSE(fplus::is_in_open_interval_around(0.1, 2.0, 2.15));

    REQUIRE_FALSE(fplus::is_in_open_interval_around(1, 2, 1));
    REQUIRE_FALSE(fplus::is_in_open_interval_around(1, 2, 3));
}

TEST_CASE("numeric_test - is_in_closed_interval_around")
{
    REQUIRE_FALSE(fplus::is_in_closed_interval_around(0.1, 2.0, 1.85));
    REQUIRE(fplus::is_in_closed_interval_around(0.1, 2.0, 1.95));
    REQUIRE(fplus::is_in_closed_interval_around(0.1, 2.0, 2.05));
    REQUIRE_FALSE(fplus::is_in_closed_interval_around(0.1, 2.0, 2.15));

    REQUIRE(fplus::is_in_closed_interval_around(1, 2, 1));
    REQUIRE(fplus::is_in_closed_interval_around(1, 2, 3));
}

TEST_CASE("numeric_test - is_negative")
{
    REQUIRE(fplus::is_negative(-0.1));
    REQUIRE_FALSE(fplus::is_negative(0.1));
}

TEST_CASE("numeric_test - is_positive")
{
    REQUIRE(fplus::is_positive(0.1));
    REQUIRE_FALSE(fplus::is_positive(-0.1));
}

TEST_CASE("numeric_test - is_even")
{
    REQUIRE(fplus::is_even(2));
    REQUIRE(fplus::is_even(-2));
    REQUIRE_FALSE(fplus::is_even(3));
    REQUIRE_FALSE(fplus::is_even(-3));
}

TEST_CASE("numeric_test - is_odd")
{
    REQUIRE(fplus::is_odd(1));
    REQUIRE(fplus::is_odd(-1));
    REQUIRE_FALSE(fplus::is_odd(2));
    REQUIRE_FALSE(fplus::is_odd(-2));
}

TEST_CASE("numeric_test - sign")
{
    REQUIRE_EQ(fplus::sign(0.1), 1);
    REQUIRE_EQ(fplus::sign(-0.1), -1);
}

TEST_CASE("numeric_test - abs_diff")
{
    REQUIRE_EQ(fplus::abs_diff<signed int>(3, 5), 2);
    REQUIRE_EQ(fplus::abs_diff<signed int>(5, 3), 2);
    REQUIRE_EQ(fplus::abs_diff<unsigned int>(3, 5), 2);
    REQUIRE_EQ(fplus::abs_diff<unsigned int>(5, 3), 2);
}

TEST_CASE("numeric_test - cyclic_value")
{
    using namespace fplus;
    REQUIRE_EQ(cyclic_value(8.0)(3), 3);
    REQUIRE_EQ(cyclic_value(8.0)(11), 3);
    REQUIRE_EQ(cyclic_value(8.0)(19), 3);
    REQUIRE_EQ(cyclic_value(8.0)(-2), 6);
    REQUIRE_EQ(cyclic_value(8.0)(-5), 3);
    REQUIRE_EQ(cyclic_value(8.0)(-13), 3);

    REQUIRE_EQ(cyclic_value(8)(3), 3);
    REQUIRE_EQ(cyclic_value(8)(11), 3);
    REQUIRE_EQ(cyclic_value(8)(19), 3);
    REQUIRE_EQ(cyclic_value(8)(-2), 6);
    REQUIRE_EQ(cyclic_value(8)(-5), 3);
    REQUIRE_EQ(cyclic_value(8)(-13), 3);

    REQUIRE_EQ(cyclic_value<unsigned int>(8)(3), 3);
    REQUIRE_EQ(cyclic_value<unsigned int>(8)(11), 3);
    REQUIRE_EQ(cyclic_value<unsigned int>(8)(19), 3);

    REQUIRE(is_in_interval(3.19, 3.21, cyclic_value(8.1)(3.2)));
}

TEST_CASE("numeric_test - cyclic_difference")
{
    using namespace fplus;
    REQUIRE_EQ(cyclic_difference(100)(5, 2), 3);
    REQUIRE_EQ(cyclic_difference(100)(2, 5), 97);
    REQUIRE_EQ(cyclic_difference(100)(3, -2), 5);
    REQUIRE_EQ(cyclic_difference(100)(-2, 3), 95);
    REQUIRE_EQ(cyclic_difference(100)(90, 10), 80);
    REQUIRE_EQ(cyclic_difference(100)(10, 90), 20);

    REQUIRE_EQ(cyclic_difference<unsigned int>(100)(5, 2), 3);
    REQUIRE_EQ(cyclic_difference<unsigned int>(100)(2, 5), 97);
    REQUIRE_EQ(cyclic_difference<unsigned int>(100)(90, 10), 80);
    REQUIRE_EQ(cyclic_difference<unsigned int>(100)(10, 90), 20);
}

TEST_CASE("numeric_test - cyclic_shortest_difference")
{
    using namespace fplus;
    REQUIRE_EQ(cyclic_shortest_difference(100)(5, 2), 3);
    REQUIRE_EQ(cyclic_shortest_difference(100)(2, 5), -3);
    REQUIRE_EQ(cyclic_shortest_difference(100)(3, -2), 5);
    REQUIRE_EQ(cyclic_shortest_difference(100)(-2, 3), -5);
    REQUIRE_EQ(cyclic_shortest_difference(100)(90, 10), -20);
    REQUIRE_EQ(cyclic_shortest_difference(100)(10, 90), 20);
}

TEST_CASE("numeric_test - cyclic_distance")
{
    using namespace fplus;
    REQUIRE_EQ(cyclic_distance(100)(2, 5), 3);
    REQUIRE_EQ(cyclic_distance(100)(5, 2), 3);
    REQUIRE_EQ(cyclic_distance(100)(-2, 3), 5);
    REQUIRE_EQ(cyclic_distance(100)(3, -2), 5);
    REQUIRE_EQ(cyclic_distance(100)(10, 90), 20);
    REQUIRE_EQ(cyclic_distance(100)(90, 10), 20);

    REQUIRE_EQ(cyclic_distance<unsigned int>(100)(2, 5), 3);
    REQUIRE_EQ(cyclic_distance<unsigned int>(100)(5, 2), 3);
    REQUIRE_EQ(cyclic_distance<unsigned int>(100)(10, 90), 20);
    REQUIRE_EQ(cyclic_distance<unsigned int>(100)(90, 10), 20);
}

TEST_CASE("numeric_test - round")
{
    // using namespace fplus;  // round is also defined in tgmath.h under msvc
    REQUIRE_EQ(round(1.4), 1);
    REQUIRE_EQ(round(1.5), 2);
    REQUIRE_EQ(round(1.6), 2);

    REQUIRE_EQ((fplus::round<double, unsigned char>(254.9)), 255);
    REQUIRE_EQ((fplus::round<double, unsigned char>(300.0)), 255);
    REQUIRE_EQ((fplus::round<double, unsigned char>(-0.0000001)), 0);
    REQUIRE_EQ((fplus::round<double, unsigned char>(-5.0)), 0);

    REQUIRE_EQ(fplus::round(-1.4), -1);
    REQUIRE_EQ(fplus::round(-1.6), -2);
}

TEST_CASE("numeric_test - integral_cast_clamp")
{
    using namespace fplus;
    REQUIRE_EQ(integral_cast_clamp<std::uint8_t>(std::int32_t(-1)), std::uint8_t(0));
    REQUIRE_EQ(integral_cast_clamp<std::uint8_t>(std::int32_t(0)), std::uint8_t(0));
    REQUIRE_EQ(integral_cast_clamp<std::uint8_t>(std::int32_t(3)), std::uint8_t(3));
    REQUIRE_EQ(integral_cast_clamp<std::uint8_t>(std::int32_t(255)), std::uint8_t(255));
    REQUIRE_EQ(integral_cast_clamp<std::uint8_t>(std::int32_t(256)), std::uint8_t(255));

    REQUIRE_EQ(integral_cast_clamp<std::int8_t>(std::int32_t(-129)), std::int8_t(-128));
    REQUIRE_EQ(integral_cast_clamp<std::int8_t>(std::int32_t(-128)), std::int8_t(-128));
    REQUIRE_EQ(integral_cast_clamp<std::int8_t>(std::int32_t(3)), std::int8_t(3));
    REQUIRE_EQ(integral_cast_clamp<std::int8_t>(std::int32_t(127)), std::int8_t(127));
    REQUIRE_EQ(integral_cast_clamp<std::int8_t>(std::int32_t(128)), std::int8_t(127));

    REQUIRE_EQ(integral_cast_clamp<std::int8_t>(std::uint8_t(0)), std::int8_t(0));
    REQUIRE_EQ(integral_cast_clamp<std::int8_t>(std::uint8_t(127)), std::int8_t(127));
    REQUIRE_EQ(integral_cast_clamp<std::int8_t>(std::uint8_t(128)), std::int8_t(127));

    REQUIRE_EQ(integral_cast_clamp<std::uint64_t>(std::numeric_limits<std::uint16_t>::lowest()), static_cast<std::int64_t>(std::numeric_limits<std::uint16_t>::lowest()));
    REQUIRE_EQ(integral_cast_clamp<std::uint64_t>(std::uint16_t(0)), std::uint64_t(0));
    REQUIRE_EQ(integral_cast_clamp<std::uint64_t>(std::numeric_limits<std::uint16_t>::max()), static_cast<std::int64_t>(std::numeric_limits<std::uint16_t>::max()));

    REQUIRE_EQ(integral_cast_clamp<std::int64_t>(std::numeric_limits<std::int16_t>::lowest()), static_cast<std::int64_t>(std::numeric_limits<std::int16_t>::lowest()));
    REQUIRE_EQ(integral_cast_clamp<std::int64_t>(std::int16_t(0)), std::int64_t(0));
    REQUIRE_EQ(integral_cast_clamp<std::int64_t>(std::numeric_limits<std::int16_t>::max()), static_cast<std::int64_t>(std::numeric_limits<std::int16_t>::max()));
}

TEST_CASE("numeric_test - ceil")
{
    using namespace fplus;
    REQUIRE_EQ(ceil(1.4), 2);
    REQUIRE_EQ(ceil(-1.4), -1);
}

TEST_CASE("numeric_test - floor")
{
    using namespace fplus;
    REQUIRE_EQ(floor(1.4), 1);
    REQUIRE_EQ(floor(-1.4), -2);
}

TEST_CASE("numeric_test - floor_to_int_mult")
{
    using namespace fplus;
    REQUIRE_EQ(floor_to_int_mult(2, -3), -4);
    REQUIRE_EQ(floor_to_int_mult(2, -2), -2);
    REQUIRE_EQ(floor_to_int_mult(2, -1), -2);
    REQUIRE_EQ(floor_to_int_mult(2, 0), 0);
    REQUIRE_EQ(floor_to_int_mult(2, 1), 0);
    REQUIRE_EQ(floor_to_int_mult(2, 2), 2);
    REQUIRE_EQ(floor_to_int_mult(2, 3), 2);
    REQUIRE_EQ(floor_to_int_mult<unsigned char>(2, 0), 0);
    REQUIRE_EQ(floor_to_int_mult<unsigned char>(2, 1), 0);
    REQUIRE_EQ(floor_to_int_mult<unsigned char>(2, 2), 2);
    REQUIRE_EQ(floor_to_int_mult<unsigned char>(2, 3), 2);

    REQUIRE_EQ(floor_to_int_mult(1, -1), -1);
    REQUIRE_EQ(floor_to_int_mult(1, 0), 0);
    REQUIRE_EQ(floor_to_int_mult(1, 1), 1);
}

TEST_CASE("numeric_test - ceil_to_int_mult")
{
    using namespace fplus;
    REQUIRE_EQ(ceil_to_int_mult(2, -3), -2);
    REQUIRE_EQ(ceil_to_int_mult(2, -2), -2);
    REQUIRE_EQ(ceil_to_int_mult(2, -1), -0);
    REQUIRE_EQ(ceil_to_int_mult(2, 0), 0);
    REQUIRE_EQ(ceil_to_int_mult(2, 1), 2);
    REQUIRE_EQ(ceil_to_int_mult(2, 2), 2);
    REQUIRE_EQ(ceil_to_int_mult(2, 3), 4);
    REQUIRE_EQ(ceil_to_int_mult<unsigned char>(2, 0), 0);
    REQUIRE_EQ(ceil_to_int_mult<unsigned char>(2, 1), 2);
    REQUIRE_EQ(ceil_to_int_mult<unsigned char>(2, 2), 2);
    REQUIRE_EQ(ceil_to_int_mult<unsigned char>(2, 3), 4);

    REQUIRE_EQ(ceil_to_int_mult(1, -1), -1);
    REQUIRE_EQ(ceil_to_int_mult(1, 0), 0);
    REQUIRE_EQ(ceil_to_int_mult(1, 1), 1);
}

TEST_CASE("numeric_test - reference_interval")
{
    using namespace fplus;
    REQUIRE_EQ(reference_interval(2, 6, 0, 4, 3), 5);
    REQUIRE_EQ(reference_interval(2, 10, 0, 4, 3), 8);
    REQUIRE_EQ(reference_interval(2, 6, 0, 4, -1), 1);
    REQUIRE_EQ(reference_interval(2, 10, 0, 4, -1), 0);
}

TEST_CASE("numeric_test - clamp")
{
    using namespace fplus;
    REQUIRE_EQ(clamp(2, 6, 5), 5);
    REQUIRE_EQ(clamp(2, 6, 1), 2);
    REQUIRE_EQ(clamp(2, 6, 8), 6);
}

TEST_CASE("numeric_test - int_power")
{
    using namespace fplus;
    REQUIRE_EQ(int_power(3, 0), 1);
    REQUIRE_EQ(int_power(3, 1), 3);
    REQUIRE_EQ(int_power(3, 2), 9);
    REQUIRE_EQ(int_power(3, 3), 27);
    REQUIRE_EQ(int_power(3, 4), 81);
}

TEST_CASE("numeric_test - min_on")
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

TEST_CASE("numeric_test - max_on")
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

TEST_CASE("numeric_test - mean")
{
    using namespace fplus;
    Ints xs = {1,4,4};
    REQUIRE_EQ(mean<int>(xs), 3);
}

TEST_CASE("numeric_test - mean_obj")
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
    REQUIRE(is_in_interval_around(0.001, mean_vec_length_squared_dest,
        vec_2d_length_squared(mean_vec_div_double)));
    REQUIRE(is_in_interval_around(0.001, mean_vec_length_squared_dest,
        vec_2d_length_squared(mean_vec_div_size_t)));
}

TEST_CASE("numeric_test - variadic")
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

    REQUIRE_EQ(max_2(2, 3), 3);
    REQUIRE_EQ(min_2(2, 3), 2);
}

TEST_CASE("numeric_test - normalize")
{
    using namespace fplus;

    REQUIRE_EQ(normalize_min_max(0.0, 10.0, Doubles({1, 3, 6})), Doubles({0, 4, 10}));
    REQUIRE_EQ(normalize_mean_stddev(3.0, 2.0, Doubles({7, 8})), Doubles({1, 5}));
    REQUIRE_EQ(standardize(Doubles({2, 6})), Doubles({-1, 1}));

    REQUIRE_EQ(normalize_min_max(0.0f, 10.0f, Floats({1, 3, 6})), Floats({0, 4, 10}));
    REQUIRE_EQ(normalize_mean_stddev(3.0f, 2.0f, Floats({7, 8})), Floats({1, 5}));
    REQUIRE_EQ(standardize(Floats({2, 6})), Floats({-1, 1}));

    Floats xs1 = {1, 3, 6};
    Floats xs2 = {7, 8};
    Floats xs3 = {2.0, 6.0};
    REQUIRE_EQ(normalize_min_max(0.0f, 10.0f, xs1), Floats({0, 4, 10}));
    REQUIRE_EQ(normalize_mean_stddev(3.0f, 2.0f, xs2), Floats({1, 5}));
    REQUIRE_EQ(standardize(xs3), Floats({-1, 1}));

}

TEST_CASE("numeric_test - winsorize")
{
    using namespace fplus;

    REQUIRE_EQ(winsorize(0.1, Doubles()), Doubles());
    REQUIRE_EQ(winsorize(0.1, Doubles({1})), Doubles({1}));
    REQUIRE_EQ(winsorize(0.4, Doubles({1,2})), Doubles({1,2}));
    REQUIRE_EQ(winsorize(0.32, Doubles({1,2,3})), Doubles({1,2,3}));
    REQUIRE_EQ(winsorize(0.34, Doubles({1,2,3})), Doubles({2,2,2}));
    REQUIRE_EQ(winsorize(0.1, Doubles({1,3,4,4,4,4,4,4,6,8})), Doubles({3,3,4,4,4,4,4,4,6,6}));
    REQUIRE_EQ(winsorize(-0.1, Doubles({1,3,4,4,4,4,4,4,6,8})), Doubles({1,3,4,4,4,4,4,4,6,8}));
    REQUIRE_EQ(winsorize(0.1, Doubles({4,4,4,3,8,4,6,4,3,4})), Doubles({3,3,4,4,4,4,4,4,6,6}));
    REQUIRE_EQ(winsorize(0, Doubles({1,3,4,4,4,4,4,4,6,8})), Doubles({1,3,4,4,4,4,4,4,6,8}));

    const auto median_result = winsorize(0.6, Doubles({1,2}));
    REQUIRE_EQ(median_result.size(), 2);
    REQUIRE(fplus::is_in_interval_around(0.001, 1.5, median_result[0]));
    REQUIRE(fplus::is_in_interval_around(0.001, 1.5, median_result[1]));
}

TEST_CASE("numeric_test - histogram")
{
    using namespace fplus;

    typedef std::vector<int> ints;
    typedef std::pair<int, int> interval;
    typedef std::vector<interval> intervals;
    typedef std::pair<interval, std::size_t> bin;
    typedef std::vector<bin> bins;

    const ints xs = {0,1,4,5,6,7,8,9};
    const intervals intervals1 = {{0,4}, {4,5}, {6,8}};
    const bins result1 = {{{0, 4}, 2}, {{4, 5}, 1}, {{6, 8}, 2}};

    REQUIRE_EQ(histogram_using_intervals(intervals1, xs), result1);
}

TEST_CASE("numeric_test - generate_consecutive_intervals")
{
    using namespace fplus;

    typedef std::pair<int, int> interval;
    typedef std::vector<interval> intervals;

    const intervals result = {{0,2}, {2,4}, {4,6}, {6,8}};

    REQUIRE_EQ(generate_consecutive_intervals(0, 2, 4), result);
}

TEST_CASE("numeric_test - histogram_intervals")
{
    using namespace fplus;

    typedef std::vector<int> ints;
    typedef std::pair<int, std::size_t> bin;
    typedef std::vector<bin> bins;

    const ints xs = {0,1,4,5,7,8,9};
    const bins result1 = {{1, 2}, {3, 0}, {5, 2}, {7, 1}};

    REQUIRE_EQ(histogram(1, 2, 4, xs), result1);
}

TEST_CASE("numeric_test - modulo_chain")
{
    using namespace fplus;
    typedef std::vector<int> ints;
    REQUIRE_EQ(modulo_chain(ints({24, 60, 60}), 23), ints({0, 0, 0, 23}));
    REQUIRE_EQ(modulo_chain(ints({24, 60, 60}), 7223), ints({0, 2, 0, 23}));
    REQUIRE_EQ(
        modulo_chain(ints({24, 60, 60, 1000}),
            3 * 24 * 60 * 60 * 1000 +
            17 * 60 * 60 * 1000 +
            4 * 60 * 1000 +
            31 * 1000 +
            256),
        ints({3, 17, 4, 31, 256}));
}

TEST_CASE("numeric_test - line_equation")
{
    using namespace fplus;

    REQUIRE(is_in_interval_around(0.001, 2.0, line_equation(
        std::make_pair(0.0, 0.0),
        std::make_pair(2.0, 1.0), 4.0)));
    REQUIRE(is_in_interval_around(0.001, -2.0, line_equation(
        std::make_pair(-1.0, 1.0),
        std::make_pair(-2.0, 4.0), 0.0)));
}
