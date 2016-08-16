// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "fplus/fplus.h"
#include <vector>

using namespace testing;

namespace {
    auto squareLambda = [](int x) -> int { return x*x; };
    auto is_even = [](int x){ return x % 2 == 0; };
    auto is_even_size_t = [](std::size_t x){ return x % 2 == 0; };
    auto is_odd = [](int x){ return x % 2 == 1; };
    typedef std::pair<int, int> IntPair;
    typedef std::vector<IntPair> IntPairs;
    typedef std::vector<int> IntVector;
    typedef std::vector<IntVector> IntVectors;
    typedef std::vector<bool> BoolVector;
    typedef std::vector<std::size_t> IdxVector;
    IntVector xs = {1,2,2,3,2};
    std::array<int, 5> xs_array = {{1,2,2,3,2}};
    IntVector xsSorted = {1,2,2,2,3};
    std::string xsShown("[1, 2, 2, 3, 2]");
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
    std::string ABC("ABC");
    std::string XY("XY");
    std::string ABCD("ABCD");

    struct ExplicitFromIntStruct
    {
        explicit ExplicitFromIntStruct(int x) : x_(x) {}
        int x_;
    };
    bool operator == (const ExplicitFromIntStruct &lhs, const ExplicitFromIntStruct & rhs) { return lhs.x_ == rhs.x_; }
}

TEST(container_common_test, group)
{
    using namespace fplus;
    EXPECT_EQ(group(intList), intLists);
    EXPECT_EQ(group(xs), IntVectors({IntVector({1}),IntVector({2,2}),IntVector({3}),IntVector({2})}));
    EXPECT_EQ(group_on(int_mod_10, IntVector({12,22,34})), IntVectors({IntVector({12,22}),IntVector({34})}));
    EXPECT_EQ(group_globally(xs), IntVectors({IntVector({1}),IntVector({2,2,2}),IntVector({3})}));
    EXPECT_EQ(group_globally_on(int_mod_10, IntVector({12,34,22})), IntVectors({IntVector({12,22}),IntVector({34})}));
    EXPECT_EQ(group_by(abs_diff_less_or_equal_3, IntVector({2,3,6,4,22,21,8,5})), IntVectors({{2,3,6,4},{22,21},{8,5}}));
}

TEST(container_common_test, transform)
{
    using namespace fplus;

    auto intTimes2 = [](int x) -> int { return x*2; };
    auto intTimes3 = [](int x) -> int { return x*3; };

    // The following works with C++14.
    // EXPECT_EQ(transform([](auto x) { return x*x; }, xs), IntVector({1,4,4,9,4}));
    std::initializer_list<int> initListInts = { 1,2,2,3,2 };
    EXPECT_EQ(transform(squareLambda, std::vector<int>(initListInts)), IntVector({1,4,4,9,4}));
    EXPECT_EQ(transform_convert<std::vector<int>>(squareLambda, initListInts), IntVector({1,4,4,9,4}));
    EXPECT_EQ(transform_convert<std::vector<int>>(squareLambda, xs_array), IntVector({1,4,4,9,4}));
    EXPECT_EQ(transform(squareLambda, xs), IntVector({1,4,4,9,4}));
    EXPECT_EQ(transform(squareLambda, intList), IntList({ 1,4,4,9,4 }));
    EXPECT_EQ(transform_parallelly(squareLambda, intList), IntList({ 1,4,4,9,4 }));
    EXPECT_EQ(transform_convert<IntList>(squareLambda, xs), IntList({ 1,4,4,9,4 }));

    EXPECT_EQ(transform(squareLambda, std::set<int>({1,2,3,-3})), std::set<int>({1,4,9}));
    EXPECT_EQ(transform_inner(intTimes2, IntVectors({{1,3,4},{1,2}})), IntVectors({{2,6,8},{2,4}}));

    auto add_size_t_and_int = [](std::size_t a, int b) -> int
    {
        return static_cast<int>(a) + b;
    };
    EXPECT_EQ(transform_with_idx(add_size_t_and_int, xs), IntVector({1+0,2+1,2+2,3+3,2+4}));

    std::vector<std::function<int(int)>> multiplyFunctions = {intTimes2, intTimes3};
    EXPECT_THAT(apply_functions(multiplyFunctions, 4), ElementsAre(8, 12));

    auto showInt = [](int x) -> std::string { return fplus::show(x); };
    std::vector<std::function<std::string(int)>> showIntFuncs = {showInt, showInt};
    EXPECT_THAT(apply_functions(showIntFuncs, 4), ElementsAre("4", "4"));
}

TEST(container_common_test, filter)
{
    using namespace fplus;
    EXPECT_EQ(keep_if(is_even, xs), IntVector({2,2,2}));
    EXPECT_EQ(drop_if(is_even, xs), IntVector({1,3}));
    EXPECT_EQ(keep_if(is_even, intList), IntList({ 2,2,2 }));
    EXPECT_EQ(drop_if(is_even, intList), IntList({ 1,3 }));
    EXPECT_EQ(without(2, intList), IntList({ 1,3 }));
    auto sumis_even = [&](std::size_t x, int y)
    {
        return is_even(static_cast<int>(x) + y);
    };
    EXPECT_EQ(keep_by_idx(is_even_size_t, xs), IntVector({ 1,2,2 }));
    EXPECT_EQ(keep_if_with_idx(sumis_even, xs), IntVector({ 2,3,2 }));
    EXPECT_EQ(drop_if_with_idx(sumis_even, xs), IntVector({ 1,2 }));
}

TEST(container_common_test, trim)
{
    using namespace fplus;
    EXPECT_EQ(trim_left(1, intList), IntList({2,2,3,2}));
    EXPECT_EQ(trim_right(2, intList), IntList({1,2,2,3}));
    EXPECT_EQ(trim(0, IntVector({0,2,4,5,6,7,8,0,0})), IntVector({2,4,5,6,7,8}));
    EXPECT_EQ(trim_token_left(IntVector({1,2}), xs), IntVector({2,3,2}));
    EXPECT_EQ(trim_token_left(IntVector({0,1,2}), IntVector({0,1,2,0,1,2,7,5,9})), IntVector({7,5,9}));
    EXPECT_EQ(trim_token_right(IntVector({3,2}), xs), IntVector({1,2,2}));
    EXPECT_EQ(trim_token(IntVector({0,1}), IntVector({0,1,7,8,9,0,1})), IntVector({7,8,9}));
}

