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

    // step 1 : make benchmarked versions of the functions we intend to benchmark
    auto split_lines_bench =
        make_benchmark_function(my_benchmark_session, "split_lines", split_lines<std::string>);
    auto convert_func = [](std::string v) { 
        return convert_container<character_set, std::string>(v);
    };
    auto convert_container_bench = 
        make_benchmark_function(my_benchmark_session, "convert_charset_string", convert_func);

    // Get lines from input.
    // const auto lines = split_lines(false, input);
    const auto lines = split_lines_bench(false, input); // replace call with benchmarked version

    // Convert string lines into sets of chars.
    //const auto sets = transform( convert_container<character_set, std::string>, lines);
    const auto sets = transform(convert_container_bench, lines); // replace call with benchmarked version

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
    // Function              |Nb calls|Total time|Av. time|Deviation|
    // ----------------------+--------+----------+--------+---------+
    // convert_charset_string|    4000|   4.942ms| 1.236ns|  1.390ns|
    // split_lines           |    1000|   4.528ms| 4.528ns|  1.896ns|
    std::cout << fplus::show(my_benchmark_session.report());
}
