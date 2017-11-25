// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <fplus/function_traits.hpp>
#include <fplus/maybe.hpp>

#include <fplus/internal/asserts/functions.hpp>
#include <fplus/internal/composition.hpp>
#include <fplus/internal/invoke.hpp>

#include <cassert>
#include <functional>
#include <memory>

namespace fplus
{

template <typename Ok, typename Error>
class result;

template <typename Ok, typename Error>
result<Ok, Error> ok(const Ok& val);

template <typename Ok, typename Error>
result<Ok, Error> error(const Error& error);

// Can hold a value of type Ok or an error of type Error.
template <typename Ok, typename Error>
class result
{
public:
    bool is_ok() const { return static_cast<bool>(ptr_ok_); }
    bool is_error() const { return static_cast<bool>(ptr_error_); }
    const Ok& unsafe_get_ok() const {
        check_either_or_invariant(); assert(is_ok()); return *ptr_ok_;
    }
    const Error& unsafe_get_error() const {
        check_either_or_invariant(); assert(is_error()); return *ptr_error_;
    }
    typedef Ok ok_t;
    typedef Error error_t;

    result(const result<Ok, Error>& other) :
        ptr_ok_(other.is_ok() ? ptr_ok(new Ok(other.unsafe_get_ok())) : ptr_ok()),
        ptr_error_(other.is_error() ? ptr_error(new Error(other.unsafe_get_error())) : ptr_error())
    {
        check_either_or_invariant();
    }
    result<Ok, Error>& operator = (const result<Ok, Error>& other)
    {
        ptr_ok_ = other.is_ok() ? ptr_ok(new Ok(other.unsafe_get_ok())) : ptr_ok();
        ptr_error_ = other.is_error() ? ptr_error(new Error(other.unsafe_get_error())) : ptr_error();
        return *this;
    }
private:
    void check_either_or_invariant() const
    {
        assert(is_ok() != is_error());
    }
    result() : ptr_ok_(ptr_ok()), ptr_error_(ptr_error()) {}
    typedef std::unique_ptr<Ok> ptr_ok;
    typedef std::unique_ptr<Error> ptr_error;
    friend result<Ok, Error> ok<Ok, Error>(const Ok& ok);
    friend result<Ok, Error> error<Ok, Error>(const Error& error);
    ptr_ok ptr_ok_;
    ptr_error ptr_error_;
};

// API search type: is_ok : Result a b -> Bool
// fwd bind count: 0
// Is not error?
template <typename Ok, typename Error>
bool is_ok(const result<Ok, Error>& result)
{
    return result.is_ok();
}

// API search type: is_error : Result a b -> Bool
// fwd bind count: 0
// Is not OK?
template <typename Ok, typename Error>
bool is_error(const result<Ok, Error>& result)
{
    return !is_ok(result);
}

// API search type: unsafe_get_ok : Result a b -> a
// fwd bind count: 0
// Crashes if result is error!
template <typename Ok, typename Error>
Ok unsafe_get_ok(const result<Ok, Error>& result)
{
    return result.unsafe_get_ok();
}

// API search type: unsafe_get_error : Result a b -> b
// fwd bind count: 0
// Crashes if result is ok!
template <typename Ok, typename Error>
Error unsafe_get_error(const result<Ok, Error>& result)
{
    return result.unsafe_get_error();
}

// API search type: ok_with_default : (a, Result a b) -> a
// fwd bind count: 1
// Get the value from a result or the default in case it is error.
template <typename Ok, typename Error>
Ok ok_with_default(const Ok& defaultValue, const result<Ok, Error>& result)
{
    if (is_ok(result))
        return unsafe_get_ok(result);
    return defaultValue;
}

// API search type: ok : a -> Result a b
// fwd bind count: 0
// Wrap a value in a result as a Ok.
template <typename Ok, typename Error>
result<Ok, Error> ok(const Ok& val)
{
    result<Ok, Error> x;
    x.ptr_ok_.reset(new Ok(val));
    return x;
}

// API search type: error : b -> Result a b
// fwd bind count: 0
// Construct an error of a certain result type.
template <typename Ok, typename Error>
result<Ok, Error> error(const Error& error)
{
    result<Ok, Error> x;
    x.ptr_error_.reset(new Error(error));
    return x;
}

// API search type: to_maybe : Result a b -> Maybe a
// fwd bind count: 0
// Convert ok to just, error to nothing.
template <typename Ok, typename Error>
maybe<Ok> to_maybe(const result<Ok, Error>& result)
{
    if (is_ok(result))
        return just<Ok>(unsafe_get_ok(result));
    else
        return nothing<Ok>();
}

// API search type: from_maybe : (b, Maybe a) -> Result a b
// fwd bind count: 1
// Convert just to ok, nothing to error.
template <typename Error, typename Ok>
result<Ok, Error> from_maybe(const Error& err, const maybe<Ok>& maybe)
{
    if (is_just(maybe))
        return ok<Ok, Error>(unsafe_get_just(maybe));
    else
        return error<Ok, Error>(err);
}

// API search type: throw_on_error : (e, Result a b) -> a
// fwd bind count: 1
// Throws the given exception in case of error.
// Return ok value if ok.
template <typename E, typename Ok, typename Error>
Ok throw_on_error(const E& e, const result<Ok, Error>& result)
{
    if (is_error(result))
        throw e;
    return unsafe_get_ok(result);
}

// API search type: throw_type_on_error : Result a b -> a
// Throws the given exception type constructed with error value if error.
// Return ok value if ok.
template <typename E, typename Ok, typename Error>
Ok throw_type_on_error(const result<Ok, Error>& result)
{
    if (is_error(result))
        throw E(unsafe_get_error(result));
    return unsafe_get_ok(result);
}

// True if ok values are the same or if errors are the same.
template <typename Ok, typename Error>
bool operator == (const result<Ok, Error>& x, const result<Ok, Error>& y)
{
    if (is_ok(x) && is_ok(y))
        return unsafe_get_ok(x) == unsafe_get_ok(y);
    if (is_error(x) && is_error(y))
        return unsafe_get_error(x) == unsafe_get_error(y);
    return false;
}

// False if ok values are the same or if both errors are the same.
template <typename Ok, typename Error>
bool operator != (const result<Ok, Error>& x, const result<Ok, Error>& y)
{
    return !(x == y);
}

// API search type: lift_result : ((a -> b), Result a c) -> Result b c
// fwd bind count: 1
// Lifts a function into the result functor.
// A function that for example was able to convert and int into a string,
// now can convert a result<int, Err> into a result<string, Err>.
// An error stays the same error, regardless of the conversion.
template <typename Error, typename F, typename A>
auto lift_result(F f, const result<A, Error>& r)
{
    internal::trigger_static_asserts<internal::unary_function_tag, F, A>();

    using B = std::decay_t<internal::invoke_result_t<F, A>>;

    if (is_ok(r))
        return ok<B, Error>(internal::invoke(f, unsafe_get_ok(r)));
    return error<B, Error>(unsafe_get_error(r));
}

// API search type: lift_result_both : ((a -> c), (b -> d), Result a b) -> Result c d
// fwd bind count: 2
// Lifts two functions into the result functor.
template <typename F, typename G, typename A, typename B>
auto lift_result_both(F f, G g, const result<A, B>& r)
{
    internal::trigger_static_asserts<internal::unary_function_tag, F, A>();
    internal::trigger_static_asserts<internal::unary_function_tag, G, B>();

    using C = std::decay_t<internal::invoke_result_t<F, A>>;
    using D = std::decay_t<internal::invoke_result_t<G, B>>;

    if (is_ok(r))
        return ok<C, D>(internal::invoke(f, unsafe_get_ok(r)));
    return error<C, D>(internal::invoke(g, unsafe_get_error(r)));
}

// API search type: unify_result : ((a -> c), (b -> c), Result a b) -> c
// fwd bind count: 2
// Extracts the value (Ok or Error) from a Result
// as defined by the two given functions.
template <typename F, typename G, typename A, typename B>
auto unify_result(F f, G g, const result<A, B>& r)
{
    internal::trigger_static_asserts<internal::unary_function_tag, F, A>();
    internal::trigger_static_asserts<internal::unary_function_tag, G, B>();
    static_assert(std::is_same<internal::invoke_result_t<F, A>,
                               internal::invoke_result_t<G, B>>::value,
                  "Both functions must return the same type.");
    if (is_ok(r))
        return internal::invoke(f, unsafe_get_ok(r));
    return internal::invoke(g, unsafe_get_error(r));
}

// API search type: join_result : Result (Result a b) b -> Result a b
// Flattens a nested result.
// join_result(Ok Ok x) == Ok x
// join_result(Ok Error e) == Error e
// join_result(Error e) == Error e
template <typename OK, typename Error>
result<OK, Error> join_result(const result<result<OK, Error>, Error>& r)
{
    if (is_ok(r))
        return unsafe_get_ok(r);
    else
        return error<OK, Error>(r.unsafe_get_error());
}

// API search type: and_then_result : ((a -> Result c b), (Result a b)) -> Result c b
// fwd bind count: 1
// Monadic bind.
// Returns the error if the result is an error.
// Otherwise return the result of applying
// the function to the ok value of the result.
template <typename Ok, typename Error, typename F>
auto and_then_result(F f, const result<Ok, Error>& r)
{
    internal::trigger_static_asserts<internal::unary_function_tag, F, Ok>();

    using FOut = std::decay_t<internal::invoke_result_t<F, Ok>>;
    static_assert(std::is_same<Error, typename FOut::error_t>::value,
                  "Error type must stay the same.");
    if (is_ok(r))
        return internal::invoke(f, unsafe_get_ok(r));
    else
        return error<typename FOut::ok_t, typename FOut::error_t>(r.unsafe_get_error());
}

// API search type: compose_result : ((a -> Result b c), (b -> Result d c)) -> (a -> Result d c)
// Left-to-right Kleisli composition of monads.
// It is possible to compose a variadic number of callables.
// The first callable can take a variadic number of parameters.
template <typename... Callables>
auto compose_result(Callables&&... callables)
{
    auto bind_result = [](auto f, auto g) {
        return [f = std::move(f), g = std::move(g)](auto&&... args)
        {
            internal::trigger_static_asserts<internal::check_arity_tag,
                                                 decltype(f),
                                                 decltype(args)...>();
            using FOut = std::decay_t<
                internal::invoke_result_t<decltype(f), decltype(args)...>>;

            internal::trigger_static_asserts<internal::unary_function_tag,
                                                 decltype(g),
                                                 typename FOut::ok_t>();
            using GOut = std::decay_t<
                internal::invoke_result_t<decltype(g), typename FOut::ok_t>>;
            static_assert(std::is_same<typename FOut::error_t,
                                       typename GOut::error_t>::value,
                          "Error type must stay the same.");

            auto resultB =
                internal::invoke(f, std::forward<decltype(args)>(args)...);
            if (is_ok(resultB))
                return internal::invoke(g, unsafe_get_ok(resultB));
            return error<typename GOut::ok_t, typename GOut::error_t>(
                unsafe_get_error(resultB));
        };
    };
    return internal::compose_binary_lift(bind_result,
                                       std::forward<Callables>(callables)...);
}
} // namespace fplus
