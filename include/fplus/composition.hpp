// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <fplus/function_traits.hpp>
#include <fplus/internal/apply.hpp>
#include <fplus/internal/asserts/functions.hpp>
#include <fplus/internal/asserts/composition.hpp>
#include <fplus/internal/composition.hpp>

#include <functional>
#include <map>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <utility>

namespace fplus
{

// API search type: bind_1st_of_2 : (((a, b) -> c), a) -> (b -> c)
// Bind first parameter of binary function.
template <typename F, typename T>
auto bind_1st_of_2(F f, T x)
{
    return [f, x](auto&& y) {
        internal::trigger_static_asserts<internal::bind_1st_of_2_tag,
                                             F,
                                             T,
                                             decltype(y)>();
        return internal::invoke(f, x, std::forward<decltype(y)>(y));
    };
}

// API search type: bind_2nd_of_2 : (((a, b) -> c), b) -> (a -> c)
// Bind second parameter of binary function.
template <typename F, typename T>
auto bind_2nd_of_2(F f, T y)
{
    return [f, y](auto&& x) {
        internal::trigger_static_asserts<internal::bind_2nd_of_2_tag,
                                             F,
                                             decltype(x),
                                             T>();
        return internal::invoke(f, std::forward<decltype(x)>(x), y);
    };
}

// API search type: bind_1st_of_3 : (((a, b, c) -> d), a) -> ((b, c) -> d)
// Bind first parameter of ternary function.
template <typename F, typename X>
auto bind_1st_of_3(F f, X x)
{
    return [f, x](auto&& y, auto&& z) {
        internal::trigger_static_asserts<internal::bind_1st_of_3_tag,
                                             F,
                                             X,
                                             decltype(y),
                                             decltype(z)>();
        return internal::invoke(
            f, x, std::forward<decltype(y)>(y), std::forward<decltype(z)>(z));
    };
}

// API search type: bind_1st_and_2nd_of_3 : (((a, b, c) -> d), a, b) -> (c -> d)
// Bind first and second parameter of ternary function.
template <typename F, typename X, typename Y>
auto bind_1st_and_2nd_of_3(F f, X x, Y y)
{
    return [f, x, y](auto&& z) {
        internal::trigger_static_asserts<internal::bind_1st_and_2nd_of_3_tag,
                                             F,
                                             X,
                                             Y,
                                             decltype(z)>();
        return internal::invoke(f, x, y, std::forward<decltype(z)>(z));
    };
}

// API search type: bind_2nd_and_3rd_of_3 : (((a, b, c) -> d), b, c) -> (a -> d)
// Bind first and second parameter of ternary function.
template <typename F, typename Y, typename Z>
auto bind_2nd_and_3rd_of_3(F f, Y y, Z z)
{
    return [f, y, z](auto&& x) {
        internal::trigger_static_asserts<internal::bind_2nd_and_3rd_of_3_tag,
                                             F,
                                             decltype(x),
                                             Y,
                                             Z>();
        return internal::invoke(f, std::forward<decltype(x)>(x), y, z);
    };
}

// API search type: flip : (a -> b) -> (b -> a)
// Flips the arguments of a binary function
// Note: The callable can take a variadic number of arguments
template <typename F>
auto flip(F f)
{
    return [f](auto&&... args) {
        return internal::apply_impl(
            f,
            std::forward_as_tuple(std::forward<decltype(args)>(args)...),
            internal::make_reverse_index_sequence<sizeof...(args)>{});
    };
}

// API search type: forward_apply : (a, (a -> b)) -> b
// Forward application.
// Returns the result of applying the function f to the value x.
template <typename X, typename F>
auto forward_apply(X&& x, F f)
{
    internal::trigger_static_asserts<internal::unary_function_tag, F, X>();
    return internal::invoke(f, std::forward<X>(x));
}

// API search type: lazy : ((a -> b), a) -> (() -> b)
// Lazy evaluation.
// Returns a function evaluating f with the given arguments when called.
// Also known as defer.
// Note: f can take a variadic number of parameters
template<typename F, typename... Args>
auto lazy(F f, Args ... args)
{
    return [f, args...] {
        internal::trigger_static_asserts<internal::check_arity_tag, F, Args...>();
        return internal::invoke(f, args...);
    };
}

// API search type: fixed : a -> (() -> a)
// Identity as a nullary function.
// Returns a function returning x when called.
// Like lazy with identity as f.
template<typename T>
auto fixed(T x)
{
    return [x]() -> T
    {
        return x;
    };
}

// API search type: compose : ((a -> b), (b -> c)) -> (a -> c)
// Forward function composition.
// compose(f, g)(x) = g(f(x))
// It is possible to compose a variadic number of callables.
// The first callable can also take a variadic number of parameters.
// compose(f, g, h)(x, y, z) = h(g(f(x, y, z)))
template <typename... Fs>
auto compose(Fs&&... fs)
{
    return internal::compose_impl<Fs...>(std::forward<Fs>(fs)...);
}

// API search type: logical_not : (a -> Bool) -> (a -> Bool)
// Converts a predicate p into a new one,
// always returning the exact opposite of p.
// logical_not(f) = \x -> !x
// Note: F can take a variadic number of parameters.
// Equivalent to std::not_fn (C++17)
template <typename Predicate>
auto logical_not(Predicate f)
{
    return [f](auto&&... args) {
        internal::trigger_static_asserts<internal::unary_function_tag,
                                             Predicate,
                                             decltype(args)...>();
        using Res =
            std::decay_t<internal::invoke_result_t<Predicate, decltype(args)...>>;
        static_assert(std::is_same<Res, bool>::value, "Function must return bool.");

        return !internal::invoke(f, std::forward<decltype(args)>(args)...);
    };
}

// API search type: logical_or : ((a -> Bool), (a -> Bool)) -> (a -> Bool)
// logical_or(f, g) = \x -> f(x) or g(x)
// Combines to unary predicates into a single one
// that holds true if at least one of the original predicated is true.
template <typename UnaryPredicateF, typename UnaryPredicateG>
auto logical_or(UnaryPredicateF f, UnaryPredicateG g)
{
    auto op = [](auto f1, auto f2, auto x) {
        return internal::invoke(f1, x) || internal::invoke(f2, x);
    };

    return internal::logical_binary_op(op, f, g);
}

// API search type: logical_and : ((a -> Bool), (a -> Bool)) -> (a -> Bool)
// logical_and(f, g) = \x -> f(x) and g(x)
// Combines to unary predicates into a single one
// that holds true if both original predicated are true.
template <typename UnaryPredicateF, typename UnaryPredicateG>
auto logical_and(UnaryPredicateF f, UnaryPredicateG g)
{
  auto op = [](auto f1, auto f2, auto x) {
    return internal::invoke(f1, x) && internal::invoke(f2, x);
  };

  return internal::logical_binary_op(op, f, g);
}

// API search type: logical_xor : ((a -> Bool), (a -> Bool)) -> (a -> Bool)
// logical_xor(f, g) = \x -> f(x) xor g(x)
// Combines to unary predicates into a single one
// that holds true if exactly one of the original predicated is true.
template <typename UnaryPredicateF, typename UnaryPredicateG>
auto logical_xor(UnaryPredicateF f, UnaryPredicateG g)
{
  auto op = [](auto f1, auto f2, auto x) {
    return internal::invoke(f1, x) != internal::invoke(f2, x);
  };

  return internal::logical_binary_op(op, f, g);
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
            return storage.emplace(x, internal::invoke(f, x)).first->second;
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
        return internal::invoke(f, params.first, params.second);
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
// const auto create_foo = constructor_as_function<foo, int, int>;
// create_foo(1,2) == foo(1, 2);
template <typename T, class ... Types>
T constructor_as_function(Types ... args)
{
    return T(args...);
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
