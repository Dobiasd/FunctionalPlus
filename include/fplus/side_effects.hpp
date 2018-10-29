// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <fplus/container_common.hpp>
#include <fplus/function_traits.hpp>
#include <fplus/generate.hpp>
#include <fplus/string_tools.hpp>
#include <fplus/internal/invoke.hpp>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <fstream>
#include <functional>
#include <future>
#include <iostream>
#include <iterator>
#include <streambuf>
#include <string>
#include <thread>
#include <vector>

namespace fplus
{

// Executes a function f in a fixed interval,
// i.e. an average timespan between two consecutive calls of f,
// given in microseconds.
// f is a unary function, taking the time delta (in microseconds)
// between the last and the current call as its argument.
// In case of a delay outdated calls are be executed immediately.
// So the average executation time of f should be way shorter
// than the requested interval.
// Call ticker::start() to run.
// The ticker stops when ticker::stop() is called
// or the instance runs out of scope.
//
// Example usage:
//
// void say_hi(std::int64_t)
// {
//     std::cout << "hi " << std::endl;
// }
// int main()
// {
//     ticker hi_ticker(say_hi, 2 * 1000 * 1000);
//     hi_ticker.start();
//     std::this_thread::sleep_for(std::chrono::milliseconds(4500));
// }
class ticker
{
public:
    typedef std::function<void(std::int64_t)> function;
    ticker(const function& f, std::int64_t interval_us) :
        f_(f),
        interval_us_(interval_us),
        control_mutex_(),
        is_running_(false),
        thread_(),
        stop_mutex_()
    {
    }
    bool is_running()
    {
        std::lock_guard<std::mutex> lock(control_mutex_);
        return is_running_;
    }
    bool start()
    {
        std::lock_guard<std::mutex> lock(control_mutex_);
        if (is_running_)
            return false;
        stop_mutex_.lock();
        thread_ = std::thread([this]() { thread_function(); });
        is_running_ = true;
        return true;
    }
    bool stop()
    {
        std::lock_guard<std::mutex> lock(control_mutex_);
        if (!is_running_)
            return false;
        stop_mutex_.unlock();
        if (thread_.joinable())
        {
            thread_.join();
            thread_ = std::thread();
        }
        is_running_ = false;
        return true;
    }
    ~ticker()
    {
        stop();
    }
private:
    void thread_function()
    {
        auto last_wake_up_time = std::chrono::steady_clock::now();
        auto last_time = last_wake_up_time;
        bool quit = false;
        while (!quit)
        {
            const auto wake_up_time =
                last_wake_up_time + std::chrono::microseconds{ interval_us_ };
            const auto sleep_time =
                wake_up_time - std::chrono::steady_clock::now();
            if (stop_mutex_.try_lock_for(sleep_time))
            {
                stop_mutex_.unlock();
                quit = true;
            }
            const auto current_time = std::chrono::steady_clock::now();
            const auto elapsed = current_time - last_time;
            last_wake_up_time = wake_up_time;
            last_time = current_time;
            const auto elapsed_us =
                std::chrono::duration_cast<std::chrono::microseconds>(
                    elapsed).count();
            try
            {
                f_(elapsed_us);
            }
            catch (...)
            {
            }
        }
    }
    const function f_;
    const std::int64_t interval_us_;
    std::mutex control_mutex_;
    bool is_running_;
    std::thread thread_;
    std::timed_mutex stop_mutex_;
};


// API search type: sleep_for_n_seconds : Int -> Io ()
// Returns a function that suspends
// the calling thread for n seconds when executed.
inline
std::function<void()> sleep_for_n_seconds(std::size_t seconds)
{
    return [seconds]()
    {
        std::this_thread::sleep_for(std::chrono::seconds(seconds));
    };
}

// API search type: sleep_for_n_milliseconds : Int -> Io ()
// Returns a function that suspends
// the calling thread for n milliseconds when executed.
inline
std::function<void()> sleep_for_n_milliseconds(std::size_t milliseconds)
{
    return [milliseconds]()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    };
}

// API search type: sleep_for_n_microseconds : Int -> Io ()
// Returns a function that suspends
// the calling thread for n microseconds when executed.
inline
std::function<void()> sleep_for_n_microseconds(std::size_t microseconds)
{
    return [microseconds]()
    {
        std::this_thread::sleep_for(std::chrono::microseconds(microseconds));
    };
}

// API search type: execute_serially : [Io ()] -> Io ()
// Returns a function that executes
// the given side effects one after another when called.
template <typename Container>
auto execute_serially(const Container& effs)
{
    using Effect = typename Container::value_type;
    using Result = internal::invoke_result_t<Effect>;

    return [effs]
    {
        std::vector<std::decay_t<Result>> results;
        for (const Effect& e : effs)
        {
            results.push_back(internal::invoke(e));
        }
        return results;
    };
}

