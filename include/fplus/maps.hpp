// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <fplus/composition.hpp>
#include <fplus/container_common.hpp>
#include <fplus/pairs.hpp>

#include <fplus/internal/invoke.hpp>

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
// Convert a list of key-value pairs to a dictionary
// while pushing values having the same key into a vector.
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
// Manipulate the values in a dictionary, keeping the key-value relationship.
// transform_map_values((*2), {0: 2, 1: 3}) == {0: 4, 1: 6}
template <typename F, typename MapIn>
auto transform_map_values(F f, const MapIn& map)
{
    using MapInPair = typename MapIn::value_type;
    using Key = std::remove_const_t<typename MapInPair::first_type>;
    using InVal = std::remove_const_t<typename MapInPair::second_type>;
    using OutVal = std::decay_t<internal::invoke_result_t<F, InVal>>;
    using MapOut = typename internal::SameMapTypeNewTypes<MapIn, Key, OutVal>::type;

    return pairs_to_map<MapOut>(
        transform(
            bind_1st_of_2(transform_snd<Key, InVal, F>, f),
            map_to_pairs(map)));
}

// API search type: map_union_with : (((val, val) -> val), Map key val, Map key val) -> Map key val
// fwd bind count: 2
// Combine two dictionaries using a binary function for the values.
// map_union_with((++), {0: a, 1: b}, {0: c, 2: d}) == {0: ac, 1: b, 2: d}
template <typename F, typename MapIn>
auto map_union_with(F f, const MapIn& dict1, const MapIn& dict2)
{
    const auto both = append(map_to_pairs(dict1), map_to_pairs(dict2));
    using Key = typename decltype(both)::value_type::first_type;
    using SingleValue = typename decltype(both)::value_type::second_type;
    auto full_map = pairs_to_map_grouped<decltype(both), Key, SingleValue,
            typename internal::SameMapTypeNewTypes<MapIn, Key, std::vector<SingleValue>>::type>(both);
    const auto group_f = [f](const auto& vals)
    {
        return fold_left_1(f, vals);
    };
    return transform_map_values(group_f, full_map);
}

