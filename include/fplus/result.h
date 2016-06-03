// Copyright Tobias Hermann 2015.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "function_traits.h"
#include "maybe.h"

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
    result() {}
    typedef std::unique_ptr<Ok> ptr_ok;
    typedef std::unique_ptr<Error> ptr_error;
    friend result<Ok, Error> ok<Ok, Error>(const Ok& ok);
    friend result<Ok, Error> error<Ok, Error>(const Error& error);
    ptr_ok ptr_ok_;
    ptr_error ptr_error_;
};

// API search type: is_ok : Result a b -> Bool
// Is not error?
template <typename Ok, typename Error>
bool is_ok(const result<Ok, Error>& result)
{
    return result.is_ok();
}

// API search type: is_error : Result a b -> Bool
// Is not OK?
template <typename Ok, typename Error>
bool is_error(const result<Ok, Error>& result)
{
    return !is_ok(result);
}

// API search type: unsafe_get_ok : Result a b -> a
// Crashes if result is error!
template <typename Ok, typename Error>
Ok unsafe_get_ok(const result<Ok, Error>& result)
{
    return result.unsafe_get_ok();
}

// API search type: unsafe_get_error : Result a b -> b
// Crashes if result is ok!
template <typename Ok, typename Error>
Error unsafe_get_error(const result<Ok, Error>& result)
{
    return result.unsafe_get_error();
}

// API search type: ok_with_default : (a, Result a b) -> a
// Get the value from a result or the default in case it is error.
template <typename Ok, typename Error>
Ok ok_with_default(const Ok& defaultValue, const result<Ok, Error>& result)
{
    if (is_ok(result))
        return unsafe_get_ok(result);
    return defaultValue;
}

// API search type: ok : a -> Result a b
// Wrap a value in a result as a Ok.
template <typename Ok, typename Error>
result<Ok, Error> ok(const Ok& val)
{
    result<Ok, Error> x;
    x.ptr_ok_.reset(new Ok(val));
    return x;
}

// API search type: error : b -> Result a b
// Construct an error of a certain result type.
template <typename Ok, typename Error>
result<Ok, Error> error(const Error& error)
{
    result<Ok, Error> x;
    x.ptr_error_.reset(new Error(error));
    return x;
}

// API search type: to_maybe : Result a b -> Maybe a
// Convert ok to just, error to nothing.
template <typename Ok, typename Error>
maybe<Ok> to_maybe(const result<Ok, Error>& result)
{
    if (is_ok(result))
        return just<Ok>(unsafe_get_ok(result));
    else
        return nothing<Ok>();
}

// API search type: from_maybe : (Maybe a, b) -> Result a b
// Convert just to ok, nothing to error.
template <typename Ok, typename Error>
result<Ok, Error> from_maybe(const maybe<Ok>& maybe, const Error& err)
{
    if (is_just(maybe))
        return ok<Ok, Error>(unsafe_get_just(maybe));
    else
        return error<Ok, Error>(err);
}

// API search type: throw_on_error : (e, Result a b) -> a
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

// API search type: lift_result : (a -> b) -> (Result a c -> Result b c)
// Lifts a function into the result functor.
// A function that for example was able to convert and int into a string,
// now can convert a result<int> into a result<string>.
// An errors stays the same error, regardless of the conversion.
template <typename Error,
    typename F,
    typename A = typename std::remove_const<typename std::remove_reference<
        typename utils::function_traits<F>::template arg<0>::type>::type>::type,
    typename B = typename std::remove_const<typename std::remove_reference<
        typename utils::function_traits<F>::result_type>::type>::type>
std::function<result<B, Error>(const result<A, Error>&)> lift_result(F f)
{
    static_assert(utils::function_traits<F>::arity == 1, "Wrong arity.");
    return [f](const result<A, Error>& r)
    {
        if (is_ok(r))
            return ok<B, Error>(f(unsafe_get_ok(r)));
        return error<B, Error>(unsafe_get_error(r));
    };
}

// API search type: and_then_result : ((a -> Result b c), (b -> Result d c)) -> (a -> Result d c)
// Monadic bind.
// Composes two functions taking a value and returning result.
// If the first function returns a ok, the value from the ok
// is extracted and shoved into the second function.
// If the first functions returns an error, the error is forwarded.
template <typename F, typename G,
    typename FIn = typename std::remove_const<typename std::remove_reference<
        typename utils::function_traits<F>::template arg<0>::type>::type>::type,
    typename FOut = typename std::remove_const<typename std::remove_reference<
        typename utils::function_traits<F>::result_type>::type>::type,
    typename GIn = typename std::remove_const<typename std::remove_reference<
        typename utils::function_traits<G>::template arg<0>::type>::type>::type,
    typename GOut = typename std::remove_const<typename std::remove_reference<
        typename utils::function_traits<G>::result_type>::type>::type,
    typename Ok = typename GOut::ok_t,
    typename Error = typename GOut::error_t>
std::function<result<Ok, Error>(const FIn&)> and_then_result(F f, G g)
{
    static_assert(utils::function_traits<F>::arity == 1, "Wrong arity.");
    static_assert(utils::function_traits<G>::arity == 1, "Wrong arity.");
    static_assert(std::is_convertible<typename FOut::ok_t,GIn>::value,
        "Function parameter types do not match");
    return [f, g](const FIn& x)
    {
        auto resultB = f(x);
        if (is_ok(resultB))
            return g(unsafe_get_ok(resultB));
        return error<Ok, Error>(unsafe_get_error(resultB));
    };
}

// API search type: and_then_result : ((a -> Result b c), (b -> Result d c), (d -> Result e c)) -> (a -> Result e c)
// Monadic bind three functions.
template <typename F, typename G, typename H,
    typename FIn = typename std::remove_const<typename std::remove_reference<
        typename utils::function_traits<F>::template arg<0>::type>::type>::type,
    typename HOut = typename std::remove_const<typename std::remove_reference<
        typename utils::function_traits<H>::result_type>::type>::type,
    typename Ok = typename HOut::ok_t,
    typename Error = typename HOut::error_t>
std::function<result<Ok, Error>(const FIn&)> and_then_result(F f, G g, H h)
{
    return and_then_result(and_then_result(f, g), h);
}

// API search type: and_then_result : ((a -> Result b c), (b -> Result d c), (d -> Result e c), (e -> Result f c)) -> (a -> Result f c)
// Monadic bind four functions.
template <typename F, typename G, typename H, typename I,
    typename FIn = typename std::remove_const<typename std::remove_reference<
        typename utils::function_traits<F>::template arg<0>::type>::type>::type,
    typename IOut = typename std::remove_const<typename std::remove_reference<
        typename utils::function_traits<I>::result_type>::type>::type,
    typename Ok = typename IOut::ok_t,
    typename Error = typename IOut::error_t>
std::function<result<Ok, Error>(const FIn&)>
and_then_result(F f, G g, H h, I i)
{
    return and_then_result(and_then_result(and_then_result(f, g), h), i);
}

} // namespace fplus
