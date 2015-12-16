// Copyright Tobias Hermann 2015.
// https://github.com/Dobiasd/FunctionalPlus
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
std::string show(const T& x)
{
    std::ostringstream ss;
    ss << x;
    return ss.str();
}

// show_cont_with_frame (" => ", [1, 2, 3], "{", "}") == "{1 => 2 => 3}"
template <typename Container>
std::string show_cont_with_frame(
    const std::string& separator,
    const std::string& prefix, const std::string& sufix,
    const Container& xs)
{
    typedef typename Container::value_type T;
    typedef std::vector<std::string> Strings;
    typedef decltype(show<T>) ShowType;
    auto showT = show<T>;
    auto mapped = transform<ShowType, Container, Strings>(showT, xs);
    return prefix + join(separator, mapped) + sufix;
}

// show_cont_with( " - ", [1, 2, 3]) == "[1 - 2 - 3]"
template <typename Container>
std::string show_cont_with(const std::string& separator, const Container& xs)
{
    return show_cont_with_frame(separator, "[", "]", xs);
}

// show_cont [1, 2, 3] -> "[1, 2, 3]"
template <typename Container>
std::string show_cont(const Container& xs)
{
    return show_cont_with(", ", xs);
}


} // namespace fplus