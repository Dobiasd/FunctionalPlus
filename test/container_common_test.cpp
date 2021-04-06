// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <doctest/doctest.h>
#include <fplus/fplus.hpp>
#include <vector>
#include <deque>

namespace {
    auto squareLambda = [](int x) -> int { return x*x; };
    auto is_even_int = [](int x){ return x % 2 == 0; };
    auto is_even_size_t = [](std::size_t x){ return x % 2 == 0; };
    auto is_odd_int = [](int x){ return x % 2 != 0; };
    typedef std::pair<int, int> IntPair;
    typedef std::vector<IntPair> IntPairs;
    typedef std::vector<int> IntVector;
    typedef std::vector<IntVector> IntVectors;
    typedef std::vector<bool> BoolVector;
    typedef std::vector<std::size_t> IdxVector;
    typedef std::array<int, 5> IntArray5;
    typedef std::deque<int> IntDeque;
    IntVector xs = {1,2,2,3,2};
    IntArray5 xs_arr = {{1,2,2,3,2}};
    IntDeque  xs_deque = {1,2,2,3,2};
    IntVector xs_reverse = {2,3,2,2,1};
    IntVector xsSorted = {1,2,2,2,3};
    IntVector xs2Times = {1,2,2,3,2,1,2,2,3,2};
    typedef std::list<int> IntList;
    typedef std::vector<IntList> IntLists;
    IntLists intLists = { { 1 },{ 2, 2 },{ 3 },{ 2 } };
    IntList intList = { 1,2,2,3,2 };
    IntList intListSorted = { 1,2,2,2,3 };
    auto int_mod_10 = [](int x) -> int { return x % 10; };
    auto is2 = fplus::is_equal_to(2);
    auto is3 = fplus::is_equal_to(3);
    auto is4 = fplus::is_equal_to(4);
    auto abs_diff_less_or_equal_3 = [](int a, int b) { return fplus::abs(b - a) <= 3; };
    typedef std::vector<std::string> string_vec;
    IntVector vec0123({0,1,2,3});
    IntList list0123({0,1,2,3});
    std::string ABC_("ABC");
    std::string XY("XY");
    std::string ABCD("ABCD");

    struct ExplicitFromIntStruct
    {
        explicit ExplicitFromIntStruct(int x) : x_(x) {}
        int x_;
    };
    bool operator == (const ExplicitFromIntStruct &lhs, const ExplicitFromIntStruct & rhs) { return lhs.x_ == rhs.x_; }
}

TEST_CASE("container_common_test - group")
{
    using namespace fplus;
    typedef std::vector<std::pair<int, std::vector<int>>> LabeledGroups;
    REQUIRE_EQ(group(intList), intLists);
    REQUIRE_EQ(group(xs), IntVectors({IntVector({1}),IntVector({2,2}),IntVector({3}),IntVector({2})}));
    REQUIRE_EQ(group_on([](auto x) { return x % 10; }, IntVector({12,22,34})), IntVectors({IntVector({12,22}),IntVector({34})}));
    REQUIRE_EQ(group_on_labeled(int_mod_10, IntVector({12,22,34})), LabeledGroups({{2, IntVector({12,22})}, {4, IntVector({34})}}));
    REQUIRE_EQ(group_on_labeled(int_mod_10, IntVector({12,22,34})), LabeledGroups({{2, IntVector({12,22})}, {4, IntVector({34})}}));
    REQUIRE_EQ(group_globally(xs), IntVectors({IntVector({1}),IntVector({2,2,2}),IntVector({3})}));
    REQUIRE_EQ(group_globally_on(int_mod_10, IntVector({12,34,22})), IntVectors({IntVector({12,22}),IntVector({34})}));
    REQUIRE_EQ(group_globally_on_labeled(int_mod_10, IntVector({12,34,22})), LabeledGroups({{2, IntVector({12,22})}, {4, IntVector({34})}}));
    REQUIRE_EQ(group_by(abs_diff_less_or_equal_3, IntVector({2,3,6,4,22,21,8,5})), IntVectors({{2,3,6,4},{22,21},{8,5}}));
}

TEST_CASE("container_common_test - separate")
{
    using namespace fplus;
    IntVector values = {1, 2, 2, 3, 3, 4, 4, 4};
    REQUIRE_EQ(separate(values), IntVectors({IntVector({1, 2, 3, 4}),IntVector({2, 3, 4}),IntVector({4})}));
    REQUIRE_EQ(separate_on(int_mod_10, IntVector({12,22,34})), IntVectors({IntVector({12,34}),IntVector({22})}));
}

TEST_CASE("container_common_test - singleton_seq")
{
    using namespace fplus;
    REQUIRE_EQ(singleton_seq(3), IntVector({3}));
}

TEST_CASE("container_common_test - filter")
{
    using namespace fplus;
    REQUIRE_EQ(keep_if(is_even_int, xs), IntVector({2,2,2}));
    REQUIRE_EQ(drop_if(is_even_int, xs), IntVector({1,3}));
    REQUIRE_EQ(keep_if(is_even_int, intList), IntList({ 2,2,2 }));
    REQUIRE_EQ(drop_if(is_even_int, intList), IntList({ 1,3 }));
    REQUIRE_EQ(without(2, intList), IntList({ 1,3 }));
    REQUIRE_EQ(without_any(IntVector({2,3}), intList), IntList({ 1 }));
    auto sumis_even = [&](std::size_t x, int y)
    {
        return is_even_int(static_cast<int>(x) + y);
    };
    REQUIRE_EQ(keep_by_idx(is_even_size_t, xs), IntVector({ 1,2,2 }));
    REQUIRE_EQ(keep_if_with_idx(sumis_even, xs), IntVector({ 2,3,2 }));
    REQUIRE_EQ(drop_if_with_idx(sumis_even, xs), IntVector({ 1,2 }));
}

