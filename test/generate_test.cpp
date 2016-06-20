
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "fplus/fplus.h"
#include <vector>
#include <utility>

using namespace testing;

namespace {

    bool compare_not_eq(char c1, char c2)
    {
        return c1 != c2;
    }

    std::pair<char, char> as_pair(char c1, char c2)
    {
        return std::make_pair(c1, c2);
    }

}

TEST(generate_test, generate)
{
    int value = 0;
    auto f = [&] { return value++; };
    auto result = fplus::generate<std::vector<int>>(f, 6);
    EXPECT_THAT(result, ElementsAre(0, 1, 2, 3, 4 ,5));
}

TEST(generate_test, generate_by_idx)
{
    auto f = [](size_t value) { return value + 10; };
    auto result = fplus::generate_by_idx<std::vector<int>>(f, 6);
    EXPECT_THAT(result, ElementsAre(10, 11, 12, 13, 14 ,15));
}

TEST(generate_test, repeat)
{
    const std::vector<int> v = { 1, 2 };
    auto result = fplus::repeat(3, v);
    EXPECT_THAT(result, ElementsAre(1, 2, 1, 2, 1, 2));
}

TEST(generate_test, replicate)
{
    auto result = fplus::replicate(3, 1);
    EXPECT_THAT(result, ElementsAre(1, 1, 1));
}

TEST(generate_test, infixes)
{
    const std::vector<int> v = { 1, 2, 3, 4, 5, 6 };
    auto result = fplus::infixes(3, v);
    EXPECT_EQ(4u, result.size());
    EXPECT_THAT(result[0], ElementsAre(1, 2, 3));
    EXPECT_THAT(result[1], ElementsAre(2, 3, 4));
    EXPECT_THAT(result[2], ElementsAre(3, 4, 5));
    EXPECT_THAT(result[3], ElementsAre(4, 5, 6));
}

TEST(generate_test, infixes_with_size_less_than_length)
{
    const std::vector<int> v = { 1, 2 };
    auto result = fplus::infixes(3, v);
    EXPECT_THAT(result, IsEmpty());
}

TEST(generate_test, carthesian_product_with_where)
{
    const std::vector<char> v1 = { 'A', 'B', 'C' };
    const std::vector<char> v2 = { 'X', 'Y' };
    auto result = fplus::carthesian_product_with_where(as_pair, compare_not_eq, v1, v2);
    EXPECT_THAT(result, ElementsAre(Pair('A', 'X'), Pair('A', 'Y'),
                                    Pair('B', 'X'), Pair('B', 'Y'),
                                    Pair('C', 'X'), Pair('C', 'Y')));
}

TEST(generate_test, carthesian_product_with)
{
    const std::vector<char> v1 = { 'A', 'B', 'C' };
    const std::vector<char> v2 = { 'X', 'Y' };
    auto result = fplus::carthesian_product_with(as_pair, v1, v2);
    EXPECT_THAT(result, ElementsAre(Pair('A', 'X'), Pair('A', 'Y'),
                                    Pair('B', 'X'), Pair('B', 'Y'),
                                    Pair('C', 'X'), Pair('C', 'Y')));
}

TEST(generate_test, carthesian_product_where)
{
    const std::vector<char> v1 = { 'A', 'B', 'C' };
    const std::vector<char> v2 = { 'X', 'Y' };
    auto result = fplus::carthesian_product_where(compare_not_eq, v1, v2);
    EXPECT_THAT(result, ElementsAre(Pair('A', 'X'), Pair('A', 'Y'),
                                    Pair('B', 'X'), Pair('B', 'Y'),
                                    Pair('C', 'X'), Pair('C', 'Y')));
}

TEST(generate_test, carthesian_product)
{
    const std::vector<char> v1 = { 'A', 'B', 'C' };
    const std::vector<char> v2 = { 'X', 'Y' };
    auto result = fplus::carthesian_product(v1, v2);
    EXPECT_THAT(result, ElementsAre(Pair('A', 'X'), Pair('A', 'Y'),
                                    Pair('B', 'X'), Pair('B', 'Y'),
                                    Pair('C', 'X'), Pair('C', 'Y')));
}

