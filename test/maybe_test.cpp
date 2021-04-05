// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <doctest/doctest.h>
#include <fplus/fplus.hpp>
#include <vector>

namespace {

    auto sqrtToMaybe = [](auto x)
    {
        return x < 0.0f ? fplus::nothing<float>() :
                fplus::just(static_cast<float>(sqrt(static_cast<float>(x))));
    };

    auto sqrtToMaybeInt = [](auto x)
    {
        return x < 0 ? fplus::nothing<int>() :
                fplus::just(fplus::round(sqrt(static_cast<float>(x))));
    };

    float IntToFloat(const int& x)
    {
        return static_cast<float>(x);
    }

    typedef std::vector<fplus::maybe<int>> IntMaybes;
    typedef std::vector<int> Ints;
}

struct foo {
    explicit foo(int) { msgs_.push_back("ctor"); }
    foo(const foo&) { msgs_.push_back("copyctor"); }
    foo(foo&&) noexcept { msgs_.push_back("movector"); }
    foo& operator = (const foo&) { msgs_.push_back("assignment"); return *this; }
    foo& operator = (foo&&) { msgs_.push_back("moveassignment"); return *this; }
    ~foo() { msgs_.push_back("dtor"); }
    static std::vector<std::string> msgs_;
};
std::vector<std::string> foo::msgs_;

TEST_CASE("maybe_test - construction and assignment")
{
    using namespace fplus;
    foo::msgs_.clear();
    REQUIRE_EQ(maybe<int>(4), just<int>(4));

    typedef std::vector<std::string> Strings;
    REQUIRE_EQ(foo::msgs_, Strings({}));
    maybe<foo> no_foo;
    REQUIRE_EQ(foo::msgs_, Strings({}));
    {
        foo foo_source = foo(1);
        maybe<foo> a_foo(foo_source);
        REQUIRE_EQ(foo::msgs_, Strings({"ctor", "copyctor"}));
    }
    REQUIRE_EQ(foo::msgs_, Strings({"ctor", "copyctor", "dtor", "dtor"}));
    foo::msgs_.clear();
}

TEST_CASE("maybe_test - move semantics")
{
    using namespace fplus;
    foo::msgs_.clear();
    typedef std::vector<std::string> Strings;
    maybe<foo> foo_a(foo(1));
    maybe<foo> foo_b(foo(2));
    maybe<foo> foo_z(std::move(foo_a));
    foo_z = std::move(foo_b);
    REQUIRE_EQ(foo::msgs_, Strings({
        "ctor", "movector", "dtor",
        "ctor", "movector", "dtor",
        "movector", "dtor", "movector"}));
    foo::msgs_.clear();
}

TEST_CASE("maybe_test - unsafe_get_just")
{
    using namespace fplus;
    fplus::maybe<int> m(4);
    REQUIRE_EQ(m.unsafe_get_just(), 4);
    m.unsafe_get_just() += 1;
    REQUIRE_EQ(m.unsafe_get_just(), 5);
}

TEST_CASE("maybe_test - as_just_if")
{
    using namespace fplus;
    REQUIRE_EQ(as_just_if(is_even<int>, 4), just<int>(4));
    REQUIRE_EQ(as_just_if(is_even<int>, 5), nothing<int>());
}

TEST_CASE("maybe_test - sequence")
{
    using namespace fplus;
    REQUIRE_EQ(maybe_to_seq(just(4)), std::vector<int>(1, 4));
    REQUIRE_EQ(maybe_to_seq(nothing<int>()), std::vector<int>());

    REQUIRE_EQ(singleton_seq_as_maybe(std::vector<int>()), nothing<int>());
    REQUIRE_EQ(singleton_seq_as_maybe(std::vector<int>(1, 4)), just(4));
    REQUIRE_EQ(singleton_seq_as_maybe(std::vector<int>(2, 4)), nothing<int>());
}

