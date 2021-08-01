// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <doctest/doctest.h>
#include <fplus/fplus.hpp>
#include <vector>

namespace {
    auto is_even = [](auto value)
    {
        return (value % 2 == 0);
    };

    auto is_even_size_t = [](auto value)
    {
        return ( value % 2 == 0 );
    };

    auto accept_with_index = [](std::size_t index, auto value)
    {
        return ( index % 2 == 0 ) && ( value >= 10 );
    };

    typedef std::vector<int> IntVector;
    typedef std::vector<IntVector> IntVectors;
}

TEST_CASE("filter_test - keep_if")
{
    const std::vector<int> v = { 1, 2, 3, 2, 4, 5 };
    auto result = fplus::keep_if(is_even, v);
    REQUIRE_EQ(result, std::vector<int>({2, 2, 4}));

    const auto keep_evens = fplus::bind_1st_of_2(
        fplus::keep_if<decltype(is_even), const IntVector&>, is_even);

    REQUIRE_EQ(
        fplus::transform(keep_evens, IntVectors({{1,3,4},{1,2}})),
        IntVectors({{4},{2}}));
}

TEST_CASE("filter_test - keep_if_r_value")
{
    auto result = fplus::keep_if(is_even, std::vector<int>({1,2,3,2,4,5}));
    REQUIRE_EQ(result, std::vector<int>({2, 2, 4}));
}

TEST_CASE("filter_test - drop_if")
{
    const std::vector<int> v = { 1, 2, 3, 2, 4, 5 };
    auto result = fplus::drop_if(is_even, v);
    REQUIRE_EQ(result, std::vector<int>({1, 3, 5}));
}

TEST_CASE("filter_test - without")
{
    using namespace fplus;
    typedef std::vector<int> Ints;
    REQUIRE_EQ(without(1, Ints({1,2,3})), Ints({2,3}));
    REQUIRE_EQ(without(5, Ints({1,2,3})), Ints({1,2,3}));
    REQUIRE_EQ(without(5, Ints({})), Ints({}));
}

TEST_CASE("filter_test - keep_if_with_idx")
{
    const std::vector<int> v = { 1, 20, 30, 4, 50, 60, 7 };
    auto result = fplus::keep_if_with_idx(accept_with_index, v);
    REQUIRE_EQ(result, std::vector<int>({30, 50}));
}

TEST_CASE("filter_test - drop_if_with_idx")
{
    const std::vector<int> v = { 1, 20, 30, 4, 50, 60, 7 };
    auto result = fplus::drop_if_with_idx(accept_with_index, v);
    REQUIRE_EQ(result, std::vector<int>({1, 20, 4, 60, 7}));
}

TEST_CASE("filter_test - keep_by_idx")
{
    const std::vector<int> v = { 11, 17, 3, 8, 49, 6 };
    auto result = fplus::keep_by_idx(is_even_size_t, v);
    REQUIRE_EQ(result, std::vector<int>({11, 3, 49}));
    auto result_rvalue = fplus::keep_by_idx(is_even_size_t, std::vector<int>({ 11, 17, 3, 8, 49, 6 }));
    REQUIRE_EQ(result_rvalue, std::vector<int>({11, 3, 49}));

    const std::vector<std::string> v_strs = { "foo", "bar" };
    const auto result_kept_all_strs = fplus::keep_by_idx(fplus::always<std::size_t>(true), v_strs);
    REQUIRE_EQ(result_kept_all_strs, v_strs);
    const auto result_kept_all_strs_r_value =
        fplus::keep_by_idx(fplus::always<std::size_t>(true), std::vector<std::string>({ "foo", "bar" }));
    REQUIRE_EQ(result_kept_all_strs_r_value, std::vector<std::string>({ "foo", "bar" }));
}

TEST_CASE("filter_test - drop_by_idx")
{
    const std::vector<int> v = { 11, 17, 3, 8, 49, 6 };
    auto result = fplus::drop_by_idx(is_even_size_t, v);
    REQUIRE_EQ(result, std::vector<int>({17, 8, 6}));
}

TEST_CASE("filter_test - keep_idxs")
{
    const std::vector<int> v = { 1, 2, 3, 4, 5, 6, 7 };
    const std::vector<std::size_t> indices = { 2, 5 };
    auto result = fplus::keep_idxs(indices, v);
    REQUIRE_EQ(result, std::vector<int>({3, 6}));
}

TEST_CASE("filter_test - drop_idx")
{
    const std::vector<int> v = { 1, 2, 3, 4, 5, 6, 7 };
    auto result = fplus::drop_idx(2, v);
    REQUIRE_EQ(result, std::vector<int>({1, 2, 4, 5, 6, 7}));
}

TEST_CASE("filter_test - drop_idxs")
{
    const std::vector<int> v = { 1, 2, 3, 4, 5, 6, 7 };
    const std::vector<std::size_t> indices = { 2, 5 };
    auto result = fplus::drop_idxs(indices, v);
    REQUIRE_EQ(result, std::vector<int>({1, 2, 4, 5, 7}));
}

TEST_CASE("filter_test - justs")
{
    using fplus::maybe;
    using fplus::just;
    using fplus::nothing;

    const std::vector<maybe<int>> v = { just(1), nothing<int>(), just(2) };
    auto result = fplus::justs(v);
    REQUIRE_EQ(result, std::vector<int>({1, 2}));
}

