// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <doctest/doctest.h>
#include <fplus/fplus.hpp>
#include <vector>

namespace {

    auto is_even_int = [](int x){ return x % 2 == 0; };
    auto is_bigger_than_3_int = [](int x){ return x > 3; };
    typedef std::vector<int> IntVector;
}

TEST_CASE("maps_test - transform_map_values")
{
    using namespace fplus;
    typedef std::vector<std::pair<std::string, int>> StringIntPairs;
    StringIntPairs stringIntPairs = {{"a", 1}, {"a", 2}, {"b", 6}, {"a", 4}};
    auto stringIntPairsAsMapGrouped = pairs_to_map_grouped(stringIntPairs);
    auto groupNameToMedianMap = transform_map_values(median<std::vector<int>>, stringIntPairsAsMapGrouped);
    typedef std::map<std::string, int> StringIntMap;
    REQUIRE_EQ(groupNameToMedianMap, StringIntMap({{"a", 2}, {"b", 6}}));
}

TEST_CASE("maps_test - choose")
{
    using namespace fplus;
    REQUIRE_EQ((choose<int, char>({{1, 'a'}, {2, 'b'}}, 2)), just('b'));
    REQUIRE_EQ((choose<int, char>({{1, 'a'}, {1, 'b'}}, 1)), nothing<char>());
    REQUIRE_EQ((choose<int, char>({{1, 'a'}, {2, 'b'}}, 3)), nothing<char>());
    REQUIRE_EQ((choose<int, char>({}, 2)), nothing<char>());
}

TEST_CASE("maps_test - choose_by")
{
    using namespace fplus;
    REQUIRE_EQ((choose_by<int, char>({{is_even_int, 'a'}, {is_bigger_than_3_int, 'b'}}, 2)), just('a'));
    REQUIRE_EQ((choose_by<int, char>({{is_even_int, 'a'}, {is_bigger_than_3_int, 'b'}}, 5)), just('b'));
    REQUIRE_EQ((choose_by<int, char>({{is_even_int, 'a'}, {is_bigger_than_3_int, 'b'}}, 1)), nothing<char>());
    REQUIRE_EQ((choose_by<int, char>({{is_even_int, 'a'}, {is_bigger_than_3_int, 'b'}}, 4)), nothing<char>());
    REQUIRE_EQ((choose_by<int, char>({}, 2)), nothing<char>());
}

TEST_CASE("maps_test - choose_lazy")
{
    using namespace fplus;
    typedef std::function<char()> char_stub;
    const char_stub a_stub = []() -> char { return 'a'; };
    const char_stub b_stub = []() -> char { return 'b'; };
    REQUIRE_EQ((choose_lazy<int, char_stub>({{1, a_stub}, {2, b_stub}}, 2)), just('b'));
    REQUIRE_EQ((choose_lazy<int, char_stub>({{1, a_stub}, {1, b_stub}}, 1)), nothing<char>());
    REQUIRE_EQ((choose_lazy<int, char_stub>({{1, a_stub}, {2, b_stub}}, 3)), nothing<char>());
    REQUIRE_EQ((choose_lazy<int, char_stub>({}, 2)), nothing<char>());
}

TEST_CASE("maps_test - choose_by_lazy")
{
    using namespace fplus;
    typedef std::function<char()> char_stub;
    const char_stub a_stub = []() -> char { return 'a'; };
    const char_stub b_stub = []() -> char { return 'b'; };
    REQUIRE_EQ((choose_by_lazy<int, char_stub>({{is_even_int, a_stub}, {is_bigger_than_3_int, b_stub}}, 2)), just('a'));
    REQUIRE_EQ((choose_by_lazy<int, char_stub>({{is_even_int, a_stub}, {is_bigger_than_3_int, b_stub}}, 5)), just('b'));
    REQUIRE_EQ((choose_by_lazy<int, char_stub>({{is_even_int, a_stub}, {is_bigger_than_3_int, b_stub}}, 1)), nothing<char>());
    REQUIRE_EQ((choose_by_lazy<int, char_stub>({{is_even_int, a_stub}, {is_bigger_than_3_int, b_stub}}, 4)), nothing<char>());
    REQUIRE_EQ((choose_by_lazy<int, char_stub>({}, 2)), nothing<char>());
}

