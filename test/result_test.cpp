// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <doctest/doctest.h>
#include <fplus/fplus.hpp>
#include <vector>

namespace {

    auto sqrtToResult = [](auto x)
    {
        return x < 0.0f ? fplus::error<float>(std::string("no sqrt of negative numbers")) :
                fplus::ok<float, std::string>(static_cast<float>(sqrt(static_cast<float>(x))));
    };

    auto sqrtToResultInt = [](auto x)
    {
        return x < 0 ? fplus::error<int>(std::string("no sqrt of negative numbers")) :
                fplus::ok<int, std::string>(fplus::round(sqrt(static_cast<float>(x))));
    };

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

TEST_CASE("result_test - ok_with_default")
{
    using namespace fplus;
    auto x = ok<int, std::string>(2);
    auto y = error<int, std::string>("an error");
    auto Or42 = bind_1st_of_2(ok_with_default<int, std::string>, 42);
    REQUIRE_EQ(Or42(x), 2);
    REQUIRE_EQ(Or42(y), 42);
}

TEST_CASE("maybe_test - join_result")
{
    using namespace fplus;
    using Ok = int;
    using Err = std::string;
    using Res = result<Ok, Err>;
    REQUIRE_EQ(join_result(ok<Res, Err>(ok<Ok, Err>(2))), (ok<Ok, Err>(2)));
    REQUIRE_EQ(join_result(ok<Res, Err>(error<Ok, Err>("e"))), (error<Ok, Err>("e")));
    REQUIRE_EQ(join_result(error<Res, Err>("e")), (error<Ok, Err>("e")));
}

TEST_CASE("result_test - and_then_result")
{
    using namespace fplus;
    auto ok_4 = ok<int, std::string>(4);
    auto ok_2 = ok<int, std::string>(2);
    auto an_error = error<int, std::string>("an error");
    REQUIRE_EQ(and_then_result(sqrtToResultInt, ok_4), ok_2);
    REQUIRE_EQ(and_then_result(sqrtToResultInt, an_error), an_error);

    const auto string_to_result_int_string = [](const auto& str) {
        if (str == "42")
            return ok<int, std::string>(42);
        else
            return error<int, std::string>("not 42");
    };
    REQUIRE_EQ(and_then_result(string_to_result_int_string, (ok<std::string, std::string>("3"))), (error<int, std::string>("not 42")));
    REQUIRE_EQ(and_then_result(string_to_result_int_string, (ok<std::string, std::string>("42"))), (ok<int, std::string>(42)));
    REQUIRE_EQ(and_then_result(string_to_result_int_string, (error<std::string, std::string>("error"))), (error<int, std::string>("error")));
}

TEST_CASE("result_test - compose_result")
{
    using namespace fplus;
    auto x = ok<int, std::string>(2);
    auto y = error<int, std::string>("an error");
    auto sqrtAndSqrt = compose_result(sqrtToResult, sqrtToResult);
    REQUIRE_EQ(lift_result(square<int>, x), (ok<int, std::string>(4)));
    REQUIRE_EQ(lift_result(square<int>, y), (error<int>(std::string("an error"))));

    auto sqrtIntAndSqrtIntAndSqrtInt = compose_result(sqrtToResultInt, sqrtToResultInt, sqrtToResultInt);
    REQUIRE_EQ(sqrtIntAndSqrtIntAndSqrtInt(256), (ok<int, std::string>(2)));
    auto sqrtIntAndSqrtIntAndSqrtIntAndSqrtInt = compose_result(sqrtToResultInt, sqrtToResultInt, sqrtToResultInt, sqrtToResultInt);
    REQUIRE_EQ(sqrtIntAndSqrtIntAndSqrtIntAndSqrtInt(65536), (ok<int, std::string>(2)));

    const auto LiftedIntToFloat =
    [](const result<int, std::string>& r) -> result<float, std::string>
    {
        return lift_result(IntToFloat, r);
    };
    auto OkInt = ok<int, std::string>;
    auto IntToResultFloat = compose(OkInt, LiftedIntToFloat);
    auto IntToFloatAndSqrtAndSqrt = compose_result(IntToResultFloat, sqrtAndSqrt);
    REQUIRE(is_in_interval(1.41f, 1.42f, unsafe_get_ok<float>
            (IntToFloatAndSqrtAndSqrt(4))));

    // first callable can take a variadic number of arguments
    auto sumToResult = [](auto a, auto b) {
        return ok<decltype(a + b), std::string>(a + b);
    };
    auto squareSumResult = compose_result(sumToResult, [](auto sum) {
        return ok<decltype(sum * sum), std::string>(sum * sum);
    });

    REQUIRE_EQ(squareSumResult(5, 5), (ok<int, std::string>(100)));
}

TEST_CASE("result_test - lift")
{
    using namespace fplus;
    auto x = ok<int, std::string>(2);
    auto y = error<int, std::string>("an error");
    auto SquareAndSquare = compose(square<int>, square<int>);
    REQUIRE_EQ((lift_result(SquareAndSquare, x)), (ok<int, std::string>(16)));
    REQUIRE_EQ((lift_result([](auto n) { return square(n) * square(n); }, x)),
               (ok<int, std::string>(16)));
}

TEST_CASE("result_test - lift_both")
{
    using namespace fplus;
    const auto x = ok<int, std::string>(2);
    const auto y = error<int, std::string>("an error");
    REQUIRE_EQ(lift_result_both(square<int>, to_upper_case<std::string>, x), (ok<int, std::string>(4)));
    REQUIRE_EQ(lift_result_both(square<int>, to_upper_case<std::string>, y), (error<int, std::string>("AN ERROR")));
    REQUIRE_EQ(lift_result_both([](auto z) { return z * z; },
                                [](auto z) { return to_upper_case(z); },
                                y),
               (error<int, std::string>("AN ERROR")));
}

TEST_CASE("result_test - unify_result")
{
    using namespace fplus;
    const auto x = ok<int, std::string>(2);
    const auto y = error<int, std::string>("an error");
    const auto unify = [](const result<int, std::string>& r) -> std::string
    {
        return unify_result(show<int>, to_upper_case<std::string>, r);
    };
    REQUIRE_EQ(unify(x), "2");
    REQUIRE_EQ(unify(y), "AN ERROR");

    const auto unifyGeneric = [](auto r) {
        return unify_result(show<typename decltype(r)::ok_t>,
                            to_upper_case<typename decltype(r)::error_t>,
                            r);
    };
    REQUIRE_EQ(unifyGeneric(x), "2");
    REQUIRE_EQ(unifyGeneric(y), "AN ERROR");
}

TEST_CASE("result_test - equality")
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

TEST_CASE("result_test - transform_and_keep_oks")
{
    using namespace fplus;
    Ints wholeNumbers = { -3, 4, 16, -1 };
    REQUIRE_EQ(transform_and_keep_oks(sqrtToResultInt, wholeNumbers)
           , Ints({2,4}));

    REQUIRE_EQ(transform_and_concat(
            bind_1st_of_2(replicate<int>, std::size_t(3)), Ints{ 1,2 })
           , Ints({ 1,1,1,2,2,2 }));
}

TEST_CASE("result_test - show_result")
{
    using namespace fplus;
    REQUIRE_EQ(show_result(ok<int, std::string>(42)), std::string("Ok 42"));
    REQUIRE_EQ(show_result(error<int, std::string>("fail")), std::string("Error fail"));
    auto x = ok<int, std::string>(2);
    REQUIRE_EQ((to_maybe<int, std::string>(x)), just(2));
    REQUIRE_EQ((from_maybe<std::string, int>(std::string("no error"), just(2))), x);
}

TEST_CASE("result_test - exceptions")
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

TEST_CASE("result_test - copy")
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
