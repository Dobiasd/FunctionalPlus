// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <fplus/replace.hpp>
#include <fplus/split.hpp>
#include <fplus/transform.hpp>

#include <cctype>
#include <string>

namespace fplus
{

// API search type: is_letter_or_digit : Char -> Bool
// fwd bind count: 0
// Is character alphanumerical?
template <typename String>
bool is_letter_or_digit(const typename String::value_type& c)
{
    return
        std::isdigit(static_cast<unsigned char>(c)) ||
        std::isalpha(static_cast<unsigned char>(c));
}

// API search type: is_whitespace : Char -> Bool
// fwd bind count: 0
// Is character a whitespace.
template <typename String>
bool is_whitespace(const typename String::value_type& c)
{
    return (c == 32 || is_in_interval(9, 14, static_cast<int>(c)));
}

// API search type: is_line_break : Char -> Bool
// fwd bind count: 0
// Newline character ('\n')?
template <typename String>
bool is_line_break(const typename String::value_type& c)
{
    return c == '\n';
}

// API search type: clean_newlines : String -> String
// fwd bind count: 0
// Replaces windows and mac newlines with linux newlines.
template <typename String>
String clean_newlines(const String& str)
{
    return replace_elems('\r', '\n',
        replace_tokens(String("\r\n"), String("\n"), str));
}

// API search type: split_words : (Bool, String) -> [String]
// fwd bind count: 1
// Splits a string by non-letter and non-digit characters.
// split_words(false, "How are you?") == ["How", "are", "you"]
template <typename String, typename ContainerOut = std::vector<String>>
ContainerOut split_words(const bool allowEmpty, const String& str)
{
    return split_by(logical_not(is_letter_or_digit<String>), allowEmpty, str);
}

// API search type: split_words_by : (Char, Bool, String) -> [String]
// fwd bind count: 2
// Splits a string by non-letter and non-digit characters.
// split_words(' ', false, "How-are you?") == ["How-are", "you?"]
template <typename String, typename ContainerOut = std::vector<String>>
ContainerOut split_words_by
        (const typename String::value_type delim,
         const bool allowEmpty, const String& str)
{
    const auto comparator = [delim](const typename String::value_type ch)
    {
        return ch == delim;
    };
    return split_by(comparator, allowEmpty, str);
}

// API search type: split_words_by_many : ([Char], Bool, String) -> [String]
// fwd bind count: 2
// Splits a string by the given delimiter(s).
// split_words_by_many(" o", false, "How are u?") == ["H","w","are","u?"]
template <typename String, typename ContainerOut = std::vector<String>>
ContainerOut split_words_by_many
        (const String& delims, const bool allowEmpty, const String& str)
{
    typedef typename String::value_type CharType;
    const auto comparator = [&delims](const CharType ch)
    {
        return std::any_of(std::begin(delims), std::end(delims),
                    [ch](const CharType delim) { return ch == delim; });
    };
    return split_by(comparator, allowEmpty, str);
}

// API search type: split_lines : (Bool, String) -> [String]
// fwd bind count: 1
// Splits a string by the found newlines.
// split_lines(false, "Hi,\nhow are you?") == ["Hi,", "How are you"]
template <typename String, typename ContainerOut = std::vector<String>>
ContainerOut split_lines(bool allowEmpty, const String& str)
{
    return split_by(is_line_break<String>, allowEmpty, clean_newlines(str));
}

// API search type: trim_whitespace_left : String -> String
// fwd bind count: 0
// trim_whitespace_left("    text  ") == "text  "
template <typename String>
String trim_whitespace_left(const String& str)
{
    return trim_left_by(is_whitespace<String>, str);
}

// API search type: trim_whitespace_right : String -> String
// fwd bind count: 0
// trim_whitespace_right("    text  ") == "    text"
template <typename String>
String trim_whitespace_right(const String& str)
{
    return trim_right_by(is_whitespace<String>, str);
}

// API search type: trim_whitespace : String -> String
// fwd bind count: 0
// trim_whitespace("    text  ") == "text"
template <typename String>
String trim_whitespace(const String& str)
{
    return trim_by(is_whitespace<String>, str);
}

// API search type: to_lower_case : String -> String
// fwd bind count: 0
// to_lower_case("ChaRacTer&WorDs23") == "character&words23"
template <typename String>
String to_lower_case(const String& str)
{
    typedef typename String::value_type Char;
    return transform([](Char c) -> Char
        {
            return static_cast<Char>(
                std::tolower(static_cast<unsigned char>(c)));
        }, str);
}

// API search type: to_upper_case : String -> String
// fwd bind count: 0
// to_upper_case("ChaRacTer&WorDs34") == "CHARACTER&WORDS34"
template <typename String>
String to_upper_case(const String& str)
{
    typedef typename String::value_type Char;
    return transform([](Char c) -> Char
        {
            return static_cast<Char>(
                std::toupper(static_cast<unsigned char>(c)));
        }, str);
}

// API search type: to_string_fill_left : (Char, Int, a) -> String
// fwd bind count: 2
// to_string_fill_left('0', 5, 42) == "00042"
// to_string_fill_left(' ', 5, 42) == "   42"
template <typename T>
std::string to_string_fill_left(const std::string::value_type& filler,
        std::size_t min_size, const T& x)
{
    return fill_left(filler, min_size, std::to_string(x));
}

// API search type: to_string_fill_right : (Char, Int, a) -> String
// fwd bind count: 2
// to_string_fill_right(' ', 5, 42) == "42   "
template <typename T>
std::string to_string_fill_right(const std::string::value_type& filler,
        std::size_t min_size, const T& x)
{
    return fill_right(filler, min_size, std::to_string(x));
}

} // namespace fplus
