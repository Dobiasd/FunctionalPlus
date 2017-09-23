// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <fplus/function_traits.hpp>

#include <functional>
#include <map>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <utility>

namespace fplus
{

// API search type: bind_unary : ((a -> b), a) -> (() -> b)
// Bind parameter of unary function for later executation.
template <typename F, typename T,
    typename FIn0 = typename utils::function_traits<F>::template arg<0>::type,
    typename FOut = typename std::result_of<F(FIn0)>::type>
std::function<FOut()> bind_unary(F f, T x)
{
    static_assert(utils::function_traits<F>::arity == 1, "Wrong arity.");
    static_assert(std::is_convertible<T, FIn0>::value,
        "Function can not take bound parameter type.");
    return [f, x] () { return f(x); };
}

// API search type: bind_1st_of_2 : (((a, b) -> c), a) -> (b -> c)
// Bind first parameter of binary function.
template <typename F, typename T,
    typename FIn0 = typename utils::function_traits<F>::template arg<0>::type,
    typename FIn1 = typename utils::function_traits<F>::template arg<1>::type,
    typename FOut = typename std::result_of<F(FIn0, FIn1)>::type>
std::function<FOut(FIn1)> bind_1st_of_2(F f, T x)
{
    static_assert(utils::function_traits<F>::arity == 2, "Wrong arity.");
    static_assert(std::is_convertible<T, FIn0>::value,
        "Function can not take bound parameter type.");
    return [f, x]
           (FIn1&& y)
           { return f(x, std::forward<FIn1>(y)); };
}

// API search type: bind_2nd_of_2 : (((a, b) -> c), b) -> (a -> c)
// Bind second parameter of binary function.
template <typename F, typename T,
    typename FIn0 = typename utils::function_traits<F>::template arg<0>::type,
    typename FIn1 = typename utils::function_traits<F>::template arg<1>::type,
    typename FOut = typename std::result_of<F(FIn0, FIn1)>::type>
std::function<FOut(FIn0)> bind_2nd_of_2(F f, T y)
{
    static_assert(utils::function_traits<F>::arity == 2, "Wrong arity.");
    static_assert(std::is_convertible<T, FIn1>::value,
        "Function can not take bound parameter type.");
    return [f, y]
           (FIn0&& x)
           { return f(std::forward<FIn0>(x), y); };
}

// API search type: bind_1st_of_3 : (((a, b, c) -> d), a) -> ((b, c) -> d)
// Bind first parameter of ternary function.
template <typename F, typename X,
    typename FIn0 = typename utils::function_traits<F>::template arg<0>::type,
    typename FIn1 = typename utils::function_traits<F>::template arg<1>::type,
    typename FIn2 = typename utils::function_traits<F>::template arg<2>::type,
    typename FOut = typename std::result_of<F(FIn0, FIn1, FIn2)>::type>
std::function<FOut(FIn1, FIn2)> bind_1st_of_3(F f, X x)
{
    static_assert(utils::function_traits<F>::arity == 3, "Wrong arity.");
    static_assert(std::is_convertible<X, FIn0>::value,
        "Function can not take bound parameter type.");
    return [f, x]
           (FIn1&& y, FIn2&& z)
           { return f(x, std::forward<FIn1>(y), std::forward<FIn2>(z)); };
}

// API search type: bind_1st_and_2nd_of_3 : (((a, b, c) -> d), a, b) -> (c -> d)
// Bind first and second parameter of ternary function.
template <typename F, typename X, typename Y,
    typename FIn0 = typename utils::function_traits<F>::template arg<0>::type,
    typename FIn1 = typename utils::function_traits<F>::template arg<1>::type,
    typename FIn2 = typename utils::function_traits<F>::template arg<2>::type,
    typename FOut = typename std::result_of<F(FIn0, FIn1, FIn2)>::type>
std::function<FOut(FIn2)> bind_1st_and_2nd_of_3(F f, X x, Y y)
{
    static_assert(utils::function_traits<F>::arity == 3, "Wrong arity.");
    static_assert(std::is_convertible<X, FIn0>::value,
        "Function can not take first bound parameter type.");
    static_assert(std::is_convertible<Y, FIn1>::value,
        "Function can not take second bound parameter type.");
    return [f, x, y]
           (FIn2&& z)
           { return f(x, y, std::forward<FIn2>(z)); };
}

// API search type: flip : (a -> b) -> (b -> a)
// Flips the arguments of a binary function
template <typename F,
    typename A = typename utils::function_traits<F>::template arg<0>::type,
    typename B = typename utils::function_traits<F>::template arg<1>::type,
    typename C = typename std::result_of<F(A, B)>::type>
std::function<C(B, A)> flip(F f)
{
    static_assert(utils::function_traits<F>::arity == 2, "Wrong arity.");
    return [f](B&& y, A&& x) {
        return f(std::forward<A>(x), std::forward<B>(y));
    };
}

// API search type: forward_apply : (a, (a -> b)) -> b
// Forward application.
// Returns the result of applying the function f to the value x.
template <typename X, typename F,
    typename FOut = typename std::result_of<F(X)>::type>
FOut forward_apply(X&& x, F f)
{
    static_assert(utils::function_traits<F>::arity == 1, "Wrong arity.");
    return f(std::forward<X>(x));
}

// API search type: lazy : ((a -> b), a) -> (() -> b)
// Lazy evaluation.
// Returns a function evaluating f with the given arguments when called.
// Also known as defer.
template<typename F, typename... Args,
    typename FOut = typename std::result_of<F(Args ...)>::type>
std::function<FOut()> lazy(F f, Args ... args)
{
    return [f, args...]() -> FOut
    {
        return f(args...);
    };
}

// API search type: fixed : a -> (() -> a)
// Identity as a nullary function.
// Returns a function returning x when called.
// Like lazy with identity as f.
template<typename T>
std::function<T()> fixed(T x)
{
    return [x]() -> T
    {
        return x;
    };
}

// API search type: compose : ((a -> b), (b -> c)) -> (a -> c)
// Forward function composition.
// compose(f, g)(x) = g(f(x))
template <typename F, typename G,
    typename FIn = typename utils::function_traits<F>::template arg<0>::type,
    typename FOut = typename std::result_of<F(FIn)>::type,
    typename GIn = typename utils::function_traits<G>::template arg<0>::type,
    typename GOut = typename std::result_of<G(GIn)>::type>
std::function<GOut(FIn)> compose(F f, G g)
{
    static_assert(utils::function_traits<F>::arity == 1, "Wrong arity.");
    static_assert(utils::function_traits<G>::arity == 1, "Wrong arity.");
    static_assert(std::is_convertible<FOut,GIn>::value,
        "Parameter types do not match");
    return [f, g](FIn&& x) { return g(f(std::forward<FIn>(x))); };
}

// API search type: compose : ((a -> b), (b -> c), (c -> d)) -> (a -> d)
// Forward function composition.
// compose(f, g, h)(x) = h(g(f(x)))
template <typename F, typename G, typename H,
    typename FIn = typename utils::function_traits<F>::template arg<0>::type,
    typename FOut = typename std::result_of<F(FIn)>::type,
    typename GIn = typename utils::function_traits<G>::template arg<0>::type,
    typename GOut = typename std::result_of<G(GIn)>::type,
    typename HIn = typename utils::function_traits<H>::template arg<0>::type,
    typename HOut = typename std::result_of<H(HIn)>::type>
std::function<HOut(FIn)> compose(F f, G g, H h)
{
    static_assert(utils::function_traits<F>::arity == 1, "Wrong arity.");
    static_assert(utils::function_traits<G>::arity == 1, "Wrong arity.");
    static_assert(utils::function_traits<H>::arity == 1, "Wrong arity.");
    static_assert(std::is_convertible<FOut,GIn>::value,
        "Parameter types do not match");
    static_assert(std::is_convertible<GOut,HIn>::value,
        "Parameter types do not match");
    return [f, g, h](FIn&& x) { return h(g(f(std::forward<FIn>(x)))); };
}

// API search type: compose : ((a -> b), (b -> c), (c -> d), (d -> e)) -> (a -> e)
// Forward function composition.
// compose(f, g, h, i)(x) = i(h(g(f(x))))
template <typename F, typename G, typename H, typename I,
    typename FIn = typename utils::function_traits<F>::template arg<0>::type,
    typename FOut = typename std::result_of<F(FIn)>::type,
    typename GIn = typename utils::function_traits<G>::template arg<0>::type,
    typename GOut = typename std::result_of<G(GIn)>::type,
    typename HIn = typename utils::function_traits<H>::template arg<0>::type,
    typename HOut = typename std::result_of<H(HIn)>::type,
    typename IIn = typename utils::function_traits<I>::template arg<0>::type,
    typename IOut = typename std::result_of<I(IIn)>::type>
std::function<IOut(FIn)> compose(F f, G g, H h, I i)
{
    static_assert(utils::function_traits<F>::arity == 1, "Wrong arity.");
    static_assert(utils::function_traits<G>::arity == 1, "Wrong arity.");
    static_assert(utils::function_traits<H>::arity == 1, "Wrong arity.");
    static_assert(utils::function_traits<I>::arity == 1, "Wrong arity.");
    static_assert(std::is_convertible<FOut,GIn>::value,
        "Parameter types do not match");
    static_assert(std::is_convertible<GOut,HIn>::value,
        "Parameter types do not match");
    static_assert(std::is_convertible<HOut,IIn>::value,
        "Parameter types do not match");
    return [f, g, h, i](FIn&& x) { return i(h(g(f(std::forward<FIn>(x))))); };
}

// API search type: compose : ((a -> b), (b -> c), (c -> d), (d -> e), (e -> f)) -> (a -> f)
// Forward function composition.
// compose(f, g, h, i, j)(x) = j(i(h(g(f(x)))))
template <typename F, typename G, typename H, typename I, typename J,
    typename FIn = typename utils::function_traits<F>::template arg<0>::type,
    typename FOut = typename std::result_of<F(FIn)>::type,
    typename GIn = typename utils::function_traits<G>::template arg<0>::type,
    typename GOut = typename std::result_of<G(GIn)>::type,
    typename HIn = typename utils::function_traits<H>::template arg<0>::type,
    typename HOut = typename std::result_of<H(HIn)>::type,
    typename IIn = typename utils::function_traits<I>::template arg<0>::type,
    typename IOut = typename std::result_of<I(IIn)>::type,
    typename JIn = typename utils::function_traits<I>::template arg<0>::type,
    typename JOut = typename std::result_of<I(JIn)>::type>
std::function<JOut(FIn)> compose(F f, G g, H h, I i, J j)
{
    static_assert(utils::function_traits<F>::arity == 1, "Wrong arity.");
    static_assert(utils::function_traits<G>::arity == 1, "Wrong arity.");
    static_assert(utils::function_traits<H>::arity == 1, "Wrong arity.");
    static_assert(utils::function_traits<I>::arity == 1, "Wrong arity.");
    static_assert(utils::function_traits<J>::arity == 1, "Wrong arity.");
    static_assert(std::is_convertible<FOut,GIn>::value,
        "Parameter types do not match");
    static_assert(std::is_convertible<GOut,HIn>::value,
        "Parameter types do not match");
    static_assert(std::is_convertible<HOut,IIn>::value,
        "Parameter types do not match");
    static_assert(std::is_convertible<IOut,JIn>::value,
        "Parameter types do not match");
    return [f, g, h, i, j](FIn&& x)
    {
        return j(i(h(g(f(std::forward<FIn>(x))))));
    };
}

// API search type: logical_not : (a -> Bool) -> (a -> Bool)
// Converts a unary predicate p
// into a new one, always returning the exact opposite of p.
// logical_not(f) = \x -> !x
template <typename UnaryPredicate,
    typename X = typename utils::function_traits<UnaryPredicate>::template arg<0>::type>
std::function<bool(X)> logical_not(UnaryPredicate f)
{
    static_assert(utils::function_traits<UnaryPredicate>::arity == 1,
        "Wrong arity.");
    typedef typename std::result_of<UnaryPredicate(X)>::type Res;
    static_assert(std::is_same<Res, bool>::value, "Must return bool.");
    return [f](X&& x) { return !f(std::forward<X>(x)); };
}

// API search type: logical_or : ((a -> Bool), (a -> Bool)) -> (a -> Bool)
// logical_or(f, g) = \x -> f(x) or g(x)
// Combines to unary predicates into a single one
// that holds true if at least one of the original predicated is true.
template <typename UnaryPredicateF, typename UnaryPredicateG,
    typename X = typename utils::function_traits<UnaryPredicateF>::template arg<0>::type,
    typename Y = typename utils::function_traits<UnaryPredicateG>::template arg<0>::type>
std::function<bool(X)> logical_or(UnaryPredicateF f, UnaryPredicateG g)
{
    static_assert(std::is_convertible<X, Y>::value,
        "Parameter types do not match");
    typedef typename std::result_of<UnaryPredicateF(X)>::type FRes;
    typedef typename std::result_of<UnaryPredicateG(Y)>::type GRes;
    static_assert(std::is_same<FRes, bool>::value, "Must return bool.");
    static_assert(std::is_same<GRes, bool>::value, "Must return bool.");
    static_assert(utils::function_traits<UnaryPredicateF>::arity == 1,
        "Wrong arity.");
    static_assert(utils::function_traits<UnaryPredicateG>::arity == 1,
        "Wrong arity.");
    return [f, g](X&& x)
    {
        return f(std::forward<X>(x)) || g(std::forward<X>(x));
    };
}

// API search type: logical_and : ((a -> Bool), (a -> Bool)) -> (a -> Bool)
// logical_and(f, g) = \x -> f(x) and g(x)
// Combines to unary predicates into a single one
// that holds true if both original predicated are true.
template <typename UnaryPredicateF, typename UnaryPredicateG,
    typename X = typename utils::function_traits<UnaryPredicateF>::template arg<0>::type,
    typename Y = typename utils::function_traits<UnaryPredicateG>::template arg<0>::type>
std::function<bool(X)> logical_and(UnaryPredicateF f, UnaryPredicateG g)
{
    static_assert(std::is_convertible<X, Y>::value,
        "Parameter types do not match");
    typedef typename std::result_of<UnaryPredicateF(X)>::type FRes;
    typedef typename std::result_of<UnaryPredicateG(Y)>::type GRes;
    static_assert(std::is_same<FRes, bool>::value, "Must return bool.");
    static_assert(std::is_same<GRes, bool>::value, "Must return bool.");
    static_assert(utils::function_traits<UnaryPredicateF>::arity == 1,
        "Wrong arity.");
    static_assert(utils::function_traits<UnaryPredicateG>::arity == 1,
        "Wrong arity.");
    return [f, g](X&& x)
    {
        return f(std::forward<X>(x)) && g(std::forward<X>(x));
    };
}

// API search type: logical_xor : ((a -> Bool), (a -> Bool)) -> (a -> Bool)
// logical_xor(f, g) = \x -> f(x) xor g(x)
// Combines to unary predicates into a single one
// that holds true if exactly one of the original predicated is true.
template <typename UnaryPredicateF, typename UnaryPredicateG,
    typename X = typename utils::function_traits<UnaryPredicateF>::template arg<0>::type,
    typename Y = typename utils::function_traits<UnaryPredicateG>::template arg<0>::type>
std::function<bool(X)> logical_xor(UnaryPredicateF f, UnaryPredicateG g)
{
    static_assert(std::is_convertible<X, Y>::value,
        "Parameter types do not match");
    typedef typename std::result_of<UnaryPredicateF(X)>::type FRes;
    typedef typename std::result_of<UnaryPredicateG(Y)>::type GRes;
    static_assert(std::is_same<FRes, bool>::value, "Must return bool.");
    static_assert(std::is_same<GRes, bool>::value, "Must return bool.");
    static_assert(utils::function_traits<UnaryPredicateF>::arity == 1,
        "Wrong arity.");
    static_assert(utils::function_traits<UnaryPredicateG>::arity == 1,
        "Wrong arity.");
    return [f, g](X&& x)
    {
        auto fx = f(std::forward<X>(x));
        auto gx = g(std::forward<X>(x));
        return (fx && !gx) || (!fx && gx);
    };
}

// API search type: memoize : (a -> b) -> (a -> b)
// Provides Memoization for a given (referentially transparent)
// unary function.
// Returns a closure mutating an internally held dictionary
// mapping input values to output values.
template <typename F,
    typename FIn = typename utils::function_traits<F>::template arg<0>::type,
    typename FOut = typename std::result_of<F(FIn)>::type,
    typename MemoMap = std::unordered_map<
        typename std::remove_reference<typename std::remove_const<FIn>::type>::type,
        FOut>>
std::function<FOut(FIn)> memoize(F f)
{
    static_assert(utils::function_traits<F>::arity == 1, "Wrong arity.");
    MemoMap storage;
    return [=](FIn x) mutable -> FOut
    {
        const auto it = storage.find(x);
        if (it == storage.end())
        {
            return storage.insert(std::make_pair(x, f(x))).first->second;
        }
        else
        {
            return it->second;
        }
    };
}

namespace internal
{
    template <typename F, typename Cache,
        typename FIn1 = typename utils::function_traits<F>::template arg<0>::type,
        typename FIn2 = typename utils::function_traits<F>::template arg<1>::type,
        typename FOut = typename std::result_of<F(FIn1, FIn2)>::type,
        typename ResultF = std::function<FOut(FIn2)>>
    ResultF memoize_recursive_helper(const F f, std::shared_ptr<Cache> storage)
    {
        return [f, storage](FIn2 x)
        {
            const auto it = storage->find(x);
            if (it == storage->end())
            {
                const auto g = memoize_recursive_helper(f, storage);
                (*storage)[x] = f(g, x);
            }
            return (*storage)[x];
        };
    }
} // namespace internal

// API search type: memoize_recursive : (a -> b) -> (a -> b)
// Provides Memoization for a given (referentially transparent)
// recursive binary function that takes a continuation as first argument.
// e.g.
// uint64_t fibo_cont(const std::function<uint64_t(uint64_t)>& cont, uint64_t n)
// {
//     if (n < 2) return n;
//     else return cont(n-1) + cont(n-2);
// }
// Returns a closure mutating an internally held dictionary
// mapping input values to output values.
template <typename F,
    typename FIn1 = typename utils::function_traits<F>::template arg<0>::type,
    typename FIn2 = typename utils::function_traits<F>::template arg<1>::type,
    typename FOut = typename std::result_of<F(FIn1, FIn2)>::type,
    typename MemoMap = std::unordered_map<
        typename std::remove_reference<typename std::remove_const<FIn2>::type>::type,
        FOut>>
std::function<FOut(FIn2)> memoize_recursive(F f)
{
    std::shared_ptr<MemoMap> storage = std::make_shared<MemoMap>();
    return internal::memoize_recursive_helper(f, storage);
}

// API search type: memoize_binary : ((a, b) -> c) -> ((a, b) -> c)
// Provides Memoization for a given (referentially transparent)
// binary function.
// Returns a closure mutating an internally held dictionary
// mapping input values to output values.
template <typename F,
    typename FIn1 = typename utils::function_traits<F>::template arg<0>::type,
    typename FIn2 = typename utils::function_traits<F>::template arg<1>::type,
    typename FOut = typename std::result_of<F(FIn1, FIn2)>::type,
    typename ParamPair = std::pair<
        typename std::remove_reference<typename std::remove_const<FIn1>::type>::type,
        typename std::remove_reference<typename std::remove_const<FIn2>::type>::type>,
    typename MemoMap = std::unordered_map<ParamPair, FOut>>
std::function<FOut(FIn1, FIn2)> memoize_binary(F f)
{
    const auto unary_f = [f](const ParamPair& params) -> FOut
    {
        return f(params.first, params.second);
    };
    auto unary_f_memoized = memoize<decltype(unary_f),
        ParamPair, FOut, std::map<ParamPair, FOut>>(unary_f);
    return [unary_f_memoized](FIn1 a, FIn2 b) mutable -> FOut
    {
        return unary_f_memoized(std::make_pair(a, b));
    };
}

// API search type: constructor_as_function : a -> b
// struct foo
// {
//     foo(int a, int b) : a_(a), b_(2*b) {}
//     int a_;
//     int b_;
// };
// const auto create_foo = constructor_as_function<foo, int int>;
// create_foo(1,2) == foo(1, 2);
template <typename T, class ... Types>
T constructor_as_function(Types ... args)
{
    return T{args...};
}

} // namespace fplus

