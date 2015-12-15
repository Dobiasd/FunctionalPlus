// Copyright Tobias Hermann 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

namespace fplus
{

// Checks if x is in [low, high), i.e. left-closed and right-open.
template <typename T>
std::function<bool(const T&)>
        is_in_range(const T& low, const T& high)
{
    return [low, high](const T& x)
    {
        return (low <= x) && (x < high);
    };
}

// Puts value into [low, high], i.e. left-closed and right-closed.
template <typename T>
std::function<T(const T&)>
        clamp(const T& low, const T& high)
{
    return [low, high](const T& x)
    {
        return std::max(low, std::min(high, x));
    };
}

// Checks if x < 0.
template <typename X>
bool is_negative(X x)
{
    return x < 0;
}

// Checks if x is not negative.
template <typename X>
bool is_positive(X x)
{
    return !is_negative(x);
}

// Converts a value to the nearest integer.
template <typename X>
int round(X x)
{
    if (is_negative(x))
        x -= 1;
    return static_cast<int>(x + 0.5f);
}

// Converts a value to the nearest smaller integer.
template <typename X>
int floor(X x)
{
    if (is_negative(x))
        x -= 1;
    return static_cast<int>(x);
}

// Converts a value to the nearest greater integer.
template <typename X>
int ceil(X x)
{
    return floor(x) + 1;
}

}