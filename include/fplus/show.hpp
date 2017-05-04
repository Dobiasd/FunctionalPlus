// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <fplus/container_common.hpp>
#include <fplus/generate.hpp>
#include <fplus/maybe.hpp>
#include <fplus/transform.hpp>

#include <iomanip>
#include <ios>
#include <sstream>
#include <string>

namespace fplus
{

// API search type: show : a -> String
// fwd bind count: 0
// 42 -> "42"
// Useful to simply show values, e.g.:
// Int to String
// Float to String
// Double to String
// Pair to String
// std::vector<std::list<T>> to String
// std::vector<T> to String
template <typename T>
std::string show(const T& x)
{
    std::ostringstream ss;
    ss << x;
    return ss.str();
}

// string identity
// "foo" -> "foo"
inline
std::string show(const std::string& str)
{
    return str;
}

template <typename T, typename A>
std::string show(const std::vector<T, A>& xs);

template <typename T, typename A>
std::string show(const std::list<T, A>& xs);

// {1, "one"} -> "(1, one)"
template <typename X, typename Y>
std::string show(const std::pair<X, Y>& p)
{
    return std::string("(") + show(p.first) + ", " + show(p.second) + ")";
}

template <typename Container> std::string show_cont(const Container& xs);

template <typename T, typename A>
std::string show(const std::vector<T, A>& xs)
{
    return show_cont(xs);
}

template <typename T, typename A>
std::string show(const std::list<T, A>& xs)
{
    return show_cont(xs);
}

template <typename T, typename A>
std::string show(const std::set<T, A>& xs)
{
    return show_cont(xs);
}

template <typename T, typename A>
std::string show(const std::deque<T, A>& xs)
{
    return show_cont(xs);
}

// API search type: show_cont_with_frame_and_newlines : (String, String, String, [a], Int) -> String
// fwd bind count: 3
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

// API search type: show_cont_with_frame : (String, String, String, [a]) -> String
// fwd bind count: 3
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

// API search type: show_cont_with : (String, [a]) -> String
// fwd bind count: 1
// show_cont_with( " - ", [1, 2, 3]) == "[1 - 2 - 3]"
template <typename Container>
std::string show_cont_with(const std::string& separator, const Container& xs)
{
    return show_cont_with_frame(separator, "[", "]", xs);
}

// API search type: show_cont : [a] -> String
// fwd bind count: 0
// show_cont [1, 2, 3] -> "[1, 2, 3]"
// Can i.a show std::vector and std::map.
template <typename Container>
std::string show_cont(const Container& xs)
{
    return show_cont_with(", ", xs);
}

// API search type: show_maybe : Maybe a -> String
// fwd bind count: 0
// show_maybe(Just 42) -> "Just 42"
template <typename T>
std::string show_maybe(const maybe<T>& maybe)
{
    if (is_nothing(maybe))
        return "Nothing";
    else
        return std::string("Just " + show(unsafe_get_just(maybe)));
}

// API search type: show_result : Result a b -> String
// fwd bind count: 0
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

// API search type: show_float : (Int, Int, Float) -> String
// fwd bind count: 2
// Can be used to show floating point values in a specific format
// (Float to String, Double to String etc.)
// Examples:
// const double pi = 3.14159
// show_float<double>(0, 3, pi) == "3.142"
// show_float<double>(1, 3, pi) == "3.142"
// show_float<double>(2, 3, pi) == "03.142"
// show_float<double>(3, 3, pi) == "003.142"
// show_float<double>(1, 2, pi) == "3.14"
// show_float<double>(1, 4, pi) == "3.1416"
// show_float<double>(1, 7, pi) == "3.1415900"
// show_float<double>(0, 3, -pi) == "-3.142"
// show_float<double>(1, 3, -pi) == "-3.142"
// show_float<double>(2, 3, -pi) == "-3.142"
// show_float<double>(3, 3, -pi) == "-03.142"
// show_float<double>(4, 3, -pi) == "-003.142"
// show_float<double>(0, 3, 0.142) == "0.142";
// show_float<double>(1, 3, 0.142) == "0.142";
// show_float<double>(2, 3, 0.142) == "00.142";
// fill_left(8, ' ', show_float<double>(0, 3, -pi)) == "  -3.142"
template <typename T>
std::string show_float(
    std::size_t min_left_chars, std::size_t right_char_count, const T& x)
{
    bool is_negative = x < 0;
    std::size_t min_left_chars_final =
        is_negative && min_left_chars > 0
        ? min_left_chars - 1
        : min_left_chars;
    std::stringstream stream;
    stream
        << std::fixed
        << std::setprecision(static_cast<int>(right_char_count))
        << std::abs(x);
    std::string s = stream.str();
    std::size_t min_dest_length = min_left_chars_final + 1 + right_char_count;
    std::string result = fill_left('0', min_dest_length, s);
    if (is_negative)
    {
        result = std::string("-") + result;
    }
    return result;
}

// API search type: show_float_fill_left : (Char, Int, Int, Float) -> String
// fwd bind count: 3
// Can be used to show floating point values in a specific precision
// left-padded with some character.
// (Float to String, Double to String etc.)
// Examples:
// const double pi = 3.14159
// show_float_fill_left<double>(' ', 8, 3, pi) == "   3.142"
// show_float_fill_left<double>(' ', 8, 6, pi) == "3.141590"
// show_float_fill_left<double>(' ', 8, 3, -pi) == "  -3.142"
// show_float_fill_left<double>(' ', 2, 3, -pi) == "-3.142"
template <typename T>
std::string show_float_fill_left(const std::string::value_type& filler,
        std::size_t min_size, std::size_t right_char_count, const T& x)
{
    return fill_left(filler, min_size, show_float<T>(0, right_char_count, x));
}

// API search type: show_fill_left : (Char, Int, a) -> String
// fwd bind count: 2
// Convert some value to a string with left-padded with some character.
// (Int to String etc.)
// Examples:
// show_fill_left<int>(' ', 4, 3) == "   3"
// show_fill_left<int>('0', 4, 3) == "0003"
// show_fill_left<int>(' ', 4, 12345) == "12345"
template <typename T>
std::string show_fill_left(
    const std::string::value_type& filler, std::size_t min_size, const T& x)
{
    return fill_left(filler, min_size, show<T>(x));
}

// API search type: show_fill_right : (Char, Int, a) -> String
// fwd bind count: 2
// Convert some value to a string with left-padded with some character.
// (Int to String etc.)
// Examples:
// show_fill_right<int>(' ', 4, 3) == "3   "
// show_fill_right<int>(' ', 4, 12345) == "12345"
template <typename T>
std::string show_fill_right(const std::string::value_type& filler,
    std::size_t min_size, const T& x)
{
    return fill_right(filler, min_size, show<T>(x));
}

} // namespace fplus
