// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <vector>
#include <fplus/fplus.hpp>
#include <fplus/benchmark_session.hpp>


// This is an example on how to use benchmark_session in order to bench separate parts of an algorithm

// We need to instantiate a session into which the stats will be collected
fplus::benchmark_session my_benchmark_session;


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
    fplus::run_n_times(10, [&]() { benchmark_example_bench(); });

    // A call to : 
    //
    // std::cout << fplus::show(my_benchmark_session.report()); 
    //
    // Would output something like
    // Function              |Nb calls|Total time|Av. time   |Deviation |
    // ----------------------+--------+----------+-----------+----------+
    // benchmark_example     |      10| 136.494ms|13649.412ns|1191.401ns|
    // sort_shuffled_sequence|      10|  66.698ms| 6669.850ns| 339.859ns|
    // shuffle               |      10|  62.881ms| 6288.136ns| 754.399ns|
    // sort_reverse_sequence |      10|   3.079ms|  307.911ns|  71.570ns|
    // numbers               |      10|   2.364ms|  236.398ns| 134.988ns|

    // Interestingly enough, we see that sort has a very good performance
    // on reversed sequences. It proves that sort is not just based on qsort
    // (qsort performs badly with reversed sequences)

    // test report_list()
    {
        const auto reports = my_benchmark_session.report_list();
        REQUIRE_EQ(reports.size(), 5);
        const auto & one_report = reports.at("benchmark_example");
        REQUIRE_EQ(one_report.nb_calls, 10);
        REQUIRE(one_report.average_time == doctest::Approx(
            one_report.total_time / static_cast<double>(one_report.nb_calls)));
    }

    // test report()
    {
        const auto & report = my_benchmark_session.report();

        const auto & lines  = fplus::split_lines(false, report);
        REQUIRE_EQ(lines.size(), 7);

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

