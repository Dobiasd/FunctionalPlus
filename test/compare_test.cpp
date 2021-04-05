// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <doctest/doctest.h>
#include <fplus/fplus.hpp>
#include <vector>

namespace {

    bool int_less(int x, int y)
    {
        return x < y;
    }

    bool int_less_eq(int x, int y)
    {
        return x <= y;
    }

    auto generic_less = [](auto x, auto y) {
      return x < y;
    };

    auto generic_less_eq = [](auto x, auto y) {
      return x <= y;
    };

    auto squareGeneric = [](auto x) { return x * x; };
}


TEST_CASE("compare_test - is_equal_to")
{
    using namespace fplus;
    REQUIRE(is_equal_to(2)(2));
    REQUIRE_FALSE(is_equal_to(2)(3));
}

TEST_CASE("compare_test - is_not_equal_to")
{
    using namespace fplus;
    REQUIRE_FALSE(is_not_equal_to(2)(2));
    REQUIRE(is_not_equal_to(2)(3));
}

TEST_CASE("compare_test - is_less")
{
    using namespace fplus;
    REQUIRE_FALSE(is_less(2, 2));
    REQUIRE(is_less(2, 3));
    REQUIRE_FALSE(is_less(3, 2));
    REQUIRE(is_less_than(3)(2));
}

TEST_CASE("compare_test - is_less_or_equal")
{
    using namespace fplus;
    REQUIRE(is_less_or_equal(2, 2));
    REQUIRE(is_less_or_equal(2, 3));
    REQUIRE_FALSE(is_less_or_equal(3, 2));
    REQUIRE(is_less_or_equal_than(3)(2));
    REQUIRE(is_less_or_equal_by_and_by(squareGeneric, squareGeneric)(2, 2));
    REQUIRE(is_less_or_equal_by_than(squareGeneric, 5)(2));
}

TEST_CASE("compare_test - is_less_by")
{
    using namespace fplus;
    auto square = [](int x) { return x * x; };
    REQUIRE(is_less_by_and_by(squareGeneric, square)(2, -3));
    REQUIRE(is_less_by(squareGeneric)(2, -3));
}

TEST_CASE("compare_test - is_less_by_than")
{
    using namespace fplus;
    auto square = [](int x) { return x * x; };
    REQUIRE(is_less_by_than(square, 5)(2));
    REQUIRE(is_less_by_than(squareGeneric, 5)(2));
}

TEST_CASE("compare_test - is_greater")
{
    using namespace fplus;
    REQUIRE_FALSE(is_greater(2, 2));
    REQUIRE_FALSE(is_greater(2, 3));
    REQUIRE(is_greater(3, 2));
    REQUIRE_FALSE(is_greater_than(3)(2));
    REQUIRE(is_greater_by_and_by(squareGeneric, squareGeneric)(3, -2));
}

TEST_CASE("compare_test - is_greater_or_equal")
{
    using namespace fplus;
    REQUIRE(is_greater_or_equal(2, 2));
    REQUIRE_FALSE(is_greater_or_equal(2, 3));
    REQUIRE(is_greater_or_equal(3, 2));
    REQUIRE_FALSE(is_greater_or_equal_than(3)(2));
    REQUIRE(is_greater_or_equal_by_and_by(squareGeneric, squareGeneric)(3, -3));
    REQUIRE(is_greater_or_equal_by(squareGeneric)(3, -3));
    REQUIRE(is_greater_or_equal_by_than(squareGeneric, 3)(-3));
}

TEST_CASE("compare_test - is_equal_by")
{
    using namespace fplus;
    auto square = [](int x) { return x * x; };
    REQUIRE(is_equal_by_and_by(square, square)(2, -2));
    REQUIRE(is_equal_by_and_by(squareGeneric, square)(2, -2));
    REQUIRE(is_equal_by(square)(2, -2));
    REQUIRE(is_not_equal_by_and_by(square, squareGeneric)(2, 3));
    REQUIRE(is_equal_by(squareGeneric)(2, -2));
    REQUIRE(is_not_equal_by(square)(2, 3));
    REQUIRE(is_not_equal_by(squareGeneric)(2, 3));
    REQUIRE(is_equal_by_to(squareGeneric, 4)(2));
    REQUIRE(is_not_equal_by_to(squareGeneric, 5)(2));
}

