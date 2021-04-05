// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <doctest/doctest.h>
#include <fplus/fplus.hpp>


namespace {
    typedef std::pair<int, int> IntPair;
    typedef std::vector<int> IntVector;
    typedef std::vector<IntPair> IntPairVector;
    typedef fplus::tree<IntPair> IntPairTree;
    typedef fplus::tree<int> IntTree;
    typedef std::vector<IntPairTree> IntPairTreeVector;
}

TEST_CASE("tree_test - are_trees_equal")
{
    using namespace fplus;

    const IntPairTree a =
        {{0, 8}, {
            {{0, 4}, {}},
            {{5, 7}, {}}}};

    const IntPairTree b =
        {{0, 8}, {
            {{5, 7}, {}},
            {{0, 4}, {}}}};

    REQUIRE(are_trees_equal(a, b));
}

TEST_CASE("tree_test - sequence_to_tree small")
{
    using namespace fplus;
    IntPairVector elems = {
        {0, 4},
        {0, 8},
        {5, 7}
    };

    const auto is_child_of = [](const IntPair& a, const IntPair& b) -> bool
    {
        return a.first >= b.first && a.second <= b.second;
    };

    const auto result_1 = trees_from_sequence(is_child_of, elems);
    const auto result_2 = trees_from_sequence(is_child_of, fplus::shuffle(0, elems));

    const IntPairTreeVector result = {
        {{0, 8}, {
            {{0, 4}, {}},
            {{5, 7}, {}}}}
        };

    REQUIRE_EQ(result_1.size(), result_2.size());
    REQUIRE(all(zip_with(are_trees_equal<IntPair>, result_1, result_2)));

    REQUIRE_EQ(result_1.size(), result.size());
    REQUIRE(all(zip_with(are_trees_equal<IntPair>, result_1, result)));
}

TEST_CASE("tree_test - sequence_to_tree medium")
{
    using namespace fplus;
    IntPairVector elems = {
        {0, 4},
        {0, 8},
        {5, 7},
        {9, 10},
        {12, 13},
        {9, 17},
        {11, 15},
        {13, 14},
        {8, 20}
    };

    const auto is_child_of = [](const IntPair& a, const IntPair& b) -> bool
    {
        return a.first >= b.first && a.second <= b.second;
    };

    const auto result_1 = trees_from_sequence(is_child_of, elems);
    const auto result_2 = trees_from_sequence(is_child_of, fplus::shuffle(0, elems));

    const IntPairTreeVector result = {
        {{0, 8}, {
            {{0, 4}, {}},
            {{5, 7}, {}}
        }},
        {{8, 20}, {
            {{9, 17}, {
                {{9, 10}, {}},
                {{11, 15}, {
                    {{12, 13}, {}},
                    {{13, 14}, {}}
                }}
            }},
        }},
    };

    REQUIRE_EQ(result_1.size(), result_2.size());
    REQUIRE(all(zip_with(are_trees_equal<IntPair>, result_1, result_2)));

    REQUIRE_EQ(result_1.size(), result.size());
    REQUIRE(all(zip_with(are_trees_equal<IntPair>, result_1, result)));
}

TEST_CASE("tree_test - tree_depth")
{
    const IntTree t =
        {{0}, {
            {{1}, {
                {{2}, {
                }},
                {{3}, {
                    {{4}, {
                    }},
                    {{5}, {
                    }}
                }},
                {{6}, {
                }}
            }},
            {{7}, {
            }},
            {{8}, {
            }}
        }};
    REQUIRE_EQ(tree_depth(t), 4);
}

TEST_CASE("tree_test - flatten_tree_depth_first")
{
    const IntTree t =
        {{0}, {
            {{1}, {
                {{2}, {
                }},
                {{3}, {
                    {{4}, {
                    }},
                    {{5}, {
                    }}
                }},
                {{6}, {
                }}
            }},
            {{7}, {
            }},
            {{8}, {
            }}
        }};
    REQUIRE_EQ(flatten_tree_depth_first(t), IntVector({0,1,2,3,4,5,6,7,8}));
}

TEST_CASE("tree_test - flatten_tree_breadth_first")
{
    const IntTree t =
        {{0}, {
            {{1}, {
                {{4}, {
                }},
                {{5}, {
                    {{7}, {
                    }},
                    {{8}, {
                    }}
                }},
                {{6}, {
                }}
            }},
            {{2}, {
            }},
            {{3}, {
            }}
        }};
    REQUIRE_EQ(flatten_tree_breadth_first(t), IntVector({0,1,2,3,4,5,6,7,8}));
}
