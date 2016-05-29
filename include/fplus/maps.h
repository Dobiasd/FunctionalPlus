// Copyright Tobias Hermann 2015.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "composition.h"
#include "container_common.h"
#include "transform.h"
#include "pairs.h"

#include <map>
#include <unordered_map>

namespace fplus
{

// API search type: pairs_to_map : [(a, b)] -> Map a b
// Converts a Container of pairs (key, value) into a dictionary.
template <typename MapOut, typename ContainerIn>
MapOut pairs_to_map(const ContainerIn& pairs)
{
    return convert_container_and_elems<MapOut>(pairs);
}

// API search type: pairs_to_map_grouped : [(a, b)] -> Map a [b]
// pairs_to_map_grouped([("a", 1), ("a", 2), ("b", 6), ("a", 4)])
//     -> [("a", [1, 2, 4]), ("b", [6])]
template <typename ContainerIn,
    typename Key = typename ContainerIn::value_type::first_type,
    typename SingleValue = typename ContainerIn::value_type::second_type,
    typename MapOut = std::map<Key, std::vector<SingleValue>>>
MapOut pairs_to_map_grouped(const ContainerIn& pairs)
{
    MapOut result;
    for (const auto& p : pairs)
    {
        result[p.first].push_back(p.second);
    }
    return result;
}

// API search type: map_to_pairs : Map a b -> [(a, b)]
// Converts a dictionary into a Container of pairs (key, value).
template <typename MapType,
    typename MapPair = typename MapType::value_type,
    typename Key = typename std::remove_const<typename MapPair::first_type>::type,
    typename Val = typename std::remove_const<typename MapPair::second_type>::type,
    typename OutPair = std::pair<Key, Val>,
    typename ContainerOut = std::vector<OutPair>>
ContainerOut map_to_pairs(const MapType& dict)
{
    return convert_container_and_elems<ContainerOut>(dict);
}

// API search type: transform_map_values : (b -> c) -> Map a b -> Map a c
// transform_map_values((*2), {(0, 2), (1, 3)}) == {(0, 4), (1, 6)}
template <typename F, typename MapIn,
    typename MapInPair = typename MapIn::value_type,
    typename Key = typename std::remove_const<typename MapInPair::first_type>::type,
    typename InVal = typename std::remove_const<typename MapInPair::second_type>::type,
    typename OutVal = typename std::remove_reference<typename std::remove_const<
        typename utils::function_traits<F>::result_type>::type>::type,
    typename MapOut = typename SameMapTypeNewTypes<MapIn, Key, OutVal>::type>
MapOut transform_map_values(F f, const MapIn& map)
{
    return pairs_to_map<MapOut>(
        transform(
            bind_1st_of_2(transform_snd<Key, InVal, F>, f),
            map_to_pairs(map)));
}

// API search type: map_union_with : (b -> b -> b) -> Map a b -> Map a b -> Map a b
// map_union_with((++), [(0,a), (1,b)], [(0,c), (2,d)]) == [(0,ac), (1,b), (2,d)]
template <typename F, typename MapIn,
    typename MapInPair = typename MapIn::value_type,
    typename Key = typename std::remove_const<typename MapInPair::first_type>::type,
    typename OutVal = typename std::remove_reference<typename std::remove_const<
        typename utils::function_traits<F>::result_type>::type>::type,
    typename MapOut = typename SameMapTypeNewTypes<MapIn, Key, OutVal>::type>
MapOut map_union_with(F f, const MapIn& dict1, const MapIn& dict2)
{
    auto full_map = pairs_to_map_grouped(
            append(map_to_pairs(dict1), map_to_pairs(dict2)));
    return transform_map_values(f, full_map);
}

// API search type: map_union : Map a b -> Map a b -> Map a b
// map_union([(0,a), (1,b)], [(0,c), (2,d)]) == [(0,a), (1,b), (2,d)]
template <typename MapType,
    typename MapInPair = typename MapType::value_type,
    typename InVal = typename std::remove_const<typename MapInPair::second_type>::type>
MapType map_union(const MapType& dict1, const MapType& dict2)
{
    typedef typename MapType::value_type::second_type value;
    typedef std::vector<value> values;
    return map_union_with(nth_element<values>(0), dict1, dict2);
}

// API search type: get_map_keys : Map a b -> [a]
// Returns all keys used in a map.
template <typename MapType,
    typename ContainerOut =
        std::vector<typename std::remove_const<typename MapType::key_type>::type>>
ContainerOut get_map_keys(const MapType& dict)
{
    auto pairs = map_to_pairs(dict);
    typedef typename decltype(pairs)::value_type::first_type FirstType;
    typedef typename decltype(pairs)::value_type::second_type SecondType;
    return transform(fst<FirstType, SecondType>, map_to_pairs(dict));
}

// API search type: get_map_values : Map a b -> [b]
// Returns all values present in a map.
template <typename MapType,
    typename ContainerOut =
        std::vector<typename std::remove_const<typename MapType::mapped_type>::type>>
ContainerOut get_map_values(const MapType& dict)
{
    auto pairs = map_to_pairs(dict);
    typedef typename decltype(pairs)::value_type::first_type FirstType;
    typedef typename decltype(pairs)::value_type::second_type SecondType;
    return transform(snd<FirstType, SecondType>, map_to_pairs(dict));
}

// API search type: swap_keys_and_values : Map a b -> Map b a
// Swaps keys and Values of a dict:
// swap_keys_and_values({(1, "a"), (2, "b")}) == {("a", 1), ("b", 2)}
template <typename MapIn,
    typename MapInPair = typename MapIn::value_type,
    typename InKey = typename MapInPair::first_type,
    typename InVal = typename MapInPair::second_type,
    typename OutKey = InVal,
    typename OutVal = typename std::remove_const<InKey>::type,
    typename MapOut = typename SameMapTypeNewTypes<MapIn, OutKey, OutVal>::type>
MapOut swap_keys_and_values(const MapIn& dict)
{
    auto inAsPairs = map_to_pairs(dict);
    auto outAsPairs = transform(swap_pair_elems<InKey, InVal>, inAsPairs);
    return pairs_to_map<MapOut>(outAsPairs);
}

// API search type: create_map : [a] -> [b] -> Map a b
// create_map([1,2,3], ["one", "two"]) == { {1,"one"}, {2,"two"} }
template <typename ContainerIn1, typename ContainerIn2,
    typename Key = typename std::remove_const<typename ContainerIn1::value_type>::type,
    typename Val = typename std::remove_const<typename ContainerIn2::value_type>::type,
    typename MapOut = std::map<Key, Val>>
MapOut create_map(const ContainerIn1& keys, const ContainerIn2& values)
{
    auto pairs = zip(keys, values);
    return pairs_to_map<MapOut>(pairs);
}

// API search type: create_unordered_map : [a] -> [b] -> Map a b
// create_unordered_map([1,2,3], ["one", "two"]) == { {1,"one"}, {2,"two"} }
template <typename ContainerIn1, typename ContainerIn2,
    typename Key = typename std::remove_const<typename ContainerIn1::value_type>::type,
    typename Val = typename std::remove_const<typename ContainerIn2::value_type>::type,
    typename MapOut = std::unordered_map<Key, Val >>
MapOut create_unordered_map(
    const ContainerIn1& keys,
    const ContainerIn2& values)
{
    auto pairs = zip(keys, values);
    return pairs_to_map<MapOut>(pairs);
}

// API search type: get_from_map : Map a b -> a -> Maybe b
// Returns just the value of a key if key is present.
// Otherwise returns nothing.
template <typename MapType,
    typename Key = typename MapType::key_type,
    typename Val = typename MapType::mapped_type>
maybe<Val> get_from_map(const MapType& map, const Key& key)
{
    auto it = map.find(key);
    if (it == std::end(map))
        return nothing<Val>();
    return just(it->second);
}

// API search type: get_from_map_with_def : Map a b -> b -> a -> b
// Returns the value of a key if key is present.
// Otherwise returns the provided default.
template <typename MapType,
    typename Key = typename MapType::key_type,
    typename Val = typename MapType::mapped_type>
Val get_from_map_with_def(const MapType& map, const Val& defVal,
    const Key& key)
{
    return just_with_default(defVal, get_from_map(map, key));
}

// API search type: map_contains : Map a b -> a -> Bool
// Checks if a map contains a key.
template <typename MapType, typename Key = typename MapType::key_type>
bool map_contains(const MapType& map, const Key& key)
{
    auto it = map.find(key);
    return it != std::end(map);
}

} // namespace fplus
