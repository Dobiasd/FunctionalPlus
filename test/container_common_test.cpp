// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "fplus/fplus.hpp"
#include <vector>

namespace {
    auto squareLambda = [](int x) -> int { return x*x; };
    auto is_even_int = [](int x){ return x % 2 == 0; };
    auto is_even_size_t = [](std::size_t x){ return x % 2 == 0; };
    auto is_odd_int = [](int x){ return x % 2 == 1; };
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

TEST_CASE("container_common_test, group")
{
    using namespace fplus;
    REQUIRE_EQ(group(intList), intLists);
    REQUIRE_EQ(group(xs), IntVectors({IntVector({1}),IntVector({2,2}),IntVector({3}),IntVector({2})}));
    REQUIRE_EQ(group_on(int_mod_10, IntVector({12,22,34})), IntVectors({IntVector({12,22}),IntVector({34})}));
    REQUIRE_EQ(group_globally(xs), IntVectors({IntVector({1}),IntVector({2,2,2}),IntVector({3})}));
    REQUIRE_EQ(group_globally_on(int_mod_10, IntVector({12,34,22})), IntVectors({IntVector({12,22}),IntVector({34})}));
    REQUIRE_EQ(group_by(abs_diff_less_or_equal_3, IntVector({2,3,6,4,22,21,8,5})), IntVectors({{2,3,6,4},{22,21},{8,5}}));
}

TEST_CASE("container_common_test, singleton_seq")
{
    using namespace fplus;
    REQUIRE_EQ(singleton_seq(3), IntVector({3}));
}

TEST_CASE("container_common_test, transform")
{
    using namespace fplus;

    auto intTimes2 = [](int x) -> int { return x*2; };
    auto intTimes3 = [](int x) -> int { return x*3; };

    // The following works with C++14.
    // REQUIRE_EQ(transform([](auto x) { return x*x; }, xs), IntVector({1,4,4,9,4}));
    std::initializer_list<int> initListInts = { 1,2,2,3,2 };
    REQUIRE_EQ(transform(squareLambda, std::vector<int>(initListInts)), IntVector({1,4,4,9,4}));
    REQUIRE_EQ(transform_convert<std::vector<int>>(squareLambda, initListInts), IntVector({1,4,4,9,4}));
    REQUIRE_EQ(transform_convert<std::vector<int>>(squareLambda, xs_array), IntVector({1,4,4,9,4}));
    REQUIRE_EQ(transform(squareLambda, xs), IntVector({1,4,4,9,4}));
    REQUIRE_EQ(transform(squareLambda, intList), IntList({ 1,4,4,9,4 }));
    REQUIRE_EQ(transform_parallelly(squareLambda, intList), IntList({ 1,4,4,9,4 }));
    REQUIRE_EQ(transform_parallelly_n_threads(3, squareLambda, intList), IntList({ 1,4,4,9,4 }));
    REQUIRE_EQ(transform_convert<IntList>(squareLambda, xs), IntList({ 1,4,4,9,4 }));

    REQUIRE_EQ(transform(squareLambda, std::set<int>({1,2,3,-3})), std::set<int>({1,4,9}));
    REQUIRE_EQ(transform_inner(intTimes2, IntVectors({{1,3,4},{1,2}})), IntVectors({{2,6,8},{2,4}}));

    auto add_size_t_and_int = [](std::size_t a, int b) -> int
    {
        return static_cast<int>(a) + b;
    };
    REQUIRE_EQ(transform_with_idx(add_size_t_and_int, xs), IntVector({1+0,2+1,2+2,3+3,2+4}));

    std::vector<std::function<int(int)>> multiplyFunctions = {intTimes2, intTimes3};
    REQUIRE_EQ(apply_functions(multiplyFunctions, 4), IntVector({8, 12}));

    auto showInt = [](int x) -> std::string { return fplus::show(x); };
    std::vector<std::function<std::string(int)>> showIntFuncs = {showInt, showInt};
    REQUIRE_EQ(apply_functions(showIntFuncs, 4), std::vector<std::string>({"4", "4"}));

    REQUIRE_EQ(apply_function_n_times(squareLambda, 3, 2), 256);
}

TEST_CASE("container_common_test, filter")
{
    using namespace fplus;
    REQUIRE_EQ(keep_if(is_even_size_t, xs), IntVector({2,2,2}));
    REQUIRE_EQ(drop_if(is_even_size_t, xs), IntVector({1,3}));
    REQUIRE_EQ(keep_if(is_even_size_t, intList), IntList({ 2,2,2 }));
    REQUIRE_EQ(drop_if(is_even_size_t, intList), IntList({ 1,3 }));
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

TEST_CASE("container_common_test, trim")
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

TEST_CASE("container_common_test, cluster")
{
    using namespace fplus;
    REQUIRE_EQ(cluster_by(abs_diff_less_or_equal_3, IntVector({2,3,6,4,12,11,20,23,8,4})), IntVectors({{2,3,6,4,12,11,8,4},{20,23}}));
}

TEST_CASE("container_common_test, run_length_encode")
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

TEST_CASE("container_common_test, keep_idxs")
{
    using namespace fplus;
    REQUIRE_EQ(keep_idxs(IdxVector({1, 3}), xs), IntVector({2,3}));
    REQUIRE_EQ(keep_idxs(IdxVector({3, 1}), xs), IntVector({2,3}));
    REQUIRE_EQ(keep_idxs(IdxVector({1, 1, 3}), xs), IntVector({2,3}));
    REQUIRE_EQ(keep_idxs(IdxVector({1, 3, 7}), xs), IntVector({2,3}));
    REQUIRE_EQ(drop_idxs(IdxVector({1, 3}), xs), IntVector({1,2,2}));
}

TEST_CASE("container_common_test, is_equal")
{
    using namespace fplus;
    REQUIRE_EQ(is_equal_by_and_by(is_even_int, is_even_int)(2, 4), true);
    REQUIRE_EQ(is_equal_by_and_by(is_even_int, is_even_int)(1, 2), false);
    REQUIRE_EQ(is_equal_by_and_by(is_odd_int, is_even_int)(1, 2), true);
    REQUIRE_EQ(is_equal_to(2)(2), true);
    REQUIRE_EQ(is_equal_to(1)(2), false);
}

TEST_CASE("container_common_test, is_empty")
{
    using namespace fplus;
    REQUIRE_EQ(is_empty(xs), false);
    REQUIRE_EQ(is_empty(IntVector()), true);
    REQUIRE_EQ(is_not_empty(xs), true);
    REQUIRE_EQ(is_not_empty(IntVector()), false);
}

TEST_CASE("container_common_test, convert")
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

TEST_CASE("container_common_test, append_elem")
{
    using namespace fplus;
    REQUIRE_EQ(append_elem(IntVector({1,2}), 3), IntVector({1,2,3}));
}

TEST_CASE("container_common_test, prepend_elem")
{
    using namespace fplus;
    REQUIRE_EQ(prepend_elem(IntVector({2,3}), 1), IntVector({1,2,3}));
}

TEST_CASE("container_common_test, append")
{
    using namespace fplus;
    REQUIRE_EQ(append(xs, xs), xs2Times);
}

TEST_CASE("container_common_test, interweave")
{
    using namespace fplus;
    REQUIRE_EQ(interweave(IntVector({1,3}), IntVector({2,4})), IntVector({1,2,3,4}));
    REQUIRE_EQ(interweave(IntVector({1,3,5,7}), IntVector({2,4})), IntVector({1,2,3,4,5,7}));
    REQUIRE_EQ(unweave(IntVector({0,1,2,3})), std::make_pair(IntVector({0,2}), IntVector({1,3})));
    REQUIRE_EQ(unweave(IntVector({0,1,2,3,4})), std::make_pair(IntVector({0,2,4}), IntVector({1,3})));
}

TEST_CASE("container_common_test, concat")
{
    using namespace fplus;
    REQUIRE_EQ(concat(intLists), intList);
    REQUIRE_EQ(concat(IntVectors(2, xs)), xs2Times);
}

TEST_CASE("container_common_test, repeat")
{
    using namespace fplus;
    REQUIRE_EQ(repeat(2, xs), xs2Times);
}

TEST_CASE("container_common_test, replicate")
{
    using namespace fplus;
    REQUIRE_EQ(replicate(2, xs), IntVectors({xs, xs}));
}

TEST_CASE("container_common_test, replicate_elems")
{
    using namespace fplus;
    REQUIRE_EQ(replicate_elems(2, xs), IntVector({1,1,2,2,2,2,3,3,2,2}));
}

TEST_CASE("container_common_test, infixes")
{
    using namespace fplus;
    REQUIRE_EQ(infixes(3, xs), IntVectors({
        IntVector({1, 2, 2}),
        IntVector({2, 2, 3}),
        IntVector({2, 3, 2})}));
}

TEST_CASE("container_common_test, carthesian_product")
{
    using namespace fplus;
    typedef std::pair<std::string::value_type, std::string::value_type> char_pair;
    typedef std::vector<char_pair> char_pair_vec;
    auto twoCharsToString = [](std::string::value_type x, std::string::value_type y) { std::string result; result += x; result += y; return result; };
    auto alwaysTrueCharAndChar = [](std::string::value_type, std::string::value_type) { return true; };
    REQUIRE_EQ(carthesian_product_with(twoCharsToString, ABC, XY), string_vec({"AX", "AY", "BX", "BY", "CX", "CY"}));
    REQUIRE_EQ(carthesian_product_where(alwaysTrueCharAndChar, ABC, XY), char_pair_vec({{'A','X'}, {'A','Y'}, {'B','X'}, {'B','Y'}, {'C','X'}, {'C','Y'}}));
    auto charAndCharSumIsEven = [&](std::string::value_type x, std::string::value_type y) { return is_even_int(x + y); };
    REQUIRE_EQ(carthesian_product_with_where(twoCharsToString, charAndCharSumIsEven, ABC, XY), string_vec({"AY", "BX", "CY"}));
    REQUIRE_EQ(carthesian_product_where(charAndCharSumIsEven, ABC, XY), char_pair_vec({{'A','Y'}, {'B','X'}, {'C','Y'}}));
    REQUIRE_EQ(carthesian_product(ABC, XY), char_pair_vec({{'A','X'}, {'A','Y'}, {'B','X'}, {'B','Y'}, {'C','X'}, {'C','Y'}}));
    REQUIRE_EQ(carthesian_product_n(2, ABCD), string_vec({"AA", "AB", "AC", "AD", "BA", "BB", "BC", "BD", "CA", "CB", "CC", "CD", "DA", "DB", "DC", "DD"}));
    REQUIRE_EQ(carthesian_product_n(2, vec0123), IntVectors({{0,0}, {0,1}, {0,2}, {0,3}, {1,0}, {1,1}, {1,2}, {1,3}, {2,0}, {2,1}, {2,2}, {2,3}, {3,0}, {3,1}, {3,2}, {3,3}}));
    REQUIRE_EQ(carthesian_product_n(0, vec0123), IntVectors({IntVector()}));
}

TEST_CASE("container_common_test, combination")
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

TEST_CASE("container_common_test, permutations")
{
    using namespace fplus;
    REQUIRE_EQ(permutations(2, ABCD), string_vec({"AB", "AC", "AD", "BA", "BC", "BD", "CA", "CB", "CD", "DA", "DB", "DC"}));
    REQUIRE_EQ(permutations(0, vec0123), IntVectors({IntVector()}));
}

TEST_CASE("container_common_test, power_set")
{
    using namespace fplus;
    REQUIRE_EQ(power_set(std::string("xyz")), string_vec({"", "x", "y", "z", "xy", "xz", "yz", "xyz"}));
}

TEST_CASE("container_common_test, rotations")
{
    using namespace fplus;
    REQUIRE_EQ(rotations_left(std::string("abcd")), string_vec({"abcd", "bcda", "cdab", "dabc"}));
    REQUIRE_EQ(rotations_right(std::string("abcd")), string_vec({"abcd", "dabc", "cdab", "bcda"}));
}

TEST_CASE("container_common_test, fill")
{
    using namespace fplus;
    REQUIRE_EQ(fill_left(0, 6, IntVector({1,2,3,4})), IntVector({0,0,1,2,3,4}));
    REQUIRE_EQ(fill_right(0, 6, IntList({1,2,3,4})), IntList({1,2,3,4,0,0}));
    REQUIRE_EQ(fill_left(' ', 6, std::string("12")), std::string("    12"));
}

TEST_CASE("container_common_test, intersperse")
{
    using namespace fplus;
    REQUIRE_EQ(intersperse(0, xs), IntVector({1,0,2,0,2,0,3,0,2}));
}

TEST_CASE("container_common_test, fold")
{
    using namespace fplus;
    REQUIRE_EQ(fold_left(std::plus<int>(), 100, xs), 110);
    REQUIRE_EQ(fold_left_1(std::plus<int>(), xs), 10);
    REQUIRE_EQ(fold_right(std::plus<int>(), 100, xs), 110);
    REQUIRE_EQ(fold_right_1(std::plus<int>(), xs), 10);
    auto appendXToStrForFoldL = [](const std::string& str, int x) { return str + std::to_string(x); };
    auto appendXToStrForFoldR = [](int x, const std::string& str) { return str + std::to_string(x); };
    std::string emptyString;
    REQUIRE_EQ(fold_left(appendXToStrForFoldL, emptyString, xs), "12232");
    REQUIRE_EQ(fold_right(appendXToStrForFoldR, emptyString, xs), "23221");
}

TEST_CASE("container_common_test, scan")
{
    using namespace fplus;
    REQUIRE_EQ(scan_left(std::plus<int>(), 20, xs), IntVector({ 20,21,23,25,28,30 }));
    REQUIRE_EQ(scan_right(std::plus<int>(), 20, xs), IntVector({ 30,29,27,25,22,20 }));
    REQUIRE_EQ(scan_left_1(std::plus<int>(), xs), IntVector({ 1,3,5,8,10 }));
    REQUIRE_EQ(scan_right_1(std::plus<int>(), xs), IntVector({ 10,9,7,5,2 }));
}

TEST_CASE("container_common_test, join")
{
    using namespace fplus;
    REQUIRE_EQ(join(std::string(", "), std::vector<std::string>({"a", "b", "sea"})), std::string("a, b, sea"));
    REQUIRE_EQ(join(IntList({0}), intLists), IntList({1,0,2,2,0,3,0,2}));
}

TEST_CASE("container_common_test, show")
{
    using namespace fplus;
    std::map<int, std::string> mapToShow = {{1, "one"}, {2, "two"}};
    REQUIRE_EQ(show_cont(mapToShow), "[(1, one), (2, two)]");
    REQUIRE_EQ(show_cont(xs), xsShown);
    REQUIRE_EQ(show_cont_with(", ", xs), xsShown);
    std::string xsShownNLs = "(1,2,\n"
                             " 2,3,\n"
                             " 2)";
    REQUIRE_EQ(show_cont_with_frame_and_newlines(",", "(", ")", xs, 2), xsShownNLs);
    REQUIRE_EQ(show<int>(1), "1");

    REQUIRE_EQ(show(std::vector<std::vector<int>>({{1,2,3},{4,5,6}})), "[[1, 2, 3], [4, 5, 6]]");
}

TEST_CASE("container_common_test, zip_with")
{
    using namespace fplus;
    const auto multiply = [](int x, int y){ return x * y; };
    REQUIRE_EQ(zip_with(multiply, xs, xs), transform(squareLambda, xs));
    const auto add = [](int x, int y){ return x + y; };
    REQUIRE_EQ(zip_with(add, IntVector({1,2,3}), IntVector({1,2})), IntVector({2,4}));
    REQUIRE_EQ(zip_with(add, IntVector({1,2}), IntVector({1,2,3})), IntVector({2,4}));
}

TEST_CASE("container_common_test, zip_with_defaults")
{
    using namespace fplus;
    const auto add = [](int x, int y){ return x + y; };
    REQUIRE_EQ(zip_with_defaults(add, 6, 7, IntVector({1,2,3}), IntVector({1,2})), IntVector({2,4,10}));
    REQUIRE_EQ(zip_with_defaults(add, 6, 7, IntVector({1,2}), IntVector({1,2,3})), IntVector({2,4,9}));
}

TEST_CASE("container_common_test, show_float")
{
    using namespace fplus;
    const double pi = 3.14159;
    REQUIRE_EQ(show_float<double>(0, 3, pi), "3.142");
    REQUIRE_EQ(show_float<double>(1, 3, pi), "3.142");
    REQUIRE_EQ(show_float<double>(2, 3, pi), "03.142");
    REQUIRE_EQ(show_float<double>(3, 3, pi), "003.142");
    REQUIRE_EQ(show_float<double>(1, 2, pi), "3.14");
    REQUIRE_EQ(show_float<double>(1, 4, pi), "3.1416");
    REQUIRE_EQ(show_float<double>(1, 7, pi), "3.1415900");
    REQUIRE_EQ(show_float<double>(0, 3, -pi), "-3.142");
    REQUIRE_EQ(show_float<double>(1, 3, -pi), "-3.142");
    REQUIRE_EQ(show_float<double>(2, 3, -pi), "-3.142");
    REQUIRE_EQ(show_float<double>(3, 3, -pi), "-03.142");
    REQUIRE_EQ(show_float<double>(4, 3, -pi), "-003.142");
    REQUIRE_EQ(show_float<double>(0, 3, 0.142), "0.142");
    REQUIRE_EQ(show_float<double>(1, 3, 0.142), "0.142");
    REQUIRE_EQ(show_float<double>(2, 3, 0.142), "00.142");
    REQUIRE_EQ(fill_left(' ', 8, show_float<double>(0, 3, -pi)), "  -3.142");

    REQUIRE_EQ(show_float_fill_left<double>(' ', 8, 3, pi), "   3.142");
    REQUIRE_EQ(show_float_fill_left<double>(' ', 8, 6, pi), "3.141590");
    REQUIRE_EQ(show_float_fill_left<double>(' ', 8, 3, -pi), "  -3.142");
    REQUIRE_EQ(show_float_fill_left<double>(' ', 2, 3, -pi), "-3.142");

    REQUIRE_EQ(show_fill_left<int>(' ', 4, 3), "   3");
    REQUIRE_EQ(show_fill_left<int>('0', 4, 3), "0003");
    REQUIRE_EQ(show_fill_left<int>(' ', 4, 12345), "12345");

    REQUIRE_EQ(show_fill_right<int>(' ', 4, 3), "3   ");
    REQUIRE_EQ(show_fill_right<int>(' ', 4, 12345), "12345");
}

TEST_CASE("container_common_test, zip")
{
    using namespace fplus;
    auto xsZippedWithXs = zip(xs, xs);
    REQUIRE_EQ(unzip(xsZippedWithXs).first, xs);
}

TEST_CASE("container_common_test, all")
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

TEST_CASE("container_common_test, any")
{
    using namespace fplus;
    REQUIRE_EQ(any(BoolVector()), false);
    REQUIRE_EQ(any(BoolVector({true})), true);
    REQUIRE_EQ(any(BoolVector({false})), false);
    REQUIRE_EQ(any(BoolVector({false, false})), false);
    REQUIRE_EQ(any(BoolVector({true, false})), true);

    REQUIRE_EQ(any_by(is_even_int, IntVector()), false);
    REQUIRE_EQ(any_by(is_even_int, IntVector({2})), true);
    REQUIRE_EQ(any_by(is_even_int, IntVector({1})), false);
    REQUIRE_EQ(any_by(is_even_int, IntVector({1, 1})), false);
    REQUIRE_EQ(any_by(is_even_int, IntVector({2, 1})), true);
}

TEST_CASE("container_common_test, none")
{
    using namespace fplus;
    REQUIRE_EQ(none(BoolVector()), true);
    REQUIRE_EQ(none(BoolVector({true})), false);
    REQUIRE_EQ(none(BoolVector({false})), true);
    REQUIRE_EQ(none(BoolVector({false, false})), true);
    REQUIRE_EQ(none(BoolVector({true, false})), false);

    REQUIRE_EQ(none_by(is_even_int, IntVector()), true);
    REQUIRE_EQ(none_by(is_even_int, IntVector({2})), false);
    REQUIRE_EQ(none_by(is_even_int, IntVector({1})), true);
    REQUIRE_EQ(none_by(is_even_int, IntVector({1, 1})), true);
    REQUIRE_EQ(none_by(is_even_int, IntVector({2, 1})), false);
}

TEST_CASE("container_common_test, minmax")
{
    using namespace fplus;
    auto negateInt = [](int i) -> int { return -i; };

    REQUIRE_EQ(minimum(xs), 1);
    REQUIRE_EQ(maximum(xs), 3);

    REQUIRE_EQ(minimum_by(std::greater<int>(), xs), 3);
    REQUIRE_EQ(maximum_by(std::greater<int>(), xs), 1);

    REQUIRE_EQ(minimum_on(negateInt, xs), 3);
    REQUIRE_EQ(maximum_on(negateInt, xs), 1);

    REQUIRE_EQ(minimum_idx(xs), 0);
    REQUIRE_EQ(maximum_idx(xs), 3);

    REQUIRE_EQ(minimum_idx_by(std::greater<int>(), xs), 3);
    REQUIRE_EQ(maximum_idx_by(std::greater<int>(), xs), 0);

    REQUIRE_EQ(minimum_idx_on(negateInt, xs), 3);
    REQUIRE_EQ(maximum_idx_on(negateInt, xs), 0);
}

TEST_CASE("container_common_test, size")
{
    using namespace fplus;
    REQUIRE_EQ(fplus::size_of_cont(xs), 5);
    REQUIRE_EQ(fplus::size_of_cont(IntVector()), 0);
    REQUIRE_EQ(is_not_empty(xs), true);
}

TEST_CASE("container_common_test, mean")
{
    using namespace fplus;
    std::vector<unsigned char> uchars = {200, 202};
    typedef std::vector<double> DoubleVector;
    REQUIRE_EQ(sum(xs), 10);
    REQUIRE_EQ(product(xs), 24);
    REQUIRE_EQ(mean<int>(xs), 2);
    REQUIRE_EQ(mean_using_doubles<unsigned char>(uchars), 201);
    REQUIRE_EQ(median(IntVector({ 3 })), 3);
    REQUIRE_EQ(median(IntVector({ 3, 5 })), 4);
    REQUIRE(is_in_range(3.49f, 3.51f, median<IntVector, float>(IntVector({ 3, 4 }))));
    REQUIRE(is_in_range(3.49, 3.51, mean<double>(DoubleVector({ 3, 4 }))));
    REQUIRE_EQ(median(IntVector({ 3, 9, 5 })), 5);
    REQUIRE_EQ(median(xs), 2);
    REQUIRE_EQ(sum(convert_container_and_elems<std::vector<int>>(std::string("hello"))), 532);
    REQUIRE(is_in_range(5.99, 6.01, mean_stddev<double>(DoubleVector({ 4, 8 })).first));
    REQUIRE(is_in_range(1.99, 2.01, mean_stddev<double>(DoubleVector({ 4, 8 })).second));
    REQUIRE(is_in_range(3.749f, 3.751f, mean_stddev<float>(IntVector({ 1, 3, 7, 4 })).first));
    REQUIRE(is_in_range(2.16f, 2.17f, mean_stddev<float>(IntVector({ 1, 3, 7, 4 })).second));
}

TEST_CASE("container_common_test, sort")
{
    using namespace fplus;
    REQUIRE_EQ(sort(reverse(xs)), xsSorted);
    REQUIRE_EQ(sort(reverse(intList)), intListSorted);
    REQUIRE_EQ(sort_by(std::greater<int>(), xs), reverse(xsSorted));

    REQUIRE_EQ(sort_on(int_mod_10, IntVector({26,3,14})), IntVector({3,14,26}));

    REQUIRE_EQ(partial_sort(2, reverse(xs)), IntVector({1,2}));
}

TEST_CASE("container_common_test, unique")
{
    using namespace fplus;
    REQUIRE_EQ(unique(xs), IntVector({1,2,3,2}));
    auto IsEqualByis_even = [&](int a, int b)
            { return is_even_int(a) == is_even_int(b); };
    REQUIRE_EQ(unique_by(IsEqualByis_even, xs), IntVector({1,2,3,2}));

    REQUIRE_EQ(unique_on(int_mod_10, IntVector({2,22,3})), IntVector({2, 3}));
}

TEST_CASE("container_common_test, all_the_same")
{
    using namespace fplus;
    REQUIRE_EQ(all_the_same(IntVector()), true);
    REQUIRE_EQ(all_the_same(IntVector({1})), true);
    REQUIRE_EQ(all_the_same(IntVector({1,1,1})), true);
    REQUIRE_EQ(all_the_same(IntVector({1,2,1})), false);

    REQUIRE_EQ(all_the_same_on(int_mod_10, IntVector({3,13,33})), true);
    REQUIRE_EQ(all_the_same_on(int_mod_10, IntVector({3,14,33})), false);
}

TEST_CASE("container_common_test, all_unique")
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

TEST_CASE("container_common_test, is_sorted")
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

TEST_CASE("container_common_test, find")
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

TEST_CASE("container_common_test, pairs")
{
    using namespace fplus;
    IntPair intPair = std::make_pair(2, 3);
    IntPairs intPairs = {{1,2}, {3,4}};
    IntPairs intPairsSwapped = {{2,1}, {4,3}};
    REQUIRE_EQ(fst(intPair), 2);
    REQUIRE_EQ(snd(intPair), 3);
    REQUIRE_EQ(swap_pair_elems(intPair), std::make_pair(3, 2));
    REQUIRE_EQ(swap_pairs_elems(intPairs), intPairsSwapped);
    REQUIRE_EQ(transform_fst(squareLambda, intPair), std::make_pair(4, 3));
    REQUIRE_EQ(transform_snd(squareLambda, intPair), std::make_pair(2, 9));

    typedef std::vector<std::pair<std::string, int>> StringIntPairs;
    StringIntPairs stringIntPairs = {{"a", 1}, {"a", 2}, {"b", 6}, {"a", 4}};
    auto stringIntPairsAsMapGrouped = pairs_to_map_grouped(stringIntPairs);
    auto groupNameToMedianMap = transform_map_values(median<std::vector<int>>, stringIntPairsAsMapGrouped);
    auto groupNames = transform(fst<std::string, int>, stringIntPairs);
    auto getMedianValue = bind_1st_and_2nd_of_3(get_from_map_with_def<std::map<std::string, int>>, groupNameToMedianMap, 0);
    auto groupMendianValues = transform(getMedianValue, groupNames);
    auto stringIntPairsSndReplacedWithGroupMedian = zip(groupNames, groupMendianValues);
    REQUIRE_EQ(stringIntPairsSndReplacedWithGroupMedian, StringIntPairs({{"a", 2}, {"a", 2}, {"b", 6}, {"a", 2}}));
}

TEST_CASE("container_common_test, is_elem_of")
{
    using namespace fplus;
    REQUIRE_EQ(is_elem_of(2, xs), true);
    REQUIRE_EQ(is_elem_of(4, xs), false);
}

TEST_CASE("container_common_test, elem_at_idx")
{
    using namespace fplus;
    REQUIRE_EQ(elem_at_idx(2, xs), 2);
    REQUIRE_EQ(elem_at_idx_flipped(xs, 2), 2);
}

TEST_CASE("container_common_test, elem_at_idx_maybe")
{
    using namespace fplus;
    REQUIRE_EQ(elem_at_idx_maybe(2, xs), maybe<int>(2));
    REQUIRE_EQ(elem_at_idx_maybe(9, xs), nothing<int>());
}

TEST_CASE("container_common_test, contains")
{
    using namespace fplus;
    REQUIRE_EQ(contains(xs, 2), true);
    REQUIRE_EQ(contains(xs, 4), false);
}

TEST_CASE("container_common_test, find_token")
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

TEST_CASE("container_common_test, split")
{
    using namespace fplus;

    REQUIRE_EQ(split(0, true, IntVector{1,0,1}), IntVectors({{1},{1}}));
    REQUIRE_EQ(split(0, true, IntVector{0,1,0}), IntVectors({{},{1},{}}));
    REQUIRE_EQ(split(0, true, IntVector{0,1}), IntVectors({{},{1}}));
    REQUIRE_EQ(split(0, true, IntVector{1,0}), IntVectors({{1},{}}));
    REQUIRE_EQ(split(0, true, IntVector{0}), IntVectors({{},{}}));
    REQUIRE_EQ(split(0, true, IntVector{}), IntVectors{{}});

    REQUIRE_EQ(split(0, false, IntVector{1,0,1}), IntVectors({{1},{1}}));
    REQUIRE_EQ(split(0, false, IntVector{0,1,0}), IntVectors{{1}});
    REQUIRE_EQ(split(0, false, IntVector{0,1}), IntVectors{{1}});
    REQUIRE_EQ(split(0, false, IntVector{1,0}), IntVectors{{1}});
    REQUIRE_EQ(split(0, false, IntVector{0}), IntVectors({}));
    REQUIRE_EQ(split(0, false, IntVector{}), IntVectors({}));

    REQUIRE_EQ(split(0, true, IntVector{1,0,1}), IntVectors({{1},{1}}));
    REQUIRE_EQ(split(0, true, IntVector{0,1,0}), IntVectors({{},{1},{}}));
    REQUIRE_EQ(split(0, true, IntVector{0,1}), IntVectors({{},{1}}));
    REQUIRE_EQ(split(0, true, IntVector{1,0}), IntVectors({{1},{}}));
    REQUIRE_EQ(split(0, true, IntVector{0}), IntVectors({{},{}}));
    REQUIRE_EQ(split(0, true, IntVector{}), IntVectors{{}});

    REQUIRE_EQ(split(0, false, IntVector{1,0,1}), IntVectors({{1},{1}}));
    REQUIRE_EQ(split(0, false, IntVector{0,1,0}), IntVectors{{1}});
    REQUIRE_EQ(split(0, false, IntVector{0,1}), IntVectors{{1}});
    REQUIRE_EQ(split(0, false, IntVector{1,0}), IntVectors{{1}});
    REQUIRE_EQ(split(0, false, IntVector{0}), IntVectors({}));
    REQUIRE_EQ(split(0, false, IntVector{}), IntVectors({}));

    REQUIRE_EQ(split_by_token(IntVector({0}), true, IntVector{1,0,1}), IntVectors({{1},{1}}));
    REQUIRE_EQ(split_by_token(IntVector({0}), true, IntVector{0,1,0}), IntVectors({{},{1},{}}));
    REQUIRE_EQ(split_by_token(IntVector({0}), true, IntVector{0,1}), IntVectors({{},{1}}));
    REQUIRE_EQ(split_by_token(IntVector({0}), true, IntVector{1,0}), IntVectors({{1},{}}));
    REQUIRE_EQ(split_by_token(IntVector({0}), true, IntVector{0}), IntVectors({{},{}}));
    REQUIRE_EQ(split_by_token(IntVector({0}), true, IntVector{}), IntVectors{{}});

    REQUIRE_EQ(split_by_token(IntVector({0}), false, IntVector{1,0,1}), IntVectors({{1},{1}}));
    REQUIRE_EQ(split_by_token(IntVector({0}), false, IntVector{0,1,0}), IntVectors{{1}});
    REQUIRE_EQ(split_by_token(IntVector({0}), false, IntVector{0,1}), IntVectors{{1}});
    REQUIRE_EQ(split_by_token(IntVector({0}), false, IntVector{1,0}), IntVectors{{1}});
    REQUIRE_EQ(split_by_token(IntVector({0}), false, IntVector{0}), IntVectors({}));
    REQUIRE_EQ(split_by_token(IntVector({0}), false, IntVector{}), IntVectors({}));

    REQUIRE_EQ(split_by_token(IntVector({0,9}), true, IntVector{1,0,9,1}), IntVectors({{1},{1}}));
    REQUIRE_EQ(split_by_token(IntVector({0,9}), true, IntVector{0,9,1,0,9}), IntVectors({{},{1},{}}));
    REQUIRE_EQ(split_by_token(IntVector({0,9}), true, IntVector{0,9,1}), IntVectors({{},{1}}));
    REQUIRE_EQ(split_by_token(IntVector({0,9}), true, IntVector{1,0,9}), IntVectors({{1},{}}));
    REQUIRE_EQ(split_by_token(IntVector({0,9}), true, IntVector{0,9}), IntVectors({{},{}}));
    REQUIRE_EQ(split_by_token(IntVector({0,9}), true, IntVector{}), IntVectors{{}});

    REQUIRE_EQ(split_by_token(IntVector({0,9}), false, IntVector{1,0,9,1}), IntVectors({{1},{1}}));
    REQUIRE_EQ(split_by_token(IntVector({0,9}), false, IntVector{0,9,1,0,9}), IntVectors{{1}});
    REQUIRE_EQ(split_by_token(IntVector({0,9}), false, IntVector{0,9,1}), IntVectors{{1}});
    REQUIRE_EQ(split_by_token(IntVector({0,9}), false, IntVector{1,0,9}), IntVectors{{1}});
    REQUIRE_EQ(split_by_token(IntVector({0,9}), false, IntVector{0,9}), IntVectors({}));
    REQUIRE_EQ(split_by_token(IntVector({0,9}), false, IntVector{}), IntVectors({}));

    REQUIRE_EQ(split_by_token(IntVector({}), true, IntVector{}), IntVectors({{},{}}));
    REQUIRE_EQ(split_by_token(IntVector({}), true, IntVector{1}), IntVectors({{},{1},{}}));
    REQUIRE_EQ(split_by_token(IntVector({}), true, IntVector{1,2}), IntVectors({{},{1},{2},{}}));

    REQUIRE_EQ(split_by_token(IntVector({}), false, IntVector{}), IntVectors({}));
    REQUIRE_EQ(split_by_token(IntVector({}), false, IntVector{1}), IntVectors{{1}});
    REQUIRE_EQ(split_by_token(IntVector({}), false, IntVector{1,2}), IntVectors({{1},{2}}));

    REQUIRE_EQ(split_one_of(IntVector({0,3}), true, IntVector{1,3,2,0,0,6,0,7,5}), IntVectors({{1},{2},{},{6},{7,5}}));
    REQUIRE_EQ(split_one_of(std::string(" ,.?"), false, std::string("Hi, how are you?")), std::vector<std::string>({"Hi", "how", "are", "you"}));

    REQUIRE_EQ(split_at_idx(2, xs), std::make_pair(IntVector({1,2}), IntVector({2,3,2})));
    REQUIRE_EQ(partition(is_even_int, xs), std::make_pair(IntVector({2,2,2}), IntVector({1,3})));
    REQUIRE_EQ(split_every(2, xs), IntVectors({{1,2}, {2, 3}, {2}}));
    auto splittedAt1And3 = split_at_idxs(IdxVector({1,3}), xs);
    IntVectors splittedAt1And3Dest = {IntVector({1}), IntVector({2,2}), IntVector({3,2})};
    REQUIRE_EQ(splittedAt1And3, splittedAt1And3Dest);
    auto splittedAt1And3And3 = split_at_idxs(IdxVector({1,3,3}), xs);
    IntVectors splittedAt1And3And3Dest = {IntVector({1}), IntVector({2,2}), IntVector({}), IntVector({3,2})};
    REQUIRE_EQ(splittedAt1And3And3, splittedAt1And3And3Dest);
    REQUIRE_EQ(split(3, true, xs), IntVectors({IntVector({1, 2, 2}), IntVector({2})}));
    REQUIRE_EQ(split(1, true, IntVector{0,1,2}), IntVectors({{0},{2}}));
    REQUIRE_EQ(split(2, true, IntVector{5,2,0,3}), IntVectors({{5},{0,3}}));

    REQUIRE_EQ(split(2, true, IntVector{1,2}), IntVectors({{1},{}}));
    REQUIRE_EQ(split(2, true, IntVector{2}), IntVectors({{},{}}));
    REQUIRE_EQ(split(2, true, IntVector{}), IntVectors{{}});

    REQUIRE_EQ(split_by(is_even_int, true, IntList({1,3,2,2,5,5,3,6,7,9})), IntLists({{1,3},{},{5,5,3},{7,9}}));
    REQUIRE_EQ(split_by_keep_separators(is_even_int, IntList({})), IntLists());
    REQUIRE_EQ(split_by_keep_separators(is_even_int, IntList({2})), IntLists({IntList({2})}));
    REQUIRE_EQ(split_by_keep_separators(is_even_int, IntList({2,2,3})), IntLists({{2},{2,3}}));
    REQUIRE_EQ(split_by_keep_separators(is_even_int, IntList({1,3,2})), IntLists({{1,3},{2}}));
    REQUIRE_EQ(split_by_keep_separators(is_even_int, IntList({1,3,2,2,5,5,3,6,7,9})), IntLists({{1,3},{2},{2,5,5,3},{6,7,9}}));
    REQUIRE_EQ(split_keep_separators(2, IntList({1,3,2,2,5,5,3,2,7,9})), IntLists({{1,3},{2},{2,5,5,3},{2,7,9}}));
}

TEST_CASE("container_common_test, insert_at_idx")
{
    using namespace fplus;
    REQUIRE_EQ(insert_at_idx(2, 0, IntVector({1,2,3,4})), IntVector({1,2,0,3,4}));
}

TEST_CASE("container_common_test, enumerate")
{
    using namespace fplus;
    REQUIRE_EQ(enumerate(xs), (std::vector<std::pair<std::size_t, int>>({{0,1}, {1,2}, {2,2}, {3,3}, {4,2}})));
}

TEST_CASE("container_common_test, range")
{
    using namespace fplus;
    IntList v789 = { 7,8,9 };
    REQUIRE_EQ(set_range(1, v789, intList), IntList({ 1,7,8,9,2 }));
    REQUIRE_EQ(get_range(1, 4, intList), IntList({ 2,2,3 }));
    REQUIRE_EQ(replace_elems(2, 5, xs), IntVector({1,5,5,3,5}));
    REQUIRE_EQ(replace_tokens(std::string("123"), std::string("_"), std::string("--123----123123")), std::string("--_----__"));
    REQUIRE_EQ(take(2, xs), IntVector({ 1,2 }));
    REQUIRE_EQ(take_exact(2, xs), IntVector({ 1,2 }));
    REQUIRE_EQ(drop(2, xs), IntVector({ 2,3,2 }));
    REQUIRE_EQ(drop_exact(2, xs), IntVector({ 2,3,2 }));
    REQUIRE_EQ(take(999, xs), xs);
    REQUIRE_EQ(drop(999, xs), IntVector());
    REQUIRE_EQ(take_while(is_odd_int, xs), IntVector({ 1 }));
    REQUIRE_EQ(drop_while(is_odd_int, xs), IntVector({ 2,2,3,2 }));
    REQUIRE_EQ(span(is_odd_int, xs), std::make_pair(IntVector({ 1 }), IntVector({ 2,2,3,2 })));
    REQUIRE_EQ(replace_range(2, IntVector({8,9}), xs), IntVector({1,2,8,9,2}));
    REQUIRE_EQ(take_cyclic(5, IntVector({0,1,2,3})), IntVector({0,1,2,3,0}));
    REQUIRE_EQ(take_cyclic(7, IntVector({0,1,2,3})), IntVector({0,1,2,3,0,1,2}));
    REQUIRE_EQ(take_cyclic(7, IntVector({0,1})), IntVector({0,1,0,1,0,1,0}));
    REQUIRE_EQ(take_cyclic(2, IntVector({0,1,2,3})), IntVector({0,1}));
    REQUIRE_EQ(take_cyclic(3, IntVector({0})), IntVector({0,0,0}));
}

TEST_CASE("container_common_test, keep_if")
{
    using namespace fplus;
    REQUIRE_EQ(keep_if(is2, xs), IntVector({ 2,2,2 }));
    REQUIRE_EQ(keep_if(is3, xs), IntVector({ 3 }));
    REQUIRE_EQ(keep_if(is4, xs), IntVector());
}

TEST_CASE("container_common_test, find_all_idxs_of")
{
    using namespace fplus;
    REQUIRE_EQ(find_all_idxs_of('h', std::string("oh, ha!")), std::vector<std::size_t>({ 1, 4 }));
    REQUIRE_EQ(find_all_idxs_of(2, xs), IdxVector({ 1,2,4 }));
}

TEST_CASE("container_common_test, count")
{
    using namespace fplus;
    REQUIRE_EQ(count(2, xs), 3);
}

TEST_CASE("container_common_test, infix")
{
    using namespace fplus;
    REQUIRE_EQ(is_infix_of(IntVector({}), IntVector({})), true);
    REQUIRE_EQ(is_infix_of(IntVector({}), IntVector({1,2})), true);
    REQUIRE_EQ(is_infix_of(IntVector({2,3}), xs), true);
    REQUIRE_EQ(is_infix_of(IntVector({2,3}), xs), true);
    REQUIRE_EQ(is_infix_of(IntVector({2,1}), xs), false);
    REQUIRE_EQ(is_prefix_of(IntVector({ 1,2 }), xs), true);
    REQUIRE_EQ(is_prefix_of(IntVector({ 2,2 }), xs), false);
    REQUIRE_EQ(is_suffix_of(IntVector({ 3,2 }), xs), true);
    REQUIRE_EQ(is_suffix_of(IntVector({ 2,2 }), xs), false);
}

TEST_CASE("container_common_test, subsequence")
{
    using namespace fplus;
    REQUIRE_EQ(is_subsequence_of(IntVector(), IntVector()), true);
    REQUIRE_EQ(is_subsequence_of(IntVector(), xs), true);
    REQUIRE_EQ(is_subsequence_of(IntVector({ 1,3 }), xs), true);
    REQUIRE_EQ(is_subsequence_of(IntVector({ 3,1 }), xs), false);
}

TEST_CASE("container_common_test, interleave")
{
    using namespace fplus;
    typedef std::vector<IntVector> IntVectors;
    REQUIRE_EQ(interleave(IntVectors()), IntVector());
    REQUIRE_EQ(interleave(IntVectors({})), IntVector());
    REQUIRE_EQ(interleave(IntVectors({IntVector({})})), IntVector());
    REQUIRE_EQ(interleave(IntVectors({{},{}})), IntVector());
    REQUIRE_EQ(interleave(IntVectors({{1},{}})), IntVector({1}));
    REQUIRE_EQ(interleave(IntVectors({{1,2,3},{4,5},{6,7,8}})), IntVector({1,4,6,2,5,7,3,8}));
    REQUIRE_EQ(interleave(IntVectors({{1,2,3},{4,5},{6,7,8}})), IntVector({1,4,6,2,5,7,3,8}));
}

TEST_CASE("container_common_test, transpose")
{
    using namespace fplus;
    typedef std::vector<IntVector> IntVectors;
    REQUIRE_EQ(transpose(IntVectors()), IntVectors());
    REQUIRE_EQ(transpose(IntVectors({})), IntVectors());
    REQUIRE_EQ(transpose(IntVectors({},{})), IntVectors());
    REQUIRE_EQ(transpose(IntVectors({ { 1 }, { 2 } })), IntVectors({ { 1, 2 } }));
    REQUIRE_EQ(transpose(IntVectors({ { 1, 2 } })), IntVectors({ { 1 }, { 2 } }));
    REQUIRE_EQ(transpose(IntVectors({ { 1, 2 }, { 3, 4 } })), IntVectors({ { 1, 3 }, { 2, 4 } }));
    REQUIRE_EQ(transpose(IntVectors({ { 1, 2, 3 }, { 4, 5, 6 } })), IntVectors({ { 1, 4 }, { 2, 5 }, { 3, 6 } }));
    REQUIRE_EQ(transpose(IntVectors({{1,2,3},{4,5,6},{7,8,9}})), IntVectors({{1,4,7},{2,5,8},{3,6,9}}));
    REQUIRE_EQ(transpose(IntVectors({{1,2,3},{4,5},{7,8,9}})), IntVectors({{1,4,7},{2,5,8},{3,9}}));
}

TEST_CASE("container_common_test, sample")
{
    using namespace fplus;
    REQUIRE_EQ(sample(3, xs).size(), 3);
}

TEST_CASE("container_common_test, generate")
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

TEST_CASE("container_common_test, nub")
{
    using namespace fplus;
    REQUIRE_EQ(nub(xs), IntVector({ 1,2,3 }));
    auto bothEven = is_equal_by(is_even_int);
    REQUIRE_EQ(nub_by(bothEven, xs), IntVector({ 1,2 }));
    REQUIRE_EQ(nub_on(int_mod_10, IntVector({12,32,15})), IntVector({12,15}));
}

TEST_CASE("container_common_test, map")
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

    IntStringMap union_map_1 = {{0, "a"}, {1, "b"}};
    IntStringMap union_map_2 = {{0, "c"}, {2, "d"}};
    IntStringMap union_map_res = {{0, "a"}, {1, "b"}, {2, "d"}};
    IntStringMap union_map_with_res = {{0, "ac"}, {1, "b"}, {2, "d"}};
    REQUIRE_EQ(map_union(union_map_1, union_map_2), union_map_res);
    REQUIRE_EQ(map_union_with(append<std::string>, union_map_1, union_map_2), union_map_with_res);

    typedef std::map<std::string::value_type, int> CharIntMap;
    CharIntMap charIntMap = {{'a', 1}, {'b', 2}, {'A', 3}, {'C', 4}};
    const auto is_upper = [](std::string::value_type c) -> bool
    {
        return std::isupper(c);
    };
    const auto is_lower = [](std::string::value_type c) -> bool
    {
        return std::islower(c);
    };
    REQUIRE_EQ(map_keep_if(is_upper, charIntMap), CharIntMap({{'A', 3}, {'C', 4}}));
    REQUIRE_EQ(map_drop_if(is_lower, charIntMap), CharIntMap({{'A', 3}, {'C', 4}}));
    typedef std::vector<std::string::value_type> CharVector;
    REQUIRE_EQ(map_keep(CharVector({'b', 'F'}), charIntMap), CharIntMap({{'b', 2}}));
    REQUIRE_EQ(map_drop(CharVector({'a', 'A', 'C', 'F'}), charIntMap), CharIntMap({{'b', 2}}));

    typedef std::vector<CharIntMap> CharIntMaps;
    typedef std::vector<maybe<int>> MaybeInts;
    REQUIRE_EQ(
        map_pluck('a', CharIntMaps({{{'a',1}, {'b',2}}, {{'a',3}}, {{'c',4}}})),
        MaybeInts({1, 3, {}}));
}

TEST_CASE("container_common_test, set")
{
    using namespace fplus;
    using IntSet = std::set<int>;
    using setVector = std::vector<IntSet>;
    using IntUnordSet = std::unordered_set<int>;
    using unordSetVector = std::vector<IntUnordSet>;

    //std::set tests
    REQUIRE(set_includes(IntSet({0,1,2,3}), IntSet({0,2})));
    REQUIRE_FALSE(set_includes(IntSet({0,2}), IntSet({0,1,2,3})));
    REQUIRE_FALSE(set_includes(IntSet({0,1,2,3}), IntSet({2,3,4,5})));
    REQUIRE_EQ(set_merge(IntSet({0,1,2,3}), IntSet({2,3,4,5})), IntSet({0,1,2,3,4,5}));
    REQUIRE_EQ(set_merge(IntSet({0,1,2,3}), IntSet({0,2})), IntSet({0,1,2,3}));
    REQUIRE_EQ(set_intersection(IntSet({0,1,2,3}), IntSet({2,3,4,5})), IntSet({2,3}));
    REQUIRE_EQ(set_difference(IntSet({0,1,2,3}), IntSet({2,3,4,5})), IntSet({0,1}));
    REQUIRE_EQ(set_symmetric_difference(IntSet({0,1,2,3}), IntSet({2,3,4,5})), IntSet({0,1,4,5}));
    REQUIRE_EQ(set_intersection(IntSet({0,1,2,3}), IntSet({2,3,4,5})), IntSet({2,3}));
    REQUIRE_EQ(sets_intersection(setVector({IntSet({0,1,2,3}), IntSet({2,3,4,5}), IntSet({0,2})})), IntSet({2}));

    //set::unordered_set tests
    REQUIRE(unordered_set_includes(IntUnordSet({0,1,2,3}), IntUnordSet({0,2})));
    REQUIRE_FALSE(unordered_set_includes(IntUnordSet({0,2}), IntUnordSet({0,1,2,3})));
    REQUIRE_FALSE(unordered_set_includes(IntUnordSet({0,1,2,3}), IntUnordSet({2,3,4,5})));
    REQUIRE_EQ(set_merge(IntUnordSet({0,1,2,3}), IntUnordSet({2,3,4,5})), IntUnordSet({0,1,2,3,4,5}));
    REQUIRE_EQ(set_merge(IntUnordSet({0,1,2,3}), IntUnordSet({0,2})), IntUnordSet({0,1,2,3}));
    REQUIRE_EQ(unordered_set_intersection(IntUnordSet({0,1,2,3}), IntUnordSet({2,3,4,5})), IntUnordSet({2,3}));
    REQUIRE_EQ(unordered_set_difference(IntUnordSet({0,1,2,3}), IntUnordSet({2,3,4,5})), IntUnordSet({0,1}));
    REQUIRE_EQ(unordered_set_symmetric_difference(IntUnordSet({0,1,2,3}), IntUnordSet({2,3,4,5})), IntUnordSet({0,1,4,5}));
    REQUIRE_EQ(unordered_set_intersection(IntUnordSet({0,1,2,3}), IntUnordSet({2,3,4,5})), IntUnordSet({2,3}));
    REQUIRE_EQ(unordered_sets_intersection(unordSetVector({IntUnordSet({0,1,2,3}), IntUnordSet({2,3,4,5}), IntUnordSet({0,2})})), IntUnordSet({2}));


}

TEST_CASE("container_common_test, count_occurrences_by")
{
    using namespace fplus;
    typedef std::map<int, std::size_t> IntSizeTMap;
    IntSizeTMap OccurrencesResult = {{1, 1}, {2, 3}, {3, 1}};
    std::vector<double> double_values = {1.1, 2.3, 2.7, 3.6, 2.4};
    const auto f = floor<double>;
    REQUIRE_EQ(count_occurrences_by(f, double_values), OccurrencesResult);
}

TEST_CASE("container_common_test, count_occurrences")
{
    using namespace fplus;
    typedef std::map<int, std::size_t> IntSizeTMap;
    IntSizeTMap OccurrencesResult = {{1, 1}, {2, 3}, {3, 1}};
    REQUIRE_EQ(count_occurrences(xs), OccurrencesResult);
}

TEST_CASE("container_common_test, insert_at")
{
    using namespace fplus;
    REQUIRE_EQ(insert_at(2, IntVector({8,9}), xs), IntVector({1,2,8,9,2,3,2}));
}

TEST_CASE("container_common_test, head")
{
    using namespace fplus;
    REQUIRE_EQ(head(xs), 1);
}

TEST_CASE("container_common_test, tail")
{
    using namespace fplus;
    REQUIRE_EQ(init(xs), IntVector({1,2,2,3}));
    REQUIRE_EQ(tail(xs), IntVector({2,2,3,2}));

    REQUIRE_EQ(inits(xs), IntVectors({{},{1},{1,2},{1,2,2},{1,2,2,3},{1,2,2,3,2}}));
    REQUIRE_EQ(tails(xs), IntVectors({{1,2,2,3,2},{2,2,3,2},{2,3,2},{3,2},{2},{}}));
}

TEST_CASE("container_common_test, iterate")
{
    using namespace fplus;
    auto times_two = [](int x) { return 2*x; };
    REQUIRE_EQ(iterate(times_two, 0, 3), IntVector({}));
    REQUIRE_EQ(iterate(times_two, 1, 3), IntVector({3}));
    REQUIRE_EQ(iterate(times_two, 2, 3), IntVector({3,6}));
    REQUIRE_EQ(iterate(times_two, 5, 3), IntVector({3,6,12,24,48}));
}

TEST_CASE("container_common_test, is_permutation_of")
{
    using namespace fplus;
    REQUIRE(is_permutation_of(IntVector({2,3,1}), IntVector({1,2,3})));
    REQUIRE_FALSE(is_permutation_of(IntVector({2,3,2}), IntVector({1,2,3})));
    REQUIRE_FALSE(is_permutation_of(IntVector({2,3}), IntVector({1,2,3})));
    REQUIRE_FALSE(is_permutation_of(IntVector({2,3,1}), IntVector({1,23})));
}

TEST_CASE("container_common_test, present_in_all")
{
    using namespace fplus;
    const std::vector<std::vector<int>> xss = { {4,1,2}, {5,2,1}, {2,4,1} };
    REQUIRE_EQ(present_in_all(xss), IntVector({1,2}));
}

TEST_CASE("container_common_test, is_unique_in")
{
    using namespace fplus;
    REQUIRE_FALSE(is_unique_in(2, xs));
    REQUIRE(is_unique_in(3, xs));
}

TEST_CASE("container_common_test, divvy")
{
    using namespace fplus;
    REQUIRE_EQ(divvy(5, 2, IntVector({0,1,2,3,4,5,6,7,8,9})), IntVectors({{0,1,2,3,4},{2,3,4,5,6},{4,5,6,7,8}}));
}

TEST_CASE("container_common_test, aperture")
{
    using namespace fplus;
    REQUIRE_EQ(aperture(5, IntVector({0,1,2,3,4,5,6})), IntVectors({{0,1,2,3,4},{1,2,3,4,5},{2,3,4,5,6}}));
    REQUIRE_EQ(divvy(5, 1, IntVector({0,1,2,3,4,5,6})), IntVectors({{0,1,2,3,4},{1,2,3,4,5},{2,3,4,5,6}}));
}

TEST_CASE("container_common_test, stride")
{
    using namespace fplus;
    REQUIRE_EQ(stride(3, IntVector({0,1,2,3,4,5,6,7})), IntVector({0,3,6}));
    REQUIRE_EQ(divvy(1, 3, IntVector({0,1,2,3,4,5,6,7})), IntVectors({{0},{3},{6}}));
}
