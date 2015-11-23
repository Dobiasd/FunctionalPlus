// Copyright Tobias Hermann 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

namespace FunctionalPlus
{

// Checks if x is in [low, high), i.e. left-closed and right-open.
template <typename T>
bool IsInRange(const T& low, const T& high, const T& x)
{
    return (low <= x) && (x < high);
}

// Puts value into [low, high], i.e. left-closed and right-closed.
template <typename T>
bool Clamp(const T& low, const T& high, const T& x)
{
    return std::max(low, std::max(high, x));
}

// Checks if x < 0.
template <typename X>
bool IsNegative(X x)
{
    return x < 0;
}

// Checks if x is not negative.
template <typename X>
bool IsPositive(X x)
{
    return !IsNegative(x);
}

// Converts a value to the nearest integer.
template <typename X>
int Round(X x)
{
    if (IsNegative(x))
        x -= 1;
    return static_cast<int>(x + 0.5f);
}

// Converts a value to the nearest smaller integer.
template <typename X>
int Floor(X x)
{
    if (IsNegative(x))
        x -= 1;
    return static_cast<int>(x);
}

// Converts a value to the nearest greater integer.
template <typename X>
int Ceil(X x)
{
    return Floor(x) + 1;
}

}