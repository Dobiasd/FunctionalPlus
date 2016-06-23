
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "fplus/fplus.h"
#include <vector>

using namespace testing;

namespace {

    int square(int x){
        return x*x;
    }

    fplus::maybe<float> sqrtToMaybe(float x)
    {
        return x < 0.0f ? fplus::nothing<float>() :
                fplus::just(static_cast<float>(sqrt(static_cast<float>(x))));
    }

    fplus::maybe<int> sqrtToMaybeInt(int x)
    {
        return x < 0 ? fplus::nothing<int>() :
                fplus::just(fplus::round<int>(sqrt(static_cast<float>(x))));
    }

    float IntToFloat(const int& x)
    {
        return static_cast<float>(x);
    }

    typedef std::vector<fplus::maybe<int>> IntMaybes;
    typedef std::vector<int> Ints;
}

class maybeTestState {
public:
    explicit maybeTestState(int x) : x_(x) {}
    void Add(int y) { x_ += y; }
    int Get() const { return x_; }
private:
    int x_;
};

TEST(maybe_test, ctor)
{
    using namespace fplus;
    EXPECT_THAT(maybe<int>(4), Eq(just<int>(4)));
}

TEST(maybe_test, just_with_default)
{
    using namespace fplus;
    auto x = just<int>(2);
    maybe<int> y = nothing<int>();
    auto Or42 = bind_1st_of_2(just_with_default<int>, 42);
    EXPECT_THAT(Or42(x), Eq(2));
    EXPECT_THAT(Or42(y), Eq(42));
}

TEST(maybe_test, lift)
{
    using namespace fplus;
    auto x = just<int>(2);
    maybe<int> y = nothing<int>();
    auto squareMaybe = lift_maybe(square);
    EXPECT_THAT(squareMaybe(x), Eq(just(4)));
    EXPECT_THAT(squareMaybe(y), Eq(nothing<int>()));
    auto SquareAndSquare = compose(square, square);
    EXPECT_THAT((lift_maybe(SquareAndSquare))(x), Eq(just(16)));
}

TEST(maybe_test, and_then)
{
    using namespace fplus;
    auto sqrtAndSqrt = and_then_maybe(sqrtToMaybe, sqrtToMaybe);
    auto sqrtIntAndSqrtIntAndSqrtInt = and_then_maybe(sqrtToMaybeInt, sqrtToMaybeInt, sqrtToMaybeInt);
    EXPECT_THAT(sqrtIntAndSqrtIntAndSqrtInt(256), Eq(just(2)));
    auto sqrtIntAndSqrtIntAndSqrtIntAndSqrtInt = and_then_maybe(sqrtToMaybeInt, sqrtToMaybeInt, sqrtToMaybeInt, sqrtToMaybeInt);
    EXPECT_THAT(sqrtIntAndSqrtIntAndSqrtIntAndSqrtInt(65536), Eq(just(2)));

    auto LiftedIntToFloat = lift_maybe(IntToFloat);
    auto JustInt = just<int>;
    auto IntToMaybeFloat = compose(JustInt, LiftedIntToFloat);
    auto IntToFloatAndSqrtAndSqrt = and_then_maybe(IntToMaybeFloat, sqrtAndSqrt);
    EXPECT_TRUE(is_in_range(1.41f, 1.42f)(unsafe_get_just<float>
            (IntToFloatAndSqrtAndSqrt(4))));
}

TEST(maybe_test, equality)
{
    using namespace fplus;
    IntMaybes maybes = {just(1), nothing<int>(), just(2)};
    EXPECT_TRUE(justs(maybes) == Ints({ 1,2 }));
    EXPECT_TRUE(just(1) == just(1));
    EXPECT_TRUE(just(1) != just(2));
    EXPECT_TRUE(just(1) != nothing<int>());
    EXPECT_TRUE(nothing<int>() == nothing<int>());
}

TEST(maybe_test, transform_and_keep_justs)
{
    using namespace fplus;
    Ints wholeNumbers = { -3, 4, 16, -1 };
    EXPECT_THAT(transform_and_keep_justs(sqrtToMaybeInt, wholeNumbers)
           , Eq(Ints({2,4})));
    EXPECT_THAT(transform_and_concat(
            bind_1st_of_2(replicate<int>, std::size_t(3)), Ints{ 1,2 })
           , Eq(Ints({ 1,1,1,2,2,2 })));
}

TEST(maybe_test, show_maybe)
{
    using namespace fplus;
    EXPECT_THAT(show_maybe(just<int>(42)), Eq(std::string("Just 42")));
    EXPECT_THAT(show_maybe(nothing<int>()), Eq(std::string("Nothing")));
}

TEST(maybe_test, exceptions)
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
    EXPECT_THAT(thrown_str, Eq("raised"));
}

TEST(maybe_test, copy)
{
    using namespace fplus;
    maybe<int> maybe_4(4);
    maybe<int> maybe_4_copy(maybe_4);
    maybe<int> maybe_4_copy_2;
    maybe_4_copy_2 = maybe_4_copy;
    EXPECT_THAT(maybe_4_copy_2, Eq(just<int>(4)));
}

TEST(maybe_test, flatten)
{
    using namespace fplus;
    maybe<int> maybe_int_nothing;
    maybe<int> maybe_int_nothing_copy(maybe_int_nothing);
    maybe<int> maybe_int_nothing_copy_2;
    maybe_int_nothing_copy_2 = maybe_int_nothing_copy;
    EXPECT_THAT(maybe_int_nothing_copy_2, Eq(nothing<int>()));
    EXPECT_THAT(flatten_maybe(maybe<maybe<int>>(maybe<int>(1))), Eq(maybe<int>(1)));
    EXPECT_THAT(flatten_maybe(maybe<maybe<int>>(maybe<int>())), Eq(nothing<int>()));
    EXPECT_THAT(flatten_maybe(maybe<maybe<int>>()), Eq(nothing<int>()));
}