// API search type: execute_serially_until_success : [Io Bool] -> Io Bool
// Returns a function that (when called) executes
// the given side effects one after another until one of it returns true.
template <typename Container>
auto execute_serially_until_success(const Container& effs)
{
    using Effect = typename Container::value_type;
    using Result = internal::invoke_result_t<Effect>;
    static_assert(std::is_convertible<Result, bool>::value,
                  "Effects must return a boolish type.");
    return [effs]() -> bool
    {
        for (const Effect& e : effs)
        {
            if (internal::invoke(e))
            {
                return true;
            }
        }
        return false;
    };
}

// API search type: execute_and_return_fixed_value : (a, [Io b]) -> Io a
// Returns a function that executes the given side effect
// and returns a fixed value when called.
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

// Converts an arbitrary callable effect to an std::function.
template <typename Effect>
std::function<internal::invoke_result_t<Effect> ()> effect_to_std_function(Effect eff)
{
    return [eff]
    {
        return internal::invoke(eff);
    };
}

// API search type: execute_max_n_times_until_success : (Int, Io (), Int) -> Io Bool
// Returns a function that (when called) executes a side effect
// until it succeds once or the maximum number
// of attempts with an optional pause in between.
template <typename Effect>
auto execute_max_n_times_until_success(std::size_t n,
                                       const Effect& eff,
                                       std::size_t pause_in_milliseconds = 0)
{
    if (pause_in_milliseconds > 0)
    {
        auto sleep_and_return_false =
            execute_and_return_fixed_value(
                false,
                sleep_for_n_milliseconds(pause_in_milliseconds));
        return execute_serially_until_success(
            intersperse(
                sleep_and_return_false,
                replicate(n, effect_to_std_function(eff))));
    }
    return execute_serially_until_success(
        replicate(n, effect_to_std_function(eff)));
}

// API search type: execute_n_times : (Int, Io a) -> Io ()
// Returns a function that (when called) executes n times
// the provided side effect function. 
// The return values (if present) are dropped.
template<typename Effect>
auto execute_n_times(std::size_t n, const Effect& eff)
{
    for (auto _ : fplus::numbers(static_cast<size_t>(0), n))
    {
        (void) _; // suppress warning / unused variable
        eff();
    }
}

// API search type: execute_serially_until_failure : [Io Bool] -> Io Bool
// Returns a function that (when called) executes the given side effects
// one after another until one of them returns false.
template <typename Container>
std::function<bool()> execute_serially_until_failure(const Container& effs)
{
    using Effect = typename Container::value_type;
    using Result = internal::invoke_result_t<Effect>;
    static_assert(std::is_convertible<Result, bool>::value,
        "Effects must return a boolish type.");
    return [effs]() -> bool
    {
        for (const Effect& e : effs)
        {
            if (!internal::invoke(e))
            {
                return false;
            }
        }
        return true;
    };
}

// API search type: execute_parallelly : [Io a] -> Io [a]
// Returns a function that (when called) executes the given side effects
// in parallel and returns the collected results.
template <typename Container>
auto execute_parallelly(const Container& effs)
{
    using Effect = typename Container::value_type;
    using Result = internal::invoke_result_t<Effect>;
    return [effs] {
        auto handles = transform(
            [](Effect e) { return std::async(std::launch::async, e); }, effs);

        std::vector<std::decay_t<Result>> results;
        results.reserve(size_of_cont(handles));
        for (auto& handle : handles)
        {
            results.push_back(handle.get());
        }
        return results;
    };
}

// API search type: execute_fire_and_forget : Io a -> Io a
// Returns a function that (when called) executes the given side effect
// in a new thread and returns immediately.
template <typename Effect>
std::function<void()> execute_fire_and_forget(Effect eff)
{
    return [eff]()
    {
        std::thread t(eff);
        t.detach();
    };
}

// API search type: read_text_file_maybe : String -> Io (Maybe String)
// Returns a function that reads the content of a text file when called.
inline
std::function<maybe<std::string>()> read_text_file_maybe(
    const std::string& filename)
{
    return [filename]() -> maybe<std::string>
    {
        std::ifstream input(filename);
        if (!input.good())
            return {};
        return just(std::string(
                std::istreambuf_iterator<std::string::value_type>(input),
                std::istreambuf_iterator<std::string::value_type>()));
    };
}

// API search type: read_text_file : String -> Io String
// Returns a function that reads the content of a text file when called.
// This function then returns an empty string if the file could not be read.
inline
std::function<std::string()> read_text_file(const std::string& filename)
{
    return [filename]() -> std::string
    {
        return just_with_default(
            std::string(),

            read_text_file_maybe(filename)());
    };
}

