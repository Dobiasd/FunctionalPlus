// Copyright Tobias Hermann 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "function_traits.h"

#include <cassert>
#include <functional>
#include <memory>

namespace fplus
{

// Can hold a value of type T or nothing.
template <typename T>
class Maybe
{
public:
    Maybe() {}
    Maybe(const Maybe<T>& other) : ptr_(other.Get() ? std::make_unique<T>(*other.Get()) : Ptr()) {}
    explicit Maybe(const T& val) : ptr_(std::make_unique<T>(val)) {}
    bool IsJust() const { return static_cast<bool>(Get()); }
    const T& UnsafeGetJust() const { assert(IsJust()); return *Get(); }
    typedef T type;
private:
    typedef std::unique_ptr<T> Ptr;
    const Ptr& Get() const { return ptr_; }
    Ptr ptr_;
};

// Is not nothing?
template <typename T>
bool IsJust(const Maybe<T>& maybe)
{
    return maybe.IsJust();
}

// Has no value?
template <typename T>
bool IsNothing(const Maybe<T>& maybe)
{
    return !IsJust(maybe);
}

// Crashes if maybe is nothing!
template <typename T>
T UnsafeGetJust(const Maybe<T>& maybe)
{
    return maybe.UnsafeGetJust();
}

// Get the value from a maybe or the default in case it is nothing.
template <typename T>
T WithDefault(const T& defaultValue, const Maybe<T>& maybe)
{
    if (IsJust(maybe))
        return UnsafeGetJust(maybe);
    return defaultValue;
}

// Wrap a value in a Maybe as a Just.
template <typename T>
Maybe<T> Just(const T& val)
{
    return Maybe<T>(val);
}

// Construct a nothing of a certain Maybe type.
template <typename T>
Maybe<T> Nothing()
{
    return Maybe<T>();
}

// True if just values are the same or if both are nothing.
template <typename T>
bool operator == (const Maybe<T>& x, const Maybe<T>& y)
{
    if (IsJust(x) && IsJust(y))
        return UnsafeGetJust(x) == UnsafeGetJust(y);
    return IsJust(x) == IsJust(y);
}

// False if just values are the same or if both are nothing.
template <typename T>
bool operator != (const Maybe<T>& x, const Maybe<T>& y)
{
    return !(x == y);
}

// Lifts a function into the maybe functor.
// A function that for example was able to convert and int into a string,
// now can convert a Maybe<int> into a Maybe<string>.
// A nothings stays a nothing, regardless of the conversion.
template <typename F,
    typename A = typename std::remove_const_t<typename std::remove_reference_t<typename utils::function_traits<F>::template arg<0>::type>>,
    typename B = typename std::remove_const_t<typename std::remove_reference_t<typename utils::function_traits<F>::result_type>>>
std::function<Maybe<B>(const Maybe<A>&)> Lift(F f)
{
    static_assert(utils::function_traits<F>::arity == 1, "Wrong arity.");
    return [f](const Maybe<A>& maybe)
    {
        if (IsJust(maybe))
            return Maybe<B>(f(UnsafeGetJust(maybe)));
        return Nothing<B>();
    };
}

// Monadic bind.
// Composes two functions taking a value and returning Maybe.
// If the first function returns a just, the value from the just
// is extracted and shoved into the second function.
// If the first functions returns a nothing, it stays a nothing.
template <typename F, typename G,
    typename FIn = typename std::remove_const_t<typename std::remove_reference_t<typename utils::function_traits<F>::template arg<0>::type>>,
    typename FOut = typename std::remove_const_t<typename std::remove_reference_t<typename utils::function_traits<F>::result_type>>,
    typename GIn = typename std::remove_const_t<typename std::remove_reference_t<typename utils::function_traits<G>::template arg<0>::type>>,
    typename GOut = typename std::remove_const_t<typename std::remove_reference_t<typename utils::function_traits<G>::result_type>>,
    typename C = typename GOut::type>
std::function<Maybe<C>(const FIn&)> AndThen(F f, G g) {
    static_assert(utils::function_traits<F>::arity == 1, "Wrong arity.");
    static_assert(utils::function_traits<G>::arity == 1, "Wrong arity.");
    static_assert(std::is_convertible<typename FOut::type,GIn>::value, "Function parameter types do not match");
    return [f, g](const FIn& x)
    {
        auto maybeB = f(x);
        if (IsJust(maybeB))
            return g(UnsafeGetJust(maybeB));
        return Nothing<C>();
    };
}

} // namespace fplus