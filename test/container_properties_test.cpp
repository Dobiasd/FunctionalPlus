// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <doctest/doctest.h>
#include <fplus/fplus.hpp>
#include <vector>

namespace {

    auto is_even_int = [](int x){ return x % 2 == 0; };
    typedef std::vector<int> IntVector;
    typedef std::vector<bool> BoolVector;
    IntVector xs = {1,2,2,3,2};
}

TEST_CASE("container_properties_test - any")
{
    using namespace fplus;
    REQUIRE_EQ(any(BoolVector()), false);
    REQUIRE_EQ(any(BoolVector({true})), true);
    REQUIRE_EQ(any(BoolVector({false})), false);
    REQUIRE_EQ(any(BoolVector({false, false})), false);
    REQUIRE_EQ(any(BoolVector({true, false})), true);

    REQUIRE_EQ(any_by(is_even_int, IntVector()), false);
    REQUIRE_EQ(any_by(is_even_int, IntVector({2})), true);
    REQUIRE_EQ(any_by(is_even_int, IntVector({1})), false);
    REQUIRE_EQ(any_by(is_even_int, IntVector({1, 1})), false);
    REQUIRE_EQ(any_by(is_even_int, IntVector({2, 1})), true);
}


TEST_CASE("container_properties_test - none")
{
    using namespace fplus;
    REQUIRE_EQ(none(BoolVector()), true);
    REQUIRE_EQ(none(BoolVector({true})), false);
    REQUIRE_EQ(none(BoolVector({false})), true);
    REQUIRE_EQ(none(BoolVector({false, false})), true);
    REQUIRE_EQ(none(BoolVector({true, false})), false);

    REQUIRE_EQ(none_by(is_even_int, IntVector()), true);
    REQUIRE_EQ(none_by(is_even_int, IntVector({2})), false);
    REQUIRE_EQ(none_by(is_even_int, IntVector({1})), true);
    REQUIRE_EQ(none_by(is_even_int, IntVector({1, 1})), true);
    REQUIRE_EQ(none_by(is_even_int, IntVector({2, 1})), false);
}

TEST_CASE("container_properties_test - minmax")
{
    using namespace fplus;
    auto negateInt = [](int i) -> int { return -i; };

    REQUIRE_EQ(minimum(xs), 1);
    REQUIRE_EQ(maximum(xs), 3);

    REQUIRE_EQ(minimum_by(std::greater<int>(), xs), 3);
    REQUIRE_EQ(maximum_by(std::greater<int>(), xs), 1);

    REQUIRE_EQ(minimum_on(negateInt, xs), 3);
    REQUIRE_EQ(maximum_on(negateInt, xs), 1);

    REQUIRE_EQ(minimum_idx(xs), 0);
    REQUIRE_EQ(maximum_idx(xs), 3);

    REQUIRE_EQ(minimum_idx_by(std::greater<int>(), xs), 3);
    REQUIRE_EQ(maximum_idx_by(std::greater<int>(), xs), 0);

    REQUIRE_EQ(minimum_idx_on(negateInt, xs), 3);
    REQUIRE_EQ(maximum_idx_on(negateInt, xs), 0);
}

TEST_CASE("container_properties_test - minmax_maybe")
{
    using namespace fplus;
    auto negateInt = [](int i) -> int { return -i; };

    REQUIRE_EQ(minimum_maybe(xs), maybe<int>(1));
    REQUIRE_EQ(maximum_maybe(xs), maybe<int>(3));

    REQUIRE_EQ(minimum_by_maybe(std::greater<int>(), xs), maybe<int>(3));
    REQUIRE_EQ(maximum_by_maybe(std::greater<int>(), xs), maybe<int>(1));

    REQUIRE_EQ(minimum_on_maybe(negateInt, xs), maybe<int>(3));
    REQUIRE_EQ(maximum_on_maybe(negateInt, xs), maybe<int>(1));

    REQUIRE_EQ(minimum_idx_maybe(xs), maybe<std::size_t>(0));
    REQUIRE_EQ(maximum_idx_maybe(xs), maybe<std::size_t>(3));

    REQUIRE_EQ(minimum_idx_by_maybe(std::greater<int>(), xs), maybe<std::size_t>(3));
    REQUIRE_EQ(maximum_idx_by_maybe(std::greater<int>(), xs), maybe<std::size_t>(0));


}

