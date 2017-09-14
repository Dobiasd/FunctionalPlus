// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <fplus/fplus.hpp>

#include <atomic>
#include <cassert>
#include <chrono>
#include <ctime>
#include <deque>
#include <functional>
#include <iostream>
#include <list>
#include <stdexcept>
#include <string>
#include <vector>

bool is_odd_int(int x) { return x % 2 == 1; }

void Test_example_KeepIf()
{
    typedef std::vector<int> Ints;
    Ints values = { 24, 11, 65, 44, 80, 18, 73, 90, 69, 18 };

    { // Version 1: hand written range based for loop
        Ints odds;
        for (int x : values)
            if (is_odd_int(x))
                odds.push_back(x);
    }

    { // Version 2: STL
        Ints odds;
        std::copy_if(std::begin(values), std::end(values),
                std::back_inserter(odds), is_odd_int);
    }

    { // Version : FunctionalPlus
        auto odds = fplus::keep_if(is_odd_int, values);
    }
}

void run_n_times(std::function<std::vector<int>(std::vector<int>)> f,
    std::size_t n, const std::string& name, const std::vector<int>& inList)
{
    typedef std::chrono::time_point<std::chrono::system_clock> Time;
    Time startTime = std::chrono::system_clock::now();
    std::size_t lengthSum = 0;
    for (std::size_t i = 0; i < n; ++i)
    {
        lengthSum += f(inList).size();
    }
    Time endTime = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = endTime - startTime;
    std::cout << name << "(check: " << lengthSum << "), elapsed time: " << elapsed_seconds.count() << "s\n";
}

void Test_example_KeepIf_performance()
{
    using namespace fplus;

    typedef std::vector<int> Ints;
    auto run_loop = [&](const Ints values)
    {
        Ints odds;
        for (int x : values)
            if (is_odd_int(x))
                odds.push_back(x);
        return odds;
    };
    auto run_stl = [&](const Ints values)
    {
        Ints odds;
        std::copy_if(std::begin(values), std::end(values),
                std::back_inserter(odds), is_odd_int);
        return odds;
    };
    auto run_FunctionalPlus = [&](const Ints values)
        { return keep_if(is_odd_int, values); };

    // make debug runs faster
#if defined NDEBUG || defined _DEBUG
    std::size_t numRuns = 10;
#else
    std::size_t numRuns = 20000;
#endif

    Ints values = generate<Ints>(rand, 5000);
    run_n_times(run_loop, numRuns, "Hand-written for loop", values);
    run_n_times(run_stl, numRuns, "std::copy_if", values);
    run_n_times(run_FunctionalPlus, numRuns, "FunctionalPlus::keep_if", values);
}

int main()
{
    Test_example_KeepIf_performance();
}