TEST(container_common_test, cluster)
{
    using namespace fplus;
    EXPECT_EQ(cluster_by(abs_diff_less_or_equal_3, IntVector({2,3,6,4,12,11,20,23,8,4})), IntVectors({{2,3,6,4,12,11,8,4},{20,23}}));
}

TEST(container_common_test, run_length_encode)
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
    EXPECT_EQ(run_length_encode(rle_input), rle_result);
    EXPECT_EQ(run_length_decode(rle_result), rle_input);
}

TEST(container_common_test, keep_idxs)
{
    using namespace fplus;
    EXPECT_EQ(keep_idxs(IdxVector({1, 3}), xs), IntVector({2,3}));
    EXPECT_EQ(keep_idxs(IdxVector({3, 1}), xs), IntVector({2,3}));
    EXPECT_EQ(keep_idxs(IdxVector({1, 1, 3}), xs), IntVector({2,3}));
    EXPECT_EQ(keep_idxs(IdxVector({1, 3, 7}), xs), IntVector({2,3}));
    EXPECT_EQ(drop_idxs(IdxVector({1, 3}), xs), IntVector({1,2,2}));
}

TEST(container_common_test, is_equal)
{
    using namespace fplus;
    EXPECT_EQ(is_equal_by_and_by(is_even, is_even)(2, 4), true);
    EXPECT_EQ(is_equal_by_and_by(is_even, is_even)(1, 2), false);
    EXPECT_EQ(is_equal_by_and_by(is_odd, is_even)(1, 2), true);
    EXPECT_EQ(is_equal_to(2)(2), true);
    EXPECT_EQ(is_equal_to(1)(2), false);
}

TEST(container_common_test, is_empty)
{
    using namespace fplus;
    EXPECT_EQ(is_empty(xs), false);
    EXPECT_EQ(is_empty(IntVector()), true);
    EXPECT_EQ(is_not_empty(xs), true);
    EXPECT_EQ(is_not_empty(IntVector()), false);
}

TEST(container_common_test, convert)
{
    using namespace fplus;
    EXPECT_EQ(convert_container<IntList>(xs), intList);
    typedef std::vector<float> FloatVector;
    EXPECT_EQ(convert_elems<float>(xs), FloatVector({1.0f,2.0f,2.0f,3.0f,2.0f}));

    typedef std::vector<ExplicitFromIntStruct> ExplicitFromIntStructs;
    ExplicitFromIntStructs explicitFromIntStructs = {
        ExplicitFromIntStruct(1),
        ExplicitFromIntStruct(2),
        ExplicitFromIntStruct(2),
        ExplicitFromIntStruct(3),
        ExplicitFromIntStruct(2)
    };

    EXPECT_EQ(convert_elems<ExplicitFromIntStruct>(xs), explicitFromIntStructs);
}

TEST(container_common_test, append)
{
    using namespace fplus;
    EXPECT_EQ(append(xs, xs), xs2Times);
}

TEST(container_common_test, interweave)
{
    using namespace fplus;
    EXPECT_EQ(interweave(IntVector({1,3}), IntVector({2,4})), IntVector({1,2,3,4}));
    EXPECT_EQ(interweave(IntVector({1,3,5,7}), IntVector({2,4})), IntVector({1,2,3,4,5,7}));
    EXPECT_EQ(unweave(IntVector({0,1,2,3})), std::make_pair(IntVector({0,2}), IntVector({1,3})));
    EXPECT_EQ(unweave(IntVector({0,1,2,3,4})), std::make_pair(IntVector({0,2,4}), IntVector({1,3})));
}

TEST(container_common_test, concat)
{
    using namespace fplus;
    EXPECT_EQ(concat(intLists), intList);
    EXPECT_EQ(concat(IntVectors(2, xs)), xs2Times);
}

TEST(container_common_test, repeat)
{
    using namespace fplus;
    EXPECT_EQ(repeat(2, xs), xs2Times);
}

TEST(container_common_test, replicate)
{
    using namespace fplus;
    EXPECT_EQ(replicate(2, xs), IntVectors({xs, xs}));
}

TEST(container_common_test, infixes)
{
    using namespace fplus;
    EXPECT_EQ(infixes(3, xs), IntVectors({
        IntVector({1, 2, 2}),
        IntVector({2, 2, 3}),
        IntVector({2, 3, 2})}));
}

TEST(container_common_test, carthesian_product)
{
    using namespace fplus;
    typedef std::pair<std::string::value_type, std::string::value_type> char_pair;
    typedef std::vector<char_pair> char_pair_vec;
    auto twoCharsToString = [](std::string::value_type x, std::string::value_type y) { std::string result; result += x; result += y; return result; };
    auto alwaysTrueCharAndChar = [](std::string::value_type, std::string::value_type) { return true; };
    EXPECT_EQ(carthesian_product_with(twoCharsToString, ABC, XY), string_vec({"AX", "AY", "BX", "BY", "CX", "CY"}));
    EXPECT_EQ(carthesian_product_where(alwaysTrueCharAndChar, ABC, XY), char_pair_vec({{'A','X'}, {'A','Y'}, {'B','X'}, {'B','Y'}, {'C','X'}, {'C','Y'}}));
    auto charAndCharSumIsEven = [&](std::string::value_type x, std::string::value_type y) { return is_even(x + y); };
    EXPECT_EQ(carthesian_product_with_where(twoCharsToString, charAndCharSumIsEven, ABC, XY), string_vec({"AY", "BX", "CY"}));
    EXPECT_EQ(carthesian_product_where(charAndCharSumIsEven, ABC, XY), char_pair_vec({{'A','Y'}, {'B','X'}, {'C','Y'}}));
    EXPECT_EQ(carthesian_product(ABC, XY), char_pair_vec({{'A','X'}, {'A','Y'}, {'B','X'}, {'B','Y'}, {'C','X'}, {'C','Y'}}));
    EXPECT_EQ(carthesian_product_n(2, ABCD), string_vec({"AA", "AB", "AC", "AD", "BA", "BB", "BC", "BD", "CA", "CB", "CC", "CD", "DA", "DB", "DC", "DD"}));
    EXPECT_EQ(carthesian_product_n(2, vec0123), IntVectors({{0,0}, {0,1}, {0,2}, {0,3}, {1,0}, {1,1}, {1,2}, {1,3}, {2,0}, {2,1}, {2,2}, {2,3}, {3,0}, {3,1}, {3,2}, {3,3}}));
    EXPECT_EQ(carthesian_product_n(0, vec0123), IntVectors({IntVector()}));
}

