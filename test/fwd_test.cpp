// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <doctest/doctest.h>
#include <fplus/fplus.hpp>

namespace {
    typedef std::vector<int> IntVector;
    bool is_odd_int(int x)
    {
        return (x % 2 != 0);
    }

    bool is_even_int(int x)
    {
        return (x % 2 == 0);
    }

    int times_3(int x)
    {
        return 3 * x;
    }

    int as_string_length(int i)
    {
        return static_cast<int>(std::to_string(i).size());
    }

    const auto times_3_lambda = [](int x){return times_3(x);};
    const auto is_odd_int_lambda = [](int x){return is_odd_int(x);};
    const auto as_string_length_lambda = [](int x){return as_string_length(x);};

    int (*times_3_fn_ptr)(int) = &times_3;

    struct times_3_struct {
        int operator() (const int x)
        { return times_3(x); }
        static int sttcMemF(int x)
        { return times_3(x); }
    };

    std::function<int(int)> times_3_std_function = times_3_lambda;
}

TEST_CASE("fwd_test - apply")
{
    using namespace fplus;

    const auto result_old_style =
        sum(
        transform(as_string_length,
        drop_if(is_odd_int,
        transform(times_3,
        numbers(0, 10)))));

    const auto result_new_style = fwd::apply(
        numbers(0, 10)
        , fwd::transform(times_3)
        , fwd::drop_if(is_odd_int)
        , fwd::transform(as_string_length)
        , fwd::sum());

    REQUIRE_EQ(result_old_style, result_new_style);
}

TEST_CASE("fwd_test - compose")
{
    using namespace fplus;

    const auto function_chain_old_style = compose(
        bind_1st_of_2(transform<decltype(times_3), const std::vector<int>&, std::vector<int>>, times_3),
        bind_1st_of_2(drop_if<decltype(is_odd_int), const std::vector<int>&>, is_odd_int),
        bind_1st_of_2(transform<decltype(as_string_length_lambda), const std::vector<int>&>, as_string_length_lambda),
        sum<std::vector<int>>);

    const auto function_chain_new_style = fwd::compose(
        fwd::transform(times_3),
        fwd::drop_if(is_odd_int_lambda),
        fwd::transform(as_string_length),
        fwd::sum());

    const auto xs = numbers(0, 10);
    REQUIRE_EQ(function_chain_old_style(xs), function_chain_new_style(xs));
}

TEST_CASE("fwd_test - and_then_maybe")
{
    using namespace fplus;
    const auto sqrtToMaybeInt = [](int x) -> fplus::maybe<int>
    {
        return x < 0 ? fplus::nothing<int>() :
                fplus::just(fplus::round(sqrt(static_cast<float>(x))));
    };
    REQUIRE_EQ(
        fwd::apply(just(4)
            , fwd::and_then_maybe(sqrtToMaybeInt))
        , just(2));
}

TEST_CASE("fwd_test - fold_left")
{
    using namespace fplus;

    const auto fold_result_old_style =
        fold_left(std::plus<int>(), 0, numbers(0, 10));

    const auto fold_result_new_style = fwd::apply(
        numbers(0, 10)
        , fwd::fold_left(std::plus<int>(), 0));

    REQUIRE_EQ(fold_result_old_style, fold_result_new_style);
}

TEST_CASE("fwd_test - transform_nested")
{
    using namespace fplus;

    typedef std::vector<int> ints;
    const std::vector<ints> nested_ints = {{1,2,3},{4,5,6}};

    const auto nested_transformed_old_style = transform(
        bind_1st_of_2(transform<decltype(times_3), const std::vector<int>&, std::vector<int>>, times_3),
        nested_ints);

    const auto nested_transformed_new_style = fwd::apply(
        nested_ints
        , fwd::transform(fwd::transform(times_3_lambda)));

    REQUIRE_EQ(nested_transformed_old_style, nested_transformed_new_style);
}