TEST_CASE("container_properties_test - mean")
{
    using namespace fplus;
    std::vector<unsigned char> uchars = {200, 202};
    typedef std::vector<double> DoubleVector;
    REQUIRE_EQ(sum(xs), 10);
    REQUIRE_EQ(product(xs), 24);
    REQUIRE_EQ(mean<int>(xs), 2);
    REQUIRE_EQ(mean_using_doubles<unsigned char>(uchars), 201);
    REQUIRE_EQ(median(IntVector({ 3 })), 3);
    REQUIRE_EQ(median(IntVector({ 3, 5 })), 4);
    REQUIRE(is_in_interval(3.49f, 3.51f, median<IntVector, float>(IntVector({ 3, 4 }))));
    REQUIRE(is_in_interval(3.49, 3.51, mean<double>(DoubleVector({ 3, 4 }))));
    REQUIRE_EQ(median(IntVector({ 3, 9, 5 })), 5);
    REQUIRE_EQ(median(xs), 2);
    REQUIRE_EQ(sum(convert_container_and_elems<std::vector<int>>(std::string("hello"))), 532);
    REQUIRE(is_in_interval(5.99, 6.01, mean_stddev<double>(DoubleVector({ 4, 8 })).first));
    REQUIRE(is_in_interval(1.99, 2.01, mean_stddev<double>(DoubleVector({ 4, 8 })).second));
    REQUIRE(is_in_interval(3.749f, 3.751f, mean_stddev<float>(IntVector({ 1, 3, 7, 4 })).first));
    REQUIRE(is_in_interval(2.16f, 2.17f, mean_stddev<float>(IntVector({ 1, 3, 7, 4 })).second));
}

TEST_CASE("container_properties_test - all_unique_less")
{
    using namespace fplus;
    REQUIRE_FALSE(all_unique_less(IntVector({1,2,3,2})));
    REQUIRE(all_unique_less(IntVector({4,2,1,3})));
}

TEST_CASE("container_properties_test - infix")
{
    using namespace fplus;
    REQUIRE_EQ(is_infix_of(IntVector({}), IntVector({})), true);
    REQUIRE_EQ(is_infix_of(IntVector({}), IntVector({1,2})), true);
    REQUIRE_EQ(is_infix_of(IntVector({2,3}), xs), true);
    REQUIRE_EQ(is_infix_of(IntVector({2,3}), xs), true);
    REQUIRE_EQ(is_infix_of(IntVector({2,1}), xs), false);
    REQUIRE_EQ(is_prefix_of(IntVector({ 1,2 }), xs), true);
    REQUIRE_EQ(is_prefix_of(IntVector({ 2,2 }), xs), false);
    REQUIRE_EQ(is_suffix_of(IntVector({ 3,2 }), xs), true);
    REQUIRE_EQ(is_suffix_of(IntVector({ 2,2 }), xs), false);
}

TEST_CASE("container_properties_test - subsequence")
{
    using namespace fplus;
    REQUIRE_EQ(is_subsequence_of(IntVector(), IntVector()), true);
    REQUIRE_EQ(is_subsequence_of(IntVector(), xs), true);
    REQUIRE_EQ(is_subsequence_of(IntVector({ 1,3 }), xs), true);
    REQUIRE_EQ(is_subsequence_of(IntVector({ 3,1 }), xs), false);
}

TEST_CASE("container_properties_test - count")
{
    using namespace fplus;
    REQUIRE_EQ(count(2, xs), 3);
}

TEST_CASE("container_properties_test - is_unique_in")
{
    using namespace fplus;
    REQUIRE_FALSE(is_unique_in(2, xs));
    REQUIRE(is_unique_in(3, xs));
}

TEST_CASE("container_properties_test - is_permutation_of")
{
    using namespace fplus;
    REQUIRE(is_permutation_of(IntVector({2,3,1}), IntVector({1,2,3})));
    REQUIRE_FALSE(is_permutation_of(IntVector({2,3,2}), IntVector({1,2,3})));
    REQUIRE_FALSE(is_permutation_of(IntVector({2,3}), IntVector({1,2,3})));
    REQUIRE_FALSE(is_permutation_of(IntVector({2,3,1}), IntVector({1,23})));
}

TEST_CASE("container_properties_test - fill_pigeonholes")
{
    const std::vector<unsigned int> ys = { 0, 1, 3, 1 };
    REQUIRE_EQ(fplus::fill_pigeonholes_to(5, ys), std::vector<std::size_t>({1,2,0,1,0}));
    REQUIRE_EQ(fplus::fill_pigeonholes_to(3, ys), std::vector<std::size_t>({1,2,0}));
    REQUIRE_EQ(fplus::fill_pigeonholes(ys), std::vector<std::size_t>({1,2,0,1}));

    REQUIRE_EQ(fplus::fill_pigeonholes_bool_to(3, ys), std::vector<unsigned char>({1,1,0}));
    REQUIRE_EQ(fplus::fill_pigeonholes_bool_to(5, ys), std::vector<unsigned char>({1,1,0,1,0}));
    REQUIRE_EQ(fplus::fill_pigeonholes_bool(ys), std::vector<unsigned char>({1,1,0,1}));
}

TEST_CASE("container_properties_test - present_in_all")
{
    using namespace fplus;
    const std::vector<std::vector<int>> xss = { {4,1,2}, {5,2,1}, {2,4,1} };
    REQUIRE_EQ(present_in_all(xss), IntVector({1,2}));
}