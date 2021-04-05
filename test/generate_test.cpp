// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <doctest/doctest.h>
#include <fplus/fplus.hpp>
#include <vector>
#include <utility>

namespace {

    auto compare_not_eq = [](auto c1, auto c2)
    {
        return c1 != c2;
    };

    auto as_pair = [](auto c1, auto c2)
    {
        return std::make_pair(c1, c2);
    };

}

TEST_CASE("generate_test - generate")
{
    int value = 0;
    auto f = [&] { return value++; };
    auto result = fplus::generate<std::vector<int>>(f, 6);
    REQUIRE_EQ(result, std::vector<int>({0, 1, 2, 3, 4 ,5}));
}

TEST_CASE("generate_test - generate_by_idx")
{
    auto f = [](std::size_t value) { return value + 10; };
    auto result = fplus::generate_by_idx<std::vector<std::size_t>>(f, 6);
    REQUIRE_EQ(result, std::vector<std::size_t>({10, 11, 12, 13, 14 ,15}));
    auto f2 = [f](auto value) { return f(value); };
    auto result2 = fplus::generate_by_idx<std::vector<std::size_t>>(f2, 6);
    REQUIRE_EQ(result2, result);
}

TEST_CASE("generate_test - repeat")
{
    const std::vector<int> v = { 1, 2 };
    auto result = fplus::repeat(3, v);
    REQUIRE_EQ(result, std::vector<int>({1, 2, 1, 2, 1, 2}));
}

TEST_CASE("generate_test - replicate")
{
    auto result = fplus::replicate(3, 1);
    REQUIRE_EQ(result, std::vector<int>({1, 1, 1}));
}

TEST_CASE("generate_test - infixes")
{
    const std::vector<int> v = { 1, 2, 3, 4, 5, 6 };
    auto result = fplus::infixes(3, v);
    REQUIRE_EQ(4u, result.size());
    REQUIRE_EQ(result[0], std::vector<int>({1, 2, 3}));
    REQUIRE_EQ(result[1], std::vector<int>({2, 3, 4}));
    REQUIRE_EQ(result[2], std::vector<int>({3, 4, 5}));
    REQUIRE_EQ(result[3], std::vector<int>({4, 5, 6}));
}

TEST_CASE("generate_test - infixes_with_size_less_than_length")
{
    const std::vector<int> v = { 1, 2 };
    auto result = fplus::infixes(3, v);
    REQUIRE(result.empty());
}

TEST_CASE("generate_test - carthesian_product_with_where")
{
    const std::vector<char> v1 = { 'A', 'B', 'C' };
    const std::vector<char> v2 = { 'X', 'Y' };
    auto result = fplus::carthesian_product_with_where(as_pair, compare_not_eq, v1, v2);
    std::vector<std::pair<char, char>> expected = {
        {'A', 'X'}, {'A', 'Y'},
        {'B', 'X'}, {'B', 'Y'},
        {'C', 'X'}, {'C', 'Y'}};
    REQUIRE_EQ(result, expected);
}

TEST_CASE("generate_test - carthesian_product_with")
{
    const std::vector<char> v1 = { 'A', 'B', 'C' };
    const std::vector<char> v2 = { 'X', 'Y' };
    auto result = fplus::carthesian_product_with(as_pair, v1, v2);
    std::vector<std::pair<char, char>> expected = {
        {'A', 'X'}, {'A', 'Y'},
        {'B', 'X'}, {'B', 'Y'},
        {'C', 'X'}, {'C', 'Y'}};
    REQUIRE_EQ(result, expected);
}

TEST_CASE("generate_test - carthesian_product_where")
{
    const std::vector<char> v1 = { 'A', 'B', 'C' };
    const std::vector<char> v2 = { 'X', 'Y' };
    auto result = fplus::carthesian_product_where(compare_not_eq, v1, v2);
    std::vector<std::pair<char, char>> expected = {
        {'A', 'X'}, {'A', 'Y'},
        {'B', 'X'}, {'B', 'Y'},
        {'C', 'X'}, {'C', 'Y'}};
    REQUIRE_EQ(result, expected);
}

TEST_CASE("generate_test - carthesian_product")
{
    const std::vector<char> v1 = { 'A', 'B', 'C' };
    const std::vector<char> v2 = { 'X', 'Y' };
    auto result = fplus::carthesian_product(v1, v2);
    std::vector<std::pair<char, char>> expected = {
        {'A', 'X'}, {'A', 'Y'},
        {'B', 'X'}, {'B', 'Y'},
        {'C', 'X'}, {'C', 'Y'}};
    REQUIRE_EQ(result, expected);
}

TEST_CASE("generate_test - carthesian_product_n")
{
    const std::vector<char> v = { 'A', 'B' };
    auto result = fplus::carthesian_product_n(2, v);
    REQUIRE_EQ(4u, result.size());
    REQUIRE_EQ(result[0], std::vector<char>({'A', 'A'}));
    REQUIRE_EQ(result[1], std::vector<char>({'A', 'B'}));
    REQUIRE_EQ(result[2], std::vector<char>({'B', 'A'}));
    REQUIRE_EQ(result[3], std::vector<char>({'B', 'B'}));
}

