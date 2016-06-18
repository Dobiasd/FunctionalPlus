// Copyright Tobias Hermann 2015.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "fplus/function_traits.h"

#include <functional>
#include <limits>
#include <cmath>

namespace fplus
{

// API search type: is_in_range : (a, a) -> (a -> Bool)
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

// API search type: clamp : (a, a) -> (a -> a)
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

// API search type: is_negative : a -> Bool
// Checks if x < 0.
template <typename X>
bool is_negative(X x)
{
    return x < 0;
}

// API search type: is_positive : a -> Bool
// Checks if x is not negative.
template <typename X>
bool is_positive(X x)
{
    return !is_negative(x);
}

// API search type: abs : a -> a
// Returns the absolute (always non-negative) value of x.
template <typename X>
X abs(X x)
{
    return is_negative(x) ? -x : x;
}

// API search type: sign : a -> Int
// Returns -1 for negative values, 1 otherwise.
// sign(-3) == -1
// sign(0) == 1
// sign(16) == 1
template <typename X>
int sign(X x)
{
    return is_negative(x) ? -1 : 1;
}

// API search type: sign_with_zero : a -> Int
// Returns -1 for negative values, 0 for zero, 1 for positive values.
// sign_with_zero(-3) == -1
// sign_with_zero(0) == 0
// sign_with_zero(16) == 1
template <typename X>
int sign_with_zero(X x)
{
    return x == 0 ? 0 : sign(x);
}

// API search type: round : a -> b
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

// API search type: floor : a -> b
// Converts a value to the nearest smaller integer.
template <typename Out, typename X>
Out floor(X x)
{
    static_assert(std::is_integral<Out>::value, "type must be integral");
    if (is_negative(x))
        x -= 1;
    return static_cast<Out>(x);
}

// API search type: ceil : a -> b
// Converts a value to the nearest greater integer.
template <typename Out, typename X>
Out ceil(X x)
{
    static_assert(std::is_integral<Out>::value, "type must be integral");
    return floor<Out>(x) + 1;
}

// API search type: int_power : (Int, Int) -> Int
// integer power
template <typename X>
X int_power(X base, X exp)
{
    static_assert(std::is_integral<X>::value,
        "type must be unsigned integral");
    assert(!is_negative(exp));
    if (exp == 0)
        return 1;
    if (exp == 1)
        return base;
    return base * int_power(base, exp - 1);
}

// API search type: min_2_on : (a -> b) -> ((a, a) -> a)
// minimum of two values after transformation
template <typename F,
    typename FIn = typename utils::function_traits<F>::template arg<0>::type>
std::function<const FIn&(const FIn& x, const FIn& y)>
        min_2_on(F f)
{
    return [f](const FIn& x, const FIn& y) -> const FIn&
    {
        return f(x) < f(y) ? x : y;
    };
}

// API search type: max_2_on : (a -> b) -> ((a, a) -> a)
// maximum of two values after transformation
template <typename F,
    typename FIn = typename utils::function_traits<F>::template arg<0>::type>
std::function<const FIn&(const FIn& x, const FIn& y)>
        max_2_on(F f)
{
    return [f](const FIn& x, const FIn& y) -> const FIn&
    {
        return f(x) > f(y) ? x : y;
    };
}

// API search type: min_3_on : (a -> b) -> ((a, a, a) -> a)
// minimum of three values after transformation
template <typename F,
    typename FIn = typename utils::function_traits<F>::template arg<0>::type>
std::function<const FIn&(const FIn& x, const FIn& y, const FIn& z)>
        min_3_on(F f)
{
    return [f](const FIn& x, const FIn& y, const FIn& z) -> const FIn&
    {
        return min_2_on(f)(min_2_on(f)(x, y), z);
    };
}

// API search type: max_3_on : (a -> b) -> ((a, a, a) -> a)
// maximum of three values after transformation
template <typename F,
    typename FIn = typename utils::function_traits<F>::template arg<0>::type>
std::function<const FIn&(const FIn& x, const FIn& y, const FIn& z)>
        max_3_on(F f)
{
    return [f](const FIn& x, const FIn& y, const FIn& z) -> const FIn&
    {
        return max_2_on(f)(max_2_on(f)(x, y), z);
    };
}

// API search type: min_2 : (a, a) -> a
// Minimum of two values.
template <typename X>
const X& min_2(const X& a, const X& b)
{
    return std::min(a, b);
}

// API search type: min_3 : (a, a, a) -> a
// Minimum of three values.
template <typename X>
const X& min_3(const X& a, const X& b, const X& c)
{
    return min_2(min_2(a, b), c);
}

// API search type: min_4 : (a, a, a, a) -> a
// Minimum of four values.
template <typename X>
const X& min_4(const X& a, const X& b, const X& c, const X& d)
{
    return min_2(min_3(a, b, c), d);
}

// API search type: min_5 : (a, a, a, a, a) -> a
// Minimum of five values.
template <typename X>
const X& min_5(const X& a, const X& b, const X& c, const X& d, const X& e)
{
    return min_3(min_3(a, b, c), d, e);
}

// API search type: min : (a, ...) -> a
// Minimum of x number of values
template <typename U, typename... V>
auto min(const U& u, const V&... v) -> typename std::common_type<U, V...>::type
{
  using rettype = typename std::common_type<U, V...>::type;
  rettype result = static_cast<rettype>(u);
  (void)std::initializer_list<int>{((v < result) ? (result = static_cast<rettype>(v), 0) : 0)...};
  return result;
}

// API search type: max_2 : (a, a) -> a
// Maximum of two values.
template <typename X>
const X& max_2(const X& a, const X& b)
{
    return std::max(a, b);
}

// API search type: max_3 : (a, a, a) -> a
// Maximum of three values.
template <typename X>
const X& max_3(const X& a, const X& b, const X& c)
{
    return max_2(max_2(a, b), c);
}

// API search type: max_4 : (a, a, a, a) -> a
// Maximum of four values.
template <typename X>
const X& max_4(const X& a, const X& b, const X& c, const X& d)
{
    return max_2(max_3(a, b, c), d);
}

// API search type: max_5 : (a, a, a, a, a) -> a
// Maximum of five values.
template <typename X>
const X& max_5(const X& a, const X& b, const X& c, const X& d, const X& e)
{
    return max_3(max_3(a, b, c), d, e);
}

// API search type: max : (a, ...) -> a
// Maximum of x number of values.
template <typename U, typename... V>
auto max(const U& u, const V&... v) -> typename std::common_type<U, V...>::type
{
  using rettype = typename std::common_type<U, V...>::type;
  rettype result = static_cast<rettype>(u);
  (void)std::initializer_list<int>{((v > result) ? (result = static_cast<rettype>(v), 0) : 0)...};
  return result;
}

// API search type: cyclic_value : Float -> (Float -> Float)
// Modulo for floating point values.
// Only positive denominators allowed;
// cyclic_value(8)(3) == 3
// cyclic_value(8)(11) == 3
// cyclic_value(8)(19) == 3
// cyclic_value(8)(-2) == 6
// cyclic_value(8)(-5) == 3
// cyclic_value(8)(-13) == 3
// Can be useful to normalize an angle into [0, 360]
// For positive values it behaves like std::fmod with flipped arguments.
template <typename X>
std::function<X(X)> cyclic_value(X circumfence)
{
    assert(circumfence > 0);
    return [circumfence](X x) -> X
    {
        if (sign(x) < 0)
            return circumfence -
                static_cast<X>(std::fmod(abs(x), abs(circumfence)));
        else
            return static_cast<X>(std::fmod(abs(x), abs(circumfence)));
    };
}

// API search type: cyclic_difference : Float -> ((Float, Float) -> Float)
// circumfence has to be positive.
// cyclic_difference(100)(5, 2) == 3
// cyclic_difference(100)(2, 5) == 97
// cyclic_difference(100)(3, -2) == 5
// cyclic_difference(100)(-2, 3) == 95
// cyclic_difference(100)(90, 10) == 80
// cyclic_difference(100)(10, 90) == 20
template <typename X>
std::function<X(X, X)> cyclic_difference(X circumfence)
{
    assert(circumfence >= 0);
    return [circumfence](X a, X b) -> X
    {
        auto cyclic_value_f = cyclic_value(circumfence);
        return cyclic_value_f(cyclic_value_f(a) - cyclic_value_f(b));
    };
}

// API search type: cyclic_shortest_difference : Float -> ((Float, Float) -> Float)
// circumfence has to be positive.
// cyclic_shortest_difference(100)(5, 2) == 3
// cyclic_shortest_difference(100)(2, 5) == -3
// cyclic_shortest_difference(100)(3, -2) == 5
// cyclic_shortest_difference(100)(-2, 3) == -5
// cyclic_shortest_difference(100)(90, 10) == -20
// cyclic_shortest_difference(100)(10, 90) == 20
template <typename X>
std::function<X(X, X)> cyclic_shortest_difference(X circumfence)
{
    assert(circumfence >= 0);
    return [circumfence](X a, X b) -> X
    {
        auto diff_func = cyclic_difference(circumfence);
        auto a_minus_b = diff_func(a, b);
        auto b_minus_a = diff_func(b, a);
        return a_minus_b <= b_minus_a ? a_minus_b : -b_minus_a;
    };
}

// API search type: cyclic_distance : Float -> ((Float, Float) -> Float)
// circumfence has to be positive.
// cyclic_distance(100)(2, 5) == 3
// cyclic_distance(100)(5, 2) == 3
// cyclic_distance(100)(-2, 3) == 5
// cyclic_distance(100)(3, -2) == 5
// cyclic_distance(100)(10, 90) == 20
// cyclic_distance(100)(90, 10) == 20
// Can be useful to calculate the difference of two angles;
template <typename X>
std::function<X(X, X)> cyclic_distance(X circumfence)
{
    assert(circumfence >= 0);
    return [circumfence](X a, X b) -> X
    {
        return abs(cyclic_shortest_difference(circumfence)(a, b));
    };
}

// API search type: pi : () -> Float
// Pi.
inline double pi()
{
    return 3.14159265358979323846;
}

// API search type: deg_to_rad : Float -> Float
// converts degrees to radians
template <typename T>
T deg_to_rad(T x)
{
    static_assert(std::is_floating_point<T>::value, "Please use a floating-point type.");
    return static_cast<T>(x * pi() / 180.0);
}

// API search type: rad_to_deg : Float -> Float
// converts radians to degrees
template <typename T>
T rad_to_deg(T x)
{
    return static_cast<T>(x * 180.0 / pi());
}

} // namespace fplus