TEST_CASE("container_common_test - trim")
{
    using namespace fplus;
    REQUIRE_EQ(trim_left(1, intList), IntList({2,2,3,2}));
    REQUIRE_EQ(trim_right(2, intList), IntList({1,2,2,3}));
    REQUIRE_EQ(trim(0, IntVector({0,2,4,5,6,7,8,0,0})), IntVector({2,4,5,6,7,8}));
    REQUIRE_EQ(trim_token_left(IntVector({1,2}), xs), IntVector({2,3,2}));
    REQUIRE_EQ(trim_token_left(IntVector({0,1,2}), IntVector({0,1,2,0,1,2,7,5,9})), IntVector({7,5,9}));
    REQUIRE_EQ(trim_token_right(IntVector({3,2}), xs), IntVector({1,2,2}));
    REQUIRE_EQ(trim_token(IntVector({0,1}), IntVector({0,1,7,8,9,0,1})), IntVector({7,8,9}));
}

TEST_CASE("container_common_test - cluster")
{
    using namespace fplus;
    REQUIRE_EQ(cluster_by(abs_diff_less_or_equal_3, IntVector({2,3,6,4,12,11,20,23,8,4})), IntVectors({{2,3,6,4,12,11,8,4},{20,23}}));
}

TEST_CASE("container_common_test - run_length_encode")
{
    using namespace fplus;
    typedef std::pair<std::size_t, int> rle_pair_int;
    typedef std::vector<rle_pair_int> rle_list_int;
    IntVector rle_input = {1,2,2,2,2,3,3,2};
    rle_list_int rle_result = {
            std::make_pair(1, 1),
            std::make_pair(4, 2),
            std::make_pair(2, 3),
            std::make_pair(1, 2)};
    REQUIRE_EQ(run_length_encode(rle_input), rle_result);
    REQUIRE_EQ(run_length_decode(rle_result), rle_input);
}

TEST_CASE("container_common_test - keep_idxs")
{
    using namespace fplus;
    REQUIRE_EQ(keep_idxs(IdxVector({1, 3}), xs), IntVector({2,3}));
    REQUIRE_EQ(keep_idxs(IdxVector({3, 1}), xs), IntVector({2,3}));
    REQUIRE_EQ(keep_idxs(IdxVector({1, 1, 3}), xs), IntVector({2,3}));
    REQUIRE_EQ(keep_idxs(IdxVector({1, 3, 7}), xs), IntVector({2,3}));
    REQUIRE_EQ(drop_idxs(IdxVector({1, 3}), xs), IntVector({1,2,2}));
}

TEST_CASE("container_common_test - is_equal")
{
    using namespace fplus;
    REQUIRE_EQ(is_equal_by_and_by(is_even_int, is_even_int)(2, 4), true);
    REQUIRE_EQ(is_equal_by_and_by(is_even_int, is_even_int)(1, 2), false);
    REQUIRE_EQ(is_equal_by_and_by(is_odd_int, is_even_int)(1, 2), true);
    REQUIRE_EQ(is_equal_to(2)(2), true);
    REQUIRE_EQ(is_equal_to(1)(2), false);
}

TEST_CASE("container_common_test - is_empty")
{
    using namespace fplus;
    REQUIRE_EQ(is_empty(xs), false);
    REQUIRE_EQ(is_empty(IntVector()), true);
    REQUIRE_EQ(is_not_empty(xs), true);
    REQUIRE_EQ(is_not_empty(IntVector()), false);
}

TEST_CASE("container_common_test - convert")
{
    using namespace fplus;
    REQUIRE_EQ(convert_container<IntList>(xs), intList);
    typedef std::vector<float> FloatVector;
    REQUIRE_EQ(convert_elems<float>(xs), FloatVector({1.0f,2.0f,2.0f,3.0f,2.0f}));

    typedef std::vector<ExplicitFromIntStruct> ExplicitFromIntStructs;
    ExplicitFromIntStructs explicitFromIntStructs = {
        ExplicitFromIntStruct(1),
        ExplicitFromIntStruct(2),
        ExplicitFromIntStruct(2),
        ExplicitFromIntStruct(3),
        ExplicitFromIntStruct(2)
    };

    REQUIRE_EQ(convert_elems<ExplicitFromIntStruct>(xs), explicitFromIntStructs);
}

TEST_CASE("container_common_test - append_elem")
{
    using namespace fplus;
    IntVector values = {1,2};
    REQUIRE_EQ(append_elem(3, values), IntVector({1,2,3}));
    REQUIRE_EQ(append_elem(3, IntVector({1,2})), IntVector({1,2,3}));
}

TEST_CASE("container_common_test - prepend_elem")
{
    using namespace fplus;
    IntVector values = {2,3};
    REQUIRE_EQ(prepend_elem(1, values), IntVector({1,2,3}));
    REQUIRE_EQ(prepend_elem(1, IntVector({2,3})), IntVector({1,2,3}));
}

