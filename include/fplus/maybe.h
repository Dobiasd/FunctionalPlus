// Copyright Tobias Hermann 2015.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "function_traits.h"

#include <cassert>
#include <exception>
#include <functional>
#include <memory>

namespace fplus
{

// Can hold a value of type T or nothing.
template <typename T>
class maybe
{
public:
    bool is_just() const { return static_cast<bool>(ptr_); }
    bool is_nothing() const { return !is_just(); }
    const T& unsafe_get_just() const { assert(is_just()); return *ptr_; }
    typedef T type;

    maybe() {};
    maybe(const T& val_just) :
        ptr_(new T(val_just))
    {}
    maybe(const maybe<T>& other) :
        ptr_(other.is_just() ? ptr_t(new T(other.unsafe_get_just())) : ptr_t())
    {}
    maybe<T>& operator = (const maybe<T>& other)
    {
        ptr_ = other.is_just() ? ptr_t(new T(other.unsafe_get_just())) : ptr_t();
        return *this;
    }
private:
    typedef std::unique_ptr<T> ptr_t;
    ptr_t ptr_;
};

// API search type: is_just : maybe a -> bool
// Is not nothing?
template <typename T>
bool is_just(const maybe<T>& maybe)
{
    return maybe.is_just();
}

// API search type: is_nothing : maybe a -> bool
// Has no value?
template <typename T>
bool is_nothing(const maybe<T>& maybe)
{
    return !is_just(maybe);
}

// API search type: unsafe_get_just : maybe a -> a
// Crashes if maybe is nothing!
template <typename T>
T unsafe_get_just(const maybe<T>& maybe)
{
    return maybe.unsafe_get_just();
}

// API search type: just_with_default : a, maybe a -> a
// Get the value from a maybe or the default in case it is nothing.
template <typename T>
T just_with_default(const T& defaultValue, const maybe<T>& maybe)
{
    if (is_just(maybe))
        return unsafe_get_just(maybe);
    return defaultValue;
}

// API search type: throw_on_nothing : e, maybe a -> a
// Throw exception if nothing. Return value if just.
template <typename E, typename T>
T throw_on_nothing(const E& e, const maybe<T>& maybe)
{
    if (is_nothing(maybe))
        throw e;
    return unsafe_get_just(maybe);
}

// API search type: just : a -> maybe a
// Wrap a value in a Maybe as a Just.
template <typename T>
maybe<T> just(const T& val)
{
    return val;
}

// API search type: nothing : () -> maybe a
// Construct a nothing of a certain Maybe type.
template <typename T>
maybe<T> nothing()
{
    return {};
}

// True if just values are the same or if both are nothing.
template <typename T>
bool operator == (const maybe<T>& x, const maybe<T>& y)
{
    if (is_just(x) && is_just(y))
        return unsafe_get_just(x) == unsafe_get_just(y);
    return is_just(x) == is_just(y);
}

// False if just values are the same or if both are nothing.
template <typename T>
bool operator != (const maybe<T>& x, const maybe<T>& y)
{
    return !(x == y);
}

// API search type: lift_maybe : (a -> b) -> (maybe a -> maybe b)
// Lifts a function into the maybe functor.
// A function that for example was able to convert and int into a string,
// now can convert a Maybe<int> into a Maybe<string>.
// A nothings stays a nothing, regardless of the conversion.
template <typename F,
    typename A = typename std::remove_const<typename std::remove_reference<typename utils::function_traits<F>::template arg<0>::type>::type>::type,
    typename B = typename std::remove_const<typename std::remove_reference<typename utils::function_traits<F>::result_type>::type>::type>
std::function<maybe<B>(const maybe<A>&)> lift_maybe(F f)
{
    static_assert(utils::function_traits<F>::arity == 1, "Wrong arity.");
    return [f](const maybe<A>& m)
    {
        if (is_just(m))
            return just<B>(f(unsafe_get_just(m)));
        return nothing<B>();
    };
}

// API search type: and_then_maybe : (a -> maybe b), (b -> maybe c) -> (a -> maybe c)
// Monadic bind.
// Composes two functions taking a value and returning Maybe.
// If the first function returns a just, the value from the just
// is extracted and shoved into the second function.
// If the first functions returns a nothing, it stays a nothing.
template <typename F, typename G,
    typename FIn = typename std::remove_const<typename std::remove_reference<typename utils::function_traits<F>::template arg<0>::type>::type>::type,
    typename FOut = typename std::remove_const<typename std::remove_reference<typename utils::function_traits<F>::result_type>::type>::type,
    typename GIn = typename std::remove_const<typename std::remove_reference<typename utils::function_traits<G>::template arg<0>::type>::type>::type,
    typename GOut = typename std::remove_const<typename std::remove_reference<typename utils::function_traits<G>::result_type>::type>::type,
    typename T = typename GOut::type>
std::function<maybe<T>(const FIn&)> and_then_maybe(F f, G g)
{
    static_assert(utils::function_traits<F>::arity == 1, "Wrong arity.");
    static_assert(utils::function_traits<G>::arity == 1, "Wrong arity.");
    static_assert(std::is_convertible<typename FOut::type,GIn>::value, "Function parameter types do not match");
    return [f, g](const FIn& x)
    {
        auto maybeB = f(x);
        if (is_just(maybeB))
            return g(unsafe_get_just(maybeB));
        return nothing<T>();
    };
}

// API search type: and_then_maybe : (a -> maybe b), (b -> maybe c), (c -> maybe d) -> (maybe a -> maybe d)
// Monadic bind.
template <typename F, typename G, typename H,
    typename FIn = typename std::remove_const<typename std::remove_reference<typename utils::function_traits<F>::template arg<0>::type>::type>::type,
    typename HOut = typename std::remove_const<typename std::remove_reference<typename utils::function_traits<H>::result_type>::type>::type,
    typename T = typename HOut::type>
std::function<maybe<T>(const FIn&)> and_then_maybe(F f, G g, H h)
{
    return and_then_maybe(and_then_maybe(f, g), h);
}

// API search type: and_then_maybe : (a -> maybe b), (b -> maybe c), (c -> maybe d), (d -> maybe e) -> (maybe a -> maybe e)
// Monadic bind.
template <typename F, typename G, typename H, typename I,
    typename FIn = typename std::remove_const<typename std::remove_reference<typename utils::function_traits<F>::template arg<0>::type>::type>::type,
    typename IOut = typename std::remove_const<typename std::remove_reference<typename utils::function_traits<I>::result_type>::type>::type,
    typename T = typename IOut::type>
std::function<maybe<T>(const FIn&)> and_then_maybe(F f, G g, H h, I i)
{
    return and_then_maybe(and_then_maybe(and_then_maybe(f, g), h), i);
}

} // namespace fplus
