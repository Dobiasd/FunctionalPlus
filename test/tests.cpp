// Copyright Tobias Hermann 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "FunctionalPlus/FunctionalPlus.h"

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
    using namespace FunctionalPlus;

    assert(IsInRange(1, 3, 1) == true);
    assert(IsInRange(1, 3, 2) == true);
    assert(IsInRange(1, 3, 0) == false);
    assert(IsInRange(1, 3, 3) == false);

    assert(IsNegative(0.1) == false);
    assert(IsPositive(0.1) == true);
    assert(IsNegative(-0.1) == true);
    assert(IsPositive(-0.1) == false);

    assert(Round(1.4) == 1);
    assert(Round(1.6) == 2);
    assert(Floor(1.4) == 1);
    assert(Ceil(1.4) == 2);

    assert(Round(-1.4) == -1);
    assert(Round(-1.6) == -2);
    assert(Floor(-1.4) == -2);
    assert(Ceil(-1.4) == -1);

    assert(Clamp(2, 6, 5) == 5);
    assert(Clamp(2, 6, 1) == 2);
    assert(Clamp(2, 6, 8) == 6);
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
    State(int x) : x_(x) {}
    void Add(int y) { x_ += y; }
    int Get() const { return x_; }
private:
    int x_;
};

template <typename IntCont, typename IntContCont>
void Test_Composition()
{

    using namespace FunctionalPlus;
    auto square = [](int x){ return x*x; };

    typedef IntCont Row;
    Row row = {1,2,3};

    typedef IntContCont Mat;
    Mat mat;
    auto squareRowElems = Bind1of2(Transform<decltype(square), Row>,
            square);
    Row squaredRow = squareRowElems(row);
    assert(squaredRow == Row({1,4,9}));

    assert((Compose(square, square)(2)) == 16);
    assert((Compose(square, square, square)(2)) == 256);
    assert((Compose(square, square, square, square)(2)) == 65536);
    assert((Compose(square, square, square, square, square)(1)) == 1);
    auto add3 = [](int x, int y, int z) { return x + y + z; };
    assert(Bind2of3(add3, 3, 5)(7) == 15);
    auto APlusTwoTimesB = [](int a, int b) { return a + 2 * b; };
    auto TwoTimesAPlusB = [](int a, int b) { return 2 * a + b; };
    assert((Flip(APlusTwoTimesB)(2, 1)) == 5);
    assert((Flip(TwoTimesAPlusB)(1, 2)) == 5);
    auto is1 = [](int x) { return x == 1; };
    auto is2 = [](int x) { return x == 2; };
    assert((Not(is1)(1)) == false);
    assert((Not(is1)(2)) == true);

    assert((Or(is1, is2)(1)) == true);
    assert((Or(is1, is2)(2)) == true);
    assert((Or(is1, is2)(3)) == false);
    assert((And(is1, is2)(1)) == false);
    assert((And(is1, is1)(1)) == true);
    assert((Xor(is1, is1)(1)) == false);
    assert((Xor(is2, is1)(1)) == true);
    assert((Xor(is2, is2)(1)) == false);

    assert((ApplyToPair(APlusTwoTimesB, std::make_pair(1, 2))) == 5);
    assert((ApplyToPair(APlusTwoTimesBFunc, std::make_pair(1, 2))) == 5);

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
    using namespace FunctionalPlus;
    using namespace std;
    auto square = [](int x){ return x*x; };
    auto sqrtToMaybe = [](float x) {
        return x < 0.0f ? Nothing<float>() :
                Just(sqrt(static_cast<float>(x)));
    };
    auto sqrtToMaybeInt = [](int x) {
        return x < 0.0f ? Nothing<int>() :
                Just(Round(sqrt(static_cast<float>(x))));
    };
    auto IntToFloat = [](const int& x) { return static_cast<float>(x); };

    Maybe<int> x(2);
    Maybe<int> y = Nothing<int>();
    auto Or42 = Bind1of2(WithDefault<int>, 42);
    auto SquareAndSquare = Compose(square, square);
    assert(Or42(x) == 2);
    assert(Or42(y) == 42);
    auto squareMaybe = Lift(square);
    auto sqrtAndSqrt = AndThen(sqrtToMaybe, sqrtToMaybe);
    assert(squareMaybe(x) == Just(4));
    assert(squareMaybe(y) == Nothing<int>());
    assert((Lift(SquareAndSquare))(x) == Just(16));
    auto LiftedIntToFloat = Lift(IntToFloat);
    auto JustInt = Just<int>;
    auto IntToMaybeFloat = Compose(JustInt, LiftedIntToFloat);
    auto IntToFloatAndSqrtAndSqrt = AndThen(IntToMaybeFloat, sqrtAndSqrt);
    assert(IsInRange(1.41f, 1.42f, UnsafeGetJust<float>
            (IntToFloatAndSqrtAndSqrt(4))));
    typedef vector<Maybe<int>> IntMaybes;
    typedef vector<int> Ints;
    IntMaybes maybes = {Just(1), Nothing<int>(), Just(2)};
    Ints justs = {1,2};
    assert(Justs(maybes) == justs);
    assert(Just(1) == Just(1));
    assert(Just(1) != Just(2));
    assert(Just(1) != Nothing<int>());
    assert(Nothing<int>() == Nothing<int>());

    Ints wholeNumbers = { -3, 4, 16, -1 };
    assert(TransformAndKeepJusts(sqrtToMaybeInt, wholeNumbers)
            == Ints({2,4}));
    assert(TransformAndConcat(Bind1of2(Replicate<Ints>, 3), Ints{ 1,2 })
            == Ints({ 1,1,1,2,2,2 }));
}

