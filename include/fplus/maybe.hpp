// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <fplus/function_traits.hpp>

#include <fplus/detail/asserts/maybe.hpp>

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
    bool is_just() const { return is_present_; }
    bool is_nothing() const { return !is_just(); }
    const T& unsafe_get_just() const
    {
        assert(is_just());
        return *reinterpret_cast<const T*>(&mem_[0]);
    }
    T& unsafe_get_just()
    {
        assert(is_just());
        return *reinterpret_cast<T*>(&mem_[0]);
    }
    typedef T type;
#ifdef __GNUC__ // workaround for bug in GCC 4.9
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
    maybe() : is_present_(false), mem_({}) {};
    ~maybe()
    {
        if (is_present_)
        {
            T* ptr = reinterpret_cast<T*>(&mem_[0]);
            ptr->~T();
        }
    }
    maybe(const T& val_just) : is_present_(true), mem_({})
    {
        new (&mem_[0]) T(val_just);
    }
    maybe(const maybe<T>& other) : is_present_(other.is_just()), mem_({})
    {
        if (other.is_just())
            new (&mem_[0]) T(other.unsafe_get_just());
    }
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
    maybe<T>& operator = (const maybe<T>& other)
    {
        is_present_ = other.is_just();
        if (other.is_just())
            new (&mem_[0]) T(other.unsafe_get_just());
        return *this;
    }
private:
    bool is_present_;
    std::array<unsigned char, sizeof(T)> mem_;
};

// API search type: is_just : Maybe a -> Bool
// fwd bind count: 0
// Is not nothing?
template <typename T>
bool is_just(const maybe<T>& maybe)
{
    return maybe.is_just();
}

// API search type: is_nothing : Maybe a -> Bool
// fwd bind count: 0
// Has no value?
template <typename T>
bool is_nothing(const maybe<T>& maybe)
{
    return !is_just(maybe);
}

// API search type: unsafe_get_just : Maybe a -> a
// fwd bind count: 0
// Crashes if maybe is nothing!
template <typename T>
T unsafe_get_just(const maybe<T>& maybe)
{
    return maybe.unsafe_get_just();
}

// API search type: just_with_default : (a, Maybe a) -> a
// fwd bind count: 0
// Get the value from a maybe or the default in case it is nothing.
template <typename T>
T just_with_default(const T& defaultValue, const maybe<T>& maybe)
{
    if (is_just(maybe))
        return unsafe_get_just(maybe);
    return defaultValue;
}

// API search type: throw_on_nothing : (e, Maybe a) -> a
// fwd bind count: 1
// Throw exception if nothing. Return value if just.
template <typename E, typename T>
T throw_on_nothing(const E& e, const maybe<T>& maybe)
{
    if (is_nothing(maybe))
        throw e;
    return unsafe_get_just(maybe);
}

// API search type: just : a -> Maybe a
// fwd bind count: 0
// Wrap a value in a Maybe as a Just.
template <typename T>
maybe<T> just(const T& val)
{
    return val;
}

// API search type: as_just_if : ((a -> bool), a) -> Maybe a
// fwd bind count: 1
// Wrap a value in a Maybe as a Just if the given predicate is fulfilled.
// Otherwise a nothing is returned.
template <typename Pred, typename T>
maybe<T> as_just_if(Pred pred, const T& val)
{
    internal::check_unary_predicate_for_type<Pred, T>();
    if (pred(val))
        return val;
    else
        return {};
}

// API search type: maybe_to_seq : Maybe a -> [a]
// fwd bind count: 0
// Converts a maybe to a sequence.
// singleton_seq(Just 3) == [3]
// singleton_seq(Nothing) == []
template <typename T, typename ContainerOut = std::vector<T>>
ContainerOut maybe_to_seq(const maybe<T>& maybe)
{
    if (is_just(maybe))
        return ContainerOut(1, unsafe_get_just(maybe));
    return {};
}

// API search type: singleton_seq_as_maybe : [a] -> Maybe a
// fwd bind count: 0
// Converts a sequence to a maybe.
// singleton_seq([]) == Nothing
// singleton_seq([3]) == Just 3
// singleton_seq([3,4]) == Nothing
template <typename Container>
maybe<typename Container::value_type>
singleton_seq_as_maybe(const Container& xs)
{
    if (xs.size() == 1)
        return xs.front();
    return {};
}

// API search type: nothing : () -> Maybe a
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