TEST(generate_test, carthesian_product_n)
{
    const std::vector<char> v = { 'A', 'B' };
    auto result = fplus::carthesian_product_n(2, v);
    EXPECT_EQ(4u, result.size());
    EXPECT_THAT(result[0], ElementsAre('A', 'A'));
    EXPECT_THAT(result[1], ElementsAre('A', 'B'));
    EXPECT_THAT(result[2], ElementsAre('B', 'A'));
    EXPECT_THAT(result[3], ElementsAre('B', 'B'));
}

TEST(generate_test, carthesian_product_n_with_power_0)
{
    const std::vector<char> v = { 'A', 'B' };
    auto result = fplus::carthesian_product_n(0, v);
    EXPECT_EQ(1u, result.size());
    EXPECT_THAT(result[0], IsEmpty());
}

TEST(generate_test, permutations)
{
    const std::vector<char> v = { 'A', 'B' };
    auto result = fplus::permutations(2, v);
    EXPECT_EQ(2u, result.size());
    EXPECT_THAT(result[0], ElementsAre('A', 'B'));
    EXPECT_THAT(result[1], ElementsAre('B', 'A'));
}

TEST(generate_test, permutations_with_power_0)
{
    const std::vector<char> v = { 'A', 'B' };
    auto result = fplus::permutations(0, v);
    EXPECT_EQ(1u, result.size());
    EXPECT_THAT(result[0], IsEmpty());
}

TEST(generate_test, combinations)
{
    const std::vector<char> v = { 'A', 'B', 'C' };
    auto result = fplus::combinations(2, v);
    EXPECT_EQ(3u, result.size());
    EXPECT_THAT(result[0], ElementsAre('A', 'B'));
    EXPECT_THAT(result[1], ElementsAre('A', 'C'));
    EXPECT_THAT(result[2], ElementsAre('B', 'C'));
}

TEST(generate_test, combinations_with_power_0)
{
    const std::vector<char> v = { 'A', 'B' };
    auto result = fplus::combinations(0, v);
    EXPECT_EQ(1u, result.size());
    EXPECT_THAT(result[0], IsEmpty());
}

TEST(generate_test, combinations_with_replacement)
{
    const std::vector<char> v = { 'A', 'B' };
    auto result = fplus::combinations_with_replacement(2, v);
    EXPECT_EQ(3u, result.size());
    EXPECT_THAT(result[0], ElementsAre('A', 'A'));
    EXPECT_THAT(result[1], ElementsAre('A', 'B'));
    EXPECT_THAT(result[2], ElementsAre('B', 'B'));
}

TEST(generate_test, combinations_with_replacement_with_power_0)
{
    const std::vector<char> v = { 'A', 'B' };
    auto result = fplus::combinations_with_replacement(0, v);
    EXPECT_EQ(1u, result.size());
    EXPECT_THAT(result[0], IsEmpty());
}

TEST(generate_test, power_set)
{
    const std::vector<char> v = { 'x', 'y' };
    auto result = fplus::power_set(v);
    EXPECT_EQ(4u, result.size());
    EXPECT_THAT(result[0], IsEmpty());
    EXPECT_THAT(result[1], ElementsAre('x'));
    EXPECT_THAT(result[2], ElementsAre('y'));
    EXPECT_THAT(result[3], ElementsAre('x', 'y'));
}

TEST(generate_test, iterate)
{
    auto f = [](int value) { return value * 2; };
    auto result = fplus::iterate(f, 5, 3);
    EXPECT_EQ(5u, result.size());
    EXPECT_THAT(result, ElementsAre(3, 6, 12, 24, 48));
}

TEST(generate_test, iterate_with_size_0)
{
    auto f = [](int value) { return value * 2; };
    auto result = fplus::iterate(f, 0, 3);
    EXPECT_THAT(result, IsEmpty());
}

TEST(generate_test, rotate_left)
{
    const std::vector<char> v = { 'x', 'y', 'z' };
    auto result = fplus::rotate_left(v);
    EXPECT_THAT(result, ElementsAre('y', 'z', 'x'));
}

TEST(generate_test, rotate_left_with_empty)
{
    const std::vector<char> v = { };
    auto result = fplus::rotate_left(v);
    EXPECT_THAT(result, IsEmpty());
}