void Test_Compare()
{
    using namespace FunctionalPlus;
    assert(IsEqual(2, 2));
    assert(!IsEqual(2, 3));
    assert(!IsNotEqual(2, 2));
    assert(IsNotEqual(2, 3));

    assert(!IsLess(2, 2));
    assert(IsLess(2, 3));
    assert(!IsLess(3, 2));

    assert(IsLessOrEqual(2, 2));
    assert(IsLessOrEqual(2, 3));
    assert(!IsLessOrEqual(3, 2));

    assert(!IsGreater(2, 2));
    assert(!IsGreater(2, 3));
    assert(IsGreater(3, 2));

    assert(IsGreaterOrEqual(2, 2));
    assert(!IsGreaterOrEqual(2, 3));
    assert(IsGreaterOrEqual(3, 2));

    assert(Identity(2) == 2);
    assert(Always(2, 5) == 2);
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
    using namespace FunctionalPlus;
    using namespace std;

    auto squareLambda = [](int x) { return x*x; };
    std::function<int(int)> squareStdFunction = squareLambda;

    auto isEven = [](int x){ return x % 2 == 0; };
    auto isOdd = [](int x){ return x % 2 == 1; };
    typedef pair<int, int> IntPair;
    typedef vector<int> IntVector;
    typedef vector<IntVector> IntVectors;
    typedef vector<bool> BoolVector;
    IntVector xs = {1,2,2,3,2};
    IntVector xsSorted = {1,2,2,2,3};
    string xsShown("[1, 2, 2, 3, 2]");
    IntVector xs2Times = {1,2,2,3,2,1,2,2,3,2};

    typedef list<int> IntList;
    typedef list<IntList> IntLists;
    IntList intList = { 1,2,2,3,2 };
    IntLists intLists = { { 1 },{ 2,2 },{ 3 },{ 2 } };
    assert(Group(intList) == intLists);

    typedef list<std::size_t> IdxList;

    assert(Transform(squareLambda, xs) == IntVector({1,4,4,9,4}));
    assert(KeepIf(isEven, xs) == IntVector({2,2,2}));
    assert(DropIf(isEven, xs) == IntVector({1,3}));
    assert(Transform(squareLambda, intList) == IntList({ 1,4,4,9,4 }));
    assert(KeepIf(isEven, intList) == IntList({ 2,2,2 }));
    assert(DropIf(isEven, intList) == IntList({ 1,3 }));
    assert(Group(xs) == std::list<IntVector>({IntVector({1}),IntVector({2,2}),IntVector({3}),IntVector({2})}));
    assert(Without(2, intList) == IntList({ 1,3 }));

    assert(TransformConvert<IntList>(squareLambda, xs) == IntList({ 1,4,4,9,4 }));

    assert(IsEqualByAndBy(isEven, isEven, 2, 4) == true);
    assert(IsEqualByAndBy(isEven, isEven, 1, 2) == false);
    assert(IsEqualByAndBy(isOdd, isEven, 1, 2) == true);
    assert(IsEqual(2, 2) == true);
    assert(IsEqual(1, 2) == false);

    assert(IsEmpty(xs) == false);
    assert(IsEmpty(IntVector()) == true);
    assert(IsNotEmpty(xs) == true);
    assert(IsNotEmpty(IntVector()) == false);
    assert(ConvertContainer<IntList>(xs) == intList);
    assert(Append(xs, xs) == xs2Times);

    typedef std::vector<float> FloatVector;
    assert(ConvertElems<float>(xs) == FloatVector({ 1.0f,2.0f,2.0f,3.0f,2.0f }));

    assert(Concat(intLists) == intList);
    assert(Concat(IntVectors(2, xs)) == xs2Times);
    assert(Repeat(2, xs) == xs2Times);
    assert(Intersperse(0, xs) == IntVector({1,0,2,0,2,0,3,0,2}));
    assert(Foldl(std::plus<int>(), 100, xs) == 110);
    assert(Foldr(std::plus<int>(), 100, xs) == 110);
    auto appendXToStrForFoldL = [](const std::string& str, int x) { return str + std::to_string(x); };
    auto appendXToStrForFoldR = [](int x, const std::string& str) { return str + std::to_string(x); };
    std::string emptyString;
    assert(Foldl(appendXToStrForFoldL, emptyString, xs) == "12232");
    assert(Foldr(appendXToStrForFoldR, emptyString, xs) == "23221");

    assert(Scanl(std::plus<int>(), 20, xs) == IntVector({ 20,21,23,25,28,30 }));
    assert(Scanr(std::plus<int>(), 20, xs) == IntVector({ 30,29,27,25,22,20 }));

    assert(Join(IntList({0}), intLists)
            == IntList({1,0,2,2,0,3,0,2}));
    assert(ShowCont(xs) == xsShown);
    assert(ShowContWith(", ", xs) == xsShown);
    assert(Show<int>(1) == "1");
    auto multiply = [](int x, int y){ return x * y; };
    assert(ZipWith(multiply, xs, xs)
            == Transform(squareLambda, xs));

    auto xsZippedWithXs = Zip(xs, xs);
    assert(Unzip(xsZippedWithXs).first == xs);
    assert(All(BoolVector()) == true);
    assert(All(BoolVector({true})) == true);
    assert(All(BoolVector({false})) == false);
    assert(All(BoolVector({true, true})) == true);
    assert(All(BoolVector({true, false})) == false);

    assert(AllBy(isEven, IntVector()) == true);
    assert(AllBy(isEven, IntVector({2})) == true);
    assert(AllBy(isEven, IntVector({1})) == false);
    assert(AllBy(isEven, IntVector({2, 2})) == true);
    assert(AllBy(isEven, IntVector({2, 1})) == false);

    assert(Any(BoolVector()) == false);
    assert(Any(BoolVector({true})) == true);
    assert(Any(BoolVector({false})) == false);
    assert(Any(BoolVector({false, false})) == false);
    assert(Any(BoolVector({true, false})) == true);

    assert(AnyBy(isEven, IntVector()) == false);
    assert(AnyBy(isEven, IntVector({2})) == true);
    assert(AnyBy(isEven, IntVector({1})) == false);
    assert(AnyBy(isEven, IntVector({1, 1})) == false);
    assert(AnyBy(isEven, IntVector({2, 1})) == true);

    assert(None(BoolVector()) == true);
    assert(None(BoolVector({true})) == false);
    assert(None(BoolVector({false})) == true);
    assert(None(BoolVector({false, false})) == true);
    assert(None(BoolVector({true, false})) == false);

    assert(NoneBy(isEven, IntVector()) == true);
    assert(NoneBy(isEven, IntVector({2})) == false);
    assert(NoneBy(isEven, IntVector({1})) == true);
    assert(NoneBy(isEven, IntVector({1, 1})) == true);
    assert(NoneBy(isEven, IntVector({2, 1})) == false);

    assert(Minimum(xs) == 1);
    assert(Maximum(xs) == 3);

    assert(MinimumBy(greater<int>(), xs) == 3);
    assert(MaximumBy(greater<int>(), xs) == 1);

    assert(Size(xs) == 5);
    assert(Size(IntVector()) == 0);
    assert(IsNotEmpty(xs) == true);


    assert(Sum(xs) == 10);
    assert(Mean<int>(xs) == 2);
    assert(Median(IntVector({ 3 })) == 3);
    assert(Median(IntVector({ 3, 5 })) == 4);
    assert(Median(IntVector({ 3, 9, 5 })) == 5);
    assert(Median(xs) == 2);
    assert(Sort(Reverse(xs)) == xsSorted);
    assert(SortBy(greater<int>(), xs) == Reverse(xsSorted));
    assert(Unique(xs) == IntVector({1,2,3,2}));
    auto IsEqualByIsEven = [&](int a, int b)
            { return isEven(a) == isEven(b); };
    assert(UniqueBy(IsEqualByIsEven, xs) == IntVector({1,2,3,2}));

    assert(AllTheSame(IntVector()) == true);
    assert(AllTheSame(IntVector({1})) == true);
    assert(AllTheSame(IntVector({1,1,1})) == true);
    assert(AllTheSame(IntVector({1,2,1})) == false);

    assert(AllUniqueEq(IntVector()) == true);
    assert(AllUniqueEq(IntVector({1})) == true);
    assert(AllUniqueEq(IntVector({1,2,1})) == false);
    assert(AllUniqueEq(IntVector({1,2,3})) == true);

    assert(AllUniqueLess(IntVector()) == true);
    assert(AllUniqueLess(IntVector({ 1 })) == true);
    assert(AllUniqueLess(IntVector({ 1,2,1 })) == false);
    assert(AllUniqueLess(IntVector({ 1,2,3 })) == true);

    assert(IsSorted(IntVector()) == true);
    assert(IsSorted(IntVector({1})) == true);
    assert(IsSorted(IntVector({1,2,3})) == true);
    assert(IsSorted(IntVector({1,2,2})) == true);
    assert(IsSorted(IntVector({1,2,1})) == false);

    auto is2 = Bind1of2(IsEqual<int>, 2);
    auto is3 = Bind1of2(IsEqual<int>, 3);
    auto is4 = Bind1of2(IsEqual<int>, 4);

    assert(FindFirstBy(is3, xs) == Just(3));
    assert(FindFirstBy(is4, xs) == Nothing<int>());
    assert(FindFirstIdxBy(is2, xs) == Just<size_t>(1));
    assert(FindFirstIdxBy(is4, xs) == Nothing<size_t>());
    assert(FindFirstIdx(2, xs) == Just<size_t>(1));
    assert(FindFirstIdx(4, xs) == Nothing<size_t>());

    assert(FindLastBy(is3, xs) == Just(3));
    assert(FindLastBy(is4, xs) == Nothing<int>());
    assert(FindLastIdxBy(is2, xs) == Just<size_t>(4));
    assert(FindLastIdxBy(is4, xs) == Nothing<size_t>());
    assert(FindLastIdx(2, xs) == Just<size_t>(4));
    assert(FindLastIdx(4, xs) == Nothing<size_t>());

    assert(NthElement(2, xs) == 2);

    IntPair intPair = make_pair(2, 3);
    assert(Fst(intPair) == 2);
    assert(Snd(intPair) == 3);
    assert(SwapPairElems(intPair) == make_pair(3, 2));
    assert(TransformFst(squareLambda, intPair) == make_pair(4, 3));
    assert(TransformSnd(squareLambda, intPair) == make_pair(2, 9));

    assert(Contains(2, xs) == true);
    assert(Contains(4, xs) == false);

    assert(FindAllInstancesOf(string("Plus"),
        string("C Plus Plus is a nice language,") +
        string(" and FunctionalPlus makes it even nicer."))
        == std::list<std::size_t>({ 2, 7, 46 }));
    assert(FindAllInstancesOf(string("xx"), string("bxxxxc"))
        == std::list<std::size_t>({ 1, 2, 3 }));

    IntList v789 = { 7,8,9 };
    assert(SetRange(1, v789, intList) == IntList({ 1,7,8,9,2 }));
    assert(GetRange(1, 4, intList) == IntList({ 2,2,3 }));
    assert(ReplaceElems(2, 5, xs) == IntVector({1,5,5,3,5}));
    assert(ReplaceTokens(std::string("123"), std::string("_"),
            std::string("--123----123123")) == std::string("--_----__"));
    assert(Take(2, xs) == IntVector({ 1,2 }));
    assert(Drop(2, xs) == IntVector({ 2,3,2 }));
    assert(TakeWhile(isOdd, xs) == IntVector({ 1 }));
    assert(DropWhile(isOdd, xs) == IntVector({ 2,2,3,2 }));
    assert(KeepIf(is2, xs) == IntVector({ 2,2,2 }));
    assert(KeepIf(is3, xs) == IntVector({ 3 }));
    assert(KeepIf(is4, xs) == IntVector());
    assert(FindAllIdxsOf(2, xs) == IdxList({ 1,2,4 }));
    assert(Count(2, xs) == 3);
    assert(IsInfixOf(IntVector({2,3}), xs) == true);
    assert(IsInfixOf(IntVector({2,1}), xs) == false);
    assert(IsPrefixOf(IntVector({ 1,2 }), xs) == true);
    assert(IsPrefixOf(IntVector({ 2,2 }), xs) == false);
    assert(isSuffixOf(IntVector({ 3,2 }), xs) == true);
    assert(isSuffixOf(IntVector({ 2,2 }), xs) == false);
    assert(IsSubsequenceOf(IntVector({ 1,3 }), xs) == true);
    assert(IsSubsequenceOf(IntVector({ 3,1 }), xs) == false);
    assert(IsSubsequenceOf(IntVector({ 3,1 }), xs) == false);
    typedef std::vector<IntVector> IntGrid2d;
    assert(Transpose(IntGrid2d({})) == IntGrid2d({}));
    assert(Transpose(IntGrid2d({ { 1, 2 } }))
            == IntGrid2d({ { 1 }, { 2 } }));
    assert(Transpose(IntGrid2d({ { 1, 2 }, { 3, 4 } }))
            == IntGrid2d({ { 1, 3 }, { 2, 4 } }));
    assert(Transpose(IntGrid2d({ { 1, 2, 3 }, { 4, 5, 6 } }))
            == IntGrid2d({ { 1, 4 }, { 2, 5 }, { 3, 6 } }));

    typedef std::vector<ExplicitFromIntStruct> ExplicitFromIntStructs;
    ExplicitFromIntStructs explicitFromIntStructs = {
        ExplicitFromIntStruct(1),
        ExplicitFromIntStruct(2),
        ExplicitFromIntStruct(2),
        ExplicitFromIntStruct(3),
        ExplicitFromIntStruct(2)
    };

    assert(ConvertElems<ExplicitFromIntStruct>(xs) == explicitFromIntStructs);

    assert(TransformWithIdx(std::plus<int>(), xs) == IntVector({1+0,2+1,2+2,3+3,2+4}));

    int countUpCounter = 0;
    auto countUp = [countUpCounter]() mutable { return countUpCounter++; };
    assert(Generate<IntVector>(countUp, 3) == IntVector({ 0,1,2 }));
    assert(GenerateByIdx<IntVector>(squareLambda, 3) == IntVector({ 0,1,4 }));

    auto sumIsEven = [&](std::size_t x, int y) { return isEven(x + y); };
    assert(KeepByIdx(isEven, xs) == IntVector({ 1,2,2 }));
    assert(KeepIfWithIdx(sumIsEven, xs) == IntVector({ 2,3,2 }));

    assert(Nub(xs) == IntVector({ 1,2,3 }));
    auto bothEven = Bind1of3(IsEqualBy<decltype(isEven), int>, isEven);
    assert(NubBy(bothEven, xs) == IntVector({ 1,2 }));

    typedef std::map<int, std::string> IntStringMap;
    typedef std::map<std::string, int> StringIntMap;
    IntStringMap intStringMap = {{1, "2"}, {4, "53"}, {7, "21"}};
    StringIntMap stringIntMap = {{ "2", 1}, { "53", 4}, { "21", 7}};
    assert(SwapKeysAndValues(intStringMap) == stringIntMap);

    typedef std::vector<std::string> StringVector;
    assert(GetMapKeys(intStringMap) == IntVector({1, 4, 7}));
    assert(GetMapValues(intStringMap) == StringVector({"2", "53", "21"}));

    typedef std::unordered_map<int, std::string> IntStringUnorderedMap;
    typedef std::unordered_map<std::string, int> StringIntUnorderedMap;
    IntStringUnorderedMap intStringUnorderedMap = { { 1, "2" },{ 4, "53" },{ 7, "21" } };
    StringIntUnorderedMap stringIntUnorderedMapSwapped = { { "2", 1 },{ "53", 4 },{ "21", 7 } };
    assert(SwapKeysAndValues(intStringUnorderedMap) == stringIntUnorderedMapSwapped);
    assert(ConvertContainer<IntStringUnorderedMap>(intStringMap) == intStringUnorderedMap);
    assert(ConvertContainer<IntStringMap>(intStringUnorderedMap) == intStringMap);

    std::vector<int> mapInts = { 1, 4, 7 };
    std::vector<std::string> mapStrings = { "2", "53", "21" };
    assert(CreateMap(mapInts, mapStrings) == intStringMap);
    assert(CreateUnorderedMap(mapInts, mapStrings) == intStringUnorderedMap);

    typedef std::map<std::string, std::string> StringStringMap;
    StringStringMap stringStringMap = { { "1", "2" },{ "16", "53" },{ "49", "21" } };

    assert(GetFromMap(intStringMap, 1) == Just<std::string>("2"));
    assert(GetFromMap(intStringMap, 9) == Nothing<std::string>());
    assert(GetFromMapWithDef(intStringMap, std::string("n/a"), 1) == "2");
    assert(GetFromMapWithDef(intStringMap, std::string("n/a"), 9) == "n/a");
    assert(MapContains(intStringMap, 1) == true);
    assert(MapContains(intStringMap, 9) == false);

    typedef std::vector<std::size_t> IdxVector;
    assert(SplitAtIdx(2, xs) == std::make_pair(IntVector({1,2}), IntVector({2,3,2})));
    assert(Partition(isEven, xs) == std::make_pair(IntVector({2,2,2,}), IntVector({1,3})));

    auto splittedAt1And3 = SplitAtIdxs(IdxVector({1,3}), xs);
    IntVectors splittedAt1And3Dest = {IntVector({1}), IntVector({2,2}), IntVector({3,2})};
    assert(splittedAt1And3 == splittedAt1And3Dest);
    assert(SplitBy(isEven, true, IntList({1,3,2,2,5,5,3,6,7,9})) == IntLists({{1,3},{},{5,5,3},{7,9}}));

    assert(ReplaceRange(2, IntVector({8,9}), xs) == IntVector({1,2,8,9,2}));
    assert(InsertAt(2, IntVector({8,9}), xs) == IntVector({1,2,8,9,2,3,2}));

    assert(Sum(Convert<std::vector<int>>(string("hello"))) == 532);
}