TEST_CASE("container_common_test - append")
{
    using namespace fplus;
    std::vector<int> xs_empty;
    REQUIRE_EQ(append(xs, xs), xs2Times);
    REQUIRE_EQ(append(xs, xs_arr), xs2Times);
    REQUIRE_EQ(append(xs, xs_empty), xs);
}

TEST_CASE("container_common_test - append_convert")
{
    using namespace fplus;
    REQUIRE_EQ(append_convert<decltype(xs2Times)>(xs_arr, xs_arr), xs2Times);
    REQUIRE_EQ(append_convert<decltype(xs2Times)>(xs_arr, xs_deque), xs2Times);
}

TEST_CASE("container_common_test - interweave")
{
    using namespace fplus;
    REQUIRE_EQ(interweave(IntVector({1,3}), IntVector({2,4})), IntVector({1,2,3,4}));
    REQUIRE_EQ(interweave(IntVector({1,3,5,7}), IntVector({2,4})), IntVector({1,2,3,4,5,7}));
    REQUIRE_EQ(unweave(IntVector({0,1,2,3})), std::make_pair(IntVector({0,2}), IntVector({1,3})));
    REQUIRE_EQ(unweave(IntVector({0,1,2,3,4})), std::make_pair(IntVector({0,2,4}), IntVector({1,3})));
}

TEST_CASE("container_common_test - concat")
{
    using namespace fplus;
    std::vector<std::vector<int>> emptyIntVecs;
    std::vector<int> emptyIntVec;
    REQUIRE_EQ(concat(emptyIntVecs), emptyIntVec);
    REQUIRE_EQ(concat(intLists), intList);
    REQUIRE_EQ(concat(IntVectors(2, xs)), xs2Times);
}

TEST_CASE("container_common_test - repeat")
{
    using namespace fplus;
    REQUIRE_EQ(repeat(2, xs), xs2Times);
}

TEST_CASE("container_common_test - replicate")
{
    using namespace fplus;
    REQUIRE_EQ(replicate(2, xs), IntVectors({xs, xs}));
}

TEST_CASE("container_common_test - infixes")
{
    using namespace fplus;
    REQUIRE_EQ(infixes(3, xs), IntVectors({
        IntVector({1, 2, 2}),
        IntVector({2, 2, 3}),
        IntVector({2, 3, 2})}));
}

TEST_CASE("container_common_test - carthesian_product")
{
    using namespace fplus;
    typedef std::pair<std::string::value_type, std::string::value_type> char_pair;
    typedef std::vector<char_pair> char_pair_vec;
    auto twoCharsToString = [](std::string::value_type x, std::string::value_type y) { std::string result; result += x; result += y; return result; };
    auto alwaysTrueCharAndChar = [](std::string::value_type, std::string::value_type) { return true; };
    REQUIRE_EQ(carthesian_product_with(twoCharsToString, ABC_, XY), string_vec({"AX", "AY", "BX", "BY", "CX", "CY"}));
    REQUIRE_EQ(carthesian_product_where(alwaysTrueCharAndChar, ABC_, XY), char_pair_vec({{'A','X'}, {'A','Y'}, {'B','X'}, {'B','Y'}, {'C','X'}, {'C','Y'}}));
    auto charAndCharSumIsEven = [&](std::string::value_type x, std::string::value_type y) { return is_even_int(x + y); };
    REQUIRE_EQ(carthesian_product_with_where(twoCharsToString, charAndCharSumIsEven, ABC_, XY), string_vec({"AY", "BX", "CY"}));
    REQUIRE_EQ(carthesian_product_where(charAndCharSumIsEven, ABC_, XY), char_pair_vec({{'A','Y'}, {'B','X'}, {'C','Y'}}));
    REQUIRE_EQ(carthesian_product(ABC_, XY), char_pair_vec({{'A','X'}, {'A','Y'}, {'B','X'}, {'B','Y'}, {'C','X'}, {'C','Y'}}));
    REQUIRE_EQ(carthesian_product_n(2, ABCD), string_vec({"AA", "AB", "AC", "AD", "BA", "BB", "BC", "BD", "CA", "CB", "CC", "CD", "DA", "DB", "DC", "DD"}));
    REQUIRE_EQ(carthesian_product_n(2, vec0123), IntVectors({{0,0}, {0,1}, {0,2}, {0,3}, {1,0}, {1,1}, {1,2}, {1,3}, {2,0}, {2,1}, {2,2}, {2,3}, {3,0}, {3,1}, {3,2}, {3,3}}));
    REQUIRE_EQ(carthesian_product_n(0, vec0123), IntVectors({IntVector()}));
}

