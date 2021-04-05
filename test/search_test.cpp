// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <doctest/doctest.h>
#include <fplus/fplus.hpp>
#include <vector>

namespace
{
    bool is_even(int value)
    {
        return value % 2 == 0;
    }
}

TEST_CASE("search_test - find_first_by")
{
    std::vector<int> v = { 1, 3, 4, 6, 9 };
    auto result = fplus::find_first_by(is_even, v);
    REQUIRE_EQ(result, fplus::just(4));
}

TEST_CASE("search_test - find_first_by_nothing_found")
{
    std::vector<int> v = { 1, 3, 5, 7, 9 };
    auto result = fplus::find_first_by(is_even, v);
    REQUIRE_EQ(result, fplus::nothing<int>());
}

TEST_CASE("search_test - find_last_by")
{
    std::vector<int> v = { 1, 3, 4, 6, 9 };
    auto result = fplus::find_last_by(is_even, v);
    REQUIRE_EQ(result, fplus::just(6));
}

TEST_CASE("search_test - find_last_by_nothing_found")
{
    std::vector<int> v = { 1, 3, 5, 7, 9 };
    auto result = fplus::find_first_by(is_even, v);
    REQUIRE_EQ(result, fplus::nothing<int>());
}

TEST_CASE("search_test - find_first_idx_by")
{
    std::vector<int> v = { 1, 3, 4, 6, 9 };
    auto result = fplus::find_first_idx_by(is_even, v);
    REQUIRE_EQ(result, fplus::just<std::size_t>(2));
}

TEST_CASE("search_test - find_first_idx_by_nothing_found")
{
    std::vector<int> v = { 1, 3, 5, 7, 9 };
    auto result = fplus::find_first_idx_by(is_even, v);
    REQUIRE_EQ(result, fplus::nothing<size_t>());
}

TEST_CASE("search_test - find_last_idx_by")
{
    std::vector<int> v = { 1, 3, 4, 6, 9 };
    auto result = fplus::find_last_idx_by(is_even, v);
    REQUIRE_EQ(result, fplus::just<size_t>(3));
}

TEST_CASE("search_test - find_last_idx_by_nothing_found")
{
    std::vector<int> v = { 1, 3, 5, 7, 9 };
    auto result = fplus::find_last_idx_by(is_even, v);
    REQUIRE_EQ(result, fplus::nothing<size_t>());
}

TEST_CASE("search_test - find_first_idx")
{
    std::vector<int> v = { 1, 3, 4, 4, 9 };
    auto result = fplus::find_first_idx(4, v);
    REQUIRE_EQ(result, fplus::just<size_t>(2));
}

TEST_CASE("search_test - find_first_idx_nothing_found")
{
    std::vector<int> v = { 1, 3, 5, 7, 9 };
    auto result = fplus::find_first_idx(4, v);
    REQUIRE_EQ(result, fplus::nothing<size_t>());
}

TEST_CASE("search_test - find_last_idx")
{
    std::vector<int> v = { 1, 3, 4, 4, 9 };
    auto result = fplus::find_last_idx(4, v);
    REQUIRE_EQ(result, fplus::just<size_t>(3));
}

TEST_CASE("search_test - find_last_idx_nothing_found")
{
    std::vector<int> v = { 1, 3, 5, 7, 9 };
    auto result = fplus::find_last_idx(4, v);
    REQUIRE_EQ(result, fplus::nothing<size_t>());
}

TEST_CASE("search_test - find_all_idxs_by")
{
    std::vector<int> v = { 1, 3, 4, 6, 9 };
    auto result = fplus::find_all_idxs_by(is_even, v);
    REQUIRE_EQ(result, std::vector<std::size_t>({2, 3}));
}

TEST_CASE("search_test - find_last_idxs_by_nothing_found")
{
    std::vector<int> v = { 1, 3, 5, 7, 9 };
    auto result = fplus::find_all_idxs_by(is_even, v);
    REQUIRE(result.empty());
}

TEST_CASE("search_test - find_all_idxs_of")
{
    std::vector<int> v = { 1, 3, 4, 4, 9 };
    auto result = fplus::find_all_idxs_of(4, v);
    REQUIRE_EQ(result, std::vector<size_t>({2, 3}));
}

TEST_CASE("search_test - find_all_instances_of_token")
{
    const std::string token = "haha";
    const std::string input = "oh, hahaha!";
    auto result = fplus::find_all_instances_of_token(token, input);
    REQUIRE_EQ(result, std::vector<std::size_t>({4, 6}));
}

TEST_CASE("search_test - find_last_idxs_of_nothing_found")
{
    std::vector<int> v = { 1, 3, 5, 7, 9 };
    auto result = fplus::find_all_idxs_of(4, v);
    REQUIRE(result.empty());
}

TEST_CASE("search_test - find_all_instances_of_token_oversized_token")
{
    const std::string token = "hahahahaha";
    const std::string input = "oh, hahaha!";
    auto result = fplus::find_all_instances_of_token(token, input);
    REQUIRE(result.empty());
}

TEST_CASE("search_test - find_all_instances_of_token_non_overlapping")
{
    const std::string token = "haha";
    const std::string input = "oh, hahaha!";
    auto result = fplus::find_all_instances_of_token_non_overlapping(token, input);
    REQUIRE_EQ(result, std::vector<std::size_t>({4}));
}

TEST_CASE("search_test - find_first_instance_of_token")
{
    const std::string token = "haha";
    const std::string input = "oh, hahaha!";
    auto result = fplus::find_first_instance_of_token(token, input);
    REQUIRE_EQ(result, fplus::just<size_t>(4));
}

TEST_CASE("search_test - find_first_instance_of_token_at_end")
{
    const std::string token = "haha";
    const std::string input = "oh, haha";
    auto result = fplus::find_first_instance_of_token(token, input);
    REQUIRE_EQ(result, fplus::just<size_t>(4));
}

TEST_CASE("search_test - find_first_instance_of_token_nothing")
{
    const std::string token = "hihi";
    const std::string input = "oh, haha";
    auto result = fplus::find_first_instance_of_token(token, input);
    REQUIRE_EQ(result, fplus::nothing<size_t>());
}

TEST_CASE("search_test - find_first_instance_of_token_oversized_token")
{
    const std::string token = "hahahahaha";
    const std::string input = "oh, hahaha!";
    auto result = fplus::find_first_instance_of_token(token, input);
    REQUIRE_EQ(result, fplus::nothing<size_t>());
}