// API search type: lift_maybe : ((a -> b), Maybe a) -> Maybe b
// fwd bind count: 1
// Lifts a function into the maybe functor.
// A function that for example was able to convert and int into a string,
// now can convert a Maybe<int> into a Maybe<string>.
// A nothing remains a nothing, regardless of the conversion.
template <typename F, typename A>
auto lift_maybe(F f, const maybe<A>& m)
{
    (void)detail::trigger_static_asserts<detail::lift_maybe_tag, F, A>();

    using B = std::decay_t<detail::invoke_result_t<F, A>>;
    if (is_just(m))
        return just<B>(detail::invoke(f, unsafe_get_just(m)));
    return nothing<B>();
}

// API search type: lift_maybe_def : (b, (a -> b), Maybe a) -> b
// fwd bind count: 2
// lift_maybe_def takes a default value and a function.
// It returns a function taking a Maybe value.
// This function returns the default value if the Maybe value is nothing.
// Otherwise it applies the function to the value inside the Just
// of the Maybe value and returns the result of this application.
template <typename F, typename A, typename Default>
auto lift_maybe_def(const Default& def, F f, const maybe<A>& m)
{
    (void)detail::trigger_static_asserts<detail::lift_maybe_tag, F, A>();

    using B = std::decay_t<detail::invoke_result_t<F, A>>;
    static_assert(
        std::is_convertible<Default, B>::value,
        "Default value must be convertible to Function's return type");
    if (is_just(m))
        return B(detail::invoke(f, unsafe_get_just(m)));
    return B(def);
}

// API search type: lift_maybe_2 : (((a, b) -> c), Maybe a, Maybe b) -> Maybe c
// fwd bind count: 2
// Lifts a binary function into the maybe functor.
// Applies the function only if both arguments are justs.
// Otherwise returns a nothing.
template <typename F, typename A, typename B>
auto lift_maybe_2(F f, const maybe<A>& m_a, const maybe<B>& m_b)
{
    (void)detail::trigger_static_asserts<detail::lift_maybe_tag, F, A, B>();

    using FOut = std::decay_t<detail::invoke_result_t<F, A, B>>;
    if (is_just(m_a) && is_just(m_b))
    {
        return just<FOut>(
            detail::invoke(f, unsafe_get_just(m_a), unsafe_get_just(m_b)));
    }
    return nothing<FOut>();
}

// API search type: lift_maybe_2_def : (c, ((a, b) -> c), Maybe a, Maybe b) -> c
// fwd bind count: 3
// lift_maybe_2_def takes a default value and a binary function.
// It returns a function taking a two Maybe values.
// This function returns the default value at least one of the
// Maybe values is nothing.
// Otherwise it applies the function to the two values inside the Justs
// and returns the result of this application.
template <typename F,
    typename A = typename std::remove_const<typename std::remove_reference<
        typename utils::function_traits<
            F>::template arg<0>::type>::type>::type,
    typename B = typename std::remove_const<typename std::remove_reference<
        typename utils::function_traits<
            F>::template arg<1>::type>::type>::type,
    typename C = typename std::remove_const<typename std::remove_reference<
        typename std::result_of<F(A, B)>::type>::type>::type>
C lift_maybe_2_def(const C& def, F f,
    const maybe<A>& m_a, const maybe<B>& m_b)
{
    static_assert(utils::function_traits<F>::arity == 2, "Wrong arity.");
    if (is_just(m_a) && is_just(m_b))
        return f(unsafe_get_just(m_a), unsafe_get_just(m_b));
    return def;
}

// API search type: and_then_maybe : ((a -> Maybe b), (Maybe a)) -> Maybe b
// fwd bind count: 1
// Monadic bind.
// Returns nothing if the maybe already is nothing.
// Otherwise return the result of applying
// the function to the just value of the maybe.
template <typename T, typename F,
    typename FIn = typename std::remove_const<typename std::remove_reference<
        typename utils::function_traits<F>::template arg<0>::type>::type>::type,
    typename FOut = typename std::remove_const<typename std::remove_reference<
        typename std::result_of<F(FIn)>::type>::type>::type,
    typename FOutJustT = typename FOut::type>
maybe<FOutJustT> and_then_maybe(F f, const maybe<T>& m)
{
    static_assert(utils::function_traits<F>::arity == 1, "Wrong arity.");
    static_assert(std::is_convertible<T, FIn>::value,
        "Function parameter types do not match.");
    if (is_just(m))
        return f(unsafe_get_just(m));
    else
        return nothing<FOutJustT>();
}

