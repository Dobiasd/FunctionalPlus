// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <doctest/doctest.h>
#include <fplus/fplus.hpp>

namespace {
    auto squareLambda = [](int x) -> int { return x*x; };
    auto cubeLambda = [](int x) -> int { return x*x*x; };
    typedef std::vector<int> IntVector;
    typedef std::list<int> IntList;
    typedef std::pair<int, int> IntPair;
    typedef std::vector<IntPair> IntPairs;
    IntVector xs = {1,2,2,3,2};

    struct dummy
    {
      int i;
    };
}

TEST_CASE("pairs_test - zip_with")
{
    using namespace fplus;
    const auto multiply = [](int x, int y){ return x * y; };
    REQUIRE_EQ(zip_with(multiply, xs, xs), transform(squareLambda, xs));
    const auto add = [](int x, int y){ return x + y; };
    REQUIRE_EQ(zip_with(add, IntVector({1,2,3}), IntVector({1,2})), IntVector({2,4}));
    REQUIRE_EQ(zip_with(add, IntVector({1,2}), IntVector({1,2,3})), IntVector({2,4}));
    REQUIRE_EQ(zip_with(add, IntVector({1,2}), IntList({1,2,3})), IntVector({2,4}));

    const auto add_generic = [](auto x, int y) {return x + y;};
    REQUIRE_EQ(zip_with(add_generic, IntVector({1,2,3}), IntVector({1,2})), IntVector({2,4}));
    REQUIRE_EQ(zip_with(std::plus<>{}, IntVector({1,2,3}), IntVector({1,2})), IntVector({2,4}));
}

TEST_CASE("pairs_test - zip_with_3")
{
    using namespace fplus;
    const auto multiply = [](int x, int y, int z){ return x * y * z; };
    const auto multiply_generic = [](auto x, auto y, auto z){ return x * y * z; };
    const auto cubed = transform(cubeLambda, xs);
    REQUIRE_EQ(zip_with_3(multiply, xs, xs, xs), cubed);
    REQUIRE_EQ(zip_with_3(multiply_generic, xs, xs, xs), cubed);
}

TEST_CASE("pairs_test - zip_with_defaults")
{
    using namespace fplus;
    const auto add = [](int x, int y){ return x + y; };
    const auto add_generic = [](auto x, auto y){ return x + y; };
    REQUIRE_EQ(zip_with_defaults(add, 6, 7, IntVector({1,2,3}), IntVector({1,2})), IntVector({2,4,10}));
    REQUIRE_EQ(zip_with_defaults(add, 6, 7, IntVector({1,2}), IntVector({1,2,3})), IntVector({2,4,9}));
    REQUIRE_EQ(zip_with_defaults(add_generic, 6, 7, IntVector({1,2}), IntVector({1,2,3})), IntVector({2,4,9}));
}

TEST_CASE("pairs_test - zip")
{
    using namespace fplus;
    auto xsZippedWithXs = zip(xs, xs);
    REQUIRE_EQ(unzip(xsZippedWithXs).first, xs);
}

TEST_CASE("pairs_test - pair functions")
{
    using namespace fplus;
    IntPair intPair = std::make_pair(2, 3);
    IntPairs intPairs = {{1,2}, {3,4}};
    IntPairs intPairsSwapped = {{2,1}, {4,3}};
    REQUIRE_EQ(fst(intPair), 2);
    REQUIRE_EQ(snd(intPair), 3);
    REQUIRE_EQ(swap_pair_elems(intPair), std::make_pair(3, 2));
    REQUIRE_EQ(swap_pairs_elems(intPairs), intPairsSwapped);
    REQUIRE_EQ(transform_fst(squareLambda, intPair), std::make_pair(4, 3));
    REQUIRE_EQ(transform_snd(squareLambda, intPair), std::make_pair(2, 9));
    REQUIRE_EQ(transform_fst([](auto i) { return i * i; }, intPair), std::make_pair(4, 3));
    REQUIRE_EQ(transform_snd([](auto i) { return i * i; }, intPair), std::make_pair(2, 9));
    REQUIRE_EQ(transform_pair(squareLambda, squareLambda, intPair), std::make_pair(4, 9));

    typedef std::vector<std::pair<std::string, int>> StringIntPairs;
    StringIntPairs stringIntPairs = {{"a", 1}, {"a", 2}, {"b", 6}, {"a", 4}};
    auto stringIntPairsAsMapGrouped = pairs_to_map_grouped(stringIntPairs);
    auto groupNames = transform(fst<std::string, int>, stringIntPairs);
    auto groupNameToMedianMap = transform_map_values(median<std::vector<int>>, stringIntPairsAsMapGrouped);
    auto getMedianValue = bind_1st_and_2nd_of_3(get_from_map_with_def<std::map<std::string, int>>, groupNameToMedianMap, 0);
    auto groupMendianValues = transform(getMedianValue, groupNames);
    auto stringIntPairsSndReplacedWithGroupMedian = zip(groupNames, groupMendianValues);
    REQUIRE_EQ(stringIntPairsSndReplacedWithGroupMedian, StringIntPairs({{"a", 2}, {"a", 2}, {"b", 6}, {"a", 2}}));
    const std::function<int(int)> double_int = [](int x) -> int {
      return 2 * x;
    };
    REQUIRE_EQ(transform_pair(double_int, double_int, IntPair({2, 3})),
        IntPair({4, 6}));
    // Thanks to invoke, such code works.
    // (I don't have a use case for it though)
    dummy dumb;
    dumb.i = 42;

    auto p = std::make_pair(dumb, dumb);
    auto result = transform_pair(&dummy::i, &dummy::i, p);
    REQUIRE_EQ(result, std::make_pair(42, 42));
}