TEST_CASE("container_common_test - combination")
{
    using namespace fplus;
    typedef std::vector<std::list<int>> intListVec;
    REQUIRE_EQ(combinations(2, ABCD), string_vec({"AB", "AC", "AD", "BC", "BD", "CD"}));
    REQUIRE_EQ(combinations(1, ABCD), string_vec({"A", "B", "C", "D"}));
    REQUIRE_EQ(combinations(3, ABCD), string_vec({"ABC", "ABD", "ACD", "BCD"}));
    REQUIRE_EQ(combinations(2, vec0123), IntVectors({{0,1}, {0,2}, {0,3}, {1,2}, {1,3}, {2,3}}));
    REQUIRE_EQ(combinations(2, list0123), intListVec({{0,1}, {0,2}, {0,3}, {1,2}, {1,3}, {2,3}}));
    REQUIRE_EQ(combinations(0, IntVector()), IntVectors({IntVector()}));
    REQUIRE_EQ(combinations(0, vec0123), IntVectors({IntVector()}));
    REQUIRE_EQ(combinations(0, ABCD), string_vec({""}));
    REQUIRE_EQ(combinations_with_replacement(2, ABCD), string_vec({"AA", "AB", "AC", "AD", "BB", "BC", "BD", "CC", "CD", "DD"}));
    REQUIRE_EQ(combinations_with_replacement(0, vec0123), IntVectors({IntVector()}));
}

TEST_CASE("container_common_test - permutations")
{
    using namespace fplus;
    REQUIRE_EQ(permutations(2, ABCD), string_vec({"AB", "AC", "AD", "BA", "BC", "BD", "CA", "CB", "CD", "DA", "DB", "DC"}));
    REQUIRE_EQ(permutations(0, vec0123), IntVectors({IntVector()}));
}

TEST_CASE("container_common_test - power_set")
{
    using namespace fplus;
    REQUIRE_EQ(power_set(std::string("xyz")), string_vec({"", "x", "y", "z", "xy", "xz", "yz", "xyz"}));
}

TEST_CASE("container_common_test - rotations")
{
    using namespace fplus;
    REQUIRE_EQ(rotations_left(std::string("abcd")), string_vec({"abcd", "bcda", "cdab", "dabc"}));
    REQUIRE_EQ(rotations_right(std::string("abcd")), string_vec({"abcd", "dabc", "cdab", "bcda"}));
}

TEST_CASE("container_common_test - fill")
{
    using namespace fplus;
    REQUIRE_EQ(fill_left(0, 6, IntVector({1,2,3,4})), IntVector({0,0,1,2,3,4}));
    REQUIRE_EQ(fill_right(0, 6, IntList({1,2,3,4})), IntList({1,2,3,4,0,0}));
    REQUIRE_EQ(fill_left(' ', 6, std::string("12")), std::string("    12"));
}

TEST_CASE("container_common_test - intersperse")
{
    using namespace fplus;
    REQUIRE_EQ(intersperse(0, xs), IntVector({1,0,2,0,2,0,3,0,2}));
}

TEST_CASE("container_common_test - fold")
{
    using namespace fplus;
    REQUIRE_EQ(fold_left(std::plus<>(), 100, xs), 110);
    REQUIRE_EQ(fold_left_1(std::plus<>(), xs), 10);
    REQUIRE_EQ(fold_right(std::plus<>(), 100, xs), 110);
    REQUIRE_EQ(fold_right_1(std::plus<>(), xs), 10);
    auto appendXToStrForFoldL = [](const std::string& str, int x) { return str + std::to_string(x); };
    auto appendXToStrForFoldR = [](int x, const std::string& str) { return str + std::to_string(x); };
    std::string emptyString;
    REQUIRE_EQ(fold_left(appendXToStrForFoldL, emptyString, xs), "12232");
    REQUIRE_EQ(fold_right(appendXToStrForFoldR, emptyString, xs), "23221");
}

TEST_CASE("container_common_test - reduce")
{
    using namespace fplus;
    REQUIRE_EQ(reduce(std::plus<int>(), 100, xs), 110);
    REQUIRE_EQ(reduce_1(std::plus<int>(), xs), 10);
}

TEST_CASE("container_common_test - scan")
{
    using namespace fplus;
    REQUIRE_EQ(scan_left(std::plus<>(), 20, xs), IntVector({ 20,21,23,25,28,30 }));
    REQUIRE_EQ(scan_right(std::plus<>(), 20, xs), IntVector({ 30,29,27,25,22,20 }));
    REQUIRE_EQ(scan_left_1(std::plus<>(), xs), IntVector({ 1,3,5,8,10 }));
    REQUIRE_EQ(scan_right_1(std::plus<>(), xs), IntVector({ 10,9,7,5,2 }));
}

TEST_CASE("container_common_test - join")
{
    using namespace fplus;
    REQUIRE_EQ(join(std::string(", "), std::vector<std::string>({"a", "b", "sea"})), std::string("a, b, sea"));
    REQUIRE_EQ(join(IntList({0}), intLists), IntList({1,0,2,2,0,3,0,2}));
}

TEST_CASE("container_common_test - join_elem")
{
    using namespace fplus;
    REQUIRE_EQ(join_elem(',', std::vector<std::string>({"a", "b", "sea"})), std::string("a,b,sea"));
    REQUIRE_EQ(join_elem(0, intLists), IntList({1,0,2,2,0,3,0,2}));
}

TEST_CASE("container_common_test - all")
{
    using namespace fplus;
    REQUIRE_EQ(all(BoolVector()), true);
    REQUIRE_EQ(all(BoolVector({true})), true);
    REQUIRE_EQ(all(BoolVector({false})), false);
    REQUIRE_EQ(all(BoolVector({true, true})), true);
    REQUIRE_EQ(all(BoolVector({true, false})), false);

    REQUIRE_EQ(all_by(is_even_int, IntVector()), true);
    REQUIRE_EQ(all_by(is_even_int, IntVector({2})), true);
    REQUIRE_EQ(all_by(is_even_int, IntVector({1})), false);
    REQUIRE_EQ(all_by(is_even_int, IntVector({2, 2})), true);
    REQUIRE_EQ(all_by(is_even_int, IntVector({2, 1})), false);
}