TEST_CASE("filter_test - oks")
{
    using fplus::ok;
    using fplus::error;
    const std::vector<fplus::result<int, std::string>> v = { ok<int, std::string>(1),
                                                            error<int>(std::string("abc")),
                                                            ok<int, std::string>(2) };
    auto result = fplus::oks(v);
    REQUIRE_EQ(result, std::vector<int>({1, 2}));
}

TEST_CASE("filter_test - errors")
{
    using fplus::ok;
    using fplus::error;
    const std::vector<fplus::result<int, std::string>> v = { ok<int, std::string>(1),
                                                            error<int>(std::string("abc")),
                                                            ok<int, std::string>(2) };
    auto result = fplus::errors(v);
    REQUIRE_EQ(result, std::vector<std::string>({"abc"}));
}

TEST_CASE("filter_test - trim_left")
{
    const std::vector<int> v = { 0, 0, 0, 5, 6, 7, 8, 6, 4 };
    auto result = fplus::trim_left(0, v);
    REQUIRE_EQ(result, std::vector<int>({5, 6, 7, 8, 6, 4}));
}

TEST_CASE("filter_test - trim_token_left")
{
    const std::vector<int> v = { 0, 1, 2, 0, 1, 2, 7, 5, 9 };
    const std::vector<int> token = { 0, 1, 2 };
    auto result = fplus::trim_token_left(token, v);
    REQUIRE_EQ(result, std::vector<int>({7, 5, 9}));
}

TEST_CASE("filter_test - trim_right_by")
{
    const std::vector<int> v = { 0, 2, 4, 5, 6, 7, 8, 6, 4 };
    auto result = fplus::trim_right_by(is_even, v);
    REQUIRE_EQ(result, std::vector<int>({0, 2, 4, 5, 6, 7}));
}

TEST_CASE("filter_test - trim_right_by_trims_all")
{
    const std::vector<int> v = { 4, 8 };
    auto result = fplus::trim_right_by(is_even, v);
    REQUIRE(result.empty());
}

TEST_CASE("filter_test - trim_right")
{
    const std::vector<int> v = { 0, 2, 4, 5, 6, 7, 8, 4, 4 };
    auto result = fplus::trim_right(4, v);
    REQUIRE_EQ(result, std::vector<int>({0, 2, 4, 5, 6, 7, 8}));
}

TEST_CASE("filter_test - trim_token_right")
{
    const std::vector<int> v = { 7, 5, 9, 0, 1, 2, 0, 1, 2 };
    const std::vector<int> token = { 0, 1, 2 };
    auto result = fplus::trim_token_right(token, v);
    REQUIRE_EQ(result, std::vector<int>({7, 5, 9}));
}

TEST_CASE("filter_test - trim_by")
{
    const std::vector<int> v = { 0, 2, 4, 5, 6, 7, 8, 6, 4 };
    auto result = fplus::trim_by(is_even, v);
    REQUIRE_EQ(result, std::vector<int>({5, 6, 7}));
}

TEST_CASE("filter_test - trim")
{
    const std::vector<int> v = { 0, 2, 4, 5, 6, 7, 8, 0, 0 };
    auto result = fplus::trim(0, v);
    REQUIRE_EQ(result, std::vector<int>({2, 4, 5, 6, 7, 8}));
}

TEST_CASE("filter_test - trim_token")
{
    const std::vector<int> v = { 0, 1, 7, 8, 9, 0, 1 };
    const std::vector<int> token = { 0, 1 };
    auto result = fplus::trim_token(token, v);
    REQUIRE_EQ(result, std::vector<int>({7, 8, 9}));
}

TEST_CASE("filter_test - adjacent_keep_snd_if")
{
    const std::vector<int> v = { 0, 1, 7, 8, 9, 0, 1 };
    REQUIRE_EQ(fplus::adjacent_keep_snd_if(std::greater<>(), v), std::vector<int>({0,0}));
    REQUIRE_EQ(fplus::adjacent_keep_snd_if(std::less<int>(), v), std::vector<int>({0,1,7,8,9,1}));
}

TEST_CASE("filter_test - adjacent_drop_snd_if")
{
    const std::vector<int> v = { 0, 1, 7, 8, 9, 0, 1 };
    REQUIRE_EQ(fplus::adjacent_drop_snd_if(std::less<>(), v), std::vector<int>({0,0}));
    REQUIRE_EQ(fplus::adjacent_drop_snd_if(std::greater<int>(), v), std::vector<int>({0,1,7,8,9,1}));
}

TEST_CASE("filter_test - adjacent_drop_fst_if")
{
    const std::vector<int> v = { 0, 1, 7, 8, 9, 0, 1 };
    REQUIRE_EQ(fplus::adjacent_drop_fst_if(std::less<>(), v), std::vector<int>({9,1}));
    REQUIRE_EQ(fplus::adjacent_drop_fst_if(std::greater<int>(), v), std::vector<int>({0,1,7,8,0,1}));
}

TEST_CASE("filter_test - adjacent_keep_fst_if")
{
    const std::vector<int> v = { 0, 1, 7, 8, 9, 0, 1 };
    REQUIRE_EQ(fplus::adjacent_keep_fst_if(std::greater<>(), v), std::vector<int>({9,1}));
    REQUIRE_EQ(fplus::adjacent_keep_fst_if(std::less<int>(), v), std::vector<int>({0,1,7,8,0,1}));
}
