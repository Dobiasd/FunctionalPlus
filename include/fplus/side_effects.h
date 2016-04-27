// Copyright Tobias Hermann 2015.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <chrono>
#include <future>
#include "container_common.h"
#include "generate.h"
#include "function_traits.h"

namespace fplus
{

std::function<void()> sleep_for_n_seconds(std::size_t seconds)
{
    return [seconds]()
    {
        std::this_thread::sleep_for(std::chrono::seconds(seconds));
    };
}

std::function<void()> sleep_for_n_milliseconds(std::size_t milliseconds)
{
    return [milliseconds]()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    };
}

std::function<void()> sleep_for_n_microseconds(std::size_t microseconds)
{
    return [microseconds]()
    {
        std::this_thread::sleep_for(std::chrono::microseconds(microseconds));
    };
}

template <typename Container,
        typename Effect = typename Container::value_type,
        typename Result = typename utils::function_traits<Effect>::result_type>
std::function<std::vector<Result>()> execute_serially(const Container& effs)
{
    return [effs]() -> std::vector<Result>
    {
        std::vector<Result> results;
        for (const Effect& e : effs)
        {
            results.push_back(e());
        }
        return results;
    };
}

template <typename Container,
        typename Effect = typename Container::value_type,
        typename Result = typename utils::function_traits<Effect>::result_type>
std::function<bool()> execute_serially_until_success(const Container& effs)
{
    static_assert(std::is_convertible<Result, bool>::value, "Effects must return a boolish type.");
    return [effs]() -> bool
    {
        for (const Effect& e : effs)
        {
            if (e())
            {
                return true;
            }
        }
        return false;
    };
}

template <typename Result, typename Effect>
std::function<Result()> execute_and_return_fixed_value(
        Result result,
        Effect eff)
{
    return [eff, result]() -> Result
    {
        eff();
        return result;
    };
}

template <typename Effect,
        typename Result = typename utils::function_traits<Effect>::result_type>
std::function<Result()> effect_to_std_function(Effect eff)
{
    return [eff]() -> Result
    {
        return eff();
    };
}

template <typename Effect,
        typename Result = typename utils::function_traits<Effect>::result_type>
std::function<bool()> execute_n_times_until_success_with_pauses_in_milliseconds(
        std::size_t n,
        std::size_t milliseconds,
        const Effect& eff)
{
    auto sleep_and_return_false =
        execute_and_return_fixed_value(
            false,
            sleep_for_n_milliseconds(milliseconds));
    return execute_serially_until_success(
        intersperse(
            sleep_and_return_false,
            replicate(n, effect_to_std_function(eff))));
}

template <typename Container,
        typename Effect = typename Container::value_type,
        typename Result = typename utils::function_traits<Effect>::result_type>
std::function<bool()> execute_serially_until_failure(const Container& effs)
{
    static_assert(std::is_convertible<Result, bool>::value, "Effects must return a boolish type.");
    return [effs]() -> bool
    {
        for (const Effect& e : effs)
        {
            if (!e())
            {
                return false;
            }
        }
        return true;
    };
}

template <typename Container,
        typename Effect = typename Container::value_type,
        typename Result = typename utils::function_traits<Effect>::result_type>
std::function<std::vector<Result>()> execute_parallelly(const Container& effs)
{
    return [effs]() -> std::vector<Result>
    {
        std::vector<std::future<Result>> handles =
            transform([](Effect e) -> std::future<Result>
            {
                return std::async(std::launch::async, e);
            }, effs);

        std::vector<Result> results;
        results.reserve(size_of_cont(handles));
        for (std::future<Result>& handle : handles)
        {
            results.push_back(handle.get());
        }
        return results;
    };
}

template <typename Effect>
std::function<void()> execute_fire_and_forget(Effect eff)
{
    return [eff]()
    {
        std::thread t(eff);
        t.detach();
    };
}

} // namespace fplus