TEST_CASE("container_common_test - size")
{
    using namespace fplus;
    REQUIRE_EQ(fplus::size_of_cont(xs), 5);
    REQUIRE_EQ(fplus::size_of_cont(IntVector()), 0);
    REQUIRE_EQ(is_not_empty(xs), true);
}

TEST_CASE("container_common_test - sort")
{
    using namespace fplus;
    REQUIRE_EQ(sort(reverse(xs)), xsSorted);
    REQUIRE_EQ(sort(reverse(intList)), intListSorted);
    REQUIRE_EQ(sort_by(std::greater<int>(), xs), reverse(xsSorted));

    REQUIRE_EQ(sort_on(int_mod_10, IntVector({26,3,14})), IntVector({3,14,26}));
    REQUIRE_EQ(sort_on(size_of_cont<IntVector>, IntVectors({{1,2,3},{4,5}})), IntVectors({{4,5},{1,2,3}}));
}

TEST_CASE("container_common_test - stable_sort")
{
    using namespace fplus;
    REQUIRE_EQ(stable_sort(reverse(xs)), xsSorted);
    REQUIRE_EQ(stable_sort(reverse(intList)), intListSorted);
    REQUIRE_EQ(stable_sort_by(std::greater<int>(), xs), reverse(xsSorted));

    REQUIRE_EQ(stable_sort_on(int_mod_10, IntVector({26,3,14})), IntVector({3,14,26}));
    REQUIRE_EQ(stable_sort_on(size_of_cont<IntVector>, IntVectors({{1,2,3},{4,5}})), IntVectors({{4,5},{1,2,3}}));
}

TEST_CASE("container_common_test - partial_sort")
{
    using namespace fplus;
    REQUIRE_EQ(partial_sort(2, xs_reverse), IntVector({1,2}));
    REQUIRE_EQ(partial_sort(2, reverse(xs)), IntVector({1,2}));
}

TEST_CASE("container_common_test - reverse")
{
    using namespace fplus;
    REQUIRE_EQ(reverse(IntVector({1,2,2,3,2})), xs_reverse);
    REQUIRE_EQ(reverse(xs), xs_reverse);
}

TEST_CASE("container_common_test - nth_element")
{
    using namespace fplus;
    REQUIRE_EQ(nth_element(0, xs), 1);
    REQUIRE_EQ(nth_element(1, xs), 2);
    REQUIRE_EQ(nth_element(2, xs), 2);
    REQUIRE_EQ(nth_element(3, xs), 2);
    REQUIRE_EQ(nth_element(4, xs), 3);

    REQUIRE_EQ(nth_element(1, xs_arr), 2);
}

TEST_CASE("container_common_test - unique")
{
    using namespace fplus;
    REQUIRE_EQ(unique(xs), IntVector({1,2,3,2}));
    auto IsEqualByis_even = [&](int a, int b)
            { return is_even_int(a) == is_even_int(b); };
    REQUIRE_EQ(unique_by(IsEqualByis_even, xs), IntVector({1,2,3,2}));

    REQUIRE_EQ(unique_on(int_mod_10, IntVector({2,22,3})), IntVector({2, 3}));
}

TEST_CASE("container_common_test - all_the_same")
{
    using namespace fplus;
    REQUIRE_EQ(all_the_same(IntVector()), true);
    REQUIRE_EQ(all_the_same(IntVector({1})), true);
    REQUIRE_EQ(all_the_same(IntVector({1,1,1})), true);
    REQUIRE_EQ(all_the_same(IntVector({1,2,1})), false);

    REQUIRE_EQ(all_the_same_on(int_mod_10, IntVector({3,13,33})), true);
    REQUIRE_EQ(all_the_same_on(int_mod_10, IntVector({3,14,33})), false);

    struct foo {};
    const auto foo_to_int = [](const foo&) -> int { return 3; };
    REQUIRE(fplus::all_the_same_on(foo_to_int, std::vector<foo>({})));
}

TEST_CASE("container_common_test - all_unique")
{
    using namespace fplus;
    REQUIRE_EQ(all_unique(IntVector()), true);
    REQUIRE_EQ(all_unique(IntVector({1})), true);
    REQUIRE_EQ(all_unique(IntVector({1,2,1})), false);
    REQUIRE_EQ(all_unique(IntVector({1,2,3})), true);

    REQUIRE_EQ(all_unique_less(IntVector()), true);
    REQUIRE_EQ(all_unique_less(IntVector({1})), true);
    REQUIRE_EQ(all_unique_less(IntVector({1,2,1})), false);
    REQUIRE_EQ(all_unique_less(IntVector({1,2,3})), true);

    REQUIRE_EQ(all_unique_on(int_mod_10, IntVector({3,14,35})), true);
    REQUIRE_EQ(all_unique_on(int_mod_10, IntVector({3,14,33})), false);
}

