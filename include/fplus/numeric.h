// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
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

// API search type: abs_diff : (a, a) -> a
// Returns the absolute difference of two values.
template <typename X>
X abs_diff(X a, X b)
{
    return abs(b - a);
}

// API search type: square : a -> a
// Returns the square (x*x) of a value x.
template <typename X>
X square(X x)
{
    return x * x;
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
    static_assert(std::is_integral<Out>::value, "type must be integral");
    if (static_cast<double>(x) < static_cast<double>(std::numeric_limits<Out>::lowest()))
        return std::numeric_limits<Out>::lowest();
    if (static_cast<double>(x) > static_cast<double>(std::numeric_limits<Out>::max()))
        return std::numeric_limits<Out>::max();
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

namespace internal
{
    // minimum of x values after transformation
    // (has an overload for non-POD types)
    // min_on(mod2, 4, 3) == 4
    // min_on(mod7, 3, 5, 7, 3) == 7
    template <typename F, typename FirstT, typename... FIn>
    auto helper_min_on(F f, const FirstT& first, const FIn&... v) ->
        typename std::common_type<FirstT, FIn...>::type
    {
      using rettype = typename std::common_type<FirstT, FIn...>::type;
      using f_rettype = decltype(f(first));

      rettype result = first;
      f_rettype result_trans = f(first);
      f_rettype v_trans;
      (void)std::initializer_list<int>{
          ((v_trans = f(v), v_trans < result_trans)
               ? (result = static_cast<rettype>(v), result_trans = v_trans, 0)
               : 0)...};
      return result;
    }

    template <typename F>
    struct helper_min_on_t
    {
        helper_min_on_t(F _f) : f(_f) {}
        template <typename T, typename... Ts>
        auto operator()(T&& x, Ts&&... xs) -> typename std::common_type<T, Ts...>::type
        {
            return helper_min_on(std::forward<F>(f), std::forward<T>(x), std::forward<Ts>(xs)...);
        }
    private:
        F f;
    };
}

// API search type: min_on : ((a -> b), a, a) -> a
// minimum of x values after transformation  (curried version)
// min_on(mod2)(4, 3) == 4
// min_on(mod7)(3, 5, 7, 3) == 7
template <typename F>
auto min_on(F f) -> internal::helper_min_on_t<F>
{
    return internal::helper_min_on_t<F>{f};
}

namespace internal
{
    // maximum of x values after transformation
    // (has an overload for non-POD types)
    // max_on(mod2, 4, 3) == 3
    // max_on(mod7, 3, 5, 7, 3) == 5
    template <typename F, typename FirstT, typename... FIn>
    auto helper_max_on(F f, const FirstT& first, const FIn&... v) ->
        typename std::common_type<FirstT, FIn...>::type
    {
      using rettype = typename std::common_type<FirstT, FIn...>::type;
      using f_rettype = decltype(f(first));

      rettype result = first;
      f_rettype result_trans = f(first);
      f_rettype v_trans;
      (void)std::initializer_list<int>{
          ((v_trans = f(v), v_trans > result_trans)
               ? (result = static_cast<rettype>(v), result_trans = v_trans, 0)
               : 0)...};
      return result;
    }

    template <typename F>
    struct helper_max_on_t
    {
        helper_max_on_t(F _f) : f(_f) {}
        template <typename T, typename... Ts>
        auto operator()(T&& x, Ts&&... xs) -> typename std::common_type<T, Ts...>::type
        {
            return helper_max_on(std::forward<F>(f), std::forward<T>(x), std::forward<Ts>(xs)...);
        }
    private:
        F f;
    };
}

// API search type: max_on : (a -> b) -> ((a, a) -> a)
// maximum of x values after transformation (curried version)
// (has an overload for non POD types)
// max_on(mod2)(4, 3) == 3
// max_on(mod7)(3, 5, 7, 3) == 5
template <typename F>
auto max_on(F f) -> internal::helper_max_on_t<F>
{
    return internal::helper_max_on_t<F>{f};
}

// API search type: min : (a, a) -> a
// Minimum of x number of values
// min(4, 3) == 3
// min(4, 3, 6, 2, 3) == 2
template <typename U, typename... V>
auto min(const U& u, const V&... v) -> typename std::common_type<U, V...>::type
{
  using rettype = typename std::common_type<U, V...>::type;
  rettype result = static_cast<rettype>(u);
  (void)std::initializer_list<int>{((v < result) ? (result = static_cast<rettype>(v), 0) : 0)...};
  return result;
}

// API search type: max : (a, a) -> a
// Maximum of x number of values.
// max(4, 3) == 4
// max(4, 3, 6, 2, 3) == 6
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
