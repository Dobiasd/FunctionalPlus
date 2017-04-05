// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <fplus/composition.hpp>
#include <fplus/container_common.hpp>
#include <fplus/pairs.hpp>

#include <map>
#include <unordered_map>

namespace fplus
{

// API search type: pairs_to_map : [(key, val)] -> Map key val
// fwd bind count: 0
// Converts a Container of pairs (key, value) into a dictionary.
template <typename MapOut, typename ContainerIn>
MapOut pairs_to_map(const ContainerIn& pairs)
{
    return convert_container_and_elems<MapOut>(pairs);
}

// API search type: pairs_to_map_grouped : [(key, val)] -> Map key [val]
// fwd bind count: 0
// pairs_to_map_grouped([("a", 1), ("a", 2), ("b", 6), ("a", 4)])
//     -> {"a": [1, 2, 4], "b": [6]}
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

// API search type: map_to_pairs : Map key val -> [(key, val)]
// fwd bind count: 0
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

// API search type: transform_map_values : ((old_val -> new_val), Map key old_val) -> Map key new_val
// fwd bind count: 1
// transform_map_values((*2), {0: 2, 1: 3}) == {0: 4, 1: 6}
template <typename F, typename MapIn,
    typename MapInPair = typename MapIn::value_type,
    typename Key = typename std::remove_const<typename MapInPair::first_type>::type,
    typename InVal = typename std::remove_const<typename MapInPair::second_type>::type,
    typename OutVal = typename std::remove_reference<typename std::remove_const<
        typename std::result_of<F(InVal)>::type>::type>::type,
    typename MapOut = typename internal::SameMapTypeNewTypes<MapIn, Key, OutVal>::type>
MapOut transform_map_values(F f, const MapIn& map)
{
    return pairs_to_map<MapOut>(
        transform(
            bind_1st_of_2(transform_snd<Key, InVal, F>, f),
            map_to_pairs(map)));
}

// API search type: map_union_with : (((val, val) -> val), Map key val, Map key val) -> Map key val
// fwd bind count: 2
// map_union_with((++), {0: a, 1: b}, {0: c, 2: d}) == {0: ac, 1: b, 2: d}
template <typename F, typename MapIn,
    typename MapInPair = typename MapIn::value_type,
    typename Key = typename std::remove_const<typename MapInPair::first_type>::type,
    typename Val = typename std::remove_const<typename MapInPair::second_type>::type,
    typename OutVal = typename std::remove_reference<typename std::remove_const<
        typename std::result_of<F(Val, Val)>::type>::type>::type,
    typename MapOut = typename internal::SameMapTypeNewTypes<MapIn, Key, OutVal>::type>
MapOut map_union_with(F f, const MapIn& dict1, const MapIn& dict2)
{
    auto full_map = pairs_to_map_grouped(
            append(map_to_pairs(dict1), map_to_pairs(dict2)));
    const auto group_f = [f](const std::vector<Val>& vals) -> OutVal
    {
        return fold_left_1(f, vals);
    };
    return transform_map_values(group_f, full_map);
}

// API search type: map_union : (Map key val, Map key val) -> Map key val
// fwd bind count: 1
// map_union({0: a, 1: b}, {0: c, 2: d}) == {0: a, 1: b, 2: d}
template <typename MapType,
    typename MapInPair = typename MapType::value_type,
    typename InVal = typename std::remove_const<typename MapInPair::second_type>::type>
MapType map_union(const MapType& dict1, const MapType& dict2)
{
    typedef typename MapType::value_type::second_type value;
    const auto get_first = [](const value& a, const value&) -> value
    {
        return a;
    };
    return map_union_with(get_first, dict1, dict2);
}

// API search type: get_map_keys : Map key val -> [key]
// fwd bind count: 0
// Returns all keys used in a map.
template <typename MapType,
    typename ContainerOut =
        std::vector<typename std::remove_const<typename MapType::key_type>::type>>
ContainerOut get_map_keys(const MapType& dict)
{
    auto pairs = map_to_pairs(dict);
    typedef typename decltype(pairs)::value_type::first_type FirstType;
    typedef typename decltype(pairs)::value_type::second_type SecondType;
    return transform_convert<ContainerOut>(
        fst<FirstType, SecondType>, map_to_pairs(dict));
}

// API search type: get_map_values : Map key val -> [val]
// fwd bind count: 0
// Returns all values present in a map.
template <typename MapType,
    typename ContainerOut =
        std::vector<typename std::remove_const<typename MapType::mapped_type>::type>>
ContainerOut get_map_values(const MapType& dict)
{
    auto pairs = map_to_pairs(dict);
    typedef typename decltype(pairs)::value_type::first_type FirstType;
    typedef typename decltype(pairs)::value_type::second_type SecondType;
    return transform_convert<ContainerOut>(
        snd<FirstType, SecondType>, map_to_pairs(dict));
}

// API search type: swap_keys_and_values : Map a b -> Map b a
// fwd bind count: 0
// Swaps keys and Values of a dict:
// swap_keys_and_values({1: "a", 2: "b"}) == {"a": 1, "b": 2}
template <typename MapIn,
    typename MapInPair = typename MapIn::value_type,
    typename InKey = typename MapInPair::first_type,
    typename InVal = typename MapInPair::second_type,
    typename OutKey = InVal,
    typename OutVal = typename std::remove_const<InKey>::type,
    typename MapOut = typename internal::SameMapTypeNewTypes<MapIn, OutKey, OutVal>::type>
MapOut swap_keys_and_values(const MapIn& dict)
{
    auto inAsPairs = map_to_pairs(dict);
    auto outAsPairs = transform(swap_pair_elems<InKey, InVal>, inAsPairs);
    return pairs_to_map<MapOut>(outAsPairs);
}

// API search type: create_map : ([key], [val]) -> Map key val
// fwd bind count: 1
// create_map([1,2,3], ["one", "two"]) == {1: "one", 2: "two"}
template <typename ContainerIn1, typename ContainerIn2,
    typename Key = typename std::remove_const<typename ContainerIn1::value_type>::type,
    typename Val = typename std::remove_const<typename ContainerIn2::value_type>::type,
    typename MapOut = std::map<Key, Val>>
MapOut create_map(const ContainerIn1& keys, const ContainerIn2& values)
{
    auto pairs = zip(keys, values);
    return pairs_to_map<MapOut>(pairs);
}

// API search type: create_map_with : ((key -> val), [key]) -> Map key val
// fwd bind count: 1
// create_map_with(show, [1,2]) == {1: "1", 2: "2"}
template <typename ContainerIn,
    typename F,
    typename Key = typename std::remove_const<typename ContainerIn::value_type>::type,
    typename Val = typename std::result_of<F(Key)>::type,
    typename MapOut = std::map<Key, Val>>
MapOut create_map_with(F f, const ContainerIn& keys)
{
    return create_map(keys, transform(f, keys));
}

// API search type: create_unordered_map : ([key], [val]) -> Map key val
// fwd bind count: 1
// create_unordered_map([1,2,3], ["one", "two"]) == {1: "one", 2: "two"}
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

// API search type: create_unordered_map_with : ((key -> val), [key]) -> Map key val
// fwd bind count: 1
// create_unordered_map_with(show, [1,2]) == {1: "1", 2: "2"}
template <typename ContainerIn,
    typename F,
    typename Key = typename std::remove_const<typename ContainerIn::value_type>::type,
    typename Val = typename std::remove_reference<typename std::remove_const<
        typename std::result_of<F(Key)>::type>::type>::type,
    typename MapOut = std::unordered_map<Key, Val>>
MapOut create_unordered_map_with(F f, const ContainerIn& keys)
{
    return create_unordered_map(keys, transform(f, keys));
}

// API search type: get_from_map : (Map key val, key) -> Maybe val
// fwd bind count: 1
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

// API search type: get_from_map_unsafe : (Map key val, key) -> val
// fwd bind count: 1
// Returns the value of a key if key is present.
// Crashes otherwise.
template <typename MapType,
    typename Key = typename MapType::key_type,
    typename Val = typename MapType::mapped_type>
Val get_from_map_unsafe(const MapType& map, const Key& key)
{
    auto it = map.find(key);
    if (it == std::end(map))
    {
        assert(false); // Key not present in map.
    }
    return it->second;
}

// API search type: get_from_map_with_def : (Map key val, val, key) -> val
// fwd bind count: 2
// Returns the value of a key if key is present.
// Otherwise returns the provided default.
// Also known as prop_or.
template <typename MapType,
    typename Key = typename MapType::key_type,
    typename Val = typename MapType::mapped_type>
Val get_from_map_with_def(const MapType& map, const Val& defVal,
    const Key& key)
{
    return just_with_default(defVal, get_from_map(map, key));
}

// API search type: map_contains : (Map key val, key) -> Bool
// fwd bind count: 1
// Checks if a map contains a key.
template <typename MapType, typename Key = typename MapType::key_type>
bool map_contains(const MapType& map, const Key& key)
{
    auto it = map.find(key);
    return it != std::end(map);
}

// API search type: map_keep_if : ((key -> Bool), Map key val) -> Map key val
// fwd bind count: 1
// Filters the map by keys.
// map_keep_if(is_upper_case, {a: 1, b: 2, A: 3, C: 4}) == {A: 3, C: 4}
// Also known as pick_by.
template <typename MapType,
    typename Pred,
    typename Key = typename MapType::key_type,
    typename Val = typename MapType::mapped_type>
MapType map_keep_if(Pred pred, const MapType& map)
{
    MapType result;
    for (const auto& key_and_value : map)
    {
        if (pred(key_and_value.first))
        {
            result.insert(key_and_value);
        }
    }
    return result;
}

// API search type: map_drop_if : ((key -> Bool), Map key val) -> Map key val
// fwd bind count: 1
// Filters the map by keys.
// map_drop_if(is_lower_case, {a: 1, b: 2, A: 3, C: 4}) == {A: 3, C: 4}
// Inverse of map_keep_if.
template <typename MapType,
    typename Pred,
    typename Key = typename MapType::key_type,
    typename Val = typename MapType::mapped_type>
MapType map_drop_if(Pred pred, const MapType& map)
{
    return map_keep_if(logical_not(pred), map);
}

// API search type: map_keep : ([key], Map key val) -> Map key val
// fwd bind count: 1
// Keeps only the pairs of the map found in the key list.
// map_keep([a, d], {a: 1, b: 2, c: 3, d: 4}) == {a: 1, d: 4}
// map_keep([a, e, f], {a: 1, b: 2, c: 3, d: 4}) == {a: 1}
// Also known as pick.
template <typename MapType,
    typename KeyContainer,
    typename Key = typename MapType::key_type,
    typename Val = typename MapType::mapped_type>
MapType map_keep(const KeyContainer& keys, const MapType& map)
{
    static_assert(std::is_same<
        typename KeyContainer::value_type,
        typename MapType::key_type>::value,
        "Key types do not match.");
    return map_keep_if(bind_1st_of_2(contains<KeyContainer>, keys), map);
}

// API search type: map_drop : ([key], Map key val) -> Map key val
// fwd bind count: 1
// Keeps only the pairs of the map not found in the key list.
// Inverse of map_keep.
// map_drop([b, c], {a: 1, b: 2, c: 3, d: 4}); //=> {a: 1, d: 4}
template <typename MapType,
    typename KeyContainer,
    typename Key = typename MapType::key_type,
    typename Val = typename MapType::mapped_type>
MapType map_drop(const KeyContainer& keys, const MapType& map)
{
    static_assert(std::is_same<
        typename KeyContainer::value_type,
        typename MapType::key_type>::value,
        "Key types do not match.");
    return map_drop_if(bind_1st_of_2(contains<KeyContainer>, keys), map);
}

// API search type: map_pluck : (key, [Map key val]) -> [val]
// fwd bind count: 1
// Extracts values to a specific key from a list of maps.
// map_pluck('a', [{a: 1, b: 2}, {a: 3}, {c: 4}]) == [Just 1, Just 3, Nothing]
template <typename MapContainer,
    typename ContainerOut =
        std::vector<maybe<typename MapContainer::value_type::mapped_type>>,
    typename MapType = typename MapContainer::value_type,
    typename Key = typename MapType::key_type,
    typename Val = typename MapType::mapped_type>
ContainerOut map_pluck(const Key& key, const MapContainer& maps)
{
    return transform_convert<ContainerOut>(
        bind_2nd_of_2(get_from_map<MapType>, key), maps);
}

// API search type: choose : ([(a, b)], a) -> Maybe b
// Selects a value assigned to a key if the key exists exactly once.
// choose([(1,a), (2,b)], 2) == Just b;
// choose([(1,a), (1,b)], 2) == Nothing;
// choose([(1,a), (2,b)], 3) == Nothing;
template<typename Key, typename Val>
maybe<Val> choose(const std::vector<std::pair<Key, Val>>& pairs, const Key& x)
{
    if (count(x, transform(fst<Key, Val>, pairs)) != 1)
        return {};
    return get_from_map(pairs_to_map<std::unordered_map<Key, Val>>(pairs), x);
}

} // namespace fplus