TEST_CASE("container_common_test - is_sorted")
{
    using namespace fplus;
    REQUIRE_EQ(is_sorted(IntVector()), true);
    REQUIRE_EQ(is_sorted(IntVector({1})), true);
    REQUIRE_EQ(is_sorted(IntVector({1,2,3})), true);
    REQUIRE_EQ(is_sorted(IntVector({1,2,2})), true);
    REQUIRE_EQ(is_sorted(IntVector({1,2,1})), false);
    REQUIRE_EQ(is_sorted_on(int_mod_10, IntVector({51,32,43})), true);
    REQUIRE_EQ(is_sorted_on(int_mod_10, IntVector({15,23})), false);
    REQUIRE_EQ(is_strictly_sorted(IntVector({1,2,3})), true);
    REQUIRE_EQ(is_strictly_sorted(IntVector({1,2,2})), false);
    REQUIRE_EQ(is_strictly_sorted(IntVector({1,2,1})), false);
    REQUIRE_EQ(is_strictly_sorted_on(int_mod_10, IntVector({51,32,43})), true);
    REQUIRE_EQ(is_strictly_sorted_on(int_mod_10, IntVector({51,32,43,63})), false);
    REQUIRE_EQ(is_strictly_sorted_on(int_mod_10, IntVector({15,23})), false);
}

TEST_CASE("container_common_test - find")
{
    using namespace fplus;

    REQUIRE_EQ(find_first_by(is3, xs), just(3));
    REQUIRE_EQ(find_first_by(is4, xs), nothing<int>());
    REQUIRE_EQ(find_first_idx_by(is2, xs), just<std::size_t>(1));
    REQUIRE_EQ(find_first_idx_by(is4, xs), nothing<std::size_t>());
    REQUIRE_EQ(find_first_idx(2, xs), just<std::size_t>(1));
    REQUIRE_EQ(find_first_idx(4, xs), nothing<std::size_t>());

    REQUIRE_EQ(find_last_by(is3, xs), just(3));
    REQUIRE_EQ(find_last_by(is4, xs), nothing<int>());
    REQUIRE_EQ(find_last_idx_by(is2, xs), just<std::size_t>(4));
    REQUIRE_EQ(find_last_idx_by(is4, xs), nothing<std::size_t>());
    REQUIRE_EQ(find_last_idx(2, xs), just<std::size_t>(4));
    REQUIRE_EQ(find_last_idx(4, xs), nothing<std::size_t>());
}

TEST_CASE("container_common_test - is_elem_of")
{
    using namespace fplus;
    REQUIRE_EQ(is_elem_of(2, xs), true);
    REQUIRE_EQ(is_elem_of(4, xs), false);
}

TEST_CASE("container_common_test - elem_at_idx")
{
    using namespace fplus;
    REQUIRE_EQ(elem_at_idx(2, xs), 2);

}

TEST_CASE("container_common_test - elem_at_idx_maybe")
{
    using namespace fplus;
    REQUIRE_EQ(elem_at_idx_maybe(2, xs), maybe<int>(2));
    REQUIRE_EQ(elem_at_idx_maybe(9, xs), nothing<int>());
}

TEST_CASE("container_common_test - find_token")
{
    using namespace fplus;
    REQUIRE_EQ(find_all_instances_of_token(std::string("Plus"),
        std::string("C Plus Plus is a nice language,") +
        std::string(" and FunctionalPlus makes it even nicer.")),
        std::vector<std::size_t>({ 2, 7, 46 }));
    REQUIRE_EQ(find_all_instances_of_token(std::string("xx"), std::string("bxxxxc")), std::vector<std::size_t>({ 1, 2, 3 }));
    REQUIRE_EQ(find_all_instances_of_token(std::string("xy"), std::string("xyaaa")), std::vector<std::size_t>({ 0 }));
    REQUIRE_EQ(find_all_instances_of_token(std::string("xy"), std::string("aaaxy")), std::vector<std::size_t>({ 3 }));
    REQUIRE_EQ(find_all_instances_of_token(std::string("xx"), std::string("xxxxx")), std::vector<std::size_t>({ 0, 1, 2, 3 }));
    REQUIRE_EQ(find_all_instances_of_token_non_overlapping(std::string("xx"), std::string("xxxx")), std::vector<std::size_t>({ 0, 2 }));
    REQUIRE_EQ(find_all_instances_of_token(IntVector({}), IntVector({})), IdxVector({0}));
    REQUIRE_EQ(find_all_instances_of_token(IntVector({}), IntVector({1})), IdxVector({0,1}));
    REQUIRE_EQ(find_all_instances_of_token_non_overlapping(IntVector({}), IntVector({})), IdxVector({0}));
    REQUIRE_EQ(find_all_instances_of_token_non_overlapping(IntVector({}), IntVector({1})), IdxVector({0,1}));
    REQUIRE_EQ(find_first_instance_of_token(IntVector({}), IntVector({})), just<std::size_t>(0));
    REQUIRE_EQ(find_first_instance_of_token(IntVector({}), IntVector({1,2})), just<std::size_t>(0));
    REQUIRE_EQ(find_first_instance_of_token(std::string("haha"), std::string("oh, hahaha!")), just<std::size_t>(4));
}

TEST_CASE("container_common_test - insert_at_idx")
{
    using namespace fplus;
    REQUIRE_EQ(insert_at_idx(2, 0, IntVector({1,2,3,4})), IntVector({1,2,0,3,4}));
}


