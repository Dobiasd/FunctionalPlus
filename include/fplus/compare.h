// Copyright Tobias Hermann 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "function_traits.h"

namespace fplus
{

template <typename UnaryPredicate, typename T>
void CheckUnaryPredicateForType()
{
    static_assert(utils::function_traits<UnaryPredicate>::arity == 1, "Wrong arity.");
    static_assert(std::is_convertible<T, typename utils::function_traits<UnaryPredicate>::template arg<0>::type>::value, "Unary predicate can not take these values.");
    static_assert(std::is_convertible<typename utils::function_traits<UnaryPredicate>::result_type, bool>::value, "Predicate must return bool.");
}

template <typename F, typename T>
void CheckIndexWithTypePredicateForType()
{
    static_assert(utils::function_traits<F>::arity == 2, "Wrong arity.");
    typedef typename utils::function_traits<F>::template arg<0>::type FIn0;
    typedef typename utils::function_traits<F>::template arg<1>::type FIn1;
    static_assert(std::is_same<FIn0, std::size_t>::value, "First parameter of function must be std::size_t.");
    static_assert(std::is_convertible<typename utils::function_traits<F>::result_type, bool>::value, "Function must return bool.");
    static_assert(std::is_convertible<T, FIn1>::value, "Function does not work with elements of Container.");
}

template <typename BinaryPredicate, typename T>
void CheckBinaryPredicateForType()
{
    static_assert(utils::function_traits<BinaryPredicate>::arity == 2, "Wrong arity.");
    typedef typename utils::function_traits<BinaryPredicate>::template arg<0>::type FIn;
    typedef typename utils::function_traits<BinaryPredicate>::template arg<1>::type FIn1;
    static_assert(std::is_same<FIn, FIn1>::value, "BinaryPredicate must take two similar types");
    static_assert(std::is_convertible<typename utils::function_traits<BinaryPredicate>::result_type, bool>::value, "BinaryPredicate must return bool.");
    static_assert(std::is_convertible<T, FIn>::value, "BinaryPredicate does not work with elements of Container.");
}

template <typename Compare, typename T>
void CheckCompareForType()
{
    static_assert(utils::function_traits<Compare>::arity == 2, "Wrong arity.");
    typedef typename utils::function_traits<Compare>::template arg<0>::type FIn;
    typedef typename utils::function_traits<Compare>::template arg<1>::type FIn1;
    static_assert(std::is_same<FIn, FIn1>::value, "Compare must take two similar types");
    static_assert(std::is_convertible<typename utils::function_traits<Compare>::result_type, bool>::value, "Compare must return bool.");
    static_assert(std::is_convertible<T, FIn>::value, "Compare does not work with elements of Container.");
}

template <typename F, typename G, typename X, typename Y>
void CheckComparePreprocessorsForTypes()
{
    static_assert(utils::function_traits<F>::arity == 1, "Wrong arity.");
    static_assert(utils::function_traits<G>::arity == 1, "Wrong arity.");
    static_assert(std::is_convertible<X, typename utils::function_traits<F>::template arg<0>::type>::value, "Function can note take elements of this type.");
    static_assert(std::is_convertible<Y, typename utils::function_traits<G>::template arg<0>::type>::value, "Function can note take elements of this type.");
    static_assert(std::is_same<typename utils::function_traits<F>::result_type, typename utils::function_traits<G>::result_type>::value, "Both functions must return same type.");
}

// Identity(x) == x
template <typename T>
const T Identity(const T& x)
{
    return x;
}

// Always(x, y) == x
template <typename X, typename Y>
const X Always(const X& x, const Y&)
{
    return x;
}

// f(x) == g(y)
template <typename F, typename G, typename X, typename Y>
bool IsEqualByAndBy(F f, G g, const X& x, const Y& y)
{
    CheckComparePreprocessorsForTypes<F, G, X, Y>();
    return f(x) == g(y);
}

// f(x) == f(y)
template <typename F, typename T>
bool IsEqualBy(F f, const T& x, const T& y)
{
    static_assert(utils::function_traits<F>::arity == 1, "Wrong arity.");
    return IsEqualByAndBy(f, f, x, y);
}
// x == y
template <typename T>
bool IsEqual(const T& x, const T& y)
{
    return IsEqualBy(Identity<T>, x, y);
}

// f(x) != g(y)
template <typename F, typename G, typename X, typename Y>
bool IsNotEqualByAndBy(F f, G g, const X& x, const Y& y)
{
    CheckComparePreprocessorsForTypes<F, G, X, Y>();
    return f(x) != g(y);
}

// f(x) != f(y)
template <typename F, typename T>
bool IsNotEqualBy(F f, const T& x, const T& y)
{
    static_assert(utils::function_traits<F>::arity == 1, "Wrong arity.");
    return IsNotEqualByAndBy(f, f, x, y);
}

// x != y
template <typename T>
bool IsNotEqual(const T& x, const T& y)
{
    return IsNotEqualBy(Identity<T>, x, y);
}

// f(x) < g(y)
template <typename F, typename G, typename X, typename Y>
bool IsLessByAndBy(F f, G g, const X& x, const Y& y)
{
    CheckComparePreprocessorsForTypes<F, G, X, Y>();
    return f(x) < g(y);
}

// f(x) < f(y)
template <typename F, typename T>
bool IsLessBy(F f, const T& x, const T& y)
{
    static_assert(utils::function_traits<F>::arity == 1, "Wrong arity.");
    return IsLessByAndBy(f, f, x, y);
}

// x < y
template <typename T>
bool IsLess(const T& x, const T& y)
{
    return IsLessBy(Identity<T>, x, y);
}

// f(x) <= g(y)
template <typename F, typename G, typename X, typename Y>
bool IsLessOrEqualByAndBy(F f, G g, const X& x, const Y& y)
{
    CheckComparePreprocessorsForTypes<F, G, X, Y>();
    return f(x) <= g(y);
}

// f(x) <= f(y)
template <typename F, typename T>
bool IsLessOrEqualBy(F f, const T& x, const T& y)
{
    static_assert(utils::function_traits<F>::arity == 1, "Wrong arity.");
    return IsLessOrEqualByAndBy(f, f, x, y);
}

// x <= y
template <typename T>
bool IsLessOrEqual(const T& x, const T& y)
{
    return IsLessOrEqualBy(Identity<T>, x, y);
}

// f(x) > g(y)
template <typename F, typename G, typename X, typename Y>
bool IsGreaterByAndBy(F f, G g, const X& x, const Y& y)
{
    CheckComparePreprocessorsForTypes<F, G, X, Y>();
    return f(x) > g(y);
}

// f(x) > f(y)
template <typename F, typename T>
bool IsGreaterBy(F f, const T& x, const T& y)
{
    static_assert(utils::function_traits<F>::arity == 1, "Wrong arity.");
    return IsGreaterByAndBy(f, f, x, y);
}

// x > y
template <typename T>
bool IsGreater(const T& x, const T& y)
{
    return IsGreaterBy(Identity<T>, x, y);
}

// f(x) >= g(y)
template <typename F, typename G, typename X, typename Y>
bool IsGreaterOrEqualByAndBy(F f, G g, const X& x, const Y& y)
{
    CheckComparePreprocessorsForTypes<F, G, X, Y>();
    return f(x) >= g(y);
}

// f(x) >= f(y)
template <typename F, typename T>
bool IsGreaterOrEqualBy(F f, const T& x, const T& y)
{
    static_assert(utils::function_traits<F>::arity == 1, "Wrong arity.");
    return IsGreaterOrEqualByAndBy(f, f, x, y);
}

// x >= y
template <typename T>
bool IsGreaterOrEqual(const T& x, const T& y)
{
    return IsGreaterOrEqualBy(Identity<T>, x, y);
}

} // namespace fplus