TEST_CASE("maps_test - choose_def")
{
    using namespace fplus;
    REQUIRE_EQ((choose_def<int>('c', {{1, 'a'}, {2, 'b'}}, 2)), 'b');
    REQUIRE_EQ((choose_def<int>('c', {{1, 'a'}, {1, 'b'}}, 1)), 'c');
    REQUIRE_EQ((choose_def<int>('c', {{1, 'a'}, {2, 'b'}}, 3)), 'c');
    REQUIRE_EQ((choose_def<int>('c', {}, 2)), 'c');
}

TEST_CASE("maps_test - choose_by_def")
{
    using namespace fplus;
    REQUIRE_EQ((choose_by_def<int, char>('c', {{is_even_int, 'a'}, {is_bigger_than_3_int, 'b'}}, 2)), 'a');
    REQUIRE_EQ((choose_by_def<int, char>('c', {{is_even_int, 'a'}, {is_bigger_than_3_int, 'b'}}, 5)), 'b');
    REQUIRE_EQ((choose_by_def<int, char>('c', {{is_even_int, 'a'}, {is_bigger_than_3_int, 'b'}}, 1)), 'c');
    REQUIRE_EQ((choose_by_def<int, char>('c', {{is_even_int, 'a'}, {is_bigger_than_3_int, 'b'}}, 4)), 'c');
    REQUIRE_EQ((choose_by_def<int, char>('c', {}, 2)), 'c');
}

TEST_CASE("maps_test - choose_def_lazy")
{
    using namespace fplus;
    typedef std::function<char()> char_stub;
    const char_stub a_stub = []() -> char { return 'a'; };
    const char_stub b_stub = []() -> char { return 'b'; };
    const char_stub c_stub = []() -> char { return 'c'; };
    REQUIRE_EQ((choose_def_lazy<int>(c_stub, {{1, a_stub}, {2, b_stub}}, 2)), 'b');
    REQUIRE_EQ((choose_def_lazy<int>(c_stub, {{1, a_stub}, {1, b_stub}}, 1)), 'c');
    REQUIRE_EQ((choose_def_lazy<int>(c_stub, {{1, a_stub}, {2, b_stub}}, 3)), 'c');
    REQUIRE_EQ((choose_def_lazy<int>(c_stub, {}, 2)), 'c');
}

TEST_CASE("maps_test - choose_by_def_lazy")
{
    using namespace fplus;
    typedef std::function<char()> char_stub;
    const char_stub a_stub = []() -> char { return 'a'; };
    const char_stub b_stub = []() -> char { return 'b'; };
    const char_stub c_stub = []() -> char { return 'c'; };
    REQUIRE_EQ((choose_by_def_lazy<int, char_stub>(c_stub, {{is_even_int, a_stub}, {is_bigger_than_3_int, b_stub}}, 2)), 'a');
    REQUIRE_EQ((choose_by_def_lazy<int, char_stub>(c_stub, {{is_even_int, a_stub}, {is_bigger_than_3_int, b_stub}}, 5)), 'b');
    REQUIRE_EQ((choose_by_def_lazy<int, char_stub>(c_stub, {{is_even_int, a_stub}, {is_bigger_than_3_int, b_stub}}, 1)), 'c');
    REQUIRE_EQ((choose_by_def_lazy<int, char_stub>(c_stub, {{is_even_int, a_stub}, {is_bigger_than_3_int, b_stub}}, 4)), 'c');
    REQUIRE_EQ((choose_by_def_lazy<int, char_stub>(c_stub, {}, 2)), 'c');
}

