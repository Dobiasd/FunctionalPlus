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

#include <atomic>
#include <chrono>
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
    class ticker
    {
    public:
        typedef std::function<void(std::int64_t)> function;
        ticker(const function& f, std::int64_t interval_us) :
            f_(f),
            interval_us_(interval_us),
            stop_flag_(false),
            thread_(std::bind(&ticker::thread_function, this))
        {}
        ~ticker() {
            stop_flag_ = true;
            if (thread_.joinable())
            {
                thread_.join();
            }
        }
    private:
        void thread_function() const
        {
            auto last_time = std::chrono::high_resolution_clock::now();
            while (!stop_flag_)
            {
                const auto current_time =
                    std::chrono::high_resolution_clock::now();
                const auto sleep_time =
                    std::chrono::microseconds{ interval_us_ } -
                        (current_time - last_time);
                std::this_thread::sleep_for(sleep_time);
                const auto elapsed = current_time - last_time;
                last_time = current_time;
                const auto elapsed_us =
                    std::chrono::duration_cast<std::chrono::microseconds>(
                        elapsed).count();
                f_(elapsed_us);
            }
        }
        function f_;
        std::int64_t interval_us_;
        std::atomic<bool> stop_flag_;
        std::thread thread_;
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
template <typename Container,
        typename Effect = typename Container::value_type,
        typename Result = typename std::result_of<Effect()>::type>
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

// API search type: execute_serially_until_success : [Io Bool] -> Io Bool
// Returns a function that (when called) executes
// the given side effects one after another until one of it returns true.
template <typename Container,
        typename Effect = typename Container::value_type,
        typename Result = typename std::result_of<Effect()>::type>
std::function<bool()> execute_serially_until_success(const Container& effs)
{
    static_assert(std::is_convertible<Result, bool>::value,
        "Effects must return a boolish type.");
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
template <typename Effect,
        typename Result = typename std::result_of<Effect()>::type>
std::function<Result()> effect_to_std_function(Effect eff)
{
    return [eff]() -> Result
    {
        return eff();
    };
}

// API search type: execute_max_n_times_until_success : (Int, Io (), Int) -> Io Bool
// Returns a function that (when called) executes a side effect
// until it succeds once or the maximum number
// of attempts with an optional pause in between.
template <typename Effect,
        typename Result = typename std::result_of<Effect()>::type>
std::function<bool()> execute_max_n_times_until_success(
        std::size_t n,
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

// API search type: execute_serially_until_failure : [Io Bool] -> Io Bool
// Returns a function that (when called) executes the given side effects
// one after another until one of them returns false.
template <typename Container,
        typename Effect = typename Container::value_type,
        typename Result = typename std::result_of<Effect()>::type>
std::function<bool()> execute_serially_until_failure(const Container& effs)
{
    static_assert(std::is_convertible<Result, bool>::value,
        "Effects must return a boolish type.");
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

// API search type: execute_parallelly : [Io a] -> Io [a]
// Returns a function that (when called) executes the given side effects
// in parallel and returns the collected results.
template <typename Container,
        typename Effect = typename Container::value_type,
        typename Result = typename std::result_of<Effect()>::type>
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

// API search type: read_binary_file : String -> Io (Maybe [Int])
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

// API search type: read_text_file_lines : (String, Bool) -> Io (Maybe [String])
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
template <typename F,
    typename FOut = typename std::result_of<F()>::type>
FOut execute_effect(const F f)
{
    return f();
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

} // namespace fplus
