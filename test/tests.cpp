// Copyright Tobias Hermann 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "fplus.h"

#include <cassert>
#include <chrono>
#include <ctime>
#include <deque>
#include <functional>
#include <iostream>
#include <list>
#include <string>
#include <vector>

void Test_Numeric()
{
    using namespace fplus;

    assert(is_in_range(1, 3, 1) == true);
    assert(is_in_range(1, 3, 2) == true);
    assert(is_in_range(1, 3, 0) == false);
    assert(is_in_range(1, 3, 3) == false);

    assert(is_negative(0.1) == false);
    assert(is_positive(0.1) == true);
    assert(is_negative(-0.1) == true);
    assert(is_positive(-0.1) == false);

    assert(round(1.4) == 1);
    assert(round(1.6) == 2);
    assert(floor(1.4) == 1);
    assert(ceil(1.4) == 2);

    assert(round(-1.4) == -1);
    assert(round(-1.6) == -2);
    assert(floor(-1.4) == -2);
    assert(ceil(-1.4) == -1);

    assert(clamp(2, 6, 5) == 5);
    assert(clamp(2, 6, 1) == 2);
    assert(clamp(2, 6, 8) == 6);
}

int APlusTwoTimesBFunc(int a, int b) { return a + 2 * b; }

std::string CcI2SFree(const std::string& str, int x)
{
    return str + std::to_string(x);
}

auto CcI2SLambda = [](const std::string& str, int x)
{ return CcI2SFree(str, x); };

std::function<std::string(const std::string&, int)>
CcI2SStdFunction = CcI2SLambda;

std::string (*CcI2SFunctionPointer)(const std::string&, int) =
&CcI2SFree;

struct CcI2SStrct {
    std::string operator() (const std::string& str, int x)
    { return CcI2SFree(str, x); }
    std::string nonCMemF (const std::string& str, int x)
    { return CcI2SFree(str, x); }
    std::string cnstMemF (const std::string& str, int x) const
    { return CcI2SFree(str, x); }
    static std::string sttcMemF (const std::string& str, int x)
    { return CcI2SFree(str, x); }
};

class State {
public:
    explicit State(int x) : x_(x) {}
    void Add(int y) { x_ += y; }
    int Get() const { return x_; }
private:
    int x_;
};

template <typename IntCont, typename IntContCont>
void Test_Composition()
{
    using namespace fplus;
    auto square = [](int x){ return x*x; };

    typedef IntCont Row;
    Row row = {1,2,3};

    typedef IntContCont Mat;
    Mat mat;
    auto squareRowElems = bind_1st_of_2(transform<decltype(square), Row>,
            square);
    Row squaredRow = squareRowElems(row);
    assert(squaredRow == Row({1,4,9}));

    assert((compose(square, square)(2)) == 16);
    assert((compose(square, square, square)(2)) == 256);
    assert((compose(square, square, square, square)(2)) == 65536);
    assert((compose(square, square, square, square, square)(1)) == 1);
    auto add3 = [](int x, int y, int z) { return x + y + z; };
    assert(bind_1st_and_2nd_of_3(add3, 3, 5)(7) == 15);
    auto APlusTwoTimesB = [](int a, int b) { return a + 2 * b; };
    auto TwoTimesAPlusB = [](int a, int b) { return 2 * a + b; };
    assert((flip(APlusTwoTimesB)(2, 1)) == 5);
    assert((flip(TwoTimesAPlusB)(1, 2)) == 5);
    auto is1 = [](int x) { return x == 1; };
    auto is2 = [](int x) { return x == 2; };
    assert((logical_not(is1)(1)) == false);
    assert((logical_not(is1)(2)) == true);

    assert((logical_or(is1, is2)(1)) == true);
    assert((logical_or(is1, is2)(2)) == true);
    assert((logical_or(is1, is2)(3)) == false);
    assert((logical_and(is1, is2)(1)) == false);
    assert((logical_and(is1, is1)(1)) == true);
    assert((logical_xor(is1, is1)(1)) == false);
    assert((logical_xor(is2, is1)(1)) == true);
    assert((logical_xor(is2, is2)(1)) == false);

    assert((apply_to_pair(APlusTwoTimesB, std::make_pair(1, 2))) == 5);
    assert((apply_to_pair(APlusTwoTimesBFunc, std::make_pair(1, 2))) == 5);

    State state(1);
    assert(state.Get() == 1);
    auto stateAdd = std::mem_fn(&State::Add);

    stateAdd(state, 2);
    assert(state.Get() == 3);

    //auto stateAddBoundFPP = Bind1of2(stateAdd, &state); // crashes VC2015 compiler
    //stateAddBoundFPP(3);
    auto stateAddBoundStl = std::bind(&State::Add, std::placeholders::_1, std::placeholders::_2);
    stateAddBoundStl(state, 3);
    assert(state.Get() == 6);
}