// API search type: map_union : (Map key val, Map key val) -> Map key val
// fwd bind count: 1
// Combine two dictionaries keeping the value of the first map
// in case of key collissions.
// map_union({0: a, 1: b}, {0: c, 2: d}) == {0: a, 1: b, 2: d}
template <typename MapType>
MapType map_union(const MapType& dict1, const MapType& dict2)
{
    using Value = typename MapType::value_type::second_type;

    const auto get_first = [](const Value& a, const Value&) -> Value
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
// Zip a sequence of keys with a sequence of values to obtain a dictionary.
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
// Take a list of keys and create a dictionary
// generating the values by applying f to each key.
// create_map_with(show, [1,2]) == {1: "1", 2: "2"}
template <typename ContainerIn, typename F>
auto create_map_with(F f, const ContainerIn& keys)
{
    return create_map(keys, transform(f, keys));
}

// API search type: create_unordered_map : ([key], [val]) -> Map key val
// fwd bind count: 1
// Zip a sequence of keys with a sequence of values to obtain a dictionary.
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
// Take a list of keys and create a dictionary
// generating the values by applying f to each key.
// create_unordered_map_with(show, [1,2]) == {1: "1", 2: "2"}
template <typename ContainerIn, typename F>
auto create_unordered_map_with(F f, const ContainerIn& keys)
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
template <typename MapType, typename Pred>
MapType map_keep_if(Pred pred, const MapType& map)
{
    MapType result;
    for (const auto& key_and_value : map)
    {
        if (internal::invoke(pred, key_and_value.first))
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
template <typename MapType, typename Pred>
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
template <typename MapType, typename KeyContainer>
MapType map_keep(const KeyContainer& keys, const MapType& map)
{
    static_assert(std::is_same<
        typename KeyContainer::value_type,
        typename MapType::key_type>::value,
        "Key types do not match.");
    return map_keep_if(bind_2nd_of_2(is_elem_of<KeyContainer>, keys), map);
}

// API search type: map_drop : ([key], Map key val) -> Map key val
// fwd bind count: 1
// Keeps only the pairs of the map not found in the key list.
// Inverse of map_keep.
// map_drop([b, c], {a: 1, b: 2, c: 3, d: 4}); == {a: 1, d: 4}
template <typename MapType, typename KeyContainer>
MapType map_drop(const KeyContainer& keys, const MapType& map)
{
    static_assert(std::is_same<
        typename KeyContainer::value_type,
        typename MapType::key_type>::value,
        "Key types do not match.");
    return map_drop_if(bind_2nd_of_2(is_elem_of<KeyContainer>, keys), map);
}

// API search type: map_keep_if_value : ((val -> Bool), Map key val) -> Map key val
// fwd bind count: 1
// Filters the map by values.
// map_keep_if_value(is_upper_case, {1: a, 2: b, 3: A, 4: C}) == {3: A, 4: C}
// Also known as filter_values.
template <typename MapType, typename Pred>
MapType map_keep_if_value(Pred pred, const MapType& map)
{
    MapType result;
    for (const auto& key_and_value : map)
    {
        if (internal::invoke(pred, key_and_value.second))
        {
            result.insert(key_and_value);
        }
    }
    return result;
}

// API search type: map_drop_if_value : ((value -> Bool), Map key val) -> Map key val
// fwd bind count: 1
// Filters the map by values.
// map_drop_if_value(is_lower_case, {1: a, 2: b, 3: A, 4: C}) == {3: A, 4: C}
// Inverse of map_keep_if_value.
template <typename MapType, typename Pred>
MapType map_drop_if_value(Pred pred, const MapType& map)
{
    return map_keep_if_value(logical_not(pred), map);
}

// API search type: map_keep_values : ([value], Map key val) -> Map key val
// fwd bind count: 1
// Keeps only the pairs of the map found in the value list.
// map_keep_values([1, 4], {a: 1, b: 2, c: 3, d: 4}) == {a: 1, d: 4}
// map_keep_values([1, 5, 6], {a: 1, b: 2, c: 3, d: 4}) == {a: 1}
template <typename MapType, typename ValueContainer>
MapType map_keep_values(const ValueContainer& values, const MapType& map)
{
    static_assert(std::is_same<
        typename ValueContainer::value_type,
        typename MapType::mapped_type>::value,
        "Value types do not match.");
    return map_keep_if_value(
        bind_2nd_of_2(is_elem_of<ValueContainer>, values), map);
}

// API search type: map_drop_values : ([value], Map key val) -> Map key val
// fwd bind count: 1
// Keeps only the pairs of the map not found in the value list.
// Inverse of map_keep_values.
// map_drop_values([2, 3], {a: 1, b: 2, c: 3, d: 4}) == {a: 1, d: 4}
template <typename MapType, typename ValueContainer>
MapType map_drop_values(const ValueContainer& values, const MapType& map)
{
    static_assert(std::is_same<
        typename ValueContainer::value_type,
        typename MapType::mapped_type>::value,
        "Value types do not match.");
    return map_drop_if_value(
        bind_2nd_of_2(is_elem_of<ValueContainer>, values), map);
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
// fwd bind count: 1
// Selects a value assigned to a key iff the key exists exactly once.
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

// API search type: choose_by : ([((a -> Bool), b)], a) -> Maybe b
// fwd bind count: 2
// Iff exactly one predicate is fulfilled
// the value assigned to this predicate is selected.
// choose_by([(is_even,a), (is_bigger_than_3,b)], 2) == Just a;
// choose_by([(is_even,a), (is_bigger_than_3,b)], 5) == Just b;
// choose_by([(is_even,a), (is_bigger_than_3,b)], 1) == Nothing;
// choose_by([(is_even,a), (is_bigger_than_3,b)], 4) == Nothing;
template<typename Key, typename Val>
maybe<Val> choose_by(
    const std::vector<std::pair<std::function<bool(const Key&)>, Val>>& pairs,
    const Key& x)
{
    maybe<Val> result;
    for (const auto& p : pairs)
    {
        if (internal::invoke(p.first, x))
        {
            if (is_just(result))
            {
                return nothing<Val>();
            }
            result = p.second;
        }
    }
    return result;
}

// API search type: choose_lazy : ([(a, (() -> b))], a) -> Maybe b
// fwd bind count: 1
// Evaluates a lazy value assigned to a key iff the key exists exactly once.
// choose_lazy([(1,a), (2,b)], 2) == Just b();
// choose_lazy([(1,a), (1,b)], 2) == Nothing;
// choose_lazy([(1,a), (2,b)], 3) == Nothing;
template <typename Key, typename ValStub>
auto choose_lazy(const std::vector<std::pair<Key, ValStub>>& pairs,
                 const Key& x)
{
    using Ret = maybe<std::decay_t<internal::invoke_result_t<ValStub>>>;
    const auto res = choose(pairs, x);
    if (res.is_nothing())
        return Ret{};
    else
        return Ret{res.unsafe_get_just()()};
}

// API search type: choose_by_lazy : ([((a -> Bool), (() -> b))], a) -> Maybe b
// fwd bind count: 2
// Iff exactly one predicate is fulfilled
// the lazy value assigned to this predicate is evaluated.
// choose_by_lazy([(is_even,a), (is_bigger_than_3,b)], 2) == Just a();
// choose_by_lazy([(is_even,a), (is_bigger_than_3,b)], 5) == Just b();
// choose_by_lazy([(is_even,a), (is_bigger_than_3,b)], 1) == Nothing;
// choose_by_lazy([(is_even,a), (is_bigger_than_3,b)], 4) == Nothing;
template <typename Key, typename ValStub>
auto choose_by_lazy(
    const std::vector<std::pair<std::function<bool(const Key&)>, ValStub>>& pairs,
    const Key& x)
{
    using Ret = maybe<std::decay_t<internal::invoke_result_t<ValStub>>>;

    const auto res = choose_by(pairs, x);
    if (res.is_nothing())
        return Ret{};
    else
        return Ret{res.unsafe_get_just()()};
}

// API search type: choose_def : (b, [(a, b)], a) -> b
// fwd bind count: 1
// Selects a value assigned to a key iff the key exists exactly once,
// otherwise returns the given default value.
// choose_def(c, [(1,a), (2,b)], 2) == b;
// choose_def(c, [(1,a), (1,b)], 2) == c;
// choose_def(c, [(1,a), (2,b)], 3) == c;
template<typename Key, typename Val>
Val choose_def(const Val& def,
    const std::vector<std::pair<Key, Val>>& pairs, const Key& x)
{
    if (count(x, transform(fst<Key, Val>, pairs)) != 1)
        return def;
    return get_from_map_with_def(
        pairs_to_map<std::unordered_map<Key, Val>>(pairs), def, x);
}

// API search type: choose_by_def : (b, [((a -> Bool), b)], a) -> b
// fwd bind count: 2
// Iff exactly one predicate is fulfilled
// the value assigned to this predicate is selected.
// Otherwise the given default value is returned.
// choose_by_def(c, [(is_even,a), (is_bigger_than_3,b)], 2) == Just a;
// choose_by_def(c, [(is_even,a), (is_bigger_than_3,b)], 5) == Just b;
// choose_by_def(c, [(is_even,a), (is_bigger_than_3,b)], 1) == c;
// choose_by_def(c, [(is_even,a), (is_bigger_than_3,b)], 4) == c;
template<typename Key, typename Val>
Val choose_by_def(const Val& def,
    const std::vector<std::pair<std::function<bool(const Key&)>, Val>>& pairs,
    const Key& x)
{
    return just_with_default(def, choose_by(pairs, x));
}

// API search type: choose_def_lazy : ((() -> b), [(a, (() -> b))], a) -> b
// fwd bind count: 1
// Evaluates a lazy value assigned to a key iff the key exists exactly once,
// otherwise evaluates the given default lazy value.
// choose_def_lazy(c, [(1,a), (2,b)], 2) == b();
// choose_def_lazy(c, [(1,a), (1,b)], 2) == c();
// choose_def_lazy(c, [(1,a), (2,b)], 3) == c();
template <typename Key, typename ValStub>
auto choose_def_lazy(const ValStub& def,
                     const std::vector<std::pair<Key, ValStub>>& pairs,
                     const Key& x)
{
    return choose_def(def, pairs, x)();
}

// API search type: choose_by_def_lazy : ((() -> b), [((a -> Bool), (() -> b))], a) -> b
// fwd bind count: 2
// Iff exactly one predicate is fulfilled
// the value assigned to this predicate is evaluated.
// Otherwise the given default value is evaluated and returned.
// choose_by_def_lazy(c, [(is_even,a), (is_bigger_than_3,b)], 2) == Just a();
// choose_by_def_lazy(c, [(is_even,a), (is_bigger_than_3,b)], 5) == Just b();
// choose_by_def_lazy(c, [(is_even,a), (is_bigger_than_3,b)], 1) == c();
// choose_by_def_lazy(c, [(is_even,a), (is_bigger_than_3,b)], 4) == c();
template <typename Key, typename ValStub>
auto choose_by_def_lazy(
    const ValStub& def,
    const std::vector<std::pair<std::function<bool(const Key&)>, ValStub>>& pairs,
    const Key& x)
{
    return choose_by_def(def, pairs, x)();
}

} // namespace fplus
