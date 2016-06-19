
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "fplus/fplus.h"
#include <vector>

using namespace testing;

class result_test : public testing::Test
{
protected:

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }

    static int square(int x)
    {
        return x*x;
    };

    static fplus::result<float, std::string> sqrtToResult(float x)
    {
        return x < 0.0f ? fplus::error<float>(std::string("no sqrt of negative numbers")) :
                fplus::ok<float, std::string>(static_cast<float>(sqrt(static_cast<float>(x))));
    };

    static fplus::result<int, std::string> sqrtToResultInt(int x)
    {
        return x < 0 ? fplus::error<int>(std::string("no sqrt of negative numbers")) :
                fplus::ok<int, std::string>(fplus::round<int>(sqrt(static_cast<float>(x))));
    };

    static float IntToFloat(const int& x)
    {
        return static_cast<float>(x);
    };

    typedef std::vector<fplus::result<int, std::string>> IntResults;
    typedef std::vector<int> Ints;
    typedef std::vector<std::string> Strings;
};

class resultTestState {
public:
    explicit resultTestState(int x) : x_(x) {}
    void Add(int y) { x_ += y; }
    int Get() const { return x_; }
private:
    int x_;
};

TEST_F(result_test, ok_with_default)
{
    using namespace fplus;
    auto x = ok<int, std::string>(2);
    auto y = error<int, std::string>("an error");
    auto Or42 = bind_1st_of_2(ok_with_default<int, std::string>, 42);
    auto SquareAndSquare = compose(square, square);
    EXPECT_THAT(Or42(x), Eq(2));
    EXPECT_THAT(Or42(y), Eq(42));
}

TEST_F(result_test, and_then_result)
{
    using namespace fplus;
    auto x = ok<int, std::string>(2);
    auto y = error<int, std::string>("an error");
    auto squareResult = lift_result<std::string>(square);
    auto sqrtAndSqrt = and_then_result(sqrtToResult, sqrtToResult);
    EXPECT_THAT(squareResult(x), Eq((ok<int, std::string>(4))));
    EXPECT_THAT(squareResult(y), Eq((error<int>(std::string("an error")))));

    auto sqrtIntAndSqrtIntAndSqrtInt = and_then_result(sqrtToResultInt, sqrtToResultInt, sqrtToResultInt);
    EXPECT_THAT(sqrtIntAndSqrtIntAndSqrtInt(256), Eq((ok<int, std::string>(2))));
    auto sqrtIntAndSqrtIntAndSqrtIntAndSqrtInt = and_then_result(sqrtToResultInt, sqrtToResultInt, sqrtToResultInt, sqrtToResultInt);
    EXPECT_THAT(sqrtIntAndSqrtIntAndSqrtIntAndSqrtInt(65536), Eq((ok<int, std::string>(2))));

    auto LiftedIntToFloat = lift_result<std::string>(IntToFloat);
    auto OkInt = ok<int, std::string>;
    auto IntToResultFloat = compose(OkInt, LiftedIntToFloat);
    auto IntToFloatAndSqrtAndSqrt = and_then_result(IntToResultFloat, sqrtAndSqrt);
    EXPECT_TRUE(is_in_range(1.41f, 1.42f)(unsafe_get_ok<float>
            (IntToFloatAndSqrtAndSqrt(4))));
}

TEST_F(result_test, lift)
{
    using namespace fplus;
    auto x = ok<int, std::string>(2);
    auto y = error<int, std::string>("an error");
    auto SquareAndSquare = compose(square, square);
    EXPECT_THAT((lift_result<std::string>(SquareAndSquare))(x), Eq((ok<int, std::string>(16))));
}

TEST_F(result_test, equality)
{
    using namespace fplus;
    IntResults results = {ok<int, std::string>(1), error<int>(std::string("no sqrt of negative numbers")), ok<int, std::string>(2)};

    EXPECT_TRUE(oks(results) == Ints({ 1,2 }));
    EXPECT_TRUE(errors(results) == Strings({std::string("no sqrt of negative numbers")}));

    EXPECT_TRUE((ok<int, std::string>(1)) == (ok<int, std::string>(1)));
    EXPECT_TRUE((ok<int, std::string>(1)) != (ok<int, std::string>(2)));
    EXPECT_TRUE((ok<int, std::string>(1)) != (error<int, std::string>(std::string("fail"))));
    EXPECT_TRUE(error<int>(std::string("fail")) == (error<int>(std::string("fail"))));
    EXPECT_TRUE(error<int>(std::string("fail 1")) != (error<int>(std::string("fail 2"))));

}

TEST_F(result_test, transform_and_keep_oks)
{
    using namespace fplus;
    Ints wholeNumbers = { -3, 4, 16, -1 };
    EXPECT_THAT(transform_and_keep_oks(sqrtToResultInt, wholeNumbers)
           , Eq(Ints({2,4})));

    EXPECT_THAT(transform_and_concat(bind_1st_of_2(replicate<int>, 3), Ints{ 1,2 })
           , Eq(Ints({ 1,1,1,2,2,2 })));
}

TEST_F(result_test, show_result)
{
    using namespace fplus;
    EXPECT_THAT(show_result(ok<int, std::string>(42)), Eq(std::string("Ok 42")));
    EXPECT_THAT(show_result(error<int, std::string>("fail")), Eq(std::string("Error fail")));
    auto x = ok<int, std::string>(2);
    EXPECT_THAT((to_maybe<int, std::string>(x)), Eq(just(2)));
    EXPECT_THAT((from_maybe<int, std::string>(just(2), std::string("no error"))), Eq(x));
}

TEST_F(result_test, exceptions)
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
    EXPECT_THAT(thrown_str, Eq("exception string"));
    thrown_str.clear();

    try
    {
        throw_type_on_error<std::invalid_argument>(error<int, std::string>("failed"));
    }
    catch (const std::exception& e)
    {
        thrown_str = e.what();
    }
    EXPECT_THAT(thrown_str, Eq("failed"));
    thrown_str.clear();
}

TEST_F(result_test, copy)
{
    using namespace fplus;
    result<int, std::string> result_4 = ok<int, std::string>(4);
    result<int, std::string> result_4_copy(result_4);
    result<int, std::string> result_4_copy_2 = error<int, std::string>("dummy");
    result_4_copy_2 = result_4_copy;
    EXPECT_THAT(result_4_copy_2, Eq((ok<int, std::string>(4))));

    result<int, std::string> result_int_string_error = error<int, std::string>("error");
    result<int, std::string> result_int_string_error_copy(result_int_string_error);
    result<int, std::string> result_int_string_error_copy_2 = ok<int, std::string>(9999);
    result_int_string_error_copy_2 = result_int_string_error_copy;
    EXPECT_THAT(result_int_string_error_copy_2, Eq((error<int, std::string>("error"))));
}