TEST(generate_test, rotate_right)
{
    const std::vector<char> v = { 'x', 'y', 'z' };
    auto result = fplus::rotate_right(v);
    EXPECT_THAT(result, ElementsAre('z', 'x', 'y'));
}

TEST(generate_test, rotate_right_with_empty)
{
    const std::vector<char> v = { };
    auto result = fplus::rotate_right(v);
    EXPECT_THAT(result, IsEmpty());
}

TEST(generate_test, rotations_left)
{
    const std::vector<char> v = { 'a', 'b', 'c', 'd' };
    auto result = fplus::rotations_left(v);
    EXPECT_EQ(4u, result.size());
    EXPECT_THAT(result[0], ElementsAre('a', 'b', 'c', 'd'));
    EXPECT_THAT(result[1], ElementsAre('b', 'c', 'd', 'a'));
    EXPECT_THAT(result[2], ElementsAre('c', 'd', 'a', 'b'));
    EXPECT_THAT(result[3], ElementsAre('d', 'a', 'b', 'c'));
}

TEST(generate_test, rotations_left_with_empty)
{
    const std::vector<char> v = { };
    auto result = fplus::rotations_left(v);
    EXPECT_THAT(result, IsEmpty());
}

TEST(generate_test, rotations_right)
{
    const std::vector<char> v = { 'a', 'b', 'c', 'd' };
    auto result = fplus::rotations_right(v);
    EXPECT_EQ(4u, result.size());
    EXPECT_THAT(result[0], ElementsAre('a', 'b', 'c', 'd'));
    EXPECT_THAT(result[1], ElementsAre('d', 'a', 'b', 'c'));
    EXPECT_THAT(result[2], ElementsAre('c', 'd', 'a', 'b'));
    EXPECT_THAT(result[3], ElementsAre('b', 'c', 'd', 'a'));
}

TEST(generate_test, rotations_right_with_empty)
{
    const std::vector<char> v = { };
    auto result = fplus::rotations_right(v);
    EXPECT_THAT(result, IsEmpty());
}

TEST(generate_test, fill_left)
{
    const std::vector<int> v = { 1, 2, 3, 4 };
    auto result = fplus::fill_left(0, 6, v);
    EXPECT_THAT(result, ElementsAre(0, 0, 1, 2, 3, 4));
}

TEST(generate_test, fill_left_with_min_size)
{
    const std::vector<int> v = { 1, 2, 3, 4 };
    auto result = fplus::fill_left(0, 4, v);
    EXPECT_THAT(result, ElementsAre(1, 2, 3, 4));
}

TEST(generate_test, fill_right)
{
    const std::vector<int> v = { 1, 2, 3, 4 };
    auto result = fplus::fill_right(0, 6, v);
    EXPECT_THAT(result, ElementsAre(1, 2, 3, 4, 0, 0));
}

TEST(generate_test, fill_right_with_min_size)
{
    const std::vector<int> v = { 1, 2, 3, 4 };
    auto result = fplus::fill_right(0, 4, v);
    EXPECT_THAT(result, ElementsAre(1, 2, 3, 4));
}

TEST(generate_test, inits)
{
    const std::vector<int> v = { 0, 1, 2, 3 };
    auto result = fplus::inits(v);
    EXPECT_EQ(5u, result.size());
    EXPECT_THAT(result[0], IsEmpty());
    EXPECT_THAT(result[1], ElementsAre(0));
    EXPECT_THAT(result[2], ElementsAre(0, 1));
    EXPECT_THAT(result[3], ElementsAre(0, 1, 2));
    EXPECT_THAT(result[4], ElementsAre(0, 1, 2, 3));
}

TEST(generate_test, tails)
{
    const std::vector<int> v = { 0, 1, 2, 3 };
    auto result = fplus::tails(v);
    EXPECT_EQ(5u, result.size());
    EXPECT_THAT(result[0], ElementsAre(0, 1, 2, 3));
    EXPECT_THAT(result[1], ElementsAre(1, 2, 3));
    EXPECT_THAT(result[2], ElementsAre(2, 3));
    EXPECT_THAT(result[3], ElementsAre(3));
    EXPECT_THAT(result[4], IsEmpty());
}