void Test_StringTools()
{
    using namespace FunctionalPlus;
    std::string untrimmed = "  \n \t   foo  ";
    assert(TrimWhitespaceLeft(untrimmed) == "foo  ");
    assert(TrimWhitespaceRight(untrimmed) == "  \n \t   foo");
    assert(TrimWhitespace(untrimmed) == "foo");
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

    assert(SplitLines(text, true)
            == textAsLinesWithEmty);
    assert(SplitLines(text, false)
            == textAsLinesWithoutEmpty);
    assert(SplitWords(text) == textAsWords);
}

bool IsOdd(int x) { return x % 2 == 1; }
void Test_example_KeepIf()
{
    using namespace std;
    using namespace FunctionalPlus;

    typedef vector<int> Ints;
    Ints numbers = { 24, 11, 65, 44, 80, 18, 73, 90, 69, 18 };

    { // Version 1: hand written range based for loop
        Ints odds;
        for (int x : numbers)
            if (IsOdd(x))
                odds.push_back(x);
    }

    { // Version 2: STL
        Ints odds;
        copy_if(begin(numbers), end(numbers), back_inserter(odds), IsOdd);
    }

    { // Version : FunctionalPlus
        auto odds = KeepIf(IsOdd, numbers);
    }
}

void run_n_times(std::function<std::list<int>(std::list<int>)> f,
    std::size_t n, const std::string& name, const std::list<int>& inList)
{
    using namespace std;
    typedef chrono::time_point<std::chrono::system_clock> Time;
    Time startTime = chrono::system_clock::now();
    size_t lengthSum = 0;
    for (size_t i = 0; i < n; ++i)
    {
        lengthSum += f(inList).size();
    }
    Time endTime = chrono::system_clock::now();
    chrono::duration<double> elapsed_seconds = endTime - startTime;
    cout << name << "(check: " << lengthSum << "), elapsed time: " << elapsed_seconds.count() << "s\n";
}