void Test_FunctionTraits()
{
    static_assert(std::is_same<
        utils::function_traits<decltype(CcI2SFree)>::arg<0>::type,
        const std::string&>::value, "No.");
    static_assert(std::is_same<
        utils::function_traits<decltype(CcI2SFree)>::arg<1>::type,
        int>::value, "No.");
    static_assert(std::is_same<
        utils::function_traits<decltype(CcI2SFree)>::result_type,
        std::string>::value, "No.");

    static_assert(std::is_same<
        utils::function_traits<decltype(CcI2SLambda)>::arg<0>::type,
        const std::string&>::value, "No.");
    static_assert(std::is_same<
        utils::function_traits<decltype(CcI2SLambda)>::arg<1>::type,
        int>::value, "No.");
    static_assert(std::is_same<
        utils::function_traits<decltype(CcI2SLambda)>::result_type,
        std::string>::value, "No.");

    static_assert(std::is_same<
        utils::function_traits<decltype(CcI2SStdFunction)>::arg<0>::type,
        const std::string&>::value, "No.");
    static_assert(std::is_same<
        utils::function_traits<decltype(CcI2SStdFunction)>::arg<1>::type,
        int>::value, "No.");
    static_assert(std::is_same<
        utils::function_traits<decltype(CcI2SStdFunction)>::result_type,
        std::string>::value, "No.");

    static_assert(std::is_same<
        utils::function_traits<decltype(CcI2SFunctionPointer)>::arg<0>::type,
        const std::string&>::value, "No.");
    static_assert(std::is_same<
        utils::function_traits<decltype(CcI2SFunctionPointer)>::arg<1>::type,
        int>::value, "No.");
    static_assert(std::is_same<
        utils::function_traits<decltype(CcI2SFunctionPointer)>::result_type,
        std::string>::value, "No.");

    CcI2SStrct ccI2SStrct;
    ccI2SStrct("dummy call to avoid unused variable warnings", 0);
    static_assert(std::is_same<
        utils::function_traits<decltype(ccI2SStrct)>::arg<0>::type,
        const std::string&>::value, "No.");
    static_assert(std::is_same<
        utils::function_traits<decltype(ccI2SStrct)>::arg<1>::type,
        int>::value, "No.");
    static_assert(std::is_same<
        utils::function_traits<decltype(ccI2SStrct)>::result_type,
        std::string>::value, "No.");

    static_assert(std::is_same<
        utils::function_traits<decltype(&CcI2SStrct::nonCMemF)>::arg<0>::type,
        const std::string&>::value, "No.");
    static_assert(std::is_same<
        utils::function_traits<decltype(&CcI2SStrct::nonCMemF)>::arg<1>::type,
        int>::value, "No.");
    static_assert(std::is_same<
        utils::function_traits<decltype(&CcI2SStrct::nonCMemF)>::result_type,
        std::string>::value, "No.");

    static_assert(std::is_same<
        utils::function_traits<decltype(&CcI2SStrct::cnstMemF)>::arg<0>::type,
        const std::string&>::value, "No.");
    static_assert(std::is_same<
        utils::function_traits<decltype(&CcI2SStrct::cnstMemF)>::arg<1>::type,
        int>::value, "No.");
    static_assert(std::is_same<
        utils::function_traits<decltype(&CcI2SStrct::cnstMemF)>::result_type,
        std::string>::value, "No.");

    static_assert(std::is_same<
        utils::function_traits<decltype(&CcI2SStrct::sttcMemF)>::arg<0>::type,
        const std::string&>::value, "No.");
    static_assert(std::is_same<
        utils::function_traits<decltype(&CcI2SStrct::sttcMemF)>::arg<1>::type,
        int>::value, "No.");
    static_assert(std::is_same<
        utils::function_traits<decltype(&CcI2SStrct::sttcMemF)>::result_type,
        std::string>::value, "No.");
}

