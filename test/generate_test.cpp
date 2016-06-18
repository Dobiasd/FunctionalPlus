
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "fplus/fplus.h"
#include <vector>

using namespace testing;

class generate_test : public testing::Test
{
protected:

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(generate_test, generate)
{
    int value = 0;
    auto f = [&] { return value++; };
    auto result = fplus::generate<std::vector<int>>(f, 6);
    EXPECT_THAT(result, ElementsAre(0, 1, 2, 3, 4 ,5));
}

TEST_F(generate_test, generate_by_idx)
{
    auto f = [](size_t value) { return value + 10; };
    auto result = fplus::generate_by_idx<std::vector<int>>(f, 6);
    EXPECT_THAT(result, ElementsAre(10, 11, 12, 13, 14 ,15));
}

TEST_F(generate_test, repeat)
{
    const std::vector<int> v = { 1, 2 };
    auto result = fplus::repeat(3, v);
    EXPECT_THAT(result, ElementsAre(1, 2, 1, 2, 1, 2));
}

TEST_F(generate_test, replicate)
{
    auto result = fplus::replicate(3, 1);
    EXPECT_THAT(result, ElementsAre(1, 1, 1));
}

TEST_F(generate_test, infixes)
{
    const std::vector<int> v = { 1, 2, 3, 4, 5, 6 };
    auto result = fplus::infixes(3, v);
    EXPECT_EQ(4u, result.size());
    EXPECT_THAT(result[0], ElementsAre(1, 2, 3));
    EXPECT_THAT(result[1], ElementsAre(2, 3, 4));
    EXPECT_THAT(result[2], ElementsAre(3, 4, 5));
    EXPECT_THAT(result[3], ElementsAre(4, 5, 6));
}

