// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <fplus/container_common.hpp>

#include <algorithm>
#include <set>
#include <unordered_set>

namespace fplus
{

// API search type: set_includes : (Set a, Set a) -> Bool
// fwd bind count: 1
// Checks if every element of the second set is also present in the first set.
// Also known as is_subset_of.
template <typename SetType>
bool set_includes(const SetType& set1, const SetType& set2)
{
    return std::includes(std::begin(set1), std::end(set1),
        std::begin(set2), std::end(set2));
}

// API search type: unordered_set_includes : (Unordered_Set a, Unordered_Set a) -> Bool
// fwd bind count: 1
// Checks if every element of the second unordered_set
// is also present in the first unordered_set.
// Also known as is_subset_of.
template <typename UnorderSetType>
bool unordered_set_includes(const UnorderSetType& set1,
                            const UnorderSetType& set2)
{
    auto first_not_included = std::find_if(set2.begin(), set2.end(),
        [&](const typename UnorderSetType::value_type& x)
        -> bool { return set1.find(x) == set1.end();});
    return first_not_included == set2.end();
}

// API search type: set_merge : (Set a, Set a) -> Set a
// fwd bind count: 1
// Returns the union of two given sets.
template <typename SetType>
SetType set_merge(const SetType& set1, const SetType& set2)
{
    SetType result;
    auto itOut = internal::get_back_inserter(result);
    std::merge(std::begin(set1), std::end(set1),
        std::begin(set2), std::end(set2),
        itOut);
    return result;
}

// API search type: unordered_set_merge : (Unordered_Set a, Unordered_Set a) -> Unordered_Set a
// fwd bind count: 1
// Returns the union of two given sets.
template <typename UnorderSetType>
UnorderSetType unordered_set_merge(const UnorderSetType& set1, const UnorderSetType& set2)
{
    UnorderSetType result;
    auto itOut = internal::get_back_inserter(result);
    std::copy(std::begin(set1), std::end(set1), itOut);
    std::copy(std::begin(set2), std::end(set2), itOut);
    return result;
}

// API search type: set_intersection : (Set a, Set a) -> Set a
// fwd bind count: 1
// Returns the intersection of both sets.
template <typename SetType>
SetType set_intersection(const SetType& set1, const SetType& set2)
{
    SetType result;
    auto itOut = internal::get_back_inserter(result);
    std::set_intersection(std::begin(set1), std::end(set1),
        std::begin(set2), std::end(set2),
        itOut);
    return result;
}

// API search type: unordered_set_intersection : (Unordered_Set a, Unordered_Set a) -> Unordered_Set a
// fwd bind count: 1
// Returns the intersection of both unordered_sets.
template <typename UnorderSetType>
UnorderSetType unordered_set_intersection(
    const UnorderSetType& set1, const UnorderSetType& set2)
{
    UnorderSetType result;
    auto itOut = internal::get_back_inserter(result);
    std::copy_if(std::begin(set2), std::end(set2),
        itOut, [&](const typename UnorderSetType::value_type &x)
        -> bool {return set1.find(x) != set1.end();});
    return result;
}

// API search type: set_is_disjoint : (Set a, Set a) -> Bool
// fwd bind count: 1
// Checks if two sets are disjoint.
template <typename SetType>
bool set_is_disjoint(const SetType& set1, const SetType& set2)
{
    return set_intersection(set1, set2).empty();
}

// API search type: unordered_set_is_disjoint : (Unordered_Set a, Unordered_Set a) -> Bool
// fwd bind count: 1
// Checks if two unordered_sets are disjoint.
template <typename UnorderSetType>
bool unordered_set_is_disjoint(
    const UnorderSetType& set1, const UnorderSetType& set2)
{
    return unordered_set_intersection(set1, set2).empty();
}

// API search type: set_difference : (Set a, Set a) -> Set a
// fwd bind count: 1
// Returns the elements in set1 that are not present in set2.
template <typename SetType>
SetType set_difference(const SetType& set1, const SetType& set2)
{
    SetType result;
    auto itOut = internal::get_back_inserter(result);
    std::set_difference(std::begin(set1), std::end(set1),
        std::begin(set2), std::end(set2),
        itOut);
    return result;
}

// API search type: unordered_set_difference : (Unordered_Set a, Unordered_Set a) -> Unordered_Set a
// fwd bind count: 1
// Returns the elements in unordered_set1
// that are not present in unordered_set2.
template <typename UnorderSetType>
UnorderSetType unordered_set_difference(const UnorderSetType& set1,
const UnorderSetType& set2)
{
    UnorderSetType result;
    auto itOut = internal::get_back_inserter(result);
    std::copy_if(std::begin(set1), std::end(set1),
        itOut, [&](const typename UnorderSetType::value_type &x)
        -> bool {return set2.find(x) == set2.end();});
    return result;
}

// API search type: set_symmetric_difference : (Set a, Set a) -> Set a
// fwd bind count: 1
// Returns the symmetric difference of both sets.
template <typename SetType>
SetType set_symmetric_difference(const SetType& set1, const SetType& set2)
{
    SetType result;
    auto itOut = internal::get_back_inserter(result);
    std::set_symmetric_difference(std::begin(set1), std::end(set1),
        std::begin(set2), std::end(set2),
        itOut);
    return result;
}

// API search type: unordered_set_symmetric_difference : (Unordered_Set a, Unordered_Set a) -> Unordered_Set a
// fwd bind count: 1
// Returns the symmetric difference of both unordered_sets.
template <typename UnorderSetType>
UnorderSetType unordered_set_symmetric_difference(
    const UnorderSetType& set1, const UnorderSetType& set2)
{
    UnorderSetType result;
    auto itOut = internal::get_back_inserter(result);
    std::copy_if(std::begin(set1), std::end(set1),
        itOut, [&](const typename UnorderSetType::value_type &x)
        -> bool {return set2.find(x) == set2.end();});
    std::copy_if(std::begin(set2), std::end(set2),
        itOut, [&](const typename UnorderSetType::value_type &x)
        -> bool {return set1.find(x) == set1.end();});
    return result;
}

// API search type: sets_intersection : [Set a] -> Set a
// fwd bind count: 0
// Returns the intersection of the given sets.
// Also known as intersect_many.
// For performance try to sort the inputs sets prior, ascendending by size.
template <typename ContainerIn,
    typename SetType = typename ContainerIn::value_type>
SetType sets_intersection(const ContainerIn& sets)
{
    return fold_left_1(set_intersection<SetType>, sets);
}

// API search type: unordered_sets_intersection : [Unordered_Set a] -> Unordered_Set a
// fwd bind count: 0
// Returns the intersection of the given unordered_sets.
// Also known as intersect_many.
template <typename ContainerIn,
    typename UnordSetType = typename ContainerIn::value_type>
UnordSetType unordered_sets_intersection(const ContainerIn& sets)
{
    return fold_left_1(unordered_set_intersection<UnordSetType>, sets);
}

} // namespace fplus