TEST_CASE("fwd_test - different_function_types_apply")
{
    using namespace fplus;

    const std::vector<int> xs = {1,2,3};
    const auto result = transform(times_3, xs);

    REQUIRE_EQ(fwd::apply(xs, fwd::transform(times_3)), result);
    REQUIRE_EQ(fwd::apply(xs, fwd::transform(times_3_lambda)), result);
    REQUIRE_EQ(fwd::apply(xs, fwd::transform(times_3_std_function)), result);
    REQUIRE_EQ(fwd::apply(xs, fwd::transform(times_3_fn_ptr)), result);
    REQUIRE_EQ(fwd::apply(xs, fwd::transform(&times_3_struct::sttcMemF)), result);
    REQUIRE_EQ(fwd::apply(xs, fwd::transform(times_3_struct())), result);
}

TEST_CASE("fwd_test - different_function_types_compose")
{
    using namespace fplus;

    const std::vector<int> xs = {1,2,3};
    const auto result = transform(times_3, transform(times_3, xs));

    REQUIRE_EQ(fwd::transform(fwd::compose(times_3, times_3))(xs), result);
    REQUIRE_EQ(fwd::transform(fwd::compose(times_3_lambda, times_3_lambda))(xs), result);
    REQUIRE_EQ(fwd::transform(fwd::compose(times_3_std_function, times_3_std_function))(xs), result);
    REQUIRE_EQ(fwd::transform(fwd::compose(&times_3_struct::sttcMemF, &times_3_struct::sttcMemF))(xs), result);
    REQUIRE_EQ(fwd::transform(fwd::compose(times_3_fn_ptr, times_3_fn_ptr))(xs), result);
}

std::list<int> collatz_seq(int x)
{
    std::list<int> result;
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

TEST_CASE("fwd_test - collatz")
{
    using namespace fplus;

    auto collatz_dict = fwd::apply(
        fplus::numbers<int>(0, 20)
        , fwd::create_map_with(fwd::compose(
            collatz_seq,
            fwd::show_cont_with(" => ")))
        );
}

TEST_CASE("fwd_test - fwd_flip")
{
    using namespace fplus;
    std::vector<std::vector<std::size_t>> idxs = {{0,1,2}, {2,0}};
    const std::vector<int> xs = {0,10,20};
    const std::vector<int> ys = fwd::transform_and_concat(fwd::flip::elems_at_idxs(xs))(idxs);
    const std::vector<int> result = {0,10,20,20,0};
    REQUIRE_EQ(ys, result);
}

TEST_CASE("fwd_test - keep_if")
{
    const std::vector<int> v = { 1, 2, 3, 2, 4, 5 };
    auto result = fplus::fwd::keep_if(is_even_int)(v);
    REQUIRE_EQ(result, std::vector<int>({2, 2, 4}));
}

TEST_CASE("fwd_test - keep_if_r_value")
{
    auto result = fplus::fwd::keep_if(is_even_int)(std::vector<int>({1,2,3,2,4,5}));
    REQUIRE_EQ(result, std::vector<int>({2, 2, 4}));
}

TEST_CASE("fwd_test - zip_with")
{
    using namespace fplus;
    const auto multiply_int = [](int x, int y) -> int { return x * y; };
    const auto multiply_generic = [](auto x, auto y){ return x * y; };

    IntVector xs = {1,2,3,4,2};
    IntVector ys = {2,2,3,1};
    IntVector xs_mult_ys = {2,4,9,4};

    REQUIRE_EQ(fwd::zip_with(multiply_int, ys)(xs), xs_mult_ys);
    REQUIRE_EQ(fwd::zip_with(multiply_generic, ys)(xs), xs_mult_ys);
}

TEST_CASE("fwd_test - append")
{
    using namespace fplus;

    IntVector xs = {1,2,3,4,2};
    IntVector ys = {2,2,3,1};
    IntVector xs_append_ys = {1,2,3,4,2,2,2,3,1};

    REQUIRE_EQ(fwd::append(xs)(ys), xs_append_ys);
}
