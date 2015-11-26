// Copyright Tobias Hermann 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "function_traits.h"

namespace fplus
{

template <typename UnaryPredicate, typename T>
void check_unary_predicate_for_type()
{
    static_assert(utils::function_traits<UnaryPredicate>::arity == 1, "Wrong arity.");
    static_assert(std::is_convertible<T, typename utils::function_traits<UnaryPredicate>::template arg<0>::type>::value, "Unary predicate can not take these values.");
    static_assert(std::is_convertible<typename utils::function_traits<UnaryPredicate>::result_type, bool>::value, "Predicate must return bool.");
}

template <typename F, typename T>
void check_index_with_type_predicate_for_type()
{
    static_assert(utils::function_traits<F>::arity == 2, "Wrong arity.");
    typedef typename utils::function_traits<F>::template arg<0>::type FIn0;
    typedef typename utils::function_traits<F>::template arg<1>::type FIn1;
    static_assert(std::is_same<FIn0, std::size_t>::value, "First parameter of function must be std::size_t.");
    static_assert(std::is_convertible<typename utils::function_traits<F>::result_type, bool>::value, "Function must return bool.");
    static_assert(std::is_convertible<T, FIn1>::value, "Function does not work with elements of Container.");
}

template <typename BinaryPredicate, typename T>
void check_binary_predicate_for_type()
{
    static_assert(utils::function_traits<BinaryPredicate>::arity == 2, "Wrong arity.");
    typedef typename utils::function_traits<BinaryPredicate>::template arg<0>::type FIn;
    typedef typename utils::function_traits<BinaryPredicate>::template arg<1>::type FIn1;
    static_assert(std::is_same<FIn, FIn1>::value, "BinaryPredicate must take two similar types");
    static_assert(std::is_convertible<typename utils::function_traits<BinaryPredicate>::result_type, bool>::value, "BinaryPredicate must return bool.");
    static_assert(std::is_convertible<T, FIn>::value, "BinaryPredicate does not work with elements of Container.");
}

template <typename Compare, typename T>
void check_compare_for_type()
{
    static_assert(utils::function_traits<Compare>::arity == 2, "Wrong arity.");
    typedef typename utils::function_traits<Compare>::template arg<0>::type FIn;
    typedef typename utils::function_traits<Compare>::template arg<1>::type FIn1;
    static_assert(std::is_same<FIn, FIn1>::value, "Compare must take two similar types");
    static_assert(std::is_convertible<typename utils::function_traits<Compare>::result_type, bool>::value, "Compare must return bool.");
    static_assert(std::is_convertible<T, FIn>::value, "Compare does not work with elements of Container.");
}

template <typename F, typename G, typename X, typename Y>
void check_compare_preprocessors_for_types()
{
    static_assert(utils::function_traits<F>::arity == 1, "Wrong arity.");
    static_assert(utils::function_traits<G>::arity == 1, "Wrong arity.");
    static_assert(std::is_convertible<X, typename utils::function_traits<F>::template arg<0>::type>::value, "Function can note take elements of this type.");
    static_assert(std::is_convertible<Y, typename utils::function_traits<G>::template arg<0>::type>::value, "Function can note take elements of this type.");
    static_assert(std::is_same<typename utils::function_traits<F>::result_type, typename utils::function_traits<G>::result_type>::value, "Both functions must return same type.");
}

// identity(x) == x
template <typename T>
const T identity(const T& x)
{
    return x;
}

// always(x, y) == x
template <typename X, typename Y>
const X always(const X& x, const Y&)
{
    return x;
}

// f(x) == g(y)
template <typename F, typename G, typename X, typename Y>
bool is_equal_by_and_by(F f, G g, const X& x, const Y& y)
{
    check_compare_preprocessors_for_types<F, G, X, Y>();
    return f(x) == g(y);
}

// f(x) == f(y)
template <typename F, typename T>
bool is_equal_by(F f, const T& x, const T& y)
{
    static_assert(utils::function_traits<F>::arity == 1, "Wrong arity.");
    return is_equal_by_and_by(f, f, x, y);
}
// x == y
template <typename T>
bool is_equal(const T& x, const T& y)
{
    return is_equal_by(identity<T>, x, y);
}

// f(x) != g(y)
template <typename F, typename G, typename X, typename Y>
bool is_not_equal_by_and_by(F f, G g, const X& x, const Y& y)
{
    check_compare_preprocessors_for_types<F, G, X, Y>();
    return f(x) != g(y);
}

// f(x) != f(y)
template <typename F, typename T>
bool is_not_equal_by(F f, const T& x, const T& y)
{
    static_assert(utils::function_traits<F>::arity == 1, "Wrong arity.");
    return is_not_equal_by_and_by(f, f, x, y);
}

// x != y
template <typename T>
bool is_not_equal(const T& x, const T& y)
{
    return is_not_equal_by(identity<T>, x, y);
}

// f(x) < g(y)
template <typename F, typename G, typename X, typename Y>
bool is_less_by_and_by(F f, G g, const X& x, const Y& y)
{
    check_compare_preprocessors_for_types<F, G, X, Y>();
    return f(x) < g(y);
}

// f(x) < f(y)
template <typename F, typename T>
bool is_less_by(F f, const T& x, const T& y)
{
    static_assert(utils::function_traits<F>::arity == 1, "Wrong arity.");
    return is_less_by_and_by(f, f, x, y);
}

// x < y
template <typename T>
bool is_less(const T& x, const T& y)
{
    return is_less_by(identity<T>, x, y);
}

// f(x) <= g(y)
template <typename F, typename G, typename X, typename Y>
bool is_less_or_equal_by_and_by(F f, G g, const X& x, const Y& y)
{
    check_compare_preprocessors_for_types<F, G, X, Y>();
    return f(x) <= g(y);
}

// f(x) <= f(y)
template <typename F, typename T>
bool is_less_or_equal_by(F f, const T& x, const T& y)
{
    static_assert(utils::function_traits<F>::arity == 1, "Wrong arity.");
    return is_less_or_equal_by_and_by(f, f, x, y);
}

// x <= y
template <typename T>
bool is_less_or_equal(const T& x, const T& y)
{
    return is_less_or_equal_by(identity<T>, x, y);
}

// f(x) > g(y)
template <typename F, typename G, typename X, typename Y>
bool is_greater_by_and_by(F f, G g, const X& x, const Y& y)
{
    check_compare_preprocessors_for_types<F, G, X, Y>();
    return f(x) > g(y);
}

// f(x) > f(y)
template <typename F, typename T>
bool is_greater_by(F f, const T& x, const T& y)
{
    static_assert(utils::function_traits<F>::arity == 1, "Wrong arity.");
    return is_greater_by_and_by(f, f, x, y);
}

// x > y
template <typename T>
bool is_greater(const T& x, const T& y)
{
    return is_greater_by(identity<T>, x, y);
}

// f(x) >= g(y)
template <typename F, typename G, typename X, typename Y>
bool is_greater_or_equal_by_and_by(F f, G g, const X& x, const Y& y)
{
    check_compare_preprocessors_for_types<F, G, X, Y>();
    return f(x) >= g(y);
}

// f(x) >= f(y)
template <typename F, typename T>
bool is_greater_or_equal_by(F f, const T& x, const T& y)
{
    static_assert(utils::function_traits<F>::arity == 1, "Wrong arity.");
    return is_greater_or_equal_by_and_by(f, f, x, y);
}

// x >= y
template <typename T>
bool is_greater_or_equal(const T& x, const T& y)
{
    return is_greater_or_equal_by(identity<T>, x, y);
}

} // namespace fplus