void Test_Maybe()
{
    using namespace fplus;
    auto square = [](int x){ return x*x; };
    auto sqrtToMaybe = [](float x) {
        return x < 0.0f ? nothing<float>() :
                just(static_cast<float>(sqrt(static_cast<float>(x))));
    };
    auto sqrtToMaybeInt = [](int x) {
        return x < 0 ? nothing<int>() :
                just(fplus::round(sqrt(static_cast<float>(x))));
    };
    auto IntToFloat = [](const int& x) { return static_cast<float>(x); };

    maybe<int> x(2);
    maybe<int> y = nothing<int>();
    auto Or42 = bind_1st_of_2(with_default<int>, 42);
    auto SquareAndSquare = compose(square, square);
    assert(Or42(x) == 2);
    assert(Or42(y) == 42);
    auto squareMaybe = lift(square);
    auto sqrtAndSqrt = and_then(sqrtToMaybe, sqrtToMaybe);
    assert(squareMaybe(x) == just(4));
    assert(squareMaybe(y) == nothing<int>());
    assert((lift(SquareAndSquare))(x) == just(16));
    auto LiftedIntToFloat = lift(IntToFloat);
    auto JustInt = just<int>;
    auto IntToMaybeFloat = compose(JustInt, LiftedIntToFloat);
    auto IntToFloatAndSqrtAndSqrt = and_then(IntToMaybeFloat, sqrtAndSqrt);
    assert(is_in_range(1.41f, 1.42f, unsafe_get_just<float>
            (IntToFloatAndSqrtAndSqrt(4))));
    typedef std::vector<maybe<int>> IntMaybes;
    typedef std::vector<int> Ints;
    IntMaybes maybes = {just(1), nothing<int>(), just(2)};
    assert(justs(maybes) == Ints({ 1,2 }));
    assert(just(1) == just(1));
    assert(just(1) != just(2));
    assert(just(1) != nothing<int>());
    assert(nothing<int>() == nothing<int>());

    Ints wholeNumbers = { -3, 4, 16, -1 };
    assert(transform_and_keep_justs(sqrtToMaybeInt, wholeNumbers)
            == Ints({2,4}));
    assert(transform_and_concat(bind_1st_of_2(replicate<Ints>, 3), Ints{ 1,2 })
            == Ints({ 1,1,1,2,2,2 }));
}

void Test_Compare()
{
    using namespace fplus;
    assert(is_equal(2, 2));
    assert(!is_equal(2, 3));
    assert(!is_not_equal(2, 2));
    assert(is_not_equal(2, 3));

    assert(!is_less(2, 2));
    assert(is_less(2, 3));
    assert(!is_less(3, 2));

    assert(is_less_or_equal(2, 2));
    assert(is_less_or_equal(2, 3));
    assert(!is_less_or_equal(3, 2));

    assert(!is_greater(2, 2));
    assert(!is_greater(2, 3));
    assert(is_greater(3, 2));

    assert(is_greater_or_equal(2, 2));
    assert(!is_greater_or_equal(2, 3));
    assert(is_greater_or_equal(3, 2));

    assert(identity(2) == 2);
    assert(always(2, 5) == 2);

    assert(xor_bools(false, false) == false);
    assert(xor_bools(true, false) == true);
    assert(xor_bools(false, true) == true);
    assert(xor_bools(true, true) == false);

    auto int_less = [](int x, int y) { return x < y; };
    auto int_less_eq = [](int x, int y) { return x <= y; };
    assert(ord_to_eq(int_less)(1, 2) == false);
    assert(ord_to_eq(int_less)(2, 2) == true);
    assert(ord_to_eq(int_less)(2, 1) == false);
    assert(ord_to_not_eq(int_less)(1, 2) == true);
    assert(ord_to_not_eq(int_less)(2, 2) == false);
    assert(ord_to_not_eq(int_less)(2, 1) == true);
    assert(ord_eq_to_eq(int_less_eq)(1, 2) == false);
    assert(ord_eq_to_eq(int_less_eq)(2, 2) == true);
    assert(ord_eq_to_eq(int_less_eq)(2, 1) == false);
    assert(ord_eq_to_not_eq(int_less_eq)(1, 2) == true);
    assert(ord_eq_to_not_eq(int_less_eq)(2, 2) == false);
    assert(ord_eq_to_not_eq(int_less_eq)(2, 1) == true);
}

struct ExplicitFromIntStruct
{
    explicit ExplicitFromIntStruct(int x) : x_(x) {}
    int x_;
};
bool operator == (const ExplicitFromIntStruct &lhs, const ExplicitFromIntStruct & rhs) { return lhs.x_ == rhs.x_; }

int squareFunc(int x)
{
    return x * x;
}

struct squareStruct
{
    int operator()(int x) { return x*x; }
};