TEST_CASE("maybe_test - just_with_default")
{
    using namespace fplus;
    auto x = just<int>(2);
    maybe<int> y = nothing<int>();
    auto Or42 = bind_1st_of_2(just_with_default<int>, 42);
    REQUIRE_EQ(Or42(x), 2);
    REQUIRE_EQ(Or42(y), 42);
}

TEST_CASE("maybe_test - lift")
{
    using namespace fplus;
    auto x = just<int>(2);
    maybe<int> y = nothing<int>();
    auto squareGeneric = [](auto n) { return n * n; };

    REQUIRE_EQ(lift_maybe(square<int>, x), just(4));
    REQUIRE_EQ(lift_maybe(square<int>, y), nothing<int>());
    REQUIRE_EQ(lift_maybe(squareGeneric, x), just(4));
    REQUIRE_EQ(lift_maybe(squareGeneric, y), nothing<int>());
    auto SquareAndSquare = compose(square<int>, square<int>);
    REQUIRE_EQ(lift_maybe(SquareAndSquare, x), just(16));

    REQUIRE_EQ(lift_maybe_def(3, square<int>, x), 4);
    REQUIRE_EQ(lift_maybe_def(3, square<int>, y), 3);
    REQUIRE_EQ(lift_maybe_def(3, squareGeneric, x), 4);
    REQUIRE_EQ(lift_maybe_def(3, squareGeneric, y), 3);

    REQUIRE_EQ(lift_maybe_2(std::plus<int>(), x, x), just(4));
    REQUIRE_EQ(lift_maybe_2(std::plus<int>(), x, y), y);
    REQUIRE_EQ(lift_maybe_2(std::plus<>(), y, x), y);
    REQUIRE_EQ(lift_maybe_2(std::plus<>(), y, y), y);

    REQUIRE_EQ(lift_maybe_2_def(3, std::plus<int>(), x, x), 4);
    REQUIRE_EQ(lift_maybe_2_def(3, std::plus<int>(), x, y), 3);
    REQUIRE_EQ(lift_maybe_2_def(3, std::plus<>(), y, x), 3);
    REQUIRE_EQ(lift_maybe_2_def(3, std::plus<>(), y, y), 3);
}

TEST_CASE("maybe_test - join_maybe")
{
    using namespace fplus;
    REQUIRE_EQ(join_maybe(just(just(2))), just(2));
    REQUIRE_EQ(join_maybe(just(nothing<int>())), nothing<int>());
    REQUIRE_EQ(join_maybe(nothing<maybe<int>>()), nothing<int>());
}

TEST_CASE("maybe_test - and_then_maybe")
{
    using namespace fplus;
    REQUIRE_EQ(and_then_maybe(sqrtToMaybeInt, just(4)), just(2));
    REQUIRE_EQ(and_then_maybe(sqrtToMaybeInt, nothing<int>()), nothing<int>());
    const auto string_to_maybe_int = [](const auto& str)
    {
        if (str == "42") return just<int>(42);
        else return nothing<int>();
    };
    REQUIRE_EQ(and_then_maybe(string_to_maybe_int, just<std::string>("3")), nothing<int>());
    REQUIRE_EQ(and_then_maybe(string_to_maybe_int, just<std::string>("42")), just<int>(42));
    REQUIRE_EQ(and_then_maybe(string_to_maybe_int, nothing<std::string>()), nothing<int>());
}

