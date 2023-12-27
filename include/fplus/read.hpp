// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <fplus/maybe.hpp>
#include <fplus/result.hpp>

#include <string>
#include <type_traits>

namespace fplus
{

namespace internal
{
    template <typename T>
    struct helper_read_value_struct {};

    template <>
    struct helper_read_value_struct <int>
    {
        static void read(const std::string& str,
            int& result, std::size_t& num_chars_used)
        {
            result = std::stoi(str, &num_chars_used);
        }
    };

    template <>
    struct helper_read_value_struct <long>
    {
        static void read(const std::string& str,
            long& result, std::size_t& num_chars_used)
        {
            result = std::stol(str, &num_chars_used);
        }
    };

    template <>
    struct helper_read_value_struct <long long>
    {
        static void read(const std::string& str,
            long long& result, std::size_t& num_chars_used)
        {
            result = std::stoll(str, &num_chars_used);
        }
    };

    template <>
    struct helper_read_value_struct <unsigned int>
    {
        static void read(const std::string& str,
            unsigned int& result, std::size_t& num_chars_used)
        {
            unsigned long result_u_l = std::stoul(str, &num_chars_used);
            result = static_cast<unsigned int>(result_u_l);
        }
    };

    template <>
    struct helper_read_value_struct <unsigned long>
    {
        static void read(const std::string& str,
            unsigned long& result, std::size_t& num_chars_used)
        {
            result = std::stoul(str, &num_chars_used);
        }
    };

    template <>
    struct helper_read_value_struct <unsigned long long>
    {
        static void read(const std::string& str,
            unsigned long long& result, std::size_t& num_chars_used)
        {
            result = std::stoull(str, &num_chars_used);
        }
    };

    template <>
    struct helper_read_value_struct <float>
    {
        static void read(const std::string& str,
            float& result, std::size_t& num_chars_used)
        {
            result = std::stof(str, &num_chars_used);
        }
    };

    template <>
    struct helper_read_value_struct <double>
    {
        static void read(const std::string& str,
            double& result, std::size_t& num_chars_used)
        {
            result = std::stod(str, &num_chars_used);
        }
    };

    template <>
    struct helper_read_value_struct <long double>
    {
        static void read(const std::string& str,
            long double& result, std::size_t& num_chars_used)
        {
            result = std::stold(str, &num_chars_used);
        }
    };

    template <>
    struct helper_read_value_struct <std::string>
    {
        static void read(const std::string& str,
            std::string& result, std::size_t& num_chars_used)
        {
            num_chars_used = str.size();
            result = str;
        }
    };
}

// API search type: read_value_result : String -> Result a
// Try to deserialize a value.
template <typename T>
result<T, std::string> read_value_result(const std::string& str)
{
    try
    {
        T result;
        std::size_t num_chars_used = 0;
        internal::helper_read_value_struct<T>::read(str,
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
// Try to deserialize/parse a value, e.g.:
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

// API search type: read_value_with_default : (a, String) -> a
// fwd bind count: 1
// Try to deserialize a value, return given default on failure, e.g.:
// String to Int
// String to Float
// String to Double
// read_value_with_default<unsigned int>(3, "42") == 42
// read_value_with_default<unsigned int>(3, "") == 3
// read_value_with_default<unsigned int>(3, "foo") == 3
// etc.
template <typename T>
T read_value_with_default(const T& def, const std::string& str)
{
    return just_with_default(def, to_maybe(read_value_result<T>(str)));
}

// API search type: read_value_unsafe : String -> a
// Try to deserialize a value, crash on failure, e.g.:
// String to Int
// String to Float
// String to Double
// read_value_unsafe<unsigned int>("42") == 42
// read_value_unsafe<unsigned int>("") == crash
// read_value_unsafe<unsigned int>("foo") == crash
// See read_value and read_value_with_default for safe versions.
// etc.
template <typename T>
T read_value_unsafe(const std::string& str)
{
    return unsafe_get_just(to_maybe(read_value_result<T>(str)));
}

} // namespace fplus