TEST_CASE("compare_test - always")
{
    using namespace fplus;
    REQUIRE_EQ(identity(2), 2);
    REQUIRE_EQ(always<int>(2)(5), 2);
    REQUIRE_EQ(always_arg_1_of_2(2, 5), 2);
    REQUIRE_EQ(always_arg_2_of_2(2, 5), 5);
}

TEST_CASE("compare_test - xor_bools")
{
    using namespace fplus;
    REQUIRE(xor_bools(false, false) == false);
    REQUIRE(xor_bools(true, false) == true);
    REQUIRE(xor_bools(false, true) == true);
    REQUIRE(xor_bools(true, true) == false);
}

TEST_CASE("compare_test - ord_to_eq")
{
    using namespace fplus;
    REQUIRE(ord_to_eq(int_less)(1, 2) == false);
    REQUIRE(ord_to_eq(int_less)(2, 2) == true);
    REQUIRE(ord_to_eq(int_less)(2, 1) == false);
    REQUIRE(ord_to_eq(generic_less)(2, 1) == false);
}

TEST_CASE("compare_test - ord_to_not_eq")
{
    using namespace fplus;
    REQUIRE(ord_to_not_eq(int_less)(1, 2) == true);
    REQUIRE(ord_to_not_eq(int_less)(2, 2) == false);
    REQUIRE(ord_to_not_eq(int_less)(2, 1) == true);
    REQUIRE(ord_to_not_eq(generic_less)(2, 1) == true);
}

TEST_CASE("compare_test - ord_eq_to_eq")
{
    using namespace fplus;
    REQUIRE(ord_eq_to_eq(int_less_eq)(1, 2) == false);
    REQUIRE(ord_eq_to_eq(int_less_eq)(2, 2) == true);
    REQUIRE(ord_eq_to_eq(int_less_eq)(2, 1) == false);
    REQUIRE(ord_eq_to_eq(generic_less_eq)(2, 1) == false);
}

TEST_CASE("compare_test - ord_eq_to_not_eq")
{
    using namespace fplus;
    REQUIRE(ord_eq_to_not_eq(int_less_eq)(1, 2) == true);
    REQUIRE(ord_eq_to_not_eq(int_less_eq)(2, 2) == false);
    REQUIRE(ord_eq_to_not_eq(int_less_eq)(2, 1) == true);
}

TEST_CASE("compare_test - lexicographical_less")
{
    using namespace fplus;
    REQUIRE(lexicographical_less(std::vector<int>{0,1,2,2,4,5}, std::vector<int>{0,1,2,3,4,5}));
    REQUIRE(lexicographical_less(std::vector<int>{}, std::vector<int>{1}));
    REQUIRE_FALSE(lexicographical_less(std::vector<int>{1}, std::vector<int>{}));
    REQUIRE(lexicographical_less(std::string("012245"), std::string("012345")));
    REQUIRE(lexicographical_less(std::string("012245"), std::string("01234")));
    REQUIRE(lexicographical_less(std::string("01234"), std::string("012345")));
    REQUIRE_FALSE(lexicographical_less(std::string("012345"), std::string("012245")));
    REQUIRE_FALSE(lexicographical_less(std::string("01234"), std::string("012245")));
    REQUIRE_FALSE(lexicographical_less(std::string("012345"), std::string("01234")));
}

TEST_CASE("compare_test - lexicographical_sort")
{
    using namespace fplus;
    std::vector<std::string> xs = {"012245", "012345", "01234"};
    std::vector<std::string> xs_lex_sorted = {"012245", "01234", "012345"};
    REQUIRE_EQ(lexicographical_sort(xs), xs_lex_sorted);
}
