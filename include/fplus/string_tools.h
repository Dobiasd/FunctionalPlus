// Copyright Tobias Hermann 2015.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "replace.h"
#include "split.h"
#include "transform.h"

#include <string>

namespace fplus
{

// API search type: is_letter_or_digit : char -> bool
// Is character alphanumerical?
template <typename String>
bool is_letter_or_digit(const typename String::value_type& c)
{
    auto IsDigit = is_in_range(48, 58);
    auto IsLowerLetter = is_in_range(65, 91);
    auto IsUpperLetter = is_in_range(97, 123);
    auto IsLetter = logical_or(IsLowerLetter, IsUpperLetter);
    return IsDigit(c) || IsLetter(c);
}

// API search type: is_whitespace : char -> bool
// Is character a whitespace.
template <typename String>
bool is_whitespace(const typename String::value_type& c)
{
    return (c == 32 || is_in_range(9, 14)(c));
}

// API search type: is_line_break : char -> bool
// Newline character ('\n')?
template <typename String>
bool is_line_break(const typename String::value_type& c)
{
    return c == '\n';
}

// API search type: clean_newlines : string -> string
// Replaces windows and mac newlines with linux newlines.
template <typename String>
String clean_newlines(const String& str)
{
    return replace_elems('\r', '\n',
        replace_tokens(String("\r\n"), String("\n"), str));
}

// API search type: split_words : string, bool -> [string]
// Splits a string by non-letter and non-digit characters.
// split_words("How are you?", false) == ["How", "are", "you"]
template <typename String, typename ContainerOut = std::vector<String>>
ContainerOut split_words(const String& str, const bool allowEmpty)
{
    return split_by(logical_not(is_letter_or_digit<String>), allowEmpty, str);
}

// API search type: split_words_by : string, char, bool -> [string]
// Splits a string by non-letter and non-digit characters.
// split_words("How-are you?", ' ', false) == ["How-are", "you?"]
template <typename String, typename ContainerOut = std::vector<String>>
ContainerOut split_words_by
        (const String& str, const typename String::value_type delim,
         const bool allowEmpty)
{
    const auto comparator = [delim](const typename String::value_type ch)
    {
        return ch == delim;
    };
    return split_by(comparator, allowEmpty, str);
}

// API search type: split_words_by_many : string, [char], bool -> [string]
// Splits a string by the given delimiter(s).
// split_words_by_many("How are u?", " o", false) == ["H","w","are","u?"]
template <typename String, typename ContainerOut = std::vector<String>>
ContainerOut split_words_by_many
        (const String& str, const String& delims, const bool allowEmpty)
{
    typedef typename String::value_type CharType;
    const auto comparator = [&delims](const CharType ch)
    {
        return std::any_of(std::begin(delims), std::end(delims),
                    [ch](const CharType delim) { return ch == delim; });
    };
    return split_by(comparator, allowEmpty, str);
}

// API search type: split_lines : string, bool -> [string]
// Splits a string by the found newlines.
// split_lines("Hi,\nhow are you?", false) == ["Hi,", "How are you"]
template <typename String, typename ContainerOut = std::vector<String>>
ContainerOut split_lines(const String& str, bool allowEmpty)
{
    return split_by(is_line_break<String>, allowEmpty, clean_newlines(str));
}

// API search type: trim_whitespace_left : string -> string
// trim_whitespace_left("    text  ") == "text  "
template <typename String>
String trim_whitespace_left(const String& str)
{
    return trim_left_by(is_whitespace<String>, str);
}

// API search type: trim_whitespace_right : string -> string
// trim_whitespace_right("    text  ") == "    text"
template <typename String>
String trim_whitespace_right(const String& str)
{
    return trim_right_by(is_whitespace<String>, str);
}

// API search type: trim_whitespace : string -> string
// trim_whitespace("    text  ") == "text"
template <typename String>
String trim_whitespace(const String& str)
{
    return trim_by(is_whitespace<String>, str);
}

// API search type: to_string_fill_left : char, int, a -> string
// to_string_fill_left('0', 5, 42) == "00042"
template <typename T>
std::string to_string_fill_left( const std::string::value_type& filler,
        std::size_t min_size, const T& x)
{
    return fill_left(filler, min_size, std::to_string(x));
}

// API search type: to_string_fill_right : char, int, a -> string
// to_string_fill_right(' ', 5, 42) == "42   "
template <typename T>
std::string to_string_fill_right( const std::string::value_type& filler,
        std::size_t min_size, const T& x)
{
    return fill_right(filler, min_size, std::to_string(x));
}

} // namespace fplus
