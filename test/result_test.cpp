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

    fplus::result<float, std::string> sqrtToResult(float x)
    {
        return x < 0.0f ? fplus::error<float>(std::string("no sqrt of negative numbers")) :
                fplus::ok<float, std::string>(static_cast<float>(sqrt(static_cast<float>(x))));
    }

    fplus::result<int, std::string> sqrtToResultInt(int x)
    {
        return x < 0 ? fplus::error<int>(std::string("no sqrt of negative numbers")) :
                fplus::ok<int, std::string>(fplus::round<int>(sqrt(static_cast<float>(x))));
    }

    float IntToFloat(const int& x)
    {
        return static_cast<float>(x);
    }

    typedef std::vector<fplus::result<int, std::string>> IntResults;
    typedef std::vector<int> Ints;
    typedef std::vector<std::string> Strings;
}

class resultTestState {
public:
    explicit resultTestState(int x) : x_(x) {}
    void Add(int y) { x_ += y; }
    int Get() const { return x_; }
private:
    int x_;
};

TEST_CASE("result_test, ok_with_default")
{
    using namespace fplus;
    auto x = ok<int, std::string>(2);
    auto y = error<int, std::string>("an error");
    auto Or42 = bind_1st_of_2(ok_with_default<int, std::string>, 42);
    auto SquareAndSquare = compose(square<int>, square<int>);
    REQUIRE_EQ(Or42(x), 2);
    REQUIRE_EQ(Or42(y), 42);
}

TEST_CASE("result_test, and_then_result")
{
    using namespace fplus;
    auto x = ok<int, std::string>(2);
    auto y = error<int, std::string>("an error");
    auto squareResult = lift_result<std::string>(square<int>);
    auto sqrtAndSqrt = and_then_result(sqrtToResult, sqrtToResult);
    REQUIRE_EQ(squareResult(x), (ok<int, std::string>(4)));
    REQUIRE_EQ(squareResult(y), (error<int>(std::string("an error"))));

    auto sqrtIntAndSqrtIntAndSqrtInt = and_then_result(sqrtToResultInt, sqrtToResultInt, sqrtToResultInt);
    REQUIRE_EQ(sqrtIntAndSqrtIntAndSqrtInt(256), (ok<int, std::string>(2)));
    auto sqrtIntAndSqrtIntAndSqrtIntAndSqrtInt = and_then_result(sqrtToResultInt, sqrtToResultInt, sqrtToResultInt, sqrtToResultInt);
    REQUIRE_EQ(sqrtIntAndSqrtIntAndSqrtIntAndSqrtInt(65536), (ok<int, std::string>(2)));

    auto LiftedIntToFloat = lift_result<std::string>(IntToFloat);
    auto OkInt = ok<int, std::string>;
    auto IntToResultFloat = compose(OkInt, LiftedIntToFloat);
    auto IntToFloatAndSqrtAndSqrt = and_then_result(IntToResultFloat, sqrtAndSqrt);
    REQUIRE(is_in_range(1.41f, 1.42f)(unsafe_get_ok<float>
            (IntToFloatAndSqrtAndSqrt(4))));
}

TEST_CASE("result_test, lift")
{
    using namespace fplus;
    auto x = ok<int, std::string>(2);
    auto y = error<int, std::string>("an error");
    auto SquareAndSquare = compose(square<int>, square<int>);
    REQUIRE_EQ((lift_result<std::string>(SquareAndSquare))(x), (ok<int, std::string>(16)));
}

TEST_CASE("result_test, lift_both")
{
    using namespace fplus;
    const auto x = ok<int, std::string>(2);
    const auto y = error<int, std::string>("an error");
    REQUIRE_EQ(lift_result_both(square<int>, to_upper_case<std::string>)(x), (ok<int, std::string>(4)));
    REQUIRE_EQ(lift_result_both(square<int>, to_upper_case<std::string>)(y), (error<int, std::string>("AN ERROR")));
}

TEST_CASE("result_test, unify_result")
{
    using namespace fplus;
    const auto x = ok<int, std::string>(2);
    const auto y = error<int, std::string>("an error");
    REQUIRE_EQ(unify_result(show<int>, to_upper_case<std::string>, x), "2");
    REQUIRE_EQ(unify_result(show<int>, to_upper_case<std::string>, y), "AN ERROR");
}

TEST_CASE("result_test, equality")
{
    using namespace fplus;
    IntResults results = {ok<int, std::string>(1), error<int>(std::string("no sqrt of negative numbers")), ok<int, std::string>(2)};

    REQUIRE(oks(results) == Ints({ 1,2 }));
    REQUIRE(errors(results) == Strings({std::string("no sqrt of negative numbers")}));

    REQUIRE((ok<int, std::string>(1)) == (ok<int, std::string>(1)));
    REQUIRE((ok<int, std::string>(1)) != (ok<int, std::string>(2)));
    REQUIRE((ok<int, std::string>(1)) != (error<int, std::string>(std::string("fail"))));
    REQUIRE(error<int>(std::string("fail")) == (error<int>(std::string("fail"))));
    REQUIRE(error<int>(std::string("fail 1")) != (error<int>(std::string("fail 2"))));
}

TEST_CASE("result_test, transform_and_keep_oks")
{
    using namespace fplus;
    Ints wholeNumbers = { -3, 4, 16, -1 };
    REQUIRE_EQ(transform_and_keep_oks(sqrtToResultInt, wholeNumbers)
           , Ints({2,4}));

    REQUIRE_EQ(transform_and_concat(
            bind_1st_of_2(replicate<int>, std::size_t(3)), Ints{ 1,2 })
           , Ints({ 1,1,1,2,2,2 }));
}

TEST_CASE("result_test, show_result")
{
    using namespace fplus;
    REQUIRE_EQ(show_result(ok<int, std::string>(42)), std::string("Ok 42"));
    REQUIRE_EQ(show_result(error<int, std::string>("fail")), std::string("Error fail"));
    auto x = ok<int, std::string>(2);
    REQUIRE_EQ((to_maybe<int, std::string>(x)), just(2));
    REQUIRE_EQ((from_maybe<int, std::string>(just(2), std::string("no error"))), x);
}

TEST_CASE("result_test, exceptions")
{
    using namespace fplus;
    std::string thrown_str;
    try
    {
        throw_on_error(std::invalid_argument("exception string"), error<int, std::string>("failed"));
    }
    catch (const std::exception& e)
    {
        thrown_str = e.what();
    }
    REQUIRE_EQ(thrown_str, std::string("exception string"));
    thrown_str.clear();

    try
    {
        throw_type_on_error<std::invalid_argument>(error<int, std::string>("failed"));
    }
    catch (const std::exception& e)
    {
        thrown_str = e.what();
    }
    REQUIRE_EQ(thrown_str, std::string("failed"));
    thrown_str.clear();
}

TEST_CASE("result_test, copy")
{
    using namespace fplus;
    result<int, std::string> result_4 = ok<int, std::string>(4);
    result<int, std::string> result_4_copy(result_4);
    result<int, std::string> result_4_copy_2 = error<int, std::string>("dummy");
    result_4_copy_2 = result_4_copy;
    REQUIRE_EQ(result_4_copy_2, (ok<int, std::string>(4)));

    result<int, std::string> result_int_string_error = error<int, std::string>("error");
    result<int, std::string> result_int_string_error_copy(result_int_string_error);
    result<int, std::string> result_int_string_error_copy_2 = ok<int, std::string>(9999);
    result_int_string_error_copy_2 = result_int_string_error_copy;
    REQUIRE_EQ(result_int_string_error_copy_2, (error<int, std::string>("error")));
}
