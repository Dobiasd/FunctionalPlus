// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <doctest/doctest.h>
#include <fplus/fplus.hpp>
#include <vector>

namespace {

    int APlusTwoTimesBFunc(int a, int b)
    {
        return a + 2 * b;
    }

    typedef std::deque<int> IntDeq;
    typedef std::deque<IntDeq> IntContCont;
    typedef IntDeq IntCont;
    typedef IntCont Row;

    std::uint64_t fibo(std::uint64_t n)
    {
        if (n < 2)
            return n;
        else
            return fibo(n-1) + fibo(n-2);
    }
    // version using continuation passing style (CPS)
    std::uint64_t fibo_cont(const std::function<std::uint64_t(std::uint64_t)>& cont, std::uint64_t n)
    {
        if (n < 2)
            return n;
        else
            return cont(n-1) + cont(n-2);
    }
}

class CompositionTestState {
public:
    explicit CompositionTestState(int x) : x_(x) {}
    void Add(int y) { x_ += y; }
    int Get() const { return x_; }
private:
    int x_;
};

TEST_CASE("composition_test - forward_apply")
{
    using namespace fplus;
    REQUIRE_EQ(forward_apply(3, square<int>), 9);
    REQUIRE_EQ(forward_apply(3, [](auto x) { return x * x; }), 9);
}

TEST_CASE("composition_test - lazy")
{
    using namespace fplus;
    const auto square_3_stub = lazy(square<int>, 3);
    REQUIRE_EQ(square_3_stub(), 9);
    REQUIRE_EQ(lazy([](auto x) { return x * x; }, 3)(), 9);
}

TEST_CASE("composition_test - fixed")
{
    using namespace fplus;
    const auto lazy_3 = fixed(3);
    REQUIRE_EQ(lazy_3(), 3);
}

TEST_CASE("composition_test - parameter_binding")
{
    using namespace fplus;
    Row row = {1,2,3};

    typedef IntContCont Mat;
    Mat mat;
    auto square = [](int x){ return x*x; };
    auto squareRowElems =
        bind_1st_of_2(transform<decltype(square), const Row&>, square);
    auto add = [](auto x, auto y) { return x + y; };
    auto add4 = bind_1st_of_2(add, 4);
    REQUIRE_EQ(add4(2), 6);
    Row squaredRow = squareRowElems(row);
    REQUIRE_EQ(squaredRow, IntCont({1,4,9}));

    auto int_division = [](int x, int y) { return x / y; };
    REQUIRE_EQ(bind_2nd_of_2(int_division, 2)(6), 3);
    REQUIRE_EQ(bind_2nd_of_2([](auto x, auto y) { return x / y; }, 2)(6), 3);

    auto add3 = [](int x, int y, int z) { return x + y + z; };
    auto genericAdd3 = [](auto x, auto y, auto z) { return x + y + z; };

    REQUIRE_EQ(bind_1st_of_3(add3, 3)(30, 9), 42);
    REQUIRE_EQ(bind_1st_of_3(genericAdd3, 3)(30, 9), 42);

    REQUIRE_EQ(bind_1st_and_2nd_of_3(add3, 3, 5)(7), 15);
    REQUIRE_EQ(bind_1st_and_2nd_of_3(genericAdd3, 3, 5)(7), 15);

    REQUIRE_EQ(bind_2nd_and_3rd_of_3(add3, 3, 5)(7), 15);
    REQUIRE_EQ(bind_2nd_and_3rd_of_3(genericAdd3, 3, 5)(7), 15);
}

TEST_CASE("composition_test - compose")
{
    using namespace fplus;
    auto square = [](int x){ return x*x; };
    REQUIRE_EQ((compose(square, square)(2)), 16);
    REQUIRE_EQ((compose(square, square, square)(2)), 256);
    REQUIRE_EQ((compose(square, square, square, square)(2)), 65536);
    REQUIRE_EQ((compose(square, square, square, square, square)(1)), 1);
    REQUIRE_EQ((compose(std::multiplies<>{}, square)(4, 2)), 64);
}

