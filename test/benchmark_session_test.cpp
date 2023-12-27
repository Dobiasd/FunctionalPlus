// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <doctest/doctest.h>

#include <vector>
#include <fplus/fplus.hpp>
#include <fplus/benchmark_session.hpp>


// This is an example on how to use benchmark_session in order to bench separate parts of an algorithm

// We need to instantiate a session into which the stats will be collected
fplus::benchmark_session my_benchmark_session;

// antic C style qsort (will be benchmarked against std::sort)
void qsort_vec_int(std::vector<int> & v)
{
    auto cmp = [](const void * a, const void * b) {
        return ( *(static_cast<const int*>(a)) - *(static_cast<const int*>(b)) );
    };
    qsort (v.data(), v.size(), sizeof(int), cmp);
}

// Benchmarked function : several sub parts of this function are benchmarked separately
void benchmark_example()
{
    using Ints = std::vector<int>;

    // Example 1 : benchmark by replacing a function
    //
    // We want to benchmark the following code :
    //    Ints ascending_numbers = fplus::numbers(0, 1000);
    //
    // So, first we make an alternate version of the function "fplus::numbers"
    // Since fplus::numbers is a template function, we need to specify
    // that the actual version we want to benchmark
    // is "fplus::numbers<int, std::vector<int>>"
    //
    // numbers_bench will our alternate version and it
    // has the same signature as fplus::numbers<int, std::vector<int>>,
    // except that it also stores stats into the benchmark session,
    // under the name "numbers"
    //
    // Note that make_benchmark_function *will add side effects* to the function
    // (since it stores data into the benchmark session at each call)
    auto numbers_bench = make_benchmark_function(
        my_benchmark_session,
        "numbers",
        fplus::numbers<int, std::vector<int>>
    );
    // Then, we replace the original code "Ints ascending_numbers = fplus::numbers(0, 1000);"
    // by a code that uses the benchmarked function
    Ints ascending_numbers = numbers_bench(0, 100000);

    // Example 2: benchmark by replacing an expression
    // Below, we will benchmark an expression
    // The original expression we want to benchmark was:
    //     Ints shuffled_numbers = fplus::shuffle(std::mt19937::default_seed, ascending_numbers);
    //
    // In order to do so, we just copy/paste this expression
    // into "bench_expression" like shown below.
    // This expression will then be benchmarked with the name "shuffle"
    //
    // Notes :
    //  - benchmark_expression is a preprocessor macro that uses an immediately invoked lambda (IIL)
    // - the expression can be paster as-is, and it is possible to not remove the ";"
    //   (although it also works if it is not present)
    Ints shuffled_numbers = benchmark_expression(
        my_benchmark_session,
        "shuffle",
        fplus::shuffle(std::mt19937::default_seed, ascending_numbers);
    );

    // Example 3: also benchmark by replacing an expression
    // The original expression was
    //    const auto sorted_numbers = fplus::sort(shuffled_numbers);
    const auto sorted_numbers = benchmark_expression(
        my_benchmark_session,
        "sort_shuffled_sequence",
        fplus::sort(shuffled_numbers);
    );
    // Verify that the sort has worked
    assert(sorted_numbers == ascending_numbers);

    // In this toy example, we will compare the performance
    // of sorting a shuffled sequence versus sorting a reversed sequence

    Ints descending_numbers = fplus::reverse(ascending_numbers); // this call is not benchmarked

    // here we benchmark the call to fplus::sort(descending_numbers)
    const auto sorted_numbers2 = benchmark_expression(
        my_benchmark_session,
        "sort_reverse_sequence",
        fplus::sort(descending_numbers);
    );
    // Verify that the sort has worked
    assert(sorted_numbers2 == ascending_numbers);

    // benchmark qsort
    benchmark_void_expression(my_benchmark_session, "qsort_reverse_sequence",  qsort_vec_int(descending_numbers) );
    REQUIRE(descending_numbers == ascending_numbers);
}


TEST_CASE("benchmark_example")
{
    // Example 4 : benchmark by replacing a function
    // We also want to benchmark the "benchmark_example" in its entirety
    auto benchmark_example_bench = make_benchmark_void_function(
        my_benchmark_session,
        "benchmark_example",
        benchmark_example);

    // For the sake of this test, we will run the benchmarked function several times
    fplus::execute_n_times(10, [&]() { benchmark_example_bench(); });

    // A call to :
    //
    //     std::cout << fplus::show(my_benchmark_session.report());
    //
    // Would output something like
    //
    // Function              |Nb calls|Total time|Av. time   |Deviation |
    // ----------------------+--------+----------+-----------+----------+
    // benchmark_example     |      10| 136.393ms|13639.255ns|2209.289ns|
    // sort_shuffled_sequence|      10|  57.006ms| 5700.557ns| 855.817ns|
    // shuffle               |      10|  49.040ms| 4903.998ns| 785.540ns|
    // qsort_reverse_sequence|      10|  24.777ms| 2477.678ns| 343.918ns|
    // sort_reverse_sequence |      10|   2.308ms|  230.782ns|  87.104ns|
    // numbers               |      10|   2.000ms|  199.965ns| 103.334ns|


    //////////// Unit tests assertions below ////////////////////////////

    // test report_list()
    {
        const auto reports = my_benchmark_session.report_list();
        REQUIRE_EQ(reports.size(), 6);
        const auto & one_report = reports.at("benchmark_example");
        REQUIRE_EQ(one_report.nb_calls, 10);
        REQUIRE(one_report.average_time == doctest::Approx(
            one_report.total_time / static_cast<double>(one_report.nb_calls)));
    }

    // test report()
    {
        const auto & report = my_benchmark_session.report();

        const auto & lines  = fplus::split_lines(false, report);
        REQUIRE_EQ(lines.size(), 8);

        const auto & lines_sizes = fplus::transform([](const std::string & s) {
            return s.size();
        }, lines );
        REQUIRE( fplus::all_the_same(lines_sizes) );

        const auto & check_nb_columns = fplus::transform([](const std::string & s) {
            return (fplus::count('|', s) + fplus::count('+', s) ) == 5;
        }, lines );
        REQUIRE(fplus::all(check_nb_columns));
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
TEST_CASE("benchmark_input_output_args")
{
    fplus::benchmark_session benchmark_session;

    // With non void function
    {
        auto function_with_input_output_params_bench = make_benchmark_function(
            my_benchmark_session,
            "function_with_input_output_params_bench",
            function_with_input_output_params
        );

        int input1 = 1;
        int output2 = 42;

        bool r = function_with_input_output_params_bench(input1, output2);
        REQUIRE_EQ(output2, 2);
        REQUIRE(r);
    }
    // With void function
    {
        auto void_function_with_input_output_params_bench = make_benchmark_void_function(
            my_benchmark_session,
            "void_function_with_input_output_params_bench",
            void_function_with_input_output_params
        );

        int input1 = 1;
        int output2 = 42;

        void_function_with_input_output_params_bench(input1, output2);
        REQUIRE_EQ(output2, 2);
    }
}