TEST_CASE("generate_test - carthesian_product_n_with_power_0")
{
    const std::vector<char> v = { 'A', 'B' };
    auto result = fplus::carthesian_product_n(0, v);
    REQUIRE_EQ(1u, result.size());
    REQUIRE(result[0].empty());
}

TEST_CASE("generate_test - permutations")
{
    const std::vector<char> v = { 'A', 'B' };
    auto result = fplus::permutations(2, v);
    REQUIRE_EQ(2u, result.size());
    REQUIRE_EQ(result[0], std::vector<char>({'A', 'B'}));
    REQUIRE_EQ(result[1], std::vector<char>({'B', 'A'}));
}

TEST_CASE("generate_test - permutations_with_power_0")
{
    const std::vector<char> v = { 'A', 'B' };
    auto result = fplus::permutations(0, v);
    REQUIRE_EQ(1u, result.size());
    REQUIRE(result[0].empty());
}

TEST_CASE("generate_test - combinations")
{
    const std::vector<char> v = { 'A', 'B', 'C' };
    auto result = fplus::combinations(2, v);
    REQUIRE_EQ(3u, result.size());
    REQUIRE_EQ(result[0], std::vector<char>({'A', 'B'}));
    REQUIRE_EQ(result[1], std::vector<char>({'A', 'C'}));
    REQUIRE_EQ(result[2], std::vector<char>({'B', 'C'}));
}

TEST_CASE("generate_test - combinations_with_power_0")
{
    const std::vector<char> v = { 'A', 'B' };
    auto result = fplus::combinations(0, v);
    REQUIRE_EQ(1u, result.size());
    REQUIRE(result[0].empty());
}

TEST_CASE("generate_test - combinations_with_replacement")
{
    const std::vector<char> v = { 'A', 'B' };
    auto result = fplus::combinations_with_replacement(2, v);
    REQUIRE_EQ(3u, result.size());
    REQUIRE_EQ(result[0], std::vector<char>({'A', 'A'}));
    REQUIRE_EQ(result[1], std::vector<char>({'A', 'B'}));
    REQUIRE_EQ(result[2], std::vector<char>({'B', 'B'}));
}

TEST_CASE("generate_test - combinations_with_replacement_with_power_0")
{
    const std::vector<char> v = { 'A', 'B' };
    auto result = fplus::combinations_with_replacement(0, v);
    REQUIRE_EQ(1u, result.size());
    REQUIRE(result[0].empty());
}

TEST_CASE("generate_test - power_set")
{
    const std::vector<char> v = { 'x', 'y' };
    auto result = fplus::power_set(v);
    REQUIRE_EQ(4u, result.size());
    REQUIRE(result[0].empty());
    REQUIRE_EQ(result[1], std::vector<char>({'x'}));
    REQUIRE_EQ(result[2], std::vector<char>({'y'}));
    REQUIRE_EQ(result[3], std::vector<char>({'x', 'y'}));
}

TEST_CASE("generate_test - iterate")
{
    auto f = [](auto value) { return value * 2; };
    auto result = fplus::iterate(f, 5, 3);
    REQUIRE_EQ(5u, result.size());
    REQUIRE_EQ(result, std::vector<int>({3, 6, 12, 24, 48}));
}

TEST_CASE("generate_test - iterate_with_size_0")
{
    auto f = [](auto value) { return value * 2; };
    auto result = fplus::iterate(f, 0, 3);
    REQUIRE(result.empty());
}

TEST_CASE("generate_test - iterate_maybe")
{
    const auto next_collatz_value = [](auto x) -> fplus::maybe<int>
    {
        if (x <= 1)
            return {};
        if (x % 2 == 0)
            return x / 2;
        else
            return 3 * x + 1;
    };
    const auto result = fplus::iterate_maybe(next_collatz_value, 5);
    REQUIRE_EQ(result, std::vector<int>({5, 16, 8, 4, 2, 1}));
}

TEST_CASE("generate_test - adjecent_difference")
{
    const std::vector<int> v = { 0, 4, 1, 2, 5 };
    auto result = fplus::adjacent_difference(v);
    REQUIRE_EQ(result, std::vector<int>({0, 4, -3, 1, 3}));
}

TEST_CASE("generate_test - rotate_left")
{
    const std::vector<char> v = { 'x', 'y', 'z' };
    auto result = fplus::rotate_left(v);
    REQUIRE_EQ(result, std::vector<char>({'y', 'z', 'x'}));
}

TEST_CASE("generate_test - rotate_left_with_empty")
{
    const std::vector<char> v = { };
    auto result = fplus::rotate_left(v);
    REQUIRE(result.empty());
}

TEST_CASE("generate_test - rotate_right")
{
    const std::vector<char> v = { 'x', 'y', 'z' };
    auto result = fplus::rotate_right(v);
    REQUIRE_EQ(result, std::vector<char>({'z', 'x', 'y'}));
}