TEST_CASE("composition_test - flip")
{
    using namespace fplus;

    auto APlusTwoTimesB = [](int a, int b) { return a + 2 * b; };
    auto TwoTimesAPlusB = [](int a, int b) { return 2 * a + b; };
    auto Minus = [](auto a, auto b, auto c) { return a - b - c; };
    REQUIRE_EQ((flip(APlusTwoTimesB)(2, 1)), 5);
    REQUIRE_EQ((flip(TwoTimesAPlusB)(1, 2)), 5);
    REQUIRE_EQ((flip(Minus)(1, 2, 3)), 0);
}

TEST_CASE("composition_test - logical")
{
    using namespace fplus;
    auto is1 = [](int x) { return x == 1; };
    auto is2 = [](auto x) { return x == 2; };
    REQUIRE_FALSE((logical_not(is1)(1)));
    REQUIRE((logical_not(is1)(2)));
    REQUIRE((logical_not(std::equal_to<>{})(2, 3)));

    REQUIRE((logical_or(is1, is2)(1)));
    REQUIRE((logical_or(is1, is2)(2)));
    REQUIRE_FALSE((logical_and(is1, is2)(1)));
    REQUIRE((logical_and([](auto x){ return x == 1; }, is1)(1)));
    REQUIRE_FALSE((logical_xor(is1, is1)(1)));
    REQUIRE((logical_xor(is2, is1)(1)));
    REQUIRE_FALSE((logical_xor(is2, is2)(1)));
}

TEST_CASE("composition_test - apply_to_pair")
{
    using namespace fplus;
    auto APlusTwoTimesB = [](int a, int b) { return a + 2 * b; };
    auto APlusTwoTimesBGenericLambda = [](auto a, auto b) { return a + 2 * b; };
    REQUIRE_EQ((apply_to_pair(APlusTwoTimesB, std::make_pair(1, 2))), 5);
    REQUIRE_EQ((apply_to_pair(APlusTwoTimesBGenericLambda, std::make_pair(1, 2))), 5);
    REQUIRE_EQ((apply_to_pair(APlusTwoTimesBFunc, std::make_pair(1, 2))), 5);
}

TEST_CASE("composition_test - state")
{
    using namespace fplus;
    CompositionTestState state(1);
    REQUIRE_EQ(state.Get(), 1);
    auto stateAdd = std::mem_fn(&CompositionTestState::Add);

    stateAdd(state, 2);
    REQUIRE_EQ(state.Get(), 3);

    //auto stateAddBoundFPP = Bind1of2(stateAdd, &state); // crashes VC2015 compiler
    //stateAddBoundFPP(3);

    auto stateAddBoundStl = std::bind(&CompositionTestState::Add, std::placeholders::_1, std::placeholders::_2);
    stateAddBoundStl(state, 3);
    REQUIRE_EQ(state.Get(), 6);
}

TEST_CASE("composition_test - memoize")
{
    using namespace fplus;
    auto f = memoize(square<int>);
    REQUIRE_EQ(f(2), 4);
    REQUIRE_EQ(f(2), 4);
    REQUIRE_EQ(f(3), 9);
    REQUIRE_EQ(f(3), 9);

    std::size_t g_call_cnt = 0;
    auto g = memoize([&g_call_cnt](int x) { ++g_call_cnt; return x * x; });
    REQUIRE_EQ(g(2), 4);
    REQUIRE_EQ(g(2), 4);
    REQUIRE_EQ(g(3), 9);
    REQUIRE_EQ(g(3), 9);
    REQUIRE_EQ(g_call_cnt, 2);

    const auto add = [](int x, int y)
    {
        return x + y;
    };
    auto add_memo = memoize_binary(add);
    REQUIRE_EQ(add_memo(2, 3), 5);
    REQUIRE_EQ(add_memo(2, 3), 5);
    REQUIRE_EQ(add_memo(1, 2), 3);
    REQUIRE_EQ(add_memo(1, 2), 3);

    const auto fibo_memo = memoize_recursive(fibo_cont);

    for (std::uint64_t n = 0; n < 10; ++n)
    {
        REQUIRE_EQ(fibo_memo(n), fibo(n));
    }
}

