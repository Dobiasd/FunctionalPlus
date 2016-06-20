
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
}

TEST(numeric_test, is_in_range)
{
    EXPECT_TRUE(fplus::is_in_range(1, 3)(1));
    EXPECT_TRUE(fplus::is_in_range(1, 3)(2));
    EXPECT_FALSE(fplus::is_in_range(1, 3)(0));
    EXPECT_FALSE(fplus::is_in_range(1, 3)(3));
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

TEST(numeric_test, min_2_on)
{
    EXPECT_THAT(fplus::min_2_on(mod2)(4, 3), Eq(4));
    std::string str1 = "hello";
    std::string str2 = "hi";
    auto str_length = [](const std::string& str) { return str.size(); };
    EXPECT_THAT(fplus::min_2_on(str_length)(str1, str2), Eq(std::string("hi")));
}

TEST(numeric_test, max_2_on)
{
    EXPECT_THAT(fplus::max_2_on(mod2)(4, 3), Eq(3));
}

TEST(numeric_test, min_3_on)
{
    EXPECT_THAT(fplus::min_3_on(mod2)(4, 3, 7), Eq(4));
}

TEST(numeric_test, max_3_on)
{
    EXPECT_THAT(fplus::max_3_on(mod2)(4, 3, 6), Eq(3));
}
