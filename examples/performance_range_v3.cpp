// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <fplus/fplus.hpp>
#include <range/v3/all.hpp>

const auto times_3 = [](int i){return 3 * i;};
const auto is_odd_int = [](int i){return i % 2 == 0;};
const auto as_string_length = [](int i){return std::to_string(i).size();};

int main()
{
    const auto times_3 = [](int i){return 3 * i;};
    const auto is_odd_int = [](int i){return i % 2 == 0;};
    const auto as_string_length = [](int i){return std::to_string(i).size();};

    typedef std::chrono::time_point<std::chrono::system_clock> Time;

    for (int i = 0; i < 3; ++i)
    {
        // FunctionalPlus
        Time startTimeFPlus = std::chrono::system_clock::now();
        using namespace fplus;
        const auto result_fplus = fwd::apply(
            numbers(0, 15000000)
            , fwd::transform(times_3)
            , fwd::drop_if(is_odd_int)
            , fwd::transform(as_string_length)
            , fwd::sum());
        Time endTimeFPlus = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_s_fplus =
            endTimeFPlus - startTimeFPlus;
        std::cout << "(check: " << result_fplus <<
            "), elapsed time fplus:    " << elapsed_s_fplus.count() << "s\n";

        // range-v3
        Time startTimeRangev3 = std::chrono::system_clock::now();
        using namespace ranges;
        const auto result_range_v3 =
            accumulate(
                view::ints(0)
                | view::take(15000000)
                | view::transform(times_3)
                | view::remove_if(is_odd_int)
                | view::transform(as_string_length)
                , 0);
        Time endTimeRangev3 = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_s_rangev3 =
            endTimeRangev3 - startTimeRangev3;
        std::cout << "(check: " << result_range_v3 <<
            "), elapsed time range-v3: " << elapsed_s_rangev3.count() << "s\n";
    }
}