TEST_CASE("maps_test - map functions")
{
    using namespace fplus;
    typedef std::map<int, std::string> IntStringMap;
    typedef std::map<std::string, int> StringIntMap;
    IntStringMap intStringMap = {{1, "2"}, {4, "53"}, {7, "21"}};
    StringIntMap stringIntMap = {{ "2", 1}, { "53", 4}, { "21", 7}};
    REQUIRE_EQ(swap_keys_and_values(intStringMap), stringIntMap);

    typedef std::vector<std::string> StringVector;
    REQUIRE_EQ(get_map_keys(intStringMap), IntVector({1, 4, 7}));
    REQUIRE_EQ(get_map_values(intStringMap), StringVector({"2", "53", "21"}));

    typedef std::unordered_map<int, std::string> IntStringUnorderedMap;
    typedef std::unordered_map<std::string, int> StringIntUnorderedMap;
    IntStringUnorderedMap intStringUnorderedMap = { { 1, "2" },{ 4, "53" },{ 7, "21" } };
    StringIntUnorderedMap stringIntUnorderedMapSwapped = { { "2", 1 },{ "53", 4 },{ "21", 7 } };
    REQUIRE_EQ(swap_keys_and_values(intStringUnorderedMap), stringIntUnorderedMapSwapped);
    REQUIRE_EQ(convert_container<IntStringUnorderedMap>(intStringMap), intStringUnorderedMap);
    REQUIRE_EQ(convert_container<IntStringMap>(intStringUnorderedMap), intStringMap);

    std::vector<int> mapInts = { 1, 4, 7 };
    std::vector<std::string> mapStrings = { "2", "53", "21" };
    REQUIRE_EQ(create_map(mapInts, mapStrings), intStringMap);
    REQUIRE_EQ(create_unordered_map(mapInts, mapStrings), intStringUnorderedMap);

    IntStringMap intsAsStringsMap = {{1, "1"}, {4, "4"}, {7, "7"}};
    REQUIRE_EQ(create_map_with(show<int>, mapInts), intsAsStringsMap);
    IntStringUnorderedMap intsAsStringsUnorderedMap = {{1, "1"}, {4, "4"}, {7, "7"}};
    REQUIRE_EQ(create_unordered_map_with(show<int>, mapInts), intsAsStringsUnorderedMap);

    const auto is_int_string_map_key_even =
        [&](const IntStringMap::value_type& p) -> bool
    {
        return is_even_int(p.first);
    };
    REQUIRE_EQ(keep_if(is_int_string_map_key_even, IntStringMap({{4, "4"}, {7, "7"}})), IntStringMap({{4, "4"}}));

    REQUIRE_EQ(get_from_map(intStringMap, 1), just<std::string>("2"));
    REQUIRE_EQ(get_from_map(intStringMap, 9), nothing<std::string>());
    REQUIRE_EQ(get_from_map_with_def(intStringMap, std::string("n/a"), 1), "2");
    REQUIRE_EQ(get_from_map_with_def(intStringMap, std::string("n/a"), 9), "n/a");
    REQUIRE_EQ(map_contains(intStringMap, 1), true);
    REQUIRE_EQ(map_contains(intStringMap, 9), false);

    REQUIRE_EQ(get_first_from_map(intStringMap, IntVector({4, 1})), just<std::string>("53"));
    REQUIRE_EQ(get_first_from_map(intStringMap, IntVector({5, 1})), just<std::string>("2"));
    REQUIRE_EQ(get_first_from_map(intStringMap, IntVector({5, 2})), nothing<std::string>());
    REQUIRE_EQ(get_first_from_map_with_def(intStringMap, std::string("n/a"), IntVector({4, 1})), "53");
    REQUIRE_EQ(get_first_from_map_with_def(intStringMap, std::string("n/a"), IntVector({5, 1})), "2");
    REQUIRE_EQ(get_first_from_map_with_def(intStringMap, std::string("n/a"), IntVector({5, 2})), "n/a");

    IntStringMap union_map_1 = {{0, "a"}, {1, "b"}};
    IntStringMap union_map_2 = {{0, "c"}, {2, "d"}};
    IntStringMap union_map_res = {{0, "a"}, {1, "b"}, {2, "d"}};
    IntStringMap union_map_with_res = {{0, "ac"}, {1, "b"}, {2, "d"}};
    REQUIRE_EQ(map_union(union_map_1, union_map_2), union_map_res);
    REQUIRE_EQ(map_union_with(append<std::string>, union_map_1, union_map_2), union_map_with_res);

    IntStringUnorderedMap union_umap_1 = {{0, "a"}, {1, "b"}};
    IntStringUnorderedMap union_umap_2 = {{0, "c"}, {2, "d"}};
    IntStringUnorderedMap union_umap_res = {{0, "a"}, {1, "b"}, {2, "d"}};
    IntStringUnorderedMap union_umap_with_res = {{0, "ac"}, {1, "b"}, {2, "d"}};
    REQUIRE_EQ(map_union(union_umap_1, union_umap_2), union_umap_res);
    REQUIRE_EQ(map_union_with(append<std::string>, union_umap_1, union_umap_2), union_umap_with_res);

    typedef std::map<std::string::value_type, int> CharIntMap;
    typedef std::map<int, std::string::value_type> IntCharMap;
    CharIntMap charIntMap = {{'a', 1}, {'b', 2}, {'A', 3}, {'C', 4}};
    IntCharMap intCharMap = {{1, 'a'}, {2, 'b'}, {3, 'A'}, {4, 'C'}};
    const auto is_upper = [](std::string::value_type c) -> bool
    {
        return (std::isupper(c) != 0);
    };
    const auto is_lower = [](std::string::value_type c) -> bool
    {
        return (std::islower(c) != 0);
    };
    REQUIRE_EQ(map_keep_if(is_upper, charIntMap), CharIntMap({{'A', 3}, {'C', 4}}));
    REQUIRE_EQ(map_drop_if(is_lower, charIntMap), CharIntMap({{'A', 3}, {'C', 4}}));
    typedef std::vector<std::string::value_type> CharVector;
    REQUIRE_EQ(map_keep(CharVector({'b', 'F'}), charIntMap), CharIntMap({{'b', 2}}));
    REQUIRE_EQ(map_drop(CharVector({'a', 'A', 'C', 'F'}), charIntMap), CharIntMap({{'b', 2}}));

    REQUIRE_EQ(map_keep_if_value(is_upper, intCharMap), IntCharMap({{3, 'A'}, {4, 'C'}}));
    REQUIRE_EQ(map_drop_if_value(is_lower, intCharMap), IntCharMap({{3, 'A'}, {4, 'C'}}));
    REQUIRE_EQ(map_keep_values(CharVector({'b', 'F'}), intCharMap), IntCharMap({{2, 'b'}}));
    REQUIRE_EQ(map_drop_values(CharVector({'a', 'A', 'C', 'F'}), intCharMap), IntCharMap({{2, 'b'}}));

    typedef std::vector<CharIntMap> CharIntMaps;
    typedef std::vector<maybe<int>> MaybeInts;
    REQUIRE_EQ(
        map_pluck('a', CharIntMaps({{{'a',1}, {'b',2}}, {{'a',3}}, {{'c',4}}})),
        MaybeInts({1, 3, {}}));
}

