// Copyright Tobias Hermann 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "Replace.h"
#include "Split.h"
#include "Transform.h"

#include <string>

namespace FunctionalPlus
{

// Is character alphanumerical?
template <typename String>
bool IsLetterOrDigit(const typename String::value_type& c)
{
    typedef typename String::value_type C;
    auto IsDigit = Bind2of3(IsInRange<C>, 48, 58);
    auto IsLowerLetter = Bind2of3(IsInRange<C>, 65, 91);
    auto IsUpperLetter = Bind2of3(IsInRange<C>, 97, 123);
    auto IsLetter = Or(IsLowerLetter, IsUpperLetter);
    return IsDigit(c) || IsLetter(c);
}

// Is character a whitespace.
template <typename String>
bool IsWhitespace(const typename String::value_type& c)
{
    typedef typename String::value_type C;
    return (c == 32 || IsInRange<C>(9, 14, c));
}

// Newline character ('\n')?
template <typename String>
bool IsLineBreak(const typename String::value_type& c)
{
    return c == '\n';
}

// Replaces windows and mac newlines with linux newlines.
template <typename String>
String CleanNewlines(const String& str)
{
    return ReplaceElems('\r', '\n',
        ReplaceTokens(String("\r\n"), String("\n"), str));
}

// Splits a string by the found whitespace characters.
// SplitWords("How are you?") == ["How", "are", "you?"]
template <typename String, typename ContainerOut = std::list<String>>
ContainerOut SplitWords(const String& str)
{
    return SplitBy(Not(IsLetterOrDigit<String>), false, str);
}

// Splits a string by the found newlines.
// SplitWords("Hi,\nhow are you?") == ["Hi,", "How are you?"]
template <typename String, typename ContainerOut = std::list<String>>
ContainerOut SplitLines(const String& str, bool allowEmpty)
{
    return SplitBy(IsLineBreak<String>, allowEmpty, CleanNewlines(str));
}

// TrimWhitespaceLeft("    text  ") == "text  "
template <typename String>
String TrimWhitespaceLeft(const String& str)
{
    return TrimLeft(IsWhitespace<String>, str);
}

// TrimWhitespaceLeft("    text  ") == "    text"
template <typename String>
String TrimWhitespaceRight(const String& str)
{
    return TrimRight(IsWhitespace<String>, str);
}

// TrimWhitespaceLeft("    text  ") == "text"
template <typename String>
String TrimWhitespace(const String& str)
{
    return Trim(IsWhitespace<String>, str);
}

} // namespace FunctionalPlus