TEST_CASE("container_common_test - segment")
{
    using namespace fplus;
    IntList v789 = { 7,8,9 };
    REQUIRE_EQ(set_segment(1, v789, IntList({ 1,2,2,3,2 })), IntList({ 1,7,8,9,2 }));
    REQUIRE_EQ(set_segment(1, v789, intList), IntList({ 1,7,8,9,2 }));
    REQUIRE_EQ(get_segment(2, 5, IntList({ 0,1,2,3,4,5,6,7,8 })), IntList({ 2,3,4 }));
    REQUIRE_EQ(get_segment(2, 15, IntList({ 0,1,2,3,4,5,6,7,8 })), IntList({ 2,3,4,5,6,7,8 }));
    REQUIRE_EQ(get_segment(5, 2, IntList({ 0,1,2,3,4,5,6,7,8 })), IntList({}));
    REQUIRE_EQ(get_segment(12, 15, IntList({ 0,1,2,3,4,5,6,7,8 })), IntList({}));
    REQUIRE_EQ(get_segment(15, 12, IntList({ 0,1,2,3,4,5,6,7,8 })), IntList({}));
    REQUIRE_EQ(replace_elems(2, 5, xs), IntVector({1,5,5,3,5}));
    REQUIRE_EQ(replace_tokens(std::string("123"), std::string("_"), std::string("--123----123123")), std::string("--_----__"));
    REQUIRE_EQ(take(2, xs), IntVector({ 1,2 }));
    REQUIRE_EQ(take_last(2, xs), IntVector({ 3,2 }));
    REQUIRE_EQ(take_exact(2, xs), IntVector({ 1,2 }));
    REQUIRE_EQ(drop(2, xs), IntVector({ 2,3,2 }));
    REQUIRE_EQ(drop_last(2, xs), IntVector({ 1,2,2 }));
    REQUIRE_EQ(drop_exact(2, xs), IntVector({ 2,3,2 }));
    REQUIRE_EQ(take(999, xs), xs);
    REQUIRE_EQ(drop(999, xs), IntVector());
    REQUIRE_EQ(take_while(is_odd_int, xs), IntVector({ 1 }));
    REQUIRE_EQ(take_while(always(true), xs), xs);
    REQUIRE_EQ(take_last_while(always(true), IntVector()), IntVector());
    REQUIRE_EQ(take_last_while(always(false), IntVector()), IntVector());
    REQUIRE_EQ(take_last_while(always(true), xs), xs);
    REQUIRE_EQ(take_last_while(always(false), xs), IntVector());
    REQUIRE_EQ(take_last_while(is_even_int, xs), IntVector({ 2 }));
    REQUIRE_EQ(take_last_while(is_even_int, intList), IntList({ 2 }));
    REQUIRE_EQ(drop_while(always(false), xs), xs);
    REQUIRE_EQ(drop_while(is_odd_int, xs), IntVector({ 2,2,3,2 }));
    REQUIRE_EQ(drop_last_while(always(false), IntVector()), IntVector());
    REQUIRE_EQ(drop_last_while(always(true), IntVector()), IntVector());
    REQUIRE_EQ(drop_last_while(always(false), xs), xs);
    REQUIRE_EQ(drop_last_while(always(true), xs), IntVector());
    REQUIRE_EQ(drop_last_while(is_even_int, xs), IntVector({ 1,2,2,3 }));
    REQUIRE_EQ(drop_last_while(is_even_int, intList), IntList({ 1,2,2,3 }));
    REQUIRE_EQ(span(is_odd_int, xs), std::make_pair(IntVector({ 1 }), IntVector({ 2,2,3,2 })));
    REQUIRE_EQ(set_segment(2, IntVector({8,9}), xs), IntVector({1,2,8,9,2}));
    REQUIRE_EQ(set_segment(2, IntVector({8,9}), IntVector({1,2,2,3,2})), IntVector({1,2,8,9,2}));
    REQUIRE_EQ(remove_segment(1, 3, xs), IntVector({1,3,2}));
    REQUIRE_EQ(remove_segment(1, 3, IntVector({1,2,2,3,2})), IntVector({1,3,2}));
    REQUIRE_EQ(take_cyclic(5, IntVector({0,1,2,3})), IntVector({0,1,2,3,0}));
    REQUIRE_EQ(take_cyclic(7, IntVector({0,1,2,3})), IntVector({0,1,2,3,0,1,2}));
    REQUIRE_EQ(take_cyclic(7, IntVector({0,1})), IntVector({0,1,0,1,0,1,0}));
    REQUIRE_EQ(take_cyclic(2, IntVector({0,1,2,3})), IntVector({0,1}));
    REQUIRE_EQ(take_cyclic(3, IntVector({0})), IntVector({0,0,0}));
    REQUIRE_EQ(take_while(is_even<int>, IntVector({ 4, 3 })), IntVector({4}));
    REQUIRE_EQ(drop_while(is_even<int>, IntVector({ 4, 8 })), IntVector());
}

TEST_CASE("container_common_test - keep_if")
{
    using namespace fplus;
    REQUIRE_EQ(keep_if(is2, xs), IntVector({ 2,2,2 }));
    REQUIRE_EQ(keep_if(is3, xs), IntVector({ 3 }));
    REQUIRE_EQ(keep_if(is4, xs), IntVector());
}

TEST_CASE("container_common_test - find_all_idxs_of")
{
    using namespace fplus;
    REQUIRE_EQ(find_all_idxs_of('h', std::string("oh, ha!")), std::vector<std::size_t>({ 1, 4 }));
    REQUIRE_EQ(find_all_idxs_of(2, xs), IdxVector({ 1,2,4 }));
}