TEST_CASE("maps_test - map_grouped_to_pairs")
{
    using namespace fplus;
    const std::map<std::string, std::vector<std::string>> teams = {
            {"teamX", {"Ron", "Alice"}},
            {"teamY", {"Joe", "Jane", "Ally"}}
    };
    using Pair = std::pair<std::string, std::string>;
    const auto result = map_grouped_to_pairs(teams);
    REQUIRE_EQ(result, std::vector<Pair>{{"teamX", "Ron"}, {"teamX", "Alice"},
                                         {"teamY", "Joe"}, {"teamY", "Jane"}, {"teamY", "Ally"}});
    REQUIRE_EQ(teams, pairs_to_map_grouped(result));
}

TEST_CASE("maps_test - create_map_grouped")
{
    using namespace fplus;
    const std::vector<std::string> words = {"one", "three", "two"};
    const auto result = create_map_grouped(size_of_cont<std::string>, words);
    REQUIRE_EQ(result.at(3), std::vector<std::string>{"one", "two"});
}

TEST_CASE("maps_test - create_unordered_map_grouped")
{
    using namespace fplus;
    const std::vector<std::string> words = {"one", "three", "two"};
    const auto result = create_unordered_map_grouped(size_of_cont<std::string>, words);
    REQUIRE_EQ(result.at(3), std::vector<std::string>{"one", "two"});
}
