// Copyright Tobias Hermann 2015.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "maybe.h"
#include "result.h"

#include <string>
#include <type_traits>

namespace fplus
{


template <typename T>
struct internal_helper_read_value_struct {};

template <>
struct internal_helper_read_value_struct <int>
{
    static void read(const std::string& str,
        int& result, std::size_t& num_chars_used)
    {
        result = std::stoi(str, &num_chars_used);
    }
};

template <>
struct internal_helper_read_value_struct <long>
{
    static void read(const std::string& str,
        long& result, std::size_t& num_chars_used)
    {
        result = std::stol(str, &num_chars_used);
    }
};

template <>
struct internal_helper_read_value_struct <long long>
{
    static void read(const std::string& str,
        long long& result, std::size_t& num_chars_used)
    {
        result = std::stoll(str, &num_chars_used);
    }
};

template <>
struct internal_helper_read_value_struct <unsigned int>
{
    static void read(const std::string& str,
        unsigned int& result, std::size_t& num_chars_used)
    {
        unsigned long result_u_l = std::stoul(str, &num_chars_used);
        result = static_cast<unsigned int>(result_u_l);
    }
};

template <>
struct internal_helper_read_value_struct <unsigned long>
{
    static void read(const std::string& str,
        unsigned long& result, std::size_t& num_chars_used)
    {
        result = std::stoul(str, &num_chars_used);
    }
};

template <>
struct internal_helper_read_value_struct <unsigned long long>
{
    static void read(const std::string& str,
        unsigned long long& result, std::size_t& num_chars_used)
    {
        result = std::stoull(str, &num_chars_used);
    }
};

template <>
struct internal_helper_read_value_struct <float>
{
    static void read(const std::string& str,
        float& result, std::size_t& num_chars_used)
    {
        result = std::stof(str, &num_chars_used);
    }
};

template <>
struct internal_helper_read_value_struct <double>
{
    static void read(const std::string& str,
        double& result, std::size_t& num_chars_used)
    {
        result = std::stod(str, &num_chars_used);
    }
};

template <>
struct internal_helper_read_value_struct <long double>
{
    static void read(const std::string& str,
        long double& result, std::size_t& num_chars_used)
    {
        result = std::stold(str, &num_chars_used);
    }
};

// API search type: read_value_result : String -> Result a
// Try to deserialize a value.
template <typename T>
result<T, std::string> read_value_result(const std::string& str)
{
    try
    {
        T result;
        std::size_t num_chars_used = 0;
        internal_helper_read_value_struct<T>::read(str,
            result, num_chars_used);
        if (num_chars_used != str.size())
        {
            return error<T>(std::string("String not fully parsable."));
        }
        return ok<T, std::string>(result);
    } catch(const std::invalid_argument& e) {
        return error<T, std::string>(e.what());
    } catch(const std::out_of_range& e) {
        return error<T, std::string>(e.what());
    }
}

// API search type: read_value : String -> Maybe a
// Try to deserialize a value, e.g.:
// String to Int
// String to Float
// String to Double
// read_value<unsigned int>("42") == 42
// etc.
template <typename T>
maybe<T> read_value(const std::string& str)
{
    return to_maybe(read_value_result<T>(str));
}

} // namespace fplus