TEST_CASE("maybe_test - compose")
{
    using namespace fplus;
    auto sqrtAndSqrt = compose_maybe(sqrtToMaybe, sqrtToMaybe);
    auto sqrtIntAndSqrtIntAndSqrtInt = compose_maybe(sqrtToMaybeInt, sqrtToMaybeInt, sqrtToMaybeInt);
    REQUIRE_EQ(sqrtIntAndSqrtIntAndSqrtInt(256), just(2));
    auto sqrtIntAndSqrtIntAndSqrtIntAndSqrtInt = compose_maybe(sqrtToMaybeInt, sqrtToMaybeInt, sqrtToMaybeInt, sqrtToMaybeInt);
    REQUIRE_EQ(sqrtIntAndSqrtIntAndSqrtIntAndSqrtInt(65536), just(2));

    auto LiftedIntToFloat = [](const maybe<int>& m) -> maybe<float>
    {
        return lift_maybe(IntToFloat, m);
    };
    auto JustInt = just<int>;
    auto IntToMaybeFloat = compose(JustInt, LiftedIntToFloat);
    auto IntToFloatAndSqrtAndSqrt = compose_maybe(IntToMaybeFloat, sqrtAndSqrt);

    auto squareMaybe = [](auto n) { return just<decltype(n * n)>(n * n); };
    auto plusMaybe = [](auto m, auto n) {
        return just<decltype(m + n)>(m + n);
    };
    REQUIRE_EQ(compose_maybe(plusMaybe, squareMaybe)(2, 3), just(25));
    REQUIRE(is_in_interval(1.41f, 1.42f, unsafe_get_just<float>
            (IntToFloatAndSqrtAndSqrt(4))));
}

TEST_CASE("maybe_test - equality")
{
    using namespace fplus;
    IntMaybes maybes = {just(1), nothing<int>(), just(2)};
    REQUIRE(justs(maybes) == Ints({ 1,2 }));
    REQUIRE(just(1) == just(1));
    REQUIRE(just(1) != just(2));
    REQUIRE(just(1) != nothing<int>());
    REQUIRE(nothing<int>() == nothing<int>());
}

TEST_CASE("maybe_test - transform_and_keep_justs")
{
    using namespace fplus;
    Ints wholeNumbers = { -3, 4, 16, -1 };
    REQUIRE_EQ(transform_and_keep_justs(sqrtToMaybeInt, wholeNumbers)
           , Ints({2,4}));
    REQUIRE_EQ(transform_and_concat(
            bind_1st_of_2(replicate<int>, std::size_t(3)), Ints{ 1,2 })
           , Ints({ 1,1,1,2,2,2 }));
}

TEST_CASE("maybe_test - show_maybe")
{
    using namespace fplus;
    REQUIRE_EQ(show_maybe(just<int>(42)), std::string("Just 42"));
    REQUIRE_EQ(show_maybe(nothing<int>()), std::string("Nothing"));
}

TEST_CASE("maybe_test - exceptions")
{
    using namespace fplus;
    std::string thrown_str;
    try
    {
        throw_on_nothing(std::invalid_argument("raised"), nothing<int>());
    }
    catch (const std::exception& e)
    {
        thrown_str = e.what();
    }
    REQUIRE_EQ(thrown_str, std::string("raised"));
}

TEST_CASE("maybe_test - copy")
{
    using namespace fplus;
    maybe<int> maybe_4(4);
    maybe<int> maybe_4_copy(maybe_4);
    maybe<int> maybe_4_copy_2;
    maybe_4_copy_2 = maybe_4_copy;
    REQUIRE_EQ(maybe_4_copy_2, just<int>(4));
}

TEST_CASE("maybe_test - flatten")
{
    using namespace fplus;
    maybe<int> maybe_int_nothing;
    maybe<int> maybe_int_nothing_copy(maybe_int_nothing);
    maybe<int> maybe_int_nothing_copy_2;
    maybe_int_nothing_copy_2 = maybe_int_nothing_copy;
    REQUIRE_EQ(maybe_int_nothing_copy_2, nothing<int>());
    REQUIRE_EQ(flatten_maybe(maybe<maybe<int>>(maybe<int>(1))), maybe<int>(1));
    REQUIRE_EQ(flatten_maybe(maybe<maybe<int>>(maybe<int>())), nothing<int>());
    REQUIRE_EQ(flatten_maybe(maybe<maybe<int>>()), nothing<int>());
}