void Test_example_KeepIf_performance()
{
    using namespace std;
    using namespace FunctionalPlus;

    typedef list<int> Ints;
    auto run_loop = [&](const Ints numbers)
    {
        Ints odds;
        for (int x : numbers)
            if (IsOdd(x))
                odds.push_back(x);
        return odds;
    };
    auto run_stl = [&](const Ints numbers)
    {
        Ints odds;
        copy_if(begin(numbers), end(numbers), back_inserter(odds), IsOdd);
        return odds;
    };
    auto run_FunctionalPlus = [&](const Ints numbers)
        { return KeepIf(IsOdd, numbers); };

    // make debug runs faster
    std::size_t numRuns = 1000;
#ifdef NDEBUG
    numRuns = 10;
#endif
#ifdef _DEBUG
    numRuns = 10;
#endif

    Ints numbers = Generate<Ints>(rand, 10000);
    run_n_times(run_loop, numRuns, "Hand-written for loop", numbers);
    run_n_times(run_stl, numRuns, "std::copy_if", numbers);
    run_n_times(run_FunctionalPlus, numRuns, "FunctionalPlus::KeepIf", numbers);
}



void Test_example_SameOldSameOld()
{
    using namespace FunctionalPlus;
    std::list<std::string> things = {"same old", "same old"};
    if (AllTheSame(things))
        std::cout << "All things being equal." << std::endl;
}

