// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <fplus/function_traits.hpp>
#include <fplus/container_common.hpp>
#include <fplus/pairs.hpp>

#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>
#include <type_traits>

namespace fplus
{

// API search type: is_in_interval : (a, a, a) -> Bool
// fwd bind count: 2
// Checks if x is in [low, high), i.e. left-closed and right-open.
template <typename T>
bool is_in_interval(const T& low, const T& high, const T& x)
{
    return (low <= x) && (x < high);
}

// API search type: is_in_interval_around : (a, a, a) -> Bool
// fwd bind count: 2
// Checks if x is in [center - radius, center + radius),
// i.e. left-closed and right-open.
template <typename T>
bool is_in_interval_around(const T& radius, const T& center, const T& x)
{
    return is_in_interval(center - radius, center + radius, x);
}

// API search type: is_in_open_interval : (a, a, a) -> Bool
// fwd bind count: 2
// Checks if x is in (low, high), i.e. left-open and right-open.
template <typename T>
bool is_in_open_interval(const T& low, const T& high, const T& x)
{
    return (low < x) && (x < high);
}

// API search type: is_in_open_interval_around : (a, a, a) -> Bool
// fwd bind count: 2
// Checks if x is in (center - radius, center + radius),
// i.e. left-open and right-open.
template <typename T>
bool is_in_open_interval_around(const T& radius, const T& center, const T& x)
{
    return is_in_open_interval(center - radius, center + radius, x);
}

// API search type: is_in_closed_interval : (a, a, a) -> Bool
// fwd bind count: 2
// Checks if x is in [low, high], i.e. left-closed and right-closed.
template <typename T>
bool is_in_closed_interval(const T& low, const T& high, const T& x)
{
    return (low <= x) && (x <= high);
}

// API search type: is_in_closed_interval_around : (a, a, a) -> Bool
// Checks if x is in [center - radius, center + radius],
// i.e. left-open and right-open.
template <typename T>
bool is_in_closed_interval_around(const T& radius, const T& center, const T& x)
{
    return is_in_closed_interval(center - radius, center + radius, x);
}

// API search type: clamp : (a, a, a) -> a
// fwd bind count: 2
// Puts value into [low, high], i.e. left-closed and right-closed.
template <typename T>
T clamp(const T& low, const T& high, const T& x)
{
    return std::max(low, std::min(high, x));
}

// API search type: is_negative : a -> Bool
// fwd bind count: 0
// Checks if x < 0.
template <typename X>
bool is_negative(X x)
{
    return x < 0;
}

// API search type: is_positive : a -> Bool
// fwd bind count: 0
// Checks if x is not negative.
template <typename X>
bool is_positive(X x)
{
    return !is_negative(x);
}

// API search type: is_even : Int -> Bool
// fwd bind count: 0
// Checks if x is even.
template <typename X>
bool is_even(X x)
{
    static_assert(std::is_integral<X>::value, "type must be integral");
    return x % 2 == 0;
}

// API search type: is_odd : Int -> Bool
// fwd bind count: 0
// Checks if x is odd.
template <typename X>
bool is_odd(X x)
{
    static_assert(std::is_integral<X>::value, "type must be integral");
    return x % 1 == 0;
}

namespace internal
{
    template <typename X>
    typename std::enable_if<std::is_unsigned<X>::value, X>::type
    abs_helper(X x)
    {
        return x;
    }