#define fplus_get_mem(fplus_get_mem_name) \
[](const auto& fplus_get_mem_x) \
{ \
    return fplus_get_mem_x.fplus_get_mem_name; \
}

#define fplus_get_ptr_mem(fplus_get_ptr_mem_name) \
[](const auto& fplus_get_ptr_mem_x) \
{ \
    return fplus_get_ptr_mem_x->fplus_get_ptr_mem_name; \
}

#define fplus_get_c_mem_t(fplus_get_c_mem_t_c, fplus_get_c_mem_t_name, fplus_get_c_mem_t_t) \
[](const fplus_get_c_mem_t_c& fplus_get_c_mem_t_x) -> fplus_get_c_mem_t_t \
{ \
    return fplus_get_c_mem_t_x.fplus_get_c_mem_t_name; \
}

#define fplus_get_c_ptr_mem_t(fplus_get_c_ptr_mem_t_c, fplus_get_c_ptr_mem_t_name, fplus_get_c_ptr_mem_t_t) \
[](const fplus_get_c_ptr_mem_t_c& fplus_get_c_ptr_mem_t_x) -> fplus_get_c_ptr_mem_t_t \
{ \
    return fplus_get_c_ptr_mem_t_x->fplus_get_c_ptr_mem_t_name; \
}