TEST(container_common_test, combination)
{
    using namespace fplus;
    typedef std::vector<std::list<int>> intListVec;
    EXPECT_EQ(combinations(2, ABCD), string_vec({"AB", "AC", "AD", "BC", "BD", "CD"}));
    EXPECT_EQ(combinations(1, ABCD), string_vec({"A", "B", "C", "D"}));
    EXPECT_EQ(combinations(3, ABCD), string_vec({"ABC", "ABD", "ACD", "BCD"}));
    EXPECT_EQ(combinations(2, vec0123), IntVectors({{0,1}, {0,2}, {0,3}, {1,2}, {1,3}, {2,3}}));
    EXPECT_EQ(combinations(2, list0123), intListVec({{0,1}, {0,2}, {0,3}, {1,2}, {1,3}, {2,3}}));
    EXPECT_EQ(combinations(0, IntVector()), IntVectors({IntVector()}));
    EXPECT_EQ(combinations(0, vec0123), IntVectors({IntVector()}));
    EXPECT_EQ(combinations(0, ABCD), string_vec({""}));
    EXPECT_EQ(combinations_with_replacement(2, ABCD), string_vec({"AA", "AB", "AC", "AD", "BB", "BC", "BD", "CC", "CD", "DD"}));
    EXPECT_EQ(combinations_with_replacement(0, vec0123), IntVectors({IntVector()}));
}

TEST(container_common_test, permutations)
{
    using namespace fplus;
    EXPECT_EQ(permutations(2, ABCD), string_vec({"AB", "AC", "AD", "BA", "BC", "BD", "CA", "CB", "CD", "DA", "DB", "DC"}));
    EXPECT_EQ(permutations(0, vec0123), IntVectors({IntVector()}));
}

TEST(container_common_test, power_set)
{
    using namespace fplus;
    EXPECT_EQ(power_set(std::string("xyz")), string_vec({"", "x", "y", "z", "xy", "xz", "yz", "xyz"}));
}

TEST(container_common_test, rotations)
{
    using namespace fplus;
    EXPECT_EQ(rotations_left(std::string("abcd")), string_vec({"abcd", "bcda", "cdab", "dabc"}));
    EXPECT_EQ(rotations_right(std::string("abcd")), string_vec({"abcd", "dabc", "cdab", "bcda"}));
}

TEST(container_common_test, fill)
{
    using namespace fplus;
    EXPECT_EQ(fill_left(0, 6, IntVector({1,2,3,4})), IntVector({0,0,1,2,3,4}));
    EXPECT_EQ(fill_right(0, 6, IntList({1,2,3,4})), IntList({1,2,3,4,0,0}));
    EXPECT_EQ(fill_left(' ', 6, std::string("12")), std::string("    12"));
}

TEST(container_common_test, intersperse)
{
    using namespace fplus;
    EXPECT_EQ(intersperse(0, xs), IntVector({1,0,2,0,2,0,3,0,2}));
}

TEST(container_common_test, fold)
{
    using namespace fplus;
    EXPECT_EQ(fold_left(std::plus<int>(), 100, xs), 110);
    EXPECT_EQ(fold_left_1(std::plus<int>(), xs), 10);
    EXPECT_EQ(fold_right(std::plus<int>(), 100, xs), 110);
    EXPECT_EQ(fold_right_1(std::plus<int>(), xs), 10);
    auto appendXToStrForFoldL = [](const std::string& str, int x) { return str + std::to_string(x); };
    auto appendXToStrForFoldR = [](int x, const std::string& str) { return str + std::to_string(x); };
    std::string emptyString;
    EXPECT_EQ(fold_left(appendXToStrForFoldL, emptyString, xs), "12232");
    EXPECT_EQ(fold_right(appendXToStrForFoldR, emptyString, xs), "23221");
}

TEST(container_common_test, scan)
{
    using namespace fplus;
    EXPECT_EQ(scan_left(std::plus<int>(), 20, xs), IntVector({ 20,21,23,25,28,30 }));
    EXPECT_EQ(scan_right(std::plus<int>(), 20, xs), IntVector({ 30,29,27,25,22,20 }));
    EXPECT_EQ(scan_left_1(std::plus<int>(), xs), IntVector({ 1,3,5,8,10 }));
    EXPECT_EQ(scan_right_1(std::plus<int>(), xs), IntVector({ 10,9,7,5,2 }));
}

TEST(container_common_test, join)
{
    using namespace fplus;
    EXPECT_EQ(join(std::string(", "), std::vector<std::string>({"a", "b", "sea"})), std::string("a, b, sea"));
    EXPECT_EQ(join(IntList({0}), intLists), IntList({1,0,2,2,0,3,0,2}));
}

TEST(container_common_test, show)
{
    using namespace fplus;
    std::map<int, std::string> mapToShow = {{1, "one"}, {2, "two"}};
    EXPECT_EQ(show_cont(mapToShow), "[(1, one), (2, two)]");
    EXPECT_EQ(show_cont(xs), xsShown);
    EXPECT_EQ(show_cont_with(", ", xs), xsShown);
    std::string xsShownNLs = "(1,2,\n"
                             " 2,3,\n"
                             " 2)";
    EXPECT_EQ(show_cont_with_frame_and_newlines(",", "(", ")", xs, 2), xsShownNLs);
    EXPECT_EQ(show<int>(1), "1");
}

TEST(container_common_test, zip_with)
{
    using namespace fplus;
    auto multiply = [](int x, int y){ return x * y; };
    EXPECT_EQ(zip_with(multiply, xs, xs), transform(squareLambda, xs));
}

