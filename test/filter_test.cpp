
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "fplus/fplus.h"
#include <vector>

using namespace testing;

class filter_test : public testing::Test
{
protected:

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }

    static bool is_even(int value)
    {
        return ( value % 2 == 0 );
    }

    static bool accept_with_index(size_t index, int value)
    {
        return ( index % 2 == 0 ) && ( value >= 10 );
    }
};

TEST_F(filter_test, keep_if)
{
    const std::vector<int> v = { 1, 2, 3, 2, 4, 5 };
    auto result = fplus::keep_if(is_even, v);
    EXPECT_THAT(result, ElementsAre(2, 2, 4));
}

TEST_F(filter_test, drop_if)
{
    const std::vector<int> v = { 1, 2, 3, 2, 4, 5 };
    auto result = fplus::drop_if(is_even, v);
    EXPECT_THAT(result, ElementsAre(1, 3, 5));
}

TEST_F(filter_test, without)
{
    const std::vector<int> v = { 1, 0, 0, 5, 3, 0, 1 };
    auto result = fplus::drop_if(is_even, v);
    EXPECT_THAT(result, ElementsAre(1, 5, 3, 1));
}

TEST_F(filter_test, keep_if_with_idx)
{
    const std::vector<int> v = { 1, 20, 30, 4, 50, 60, 7 };
    auto result = fplus::keep_if_with_idx(accept_with_index, v);
    EXPECT_THAT(result, ElementsAre(30, 50));
}

TEST_F(filter_test, drop_if_with_idx)
{
    const std::vector<int> v = { 1, 20, 30, 4, 50, 60, 7 };
    auto result = fplus::drop_if_with_idx(accept_with_index, v);
    EXPECT_THAT(result, ElementsAre(1, 20, 4, 60, 7));
}
