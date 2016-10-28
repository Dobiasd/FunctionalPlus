// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "fplus/container_common.hpp"
#include "fplus/transform.hpp"
#include <array>
#include <chrono>
#include <functional>

namespace fplus
{

// API search type: minimize_downhill : (([Float] -> Float), Float, Float, [Float], Float, Float, Int, Int, ((Int, Float, [Float]) -> IO ())) -> [Float]
// Optimizes the initial position to the nearest local minimum
// in regards to the objective_function
// using numerical gradient descent based on the epsilon neighborhood.
// If one iteration results in no further improvement,
// the step size is reduced by a factor of 0.5.
// The callback is executed with epoch, step and current position
// after every iteration.
// minimize_downhill<1>(\x -> square(x[0] + 2), 0.0001, 0.01, {123})[0] == -2;
template <std::size_t N, typename F, typename pos_t = std::array<double, N>>
pos_t minimize_downhill(
        F objective_function,
        double epsilon,
        double init_step,
        const pos_t& init_pos,
        double sufficing_value = std::numeric_limits<double>::lowest(),
        double min_step = std::numeric_limits<double>::min(),
        std::size_t max_epochs = std::numeric_limits<std::size_t>::max(),
        long int max_milliseconds = std::numeric_limits<double>::max(),
        const std::function<void (std::size_t, double, const pos_t&)>& callback = std::function<void (std::size_t, double, const pos_t&)>())
{

    std::size_t epoch = 0;
    double step = init_step;
    pos_t position = init_pos;
    double value = objective_function(position);

    const auto start_time = std::chrono::high_resolution_clock::now();
    const auto is_done = [&]() -> bool
    {
        if (max_milliseconds != std::numeric_limits<double>::max())
        {
            const auto current_time = std::chrono::high_resolution_clock::now();
            const auto elapsed = current_time - start_time;
            const auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
            if (elapsed_ms >= max_milliseconds)
            {
                return true;
            }
        }
        return
            epoch >= max_epochs ||
            step <= min_step ||
            value <= sufficing_value;
    };

    const auto calc_gradient =
        [&](const pos_t& pos) -> pos_t
    {
        pos_t result;
        for (std::size_t dim = 0; dim < N; ++dim)
        {
            auto test_pos_1 = pos;
            auto test_pos_2 = pos;
            test_pos_1[dim] -= epsilon / 2.0;
            test_pos_2[dim] += epsilon / 2.0;
            const auto val_1 = objective_function(test_pos_1);
            const auto val_2 = objective_function(test_pos_2);
            result[dim] = (val_2 - val_1) / epsilon;
        }
        return result;
    };

    const auto subtract = [](const pos_t& p1, const pos_t& p2) -> pos_t
    {
        pos_t result;
        for (std::size_t dim = 0; dim < N; ++dim)
        {
            result[dim] = p1[dim] - p2[dim];
        }
        return result;
    };

    const auto multiply = [](const pos_t& p, double f) -> pos_t
    {
        pos_t result;
        for (std::size_t dim = 0; dim < N; ++dim)
        {
            result[dim] = p[dim] * f;
        }
        return result;
    };

    const auto dist_to_origin = [](const pos_t& p) -> double
    {
        double acc = 0;
        for (std::size_t dim = 0; dim < N; ++dim)
        {
            acc += square(p[dim]);
        }
        return sqrt(acc);
    };

    const auto normalize = [&](const pos_t& p) -> pos_t
    {
        return multiply(p, 1.0 / dist_to_origin(p));
    };

    while (!is_done())
    {
        pos_t gradient = calc_gradient(position);
        if (dist_to_origin(gradient) <= 0)
        {
            return position;
        }
        position = subtract(position, multiply(normalize(gradient), step));
        const auto new_value = objective_function(position);
        if (new_value >= value)
        {
            step /= 2.0;
        }
        value = new_value;
        ++epoch;
        if (callback)
        {
            callback(epoch, step, position);
        }
    }
    return position;
}

} // namespace fplus