TEST(container_common_test, show_float)
{
    using namespace fplus;
    const double pi = 3.14159;
    EXPECT_EQ(show_float<double>(0, 3)(pi), "3.142");
    EXPECT_EQ(show_float<double>(1, 3)(pi), "3.142");
    EXPECT_EQ(show_float<double>(2, 3)(pi), "03.142");
    EXPECT_EQ(show_float<double>(3, 3)(pi), "003.142");
    EXPECT_EQ(show_float<double>(1, 2)(pi), "3.14");
    EXPECT_EQ(show_float<double>(1, 4)(pi), "3.1416");
    EXPECT_EQ(show_float<double>(1, 7)(pi), "3.1415900");
    EXPECT_EQ(show_float<double>(0, 3)(-pi), "-3.142");
    EXPECT_EQ(show_float<double>(1, 3)(-pi), "-3.142");
    EXPECT_EQ(show_float<double>(2, 3)(-pi), "-3.142");
    EXPECT_EQ(show_float<double>(3, 3)(-pi), "-03.142");
    EXPECT_EQ(show_float<double>(4, 3)(-pi), "-003.142");
    EXPECT_EQ(show_float<double>(0, 3)(0.142), "0.142");
    EXPECT_EQ(show_float<double>(1, 3)(0.142), "0.142");
    EXPECT_EQ(show_float<double>(2, 3)(0.142), "00.142");
    EXPECT_EQ(fill_left(' ', 8, show_float<double>(0, 3)(-pi)), "  -3.142");

    EXPECT_EQ(show_float_fill_left<double>(' ', 8, 3)(pi), "   3.142");
    EXPECT_EQ(show_float_fill_left<double>(' ', 8, 6)(pi), "3.141590");
    EXPECT_EQ(show_float_fill_left<double>(' ', 8, 3)(-pi), "  -3.142");
    EXPECT_EQ(show_float_fill_left<double>(' ', 2, 3)(-pi), "-3.142");
}

TEST(container_common_test, zip)
{
    using namespace fplus;
    auto xsZippedWithXs = zip(xs, xs);
    EXPECT_EQ(unzip(xsZippedWithXs).first, xs);
}

TEST(container_common_test, all)
{
    using namespace fplus;
    EXPECT_EQ(all(BoolVector()), true);
    EXPECT_EQ(all(BoolVector({true})), true);
    EXPECT_EQ(all(BoolVector({false})), false);
    EXPECT_EQ(all(BoolVector({true, true})), true);
    EXPECT_EQ(all(BoolVector({true, false})), false);

    EXPECT_EQ(all_by(is_even, IntVector()), true);
    EXPECT_EQ(all_by(is_even, IntVector({2})), true);
    EXPECT_EQ(all_by(is_even, IntVector({1})), false);
    EXPECT_EQ(all_by(is_even, IntVector({2, 2})), true);
    EXPECT_EQ(all_by(is_even, IntVector({2, 1})), false);
}

TEST(container_common_test, any)
{
    using namespace fplus;
    EXPECT_EQ(any(BoolVector()), false);
    EXPECT_EQ(any(BoolVector({true})), true);
    EXPECT_EQ(any(BoolVector({false})), false);
    EXPECT_EQ(any(BoolVector({false, false})), false);
    EXPECT_EQ(any(BoolVector({true, false})), true);

    EXPECT_EQ(any_by(is_even, IntVector()), false);
    EXPECT_EQ(any_by(is_even, IntVector({2})), true);
    EXPECT_EQ(any_by(is_even, IntVector({1})), false);
    EXPECT_EQ(any_by(is_even, IntVector({1, 1})), false);
    EXPECT_EQ(any_by(is_even, IntVector({2, 1})), true);
}

TEST(container_common_test, none)
{
    using namespace fplus;
    EXPECT_EQ(none(BoolVector()), true);
    EXPECT_EQ(none(BoolVector({true})), false);
    EXPECT_EQ(none(BoolVector({false})), true);
    EXPECT_EQ(none(BoolVector({false, false})), true);
    EXPECT_EQ(none(BoolVector({true, false})), false);

    EXPECT_EQ(none_by(is_even, IntVector()), true);
    EXPECT_EQ(none_by(is_even, IntVector({2})), false);
    EXPECT_EQ(none_by(is_even, IntVector({1})), true);
    EXPECT_EQ(none_by(is_even, IntVector({1, 1})), true);
    EXPECT_EQ(none_by(is_even, IntVector({2, 1})), false);
}

TEST(container_common_test, minmax)
{
    using namespace fplus;
    auto negateInt = [](int i) -> int { return -i; };

    EXPECT_EQ(minimum(xs), 1);
    EXPECT_EQ(maximum(xs), 3);

    EXPECT_EQ(minimum_by(std::greater<int>(), xs), 3);
    EXPECT_EQ(maximum_by(std::greater<int>(), xs), 1);

    EXPECT_EQ(minimum_on(negateInt, xs), 3);
    EXPECT_EQ(maximum_on(negateInt, xs), 1);

    EXPECT_EQ(minimum_idx(xs), 0);
    EXPECT_EQ(maximum_idx(xs), 3);

    EXPECT_EQ(minimum_idx_by(std::greater<int>(), xs), 3);
    EXPECT_EQ(maximum_idx_by(std::greater<int>(), xs), 0);

    EXPECT_EQ(minimum_idx_on(negateInt, xs), 3);
    EXPECT_EQ(maximum_idx_on(negateInt, xs), 0);
}

TEST(container_common_test, size)
{
    using namespace fplus;
    EXPECT_EQ(fplus::size_of_cont(xs), 5);
    EXPECT_EQ(fplus::size_of_cont(IntVector()), 0);
    EXPECT_EQ(is_not_empty(xs), true);
}

