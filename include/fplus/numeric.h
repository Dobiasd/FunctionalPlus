// Copyright Tobias Hermann 2015.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "function_traits.h"
#include <functional>
#include <limits>

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
template <typename Out, typename X>
Out round(X x)
{
    if (x < std::numeric_limits<Out>::lowest())
        return std::numeric_limits<Out>::lowest();
    if (x > std::numeric_limits<Out>::max())
        return std::numeric_limits<Out>::max();
    static_assert(std::is_integral<Out>::value, "type must be integral");
    if (is_negative(x))
        x -= 1;
    return static_cast<Out>(x + 0.5);
}

// Converts a value to the nearest smaller integer.
template <typename Out, typename X>
Out floor(X x)
{
    static_assert(std::is_integral<Out>::value, "type must be integral");
    if (is_negative(x))
        x -= 1;
    return static_cast<Out>(x);
}

// Converts a value to the nearest greater integer.
template <typename Out, typename X>
Out ceil(X x)
{
    static_assert(std::is_integral<Out>::value, "type must be integral");
    return floor<Out>(x) + 1;
}

template <typename X>
X int_power(X base, X exp)
{
    static_assert(std::is_integral<X>::value, "type must be unsigned integral");
    assert(!is_negative(exp));
    if (exp == 0)
        return 1;
    if (exp == 1)
        return base;
    return base * int_power(base, exp - 1);
}

template <typename F,
    typename FIn = typename utils::function_traits<F>::template arg<0>::type>
std::function<const FIn&(const FIn& x, const FIn& y)>
        min_2_by(F f)
{
    return [f](const FIn& x, const FIn& y) -> const FIn&
    {
        return f(x) < f(y) ? x : y;
    };
}

template <typename F,
    typename FIn = typename utils::function_traits<F>::template arg<0>::type>
std::function<const FIn&(const FIn& x, const FIn& y)>
        max_2_by(F f)
{
    return [f](const FIn& x, const FIn& y) -> const FIn&
    {
        return f(x) > f(y) ? x : y;
    };
}

template <typename X>
const X& min_2(const X& a, const X& b)
{
    return std::min(a, b);
}

template <typename X>
const X& min_3(const X& a, const X& b, const X& c)
{
    return min_2(min_2(a, b), c);
}

template <typename X>
const X& min_4(const X& a, const X& b, const X& c, const X& d)
{
    return min_2(min_3(a, b, c), d);
}

template <typename X>
const X& min_5(const X& a, const X& b, const X& c, const X& d, const X& e)
{
    return min_3(min_3(a, b, c), d, e);
}

template <typename X>
const X& max_2(const X& a, const X& b)
{
    return std::max(a, b);
}

template <typename X>
const X& max_3(const X& a, const X& b, const X& c)
{
    return max_2(max_2(a, b), c);
}

template <typename X>
const X& max_4(const X& a, const X& b, const X& c, const X& d)
{
    return max_2(max_3(a, b, c), d);
}

template <typename X>
const X& max_5(const X& a, const X& b, const X& c, const X& d, const X& e)
{
    return max_3(max_3(a, b, c), d, e);
}

} // namespace fplus
