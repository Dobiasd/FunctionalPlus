
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "fplus/fplus.h"
#include <vector>

using namespace testing;

namespace {

    bool int_less(int x, int y)
    {
        return x < y;
    }

    bool int_less_eq(int x, int y)
    {
        return x <= y;
    }
}


TEST(compare_test, is_equal_to)
{
    using namespace fplus;
    EXPECT_TRUE(is_equal_to(2)(2));
    EXPECT_FALSE(is_equal_to(2)(3));
}

TEST(compare_test, is_not_equal_to)
{
    using namespace fplus;
    EXPECT_FALSE(is_not_equal_to(2)(2));
    EXPECT_TRUE(is_not_equal_to(2)(3));
}

TEST(compare_test, is_less)
{
    using namespace fplus;
    EXPECT_FALSE(is_less(2, 2));
    EXPECT_TRUE(is_less(2, 3));
    EXPECT_FALSE(is_less(3, 2));
    EXPECT_TRUE(is_less_than(3)(2));
}

TEST(compare_test, is_less_or_equal)
{
    using namespace fplus;
    EXPECT_TRUE(is_less_or_equal(2, 2));
    EXPECT_TRUE(is_less_or_equal(2, 3));
    EXPECT_FALSE(is_less_or_equal(3, 2));
    EXPECT_TRUE(is_less_or_equal_than(3)(2));
}

TEST(compare_test, is_greater)
{
    using namespace fplus;
    EXPECT_FALSE(is_greater(2, 2));
    EXPECT_FALSE(is_greater(2, 3));
    EXPECT_TRUE(is_greater(3, 2));
    EXPECT_FALSE(is_greater_than(3)(2));
}

TEST(compare_test, is_greater_or_equal)
{
    using namespace fplus;
    EXPECT_TRUE(is_greater_or_equal(2, 2));
    EXPECT_FALSE(is_greater_or_equal(2, 3));
    EXPECT_TRUE(is_greater_or_equal(3, 2));
    EXPECT_FALSE(is_greater_or_equal_than(3)(2));
}

TEST(compare_test, is_equal_by)
{
    using namespace fplus;
    EXPECT_THAT(identity(2), Eq(2));
    EXPECT_THAT(always<int>(2)(5), Eq(2));
    auto square = [](int x){ return x*x; };
    EXPECT_TRUE(is_equal_by_and_by(square, square)(2, -2));
    EXPECT_TRUE(is_equal_by(square)(2, -2));
    EXPECT_TRUE(is_not_equal_by_and_by(square, square)(2, 3));
    EXPECT_TRUE(is_not_equal_by(square)(2, 3));
}

TEST(compare_test, xor_bools)
{
    using namespace fplus;
    EXPECT_TRUE(xor_bools(false, false) == false);
    EXPECT_TRUE(xor_bools(true, false) == true);
    EXPECT_TRUE(xor_bools(false, true) == true);
    EXPECT_TRUE(xor_bools(true, true) == false);
}

TEST(compare_test, ord_to_eq)
{
    using namespace fplus;
    EXPECT_TRUE(ord_to_eq(int_less)(1, 2) == false);
    EXPECT_TRUE(ord_to_eq(int_less)(2, 2) == true);
    EXPECT_TRUE(ord_to_eq(int_less)(2, 1) == false);
}

TEST(compare_test, ord_to_not_eq)
{
    using namespace fplus;
    EXPECT_TRUE(ord_to_not_eq(int_less)(1, 2) == true);
    EXPECT_TRUE(ord_to_not_eq(int_less)(2, 2) == false);
    EXPECT_TRUE(ord_to_not_eq(int_less)(2, 1) == true);
}

TEST(compare_test, ord_eq_to_eq)
{
    using namespace fplus;
    EXPECT_TRUE(ord_eq_to_eq(int_less_eq)(1, 2) == false);
    EXPECT_TRUE(ord_eq_to_eq(int_less_eq)(2, 2) == true);
    EXPECT_TRUE(ord_eq_to_eq(int_less_eq)(2, 1) == false);
}

TEST(compare_test, ord_eq_to_not_eq)
{
    using namespace fplus;
    EXPECT_TRUE(ord_eq_to_not_eq(int_less_eq)(1, 2) == true);
    EXPECT_TRUE(ord_eq_to_not_eq(int_less_eq)(2, 2) == false);
    EXPECT_TRUE(ord_eq_to_not_eq(int_less_eq)(2, 1) == true);
}
