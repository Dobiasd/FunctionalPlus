// Copyright Tobias Hermann 2015.
// https://github.com/Dobiasd/FunctionalPlus
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
    static_assert(utils::function_traits<UnaryPredicate>::arity == 1,"Wrong arity.");
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
T identity(const T& x)
{
    return x;
}


// x == y
template <typename T>
bool is_equal(const T& x, const T& y)
{
    return x == y;
}

// always(x, y) == x
template <typename Y, typename X>
std::function<X(const Y&)> always(const X& x)
{
    return [x](const Y&) { return x; };
}

// f(x) == g(y)
template <typename F, typename G,
    typename FIn = typename utils::function_traits<F>::template arg<0>::type,
    typename GIn = typename utils::function_traits<G>::template arg<0>::type,
    typename FOut = typename utils::function_traits<F>::result_type,
    typename GOut = typename utils::function_traits<G>::result_type>
std::function<bool(const FIn& x, const GIn& y)>
        is_equal_by_and_by(F f, G g)
{
    check_compare_preprocessors_for_types<F, G, FIn, GIn>();
    return [f, g](const FIn& x, const GIn& y)
    {
        return is_equal(f(x), g(y));
    };
}

// f(x) == f(y)
template <typename F,
    typename FIn = typename utils::function_traits<F>::template arg<0>::type,
    typename FOut = typename utils::function_traits<F>::result_type>
std::function<bool(const FIn& x, const FIn& y)>
        is_equal_by(F f)
{
    return is_equal_by_and_by(f, f);
}

// x == y
template <typename T>
std::function<bool(const T&)> is_equal_to(const T& x)
{
    return [x](const T& y)
    {
        return is_equal(y, x);
    };
}


// x != y
template <typename T>
bool is_not_equal(const T& x, const T& y)
{
    return x != y;
}


// f(x) != g(y)
template <typename F, typename G,
    typename FIn = typename utils::function_traits<F>::template arg<0>::type,
    typename GIn = typename utils::function_traits<G>::template arg<0>::type,
    typename FOut = typename utils::function_traits<F>::result_type,
    typename GOut = typename utils::function_traits<G>::result_type>
std::function<bool(const FIn& x, const GIn& y)>
        is_not_equal_by_and_by(F f, G g)
{
    check_compare_preprocessors_for_types<F, G, FIn, GIn>();
    return [f, g](const FIn& x, const GIn& y)
    {
        return is_not_equal(f(x), g(y));
    };
}

// f(x) != f(y)
template <typename F,
    typename FIn = typename utils::function_traits<F>::template arg<0>::type,
    typename FOut = typename utils::function_traits<F>::result_type>
std::function<bool(const FIn& x, const FIn& y)>
        is_not_equal_by(F f)
{
    return is_not_equal_by_and_by(f, f);
}

// x != y
template <typename T>
std::function<bool(const T&)> is_not_equal_to(const T& x)
{
    return [x](const T& y)
    {
        return is_not_equal(y, x);
    };
}


// x < y
template <typename T>
bool is_less(const T& x, const T& y)
{
    return x < y;
}


// f(x) < g(y)
template <typename F, typename G,
    typename FIn = typename utils::function_traits<F>::template arg<0>::type,
    typename GIn = typename utils::function_traits<G>::template arg<0>::type,
    typename FOut = typename utils::function_traits<F>::result_type,
    typename GOut = typename utils::function_traits<G>::result_type>
std::function<bool(const FIn& x, const GIn& y)>
        is_less_by_and_by(F f, G g)
{
    check_compare_preprocessors_for_types<F, G, FIn, GIn>();
    return [f, g](const FIn& x, const GIn& y)
    {
        return is_less(f(x), g(y));
    };
}

// f(x) < f(y)
template <typename F,
    typename FIn = typename utils::function_traits<F>::template arg<0>::type,
    typename FOut = typename utils::function_traits<F>::result_type>
std::function<bool(const FIn& x, const FIn& y)>
        is_less_by(F f)
{
    return is_less_by_and_by(f, f);
}

// y < x
template <typename T>
std::function<bool(const T&)> is_less_than(const T& x)
{
    return [x](const T& y)
    {
        return is_less(y, x);
    };
}


// x <= y
template <typename T>
bool is_less_or_equal(const T& x, const T& y)
{
    return x <= y;
}


// f(x) <= g(y)
template <typename F, typename G,
    typename FIn = typename utils::function_traits<F>::template arg<0>::type,
    typename GIn = typename utils::function_traits<G>::template arg<0>::type,
    typename FOut = typename utils::function_traits<F>::result_type,
    typename GOut = typename utils::function_traits<G>::result_type>
std::function<bool(const FIn& x, const GIn& y)>
        is_less_or_equal_by_and_by(F f, G g)
{
    check_compare_preprocessors_for_types<F, G, FIn, GIn>();
    return [f, g](const FIn& x, const GIn& y)
    {
        return is_less_or_equal(f(x), g(y));
    };
}

// f(x) <= f(y)
template <typename F,
    typename FIn = typename utils::function_traits<F>::template arg<0>::type,
    typename FOut = typename utils::function_traits<F>::result_type>
std::function<bool(const FIn& x, const FIn& y)>
        is_less_or_equal_by(F f)
{
    return is_less_or_equal_by_and_by(f, f);
}

// y <= x
template <typename T>
std::function<bool(const T&)> is_less_or_equal_than(const T& x)
{
    return [x](const T& y)
    {
        return is_less_or_equal(y, x);
    };
}


// x > y
template <typename T>
bool is_greater(const T& x, const T& y)
{
    return x > y;
}