void Test_ContainerTools()
{
    using namespace fplus;

    auto squareLambda = [](int x) { return x*x; };

    auto is_even = [](int x){ return x % 2 == 0; };
    auto is_odd = [](int x){ return x % 2 == 1; };
    typedef std::pair<int, int> IntPair;
    typedef std::vector<int> IntVector;
    typedef std::vector<IntVector> IntVectors;
    typedef std::vector<bool> BoolVector;
    typedef std::vector<std::size_t> IdxVector;
    IntVector xs = {1,2,2,3,2};
    IntVector xsSorted = {1,2,2,2,3};
    std::string xsShown("[1, 2, 2, 3, 2]");
    IntVector xs2Times = {1,2,2,3,2,1,2,2,3,2};

    typedef std::list<int> IntList;
    typedef std::list<IntList> IntLists;
    IntList intList = { 1,2,2,3,2 };
    IntLists intLists = { { 1 },{ 2, 2 },{ 3 },{ 2 } };
    assert(group(intList) == intLists);

    typedef std::list<std::size_t> IdxList;

    assert(transform(squareLambda, xs) == IntVector({1,4,4,9,4}));
    assert(keep_if(is_even, xs) == IntVector({2,2,2}));
    assert(drop_if(is_even, xs) == IntVector({1,3}));
    assert(transform(squareLambda, intList) == IntList({ 1,4,4,9,4 }));
    assert(keep_if(is_even, intList) == IntList({ 2,2,2 }));
    assert(drop_if(is_even, intList) == IntList({ 1,3 }));
    assert(group(xs) == std::list<IntVector>({IntVector({1}),IntVector({2,2}),IntVector({3}),IntVector({2})}));
    assert(group_globally(xs) == std::list<IntVector>({IntVector({1}),IntVector({2,2,2}),IntVector({3})}));

    assert(without(2, intList) == IntList({ 1,3 }));

    assert(keep_idxs(IdxVector({1, 3}), xs) == IntVector({2,3}));
    assert(keep_idxs(IdxVector({3, 1}), xs) == IntVector({2,3}));
    assert(keep_idxs(IdxVector({1, 1, 3}), xs) == IntVector({2,3}));
    assert(keep_idxs(IdxVector({1, 3, 7}), xs) == IntVector({2,3}));

    assert(drop_idxs(IdxVector({1, 3}), xs) == IntVector({1,2,2}));

    assert(transform_convert<IntList>(squareLambda, xs) == IntList({ 1,4,4,9,4 }));

    assert(is_equal_by_and_by(is_even, is_even, 2, 4) == true);
    assert(is_equal_by_and_by(is_even, is_even, 1, 2) == false);
    assert(is_equal_by_and_by(is_odd, is_even, 1, 2) == true);
    assert(is_equal(2, 2) == true);
    assert(is_equal(1, 2) == false);

    assert(is_empty(xs) == false);
    assert(is_empty(IntVector()) == true);
    assert(is_not_empty(xs) == true);
    assert(is_not_empty(IntVector()) == false);
    assert(convert_container<IntList>(xs) == intList);
    assert(append(xs, xs) == xs2Times);

    typedef std::vector<float> FloatVector;
    assert(convert_elems<float>(xs) == FloatVector({ 1.0f,2.0f,2.0f,3.0f,2.0f }));

    assert(concat(intLists) == intList);
    assert(concat(IntVectors(2, xs)) == xs2Times);
    assert(repeat(2, xs) == xs2Times);
    assert(intersperse(0, xs) == IntVector({1,0,2,0,2,0,3,0,2}));
    assert(fold_left(std::plus<int>(), 100, xs) == 110);
    assert(fold_left_1(std::plus<int>(), xs) == 10);
    assert(fold_right(std::plus<int>(), 100, xs) == 110);
    assert(fold_right_1(std::plus<int>(), xs) == 10);
    auto appendXToStrForFoldL = [](const std::string& str, int x) { return str + std::to_string(x); };
    auto appendXToStrForFoldR = [](int x, const std::string& str) { return str + std::to_string(x); };
    std::string emptyString;
    assert(fold_left(appendXToStrForFoldL, emptyString, xs) == "12232");
    assert(fold_right(appendXToStrForFoldR, emptyString, xs) == "23221");

    assert(scan_left(std::plus<int>(), 20, xs) == IntVector({ 20,21,23,25,28,30 }));
    assert(scan_right(std::plus<int>(), 20, xs) == IntVector({ 30,29,27,25,22,20 }));
    assert(scan_left_1(std::plus<int>(), xs) == IntVector({ 1,3,5,8,10 }));
    assert(scan_right_1(std::plus<int>(), xs) == IntVector({ 10,9,7,5,2 }));

    assert(join(IntList({0}), intLists)
            == IntList({1,0,2,2,0,3,0,2}));
    assert(show_cont(xs) == xsShown);
    assert(show_cont_with(", ", xs) == xsShown);
    assert(show<int>(1) == "1");
    auto multiply = [](int x, int y){ return x * y; };
    assert(zip_with(multiply, xs, xs)
            == transform(squareLambda, xs));

    auto xsZippedWithXs = zip(xs, xs);
    assert(unzip(xsZippedWithXs).first == xs);
    assert(all(BoolVector()) == true);
    assert(all(BoolVector({true})) == true);
    assert(all(BoolVector({false})) == false);
    assert(all(BoolVector({true, true})) == true);
    assert(all(BoolVector({true, false})) == false);

    assert(all_by(is_even, IntVector()) == true);
    assert(all_by(is_even, IntVector({2})) == true);
    assert(all_by(is_even, IntVector({1})) == false);
    assert(all_by(is_even, IntVector({2, 2})) == true);
    assert(all_by(is_even, IntVector({2, 1})) == false);

    assert(any(BoolVector()) == false);
    assert(any(BoolVector({true})) == true);
    assert(any(BoolVector({false})) == false);
    assert(any(BoolVector({false, false})) == false);
    assert(any(BoolVector({true, false})) == true);

    assert(any_by(is_even, IntVector()) == false);
    assert(any_by(is_even, IntVector({2})) == true);
    assert(any_by(is_even, IntVector({1})) == false);
    assert(any_by(is_even, IntVector({1, 1})) == false);
    assert(any_by(is_even, IntVector({2, 1})) == true);

    assert(none(BoolVector()) == true);
    assert(none(BoolVector({true})) == false);
    assert(none(BoolVector({false})) == true);
    assert(none(BoolVector({false, false})) == true);
    assert(none(BoolVector({true, false})) == false);

    assert(none_by(is_even, IntVector()) == true);
    assert(none_by(is_even, IntVector({2})) == false);
    assert(none_by(is_even, IntVector({1})) == true);
    assert(none_by(is_even, IntVector({1, 1})) == true);
    assert(none_by(is_even, IntVector({2, 1})) == false);

    assert(minimum(xs) == 1);
    assert(maximum(xs) == 3);

    assert(minimum_by(std::greater<int>(), xs) == 3);
    assert(maximum_by(std::greater<int>(), xs) == 1);

    assert(fplus::size_of_cont(xs) == 5);
    assert(fplus::size_of_cont(IntVector()) == 0);
    assert(is_not_empty(xs) == true);


    assert(sum(xs) == 10);
    assert(mean<int>(xs) == 2);
    assert(median(IntVector({ 3 })) == 3);
    assert(median(IntVector({ 3, 5 })) == 4);
    assert(median(IntVector({ 3, 9, 5 })) == 5);
    assert(median(xs) == 2);
    assert(sort(reverse(xs)) == xsSorted);
    assert(sort_by(std::greater<int>(), xs) == reverse(xsSorted));
    assert(unique(xs) == IntVector({1,2,3,2}));
    auto IsEqualByis_even = [&](int a, int b)
            { return is_even(a) == is_even(b); };
    assert(unique_by(IsEqualByis_even, xs) == IntVector({1,2,3,2}));

    assert(all_the_same(IntVector()) == true);
    assert(all_the_same(IntVector({1})) == true);
    assert(all_the_same(IntVector({1,1,1})) == true);
    assert(all_the_same(IntVector({1,2,1})) == false);

    assert(all_unique(IntVector()) == true);
    assert(all_unique(IntVector({1})) == true);
    assert(all_unique(IntVector({1,2,1})) == false);
    assert(all_unique(IntVector({1,2,3})) == true);

    assert(all_unique_less(IntVector()) == true);
    assert(all_unique_less(IntVector({ 1 })) == true);
    assert(all_unique_less(IntVector({ 1,2,1 })) == false);
    assert(all_unique_less(IntVector({ 1,2,3 })) == true);

    assert(is_sorted(IntVector()) == true);
    assert(is_sorted(IntVector({1})) == true);
    assert(is_sorted(IntVector({1,2,3})) == true);
    assert(is_sorted(IntVector({1,2,2})) == true);
    assert(is_sorted(IntVector({1,2,1})) == false);

    auto is2 = bind_1st_of_2(is_equal<int>, 2);
    auto is3 = bind_1st_of_2(is_equal<int>, 3);
    auto is4 = bind_1st_of_2(is_equal<int>, 4);

    assert(find_first_by(is3, xs) == just(3));
    assert(find_first_by(is4, xs) == nothing<int>());
    assert(find_first_idx_by(is2, xs) == just<size_t>(1));
    assert(find_first_idx_by(is4, xs) == nothing<size_t>());
    assert(find_first_idx(2, xs) == just<size_t>(1));
    assert(find_first_idx(4, xs) == nothing<size_t>());

    assert(find_last_by(is3, xs) == just(3));
    assert(find_last_by(is4, xs) == nothing<int>());
    assert(find_last_idx_by(is2, xs) == just<size_t>(4));
    assert(find_last_idx_by(is4, xs) == nothing<size_t>());
    assert(find_last_idx(2, xs) == just<size_t>(4));
    assert(find_last_idx(4, xs) == nothing<size_t>());

    assert(nth_element(2, xs) == 2);

    IntPair intPair = std::make_pair(2, 3);
    assert(fst(intPair) == 2);
    assert(snd(intPair) == 3);
    assert(swap_pair_elems(intPair) == std::make_pair(3, 2));
    assert(transform_fst(squareLambda, intPair) == std::make_pair(4, 3));
    assert(transform_snd(squareLambda, intPair) == std::make_pair(2, 9));

    assert(contains(2, xs) == true);
    assert(contains(4, xs) == false);

    assert(find_all_instances_of(std::string("Plus"),
        std::string("C Plus Plus is a nice language,") +
        std::string(" and FunctionalPlus makes it even nicer."))
        == std::list<std::size_t>({ 2, 7, 46 }));
    assert(find_all_instances_of(std::string("xx"), std::string("bxxxxc"))
        == std::list<std::size_t>({ 1, 2, 3 }));

    IntList v789 = { 7,8,9 };
    assert(set_range(1, v789, intList) == IntList({ 1,7,8,9,2 }));
    assert(get_range(1, 4, intList) == IntList({ 2,2,3 }));
    assert(replace_elems(2, 5, xs) == IntVector({1,5,5,3,5}));
    assert(replace_tokens(std::string("123"), std::string("_"),
            std::string("--123----123123")) == std::string("--_----__"));
    assert(take(2, xs) == IntVector({ 1,2 }));
    assert(drop(2, xs) == IntVector({ 2,3,2 }));
    assert(take_while(is_odd, xs) == IntVector({ 1 }));
    assert(drop_while(is_odd, xs) == IntVector({ 2,2,3,2 }));
    assert(keep_if(is2, xs) == IntVector({ 2,2,2 }));
    assert(keep_if(is3, xs) == IntVector({ 3 }));
    assert(keep_if(is4, xs) == IntVector());
    assert(find_all_idxs_of(2, xs) == IdxList({ 1,2,4 }));
    assert(count(2, xs) == 3);
    assert(is_infix_of(IntVector({2,3}), xs) == true);
    assert(is_infix_of(IntVector({2,1}), xs) == false);
    assert(is_prefix_of(IntVector({ 1,2 }), xs) == true);
    assert(is_prefix_of(IntVector({ 2,2 }), xs) == false);
    assert(is_suffix_of(IntVector({ 3,2 }), xs) == true);
    assert(is_suffix_of(IntVector({ 2,2 }), xs) == false);
    assert(is_subsequence_of(IntVector({ 1,3 }), xs) == true);
    assert(is_subsequence_of(IntVector({ 3,1 }), xs) == false);
    assert(is_subsequence_of(IntVector({ 3,1 }), xs) == false);
    typedef std::vector<IntVector> IntGrid2d;
    assert(transpose(IntGrid2d({})) == IntGrid2d({}));
    assert(transpose(IntGrid2d({ { 1, 2 } }))
            == IntGrid2d({ { 1 }, { 2 } }));
    assert(transpose(IntGrid2d({ { 1, 2 }, { 3, 4 } }))
            == IntGrid2d({ { 1, 3 }, { 2, 4 } }));
    assert(transpose(IntGrid2d({ { 1, 2, 3 }, { 4, 5, 6 } }))
            == IntGrid2d({ { 1, 4 }, { 2, 5 }, { 3, 6 } }));

    assert(sample(3, xs).size() == 3);

    typedef std::vector<ExplicitFromIntStruct> ExplicitFromIntStructs;
    ExplicitFromIntStructs explicitFromIntStructs = {
        ExplicitFromIntStruct(1),
        ExplicitFromIntStruct(2),
        ExplicitFromIntStruct(2),
        ExplicitFromIntStruct(3),
        ExplicitFromIntStruct(2)
    };

    assert(convert_elems<ExplicitFromIntStruct>(xs) == explicitFromIntStructs);

    assert(transform_with_idx(std::plus<int>(), xs) == IntVector({1+0,2+1,2+2,3+3,2+4}));

    int countUpCounter = 0;
    auto countUp = [countUpCounter]() mutable { return countUpCounter++; };
    assert(generate<IntVector>(countUp, 3) == IntVector({ 0,1,2 }));
    assert(generate_by_idx<IntVector>(squareLambda, 3) == IntVector({ 0,1,4 }));

    auto sumis_even = [&](std::size_t x, int y) { return is_even(x + y); };
    assert(keep_by_idx(is_even, xs) == IntVector({ 1,2,2 }));
    assert(keep_if_with_idx(sumis_even, xs) == IntVector({ 2,3,2 }));

    assert(nub(xs) == IntVector({ 1,2,3 }));
    auto bothEven = bind_1st_of_3(is_equal_by<decltype(is_even), int>, is_even);
    assert(nub_by(bothEven, xs) == IntVector({ 1,2 }));

    typedef std::map<int, std::string> IntStringMap;
    typedef std::map<std::string, int> StringIntMap;
    IntStringMap intStringMap = {{1, "2"}, {4, "53"}, {7, "21"}};
    StringIntMap stringIntMap = {{ "2", 1}, { "53", 4}, { "21", 7}};
    assert(swap_keys_and_values(intStringMap) == stringIntMap);

    typedef std::vector<std::string> StringVector;
    assert(get_map_keys(intStringMap) == IntVector({1, 4, 7}));
    assert(get_map_values(intStringMap) == StringVector({"2", "53", "21"}));

    typedef std::unordered_map<int, std::string> IntStringUnorderedMap;
    typedef std::unordered_map<std::string, int> StringIntUnorderedMap;
    IntStringUnorderedMap intStringUnorderedMap = { { 1, "2" },{ 4, "53" },{ 7, "21" } };
    StringIntUnorderedMap stringIntUnorderedMapSwapped = { { "2", 1 },{ "53", 4 },{ "21", 7 } };
    assert(swap_keys_and_values(intStringUnorderedMap) == stringIntUnorderedMapSwapped);
    assert(convert_container<IntStringUnorderedMap>(intStringMap) == intStringUnorderedMap);
    assert(convert_container<IntStringMap>(intStringUnorderedMap) == intStringMap);

    std::vector<int> mapInts = { 1, 4, 7 };
    std::vector<std::string> mapStrings = { "2", "53", "21" };
    assert(create_map(mapInts, mapStrings) == intStringMap);
    assert(create_unordered_map(mapInts, mapStrings) == intStringUnorderedMap);

    assert(get_from_map(intStringMap, 1) == just<std::string>("2"));
    assert(get_from_map(intStringMap, 9) == nothing<std::string>());
    assert(get_from_map_with_def(intStringMap, std::string("n/a"), 1) == "2");
    assert(get_from_map_with_def(intStringMap, std::string("n/a"), 9) == "n/a");
    assert(map_contains(intStringMap, 1) == true);
    assert(map_contains(intStringMap, 9) == false);

    assert(split_at_idx(2, xs) == std::make_pair(IntVector({1,2}), IntVector({2,3,2})));
    assert(partition(is_even, xs) == std::make_pair(IntVector({2,2,2,}), IntVector({1,3})));

    auto splittedAt1And3 = split_at_idxs(IdxVector({1,3}), xs);
    IntVectors splittedAt1And3Dest = {IntVector({1}), IntVector({2,2}), IntVector({3,2})};
    assert(splittedAt1And3 == splittedAt1And3Dest);
    assert(split_by(is_even, true, IntList({1,3,2,2,5,5,3,6,7,9})) == IntLists({{1,3},{},{5,5,3},{7,9}}));
    typedef std::map<int, std::size_t> IntSizeTMap;
    IntSizeTMap OccurrencesResult = {{1, 1}, {2, 3}, {3, 1}};
    assert(count_occurrences(xs) == OccurrencesResult);

    assert(replace_range(2, IntVector({8,9}), xs) == IntVector({1,2,8,9,2}));
    assert(insert_at(2, IntVector({8,9}), xs) == IntVector({1,2,8,9,2,3,2}));

    assert(sum(convert<std::vector<int>>(std::string("hello"))) == 532);
}

