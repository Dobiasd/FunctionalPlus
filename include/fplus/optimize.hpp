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

// API search type: minimize_downhill : (([Float] -> Float), Float, [Float], Float, Float, Float, Float, Int, Int, ((Int, Float, [Float], [Float]) -> IO ())) -> [Float]
// Optimizes the initial position to the nearest local minimum
// in regards to the objective_function
// using numerical gradient descent with momentum
// based on the epsilon neighborhood.
// momentum_conservation should be in [0, 1). A low value means much decay.
// The step factor determines how far in the direction of the gradient each step
// will advance.
// If one iteration results in no further improvement,
// the step factor is reduced by a factor of 0.5.
// The callback is executed with epoch, step factor, momentum and current position
// after every iteration.
// An initial step factor that differs between the dimensions
// can be emulated by scaling ones objective function accordingly.
// minimize_downhill<1>(\x -> square(x[0] + 2), 0.0001, 0.01, {123})[0] == -2;
template <std::size_t N, typename F, typename pos_t = std::array<double, N>>
pos_t minimize_downhill(
        F objective_function,
        double epsilon,
        const pos_t& init_pos,
        double init_step_factor = 1.0,
        double momentum_conservation = 0.5,
        double sufficing_value = std::numeric_limits<double>::lowest(),
        double min_step_factor = std::numeric_limits<double>::min(),
        std::size_t max_epochs = std::numeric_limits<std::size_t>::max(),
        long int max_milliseconds = std::numeric_limits<long int>::max(),
        const std::function<void (std::size_t, double, const pos_t&, const pos_t&)>& callback = std::function<void (std::size_t, double, const pos_t&, const pos_t&)>())
{
    std::size_t epoch = 0;
    double step_factor = init_step_factor;
    pos_t position = init_pos;
    double value = objective_function(position);

    const auto start_time = std::chrono::high_resolution_clock::now();
    const auto is_done = [&]() -> bool
    {
        if (max_milliseconds != std::numeric_limits<long int>::max())
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
            step_factor <= min_step_factor ||
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

    const auto add = [](const pos_t& p1, const pos_t& p2) -> pos_t
    {
        pos_t result;
        for (std::size_t dim = 0; dim < N; ++dim)
        {
            result[dim] = p1[dim] + p2[dim];
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

    const auto null_vector = []() -> pos_t
    {
        pos_t result;
        for (std::size_t dim = 0; dim < N; ++dim)
        {
            result[dim] = 0;
        }
        return result;
    };

    pos_t momentum = null_vector();
    while (!is_done())
    {
        auto new_momentum = multiply(momentum, momentum_conservation);
        pos_t gradient = calc_gradient(add(position, new_momentum));
        new_momentum =
            multiply(
                add(
                    new_momentum,
                    multiply(gradient, -1.0)),
                step_factor);
        if (dist_to_origin(new_momentum) <= std::numeric_limits<double>::min())
        {
            break;
        }
        const auto new_position = add(position, new_momentum);
        const auto new_value = objective_function(new_position);
        if (new_value >= value)
        {
            step_factor /= 2.0;
        }
        else
        {
            value = new_value;
            position = new_position;
            momentum = new_momentum;
        }
        ++epoch;
        if (callback)
        {
            callback(epoch, step_factor, momentum, position);
        }
    }
    return position;
}

} // namespace fplus