// API search type: compose_maybe : ((a -> Maybe b), (b -> Maybe c)) -> (a -> Maybe c)
// Left-to-right Kleisli composition of monads (2 functions).
// Composes two functions taking a value and returning Maybe.
// If the first function returns a just, the value from the just
// is extracted and shoved into the second function.
// If the first functions returns a nothing, it remains a nothing.
template <typename F, typename G,
    typename FIn = typename std::remove_const<typename std::remove_reference<
        typename utils::function_traits<
            F>::template arg<0>::type>::type>::type,
    typename FOut = typename std::remove_const<typename std::remove_reference<
        typename std::result_of<F(FIn)>::type>::type>::type,
    typename GIn = typename std::remove_const<typename std::remove_reference<
        typename utils::function_traits<G>::template arg<0>::type>::type>::type,
    typename GOut = typename std::remove_const<typename std::remove_reference<
        typename std::result_of<G(GIn)>::type>::type>::type,
    typename T = typename GOut::type>
std::function<maybe<T>(const FIn&)> compose_maybe(F f, G g)
{
    static_assert(utils::function_traits<F>::arity == 1, "Wrong arity.");
    static_assert(utils::function_traits<G>::arity == 1, "Wrong arity.");
    static_assert(std::is_convertible<typename FOut::type,GIn>::value,
        "Function parameter types do not match");
    return [f, g](const FIn& x) -> maybe<T>
    {
        auto maybeB = f(x);
        if (is_just(maybeB))
            return g(unsafe_get_just(maybeB));
        return nothing<T>();
    };
}

// API search type: compose_maybe : ((a -> Maybe b), (b -> Maybe c), (c -> Maybe d)) -> (Maybe a -> Maybe d)
// Left-to-right Kleisli composition of monads (3 functions).
template <typename F, typename G, typename H,
    typename FIn = typename std::remove_const<typename std::remove_reference<
        typename utils::function_traits<F>::template arg<0>::type>::type>::type,
    typename FOut = typename std::remove_const<typename std::remove_reference<
        typename std::result_of<F(FIn)>::type>::type>::type,
    typename GIn = typename std::remove_const<typename std::remove_reference<
        typename utils::function_traits<G>::template arg<0>::type>::type>::type,
    typename GOut = typename std::remove_const<typename std::remove_reference<
        typename std::result_of<G(GIn)>::type>::type>::type,
    typename HIn = typename std::remove_const<typename std::remove_reference<
        typename utils::function_traits<H>::template arg<0>::type>::type>::type,
    typename HOut = typename std::remove_const<typename std::remove_reference<
        typename std::result_of<H(HIn)>::type>::type>::type,
    typename T = typename HOut::type>
std::function<maybe<T>(const FIn&)> compose_maybe(F f, G g, H h)
{
    return compose_maybe(compose_maybe(f, g), h);
}

// API search type: compose_maybe : ((a -> Maybe b), (b -> Maybe c), (c -> Maybe d), (d -> Maybe e)) -> (Maybe a -> Maybe e)
// Left-to-right Kleisli composition of monads (4 functions).
template <typename F, typename G, typename H, typename I,
    typename FIn = typename std::remove_const<typename std::remove_reference<
        typename utils::function_traits<F>::template arg<0>::type>::type>::type,
    typename FOut = typename std::remove_const<typename std::remove_reference<
        typename std::result_of<F(FIn)>::type>::type>::type,
    typename GIn = typename std::remove_const<typename std::remove_reference<
        typename utils::function_traits<G>::template arg<0>::type>::type>::type,
    typename GOut = typename std::remove_const<typename std::remove_reference<
        typename std::result_of<G(GIn)>::type>::type>::type,
    typename HIn = typename std::remove_const<typename std::remove_reference<
        typename utils::function_traits<H>::template arg<0>::type>::type>::type,
    typename HOut = typename std::remove_const<typename std::remove_reference<
        typename std::result_of<H(HIn)>::type>::type>::type,
    typename IIn = typename std::remove_const<typename std::remove_reference<
        typename utils::function_traits<I>::template arg<0>::type>::type>::type,
    typename IOut = typename std::remove_const<typename std::remove_reference<
        typename std::result_of<I(IIn)>::type>::type>::type,
    typename T = typename IOut::type>
std::function<maybe<T>(const FIn&)> compose_maybe(F f, G g, H h, I i)
{
    return compose_maybe(compose_maybe(compose_maybe(f, g), h), i);
}

// API search type: flatten_maybe : (Maybe (Maybe a)) -> Maybe a
// fwd bind count: 0
// Also known as join.
template <typename T>
maybe<T> flatten_maybe(const maybe<maybe<T>>& maybe_maybe)
{
    if (is_nothing(maybe_maybe))
        return nothing<T>();
    return unsafe_get_just(maybe_maybe);
}

} // namespace fplus
