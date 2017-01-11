// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <fplus/fplus.hpp>

namespace {
    auto squareLambda = [](int x) -> int { return x*x; };
    typedef std::vector<int> IntVector;
    typedef std::pair<int, int> IntPair;
    typedef std::vector<IntPair> IntPairs;
    IntVector xs = {1,2,2,3,2};
}


TEST_CASE("pairs_test, zip_with")
{
    using namespace fplus;
    const auto multiply = [](int x, int y){ return x * y; };
    REQUIRE_EQ(zip_with(multiply, xs, xs), transform(squareLambda, xs));
    const auto add = [](int x, int y){ return x + y; };
    REQUIRE_EQ(zip_with(add, IntVector({1,2,3}), IntVector({1,2})), IntVector({2,4}));
    REQUIRE_EQ(zip_with(add, IntVector({1,2}), IntVector({1,2,3})), IntVector({2,4}));
}

TEST_CASE("pairs_test, zip_with_defaults")
{
    using namespace fplus;
    const auto add = [](int x, int y){ return x + y; };
    REQUIRE_EQ(zip_with_defaults(add, 6, 7, IntVector({1,2,3}), IntVector({1,2})), IntVector({2,4,10}));
    REQUIRE_EQ(zip_with_defaults(add, 6, 7, IntVector({1,2}), IntVector({1,2,3})), IntVector({2,4,9}));
}

TEST_CASE("pairs_test, zip")
{
    using namespace fplus;
    auto xsZippedWithXs = zip(xs, xs);
    REQUIRE_EQ(unzip(xsZippedWithXs).first, xs);
}

TEST_CASE("pairs_test, pair functions")
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

    typedef std::vector<std::pair<std::string, int>> StringIntPairs;
    StringIntPairs stringIntPairs = {{"a", 1}, {"a", 2}, {"b", 6}, {"a", 4}};
    auto stringIntPairsAsMapGrouped = pairs_to_map_grouped(stringIntPairs);
    auto groupNames = transform(fst<std::string, int>, stringIntPairs);
    auto groupNameToMedianMap = transform_map_values(median<std::vector<int>>, stringIntPairsAsMapGrouped);
    auto getMedianValue = bind_1st_and_2nd_of_3(get_from_map_with_def<std::map<std::string, int>>, groupNameToMedianMap, 0);
    auto groupMendianValues = transform(getMedianValue, groupNames);
    auto stringIntPairsSndReplacedWithGroupMedian = zip(groupNames, groupMendianValues);
    REQUIRE_EQ(stringIntPairsSndReplacedWithGroupMedian, StringIntPairs({{"a", 2}, {"a", 2}, {"b", 6}, {"a", 2}}));
}

TEST_CASE("pairs_test, enumerate")
{
    using namespace fplus;
    REQUIRE_EQ(enumerate(xs), (std::vector<std::pair<std::size_t, int>>({{0,1}, {1,2}, {2,2}, {3,3}, {4,2}})));
}