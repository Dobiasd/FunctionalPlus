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

namespace
{
    template <typename T>
    void read_value_helper(const std::string& str, T& result, std::size_t& num_chars_used);

    template <>
    void read_value_helper(const std::string& str, int& result, std::size_t& num_chars_used)
    {
        result = std::stoi(str, &num_chars_used);
    }
    template <>
    void read_value_helper(const std::string& str, long& result, std::size_t& num_chars_used)
    {
        result = std::stol(str, &num_chars_used);
    }
    template <>
    void read_value_helper(const std::string& str, long long& result, std::size_t& num_chars_used)
    {
        result = std::stoll(str, &num_chars_used);
    }
    template <>
    void read_value_helper(const std::string& str, float& result, std::size_t& num_chars_used)
    {
        result = std::stof(str, &num_chars_used);
    }
    template <>
    void read_value_helper(const std::string& str, double& result, std::size_t& num_chars_used)
    {
        result = std::stod(str, &num_chars_used);
    }
    template <>
    void read_value_helper(const std::string& str, long double& result, std::size_t& num_chars_used)
    {
        result = std::stold(str, &num_chars_used);
    }
}

template <typename T>
result<T, std::string> read_value_result(const std::string& str)
{
    static_assert(std::is_signed<T>::value, "Only signed types supported");
    try
    {
        T result;
        std::size_t num_chars_used = 0;
        read_value_helper(str, result, num_chars_used);
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

template <typename T>
maybe<T> read_value(const std::string& str)
{
    return to_maybe(read_value_result<T>(str));
}

} // namespace fplus