TEST_CASE("generate_test - rotate_right_with_empty")
{
    const std::vector<char> v = { };
    auto result = fplus::rotate_right(v);
    REQUIRE(result.empty());
}

TEST_CASE("generate_test - rotations_left")
{
    const std::vector<char> v = { 'a', 'b', 'c', 'd' };
    auto result = fplus::rotations_left(v);
    REQUIRE_EQ(4u, result.size());
    REQUIRE_EQ(result[0], std::vector<char>({'a', 'b', 'c', 'd'}));
    REQUIRE_EQ(result[1], std::vector<char>({'b', 'c', 'd', 'a'}));
    REQUIRE_EQ(result[2], std::vector<char>({'c', 'd', 'a', 'b'}));
    REQUIRE_EQ(result[3], std::vector<char>({'d', 'a', 'b', 'c'}));
}

TEST_CASE("generate_test - rotations_left_with_empty")
{
    const std::vector<char> v = { };
    auto result = fplus::rotations_left(v);
    REQUIRE(result.empty());
}

TEST_CASE("generate_test - rotations_right")
{
    const std::vector<char> v = { 'a', 'b', 'c', 'd' };
    auto result = fplus::rotations_right(v);
    REQUIRE_EQ(4u, result.size());
    REQUIRE_EQ(result[0], std::vector<char>({'a', 'b', 'c', 'd'}));
    REQUIRE_EQ(result[1], std::vector<char>({'d', 'a', 'b', 'c'}));
    REQUIRE_EQ(result[2], std::vector<char>({'c', 'd', 'a', 'b'}));
    REQUIRE_EQ(result[3], std::vector<char>({'b', 'c', 'd', 'a'}));
}

TEST_CASE("generate_test - rotations_right_with_empty")
{
    const std::vector<char> v = { };
    auto result = fplus::rotations_right(v);
    REQUIRE(result.empty());
}

TEST_CASE("generate_test - fill_left")
{
    const std::vector<int> v = { 1, 2, 3, 4 };
    auto result = fplus::fill_left(0, 6, v);
    REQUIRE_EQ(result, std::vector<int>({0, 0, 1, 2, 3, 4}));
}

TEST_CASE("generate_test - fill_left_with_min_size")
{
    const std::vector<int> v = { 1, 2, 3, 4 };
    auto result = fplus::fill_left(0, 4, v);
    REQUIRE_EQ(result, std::vector<int>({1, 2, 3, 4}));
}

TEST_CASE("generate_test - fill_right")
{
    const std::vector<int> v = { 1, 2, 3, 4 };
    auto result = fplus::fill_right(0, 6, v);
    REQUIRE_EQ(result, std::vector<int>({1, 2, 3, 4, 0, 0}));
}

TEST_CASE("generate_test - fill_right_with_min_size")
{
    const std::vector<int> v = { 1, 2, 3, 4 };
    auto result = fplus::fill_right(0, 4, v);
    REQUIRE_EQ(result, std::vector<int>({1, 2, 3, 4}));
}

TEST_CASE("generate_test - inits")
{
    const std::vector<int> v = { 0, 1, 2, 3 };
    auto result = fplus::inits(v);
    REQUIRE_EQ(5u, result.size());
    REQUIRE_EQ(result[0], std::vector<int>());
    REQUIRE_EQ(result[1], std::vector<int>({0}));
    REQUIRE_EQ(result[2], std::vector<int>({0, 1}));
    REQUIRE_EQ(result[3], std::vector<int>({0, 1, 2}));
    REQUIRE_EQ(result[4], std::vector<int>({0, 1, 2, 3}));
}

TEST_CASE("generate_test - tails")
{
    const std::vector<int> v = { 0, 1, 2, 3 };
    auto result = fplus::tails(v);
    REQUIRE_EQ(5u, result.size());
    REQUIRE_EQ(result[0], std::vector<int>({0, 1, 2, 3}));
    REQUIRE_EQ(result[1], std::vector<int>({1, 2, 3}));
    REQUIRE_EQ(result[2], std::vector<int>({2, 3}));
    REQUIRE_EQ(result[3], std::vector<int>({3}));
    REQUIRE(result[4].empty());
}

TEST_CASE("generate_test - inner_product")
{
    const std::vector<int> xs = { 1, 2, 3 };
    const std::vector<int> ys = { 4, 5, 6 };
    const auto plus = [](int x, int y) { return x + y; };
    const auto mult = [](int x, int y) { return x * y; };
    REQUIRE_EQ(fplus::inner_product(0, xs, ys), 32);
    REQUIRE_EQ(fplus::inner_product_with(plus, mult, 0, xs, ys), 32);
    REQUIRE_EQ(fplus::inner_product_with(std::plus<>{}, std::multiplies<>{}, 0, xs, ys), 32);
}

TEST_CASE("generate_test - numbers")
{
    typedef std::vector<int> ints;
    REQUIRE_EQ(fplus::numbers<int>(2, 5), ints({2,3,4}));
    REQUIRE_EQ(fplus::numbers_step<int>(0, 6, 2), ints({0,2,4}));
}
