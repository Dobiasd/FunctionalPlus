// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <fplus/function_traits.hpp>
#include <fplus/composition.hpp>

#include <fplus/internal/invoke.hpp>
#include <fplus/internal/compare.hpp>
#include <fplus/internal/asserts/functions.hpp>

namespace fplus
{

namespace internal
{
    template <typename UnaryPredicate, typename T>
    void check_unary_predicate_for_type()
    {
        internal::trigger_static_asserts<internal::unary_function_tag, UnaryPredicate, T>();
        static_assert(std::is_convertible<
            internal::invoke_result_t<UnaryPredicate, T>, bool>::value,
            "Predicate must return bool.");
    }
    template <typename F, typename G, typename X, typename Y>
    void check_compare_preprocessors_for_types()
    {
        internal::trigger_static_asserts<internal::unary_function_tag, F, X>();
        internal::trigger_static_asserts<internal::unary_function_tag, G, Y>();
        static_assert(std::is_same<
            std::decay_t<internal::invoke_result_t<F, X>>,
            std::decay_t<internal::invoke_result_t<G, Y>>>::value,
            "Both functions must return the same type.");
    }
} // namespace internal

// API search type: identity : a -> a
// fwd bind count: 0
// identity(x) == x
template <typename T>
T identity(const T& x)
{
    return x;
}

// API search type: is_equal : (a, a) -> Bool
// fwd bind count: 1
// x == y
// Equality check.
template <typename T>
bool is_equal(const T& x, const T& y)
{
    return x == y;
}

// API search type: always : a -> (b -> a)
// always(x)(y) == x
template <typename X>
auto always(const X& x)
{
    return [x](const auto&) { return x; };
}

// API search type: always_arg_1_of_2 : (a, b) -> a
// always_arg_1_of_2(x, y) == x
template <typename X, typename Y>
X always_arg_1_of_2(const X& x, const Y&)
{
    return x;
}

// API search type: always_arg_2_of_2 : (a, b) -> a
// always_arg_2_of_2(x, y) == x
template <typename X, typename Y>
Y always_arg_2_of_2(const X&, const Y& y)
{
    return y;
}

// API search type: is_equal_by_and_by : ((a -> b), (c -> b)) -> ((a, c) -> Bool)
// f(x) == g(y)
// Provides an equality check of two values
// after applying a transformation function each.
template <typename F, typename G>
auto is_equal_by_and_by(F f, G g)
{
    return [f, g](const auto& x, const auto& y) {
        internal::trigger_static_asserts<internal::unary_function_tag,
                                             F,
                                             decltype(x)>();
        internal::trigger_static_asserts<internal::unary_function_tag,
                                             G,
                                             decltype(y)>();
        return is_equal(internal::invoke(f, x), internal::invoke(g, y));
    };
}

// API search type: is_equal_by : (a -> b) -> (a -> Bool)
// f(x) == f(y)
// Provides an equality check of two values
// after applying the same transformation function to both.
template <typename F>
auto is_equal_by(F f)
{
    return is_equal_by_and_by(f, f);
}

// API search type: is_equal_by_to : ((b -> a), a) -> (b -> Bool)
// f(y) == x
// Provides an equality check to a fixed value
// after applying a transformation function.
template <typename F, typename X>
auto is_equal_by_to(F f, const X& x)
{
    return [f, x](const auto& y)
    {
        internal::trigger_static_asserts<internal::unary_function_tag,
                                             F,
                                             decltype(y)>();
        return is_equal(internal::invoke(f, y), x);
    };
}

// API search type: is_equal_to : a -> (a -> Bool)
// x == y
// curried version of is_equal
// Provides an equality check with a fixed value.
template <typename X>
auto is_equal_to(const X& x)
{
    return is_equal_by_to(identity<X>, x);
}

// API search type: is_not_equal : (a, a) -> Bool
// fwd bind count: 1
// x != y
// Unequally check.
template <typename T>
bool is_not_equal(const T& x, const T& y)
{
    return x != y;
}

// API search type: is_not_equal_by_and_by : ((a -> c), (b -> c)) -> ((a, b) -> Bool)
// f(x) != g(y)
// Provides an unequality check of two values
// after applying a transformation function eac
template <typename F, typename G>
auto is_not_equal_by_and_by(F f, G g)
{
    return [f, g](const auto& x, const auto& y) {
        internal::trigger_static_asserts<internal::unary_function_tag,
                                             F,
                                             decltype(x)>();
        internal::trigger_static_asserts<internal::unary_function_tag,
                                             G,
                                             decltype(y)>();
        using FOut = std::decay_t<internal::invoke_result_t<F, decltype(x)>>;
        using GOut = std::decay_t<internal::invoke_result_t<G, decltype(y)>>;
        static_assert(std::is_same<FOut, GOut>::value,
                      "Functions must return the same type.");
        return is_not_equal(internal::invoke(f, x), internal::invoke(g, y));
    };
}

// API search type: is_not_equal_by : (a -> b) -> ((a, a) -> Bool)
// f(x) != f(y)
// Provides an unequality check of two values
// after applying the same transformation function to both.
template <typename F>
auto is_not_equal_by(F f)
{
    return is_not_equal_by_and_by(f, f);
}

// API search type: is_not_equal_by_to : ((a -> b), b) -> (a -> Bool)
// f(y) != x
// Provides an unequality check to a fixed value
// after applying a transformation function.
template <typename F, typename X>
auto is_not_equal_by_to(F f, const X& x)
{
    return [f, x](const auto& y) {
        internal::trigger_static_asserts<internal::unary_function_tag,
                                             F,
                                             decltype(y)>();
        return is_not_equal(internal::invoke(f, y), x);
    };
}

// API search type: is_not_equal_to : a -> (a -> Bool)
// y != x
// curried version of is_not_equal
// Provides an unequality check with a fixed value.
template <typename X>
auto is_not_equal_to(const X& x)
{
    return is_not_equal_by_to(identity<X>, x);
}

// API search type: is_less : (a, a) -> Bool
// fwd bind count: 1
// x < y
// Less check.
template <typename T>
bool is_less(const T& x, const T& y)
{
    return x < y;
}

// API search type: is_less_by_and_by : ((a -> c), (b -> c)) -> ((a, b) -> Bool)
// f(x) < g(y)
// Provides a less check of two values
// after applying a transformation function each.
template <typename F, typename G>
auto is_less_by_and_by(F f, G g)
{
    return [f, g](const auto& x, const auto& y)
    {
        internal::trigger_static_asserts<internal::unary_function_tag,
                                             F,
                                             decltype(x)>();
        internal::trigger_static_asserts<internal::unary_function_tag,
                                             G,
                                             decltype(y)>();
        using FOut = std::decay_t<internal::invoke_result_t<F, decltype(x)>>;
        using GOut = std::decay_t<internal::invoke_result_t<G, decltype(y)>>;
        static_assert(std::is_same<FOut, GOut>::value,
                      "Functions must return the same type.");
        return is_less(internal::invoke(f, x), internal::invoke(g, y));
    };
}

// API search type: is_less_by : (a -> b) -> ((a, a) -> Bool)
// f(x) < f(y)
// Provides a less check of two values
// after applying the same transformation function to both.
template <typename F>
auto is_less_by(F f)
{
    return is_less_by_and_by(f, f);
}

// API search type: is_less_by_than : ((a -> b), b) -> (a -> Bool)
// f(y) < x
// Provides a less check to a fixed value
// after applying a transformation function.
template <typename F, typename X>
auto is_less_by_than(F f, const X& x)
{
    return [f, x](const auto& y)
    {
        internal::trigger_static_asserts<internal::unary_function_tag,
                                             F,
                                             decltype(y)>();
        return is_less(internal::invoke(f, y), x);
    };
}

// API search type: is_less_than : a -> (a -> Bool)
// y < x
// curried version of is_less
// Provides a less check with a fixed value.
template <typename X>
auto is_less_than(const X& x)
{
    return is_less_by_than(identity<X>, x);
}

// API search type: is_less_or_equal : (a, a) -> Bool
// fwd bind count: 1
// x <= y
// Less-or-equal check.
template <typename T>
bool is_less_or_equal(const T& x, const T& y)
{
    return x <= y;
}

// API search type: is_less_or_equal_by_and_by : ((a -> c), (b -> c)) -> ((a, b) -> Bool)
// f(x) <= g(y)
// Provides a less-or-equal check of two values
// after applying a transformation function each.
template <typename F, typename G>
auto is_less_or_equal_by_and_by(F f, G g)
{
    return [f, g](const auto& x, const auto& y)
    {
        using FIn = decltype(x);
        using GIn = decltype(y);
        internal::check_compare_preprocessors_for_types<F, G, FIn, GIn>();
        return is_less_or_equal(internal::invoke(f, x), internal::invoke(g, y));
    };
}

// API search type: is_less_or_equal_by : (a -> b) -> ((a, a) -> Bool)
// f(x) <= f(y)
// Provides a less-or-equal check of two values
// after applying the same transformation function to both.
template <typename F>
auto is_less_or_equal_by(F f)
{
    return is_less_or_equal_by_and_by(f, f);
}

// API search type: is_less_or_equal_by_than : ((a -> b), b) -> (a -> Bool)
// f(y) <= x
// Provides a less-or-equal check to a fixed value
// after applying a transformation function.
template <typename F, typename X>
auto is_less_or_equal_by_than(F f, const X& x)
{
    return [f, x](const auto& y)
    {
        internal::
            trigger_static_asserts<internal::unary_function_tag, F, decltype(y)>();
        return is_less_or_equal(internal::invoke(f, y), x);
    };
}

// API search type: is_less_or_equal_than : a -> (a -> Bool)
// y <= x
// curried version of is_less_or_equal
// Provides a less-or-equal check with a fixed value
template <typename X>
auto is_less_or_equal_than(const X& x)
{
    return is_less_or_equal_by_than(identity<X>, x);
}

// API search type: is_greater : a -> a -> Bool
// fwd bind count: 1
// x > y
// Greater check.
template <typename T>
bool is_greater(const T& x, const T& y)
{
    return x > y;
}

// API search type: is_greater_by_and_by : ((a -> c), (b -> c)) -> ((a, b) -> Bool)
// f(x) > g(y)
// Provides a greater check of two values
// after applying a transformation function each.
template <typename F, typename G>
auto is_greater_by_and_by(F f, G g)
{
    return [f, g](const auto& x, const auto& y)
    {
        using FIn = decltype(x);
        using GIn = decltype(y);

        internal::check_compare_preprocessors_for_types<F, G, FIn, GIn>();
        return is_greater(internal::invoke(f, x), internal::invoke(g, y));
    };
}

// API search type: is_greater_by : (a -> b) -> ((a, a) -> Bool)
// f(x) > f(y)
// Provides a greater check of two values
// after applying the same transformation function to both.
template <typename F>
auto is_greater_by(F f)
{
    return is_greater_by_and_by(f, f);
}

// API search type: is_greater_by_than : ((a -> b), b) -> (a -> Bool)
// f(y) > x
// Provides a greater check to a fixed value
// after applying a transformation function.
template <typename F, typename X>
auto is_greater_by_than(F f, const X& x)
{
    return [f, x](const auto& y)
    {
        return is_greater(internal::invoke(f, y), x);
    };
}

// API search type: is_greater_than : a -> (a -> Bool)
// y > x
// curried version of is_greater
// Provides a greater check with a fixed value.
template <typename X>
auto is_greater_than(const X& x)
{
    return is_greater_by_than(identity<X>, x);
}

// API search type: is_greater_or_equal : (a, a) -> Bool
// fwd bind count: 1
// x >= y
// Greater-or-equal check.
template <typename T>
bool is_greater_or_equal(const T& x, const T& y)
{
    return x >= y;
}

// API search type: is_greater_or_equal_by_and_by : ((a -> c), (b -> c)) -> ((a, b) -> Bool)
// f(x) >= g(y)
// Provides a greater-or-equal check of two values
// after applying a transformation function each.
template <typename F, typename G>
auto is_greater_or_equal_by_and_by(F f, G g)
{
    return [f, g](const auto& x, const auto& y)
    {
        using FIn = decltype(x);
        using GIn = decltype(y);
        internal::check_compare_preprocessors_for_types<F, G, FIn, GIn>();
        return is_greater_or_equal(internal::invoke(f, x), internal::invoke(g, y));
    };
}

// API search type: is_greater_or_equal_by : (a -> b) -> ((a, a) -> Bool)
// f(x) >= f(y)
// Provides a greater-or-equal check of two values
// after applying the same transformation function to both.
template <typename F>
auto is_greater_or_equal_by(F f)
{
    return is_greater_or_equal_by_and_by(f, f);
}

// API search type: is_greater_or_equal_by_than : ((a -> b), b) -> (a -> Bool)
// f(y) >= x
// Provides a greater-or-equal check to a fixed value
// after applying a transformation function.
template <typename F, typename X>
auto is_greater_or_equal_by_than(F f, const X& x)
{
    return [f, x](const auto& y)
    {
        internal::trigger_static_asserts<internal::unary_function_tag, F, decltype(y)>();
        return is_greater_or_equal(internal::invoke(f, y), x);
    };
}

// API search type: is_greater_or_equal_than : a -> (a -> Bool)
// y >= x
// curried version of is_less_or_equal
// Provides a greater-or-equal check with a fixed valu
template <typename X>
auto is_greater_or_equal_than(const X& x)
{
    return is_greater_or_equal_by_than(identity<X>, x);
}

// API search type: xor_bools : (Bool, Bool) -> Bool
// fwd bind count: 1
// Exclusive or.
template <typename T>
bool xor_bools(const T& x, const T& y)
{
    static_assert(std::is_convertible<T, bool>::value,
        "Type must be convertible to bool.");
    return (x && !y) || (!x && y);
}

// API search type: ord_to_eq : ((a, a) -> Bool) -> ((a, a) -> Bool)
// ord_to_eq((<)) == (==)
// Takes a less-than function and converts it
// into an equality check function
// which considers two values as equal if none are lesser than the other one.
template <typename Compare>
auto ord_to_eq(Compare comp)
{
    return [comp](auto x, auto y)
    {
        static_assert(std::is_same<decltype(x), decltype(y)>::value,
            "Argument types must be the same");
        auto p = internal::ord_to_impl(comp)(x, y);
        return !p.first && !p.second;
    };
}

// API search type: ord_to_not_eq : ((a, a) -> Bool) -> ((a, a) -> Bool)
// ord_to_not_eq((<)) == (!=)
// Takes a less-than function and converts it
// into an inequality check function
// which considers to values as unequal if one is less than the other one.
template <typename Compare>
auto ord_to_not_eq(Compare comp)
{
    return logical_not(ord_to_eq(comp));
}

// API search type: ord_eq_to_eq : ((a, a) -> Bool) -> ((a, a) -> Bool)
// ord_eq_to_eq((<=)) == (==)
// ord_to_eq((<)) == (==)
// Takes a less-or-equal-than function and converts it
// into an equality check function
// which considers to values as equal if a <= b and b <= a.
template <typename Compare>
auto ord_eq_to_eq(Compare comp)
{
    return [comp](auto x, auto y)
    {
        static_assert(std::is_same<decltype(x), decltype(y)>::value,
            "Argument types must be the same");
        auto p = internal::ord_to_impl(comp)(x, y);
        return p.first && p.second;
    };
}

// API search type: ord_eq_to_not_eq : ((a, a) -> Bool) -> ((a, a) -> Bool)
// ord_eq_to_not_eq((<=)) == (!=)
// Takes a less-or-equal-than function and converts it
// into an inequality check function
// which considers to values as equal if not a <= b and not b <= a.
template <typename Compare>
auto ord_eq_to_not_eq(Compare comp)
{
  return logical_not(ord_eq_to_eq(comp));
}

} // namespace fplus