TEST(container_common_test, mean)
{
    using namespace fplus;
    std::vector<unsigned char> uchars = {200, 202};
    typedef std::vector<double> DoubleVector;
    EXPECT_EQ(sum(xs), 10);
    EXPECT_EQ(product(xs), 24);
    EXPECT_EQ(mean<int>(xs), 2);
    EXPECT_EQ(mean_using_doubles<unsigned char>(uchars), 201);
    EXPECT_EQ(median(IntVector({ 3 })), 3);
    EXPECT_EQ(median(IntVector({ 3, 5 })), 4);
    EXPECT_TRUE(is_in_range(3.49f, 3.51f)(median<IntVector, float>(IntVector({ 3, 4 }))));
    EXPECT_TRUE(is_in_range(3.49, 3.51)(mean<double>(DoubleVector({ 3, 4 }))));
    EXPECT_EQ(median(IntVector({ 3, 9, 5 })), 5);
    EXPECT_EQ(median(xs), 2);
    EXPECT_EQ(sum(convert_container_and_elems<std::vector<int>>(std::string("hello"))), 532);
    EXPECT_TRUE(is_in_range(5.99, 6.01)(mean_stddev<double>(DoubleVector({ 4, 8 })).first));
    EXPECT_TRUE(is_in_range(1.99, 2.01)(mean_stddev<double>(DoubleVector({ 4, 8 })).second));
    EXPECT_TRUE(is_in_range(3.749f, 3.751f)(mean_stddev<float>(IntVector({ 1, 3, 7, 4 })).first));
    EXPECT_TRUE(is_in_range(2.16f, 2.17f)(mean_stddev<float>(IntVector({ 1, 3, 7, 4 })).second));
}

TEST(container_common_test, sort)
{
    using namespace fplus;
    EXPECT_EQ(sort(reverse(xs)), xsSorted);
    EXPECT_EQ(sort(reverse(intList)), intListSorted);
    EXPECT_EQ(sort_by(std::greater<int>(), xs), reverse(xsSorted));

    EXPECT_EQ(sort_on(int_mod_10, IntVector({26,3,14})), IntVector({3,14,26}));

    EXPECT_EQ(partial_sort(2, reverse(xs)), IntVector({1,2}));
}

TEST(container_common_test, unique)
{
    using namespace fplus;
    EXPECT_EQ(unique(xs), IntVector({1,2,3,2}));
    auto IsEqualByis_even = [&](int a, int b)
            { return is_even(a) == is_even(b); };
    EXPECT_EQ(unique_by(IsEqualByis_even, xs), IntVector({1,2,3,2}));

    EXPECT_EQ(unique_on(int_mod_10, IntVector({2,22,3})), IntVector({2, 3}));
}

TEST(container_common_test, all_the_same)
{
    using namespace fplus;
    EXPECT_EQ(all_the_same(IntVector()), true);
    EXPECT_EQ(all_the_same(IntVector({1})), true);
    EXPECT_EQ(all_the_same(IntVector({1,1,1})), true);
    EXPECT_EQ(all_the_same(IntVector({1,2,1})), false);

    EXPECT_EQ(all_the_same_on(int_mod_10, IntVector({3,13,33})), true);
    EXPECT_EQ(all_the_same_on(int_mod_10, IntVector({3,14,33})), false);
}

TEST(container_common_test, all_unique)
{
    using namespace fplus;
    EXPECT_EQ(all_unique(IntVector()), true);
    EXPECT_EQ(all_unique(IntVector({1})), true);
    EXPECT_EQ(all_unique(IntVector({1,2,1})), false);
    EXPECT_EQ(all_unique(IntVector({1,2,3})), true);

    EXPECT_EQ(all_unique_less(IntVector()), true);
    EXPECT_EQ(all_unique_less(IntVector({1})), true);
    EXPECT_EQ(all_unique_less(IntVector({1,2,1})), false);
    EXPECT_EQ(all_unique_less(IntVector({1,2,3})), true);

    EXPECT_EQ(all_unique_on(int_mod_10, IntVector({3,14,35})), true);
    EXPECT_EQ(all_unique_on(int_mod_10, IntVector({3,14,33})), false);
}

TEST(container_common_test, is_sorted)
{
    using namespace fplus;
    EXPECT_EQ(is_sorted(IntVector()), true);
    EXPECT_EQ(is_sorted(IntVector({1})), true);
    EXPECT_EQ(is_sorted(IntVector({1,2,3})), true);
    EXPECT_EQ(is_sorted(IntVector({1,2,2})), true);
    EXPECT_EQ(is_sorted(IntVector({1,2,1})), false);
    EXPECT_EQ(is_sorted_on(int_mod_10, IntVector({51,32,43})), true);
    EXPECT_EQ(is_sorted_on(int_mod_10, IntVector({15,23})), false);
    EXPECT_EQ(is_strictly_sorted(IntVector({1,2,3})), true);
    EXPECT_EQ(is_strictly_sorted(IntVector({1,2,2})), false);
    EXPECT_EQ(is_strictly_sorted(IntVector({1,2,1})), false);
    EXPECT_EQ(is_strictly_sorted_on(int_mod_10, IntVector({51,32,43})), true);
    EXPECT_EQ(is_strictly_sorted_on(int_mod_10, IntVector({51,32,43,63})), false);
    EXPECT_EQ(is_strictly_sorted_on(int_mod_10, IntVector({15,23})), false);
}

TEST(container_common_test, find)
{
    using namespace fplus;

    EXPECT_EQ(find_first_by(is3, xs), just(3));
    EXPECT_EQ(find_first_by(is4, xs), nothing<int>());
    EXPECT_EQ(find_first_idx_by(is2, xs), just<std::size_t>(1));
    EXPECT_EQ(find_first_idx_by(is4, xs), nothing<std::size_t>());
    EXPECT_EQ(find_first_idx(2, xs), just<std::size_t>(1));
    EXPECT_EQ(find_first_idx(4, xs), nothing<std::size_t>());

    EXPECT_EQ(find_last_by(is3, xs), just(3));
    EXPECT_EQ(find_last_by(is4, xs), nothing<int>());
    EXPECT_EQ(find_last_idx_by(is2, xs), just<std::size_t>(4));
    EXPECT_EQ(find_last_idx_by(is4, xs), nothing<std::size_t>());
    EXPECT_EQ(find_last_idx(2, xs), just<std::size_t>(4));
    EXPECT_EQ(find_last_idx(4, xs), nothing<std::size_t>());
}

