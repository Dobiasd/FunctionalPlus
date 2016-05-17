// Copyright Tobias Hermann 2015.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "container_common.h"
#include "generate.h"
#include "maybe.h"
#include "transform.h"

#include <iomanip>
#include <ios>
#include <sstream>
#include <string>

namespace fplus
{

// API search type: show : a -> string
// 42 -> "42"
template <typename T>
std::string show(const T& x)
{
    std::ostringstream ss;
    ss << x;
    return ss.str();
}

// API search type: show : string -> string
// string identity
// "foo" -> "foo"
inline
std::string show(const std::string& str)
{
    return str;
}

// API search type: show : (a, b) -> string
// {1, "one"} -> "(1, one)"
template <typename X, typename Y>
std::string show(const std::pair<X, Y>& p)
{
    return std::string("(") + show(p.first) + ", " + show(p.second) + ")";
}

// API search type: show_cont_with_frame_and_newlines : string, string, string, [a], int -> string
// show_cont_with_frame_and_newlines (",", "(", ")", [1, 2, 3, 4, 5], 2)
// == "(1,2)
//      3,4)
//      5)"
template <typename Container>
std::string show_cont_with_frame_and_newlines(
    const std::string& separator,
    const std::string& prefix, const std::string& suffix,
    const Container& xs,
    std::size_t new_line_every_nth_elem )
{
    std::vector<std::string> elemStrs;
    elemStrs.reserve(xs.size());
    if (new_line_every_nth_elem == 0)
    {
        for (const auto& x : xs)
        {
            elemStrs.push_back(show(x));
        }
    }
    else
    {
        std::size_t i = 0;
        std::string newline =
            std::string("\n") + std::string(prefix.size(), ' ');
        for (const auto& x : xs)
        {
            if ( i && i % new_line_every_nth_elem == 0)
                elemStrs.push_back(newline + show(x));
            else
                elemStrs.push_back(show(x));
            ++i;
        }
    }
    return prefix + join(separator, elemStrs) + suffix;
}

// API search type: show_cont_with_frame : string, string, string, [a] -> string
// show_cont_with_frame (" => ", "{", "}", [1, 2, 3]) == "{1 => 2 => 3}"
template <typename Container>
std::string show_cont_with_frame(
    const std::string& separator,
    const std::string& prefix, const std::string& suffix,
    const Container& xs)
{
    return
        show_cont_with_frame_and_newlines( separator, prefix, suffix, xs, 0);
}

// API search type: show_cont_with : string, [a] -> string
// show_cont_with( " - ", [1, 2, 3]) == "[1 - 2 - 3]"
template <typename Container>
std::string show_cont_with(const std::string& separator, const Container& xs)
{
    return show_cont_with_frame(separator, "[", "]", xs);
}

// API search type: show_cont : [a] -> string
// show_cont [1, 2, 3] -> "[1, 2, 3]"
template <typename Container>
std::string show_cont(const Container& xs)
{
    return show_cont_with(", ", xs);
}

// API search type: show_maybe : maybe a -> string
// show_maybe(Just 42) -> "Just 42"
template <typename T>
std::string show_maybe(const maybe<T>& maybe)
{
    if (is_nothing(maybe))
        return "Nothing";
    else
        return std::string("Just " + show(unsafe_get_just(maybe)));
}

// API search type: show_result : result a b -> string
// show_result(Ok 42) -> "Ok 42"
// show_result(Error "fail") -> "Error fail"
template <typename Ok, typename Error>
std::string show_result(const result<Ok, Error>& result)
{
    if (is_error(result))
        return std::string("Error " + show(unsafe_get_error(result)));
    else
        return std::string("Ok " + show(unsafe_get_ok(result)));
}

template <typename T>
std::function<std::string(const T&)>
show_float(std::size_t min_left_chars, std::size_t right_char_count,
        std::string::value_type fillChar)
{
    return [min_left_chars, right_char_count, fillChar](const T& x)
            -> std::string
    {
        std::stringstream stream;
        stream << std::fixed << std::setprecision(right_char_count) << x;
        std::string s = stream.str();
        std::size_t min_dest_length = min_left_chars + 1 + right_char_count;
        return fill_left(fillChar, min_dest_length, s);
    };
}

} // namespace fplus
