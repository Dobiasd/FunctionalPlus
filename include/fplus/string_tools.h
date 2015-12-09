// Copyright Tobias Hermann 2015.
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

// Is character alphanumerical?
template <typename String>
bool is_letter_or_digit(const typename String::value_type& c)
{
    typedef typename String::value_type C;
    auto IsDigit = bind_1st_and_2nd_of_3(is_in_range<C>, 48, 58);
    auto IsLowerLetter = bind_1st_and_2nd_of_3(is_in_range<C>, 65, 91);
    auto IsUpperLetter = bind_1st_and_2nd_of_3(is_in_range<C>, 97, 123);
    auto IsLetter = logical_or(IsLowerLetter, IsUpperLetter);
    return IsDigit(c) || IsLetter(c);
}

// Is character a whitespace.
template <typename String>
bool is_whitespace(const typename String::value_type& c)
{
    typedef typename String::value_type C;
    return (c == 32 || is_in_range<C>(9, 14, c));
}

// Newline character ('\n')?
template <typename String>
bool is_line_break(const typename String::value_type& c)
{
    return c == '\n';
}

// Replaces windows and mac newlines with linux newlines.
template <typename String>
String clean_newlines(const String& str)
{
    return replace_elems('\r', '\n',
        replace_tokens(String("\r\n"), String("\n"), str));
}

// Splits a string by the found whitespace characters.
// split_words("How are you?") == ["How", "are", "you?"]
template <typename String, typename ContainerOut = std::list<String>>
ContainerOut split_words(const String& str)
{
    return split_by(logical_not(is_letter_or_digit<String>), false, str);
}

// Splits a string by the found newlines.
// split_lines("Hi,\nhow are you?") == ["Hi,", "How are you"]
template <typename String, typename ContainerOut = std::list<String>>
ContainerOut split_lines(const String& str, bool allowEmpty)
{
    return split_by(is_line_break<String>, allowEmpty, clean_newlines(str));
}

// trim_whitespace_left("    text  ") == "text  "
template <typename String>
String trim_whitespace_left(const String& str)
{
    return trim_left_by(is_whitespace<String>, str);
}

// trim_whitespace_right("    text  ") == "    text"
template <typename String>
String trim_whitespace_right(const String& str)
{
    return trim_right_by(is_whitespace<String>, str);
}

// trim_whitespace("    text  ") == "text"
template <typename String>
String trim_whitespace(const String& str)
{
    return trim_by(is_whitespace<String>, str);
}

} // namespace fplus