    template <typename X>
    typename std::enable_if<!std::is_unsigned<X>::value, X>::type
    abs_helper(X x)
    {
        return std::abs(x);
    }
}

// API search type: abs : a -> a
// fwd bind count: 0
// Returns the absolute (always non-negative) value of x.
template <typename X>
X abs(X x)
{
    return internal::abs_helper(x);
}

// API search type: abs_diff : (a, a) -> a
// fwd bind count: 1
// Returns the absolute difference of two values.
template <typename X>
X abs_diff(X a, X b)
{
    return a > b ? a - b : b - a;
}

// API search type: square : a -> a
// fwd bind count: 0
// Returns the square (x*x) of a value x.
template <typename X>
X square(X x)
{
    return x * x;
}

// API search type: cube : a -> a
// fwd bind count: 0
// Returns the cube (x*x*x) of a value x.
template <typename X>
X cube(X x)
{
    return x * x * x;
}

// API search type: sign : a -> Int
// fwd bind count: 0
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
// fwd bind count: 0
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
// fwd bind count: 0
// Converts a value to the nearest integer.
template <typename X, typename Out = int>
Out round(X x)
{
    static_assert(!std::is_integral<X>::value, "type must be non-integral");
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
// fwd bind count: 0
// Converts a value to the nearest smaller integer.
template <typename X, typename Out = int>
Out floor(X x)
{
    static_assert(!std::is_integral<X>::value, "type must be non-integral");
    static_assert(std::is_integral<Out>::value, "type must be integral");
    if (is_negative(x))
        x -= 1;
    return static_cast<Out>(x);
}

// API search type: floor_to_int_mult : (Int, Int) -> Int
// fwd bind count: 1
// Rounds an integer down to the nearest smaller or equal multiple of n.
// n may not be zero.
template <typename X>
X floor_to_int_mult(X n, X x)
{
    static_assert(std::is_integral<X>::value, "type must be integral");
    assert(n != 0);
    if (is_negative(n))
        n = abs(n);
    if (is_negative(x) && n != 1)
        x = static_cast<X>(x - 1);
    return static_cast<X>((x / n) * n);
}

// API search type: ceil_to_int_mult : (Int, Int) -> Int
// fwd bind count: 1
// Rounds an integer up to the nearest greater or equal multiple of n.
// n may not be zero.
template <typename X>
X ceil_to_int_mult(X n, X x)
{
    return floor_to_int_mult(n, static_cast<X>(x + abs(n) - 1));
}

// API search type: ceil : a -> b
// fwd bind count: 0
// Converts a value to the nearest greater integer.
template <typename X, typename Out = int>
Out ceil(X x)
{
    static_assert(!std::is_integral<X>::value, "type must be non-integral");
    static_assert(std::is_integral<Out>::value, "type must be integral");
    return floor<X, Out>(x) + 1;
}

// API search type: int_power : (Int, Int) -> Int
// fwd bind count: 1
// integer power, only exponents >= 0
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
      using f_rettype = typename std::result_of<F(decltype(first))>::type;

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
// minimum of x values after transformation (curried version)
// min_on(mod2)(4, 3) == 4
// min_on(mod7)(3, 5, 7, 3) == 7
template <typename F>
auto min_on(F f) -> internal::helper_min_on_t<F>
{
    return internal::helper_min_on_t<F>{f};
}

// API search type: min_2_on : ((a -> b), a, a) -> a
// fwd bind count: 2
// minimum of 2 values after transformation
// min_2_on(mod2, 4, 3) == 4
template <typename F, typename T>
T min_2_on(F f, const T& x, const T& y)
{
    return f(y) < f(x) ? y : x;
}

namespace internal
{
    // Marks a variable as unused. Prevents the compiler warning
    // for set but unused variables.
    template<class T>
    inline void unused(T&&) { }


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
      unused(result_trans);
      unused(v_trans);

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

// API search type: max_2_on : ((a -> b), a, a) -> a
// fwd bind count: 2
// maximum of 2 values after transformation
// max_2_on(mod2, 4, 3) == 3
template <typename F, typename T>
T max_2_on(F f, const T& x, const T& y)
{
    return f(y) > f(x) ? y : x;
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

// API search type: min_2 : (a, a) -> a
// fwd bind count: 1
// minimum of 2 values
// min_2(4, 3) == 3
template <typename T>
T min_2(const T& x, const T& y)
{
    return y < x ? y : x;
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

// API search type: max_2 : (a, a) -> a
// fwd bind count: 1
// maximum of 2 values
// max_2(4, 3) == 4
template <typename T>
T max_2(const T& x, const T& y)
{
    return y > x ? y : x;
}

namespace internal
{
    template <typename X>
    typename std::enable_if<std::is_floating_point<X>::value, X>::type
    cyclic_value_helper_mod(X x, X y)
    {
        return std::fmod(x, y);
    }

    template <typename X>
    typename std::enable_if<std::is_integral<X>::value, X>::type
    cyclic_value_helper_mod(X x, X y)
    {
        return x % y;
    }
}

// API search type: cyclic_value : a -> (a -> a)
// Modulo for floating point values.
// circumfence must be > 0
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
            return circumfence - internal::cyclic_value_helper_mod(
                abs(x), abs(circumfence));
        else
            return internal::cyclic_value_helper_mod(
                abs(x), abs(circumfence));
    };
}

// API search type: cyclic_difference : a -> ((a, a) -> a)
// Returns the distance the first value has to advance forward on a circle
// to reach the second value.
// circumfence must be > 0
// cyclic_difference(100)(5, 2) == 3
// cyclic_difference(100)(2, 5) == 97
// cyclic_difference(100)(3, -2) == 5
// cyclic_difference(100)(-2, 3) == 95
// cyclic_difference(100)(90, 10) == 80
// cyclic_difference(100)(10, 90) == 20
template <typename X>
std::function<X(X, X)> cyclic_difference(X circumfence)
{
    assert(circumfence > 0);
    return [circumfence](X a, X b) -> X
    {
        auto cyclic_value_f = cyclic_value(circumfence);
        const auto c_v_a = cyclic_value_f(a);
        const auto c_v_b = cyclic_value_f(b);
        return c_v_a > c_v_b ?
            c_v_a - c_v_b :
            circumfence + c_v_a - c_v_b;
    };
}

// API search type: cyclic_shortest_difference : a -> ((a, a) -> a)
// Returns displacement (shortest way) the first value has to move on a circle
// to reach the second value.
// circumfence must be > 0
// cyclic_shortest_difference(100)(5, 2) == 3
// cyclic_shortest_difference(100)(2, 5) == -3
// cyclic_shortest_difference(100)(3, -2) == 5
// cyclic_shortest_difference(100)(-2, 3) == -5
// cyclic_shortest_difference(100)(90, 10) == -20
// cyclic_shortest_difference(100)(10, 90) == 20
template <typename X>
std::function<X(X, X)> cyclic_shortest_difference(X circumfence)
{
    assert(circumfence > 0);
    return [circumfence](X a, X b) -> X
    {
        auto diff_func = cyclic_difference(circumfence);
        auto a_minus_b = diff_func(a, b);
        auto b_minus_a = diff_func(b, a);
        return a_minus_b <= b_minus_a ? a_minus_b : -b_minus_a;
    };
}

// API search type: cyclic_distance : a -> ((a, a) -> a)
// Returns distance (shortest way) the first value has to move on a circle
// to reach the second value.
// circumfence must be > 0
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
    assert(circumfence > 0);
    return [circumfence](X a, X b) -> X
    {
        auto diff_func = cyclic_difference(circumfence);
        auto a_minus_b = diff_func(a, b);
        auto b_minus_a = diff_func(b, a);
        return a_minus_b <= b_minus_a ? a_minus_b : b_minus_a;
    };
}

// API search type: pi : () -> Float
// Pi.
constexpr inline double pi()
{
    return 3.14159265358979323846;
}

// API search type: deg_to_rad : Float -> Float
// fwd bind count: 0
// converts degrees to radians
template <typename T>
T deg_to_rad(T x)
{
    static_assert(std::is_floating_point<T>::value, "Please use a floating-point type.");
    return static_cast<T>(x * pi() / 180.0);
}

// API search type: rad_to_deg : Float -> Float
// fwd bind count: 0
// converts radians to degrees
template <typename T>
T rad_to_deg(T x)
{
    static_assert(std::is_floating_point<T>::value, "Please use a floating-point type.");
    return static_cast<T>(x * 180.0 / pi());
}

// API search type: normalize_min_max : (a, a, [a]) -> [a]
// fwd bind count: 2
// Linearly scales the values into the given interval.
// normalize_min_max(0, 10, [1, 3, 6]) == [0, 4, 10]
// It is recommended to convert integers to double beforehand.
template <typename Container>
Container normalize_min_max(
    const typename Container::value_type& lower,
    const typename Container::value_type& upper, const Container& xs)
{
    assert(size_of_cont(xs) != 0);
    assert(lower <= upper);
    typedef typename Container::value_type T;
    const auto minmax_it_p = std::minmax_element(std::begin(xs), std::end(xs));
    const T x_min = *minmax_it_p.first;
    const T x_max = *minmax_it_p.second;
    const auto f = [&](const T& x) -> T
    {
        return lower + (upper - lower) * (x - x_min) / (x_max - x_min);
    };
    return fplus::transform(f, xs);
}

// API search type: normalize_mean_stddev : (a, a, [a]) -> [a]
// fwd bind count: 2
// Linearly scales the values
// to match the given mean and population standard deviation.
// normalize_mean_stddev(3, 2, [7, 8]) == [1, 5]
template <typename Container>
Container normalize_mean_stddev(
    const typename Container::value_type& mean,
    const typename Container::value_type& stddev,
    const Container& xs)
{
    assert(size_of_cont(xs) != 0);
    typedef typename Container::value_type T;
    const auto mean_and_stddev = fplus::mean_stddev<T>(xs);
    const auto f = [&](const T& x) -> T
    {
        return mean +
            stddev * (x - mean_and_stddev.first) / mean_and_stddev.second;
    };
    return fplus::transform(f, xs);
}

// API search type: standardize : [a] -> [a]
// fwd bind count: 0
// Linearly scales the values to zero mean and population standard deviation 1.
// standardize([7, 8]) == [-1, 1]
template <typename Container>
Container standardize(const Container& xs)
{
    return normalize_mean_stddev(0, 1, xs);
}

// API search type: add_to : a -> (a -> a)
// Provide a function adding to a given constant.
// add_to(3)(2) == 5
template <typename X>
std::function<X(X)> add_to(const X& x)
{
    return [x](X y) -> X
    {
        return x + y;
    };
}

// API search type: subtract_from : a -> (a -> a)
// Provide a function subtracting from a given constant.
// add_to(3)(2) == 1
template <typename X>
std::function<X(X)> subtract_from(const X& x)
{
    return [x](X y) -> X
    {
        return x - y;
    };
}

// API search type: multiply_with : a -> (a -> a)
// Provide a function multiplying with a given constant.
// multiply_with(3)(2) == 6
template <typename X>
std::function<X(X)> multiply_with(const X& x)
{
    return [x](X y) -> X
    {
        return y * x;
    };
}

// API search type: divide_by : a -> (a -> a)
// Provide a function dividing by a given constant.
// divide_by(2)(6) == 3
template <typename X>
std::function<X(X)> divide_by(const X& x)
{
    return [x](X y) -> X
    {
        return y / x;
    };
}

// API search type: histogram_using_intervals : ([(a, a)], [a]) -> [((a, a), Int)]
// fwd bind count: 1
// Generate a histogram of a sequence with given bins.
// histogram_using_intervals([(0,4), (4,5), (6,8)], [0,1,4,5,6,7,8,9]) ==
//     [((0, 4), 2), ((4, 5), 1), ((6, 8), 2)]
template <typename ContainerIn,
        typename ContainerIntervals,
        typename ContainerOut =
            std::vector<
                std::pair<
                    typename ContainerIntervals::value_type,
                    std::size_t>>,
        typename T = typename ContainerIn::value_type>
ContainerOut histogram_using_intervals(
        const ContainerIntervals& intervals, const ContainerIn& xs)
{
    ContainerOut bins;
    internal::prepare_container(bins, size_of_cont(intervals));
    auto itOut = internal::get_back_inserter(bins);
    for (const auto& interval : intervals)
    {
        *itOut = std::make_pair(interval, 0);
    }
    for (const auto& x : xs)
    {
        for (auto& bin : bins)
        {
            if (x >= bin.first.first && x < bin.first.second)
            {
                ++bin.second;
            }
        }
    }
    return bins;
}

// API search type: generate_consecutive_intervals : (a, a, a) -> [(a, a)]
// fwd bind count: 2
// Return intervals of a given size adjacent to each other
// generate_consecutive_intervals(0, 2, 4) == [(0,2), (2,4), (4,6), (6,8)]
template <typename T>
std::vector<std::pair<T, T>> generate_consecutive_intervals(
        const T& first_lower_bound, const T& step, std::size_t count)
{
    const auto count_as_T = static_cast<T>(count);
    return zip(
        numbers_step<T>(
            first_lower_bound,
            first_lower_bound + count_as_T * step,
            step),
        numbers_step<T>(
            first_lower_bound + step,
            first_lower_bound + step + count_as_T * step,
            step));
}

// API search type: histogram : (a, a, a, [a]) -> [((a, a), Int)]
// fwd bind count: 3
// Calculate the histogram of a sequence using a given bin width.
// histogram(1, 2, 4, [0,1,4,5,7,8,9]) == [(1, 2), (3, 0), (5, 2), (7, 1)]
template <typename ContainerIn,
        typename ContainerOut =
            std::vector<
                std::pair<
                    typename ContainerIn::value_type,
                    std::size_t>>,
        typename T = typename ContainerIn::value_type>
ContainerOut histogram(
        const T& first_center, const T& bin_width, std::size_t count,
        const ContainerIn& xs)
{
    const auto interval_histogram = histogram_using_intervals(
        generate_consecutive_intervals(
            first_center - bin_width / 2,
            bin_width,
            count),
        xs);

    assert(size_of_cont(interval_histogram) == count);

    ContainerOut histo;
    internal::prepare_container(histo, count);
    auto itOut = internal::get_back_inserter(histo);
    for (const auto& bin : interval_histogram)
    {
        const auto current_center = (bin.first.first + bin.first.second) / 2;
        *itOut = std::make_pair(current_center, bin.second);
    }
    return histo;
}

// API search type: modulo_chain : ([Int], Int) -> [Int]
// fwd bind count: 1
// For every factor (value % factor) is pushed into the result,
// and value is divided by this factor for the next iteration.
// Can be useful to convert a time in seconds
// into hours, minutes and seconds and similar calculations.
// modulo_chain([24, 60, 60], 7223) == [0, 2, 0, 23]
template <typename T>
std::vector<T> modulo_chain(const std::vector<T>& factors, T val)
{
    std::vector<T> result;
    result.reserve(factors.size());
    const auto factors_reversed = reverse(factors);
    for (const auto& factor : factors_reversed)
    {
        result.push_back(val % factor);
        val /= factor;
    }
    result.push_back(val);
    return reverse(result);
}

} // namespace fplus
