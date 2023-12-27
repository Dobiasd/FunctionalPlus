// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <doctest/doctest.h>
#include <fplus/fplus.hpp>


TEST_CASE("set_test - set functions")
{
    using namespace fplus;
    using IntSet = std::set<int>;
    using setVector = std::vector<IntSet>;
    using IntUnordSet = std::unordered_set<int>;
    using unordSetVector = std::vector<IntUnordSet>;
    using StringUnordSet = std::unordered_set<std::string>;

    //std::set tests
    REQUIRE(set_includes(IntSet({0,1,2,3}), IntSet({0,2})));
    REQUIRE_FALSE(set_includes(IntSet({0,2}), IntSet({0,1,2,3})));
    REQUIRE_FALSE(set_includes(IntSet({0,1,2,3}), IntSet({2,3,4,5})));
    REQUIRE_EQ(set_merge(IntSet({0,1,2,3}), IntSet({2,3,4,5})), IntSet({0,1,2,3,4,5}));
    REQUIRE_EQ(set_merge(IntSet({0,1,2,3}), IntSet({0,2})), IntSet({0,1,2,3}));
    REQUIRE_EQ(set_intersection(IntSet({0,1,2,3}), IntSet({2,3,4,5})), IntSet({2,3}));
    REQUIRE_EQ(set_difference(IntSet({0,1,2,3}), IntSet({2,3,4,5})), IntSet({0,1}));
    REQUIRE_EQ(set_symmetric_difference(IntSet({0,1,2,3}), IntSet({2,3,4,5})), IntSet({0,1,4,5}));
    REQUIRE_EQ(set_intersection(IntSet({0,1,2,3}), IntSet({2,3,4,5})), IntSet({2,3}));
    REQUIRE_EQ(sets_intersection(setVector({IntSet({0,1,2,3}), IntSet({2,3,4,5}), IntSet({0,2})})), IntSet({2}));

    //set::unordered_set tests
    REQUIRE(unordered_set_includes(IntUnordSet({0,1,2,3}), IntUnordSet({0,2})));
    REQUIRE_FALSE(unordered_set_includes(IntUnordSet({0,2}), IntUnordSet({0,1,2,3})));
    REQUIRE_FALSE(unordered_set_includes(IntUnordSet({0,1,2,3}), IntUnordSet({2,3,4,5})));
    REQUIRE_EQ(unordered_set_merge(IntUnordSet({0,1,2,3}), IntUnordSet({2,3,4,5})), IntUnordSet({0,1,2,3,4,5}));
    REQUIRE_EQ(unordered_set_merge(IntUnordSet({0,1,2,3}), IntUnordSet({0,2})), IntUnordSet({0,1,2,3}));
    REQUIRE_EQ(unordered_set_intersection(IntUnordSet({0,1,2,3}), IntUnordSet({2,3,4,5})), IntUnordSet({2,3}));
    REQUIRE_EQ(unordered_set_difference(IntUnordSet({0,1,2,3}), IntUnordSet({2,3,4,5})), IntUnordSet({0,1}));
    REQUIRE_EQ(unordered_set_symmetric_difference(IntUnordSet({0,1,2,3}), IntUnordSet({2,3,4,5})), IntUnordSet({0,1,4,5}));
    REQUIRE_EQ(unordered_set_intersection(IntUnordSet({0,1,2,3}), IntUnordSet({2,3,4,5})), IntUnordSet({2,3}));
    REQUIRE_EQ(unordered_sets_intersection(unordSetVector({IntUnordSet({0,1,2,3}), IntUnordSet({2,3,4,5}), IntUnordSet({0,2})})), IntUnordSet({2}));

    REQUIRE_EQ(unordered_set_merge(StringUnordSet({"yes", "no", "hello"}), StringUnordSet({"hello", "what"})), StringUnordSet({"yes", "no", "what", "hello"}));

    REQUIRE(set_is_disjoint(IntSet({0,1,3}), IntSet({2,4})));
    REQUIRE_FALSE(set_is_disjoint(IntSet({0,1,3}), IntSet({2,1})));

    REQUIRE(unordered_set_is_disjoint(IntUnordSet({0,1,3}), IntUnordSet({2,4})));
    REQUIRE_FALSE(unordered_set_is_disjoint(IntUnordSet({0,1,3}), IntUnordSet({2,1})));
}