void Test_StringTools()
{
    using namespace fplus;
    std::string untrimmed = "  \n \t   foo  ";
    assert(trim_whitespace_left(untrimmed) == "foo  ");
    assert(trim_whitespace_right(untrimmed) == "  \n \t   foo");
    assert(trim_whitespace(untrimmed) == "foo");
    std::string text = "Hi,\nI am a\r\n***strange***\n\rstring.";
    std::list<std::string> textAsLinesWithEmty = {
        std::string("Hi,"),
        std::string("I am a"),
        std::string("***strange***"),
        std::string(""),
        std::string("string.") };
    std::list<std::string> textAsLinesWithoutEmpty = {
        std::string("Hi,"),
        std::string("I am a"),
        std::string("***strange***"),
        std::string("string.") };
    std::list<std::string> textAsWords = {
        std::string("Hi"),
        std::string("I"),
        std::string("am"),
        std::string("a"),
        std::string("strange"),
        std::string("string") };

    assert(split_lines(text, true)
            == textAsLinesWithEmty);
    assert(split_lines(text, false)
            == textAsLinesWithoutEmpty);
    assert(split_words(text) == textAsWords);
}

bool is_odd(int x) { return x % 2 == 1; }
void Test_example_KeepIf()
{
    typedef std::vector<int> Ints;
    Ints numbers = { 24, 11, 65, 44, 80, 18, 73, 90, 69, 18 };

    { // Version 1: hand written range based for loop
    Ints odds;
    for (int x : numbers)
        if (is_odd(x))
            odds.push_back(x);
    }

    { // Version 2: STL
    Ints odds;
    std::copy_if(std::begin(numbers), std::end(numbers),
            std::back_inserter(odds), is_odd);
    }

    { // Version : FunctionalPlus
    auto odds = fplus::keep_if(is_odd, numbers);
    }
}

