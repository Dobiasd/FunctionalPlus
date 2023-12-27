// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <doctest/doctest.h>
#include <fplus/fplus.hpp>

namespace {
    typedef std::vector<int> IntVector;
    IntVector xs = {1,2,2,3,2};
    typedef std::vector<IntVector> IntVectors;
    typedef std::vector<std::size_t> IdxVector;
    typedef std::list<int> IntList;
    typedef std::vector<IntList> IntLists;
    auto is_even_int = [](int x){ return x % 2 == 0; };
}

TEST_CASE("split_test - split functions")
{
    using namespace fplus;

    REQUIRE_EQ(split(0, true, IntVector{1,0,1}), IntVectors({{1},{1}}));
    REQUIRE_EQ(split(0, true, IntVector{0,1,0}), IntVectors({{},{1},{}}));
    REQUIRE_EQ(split(0, true, IntVector{0,1}), IntVectors({{},{1}}));
    REQUIRE_EQ(split(0, true, IntVector{1,0}), IntVectors({{1},{}}));
    REQUIRE_EQ(split(0, true, IntVector{0}), IntVectors({{},{}}));
    REQUIRE_EQ(split(0, true, IntVector{}), IntVectors{{}});

    REQUIRE_EQ(split(0, false, IntVector{1,0,1}), IntVectors({{1},{1}}));
    REQUIRE_EQ(split(0, false, IntVector{0,1,0}), IntVectors{{1}});
    REQUIRE_EQ(split(0, false, IntVector{0,1}), IntVectors{{1}});
    REQUIRE_EQ(split(0, false, IntVector{1,0}), IntVectors{{1}});
    REQUIRE_EQ(split(0, false, IntVector{0}), IntVectors({}));
    REQUIRE_EQ(split(0, false, IntVector{}), IntVectors({}));

    REQUIRE_EQ(split(0, true, IntVector{1,0,1}), IntVectors({{1},{1}}));
    REQUIRE_EQ(split(0, true, IntVector{0,1,0}), IntVectors({{},{1},{}}));
    REQUIRE_EQ(split(0, true, IntVector{0,1}), IntVectors({{},{1}}));
    REQUIRE_EQ(split(0, true, IntVector{1,0}), IntVectors({{1},{}}));
    REQUIRE_EQ(split(0, true, IntVector{0}), IntVectors({{},{}}));
    REQUIRE_EQ(split(0, true, IntVector{}), IntVectors{{}});

    REQUIRE_EQ(split(0, false, IntVector{1,0,1}), IntVectors({{1},{1}}));
    REQUIRE_EQ(split(0, false, IntVector{0,1,0}), IntVectors{{1}});
    REQUIRE_EQ(split(0, false, IntVector{0,1}), IntVectors{{1}});
    REQUIRE_EQ(split(0, false, IntVector{1,0}), IntVectors{{1}});
    REQUIRE_EQ(split(0, false, IntVector{0}), IntVectors({}));
    REQUIRE_EQ(split(0, false, IntVector{}), IntVectors({}));

    REQUIRE_EQ(split_by_token(IntVector({0}), true, IntVector{1,0,1}), IntVectors({{1},{1}}));
    REQUIRE_EQ(split_by_token(IntVector({0}), true, IntVector{0,1,0}), IntVectors({{},{1},{}}));
    REQUIRE_EQ(split_by_token(IntVector({0}), true, IntVector{0,1}), IntVectors({{},{1}}));
    REQUIRE_EQ(split_by_token(IntVector({0}), true, IntVector{1,0}), IntVectors({{1},{}}));
    REQUIRE_EQ(split_by_token(IntVector({0}), true, IntVector{0}), IntVectors({{},{}}));
    REQUIRE_EQ(split_by_token(IntVector({0}), true, IntVector{}), IntVectors{{}});

    REQUIRE_EQ(split_by_token(IntVector({0}), false, IntVector{1,0,1}), IntVectors({{1},{1}}));
    REQUIRE_EQ(split_by_token(IntVector({0}), false, IntVector{0,1,0}), IntVectors{{1}});
    REQUIRE_EQ(split_by_token(IntVector({0}), false, IntVector{0,1}), IntVectors{{1}});
    REQUIRE_EQ(split_by_token(IntVector({0}), false, IntVector{1,0}), IntVectors{{1}});
    REQUIRE_EQ(split_by_token(IntVector({0}), false, IntVector{0}), IntVectors({}));
    REQUIRE_EQ(split_by_token(IntVector({0}), false, IntVector{}), IntVectors({}));

    REQUIRE_EQ(split_by_token(IntVector({0,9}), true, IntVector{1,0,9,1}), IntVectors({{1},{1}}));
    REQUIRE_EQ(split_by_token(IntVector({0,9}), true, IntVector{0,9,1,0,9}), IntVectors({{},{1},{}}));
    REQUIRE_EQ(split_by_token(IntVector({0,9}), true, IntVector{0,9,1}), IntVectors({{},{1}}));
    REQUIRE_EQ(split_by_token(IntVector({0,9}), true, IntVector{1,0,9}), IntVectors({{1},{}}));
    REQUIRE_EQ(split_by_token(IntVector({0,9}), true, IntVector{0,9}), IntVectors({{},{}}));
    REQUIRE_EQ(split_by_token(IntVector({0,9}), true, IntVector{}), IntVectors{{}});

    REQUIRE_EQ(split_by_token(IntVector({0,9}), false, IntVector{1,0,9,1}), IntVectors({{1},{1}}));
    REQUIRE_EQ(split_by_token(IntVector({0,9}), false, IntVector{0,9,1,0,9}), IntVectors{{1}});
    REQUIRE_EQ(split_by_token(IntVector({0,9}), false, IntVector{0,9,1}), IntVectors{{1}});
    REQUIRE_EQ(split_by_token(IntVector({0,9}), false, IntVector{1,0,9}), IntVectors{{1}});
    REQUIRE_EQ(split_by_token(IntVector({0,9}), false, IntVector{0,9}), IntVectors({}));
    REQUIRE_EQ(split_by_token(IntVector({0,9}), false, IntVector{}), IntVectors({}));

    REQUIRE_EQ(split_by_token(IntVector({}), true, IntVector{}), IntVectors({{},{}}));
    REQUIRE_EQ(split_by_token(IntVector({}), true, IntVector{1}), IntVectors({{},{1},{}}));
    REQUIRE_EQ(split_by_token(IntVector({}), true, IntVector{1,2}), IntVectors({{},{1},{2},{}}));

    REQUIRE_EQ(split_by_token(IntVector({}), false, IntVector{}), IntVectors({}));
    REQUIRE_EQ(split_by_token(IntVector({}), false, IntVector{1}), IntVectors{{1}});
    REQUIRE_EQ(split_by_token(IntVector({}), false, IntVector{1,2}), IntVectors({{1},{2}}));

    REQUIRE_EQ(split_one_of(IntVector({0,3}), true, IntVector{1,3,2,0,0,6,0,7,5}), IntVectors({{1},{2},{},{6},{7,5}}));
    REQUIRE_EQ(split_one_of(std::string(" ,.?"), false, std::string("Hi, how are you?")), std::vector<std::string>({"Hi", "how", "are", "you"}));

    REQUIRE_EQ(split_at_idx(2, xs), std::make_pair(IntVector({1,2}), IntVector({2,3,2})));
    REQUIRE_EQ(partition(is_even_int, xs), std::make_pair(IntVector({2,2,2}), IntVector({1,3})));
    REQUIRE_EQ(split_every(2, xs), IntVectors({{1,2}, {2, 3}, {2}}));
    auto splittedAt1And3 = split_at_idxs(IdxVector({1,3}), xs);
    IntVectors splittedAt1And3Dest = {IntVector({1}), IntVector({2,2}), IntVector({3,2})};
    REQUIRE_EQ(splittedAt1And3, splittedAt1And3Dest);
    auto splittedAt1And3And3 = split_at_idxs(IdxVector({1,3,3}), xs);
    IntVectors splittedAt1And3And3Dest = {IntVector({1}), IntVector({2,2}), IntVector({}), IntVector({3,2})};
    REQUIRE_EQ(splittedAt1And3And3, splittedAt1And3And3Dest);
    REQUIRE_EQ(split(3, true, xs), IntVectors({IntVector({1, 2, 2}), IntVector({2})}));
    REQUIRE_EQ(split(1, true, IntVector{0,1,2}), IntVectors({{0},{2}}));
    REQUIRE_EQ(split(2, true, IntVector{5,2,0,3}), IntVectors({{5},{0,3}}));

    REQUIRE_EQ(split(2, true, IntVector{1,2}), IntVectors({{1},{}}));
    REQUIRE_EQ(split(2, true, IntVector{2}), IntVectors({{},{}}));
    REQUIRE_EQ(split(2, true, IntVector{}), IntVectors{{}});

    REQUIRE_EQ(split_by(is_even_int, true, IntList({1,3,2,2,5,5,3,6,7,9})), IntLists({{1,3},{},{5,5,3},{7,9}}));
    REQUIRE_EQ(split_by_keep_separators(is_even_int, IntList({})), IntLists());
    REQUIRE_EQ(split_by_keep_separators(is_even_int, IntList({2})), IntLists({IntList({2})}));
    REQUIRE_EQ(split_by_keep_separators(is_even_int, IntList({2,2,3})), IntLists({{2},{2,3}}));
    REQUIRE_EQ(split_by_keep_separators(is_even_int, IntList({1,3,2})), IntLists({{1,3},{2}}));
    REQUIRE_EQ(split_by_keep_separators(is_even_int, IntList({1,3,2,2,5,5,3,6,7,9})), IntLists({{1,3},{2},{2,5,5,3},{6,7,9}}));
    REQUIRE_EQ(split_keep_separators(2, IntList({1,3,2,2,5,5,3,2,7,9})), IntLists({{1,3},{2},{2,5,5,3},{2,7,9}}));
}