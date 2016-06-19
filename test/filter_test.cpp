
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "fplus/fplus.h"
#include <vector>

using namespace testing;

namespace {

    bool is_even(int value)
    {
        return ( value % 2 == 0 );
    }

    bool accept_with_index(size_t index, int value)
    {
        return ( index % 2 == 0 ) && ( value >= 10 );
    }
}

TEST(filter_test, keep_if)
{
    const std::vector<int> v = { 1, 2, 3, 2, 4, 5 };
    auto result = fplus::keep_if(is_even, v);
    EXPECT_THAT(result, ElementsAre(2, 2, 4));
}

TEST(filter_test, drop_if)
{
    const std::vector<int> v = { 1, 2, 3, 2, 4, 5 };
    auto result = fplus::drop_if(is_even, v);
    EXPECT_THAT(result, ElementsAre(1, 3, 5));
}

TEST(filter_test, without)
{
    const std::vector<int> v = { 1, 0, 0, 5, 3, 0, 1 };
    auto result = fplus::drop_if(is_even, v);
    EXPECT_THAT(result, ElementsAre(1, 5, 3, 1));
}

TEST(filter_test, keep_if_with_idx)
{
    const std::vector<int> v = { 1, 20, 30, 4, 50, 60, 7 };
    auto result = fplus::keep_if_with_idx(accept_with_index, v);
    EXPECT_THAT(result, ElementsAre(30, 50));
}

TEST(filter_test, drop_if_with_idx)
{
    const std::vector<int> v = { 1, 20, 30, 4, 50, 60, 7 };
    auto result = fplus::drop_if_with_idx(accept_with_index, v);
    EXPECT_THAT(result, ElementsAre(1, 20, 4, 60, 7));
}

TEST(filter_test, keep_by_idx)
{
    const std::vector<int> v = { 11, 17, 3, 8, 49, 6 };
    auto result = fplus::keep_by_idx(is_even, v);
    EXPECT_THAT(result, ElementsAre(11, 3, 49));
}

TEST(filter_test, drop_by_idx)
{
    const std::vector<int> v = { 11, 17, 3, 8, 49, 6 };
    auto result = fplus::drop_by_idx(is_even, v);
    EXPECT_THAT(result, ElementsAre(17, 8, 6));
}

TEST(filter_test, keep_idxs)
{
    const std::vector<int> v = { 1, 2, 3, 4, 5, 6, 7 };
    const std::vector<size_t> indices = { 2, 5 };
    auto result = fplus::keep_idxs(indices, v);
    EXPECT_THAT(result, ElementsAre(3, 6));
}

TEST(filter_test, drop_idxs)
{
    const std::vector<int> v = { 1, 2, 3, 4, 5, 6, 7 };
    const std::vector<size_t> indices = { 2, 5 };
    auto result = fplus::drop_idxs(indices, v);
    EXPECT_THAT(result, ElementsAre(1, 2, 4, 5, 7));
}

TEST(filter_test, justs)
{
    using fplus::maybe;
    using fplus::just;
    using fplus::nothing;

    const std::vector<maybe<int>> v = { just(1), nothing<int>(), just(2) };
    auto result = fplus::justs(v);
    EXPECT_THAT(result, ElementsAre(1, 2));
}

TEST(filter_test, oks)
{
    using fplus::ok;
    using fplus::error;
    const std::vector<fplus::result<int, std::string>> v = { ok<int, std::string>(1),
                                                            error<int>(std::string("abc")),
                                                            ok<int, std::string>(2) };
    auto result = fplus::oks(v);
    EXPECT_THAT(result, ElementsAre(1, 2));
}

TEST(filter_test, errors)
{
    using fplus::ok;
    using fplus::error;
    const std::vector<fplus::result<int, std::string>> v = { ok<int, std::string>(1),
                                                            error<int>(std::string("abc")),
                                                            ok<int, std::string>(2) };
    auto result = fplus::errors(v);
    EXPECT_THAT(result, ElementsAre("abc"));
}

TEST(filter_test, trim_left_by)
{
    const std::vector<int> v = { 0, 2, 4, 5, 6, 7, 8, 6, 4 };
    auto result = fplus::trim_left_by(is_even, v);
    EXPECT_THAT(result, ElementsAre(5, 6, 7, 8, 6, 4));
}

TEST(filter_test, trim_left)
{
    const std::vector<int> v = { 0, 0, 0, 5, 6, 7, 8, 6, 4 };
    auto result = fplus::trim_left(0, v);
    EXPECT_THAT(result, ElementsAre(5, 6, 7, 8, 6, 4));
}

TEST(filter_test, trim_token_left)
{
    const std::vector<int> v = { 0, 1, 2, 0, 1, 2, 7, 5, 9 };
    const std::vector<int> token = { 0, 1, 2 };
    auto result = fplus::trim_token_left(token, v);
    EXPECT_THAT(result, ElementsAre(7, 5, 9));
}

TEST(filter_test, trim_right_by)
{
    const std::vector<int> v = { 0, 2, 4, 5, 6, 7, 8, 6, 4 };
    auto result = fplus::trim_right_by(is_even, v);
    EXPECT_THAT(result, ElementsAre(0, 2, 4, 5, 6, 7));
}

TEST(filter_test, trim_right)
{
    const std::vector<int> v = { 0, 2, 4, 5, 6, 7, 8, 4, 4 };
    auto result = fplus::trim_right(4, v);
    EXPECT_THAT(result, ElementsAre(0, 2, 4, 5, 6, 7, 8));
}

TEST(filter_test, trim_token_right)
{
    const std::vector<int> v = { 7, 5, 9, 0, 1, 2, 0, 1, 2 };
    const std::vector<int> token = { 0, 1, 2 };
    auto result = fplus::trim_token_right(token, v);
    EXPECT_THAT(result, ElementsAre(7, 5, 9));
}

TEST(filter_test, trim_by)
{
    const std::vector<int> v = { 0, 2, 4, 5, 6, 7, 8, 6, 4 };
    auto result = fplus::trim_by(is_even, v);
    EXPECT_THAT(result, ElementsAre(5, 6, 7));
}

TEST(filter_test, trim)
{
    const std::vector<int> v = { 0, 2, 4, 5, 6, 7, 8, 0, 0 };
    auto result = fplus::trim(0, v);
    EXPECT_THAT(result, ElementsAre(2, 4, 5, 6, 7, 8));
}

TEST(filter_test, trim_token)
{
    const std::vector<int> v = { 0, 1, 7, 8, 9, 0, 1 };
    const std::vector<int> token = { 0, 1 };
    auto result = fplus::trim_token(token, v);
    EXPECT_THAT(result, ElementsAre(7, 8, 9));
}