void run_n_times(std::function<std::list<int>(std::list<int>)> f,
    std::size_t n, const std::string& name, const std::list<int>& inList)
{
    typedef std::chrono::time_point<std::chrono::system_clock> Time;
    Time startTime = std::chrono::system_clock::now();
    size_t lengthSum = 0;
    for (size_t i = 0; i < n; ++i)
    {
        lengthSum += f(inList).size();
    }
    Time endTime = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = endTime - startTime;
    std::cout << name << "(check: " << lengthSum << "), elapsed time: " << elapsed_seconds.count() << "s\n";
}

void Test_example_KeepIf_performance()
{
    using namespace fplus;

    typedef std::list<int> Ints;
    auto run_loop = [&](const Ints numbers)
    {
        Ints odds;
        for (int x : numbers)
            if (is_odd(x))
                odds.push_back(x);
        return odds;
    };
    auto run_stl = [&](const Ints numbers)
    {
        Ints odds;
            std::copy_if(std::begin(numbers), std::end(numbers),
                    std::back_inserter(odds), is_odd);
        return odds;
    };
    auto run_FunctionalPlus = [&](const Ints numbers)
        { return keep_if(is_odd, numbers); };

    // make debug runs faster
#if defined NDEBUG || defined _DEBUG
    std::size_t numRuns = 10;
#else
    std::size_t numRuns = 1000;
#endif

    Ints numbers = generate<Ints>(rand, 10000);
    run_n_times(run_loop, numRuns, "Hand-written for loop", numbers);
    run_n_times(run_stl, numRuns, "std::copy_if", numbers);
    run_n_times(run_FunctionalPlus, numRuns, "FunctionalPlus::keep_if", numbers);
}


