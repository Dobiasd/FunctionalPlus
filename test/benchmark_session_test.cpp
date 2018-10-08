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

fplus::benchmark_session my_benchmark_session;


// In this example, we are benchmarking gemstones_bench, 
// and only benchmark 2 sub-functions : split_lines and convert_container
std::string gemstones_bench(const std::string& input)
{
    using namespace fplus;

    typedef std::set<std::string::value_type> character_set;

    // Benchmark by replacing a function:
    // Below, we will benchmark the call to split_lines:
    // // const auto lines = split_lines(false, input); // this was the original code 
    // 1. we define an alternate version split_lines_bench
    auto split_lines_bench = make_benchmark_function(my_benchmark_session, "split_lines", split_lines<std::string>);
    // 2. we replace the call by using the alternate version
    const auto lines = split_lines_bench(false, input); // replace call with benchmarked version

    // Benchmark by replacing an expression
    // Below, we will benchmark an expression
    // The original expression was:
    // const auto sets = transform( convert_container<character_set, std::string>, lines);
    // We just copy paste this expression into the bench_expression macro, like shown below
    // Note: since this is a macro we must replace the ',' by COMMA
    const auto sets = benchmark_expression(
        my_benchmark_session, 
        "transform", 
        transform(convert_container<character_set, std::string> COMMA lines)
    );

    // Build the intersection of all character sets.
    const auto gem_elements = fold_left_1(
        set_intersection<character_set>, sets);

    // Convert gem_elements.size() into a std::string.
    return show(size_of_cont(gem_elements));
}


TEST_CASE("my_benchmark")
{
    std::string input("Lorem ipsum\ndolor sit amet,\nconsectetur,\nadipisci velit");
    fplus::run_n_times(1000, [&]() { gemstones_bench(input); });

    // Will output something like
    // Function   |Nb calls|Total time|Av. time|Deviation|
    // -----------+--------+----------+--------+---------+
    // transform  |    1000|   4.828ms| 4.828ns|  2.473ns|
    // split_lines|    1000|   3.795ms| 3.795ns|  2.729ns|
    std::cout << fplus::show(my_benchmark_session.report());
}
