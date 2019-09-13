// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <fplus/container_common.hpp>
#include <fplus/transform.hpp>
#include <array>
#include <chrono>
#include <functional>

namespace fplus
{

// Optimizes the initial position to the nearest local minimum
// in regards to the objective_function
// using numerical gradient descent based on the epsilon neighborhood.
// momentum_conservation should be in [0, 1). A low value means much decay.
// If no fixed step size is provided, each step advances by the length
// of the gradient.
// In both cases the step is scaled with a step factor, starting at 1.0.
// If one iteration results in no further improvement,
// the step factor is reduced by a factor of 0.5.
// The callback is executed with
// iteration, step factor, momentum and current position
// after every iteration.
// A initial step factor other than 1.0 in all dimensions
// can be emulated by scaling ones objective function accordingly.
// Optimization stops if one of the provided criteria is met.
// minimize_downhill<1>(\x -> square(x[0] + 2), 0.0001, 0.01, {123})[0] == -2;
template <std::size_t N, typename F, typename pos_t = std::array<double, N>>
pos_t minimize_downhill(
        F objective_function,
        double epsilon,
        const pos_t& init_pos,
        maybe<double> fixed_step_size = nothing<double>(),
        double momentum_conservation = 0.5,
        double sufficing_value = std::numeric_limits<double>::lowest(),
        double min_step_factor = std::numeric_limits<double>::min(),
        std::size_t max_iterations = std::numeric_limits<std::size_t>::max(),
        long int max_milliseconds = std::numeric_limits<long int>::max(),
        const std::function<
                void (std::size_t, double, const pos_t&, const pos_t&)>&
            callback =
            std::function<
                void (std::size_t, double, const pos_t&, const pos_t&)>())
{
    std::size_t iteration = 0;
    double step_factor = 1.0;
    pos_t position = init_pos;
    double value = internal::invoke(objective_function, position);

    const auto start_time = std::chrono::steady_clock::now();
    const auto is_done = [&]() -> bool
    {
        if (max_milliseconds != std::numeric_limits<long int>::max())
        {
            const auto current_time = std::chrono::steady_clock::now();
            const auto elapsed = current_time - start_time;
            const auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
            if (elapsed_ms >= max_milliseconds)
            {
                return true;
            }
        }
        return
            iteration >= max_iterations ||
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
            const auto val_1 = internal::invoke(objective_function, test_pos_1);
            const auto val_2 = internal::invoke(objective_function, test_pos_2);
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

    const auto normalize = [&](const pos_t& p) -> pos_t
    {
        return multiply(p, 1.0 / dist_to_origin(p));
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
        const auto inverse_gradient = multiply(gradient, -1.0);

        auto new_momentum_add =
            is_nothing(fixed_step_size) ?
                inverse_gradient :
                multiply(
                    normalize(inverse_gradient),
                    fixed_step_size.unsafe_get_just());

        new_momentum =
            multiply(
                add(new_momentum, new_momentum_add),
                step_factor);
        if (dist_to_origin(momentum) <= std::numeric_limits<double>::min() &&
            dist_to_origin(new_momentum) <= std::numeric_limits<double>::min())
        {
            break;
        }
        const auto new_position = add(position, new_momentum);
        const auto new_value = internal::invoke(objective_function, new_position);
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
        ++iteration;
        if (callback)
        {
            callback(iteration, step_factor, momentum, position);
        }
    }
    return position;
}

} // namespace fplus