void Test_example_SameOldSameOld()
{
    std::list<std::string> things = {"same old", "same old"};
    if (fplus::all_the_same(things))
        std::cout << "All things being equal." << std::endl;
}

void Test_example_IInTeam()
{
    std::string team = "Our team is great. I love everybody.";
    if (fplus::contains("I", fplus::split_words(team)))
        std::cout << "There actually is an I in team." << std::endl;
}

struct Entity
{
    Entity() : calm_(true), bright_(true) {}
    bool calm_;
    bool bright_;
};

void Test_example_AllIsCalmAndBright()
{
    auto isCalm = [](const Entity& e) { return e.calm_; };
    auto isBright = [](const Entity& e) { return e.bright_; };
    std::vector<Entity> entities(4);
    if (fplus::all_by(fplus::logical_and(isCalm, isBright), entities))
        std::cout << "Silent night." << std::endl;
}

std::list<std::uint64_t> collatz_seq(std::uint64_t x)
{
    std::list<std::uint64_t> result;
    while (x > 1)
    {
        result.push_back(x);
        if (x % 2 == 0)
            x = x / 2;
        else
            x = 3 * x + 1;
    }
    result.push_back(x);
    return result;
}

void Test_example_CollatzSequence()
{
    typedef std::list<uint64_t> Ints;

    // [1, 2, 3 ... 29]
    auto numbers = fplus::generate_integral_range<Ints>(1, 30);

    // A function that does [1, 2, 3, 4, 5] -> "[1 => 2 => 3 => 4 => 5]"
    auto show_ints = fplus::bind_1st_of_2(fplus::show_cont_with<Ints>, " => ");

    // A composed function that calculates a Collatz sequence and shows it.
    auto show_collats_seq = fplus::compose(collatz_seq, show_ints);

    // Apply it to all our numbers.
    auto seq_strs = fplus::transform(show_collats_seq, numbers);

    // Combine the numbers and their sequence representations into a map.
    auto collatz_dict = fplus::create_map(numbers, seq_strs);

    // Print some of the sequences.
    std::cout << collatz_dict[13] << std::endl;
    std::cout << collatz_dict[17] << std::endl;
}

