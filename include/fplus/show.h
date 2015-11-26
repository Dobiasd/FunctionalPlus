// Copyright Tobias Hermann 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "container_common.h"
#include "transform.h"

#include <sstream>
#include <string>

namespace fplus
{

// 42 -> "42"
template <typename T>
std::string Show(const T& x)
{
    std::ostringstream ss;
    ss << x;
    return ss.str();
}

// ShowContWith (" => ", [1, 2, 3], "{", "}") == "{1 => 2 => 3}"
template <typename Container>
std::string ShowContWithFrame(
    const std::string& separator,
    const std::string& prefix, const std::string& sufix,
    const Container& xs)
{
    typedef typename Container::value_type T;
    typedef std::vector<std::string> Strings;
    typedef decltype(Show<T>) ShowType;
    auto showT = Show<T>;
    auto mapped = Transform<ShowType, Container, Strings>(showT, xs);
    return prefix + Join(separator, mapped) + sufix;
}

// ShowContWith( " - ", [1, 2, 3]) == "[1 - 2 - 3]"
template <typename Container>
std::string ShowContWith(const std::string& separator, const Container& xs)
{
    return ShowContWithFrame(separator, "[", "]", xs);
}

// ShowCont [1, 2, 3] -> "[1, 2, 3]"
template <typename Container>
std::string ShowCont(const Container& xs)
{
    return ShowContWith(", ", xs);
}


} // namespace fplus