// API search type: read_binary_file_maybe : String -> Io (Maybe [Int])
// Returns a function that reads the content of a binary file when executed.
inline
std::function<maybe<std::vector<std::uint8_t>>()> read_binary_file_maybe(
    const std::string& filename)
{
    return [filename]() -> maybe<std::vector<std::uint8_t>>
    {
        std::ifstream file(filename, std::ios::binary);
        if (!file.good())
            return {};
        file.unsetf(std::ios::skipws);
        std::streampos fileSize;
        file.seekg(0, std::ios::end);
        fileSize = file.tellg();
        if (fileSize == static_cast<std::streamsize>(0))
            return {};
        file.seekg(0, std::ios::beg);
        std::vector<std::uint8_t> vec(static_cast<std::size_t>(fileSize), 0);
        file.read(reinterpret_cast<char*>(&vec[0]), fileSize);
        return vec;
    };
}

// API search type: read_binary_file : String -> Io [Int]
// Returns a function that reads the content of a binary file when executed.
// This function then returns an empty vector if the file could not be read.
inline
std::function<std::vector<std::uint8_t>()> read_binary_file(
    const std::string& filename)
{
    return [filename]() -> std::vector<std::uint8_t>
    {
        return just_with_default(
            std::vector<std::uint8_t>(),
            read_binary_file_maybe(filename)());
    };
}

// API search type: read_text_file_lines_maybe : (String, Bool) -> Io (Maybe [String])
// Returns a function that (when called) reads the content of a text file
// and returns it line by line.
inline
std::function<maybe<std::vector<std::string>>()> read_text_file_lines_maybe(
        bool allow_empty, const std::string& filename)
{
    return [filename, allow_empty]() -> maybe<std::vector<std::string>>
    {
        const auto maybe_content = read_text_file_maybe(filename)();
        if (maybe_content.is_nothing())
            return {};
        else
            return split_lines(allow_empty, maybe_content.unsafe_get_just());
    };
}

// API search type: read_text_file_lines : (String, Bool) -> Io [String]
// Returns a function that (when called) reads the content of a text file
// and returns it line by line.
// This function then returns an empty vector if the file could not be read.
inline
std::function<std::vector<std::string>()> read_text_file_lines(
        bool allow_empty, const std::string& filename)
{
    return [filename, allow_empty]() -> std::vector<std::string>
    {
        return just_with_default(
            std::vector<std::string>(),
            read_text_file_lines_maybe(allow_empty, filename)());
    };
}

// API search type: write_text_file : (String, String) -> Io Bool
// Returns a function that (when called) writes content into a text file,
// replacing it if it already exists.
inline
std::function<bool()> write_text_file(const std::string& filename,
        const std::string& content)
{
    return [filename, content]() -> bool
    {
        std::ofstream output(filename);
        output << content;
        return output.good();
    };
}

// API search type: write_binary_file : (String, [Int]) -> Io Bool
// Returns a function that (when called) writes content into a binary file,
// replacing it if it already exists.
inline
std::function<bool()> write_binary_file(const std::string& filename,
        const std::vector<uint8_t>& content)
{
    return [filename, content]() -> bool
    {
        std::ofstream file(filename, std::ios::binary);
        file.write(reinterpret_cast<const char*>(&content[0]),
            static_cast<std::streamsize>(content.size()));
        return file.good();
    };
}

// API search type: write_text_file_lines : (String, [String], Bool) -> Io Bool
// Returns a function that (when called) writes lines into a text file,
// replacing it if it already exists.
inline
std::function<bool()> write_text_file_lines(bool trailing_newline,
        const std::string& filename,
        const std::vector<std::string>& lines)
{
    std::string content = join(std::string("\n"), lines);
    if (trailing_newline)
    {
        content += "\n";
    }
    return write_text_file(filename, content);
}

// API search type: execute_effect : Io a -> a
// Simply run a side effect (call a function without parameters)
// and returns the result.
// Can be useful for chaining.
template <typename F>
auto execute_effect(const F f)
{
    return internal::invoke(f);
}

// API search type: interact : (String -> String) -> Io ()
// Takes a function F of type (String -> String)
// and returns a function that
// reads the entire input from standard input,
// passes it through the given function,
// and writes the result to standard output.
template <typename F>
std::function<void()> interact(F f)
{
    return [f]() -> void
    {
        std::cout << f(std::string(
            std::istreambuf_iterator<char>(std::cin.rdbuf()),
            std::istreambuf_iterator<char>()));
    };
}

// API search type: execute_with_maybe : ((a -> void), Maybe a) -> Io Bool
// Returns a function that
// akes a unary side-effect function with
// a maybe holding a matching type
// and runs the sideeffect if the Maybe holds a just.
// The returned function returns false if the maybe was a nothing.
template <typename Effect, typename X>
std::function<bool()> execute_with_maybe(Effect eff, const maybe<X>& m)
{
    return [eff, m]() -> bool
    {
        if (m.is_nothing())
        {
            return false;
        }
        eff(m.unsafe_get_just());
        return true;
    };
}

} // namespace fplus