TEST_CASE("container_common_test - generate")
{
    using namespace fplus;
    int countUpCounter = 0;
    auto countUp = [countUpCounter]() mutable { return countUpCounter++; };
    REQUIRE_EQ(generate<IntVector>(countUp, 3), IntVector({ 0,1,2 }));
    auto square_size_t_return_int = [&](std::size_t x) -> int
    {
        return squareLambda(static_cast<int>(x));
    };
    REQUIRE_EQ(generate_by_idx<IntVector>(square_size_t_return_int, 3), IntVector({ 0,1,4 }));
}

TEST_CASE("container_common_test - nub")
{
    using namespace fplus;
    REQUIRE_EQ(nub(xs), IntVector({ 1,2,3 }));
    auto bothEven = is_equal_by(is_even_int);
    REQUIRE_EQ(nub_by(bothEven, xs), IntVector({ 1,2 }));
    REQUIRE_EQ(nub_on(int_mod_10, IntVector({12,32,15})), IntVector({12,15}));
}

TEST_CASE("container_common_test - coucount_occurrences_bynt_occurrences_on")
{
    using namespace fplus;
    typedef std::map<int, std::size_t> IntSizeTMap;
    IntSizeTMap OccurrencesResult = {{1, 1}, {2, 3}, {3, 1}};
    std::vector<double> double_values = {1.1, 2.3, 2.7, 3.6, 2.4};
    const auto f = floor<double>;
    REQUIRE_EQ(count_occurrences_by(f, double_values), OccurrencesResult);
}

TEST_CASE("container_common_test - count_occurrences")
{
    using namespace fplus;
    typedef std::map<int, std::size_t> IntSizeTMap;
    IntSizeTMap OccurrencesResult = {{1, 1}, {2, 3}, {3, 1}};
    REQUIRE_EQ(count_occurrences(xs), OccurrencesResult);
}

TEST_CASE("container_common_test - insert_at")
{
    using namespace fplus;
    REQUIRE_EQ(insert_at(2, IntVector({8,9}), xs), IntVector({1,2,8,9,2,3,2}));
}

TEST_CASE("container_common_test - head")
{
    using namespace fplus;
    REQUIRE_EQ(head(xs), 1);
}

TEST_CASE("container_common_test - last")
{
    using namespace fplus;
    REQUIRE_EQ(last(xs), 2);
}

TEST_CASE("container_common_test - tail")
{
    using namespace fplus;
    REQUIRE_EQ(init(xs), IntVector({1,2,2,3}));
    REQUIRE_EQ(init(IntVector({1,2,2,3,2})), IntVector({1,2,2,3}));
    REQUIRE_EQ(tail(xs), IntVector({2,2,3,2}));
    REQUIRE_EQ(tail(IntVector({1,2,2,3,2})), IntVector({2,2,3,2}));
    REQUIRE_EQ(inits(xs), IntVectors({{},{1},{1,2},{1,2,2},{1,2,2,3},{1,2,2,3,2}}));
    REQUIRE_EQ(tails(xs), IntVectors({{1,2,2,3,2},{2,2,3,2},{2,3,2},{3,2},{2},{}}));
}

TEST_CASE("container_common_test - iterate")
{
    using namespace fplus;
    auto times_two = [](int x) { return 2*x; };
    REQUIRE_EQ(iterate(times_two, 0, 3), IntVector({}));
    REQUIRE_EQ(iterate(times_two, 1, 3), IntVector({3}));
    REQUIRE_EQ(iterate(times_two, 2, 3), IntVector({3,6}));
    REQUIRE_EQ(iterate(times_two, 5, 3), IntVector({3,6,12,24,48}));
}

TEST_CASE("container_common_test - divvy")
{
    using namespace fplus;
    REQUIRE_EQ(divvy(5, 2, IntVector({0,1,2,3,4,5,6,7,8,9})), IntVectors({{0,1,2,3,4},{2,3,4,5,6},{4,5,6,7,8}}));
}

TEST_CASE("container_common_test - aperture")
{
    using namespace fplus;
    REQUIRE_EQ(aperture(5, IntVector({0,1,2,3,4,5,6})), IntVectors({{0,1,2,3,4},{1,2,3,4,5},{2,3,4,5,6}}));
    REQUIRE_EQ(divvy(5, 1, IntVector({0,1,2,3,4,5,6})), IntVectors({{0,1,2,3,4},{1,2,3,4,5},{2,3,4,5,6}}));
}

TEST_CASE("container_common_test - stride")
{
    using namespace fplus;
    REQUIRE_EQ(stride(3, IntVector({0,1,2,3,4,5,6,7})), IntVector({0,3,6}));
    REQUIRE_EQ(divvy(1, 3, IntVector({0,1,2,3,4,5,6,7})), IntVectors({{0},{3},{6}}));
}

TEST_CASE("composition_test - instead_of_if_empty")
{
    using namespace fplus;

    std::vector<int> xs_empty;
    std::vector<int> xs_full = {1,2,3};
    std::vector<int> xs_alt = {0};
    REQUIRE_EQ(instead_of_if_empty(xs_alt, xs_empty), xs_alt);
    REQUIRE_EQ(instead_of_if_empty(xs_alt, xs_full), xs_full);
}