// f(x) > g(y)
template <typename F, typename G,
    typename FIn = typename utils::function_traits<F>::template arg<0>::type,
    typename GIn = typename utils::function_traits<G>::template arg<0>::type,
    typename FOut = typename utils::function_traits<F>::result_type,
    typename GOut = typename utils::function_traits<G>::result_type>
std::function<bool(const FIn& x, const GIn& y)>
        is_greater_by_and_by(F f, G g)
{
    check_compare_preprocessors_for_types<F, G, FIn, GIn>();
    return [f, g](const FIn& x, const GIn& y)
    {
        return is_greater(f(x), g(y));
    };
}

// f(x) > f(y)
template <typename F,
    typename FIn = typename utils::function_traits<F>::template arg<0>::type,
    typename FOut = typename utils::function_traits<F>::result_type>
std::function<bool(const FIn& x, const FIn& y)>
        is_greater_by(F f)
{
    return is_greater_by_and_by(f, f);
}

// y > x
template <typename T>
std::function<bool(const T&)> is_greater_than(const T& x)
{
    return [x](const T& y)
    {
        return is_greater(y, x);
    };
}


// x >= y
template <typename T>
bool is_greater_or_equal(const T& x, const T& y)
{
    return x >= y;
}


// f(x) >= g(y)
template <typename F, typename G,
    typename FIn = typename utils::function_traits<F>::template arg<0>::type,
    typename GIn = typename utils::function_traits<G>::template arg<0>::type,
    typename FOut = typename utils::function_traits<F>::result_type,
    typename GOut = typename utils::function_traits<G>::result_type>
std::function<bool(const FIn& x, const GIn& y)>
        is_greater_or_equal_by_and_by(F f, G g)
{
    check_compare_preprocessors_for_types<F, G, FIn, GIn>();
    return [f, g](const FIn& x, const GIn& y)
    {
        return is_greater_or_equal(f(x), g(y));
    };
}

// f(x) >= f(y)
template <typename F,
    typename FIn = typename utils::function_traits<F>::template arg<0>::type,
    typename FOut = typename utils::function_traits<F>::result_type>
std::function<bool(const FIn& x, const FIn& y)>
        is_greater_or_equal_by(F f)
{
    return is_greater_or_equal_by_and_by(f, f);
}

// y >= x
template <typename T>
std::function<bool(const T&)> is_greater_or_equal_than(const T& x)
{
    return [x](const T& y)
    {
        return is_greater_or_equal(y, x);
    };
}


template <typename T>
bool xor_bools(const T& x, const T& y)
{
    static_assert(std::is_convertible<T, bool>::value, "Type must be convertible to bool.");
    return (x && !y) || (!x && y);
}

// ord_to_eq((<)) == (==)
template <typename Compare,
    typename FIn0 = typename utils::function_traits<Compare>::template arg<0>::type,
    typename FIn1 = typename utils::function_traits<Compare>::template arg<1>::type,
    typename FOut = typename utils::function_traits<Compare>::result_type>
std::function<FOut(FIn0, FIn1)> ord_to_eq(Compare comp)
{
    static_assert(utils::function_traits<Compare>::arity == 2, "Wrong arity.");
    static_assert(std::is_same<FOut, bool>::value, "Function must return bool.");
    static_assert(std::is_same<FIn0, FIn1>::value, "Function must take two equal types.");
    return [comp]
           (FIn0 x, FIn1 y)
           { return !comp(x, y) && !comp(y, x); };
}

// ord_to_not_eq((<)) == (!=)
template <typename Compare,
    typename FIn0 = typename utils::function_traits<Compare>::template arg<0>::type,
    typename FIn1 = typename utils::function_traits<Compare>::template arg<1>::type,
    typename FOut = typename utils::function_traits<Compare>::result_type>
std::function<FOut(FIn0, FIn1)> ord_to_not_eq(Compare comp)
{
    static_assert(utils::function_traits<Compare>::arity == 2, "Wrong arity.");
    static_assert(std::is_same<FOut, bool>::value, "Function must return bool.");
    static_assert(std::is_same<FIn0, FIn1>::value, "Function must take two equal types.");
    return [comp]
           (FIn0 x, FIn1 y)
           { return comp(x, y) || comp(y, x); };
}

// ord_eq_to_eq((<=)) == (==)
template <typename Compare,
    typename FIn0 = typename utils::function_traits<Compare>::template arg<0>::type,
    typename FIn1 = typename utils::function_traits<Compare>::template arg<1>::type,
    typename FOut = typename utils::function_traits<Compare>::result_type>
std::function<FOut(FIn0, FIn1)> ord_eq_to_eq(Compare comp)
{
    static_assert(utils::function_traits<Compare>::arity == 2, "Wrong arity.");
    static_assert(std::is_same<FOut, bool>::value, "Function must return bool.");
    static_assert(std::is_same<FIn0, FIn1>::value, "Function must take two equal types.");
    return [comp]
           (FIn0 x, FIn1 y)
           { return comp(x, y) && comp(y, x); };
}

// ord_eq_to_not_eq((<=)) == (!=)
template <typename Compare,
    typename FIn0 = typename utils::function_traits<Compare>::template arg<0>::type,
    typename FIn1 = typename utils::function_traits<Compare>::template arg<1>::type,
    typename FOut = typename utils::function_traits<Compare>::result_type>
std::function<FOut(FIn0, FIn1)> ord_eq_to_not_eq(Compare comp)
{
    static_assert(utils::function_traits<Compare>::arity == 2, "Wrong arity.");
    static_assert(std::is_same<FOut, bool>::value, "Function must return bool.");
    static_assert(std::is_same<FIn0, FIn1>::value, "Function must take two equal types.");
    return [comp]
           (FIn0 x, FIn1 y)
           {
                bool a = comp(x, y);
                bool b = comp(y, x);
                return (a && !b) || (!a && b);
           };
}

} // namespace fplus