int main()
{
    std::cout << "Running all tests." << std::endl;

    std::cout << "Testing Numeric." << std::endl;
        Test_Numeric();
    std::cout << "Numeric OK." << std::endl;

    std::cout << "Testing FunctionTraits." << std::endl;
    Test_FunctionTraits();
    std::cout << "FunctionTraits OK." << std::endl;

    std::cout << "Testing Composition." << std::endl;
    typedef std::vector<int> IntVec;
    typedef std::vector<IntVec> IntVecVec;
    typedef std::list<int> IntList;
    typedef std::list<IntList> IntListList;
    typedef std::deque<int> IntDeq;
    typedef std::deque<IntDeq> IntDeqDeq;
    Test_Composition<IntVec, IntVecVec>();
    Test_Composition<IntList, IntListList>();
    Test_Composition<IntDeq, IntDeqDeq>();
    std::cout << "Composition OK." << std::endl;

    std::cout << "Testing Maybe." << std::endl;
    Test_Maybe();
    std::cout << "Maybe OK." << std::endl;

    std::cout << "Testing Compare." << std::endl;
    Test_Compare();
    std::cout << "Compare OK." << std::endl;

    std::cout << "Testing ContainerTools." << std::endl;
    Test_ContainerTools();
    std::cout << "ContainerTools OK." << std::endl;

    std::cout << "Testing StringTools." << std::endl;
    Test_StringTools();
    std::cout << "StringTools OK." << std::endl;

    std::cout << "Testing Applications." << std::endl;
    Test_example_KeepIf();
    Test_example_KeepIf_performance();
    Test_example_SameOldSameOld();
    Test_example_IInTeam();
    Test_example_AllIsCalmAndBright();
    Test_example_CollatzSequence();
    std::cout << "Applications OK." << std::endl;

    std::cout << "All OK." << std::endl;
}