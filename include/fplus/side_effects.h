// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "fplus/container_common.h"
#include "fplus/function_traits.h"
#include "fplus/generate.h"
#include "fplus/string_tools.h"

#include <chrono>
#include <fstream>
#include <future>
#include <streambuf>
#include <string>
#include <vector>

namespace fplus
{

// API search type: sleep_for_n_seconds : Int -> Io ()
// Returns a function that suspends the calling thread for n seconds.
inline
std::function<void()> sleep_for_n_seconds(std::size_t seconds)
{
    return [seconds]()
    {
        std::this_thread::sleep_for(std::chrono::seconds(seconds));
    };
}

// API search type: sleep_for_n_milliseconds : Int -> Io ()
// Returns a function that suspends the calling thread for n milliseconds.
inline
std::function<void()> sleep_for_n_milliseconds(std::size_t milliseconds)
{
    return [milliseconds]()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    };
}

// API search type: sleep_for_n_microseconds : Int -> Io ()
// Returns a function that suspends the calling thread for n microseconds.
inline
std::function<void()> sleep_for_n_microseconds(std::size_t microseconds)
{
    return [microseconds]()
    {
        std::this_thread::sleep_for(std::chrono::microseconds(microseconds));
    };
}

// API search type: execute_serially : [Io ()] -> Io ()
// Returns a function that executes the given side effects one after another.
template <typename Container,
        typename Effect = typename Container::value_type,
        typename Result = typename utils::function_traits<
            Effect>::result_type>
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
// Returns a function that executes the given side effects one after another
// until one of it returns true.
template <typename Container,
        typename Effect = typename Container::value_type,
        typename Result = typename utils::function_traits<
            Effect>::result_type>
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
// and returns a fixed value.
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
        typename Result = typename utils::function_traits<
            Effect>::result_type>
std::function<Result()> effect_to_std_function(Effect eff)
{
    return [eff]() -> Result
    {
        return eff();
    };
}

// API search type: execute_max_n_times_until_success : (Int, Io (), Int) -> Io Bool
// Returns a function that executes a side effect until it succeds once
// or the maximum number of attempts with an optional pause in between.
template <typename Effect,
        typename Result = typename utils::function_traits<
            Effect>::result_type>
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
// Returns a function that executes the given side effects one after another
// until one of them returns false.
template <typename Container,
        typename Effect = typename Container::value_type,
        typename Result = typename utils::function_traits<
            Effect>::result_type>
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
// Returns a function that executes the given side effects in parallel
// and returns the collected results.
template <typename Container,
        typename Effect = typename Container::value_type,
        typename Result = typename utils::function_traits<
            Effect>::result_type>
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
// Returns a function that executes the given side effect in a new thread
// and return immediately.
template <typename Effect>
std::function<void()> execute_fire_and_forget(Effect eff)
{
    return [eff]()
    {
        std::thread t(eff);
        t.detach();
    };
}

// API search type: read_text_file : String -> Io String
// Returns a function that reads the content of a text file.
inline
std::function<std::string()> read_text_file(const std::string& filename)
{
    return [filename]() -> std::string
    {
        std::ifstream input(filename);
        if (!input.good())
            return {};
        return std::string(
                std::istreambuf_iterator<std::string::value_type>(input),
                std::istreambuf_iterator<std::string::value_type>());
    };
}

// API search type: read_binary_file : String -> Io [Int]
// Returns a function that reads the content of a binary file.
inline
std::function<std::vector<std::uint8_t>()> read_binary_file(
    const std::string& filename)
{
    return [filename]() -> std::vector<std::uint8_t>
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

// API search type: read_text_file_lines : String -> Io [String]
// Returns a function that reads the content of a text file
// and returns it line by line.
inline
std::function<std::vector<std::string>()> read_text_file_lines(
        const std::string& filename)
{
    return [filename]() -> std::vector<std::string>
    {
        return split_lines(read_text_file(filename)(), true);
    };
}

// API search type: write_text_file : (String, String) -> Io Bool
// Returns a function that writes content into a text file,
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
// Returns a function that writes content into a binary file,
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
// Returns a function that writes lines into a text file,
// replacing it if it already exists.
inline
std::function<bool()> write_text_file_lines(const std::string& filename,
        const std::vector<std::string>& lines, bool trailing_newline = true)
{
    std::string content = join(std::string("\n"), lines);
    if (trailing_newline)
    {
        content += "\n";
    }
    return write_text_file(filename, content);
}

} // namespace fplus
