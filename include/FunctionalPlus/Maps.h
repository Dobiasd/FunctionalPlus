// Copyright Tobias Hermann 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "Composition.h"
#include "ContainerCommon.h"
#include "Transform.h"
#include "Pairs.h"

#include <map>
#include <unordered_map>

namespace FunctionalPlus
{

// Converts a Container of pairs (key, value) into a dictionary.
template <typename MapOut, typename ContainerIn>
MapOut PairsToMap(const ContainerIn& pairs)
{
    return Convert<MapOut>(pairs);
}

// Converts a dictionary into a Container of pairs (key, value).
template <typename MapType,
    typename MapPair = typename MapType::value_type,
    typename Key = typename std::remove_const_t<typename MapPair::first_type>,
    typename Val = typename std::remove_const_t<typename MapPair::second_type>,
    typename OutPair = std::pair<Key, Val>,
    typename ContainerOut = std::vector<OutPair>>
ContainerOut MapToPairs(const MapType& dict)
{
    return Convert<ContainerOut>(dict);
}

template <typename MapType,
    typename ContainerOut = std::vector<std::remove_const_t<typename MapType::key_type>>>
ContainerOut GetMapKeys(const MapType& dict)
{
    auto pairs = MapToPairs(dict);
    typedef typename decltype(pairs)::value_type::first_type FirstType;
    typedef typename decltype(pairs)::value_type::second_type SecondType;
    return Transform(Fst<FirstType, SecondType>, MapToPairs(dict));
}

template <typename MapType,
    typename ContainerOut = std::vector<std::remove_const_t<typename MapType::mapped_type>>>
ContainerOut GetMapValues(const MapType& dict)
{
    auto pairs = MapToPairs(dict);
    typedef typename decltype(pairs)::value_type::first_type FirstType;
    typedef typename decltype(pairs)::value_type::second_type SecondType;
    return Transform(Snd<FirstType, SecondType>, MapToPairs(dict));
}

// Swaps keys and Values of a dict:
// SwapKeysAndValues({(1, "a"), (2, "b")}) == {("a", 1), ("b", 2)}
template <typename MapIn,
    typename MapInPair = typename MapIn::value_type,
    typename InKey = typename MapInPair::first_type,
    typename InVal = typename MapInPair::second_type,
    typename OutKey = InVal,
    typename OutVal = std::remove_const_t<InKey>,
    typename MapOut = typename SameMapTypeNewTypes<MapIn, OutKey, OutVal>::type>
MapOut SwapKeysAndValues(const MapIn& dict)
{
    auto inAsPairs = MapToPairs(dict);
    auto outAsPairs = Transform(SwapPairElems<InKey, InVal>, inAsPairs);
    return PairsToMap<MapOut>(outAsPairs);
}

// CreateMap([1,2,3], ["one", "two"]) == { {1,"one"}, {2,"two"} }
template <typename ContainerIn1, typename ContainerIn2,
    typename Key = std::remove_const_t<typename ContainerIn1::value_type>,
    typename Val = std::remove_const_t<typename ContainerIn2::value_type>,
    typename MapOut = std::map<Key, Val>>
MapOut CreateMap(const ContainerIn1& keys, const ContainerIn2& values)
{
    auto pairs = Zip(keys, values);
    return PairsToMap<MapOut>(pairs);
}

// CreateMapUnorderedMap([1,2,3], ["one", "two"]) == { {1,"one"}, {2,"two"} }
template <typename ContainerIn1, typename ContainerIn2,
    typename Key = std::remove_const_t<typename ContainerIn1::value_type>,
    typename Val = std::remove_const_t<typename ContainerIn2::value_type>,
    typename MapOut = std::unordered_map<Key, Val >>
MapOut CreateUnorderedMap(
    const ContainerIn1& keys,
    const ContainerIn2& values)
{
    auto pairs = Zip(keys, values);
    return PairsToMap<MapOut>(pairs);
}

// Returns just the value of a key if key is present.
// Otherwise returns nothing.
template <typename MapType,
    typename Key = typename MapType::key_type,
    typename Val = typename MapType::mapped_type>
Maybe<Val> GetFromMap(const MapType& map, const Key& key)
{
    auto it = map.find(key);
    if (it == std::end(map))
        return Nothing<Val>();
    return Just(it->second);
}

// Returns the value of a key if key is present.
// Otherwise returns the provided default.
template <typename MapType,
    typename Key = typename MapType::key_type,
    typename Val = typename MapType::mapped_type>
Val GetFromMapWithDef(const MapType& map, const Val& defVal, const Key& key)
{
    return WithDefault(defVal, GetFromMap(map, key));
}

// Checks if a map contains a key.
template <typename MapType, typename Key = typename MapType::key_type>
bool MapContains(const MapType& map, const Key& key)
{
    auto it = map.find(key);
    return it != std::end(map);
}

} // namespace FunctionalPlus