#define fplus_mem_fn(fplus_mem_fn_name) \
[](const auto& fplus_mem_fn_x) \
{ \
    return fplus_mem_fn_x.fplus_mem_fn_name(); \
}

#define fplus_ptr_mem_fn(fplus_ptr_mem_fn_name) \
[](const auto& fplus_ptr_mem_fn_x) \
{ \
    return fplus_ptr_mem_fn_x->fplus_ptr_mem_fn_name(); \
}

#define fplus_c_mem_fn_t(fplus_c_mem_fn_t_c, fplus_c_mem_fn_t_name, fplus_c_mem_fn_t_t) \
[](const fplus_c_mem_fn_t_c& fplus_c_mem_fn_t_x) -> fplus_c_mem_fn_t_t \
{ \
    return fplus_c_mem_fn_t_x.fplus_c_mem_fn_t_name(); \
}

#define fplus_c_ptr_mem_fn_t(fplus_c_ptr_mem_fn_t_c, fplus_c_ptr_mem_fn_t_name, fplus_c_ptr_mem_fn_t_t) \
[](const fplus_c_ptr_mem_fn_t_c& fplus_c_ptr_mem_fn_t_x) -> fplus_c_ptr_mem_fn_t_t \
{ \
    return fplus_c_ptr_mem_fn_t_x->fplus_c_ptr_mem_fn_t_name(); \
}