void Test_example_IInTeam()
{
    using namespace FunctionalPlus;
    std::string team = "Our team is great. I love everybody.";
    if (Contains("I", SplitWords(team)))
        std::cout << "There actually is an I in team." << std::endl;
}

struct Entity
{
    Entity() : calm_(true), bright_(true) {}
    bool calm_;
    bool bright_;
};
bool IsCalm(const Entity& entity) { return entity.calm_; }
bool IsBright(const Entity& entity) { return entity.bright_; }

void Test_example_AllIsCalmAndBright()
{
    using namespace FunctionalPlus;
    std::vector<Entity> entities(4);
    if (AllBy(And(IsCalm, IsBright), entities))
        std::cout << "Silent night." << std::endl;
}

std::list<std::uint64_t> CollatzSeq(std::uint64_t x)
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
    using namespace FunctionalPlus;
    using namespace std;

    typedef list<uint64_t> Ints;

    // [1, 2, 3 ... 29]
    auto numbers = GenerateIntegralRange<Ints>(1, 30);

    // A function that does [1, 2, 3, 4, 5] -> "[1 => 2 => 3 => 4 => 5]"
    auto ShowInts = Bind1of2(ShowContWith<Ints>, " => ");

    // A composed function that calculates a Collatz sequence and shows it.
    auto ShowCollatsSeq = Compose(CollatzSeq, ShowInts);

    // Apply it to all our numbers.
    auto seqStrs = Transform(ShowCollatsSeq, numbers);

    // Combine the numbers and their sequence representations into a map.
    auto collatzDict = CreateMap(numbers, seqStrs);

    // Print some of the sequences.
    cout << collatzDict[13] << endl;
    cout << collatzDict[17] << endl;
}

