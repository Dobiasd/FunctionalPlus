// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "fplus/fplus.hpp"
#include <vector>

using namespace testing;

namespace {

    int APlusTwoTimesBFunc(int a, int b)
    {
        return a + 2 * b;
    }

    typedef std::deque<int> IntDeq;
    typedef std::deque<IntDeq> IntContCont;
    typedef IntDeq IntCont;
    typedef IntCont Row;
}

class CompositionTestState {
public:
    explicit CompositionTestState(int x) : x_(x) {}
    void Add(int y) { x_ += y; }
    int Get() const { return x_; }
private:
    int x_;
};

TEST(composition_test, forward_apply)
{
    using namespace fplus;
    EXPECT_THAT(forward_apply(3, square<int>), Eq(9));
}

TEST(composition_test, parameter_binding)
{
    using namespace fplus;
    Row row = {1,2,3};

    typedef IntContCont Mat;
    Mat mat;
    auto square = [](int x){ return x*x; };
    auto squareRowElems = bind_1st_of_2(transform<decltype(square), Row>,
            square);
    Row squaredRow = squareRowElems(row);
    EXPECT_THAT(squaredRow, ElementsAre(1,4,9));

    auto int_division = [](int x, int y) { return x / y; };
    EXPECT_THAT(bind_2nd_of_2(int_division, 2)(6), Eq(3));

    auto add3 = [](int x, int y, int z) { return x + y + z; };
    EXPECT_THAT(bind_1st_and_2nd_of_3(add3, 3, 5)(7), Eq(15));
}

TEST(composition_test, compose)
{
    using namespace fplus;
    auto square = [](int x){ return x*x; };
    EXPECT_THAT((compose(square, square)(2)), Eq(16));
    EXPECT_THAT((compose(square, square, square)(2)), Eq(256));
    EXPECT_THAT((compose(square, square, square, square)(2)), Eq(65536));
    EXPECT_THAT((compose(square, square, square, square, square)(1)), Eq(1));
}

TEST(composition_test, flip)
{
    using namespace fplus;

    auto APlusTwoTimesB = [](int a, int b) { return a + 2 * b; };
    auto TwoTimesAPlusB = [](int a, int b) { return 2 * a + b; };
    EXPECT_THAT((flip(APlusTwoTimesB)(2, 1)), Eq(5));
    EXPECT_THAT((flip(TwoTimesAPlusB)(1, 2)), Eq(5));
}

TEST(composition_test, logical)
{
    using namespace fplus;
    auto is1 = [](int x) { return x == 1; };
    auto is2 = [](int x) { return x == 2; };
    EXPECT_FALSE((logical_not(is1)(1)));
    EXPECT_TRUE((logical_not(is1)(2)));

    EXPECT_TRUE((logical_or(is1, is2)(1)));
    EXPECT_TRUE((logical_or(is1, is2)(2)));
    EXPECT_FALSE((logical_or(is1, is2)(3)));
    EXPECT_FALSE((logical_and(is1, is2)(1)));
    EXPECT_TRUE((logical_and(is1, is1)(1)));
    EXPECT_FALSE((logical_xor(is1, is1)(1)));
    EXPECT_TRUE((logical_xor(is2, is1)(1)));
    EXPECT_FALSE((logical_xor(is2, is2)(1)));

}

TEST(composition_test, apply_to_pair)
{
    using namespace fplus;
    auto APlusTwoTimesB = [](int a, int b) { return a + 2 * b; };
    EXPECT_THAT((apply_to_pair(APlusTwoTimesB)(std::make_pair(1, 2))), Eq(5));
    EXPECT_THAT((apply_to_pair(APlusTwoTimesBFunc)(std::make_pair(1, 2))), Eq(5));
}

TEST(composition_test, state)
{
    using namespace fplus;
    CompositionTestState state(1);
    EXPECT_THAT(state.Get(), Eq(1));
    auto stateAdd = std::mem_fn(&CompositionTestState::Add);

    stateAdd(state, 2);
    EXPECT_THAT(state.Get(), Eq(3));

    //auto stateAddBoundFPP = Bind1of2(stateAdd, &state); // crashes VC2015 compiler
    //stateAddBoundFPP(3);

    auto stateAddBoundStl = std::bind(&CompositionTestState::Add, std::placeholders::_1, std::placeholders::_2);
    stateAddBoundStl(state, 3);
    EXPECT_THAT(state.Get(), Eq(6));
}
