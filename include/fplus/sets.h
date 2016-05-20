// Copyright Tobias Hermann 2015.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "container_common.h"

#include <algorithm>
#include <set>
#include <unordered_set>

namespace fplus
{

// API search type: includes : Set a -> Set a -> Bool
// Checks if every element of the second set is also present in the first set.
template <typename SetType>
bool set_includes(const SetType& set1, const SetType& set2)
{
    return std::includes(std::begin(set1), std::end(set1),
        std::begin(set2), std::end(set2));
}

// API search type: merge : Set a -> Set a -> Set a
// Returns the union of two given sets.
template <typename SetType>
SetType set_merge(const SetType& set1, const SetType& set2)
{
    SetType result;
    auto itOut = get_back_inserter(result);
    std::merge(std::begin(set1), std::end(set1),
        std::begin(set2), std::end(set2),
        itOut);
    return result;
}

// API search type: set_intersection : Set a -> Set a -> Set a
// Returns the intersection of both sets.
template <typename SetType>
SetType set_intersection(const SetType& set1, const SetType& set2)
{
    SetType result;
    auto itOut = get_back_inserter(result);
    std::set_intersection(std::begin(set1), std::end(set1),
        std::begin(set2), std::end(set2),
        itOut);
    return result;
}

// API search type: set_difference : Set a -> Set a -> Set a
// Returns the elements in set1 that are not present in set2.
template <typename SetType>
SetType set_difference(const SetType& set1, const SetType& set2)
{
    SetType result;
    auto itOut = get_back_inserter(result);
    std::set_difference(std::begin(set1), std::end(set1),
        std::begin(set2), std::end(set2),
        itOut);
    return result;
}

// API search type: set_symmetric_difference : Set a -> Set a -> Set a
// Returns the symmetric difference of both sets.
template <typename SetType>
SetType set_symmetric_difference(const SetType& set1, const SetType& set2)
{
    SetType result;
    auto itOut = get_back_inserter(result);
    std::set_symmetric_difference(std::begin(set1), std::end(set1),
        std::begin(set2), std::end(set2),
        itOut);
    return result;
}
} // namespace fplus