TEST(container_common_test, nth_element)
{
    using namespace fplus;
    EXPECT_EQ(nth_element<IntVector>(2)(xs), 2);
    EXPECT_EQ(nth_element_flipped(xs)(2), 2);
}

TEST(container_common_test, pairs)
{
    using namespace fplus;
    IntPair intPair = std::make_pair(2, 3);
    IntPairs intPairs = {{1,2}, {3,4}};
    IntPairs intPairsSwapped = {{2,1}, {4,3}};
    EXPECT_EQ(fst(intPair), 2);
    EXPECT_EQ(snd(intPair), 3);
    EXPECT_EQ(swap_pair_elems(intPair), std::make_pair(3, 2));
    EXPECT_EQ(swap_pairs_elems(intPairs), intPairsSwapped);
    EXPECT_EQ(transform_fst(squareLambda, intPair), std::make_pair(4, 3));
    EXPECT_EQ(transform_snd(squareLambda, intPair), std::make_pair(2, 9));

    typedef std::vector<std::pair<std::string, int>> StringIntPairs;
    StringIntPairs stringIntPairs = {{"a", 1}, {"a", 2}, {"b", 6}, {"a", 4}};
    auto stringIntPairsAsMapGrouped = pairs_to_map_grouped(stringIntPairs);
    auto groupNameToMedianMap = transform_map_values(median<std::vector<int>>, stringIntPairsAsMapGrouped);
    auto groupNames = transform(fst<std::string, int>, stringIntPairs);
    auto getMedianValue = bind_1st_and_2nd_of_3(get_from_map_with_def<std::map<std::string, int>>, groupNameToMedianMap, 0);
    auto groupMendianValues = transform(getMedianValue, groupNames);
    auto stringIntPairsSndReplacedWithGroupMedian = zip(groupNames, groupMendianValues);
    EXPECT_EQ(stringIntPairsSndReplacedWithGroupMedian, StringIntPairs({{"a", 2}, {"a", 2}, {"b", 6}, {"a", 2}}));
}

TEST(container_common_test, is_elem_of)
{
    using namespace fplus;
    EXPECT_EQ(is_elem_of(2, xs), true);
    EXPECT_EQ(is_elem_of(4, xs), false);
}

TEST(container_common_test, find_token)
{
    using namespace fplus;
    EXPECT_EQ(find_all_instances_of_token(std::string("Plus"),
        std::string("C Plus Plus is a nice language,") +
        std::string(" and FunctionalPlus makes it even nicer.")),
        std::vector<std::size_t>({ 2, 7, 46 }));
    EXPECT_EQ(find_all_instances_of_token(std::string("xx"), std::string("bxxxxc")), std::vector<std::size_t>({ 1, 2, 3 }));
    EXPECT_EQ(find_all_instances_of_token(std::string("xy"), std::string("xyaaa")), std::vector<std::size_t>({ 0 }));
    EXPECT_EQ(find_all_instances_of_token(std::string("xy"), std::string("aaaxy")), std::vector<std::size_t>({ 3 }));
    EXPECT_EQ(find_all_instances_of_token(std::string("xx"), std::string("xxxxx")), std::vector<std::size_t>({ 0, 1, 2, 3 }));
    EXPECT_EQ(find_all_instances_of_token_non_overlapping(std::string("xx"), std::string("xxxx")), std::vector<std::size_t>({ 0, 2 }));
    EXPECT_EQ(find_all_instances_of_token(IntVector({}), IntVector({})), IdxVector({0}));
    EXPECT_EQ(find_all_instances_of_token(IntVector({}), IntVector({1})), IdxVector({0,1}));
    EXPECT_EQ(find_all_instances_of_token_non_overlapping(IntVector({}), IntVector({})), IdxVector({0}));
    EXPECT_EQ(find_all_instances_of_token_non_overlapping(IntVector({}), IntVector({1})), IdxVector({0,1}));
    EXPECT_EQ(find_first_instance_of_token(IntVector({}), IntVector({})), just<std::size_t>(0));
    EXPECT_EQ(find_first_instance_of_token(IntVector({}), IntVector({1,2})), just<std::size_t>(0));
    EXPECT_EQ(find_first_instance_of_token(std::string("haha"), std::string("oh, hahaha!")), just<std::size_t>(4));
}

TEST(container_common_test, split)
{
    using namespace fplus;
    EXPECT_EQ(split_by_token(IntVector({}), false, IntVector({1,2})), IntVectors({{1},{2}}));
    EXPECT_EQ(split_by_token(IntVector({}), true, IntVector()), IntVectors({{},{}}));
    EXPECT_EQ(split_by_token(IntVector({}), true, IntVector({1,2})), IntVectors({{},{1},{2},{}}));

    EXPECT_EQ(split_at_idx(2, xs), std::make_pair(IntVector({1,2}), IntVector({2,3,2})));
    EXPECT_EQ(partition(is_even, xs), std::make_pair(IntVector({2,2,2}), IntVector({1,3})));
    EXPECT_EQ(split_every(2, xs), IntVectors({{1,2}, {2, 3}, {2}}));
    auto splittedAt1And3 = split_at_idxs(IdxVector({1,3}), xs);
    IntVectors splittedAt1And3Dest = {IntVector({1}), IntVector({2,2}), IntVector({3,2})};
    EXPECT_EQ(splittedAt1And3, splittedAt1And3Dest);
    auto splittedAt1And3And3 = split_at_idxs(IdxVector({1,3,3}), xs);
    IntVectors splittedAt1And3And3Dest = {IntVector({1}), IntVector({2,2}), IntVector({}), IntVector({3,2})};
    EXPECT_EQ(splittedAt1And3And3, splittedAt1And3And3Dest);
    EXPECT_EQ(split(3, true, xs), IntVectors({IntVector({1, 2, 2}), IntVector({2})}));
    EXPECT_EQ(split(1, true, IntVector{0,1,2}), IntVectors({{0},{2}}));
    EXPECT_EQ(split(2, true, IntVector{5,2,0,3}), IntVectors({{5},{0,3}}));
    EXPECT_EQ(split(2, true, IntVector{2}), IntVectors({{},{}}));
    EXPECT_EQ(split_by(is_even, true, IntList({1,3,2,2,5,5,3,6,7,9})), IntLists({{1,3},{},{5,5,3},{7,9}}));
    EXPECT_EQ(split_by_keep_separators(is_even, IntList({})), IntLists());
    EXPECT_EQ(split_by_keep_separators(is_even, IntList({2})), IntLists({IntList({2})}));
    EXPECT_EQ(split_by_keep_separators(is_even, IntList({2,2,3})), IntLists({{2},{2,3}}));
    EXPECT_EQ(split_by_keep_separators(is_even, IntList({1,3,2})), IntLists({{1,3},{2}}));
    EXPECT_EQ(split_by_keep_separators(is_even, IntList({1,3,2,2,5,5,3,6,7,9})), IntLists({{1,3},{2},{2,5,5,3},{6,7,9}}));
}

