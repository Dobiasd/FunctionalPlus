
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "fplus/fplus.h"
#include <vector>

using namespace testing;

namespace
{
    bool is_even(int value)
    {
        return value % 2 == 0;
    }
}

TEST(search_test, find_first_by)
{
    std::vector<int> v = { 1, 3, 4, 6, 9 };
    auto result = fplus::find_first_by(is_even, v);
    EXPECT_THAT(result, Eq(fplus::just(4)));
}

TEST(search_test, find_first_by_nothing_found)
{
    std::vector<int> v = { 1, 3, 5, 7, 9 };
    auto result = fplus::find_first_by(is_even, v);
    EXPECT_THAT(result, Eq(fplus::nothing<int>()));
}

TEST(search_test, find_last_by)
{
    std::vector<int> v = { 1, 3, 4, 6, 9 };
    auto result = fplus::find_last_by(is_even, v);
    EXPECT_THAT(result, Eq(fplus::just(6)));
}

TEST(search_test, find_last_by_nothing_found)
{
    std::vector<int> v = { 1, 3, 5, 7, 9 };
    auto result = fplus::find_first_by(is_even, v);
    EXPECT_THAT(result, Eq(fplus::nothing<int>()));
}

TEST(search_test, find_first_idx_by)
{
    std::vector<int> v = { 1, 3, 4, 6, 9 };
    auto result = fplus::find_first_idx_by(is_even, v);
    EXPECT_THAT(result, Eq(fplus::just<std::size_t>(2)));
}

TEST(search_test, find_first_idx_by_nothing_found)
{
    std::vector<int> v = { 1, 3, 5, 7, 9 };
    auto result = fplus::find_first_idx_by(is_even, v);
    EXPECT_THAT(result, Eq(fplus::nothing<size_t>()));
}

TEST(search_test, find_last_idx_by)
{
    std::vector<int> v = { 1, 3, 4, 6, 9 };
    auto result = fplus::find_last_idx_by(is_even, v);
    EXPECT_THAT(result, Eq(fplus::just<size_t>(3)));
}

TEST(search_test, find_last_idx_by_nothing_found)
{
    std::vector<int> v = { 1, 3, 5, 7, 9 };
    auto result = fplus::find_last_idx_by(is_even, v);
    EXPECT_THAT(result, Eq(fplus::nothing<size_t>()));
}

TEST(search_test, find_first_idx)
{
    std::vector<int> v = { 1, 3, 4, 4, 9 };
    auto result = fplus::find_first_idx(4, v);
    EXPECT_THAT(result, Eq(fplus::just<size_t>(2)));
}

TEST(search_test, find_first_idx_nothing_found)
{
    std::vector<int> v = { 1, 3, 5, 7, 9 };
    auto result = fplus::find_first_idx(4, v);
    EXPECT_THAT(result, Eq(fplus::nothing<size_t>()));
}

TEST(search_test, find_last_idx)
{
    std::vector<int> v = { 1, 3, 4, 4, 9 };
    auto result = fplus::find_last_idx(4, v);
    EXPECT_THAT(result, Eq(fplus::just<size_t>(3)));
}

TEST(search_test, find_last_idx_nothing_found)
{
    std::vector<int> v = { 1, 3, 5, 7, 9 };
    auto result = fplus::find_last_idx(4, v);
    EXPECT_THAT(result, Eq(fplus::nothing<size_t>()));
}

TEST(search_test, find_all_idxs_by)
{
    std::vector<int> v = { 1, 3, 4, 6, 9 };
    auto result = fplus::find_all_idxs_by(is_even, v);
    EXPECT_THAT(result, ElementsAre(2, 3));
}

TEST(search_test, find_last_idxs_by_nothing_found)
{
    std::vector<int> v = { 1, 3, 5, 7, 9 };
    auto result = fplus::find_all_idxs_by(is_even, v);
    EXPECT_THAT(result, IsEmpty());
}

TEST(search_test, find_all_idxs_of)
{
    std::vector<int> v = { 1, 3, 4, 4, 9 };
    auto result = fplus::find_all_idxs_of(4, v);
    EXPECT_THAT(result, ElementsAre(2, 3));
}

TEST(search_test, find_all_instances_of_token)
{
    const std::string token = "haha";
    const std::string input = "oh, hahaha!";
    auto result = fplus::find_all_instances_of_token(token, input);
    EXPECT_THAT(result, ElementsAre(4, 6));
}

TEST(search_test, find_last_idxs_of_nothing_found)
{
    std::vector<int> v = { 1, 3, 5, 7, 9 };
    auto result = fplus::find_all_idxs_of(4, v);
    EXPECT_THAT(result, IsEmpty());
}

TEST(search_test, find_all_instances_of_token_oversized_token)
{
    const std::string token = "hahahahaha";
    const std::string input = "oh, hahaha!";
    auto result = fplus::find_all_instances_of_token(token, input);
    EXPECT_THAT(result, IsEmpty());
}

TEST(search_test, find_all_instances_of_token_non_overlapping)
{
    const std::string token = "haha";
    const std::string input = "oh, hahaha!";
    auto result = fplus::find_all_instances_of_token_non_overlapping(token, input);
    EXPECT_THAT(result, ElementsAre(4));
}

TEST(search_test, find_first_instance_of_token)
{
    const std::string token = "haha";
    const std::string input = "oh, hahaha!";
    auto result = fplus::find_first_instance_of_token(token, input);
    EXPECT_THAT(result, Eq(fplus::just<size_t>(4)));
}

TEST(search_test, find_first_instance_of_token_oversized_token)
{
    const std::string token = "hahahahaha";
    const std::string input = "oh, hahaha!";
    auto result = fplus::find_first_instance_of_token(token, input);
    EXPECT_THAT(result, Eq(fplus::nothing<size_t>()));
}