TEST_CASE("composition_test - constructor_as_function")
{
    using namespace fplus;

    struct foo
    {
        foo(int a, int b) : a_(a), b_(2*b) {}
        int a_;
        int b_;
    };
    const auto create_foo = constructor_as_function<foo, int, int>;
    const auto my_foo = create_foo(1,2);
    REQUIRE_EQ(my_foo.a_, 1);
    REQUIRE_EQ(my_foo.b_, 4);

    struct two_ctors
    {
        two_ctors(int, int) : val_(1) {}
        two_ctors(std::initializer_list<int>) : val_(2) {}
        int val_;
    };
    const two_ctors one_or_two =
        constructor_as_function<two_ctors, int, int>(3, 4);
    REQUIRE_EQ(one_or_two.val_, 1);
}

TEST_CASE("composition_test - constructor_as_function")
{
    const std::vector<int> xs = {1,2,3,4,5};
    const auto ys = fplus::keep_if(
        fplus::compose(
            fplus::square<int>,
            fplus::is_greater_or_equal_than(3)),
        xs);
    REQUIRE_EQ(ys.size(), 4);
}

TEST_CASE("composition_test - get_mem")
{
    struct foo
    {
        int bar_;
    };
    const std::vector<foo> foos = {{1},{2},{3}};
    const auto bars = fplus::transform(fplus_get_mem(bar_), foos);
    REQUIRE_EQ(bars, std::vector<int>({1,2,3}));
    REQUIRE(fplus::all_unique_on(fplus_get_c_mem_t(foo, bar_, int), foos));
}

TEST_CASE("composition_test - get_ptr_mem")
{
    struct foo
    {
        foo(int bar) : bar_(bar) {}
        int bar_;
    };
    const std::vector<std::shared_ptr<foo>> foo_ptrs = {
        std::make_shared<foo>(1),
        std::make_shared<foo>(2),
        std::make_shared<foo>(3)};
    const auto bars = fplus::transform(fplus_get_ptr_mem(bar_), foo_ptrs);
    REQUIRE_EQ(bars, std::vector<int>({1,2,3}));
    REQUIRE(fplus::all_unique_on(
        fplus_get_c_ptr_mem_t(std::shared_ptr<foo>, bar_, int), foo_ptrs));
}

TEST_CASE("composition_test - mem_func")
{
    struct foo
    {
        int bar_;
        int bar() const { return bar_; }
    };
    const std::vector<foo> foos = {{1},{2},{3}};
    const auto bars = fplus::transform(fplus_mem_fn(bar), foos);
    REQUIRE_EQ(bars, std::vector<int>({1,2,3}));
    REQUIRE(fplus::all_unique_on(fplus_c_mem_fn_t(foo, bar, int), foos));
}

TEST_CASE("composition_test - ptr_mem_func")
{
    struct foo
    {
        foo(int bar) : bar_(bar) {}
        int bar_;
        int bar() const { return bar_; }
    };
    const std::vector<std::shared_ptr<foo>> foo_ptrs = {
        std::make_shared<foo>(1),
        std::make_shared<foo>(2),
        std::make_shared<foo>(3)};
    const auto bars = fplus::transform(fplus_ptr_mem_fn(bar), foo_ptrs);
    REQUIRE_EQ(bars, std::vector<int>({1,2,3}));
    REQUIRE(fplus::all_unique_on(
        fplus_c_ptr_mem_fn_t(std::shared_ptr<foo>, bar, int), foo_ptrs));
}