TEST(container_common_test, enumerate)
{
    using namespace fplus;
    EXPECT_EQ(enumerate(xs), (std::vector<std::pair<std::size_t, int>>({{0,1}, {1,2}, {2,2}, {3,3}, {4,2}})));
}

TEST(container_common_test, range)
{
    using namespace fplus;
    IntList v789 = { 7,8,9 };
    EXPECT_EQ(set_range(1, v789, intList), IntList({ 1,7,8,9,2 }));
    EXPECT_EQ(get_range(1, 4, intList), IntList({ 2,2,3 }));
    EXPECT_EQ(replace_elems(2, 5, xs), IntVector({1,5,5,3,5}));
    EXPECT_EQ(replace_tokens(std::string("123"), std::string("_"), std::string("--123----123123")), std::string("--_----__"));
    EXPECT_EQ(take(2, xs), IntVector({ 1,2 }));
    EXPECT_EQ(drop(2, xs), IntVector({ 2,3,2 }));
    EXPECT_EQ(take(999, xs), xs);
    EXPECT_EQ(drop(999, xs), IntVector());
    EXPECT_EQ(take_while(is_odd, xs), IntVector({ 1 }));
    EXPECT_EQ(drop_while(is_odd, xs), IntVector({ 2,2,3,2 }));
    EXPECT_EQ(span(is_odd, xs), std::make_pair(IntVector({ 1 }), IntVector({ 2,2,3,2 })));
    EXPECT_EQ(replace_range(2, IntVector({8,9}), xs), IntVector({1,2,8,9,2}));
}

TEST(container_common_test, )
{
    using namespace fplus;
    EXPECT_EQ(keep_if(is2, xs), IntVector({ 2,2,2 }));
    EXPECT_EQ(keep_if(is3, xs), IntVector({ 3 }));
    EXPECT_EQ(keep_if(is4, xs), IntVector());
}

TEST(container_common_test, find_all_idxs_of)
{
    using namespace fplus;
    EXPECT_EQ(find_all_idxs_of('h', std::string("oh, ha!")), std::vector<std::size_t>({ 1, 4 }));
    EXPECT_EQ(find_all_idxs_of(2, xs), IdxVector({ 1,2,4 }));
}

TEST(container_common_test, count)
{
    using namespace fplus;
    EXPECT_EQ(count(2, xs), 3);
}

TEST(container_common_test, infix)
{
    using namespace fplus;
    EXPECT_EQ(is_infix_of(IntVector({}), IntVector({})), true);
    EXPECT_EQ(is_infix_of(IntVector({}), IntVector({1,2})), true);
    EXPECT_EQ(is_infix_of(IntVector({2,3}), xs), true);
    EXPECT_EQ(is_infix_of(IntVector({2,3}), xs), true);
    EXPECT_EQ(is_infix_of(IntVector({2,1}), xs), false);
    EXPECT_EQ(is_prefix_of(IntVector({ 1,2 }), xs), true);
    EXPECT_EQ(is_prefix_of(IntVector({ 2,2 }), xs), false);
    EXPECT_EQ(is_suffix_of(IntVector({ 3,2 }), xs), true);
    EXPECT_EQ(is_suffix_of(IntVector({ 2,2 }), xs), false);
}

TEST(container_common_test, subsequence)
{
    using namespace fplus;
    EXPECT_EQ(is_subsequence_of(IntVector(), IntVector()), true);
    EXPECT_EQ(is_subsequence_of(IntVector(), xs), true);
    EXPECT_EQ(is_subsequence_of(IntVector({ 1,3 }), xs), true);
    EXPECT_EQ(is_subsequence_of(IntVector({ 3,1 }), xs), false);
}

TEST(container_common_test, transpose)
{
    using namespace fplus;
    typedef std::vector<IntVector> IntGrid2d;
    EXPECT_EQ(transpose(IntGrid2d({})), IntGrid2d({}));
    EXPECT_EQ(transpose(IntGrid2d({ { 1, 2 } })), IntGrid2d({ { 1 }, { 2 } }));
    EXPECT_EQ(transpose(IntGrid2d({ { 1, 2 }, { 3, 4 } })), IntGrid2d({ { 1, 3 }, { 2, 4 } }));
    EXPECT_EQ(transpose(IntGrid2d({ { 1, 2, 3 }, { 4, 5, 6 } })), IntGrid2d({ { 1, 4 }, { 2, 5 }, { 3, 6 } }));
}

TEST(container_common_test, sample)
{
    using namespace fplus;
    EXPECT_EQ(sample(3, xs).size(), 3);
}

TEST(container_common_test, generate)
{
    using namespace fplus;
    int countUpCounter = 0;
    auto countUp = [countUpCounter]() mutable { return countUpCounter++; };
    EXPECT_EQ(generate<IntVector>(countUp, 3), IntVector({ 0,1,2 }));
    auto square_size_t_return_int = [&](std::size_t x) -> int
    {
        return squareLambda(static_cast<int>(x));
    };
    EXPECT_EQ(generate_by_idx<IntVector>(square_size_t_return_int, 3), IntVector({ 0,1,4 }));
}

TEST(container_common_test, nub)
{
    using namespace fplus;
    EXPECT_EQ(nub(xs), IntVector({ 1,2,3 }));
    auto bothEven = is_equal_by(is_even);
    EXPECT_EQ(nub_by(bothEven, xs), IntVector({ 1,2 }));
    EXPECT_EQ(nub_on(int_mod_10, IntVector({12,32,15})), IntVector({12,15}));
}