TEST_CASE("pairs_test - enumerate")
{
    using namespace fplus;
    REQUIRE_EQ(enumerate(xs), (std::vector<std::pair<std::size_t, int>>({{0,1}, {1,2}, {2,2}, {3,3}, {4,2}})));
}

TEST_CASE("pairs_test - adjacent_pairs")
{
    using namespace fplus;
    REQUIRE_EQ(adjacent_pairs(xs), IntPairs({{1,2},{2,3}}));
    REQUIRE_EQ(adjacent_pairs(IntVector({1,2,2,3})), IntPairs({{1,2},{2,3}}));
}

TEST_CASE("pairs_test - overlapping_pairs")
{
    using namespace fplus;
    REQUIRE_EQ(overlapping_pairs(xs), IntPairs({{1,2},{2,2},{2,3},{3,2}}));
}

TEST_CASE("pairs_test - overlapping_pairs_cyclic")
{
    using namespace fplus;
    REQUIRE_EQ(overlapping_pairs_cyclic(xs), IntPairs({{1,2},{2,2},{2,3},{3,2},{2,1}}));
}

TEST_CASE("pairs_test - first_mismatch_idx_on")
{
    using namespace fplus;
    REQUIRE_EQ(first_mismatch_idx_on(is_even<int>, IntVector({1,2,3}), IntVector({3,5,3})), just<std::size_t>(1));
    REQUIRE_EQ(first_mismatch_idx_on(is_even<int>, IntVector({1,2,3}), IntVector({1,5})), just<std::size_t>(1));
    REQUIRE_EQ(first_mismatch_idx_on(is_even<int>, IntVector({1,2,3}), IntVector({1,6})), nothing<std::size_t>());
    REQUIRE_EQ(first_mismatch_idx_on(is_even<int>, IntVector(), IntVector({1,2})), nothing<std::size_t>());
}

TEST_CASE("pairs_test - first_mismatch_on")
{
    using namespace fplus;
    REQUIRE_EQ(first_mismatch_on(is_even<int>, IntVector({1,2,3}), IntVector({3,5,3})), just(IntPair(2,5)));
    REQUIRE_EQ(first_mismatch_on(is_even<int>, IntVector({1,2,3}), IntVector({1,5})), just(IntPair(2,5)));
    REQUIRE_EQ(first_mismatch_on(is_even<int>, IntVector({1,2,3}), IntVector({1,6})), nothing<IntPair>());
    REQUIRE_EQ(first_mismatch_on(is_even<int>, IntVector(), IntVector({1,2})), nothing<IntPair>());
}

TEST_CASE("pairs_test - first_mismatch_idx")
{
    using namespace fplus;
    REQUIRE_EQ(first_mismatch_idx(IntVector({1,2,3}), IntVector({1,4,3})), just<std::size_t>(1));
    REQUIRE_EQ(first_mismatch_idx(IntVector({1,2,3}), IntVector({1,4})), just<std::size_t>(1));
    REQUIRE_EQ(first_mismatch_idx(IntVector({1,2,3}), IntVector({1,2})), nothing<std::size_t>());
    REQUIRE_EQ(first_mismatch_idx(IntVector(), IntVector({1,2})), nothing<std::size_t>());
}

TEST_CASE("pairs_test - first_mismatch")
{
    using namespace fplus;
    REQUIRE_EQ(first_mismatch(IntVector({1,2,3}), IntVector({1,4,3})), just(IntPair(2,4)));
    REQUIRE_EQ(first_mismatch(IntVector({1,2,3}), IntVector({1,4})), just(IntPair(2,4)));
    REQUIRE_EQ(first_mismatch(IntVector({1,2,3}), IntVector({1,2})), nothing<IntPair>());
    REQUIRE_EQ(first_mismatch(IntVector(), IntVector({1,2})), nothing<IntPair>());
}

TEST_CASE("pairs_test - first_match_idx_on")
{
    using namespace fplus;
    REQUIRE_EQ(first_match_idx_on(is_even<int>, IntVector({1,2,3}), IntVector({2,4,3})), just<std::size_t>(1));
    REQUIRE_EQ(first_match_idx_on(is_even<int>, IntVector(), IntVector({1,2})), nothing<std::size_t>());
}

TEST_CASE("pairs_test - first_match")
{
    using namespace fplus;
    REQUIRE_EQ(first_match(IntVector({1,2,3}), IntVector({5,2,3})), just(IntPair(2,2)));
    REQUIRE_EQ(first_match(IntVector(), IntVector({1,2})), nothing<IntPair>());
}

TEST_CASE("pairs_test - zip_repeat")
{
    using namespace fplus;
    typedef std::vector<std::string> Strings;
    typedef std::pair<std::string, int> Pairs;
    Strings label{"even", "odd"};
    std::vector<Pairs> expected{{"even", 0}, {"odd", 1}, {"even", 2}};
    REQUIRE_EQ(zip_repeat(label, IntVector{0,1}), zip(label, IntVector{0, 1}));
    REQUIRE_EQ(zip_repeat(label, IntVector{0,1,2}), expected);
    REQUIRE_EQ(zip_repeat(IntVector{0,1,2}, label), transform(fwd::swap_pair_elems(), expected));
}