int main()
{
    using namespace std;
    cout << "Running all tests." << endl;

    cout << "Testing Numeric." << endl;
        Test_Numeric();
    cout << "Numeric OK." << endl;

    cout << "Testing FunctionTraits." << endl;
    Test_FunctionTraits();
    cout << "FunctionTraits OK." << endl;

    cout << "Testing Composition." << endl;
    typedef vector<int> IntVec;
    typedef vector<IntVec> IntVecVec;
    typedef list<int> IntList;
    typedef list<IntList> IntListList;
    typedef deque<int> IntDeq;
    typedef deque<IntDeq> IntDeqDeq;
    Test_Composition<IntVec, IntVecVec>();
    Test_Composition<IntList, IntListList>();
    Test_Composition<IntDeq, IntDeqDeq>();
    cout << "Composition OK." << endl;

    cout << "Testing Maybe." << endl;
    Test_Maybe();
    cout << "Maybe OK." << endl;

    cout << "Testing Compare." << endl;
    Test_Compare();
    cout << "Compare OK." << endl;

    cout << "Testing ContainerTools." << endl;
    Test_ContainerTools();
    cout << "ContainerTools OK." << endl;

    cout << "Testing StringTools." << endl;
    Test_StringTools();
    cout << "StringTools OK." << endl;

    cout << "Testing Applications." << endl;
    Test_example_KeepIf();
    Test_example_KeepIf_performance();
    Test_example_SameOldSameOld();
    Test_example_IInTeam();
    Test_example_AllIsCalmAndBright();
    Test_example_CollatzSequence();
    cout << "Applications OK." << endl;

    cout << "All OK." << endl;
}