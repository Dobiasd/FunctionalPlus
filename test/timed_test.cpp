// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <doctest/doctest.h>

#include <vector>
#include <cmath>
#include <chrono>
#include <fplus/fplus.hpp>

// Utility functions
namespace
{
    void require_are_execution_times_close(double t1, double t2)
    {
        // up to 30 ms difference, since the cpu scheduler might switch to another process during a sleep
        double max_acceptable_delta__task_scheduler = 0.03;
        #if defined(__APPLE__)
            max_acceptable_delta__task_scheduler = 0.2;
        #endif
        REQUIRE(fabs(t1 - t2) < max_acceptable_delta__task_scheduler);
    }

    template<typename T>
    void require_are_timed_equal(const fplus::timed<T> & a, const fplus::timed<T> & b)
    {
        REQUIRE(a.get() == b.get());
        require_are_execution_times_close(a.time_in_s(), b.time_in_s());
    }

    void sleep_seconds(double sleep_seconds)
    {
        long long sleep_ns = static_cast<long long>(sleep_seconds * 1E9);
        std::this_thread::sleep_for(std::chrono::nanoseconds(sleep_ns));
    }

    int add(int a, int b)  // a simple function that will be decorated
    {
        sleep_seconds(0.002);
        return a + b;
    }

    void void_function()
    {
        sleep_seconds(0.002);
    }
}


// Test timed class
TEST_CASE("timed - ctor")
{
    using namespace fplus;
    {
        // default constructor
        fplus::timed<double> v;
        REQUIRE(v.get() == doctest::Approx(0.));
        REQUIRE(v.time_in_s() == doctest::Approx(0.));
    }
    {
        // T +  time constructor
        fplus::timed<double> v(1., 3.);
        REQUIRE(v.get() == doctest::Approx(1.));
        REQUIRE(v.time_in_s() == doctest::Approx(3.));
    }
    {
        // timed<T> constructor
        fplus::timed<double> v1(2.);
        auto v2(v1);
        REQUIRE(v1.time_in_s() == doctest::Approx(v2.time_in_s()));
        REQUIRE(v1.get() == doctest::Approx(v2.get()));
    }
}

TEST_CASE("timed - operator=")
{
    {
        auto v1 = fplus::timed<double>(1.);
        auto v2 = fplus::timed<double>(2.);
        REQUIRE(v1.get() != doctest::Approx(v2.get()));
        v2 = v1;
        REQUIRE(v1.get() == doctest::Approx(v2.get()));
    }
}


TEST_CASE("timed - show_timed")
{
    {
        fplus::timed<int> v(42, 1);
        auto s = show_timed(v);
        REQUIRE_EQ(s, "42 (1000ms)");
    }
}

TEST_CASE("timed - duration_in_s")
{
    {
        fplus::timed<int> v(42, 1.2345);
        auto d = v.duration_in_s();
        double seconds = d.count();
        REQUIRE( seconds == doctest::Approx(1.2345) );
    }
}


// Test make_timed_function
TEST_CASE("make_timed_function")
{
    using namespace fplus;

    {
        // Test decorated function
        auto add_timed = make_timed_function(add);
        auto result = add_timed(39, 3);
        auto expected = timed<int>(42, 0.02);
        require_are_timed_equal(result, expected);
    }

    {
        // Test void function
        auto void_function_timed = fplus::make_timed_void_function(void_function);
        auto execution_time = void_function_timed();
        require_are_execution_times_close(execution_time, 0.02);
    }

    {
        // Test decorated lambda
        auto sub = [](int a, int b) -> int {
            sleep_seconds(0.03);
            return a - b;
        };
        auto sub_timed = make_timed_function(sub);
        auto result = sub_timed(45, 3);
        auto expected = timed<int>(42, 0.03);
        require_are_timed_equal(result, expected);
    }

    {
        // Test decorated void lambda
        auto fn = []() {
            sleep_seconds(0.03);
        };
        auto fn_timed = make_timed_void_function(fn);
        auto execution_time = fn_timed();
        require_are_execution_times_close(execution_time, 0.03);
    }

    {
        // Test std::function
        auto sub_lambda = [](int a, int b) -> int {
            sleep_seconds(0.03);
            return a - b;
        };
        std::function<int(int,int)> sub = sub_lambda;
        auto sub_timed = make_timed_function(sub);
        auto result = sub_timed(45, 3);
        auto expected = timed<int>(42, 0.03);
        require_are_timed_equal(result, expected);
    }

    {
        using Ints = std::vector<int>;

        Ints ascending_numbers = fplus::numbers(0, 1000);
        Ints shuffled_numbers = fplus::shuffle(std::mt19937::default_seed, ascending_numbers);

        auto sort_func = [](const Ints& values) { return fplus::sort(values); };
        auto sort_bench = fplus::make_timed_function(sort_func);

        auto sorted_numbers = sort_bench(shuffled_numbers);
        REQUIRE_EQ(sorted_numbers.get(), ascending_numbers);
        // sorting 1000 numbers should require less than 0.1 seconds (in practice it requires about 0.2ms)
        REQUIRE_LT(sorted_numbers.time_in_s(), 0.1);
    }
}


//
// The test below asserts that variadic arguments containing modifiable references are correctly forwarded
//
bool function_with_input_output_params(int input1, int& output2)
{
    output2 = input1 + 1;
    return true;
}
void void_function_with_input_output_params(int input1, int& output2)
{
    output2 = input1 + 1;
}
TEST_CASE("timed_with_input_output_args")
{
    // With non void function
    {
        auto timed_function_with_input_output_params = fplus::make_timed_function(function_with_input_output_params);

        int input1 = 1;
        int output2 = 42;

        fplus::timed<bool> r = timed_function_with_input_output_params(input1, output2);
        REQUIRE_EQ(output2, 2);
        REQUIRE(r.get());
    }
    // With void function
    {
        auto timed_void_function_with_input_output_params = fplus::make_timed_void_function(void_function_with_input_output_params);

        int input1 = 1;
        int output2 = 42;

        timed_void_function_with_input_output_params(input1, output2);
        REQUIRE_EQ(output2, 2);
    }
}