TEST(container_common_test, map)
{
    using namespace fplus;
    typedef std::map<int, std::string> IntStringMap;
    typedef std::map<std::string, int> StringIntMap;
    IntStringMap intStringMap = {{1, "2"}, {4, "53"}, {7, "21"}};
    StringIntMap stringIntMap = {{ "2", 1}, { "53", 4}, { "21", 7}};
    EXPECT_EQ(swap_keys_and_values(intStringMap), stringIntMap);

    typedef std::vector<std::string> StringVector;
    EXPECT_EQ(get_map_keys(intStringMap), IntVector({1, 4, 7}));
    EXPECT_EQ(get_map_values(intStringMap), StringVector({"2", "53", "21"}));

    typedef std::unordered_map<int, std::string> IntStringUnorderedMap;
    typedef std::unordered_map<std::string, int> StringIntUnorderedMap;
    IntStringUnorderedMap intStringUnorderedMap = { { 1, "2" },{ 4, "53" },{ 7, "21" } };
    StringIntUnorderedMap stringIntUnorderedMapSwapped = { { "2", 1 },{ "53", 4 },{ "21", 7 } };
    EXPECT_EQ(swap_keys_and_values(intStringUnorderedMap), stringIntUnorderedMapSwapped);
    EXPECT_EQ(convert_container<IntStringUnorderedMap>(intStringMap), intStringUnorderedMap);
    EXPECT_EQ(convert_container<IntStringMap>(intStringUnorderedMap), intStringMap);

    std::vector<int> mapInts = { 1, 4, 7 };
    std::vector<std::string> mapStrings = { "2", "53", "21" };
    EXPECT_EQ(create_map(mapInts, mapStrings), intStringMap);
    EXPECT_EQ(create_unordered_map(mapInts, mapStrings), intStringUnorderedMap);

    EXPECT_EQ(get_from_map(intStringMap, 1), just<std::string>("2"));
    EXPECT_EQ(get_from_map(intStringMap, 9), nothing<std::string>());
    EXPECT_EQ(get_from_map_with_def(intStringMap, std::string("n/a"), 1), "2");
    EXPECT_EQ(get_from_map_with_def(intStringMap, std::string("n/a"), 9), "n/a");
    EXPECT_EQ(map_contains(intStringMap, 1), true);
    EXPECT_EQ(map_contains(intStringMap, 9), false);

    IntStringMap union_map_1 = {{0, "a"}, {1, "b"}};
    IntStringMap union_map_2 = {{0, "c"}, {2, "d"}};
    IntStringMap union_map_res = {{0, "a"}, {1, "b"}, {2, "d"}};
    IntStringMap union_map_with_res = {{0, "ac"}, {1, "b"}, {2, "d"}};
    EXPECT_EQ(map_union(union_map_1, union_map_2), union_map_res);
    EXPECT_EQ(map_union_with(concat<std::vector<std::string>>, union_map_1, union_map_2), union_map_with_res);
}

TEST(container_common_test, set)
{
    using namespace fplus;
    typedef std::set<int> IntSet;
    IntSet intSet1 = {0,1,2,3};
    IntSet intSet2 = {2,3,4,5};
    IntSet intSet3 = {0,2};
    IntSet intSet4 = {2,3};
    IntSet intSet5 = {0,1};
    IntSet intSet6 = {0,1,2,3,4,5};
    IntSet intSet7 = {0,1,4,5};
    EXPECT_EQ(set_includes(intSet1, intSet3), true);
    EXPECT_EQ(set_includes(intSet3, intSet1), false);
    EXPECT_EQ(set_includes(intSet1, intSet2), false);
    EXPECT_EQ(set_merge(intSet1, intSet2), intSet6);
    EXPECT_EQ(set_merge(intSet1, intSet3), intSet1);
    EXPECT_EQ(set_intersection(intSet1, intSet2), intSet4);
    EXPECT_EQ(set_difference(intSet1, intSet2), intSet5);
    EXPECT_EQ(set_symmetric_difference(intSet1, intSet2), intSet7);
}

TEST(container_common_test, count_occurrences)
{
    using namespace fplus;
    typedef std::map<int, std::size_t> IntSizeTMap;
    IntSizeTMap OccurrencesResult = {{1, 1}, {2, 3}, {3, 1}};
    EXPECT_EQ(count_occurrences(xs), OccurrencesResult);
}

TEST(container_common_test, insert_at)
{
    using namespace fplus;
    EXPECT_EQ(insert_at(2, IntVector({8,9}), xs), IntVector({1,2,8,9,2,3,2}));
}

TEST(container_common_test, tail)
{
    using namespace fplus;
    EXPECT_EQ(init(xs), IntVector({1,2,2,3}));
    EXPECT_EQ(tail(xs), IntVector({2,2,3,2}));

    EXPECT_EQ(inits(xs), IntVectors({{},{1},{1,2},{1,2,2},{1,2,2,3},{1,2,2,3,2}}));
    EXPECT_EQ(tails(xs), IntVectors({{1,2,2,3,2},{2,2,3,2},{2,3,2},{3,2},{2},{}}));
}

TEST(container_common_test, iterate)
{
    using namespace fplus;
    auto times_two = [](int x) { return 2*x; };
    EXPECT_EQ(iterate(times_two, 0, 3), IntVector({}));
    EXPECT_EQ(iterate(times_two, 1, 3), IntVector({3}));
    EXPECT_EQ(iterate(times_two, 2, 3), IntVector({3,6}));
    EXPECT_EQ(iterate(times_two, 5, 3), IntVector({3,6,12,24,48}));
}

TEST(container_common_test, is_permutation_of)
{
    using namespace fplus;
    EXPECT_TRUE(is_permutation_of(IntVector({2,3,1}), IntVector({1,2,3})));
    EXPECT_FALSE(is_permutation_of(IntVector({2,3,2}), IntVector({1,2,3})));
    EXPECT_FALSE(is_permutation_of(IntVector({2,3}), IntVector({1,2,3})));
    EXPECT_FALSE(is_permutation_of(IntVector({2,3,1}), IntVector({1,23})));
}
