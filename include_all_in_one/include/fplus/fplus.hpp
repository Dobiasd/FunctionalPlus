#pragma once

//
// fplus.hpp
//

// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)



//
// compare.hpp
//

// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)



//
// function_traits.hpp
//

//--------------------------------------
// utils/traits: Additional type traits
//--------------------------------------
//
//          Copyright kennytm (auraHT Ltd.) 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

/**

``<utils/traits.hpp>`` --- Additional type traits
=================================================

This module provides additional type traits and related functions, missing from
the standard library.

*/

#ifndef TRAITS_HPP_9ALQFEFX7TO
#define TRAITS_HPP_9ALQFEFX7TO 1

#include <cstdlib>
#include <tuple>
#include <functional>
#include <type_traits>


//
// internal/meta.hpp
//

// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)


#include <type_traits>

namespace fplus
{
namespace internal
{
// C++14 compatible void_t (http://en.cppreference.com/w/cpp/types/void_t)
template <typename... Ts>
struct make_void
{
  using type = void;
};

template <typename... Ts>
using void_t = typename make_void<Ts...>::type;

// Sometimes you don't want to use std::decay_t, and the temptation of short
// writing can be huge...
template <typename T>
using uncvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

// disjunction/conjunction/negation, useful to short circuit SFINAE checks
// Use with parsimony, MSVC 2015 can have ICEs quite easily
template <typename...>
struct disjunction : std::false_type
{
};

template <typename B1>
struct disjunction<B1> : B1
{
};

template <typename B1, typename... Bn>
struct disjunction<B1, Bn...>
    : std::conditional<bool(B1::value), B1, disjunction<Bn...>>::type
{
};

template <typename...>
struct conjunction : std::true_type
{
};

template <typename B1>
struct conjunction<B1> : B1
{
};

template <typename B1, typename... Bn>
struct conjunction<B1, Bn...>
    : std::conditional<bool(B1::value), conjunction<Bn...>, B1>::type
{
};

template <typename B>
struct negation : std::integral_constant<bool, !bool(B::value)>
{
};

// non short-circuiting meta functions
// source: https://stackoverflow.com/a/27221517/4116453
template <bool...>
struct bool_pack;

template <bool... Values>
struct all_of
    : std::is_same<bool_pack<Values..., true>, bool_pack<true, Values...>>
{
};

// there seems to be a bug in libc++'s std::is_function
// provide our own (cppreference one)
// (the MSVC implementation seems correct)
#ifndef _MSC_VER
#define PROVIDE_IS_FUNCTION_POLYFILL
#endif

#ifndef PROVIDE_IS_FUNCTION_POLYFILL
template<class... Any>
using is_function = std::is_function<Any...>;
#else //PROVIDE_IS_FUNCTION_POLYFILL
// primary template
template<class>
struct is_function : std::false_type { };

// specialization for regular functions
template<class Ret, class... Args>
struct is_function<Ret(Args...)> : std::true_type {};

// specialization for variadic functions such as std::printf
template<class Ret, class... Args>
struct is_function<Ret(Args...,...)> : std::true_type {};

// specialization for function types that have cv-qualifiers
template<class Ret, class... Args>
struct is_function<Ret(Args...) const> : std::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...) volatile> : std::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...) const volatile> : std::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...,...) const> : std::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...,...) volatile> : std::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...,...) const volatile> : std::true_type {};

// specialization for function types that have ref-qualifiers
template<class Ret, class... Args>
struct is_function<Ret(Args...) &> : std::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...) const &> : std::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...) volatile &> : std::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...) const volatile &> : std::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...,...) &> : std::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...,...) const &> : std::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...,...) volatile &> : std::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...,...) const volatile &> : std::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...) &&> : std::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...) const &&> : std::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...) volatile &&> : std::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...) const volatile &&> : std::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...,...) &&> : std::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...,...) const &&> : std::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...,...) volatile &&> : std::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...,...) const volatile &&> : std::true_type {};
#endif //PROVIDE_IS_FUNCTION_POLYFILL

template <typename>
struct reverse_integer_sequence_impl;

template <typename T>
struct reverse_integer_sequence_impl<std::integer_sequence<T>>
    : std::integer_sequence<T>
{
};

template <typename T, T... Ints>
struct reverse_integer_sequence_impl<std::integer_sequence<T, Ints...>>
    : std::integer_sequence<T, sizeof...(Ints) - 1 - Ints...>
{
};

template <typename Seq>
using reverse_integer_sequence = reverse_integer_sequence_impl<Seq>;

template <typename T, T N>
using make_reverse_integer_sequence =
    reverse_integer_sequence<std::make_integer_sequence<T, N>>;

template <std::size_t... Idx>
using reverse_index_sequence =
    reverse_integer_sequence<std::index_sequence<Idx...>>;

template <std::size_t N>
using make_reverse_index_sequence =
    make_reverse_integer_sequence<std::size_t, N>;
}
}

namespace fplus {

// source: https://github.com/kennytm/utils
namespace utils {

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
#endif

/**
.. macro:: DECLARE_HAS_TYPE_MEMBER(member_name)

    This macro declares a template ``has_member_name`` which will check whether
    a type member ``member_name`` exists in a particular type.

    Example::

        DECLARE_HAS_TYPE_MEMBER(result_type)

        ...

        printf("%d\n", has_result_type< std::plus<int> >::value);
        // ^ prints '1' (true)
        printf("%d\n", has_result_type< double(*)() >::value);
        // ^ prints '0' (false)
*/
#define DECLARE_HAS_TYPE_MEMBER(member_name) \
    template <typename, typename = void> \
    struct has_##member_name \
    { enum { value = false }; }; \
    template <typename T> \
    struct has_##member_name<T, typename std::enable_if<sizeof(typename T::member_name)||true>::type> \
    { enum { value = true }; };

/**
.. type:: struct utils::function_traits<F>

    Obtain compile-time information about a function object *F*.

    This template currently supports the following types:

    * Normal function types (``R(T...)``), function pointers (``R(*)(T...)``)
      and function references (``R(&)(T...)`` and ``R(&&)(T...)``).
    * Member functions (``R(C::*)(T...)``)
    * ``std::function<F>``
    * Type of lambda functions, and any other types that has a unique
      ``operator()``.
    * Type of ``std::mem_fn`` (only for GCC's libstdc++ and LLVM's libc++).
      Following the C++ spec, the first argument will be a raw pointer.
*/
template <typename T>
struct function_traits
    : public function_traits<decltype(&T::operator())>
{};

namespace xx_impl
{
    template <typename C, typename R, typename... A>
    struct memfn_type
    {
        typedef typename std::conditional<
            std::is_const<C>::value,
            typename std::conditional<
                std::is_volatile<C>::value,
                R (C::*)(A...) const volatile,
                R (C::*)(A...) const
            >::type,
            typename std::conditional<
                std::is_volatile<C>::value,
                R (C::*)(A...) volatile,
                R (C::*)(A...)
            >::type
        >::type type;
    };
}

template <typename ReturnType, typename... Args>
struct function_traits<ReturnType(Args...)>
{
    /**
    .. type:: type result_type

        The type returned by calling an instance of the function object type *F*.
    */
    typedef ReturnType result_type;

    /**
    .. type:: type function_type

        The function type (``R(T...)``).
    */
    typedef ReturnType function_type(Args...);

    /**
    .. type:: type member_function_type<OwnerType>

        The member function type for an *OwnerType* (``R(OwnerType::*)(T...)``).
    */
    template <typename OwnerType>
    using member_function_type = typename xx_impl::memfn_type<
        typename std::remove_pointer<typename std::remove_reference<OwnerType>::type>::type,
        ReturnType, Args...
    >::type;

    /**
    .. data:: static const size_t arity

        Number of arguments the function object will take.
    */
    enum { arity = sizeof...(Args) };

    /**
    .. type:: type arg<n>::type

        The type of the *n*-th argument.
    */
    template <size_t i>
    struct arg
    {
        typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
    };
};

template <typename ReturnType, typename... Args>
struct function_traits<ReturnType(*)(Args...)>
    : public function_traits<ReturnType(Args...)>
{};

template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits<ReturnType(ClassType::*)(Args...)>
    : public function_traits<ReturnType(Args...)>
{
    typedef ClassType& owner_type;
};

template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits<ReturnType(ClassType::*)(Args...) const>
    : public function_traits<ReturnType(Args...)>
{
    typedef const ClassType& owner_type;
};

template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits<ReturnType(ClassType::*)(Args...) volatile>
    : public function_traits<ReturnType(Args...)>
{
    typedef volatile ClassType& owner_type;
};

template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits<ReturnType(ClassType::*)(Args...) const volatile>
    : public function_traits<ReturnType(Args...)>
{
    typedef const volatile ClassType& owner_type;
};

template <typename FunctionType>
struct function_traits<std::function<FunctionType>>
    : public function_traits<FunctionType>
{};

#if defined(_GLIBCXX_FUNCTIONAL)
#define MEM_FN_SYMBOL_XX0SL7G4Z0J std::_Mem_fn
#elif defined(_LIBCPP_FUNCTIONAL)
#define MEM_FN_SYMBOL_XX0SL7G4Z0J std::__mem_fn
#endif

#ifdef MEM_FN_SYMBOL_XX0SL7G4Z0J

template <typename R, typename C>
struct function_traits<MEM_FN_SYMBOL_XX0SL7G4Z0J<R C::*>>
    : public function_traits<R(C*)>
{};
template <typename R, typename C, typename... A>
struct function_traits<MEM_FN_SYMBOL_XX0SL7G4Z0J<R(C::*)(A...)>>
    : public function_traits<R(C*, A...)>
{};
template <typename R, typename C, typename... A>
struct function_traits<MEM_FN_SYMBOL_XX0SL7G4Z0J<R(C::*)(A...) const>>
    : public function_traits<R(const C*, A...)>
{};
template <typename R, typename C, typename... A>
struct function_traits<MEM_FN_SYMBOL_XX0SL7G4Z0J<R(C::*)(A...) volatile>>
    : public function_traits<R(volatile C*, A...)>
{};
template <typename R, typename C, typename... A>
struct function_traits<MEM_FN_SYMBOL_XX0SL7G4Z0J<R(C::*)(A...) const volatile>>
    : public function_traits<R(const volatile C*, A...)>
{};

#undef MEM_FN_SYMBOL_XX0SL7G4Z0J
#endif

template <typename T>
struct function_traits<T&> : public function_traits<T> {};
template <typename T>
struct function_traits<const T&> : public function_traits<T> {};
template <typename T>
struct function_traits<volatile T&> : public function_traits<T> {};
template <typename T>
struct function_traits<const volatile T&> : public function_traits<T> {};
template <typename T>
struct function_traits<T&&> : public function_traits<T> {};
template <typename T>
struct function_traits<const T&&> : public function_traits<T> {};
template <typename T>
struct function_traits<volatile T&&> : public function_traits<T> {};
template <typename T>
struct function_traits<const volatile T&&> : public function_traits<T> {};


#define FORWARD_RES_8QR485JMSBT \
    typename std::conditional< \
        std::is_lvalue_reference<R>::value, \
        T&, \
        typename std::remove_reference<T>::type&& \
    >::type

/**
.. function:: auto utils::forward_like<Like, T>(T&& t) noexcept

    Forward the reference *t* like the type of *Like*. That means, if *Like* is
    an lvalue (reference), this function will return an lvalue reference of *t*.
    Otherwise, if *Like* is an rvalue, this function will return an rvalue
    reference of *t*.

    This is mainly used to propagate the expression category (lvalue/rvalue) of
    a member of *Like*, generalizing ``std::forward``.
*/
template <typename R, typename T>
FORWARD_RES_8QR485JMSBT forward_like(T&& input) noexcept
{
    return static_cast<FORWARD_RES_8QR485JMSBT>(input);
}

#undef FORWARD_RES_8QR485JMSBT

/**
.. type:: struct utils::copy_cv<From, To>

    Copy the CV qualifier between the two types. For example,
    ``utils::copy_cv<const int, double>::type`` will become ``const double``.
*/
template <typename From, typename To>
struct copy_cv
{
private:
    typedef typename std::remove_cv<To>::type raw_To;
    typedef typename std::conditional<std::is_const<From>::value,
                                      const raw_To, raw_To>::type const_raw_To;
public:
    /**
    .. type:: type type

        Result of cv-copying.
    */
    typedef typename std::conditional<std::is_volatile<From>::value,
                                      volatile const_raw_To, const_raw_To>::type type;
};

/**
.. type:: struct utils::pointee<T>

    Returns the type by derefering an instance of *T*. This is a generalization
    of ``std::remove_pointer``, that it also works with iterators.
*/
template <typename T>
struct pointee
{
    /**
    .. type:: type type

        Result of dereferencing.
    */
    typedef typename std::remove_reference<decltype(*std::declval<T>())>::type type;
};

/**
.. function:: std::add_rvalue_reference<T>::type utils::rt_val<T>() noexcept

    Returns a value of type *T*. It is guaranteed to do nothing and will not
    throw a compile-time error, but using the returned result will cause
    undefined behavior.
*/
template <typename T>
typename std::add_rvalue_reference<T>::type rt_val() noexcept
{
    return std::move(*static_cast<T*>(nullptr));
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

}
}

namespace fplus
{
namespace internal
{
template <typename>
struct is_std_function : std::false_type
{
};

template <typename T>
struct is_std_function<std::function<T>> : std::true_type
{
};

// Those traits are needed to not perform arity checks on a generic-lambd
// or a templated/overloaded operator()
template <typename T, typename = void>
struct has_function_traits : std::false_type
{
};

// There is a bug with GCC 7 when a std::function is passed as T.
// It produces an ambiguous call between this one and the std::function overload
// It's related to our void_t implementation, the C++14 compatible version does not
// work, whereas the C++17 one does...
//
// So, help GCC a bit with is_std_function
template <typename T>
struct has_function_traits<T,
                           std::enable_if_t<!is_std_function<T>::value,
                                            void_t<decltype(&T::operator())>>>
    : std::true_type
{
};

template <typename ReturnType, typename... Args>
struct has_function_traits<ReturnType(Args...)> : std::true_type
{
};

template <typename ReturnType, typename... Args>
struct has_function_traits<ReturnType (*)(Args...)> : std::true_type
{
};

template <typename ReturnType, typename ClassType, typename... Args>
struct has_function_traits<ReturnType (ClassType::*)(Args...)> : std::true_type
{
};

template <typename ReturnType, typename ClassType, typename... Args>
struct has_function_traits<ReturnType (ClassType::*)(Args...) const>
    : std::true_type
{
};

template <typename ReturnType, typename ClassType, typename... Args>
struct has_function_traits<ReturnType (ClassType::*)(Args...) volatile>
    : std::true_type
{
};

template <typename ReturnType, typename ClassType, typename... Args>
struct has_function_traits<ReturnType (ClassType::*)(Args...) const volatile>
    : std::true_type
{
};

template <typename ReturnType, typename ClassType, typename... Args>
struct has_function_traits<ReturnType (ClassType::*)(Args...)&> : std::true_type
{
};

template <typename ReturnType, typename ClassType, typename... Args>
struct has_function_traits<ReturnType (ClassType::*)(Args...) const &>
    : std::true_type
{
};

template <typename ReturnType, typename ClassType, typename... Args>
struct has_function_traits<ReturnType (ClassType::*)(Args...) volatile&>
    : std::true_type
{
};

template <typename ReturnType, typename ClassType, typename... Args>
struct has_function_traits<ReturnType (ClassType::*)(Args...) const volatile&>
    : std::true_type
{
};

template <typename ReturnType, typename ClassType, typename... Args>
struct has_function_traits<ReturnType (ClassType::*)(Args...) &&>
    : std::true_type
{
};

template <typename ReturnType, typename ClassType, typename... Args>
struct has_function_traits<ReturnType (ClassType::*)(Args...) const &&>
    : std::true_type
{
};

template <typename ReturnType, typename ClassType, typename... Args>
struct has_function_traits<ReturnType (ClassType::*)(Args...) volatile&&>
    : std::true_type
{
};

template <typename ReturnType, typename ClassType, typename... Args>
struct has_function_traits<ReturnType (ClassType::*)(Args...) const volatile&&>
    : std::true_type
{
};

template <typename FunctionType>
struct has_function_traits<std::function<FunctionType>> : std::true_type
{
};
}
}

#endif

//
// composition.hpp
//

// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)



//
// internal/apply.hpp
//

// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)


#include <tuple>
#include <type_traits>
#include <utility>


//
// internal/invoke.hpp
//

// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)


#include <functional>
#include <type_traits>
#include <utility>


// borrowed to libc++
#define FPLUS_INVOKE_RETURN(...) \
  ->decltype(__VA_ARGS__)        \
  {                              \
    return __VA_ARGS__;          \
  }

namespace fplus
{
namespace internal
{
// We need std::invoke to detect callable objects
//
// source:
// http://en.cppreference.com/mwiki/index.php?title=cpp/utility/functional/invoke&oldid=82514
template <typename U>
static std::true_type is_refwrap_test(const std::reference_wrapper<U>&);

template <typename U>
static std::false_type is_refwrap_test(const U&);

template <typename T>
struct is_reference_wrapper : decltype(is_refwrap_test(std::declval<T>()))
{
};

template <typename T, typename U = typename std::decay<T>::type>
struct unwrap_reference_wrapper
{
  using type = T;
};

template <typename T, typename U>
struct unwrap_reference_wrapper<T, std::reference_wrapper<U>>
{
  using type = U&;
};

template <typename T>
using unwrap_reference_wrapper_t = typename unwrap_reference_wrapper<T>::type;

// note: clang only triggers the second static_assert
//      - static_assert(is_invocable<&base_class::non_const_method, const derived_class&>::value, "");
//      - static_assert(is_invocable<&base_class::non_const_method, const base_class&>::value, "");
// GCC triggers both. To workaround this clang bug, we have to manage cv correctness ourselves

template <typename T>
struct is_const_member_function : std::false_type
{
};

// decay doesn't add pointer to abominable functions, don't bother writing them
template <typename R, typename... Args>
struct is_const_member_function<R(Args...) const> : std::true_type
{
};

template <typename R, typename... Args>
struct is_const_member_function<R(Args...) const&> : std::true_type
{
};

template <typename R, typename... Args>
struct is_const_member_function<R(Args...) const&&> : std::true_type
{
};

template <typename R, typename... Args>
struct is_const_member_function<R(Args...) const volatile> : std::true_type
{
};

template <typename R, typename... Args>
struct is_const_member_function<R(Args...) const volatile&> : std::true_type
{
};

template <typename R, typename... Args>
struct is_const_member_function<R(Args...) const volatile&&> : std::true_type
{
};

template <typename T>
struct is_volatile_member_function : std::false_type
{
};

// decay doesn't add pointer to abominable functions, don't bother writing them
template <typename R, typename... Args>
struct is_volatile_member_function<R(Args...) volatile> : std::true_type
{
};

template <typename R, typename... Args>
struct is_volatile_member_function<R(Args...) volatile&> : std::true_type
{
};

template <typename R, typename... Args>
struct is_volatile_member_function<R(Args...) volatile&&> : std::true_type
{
};

template <typename R, typename... Args>
struct is_volatile_member_function<R(Args...) const volatile> : std::true_type
{
};

template <typename R, typename... Args>
struct is_volatile_member_function<R(Args...) const volatile&> : std::true_type
{
};

template <typename R, typename... Args>
struct is_volatile_member_function<R(Args...) const volatile&&> : std::true_type
{
};

template <typename Object, typename Signature>
struct has_correct_cv
{
  // if object has no cv, every method can be called
  // else the method must have the same cv than the object
  static constexpr bool value =
      std::is_same<typename std::remove_cv<Object>::type, Object>::value ||
      ((is_volatile_member_function<Signature>::value ==
        std::is_volatile<Object>::value) &&
       (is_const_member_function<Signature>::value ==
        std::is_const<Object>::value));
};

// pointer to member function - reference to object
template <
    typename Base,
    typename T,
    typename Derived,
    typename... Args,
    typename Unwrapped = unwrap_reference_wrapper_t<Derived>,
    typename std::enable_if<
        is_function<T>::value &&
            has_correct_cv<typename std::remove_reference<Unwrapped>::type, T>::value &&
            std::is_base_of<Base, typename std::decay<Unwrapped>::type>::value,
        int>::type = 0>
inline auto invoke_impl(T Base::*pmf, Derived&& ref, Args&&... args)
    FPLUS_INVOKE_RETURN((std::forward<Unwrapped>(ref).*
                         pmf)(std::forward<Args>(args)...))

// pointer to member function - pointer to object
template <
    typename Base,
    typename T,
    typename Pointer,
    typename... Args,
    typename std::enable_if<
        is_function<T>::value &&
            has_correct_cv<typename std::remove_pointer<
                               typename std::decay<Pointer>::type>::type,
                           T>::value &&
            !std::is_base_of<Base, typename std::decay<Pointer>::type>::value,
        int>::type = 0>
inline auto invoke_impl(T Base::*pmf, Pointer&& ptr, Args&&... args)
    FPLUS_INVOKE_RETURN(((*std::forward<Pointer>(ptr)).*
                         pmf)(std::forward<Args>(args)...))

// pointer to non-static data member - reference to object
template <
    typename Base,
    typename T,
    typename Derived,
    typename Unwrapped = unwrap_reference_wrapper_t<Derived>,
    typename std::enable_if<
        !is_function<T>::value &&
            std::is_base_of<Base, typename std::decay<Unwrapped>::type>::value,
        int>::type = 0>
inline auto invoke_impl(T Base::*pmd, Derived&& ref)
    FPLUS_INVOKE_RETURN((std::forward<Unwrapped>(ref).*pmd))

// pointer to non-static data member - pointer to object
template <
    typename Base,
    typename T,
    typename Pointer,
    typename std::enable_if<
        !is_function<T>::value &&
            !std::is_base_of<Base, typename std::decay<Pointer>::type>::value,
        int>::type = 0>
inline auto invoke_impl(T Base::*pmd, Pointer&& ptr)
    FPLUS_INVOKE_RETURN((*std::forward<Pointer>(ptr)).*pmd)

// normal case - functions, lambdas, function objects
template <typename F,
          typename... Args,
          typename std::enable_if<
              !std::is_member_pointer<typename std::decay<F>::type>::value,
              int>::type = 0>
inline auto invoke_impl(F&& f, Args&&... args)
    FPLUS_INVOKE_RETURN((std::forward<F>(f)(std::forward<Args>(args)...)))

template <typename AlwaysVoid, typename, typename...>
struct invoke_result_impl
{
};

template <typename F, typename... Args>
struct invoke_result_impl<decltype(void(invoke_impl(std::declval<F>(),
                                                    std::declval<Args>()...))),
                          F,
                          Args...>
{
  using type =
      decltype(invoke_impl(std::declval<F>(), std::declval<Args>()...));
};

template <typename F, typename... ArgTypes>
struct invoke_result : invoke_result_impl<void, F, ArgTypes...>
{
};

template <typename F, typename... Args>
using invoke_result_t = typename invoke_result<F, Args...>::type;

// noexcept omitted on purpose, cannot be implemented without C++17.
// GCC 7.1 works with libstdc++, but clang fails, even with latest build,
// on both libstdc++/libc++, I suspect an internal compiler trait is at
// play to make GCC work.
//
// We could detect if C++17 is used and use std::invoke directly.
template <typename F, typename... ArgTypes>
invoke_result_t<F, ArgTypes...> invoke(F&& f, ArgTypes&&... args)
{
  return invoke_impl(std::forward<F>(f), std::forward<ArgTypes>(args)...);
}

// Invoke useful traits (libstdc++ 7.1.0's implementation, ugly-case removed)
template <typename Result, typename ReturnType, typename = void>
struct is_invocable_impl : std::false_type
{
};

template <typename Result, typename ReturnType>
struct is_invocable_impl<Result, ReturnType, void_t<typename Result::type>>
    : disjunction<std::is_void<ReturnType>,
                  std::is_convertible<typename Result::type, ReturnType>>::type
{
};

template <typename F, typename... ArgTypes>
struct is_invocable
    : is_invocable_impl<invoke_result<F, ArgTypes...>, void>::type
{
};

template <typename ReturnType, typename F, typename... ArgTypes>
struct is_invocable_r
    : is_invocable_impl<invoke_result<F, ArgTypes...>, ReturnType>::type
{
};
}
}

#undef FPLUS_INVOKE_RETURN

namespace fplus
{
namespace internal
{
// C++17 std::apply (http://en.cppreference.com/w/cpp/utility/apply)
template <typename F, typename Tuple, std::size_t... I>
constexpr decltype(auto) apply_impl(F&& f, Tuple&& t, std::index_sequence<I...>)
{
    return internal::invoke(std::forward<F>(f),
                          std::get<I>(std::forward<Tuple>(t))...);
}

template <typename F, typename Tuple>
constexpr decltype(auto) apply(F&& f, Tuple&& t)
{
    return internal::apply_impl(
        std::forward<F>(f),
        std::forward<Tuple>(t),
        std::make_index_sequence<
            std::tuple_size<std::decay_t<Tuple>>::value>{});
}
}
}

//
// internal/asserts/functions.hpp
//

// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)



//
// internal/function_traits_asserts.hpp
//

// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)



namespace fplus
{
namespace internal
{
template <typename T, typename...>
struct function_traits_asserts;

template <
    typename,
    typename F,
    typename... Args,
    typename std::enable_if<is_invocable<F, Args...>::value, int>::type = 0>
constexpr void trigger_static_asserts()
{
}

// Marks a variable as unused. Prevents the compiler warning
// for set but unused variables.
template<class T>
inline void unused(T&&) { }

template <typename Tag,
          typename F,
          typename... Args,
          typename std::enable_if<has_function_traits<F>::value &&
                                      !is_invocable<F, Args...>::value,
                                  int>::type = 0>
constexpr void trigger_static_asserts()
{
    // don't perform checks if function_traits<F> doesn't exist
    unused(function_traits_asserts<Tag, F, Args...>{});
}

template <typename,
          typename F,
          typename... Args,
          typename std::enable_if<!has_function_traits<F>::value &&
                                      !is_invocable<F, Args...>::value,
                                  int>::type = 0>
constexpr void trigger_static_asserts()
{
  static_assert(sizeof(F) == 0,
                "F is not a Callable, or its definition is ill-formed");
}
}
}

namespace fplus
{
namespace internal
{

struct nullary_function_tag
{
};

struct unary_function_tag
{
};

struct binary_function_tag
{
};

struct binary_predicate_tag
{
};

struct check_arity_tag
{
};

template <typename F>
struct function_traits_asserts<nullary_function_tag, F>
{
    static_assert(utils::function_traits<F>::arity == 0,
                  "Function must take no parameters.");
};

template <typename F, typename X>
struct function_traits_asserts<unary_function_tag, F, X>
{
    static_assert(utils::function_traits<F>::arity == 1,
                  "Function must take one parameter.");
    typedef typename utils::function_traits<F>::template arg<0>::type FIn0;
    static_assert(std::is_convertible<X, FIn0>::value,
                  "Invalid argument type for function");
};

template <typename F>
struct function_traits_asserts<binary_function_tag, F>
{
    static_assert(utils::function_traits<F>::arity == 2,
        "Function must take two parameters.");
};

template <typename F, typename X, typename Y>
struct function_traits_asserts<binary_function_tag, F, X ,Y>
{
    static_assert(utils::function_traits<F>::arity == 2,
        "Function must take two parameters.");
    typedef typename utils::function_traits<F>::template arg<0>::type FIn0;
    static_assert(std::is_convertible<X, FIn0>::value,
                  "Invalid first argument type for function");
    typedef typename utils::function_traits<F>::template arg<1>::type FIn1;
    static_assert(std::is_convertible<Y, FIn1>::value,
                  "Invalid second argument type for function");
};

template <typename F>
struct function_traits_asserts<binary_predicate_tag, F>
{
    static_assert(utils::function_traits<F>::arity == 2,
        "Function must take two parameters.");
    typedef typename utils::function_traits<F>::template arg<0>::type FIn0;
    typedef typename utils::function_traits<F>::template arg<1>::type FIn1;
    static_assert(std::is_same<FIn0, FIn1>::value,
                  "Both parameters must have the same type.");
    static_assert(std::is_same<std::decay_t<internal::invoke_result_t<F, FIn0, FIn1>>, bool>::value,
                "Predicate must return bool.");
};

template <typename F, typename... Args>
struct function_traits_asserts<check_arity_tag, F, Args...>
{
    static_assert(utils::function_traits<F>::arity == sizeof...(Args),
                  "Wrong arity.");
};

}
}

//
// internal/asserts/composition.hpp
//

// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)



namespace fplus
{
namespace internal
{

struct bind_1st_of_2_tag
{
};

struct bind_2nd_of_2_tag
{
};

struct bind_1st_of_3_tag
{
};

struct bind_1st_and_2nd_of_3_tag
{
};

struct bind_2nd_and_3rd_of_3_tag
{
};

template <typename F, typename X, typename Y>
struct function_traits_asserts<bind_1st_of_2_tag, F, X, Y>
{
    static_assert(utils::function_traits<F>::arity == 2,
                  "Function must take two parameters.");
    typedef typename utils::function_traits<F>::template arg<0>::type FIn0;
    typedef typename utils::function_traits<F>::template arg<1>::type FIn1;
    static_assert(std::is_convertible<X, FIn0>::value,
                  "Function can not take bound parameter type");
    static_assert(std::is_convertible<Y, FIn1>::value,
                  "Function can not take provided parameter type");
};

template <typename F, typename X, typename Y>
struct function_traits_asserts<bind_2nd_of_2_tag, F, X, Y>
{
    static_assert(utils::function_traits<F>::arity == 2,
                  "Function must take two parameters.");
    typedef typename utils::function_traits<F>::template arg<0>::type FIn0;
    typedef typename utils::function_traits<F>::template arg<1>::type FIn1;
    static_assert(std::is_convertible<X, FIn0>::value,
                  "Function can not take provided parameter type");
    static_assert(std::is_convertible<Y, FIn1>::value,
                  "Function can not take bound parameter type");
};

template <typename F, typename X, typename Y, typename Z>
struct function_traits_asserts<bind_1st_of_3_tag, F, X, Y, Z>
{
    static_assert(utils::function_traits<F>::arity == 3,
                  "Function must take three parameters.");
    typedef typename utils::function_traits<F>::template arg<0>::type FIn0;
    typedef typename utils::function_traits<F>::template arg<1>::type FIn1;
    typedef typename utils::function_traits<F>::template arg<2>::type FIn2;
    static_assert(std::is_convertible<X, FIn0>::value,
                  "Function can not take bound parameter type");
    static_assert(std::is_convertible<Y, FIn1>::value,
                  "Function can not take provided first parameter type");
    static_assert(std::is_convertible<Z, FIn2>::value,
                  "Function can not take provided second parameter type");
};

template <typename F, typename X, typename Y, typename Z>
struct function_traits_asserts<bind_1st_and_2nd_of_3_tag, F, X, Y, Z>
{
    static_assert(utils::function_traits<F>::arity == 3,
                  "Function must take three parameters.");
    typedef typename utils::function_traits<F>::template arg<0>::type FIn0;
    typedef typename utils::function_traits<F>::template arg<1>::type FIn1;
    typedef typename utils::function_traits<F>::template arg<2>::type FIn2;
    static_assert(std::is_convertible<X, FIn0>::value,
                  "Function can not take first bound parameter type");
    static_assert(std::is_convertible<Y, FIn1>::value,
                  "Function can not take second bound parameter type");
    static_assert(std::is_convertible<Z, FIn2>::value,
                  "Function can not take provided parameter type");
};

template <typename F, typename X, typename Y, typename Z>
struct function_traits_asserts<bind_2nd_and_3rd_of_3_tag, F, X, Y, Z>
{
    static_assert(utils::function_traits<F>::arity == 3,
                  "Function must take three parameters.");
    typedef typename utils::function_traits<F>::template arg<0>::type FIn0;
    typedef typename utils::function_traits<F>::template arg<1>::type FIn1;
    typedef typename utils::function_traits<F>::template arg<2>::type FIn2;
    static_assert(std::is_convertible<X, FIn0>::value,
                  "Function can not take provided parameter type");
    static_assert(std::is_convertible<Y, FIn1>::value,
                  "Function can not take second bound parameter type");
    static_assert(std::is_convertible<Z, FIn2>::value,
                  "Function can not take first bound parameter type");
};

}
}

//
// internal/composition.hpp
//

// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)


#include <tuple>
#include <utility>


namespace fplus
{
namespace internal
{
// source: https://codereview.stackexchange.com/a/63893
// note: the code in the link above is called with the arguments in reverse order
template <typename... Fs>
class compose_impl
{
    static constexpr std::size_t size = sizeof...(Fs);
    static_assert(size > 1,
                  "Invalid number of functions to compose, minimum is two.");

  public:
    compose_impl(Fs&&... fs) : _functionTuple(std::forward<Fs>(fs)...)
    {
    }

    template <typename... Ts>
    auto operator()(Ts&&... ts) const
    {
        return _apply(std::integral_constant<std::size_t, 0>{},
                      std::forward<Ts>(ts)...);
    }

  private:
    template <std::size_t N, typename... Ts>
    auto _apply(std::integral_constant<std::size_t, N>, Ts&&... ts) const
    {
        return _apply(std::integral_constant<std::size_t, N + 1>{},
                      std::get<N>(_functionTuple)(std::forward<Ts>(ts)...));
    }

    template <typename... Ts>
    auto _apply(std::integral_constant<std::size_t, size - 1>, Ts&&... ts) const
    {
        return internal::invoke(std::get<size - 1>(_functionTuple),
                              std::forward<Ts>(ts)...);
    }

    std::tuple<Fs...> _functionTuple;
};

// Is BinaryLift really correct?
template <typename Tuple, typename BinaryLift>
auto compose_binary_lift_impl(std::integral_constant<std::size_t, 1>,
                              const Tuple& tup,
                              const BinaryLift& lifter)
{
    return lifter(std::get<0>(tup), std::get<1>(tup));
}

template <std::size_t N, typename Tuple, typename BinaryLift>
auto compose_binary_lift_impl(std::integral_constant<std::size_t, N>,
                              const Tuple& tup,
                              const BinaryLift& lifter)
{
    return lifter(
        compose_binary_lift_impl(
            std::integral_constant<std::size_t, N - 1>{}, tup, lifter),
        std::get<N>(tup));
}

template <typename BinaryLift, typename... Callables>
auto compose_binary_lift(const BinaryLift& lifter, Callables&&... args)
{
    static_assert(sizeof...(Callables) > 1,
                  "Invalid number of functions to compose, minimum is two.");
    const auto tup = std::forward_as_tuple(std::forward<Callables>(args)...);
    return compose_binary_lift_impl(
        std::integral_constant<std::size_t, sizeof...(Callables) - 1>{},
        tup,
        lifter);
}

// concentrate asserts in this method. Lambda is provided by the library.
template <typename Lambda, typename F, typename G>
auto logical_binary_op(Lambda op, F f, G g)
{
    // Perfect-forwarding might move twice, if we add a requirement on F and G,
    // that might not be an issue.
    return [op, f, g](auto x) {
        internal::trigger_static_asserts<internal::unary_function_tag,
                                             F,
                                             decltype(x)>();
        internal::trigger_static_asserts<internal::unary_function_tag,
                                             G,
                                             decltype(x)>();
        using FRes = std::decay_t<internal::invoke_result_t<F, decltype(x)>>;
        using GRes = std::decay_t<internal::invoke_result_t<G, decltype(x)>>;
        static_assert(std::is_same<FRes, bool>::value, "Must return bool.");
        static_assert(std::is_same<GRes, bool>::value, "Must return bool.");

        return op(f, g, x);
    };
}
}
}

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


//
// internal/compare.hpp
//

// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)


#include <type_traits>


namespace fplus
{
namespace internal
{
template <typename Compare>
auto ord_to_impl(Compare comp)
{
    return [comp](auto x, auto y)
    {
        static_assert(std::is_same<decltype(x), decltype(y)>::value,
            "Argument types must be the same");
        using In = decltype(x);
        internal::trigger_static_asserts<internal::binary_predicate_tag, Compare, In, In>();

        using CompareOut = std::decay_t<internal::invoke_result_t<Compare, In, In>>;
        static_assert(std::is_same<CompareOut, bool>::value,
                      "Function must return bool.");
        return std::make_pair(internal::invoke(comp, x, y),
                              internal::invoke(comp, y, x));
    };
}
}
}

namespace fplus
{

namespace internal
{
    template <typename UnaryPredicate, typename T>
    void check_unary_predicate_for_type()
    {
        internal::trigger_static_asserts<internal::unary_function_tag, UnaryPredicate, T>();
        static_assert(std::is_convertible<
            internal::invoke_result_t<UnaryPredicate, T>, bool>::value,
            "Predicate must return bool.");
    }
    template <typename F, typename G, typename X, typename Y>
    void check_compare_preprocessors_for_types()
    {
        internal::trigger_static_asserts<internal::unary_function_tag, F, X>();
        internal::trigger_static_asserts<internal::unary_function_tag, G, Y>();
        static_assert(std::is_same<
            std::decay_t<internal::invoke_result_t<F, X>>,
            std::decay_t<internal::invoke_result_t<G, Y>>>::value,
            "Both functions must return the same type.");
    }
} // namespace internal

// API search type: identity : a -> a
// fwd bind count: 0
// identity(x) == x
template <typename T>
T identity(const T& x)
{
    return x;
}

// API search type: is_equal : (a, a) -> Bool
// fwd bind count: 1
// x == y
// Equality check.
template <typename T>
bool is_equal(const T& x, const T& y)
{
    return x == y;
}

// API search type: always : a -> (b -> a)
// always(x)(y) == x
template <typename X>
auto always(const X& x)
{
    return [x](const auto&) { return x; };
}

// API search type: always_arg_1_of_2 : (a, b) -> a
// always_arg_1_of_2(x, y) == x
template <typename X, typename Y>
X always_arg_1_of_2(const X& x, const Y&)
{
    return x;
}

// API search type: always_arg_2_of_2 : (a, b) -> a
// always_arg_2_of_2(x, y) == x
template <typename X, typename Y>
Y always_arg_2_of_2(const X&, const Y& y)
{
    return y;
}

// API search type: is_equal_by_and_by : ((a -> b), (c -> b)) -> ((a, c) -> Bool)
// f(x) == g(y)
// Provides an equality check of two values
// after applying a transformation function each.
template <typename F, typename G>
auto is_equal_by_and_by(F f, G g)
{
    return [f, g](const auto& x, const auto& y) {
        internal::trigger_static_asserts<internal::unary_function_tag,
                                             F,
                                             decltype(x)>();
        internal::trigger_static_asserts<internal::unary_function_tag,
                                             G,
                                             decltype(y)>();
        return is_equal(internal::invoke(f, x), internal::invoke(g, y));
    };
}

// API search type: is_equal_by : (a -> b) -> (a -> Bool)
// f(x) == f(y)
// Provides an equality check of two values
// after applying the same transformation function to both.
template <typename F>
auto is_equal_by(F f)
{
    return is_equal_by_and_by(f, f);
}

// API search type: is_equal_by_to : ((b -> a), a) -> (b -> Bool)
// f(y) == x
// Provides an equality check to a fixed value
// after applying a transformation function.
template <typename F, typename X>
auto is_equal_by_to(F f, const X& x)
{
    return [f, x](const auto& y)
    {
        internal::trigger_static_asserts<internal::unary_function_tag,
                                             F,
                                             decltype(y)>();
        return is_equal(internal::invoke(f, y), x);
    };
}

// API search type: is_equal_to : a -> (a -> Bool)
// x == y
// curried version of is_equal
// Provides an equality check with a fixed value.
template <typename X>
auto is_equal_to(const X& x)
{
    return is_equal_by_to(identity<X>, x);
}

// API search type: is_not_equal : (a, a) -> Bool
// fwd bind count: 1
// x != y
// Unequally check.
template <typename T>
bool is_not_equal(const T& x, const T& y)
{
    return x != y;
}

// API search type: is_not_equal_by_and_by : ((a -> c), (b -> c)) -> ((a, b) -> Bool)
// f(x) != g(y)
// Provides an unequality check of two values
// after applying a transformation function eac
template <typename F, typename G>
auto is_not_equal_by_and_by(F f, G g)
{
    return [f, g](const auto& x, const auto& y) {
        internal::trigger_static_asserts<internal::unary_function_tag,
                                             F,
                                             decltype(x)>();
        internal::trigger_static_asserts<internal::unary_function_tag,
                                             G,
                                             decltype(y)>();
        using FOut = std::decay_t<internal::invoke_result_t<F, decltype(x)>>;
        using GOut = std::decay_t<internal::invoke_result_t<G, decltype(y)>>;
        static_assert(std::is_same<FOut, GOut>::value,
                      "Functions must return the same type.");
        return is_not_equal(internal::invoke(f, x), internal::invoke(g, y));
    };
}

// API search type: is_not_equal_by : (a -> b) -> ((a, a) -> Bool)
// f(x) != f(y)
// Provides an unequality check of two values
// after applying the same transformation function to both.
template <typename F>
auto is_not_equal_by(F f)
{
    return is_not_equal_by_and_by(f, f);
}

// API search type: is_not_equal_by_to : ((a -> b), b) -> (a -> Bool)
// f(y) != x
// Provides an unequality check to a fixed value
// after applying a transformation function.
template <typename F, typename X>
auto is_not_equal_by_to(F f, const X& x)
{
    return [f, x](const auto& y) {
        internal::trigger_static_asserts<internal::unary_function_tag,
                                             F,
                                             decltype(y)>();
        return is_not_equal(internal::invoke(f, y), x);
    };
}

// API search type: is_not_equal_to : a -> (a -> Bool)
// y != x
// curried version of is_not_equal
// Provides an unequality check with a fixed value.
template <typename X>
auto is_not_equal_to(const X& x)
{
    return is_not_equal_by_to(identity<X>, x);
}

// API search type: is_less : (a, a) -> Bool
// fwd bind count: 1
// x < y
// Less check.
template <typename T>
bool is_less(const T& x, const T& y)
{
    return x < y;
}

// API search type: is_less_by_and_by : ((a -> c), (b -> c)) -> ((a, b) -> Bool)
// f(x) < g(y)
// Provides a less check of two values
// after applying a transformation function each.
template <typename F, typename G>
auto is_less_by_and_by(F f, G g)
{
    return [f, g](const auto& x, const auto& y)
    {
        internal::trigger_static_asserts<internal::unary_function_tag,
                                             F,
                                             decltype(x)>();
        internal::trigger_static_asserts<internal::unary_function_tag,
                                             G,
                                             decltype(y)>();
        using FOut = std::decay_t<internal::invoke_result_t<F, decltype(x)>>;
        using GOut = std::decay_t<internal::invoke_result_t<G, decltype(y)>>;
        static_assert(std::is_same<FOut, GOut>::value,
                      "Functions must return the same type.");
        return is_less(internal::invoke(f, x), internal::invoke(g, y));
    };
}

// API search type: is_less_by : (a -> b) -> ((a, a) -> Bool)
// f(x) < f(y)
// Provides a less check of two values
// after applying the same transformation function to both.
template <typename F>
auto is_less_by(F f)
{
    return is_less_by_and_by(f, f);
}

// API search type: is_less_by_than : ((a -> b), b) -> (a -> Bool)
// f(y) < x
// Provides a less check to a fixed value
// after applying a transformation function.
template <typename F, typename X>
auto is_less_by_than(F f, const X& x)
{
    return [f, x](const auto& y)
    {
        internal::trigger_static_asserts<internal::unary_function_tag,
                                             F,
                                             decltype(y)>();
        return is_less(internal::invoke(f, y), x);
    };
}

// API search type: is_less_than : a -> (a -> Bool)
// y < x
// curried version of is_less
// Provides a less check with a fixed value.
template <typename X>
auto is_less_than(const X& x)
{
    return is_less_by_than(identity<X>, x);
}

// API search type: is_less_or_equal : (a, a) -> Bool
// fwd bind count: 1
// x <= y
// Less-or-equal check.
template <typename T>
bool is_less_or_equal(const T& x, const T& y)
{
    return x <= y;
}

// API search type: is_less_or_equal_by_and_by : ((a -> c), (b -> c)) -> ((a, b) -> Bool)
// f(x) <= g(y)
// Provides a less-or-equal check of two values
// after applying a transformation function each.
template <typename F, typename G>
auto is_less_or_equal_by_and_by(F f, G g)
{
    return [f, g](const auto& x, const auto& y)
    {
        using FIn = decltype(x);
        using GIn = decltype(y);
        internal::check_compare_preprocessors_for_types<F, G, FIn, GIn>();
        return is_less_or_equal(internal::invoke(f, x), internal::invoke(g, y));
    };
}

// API search type: is_less_or_equal_by : (a -> b) -> ((a, a) -> Bool)
// f(x) <= f(y)
// Provides a less-or-equal check of two values
// after applying the same transformation function to both.
template <typename F>
auto is_less_or_equal_by(F f)
{
    return is_less_or_equal_by_and_by(f, f);
}

// API search type: is_less_or_equal_by_than : ((a -> b), b) -> (a -> Bool)
// f(y) <= x
// Provides a less-or-equal check to a fixed value
// after applying a transformation function.
template <typename F, typename X>
auto is_less_or_equal_by_than(F f, const X& x)
{
    return [f, x](const auto& y)
    {
        internal::
            trigger_static_asserts<internal::unary_function_tag, F, decltype(y)>();
        return is_less_or_equal(internal::invoke(f, y), x);
    };
}

// API search type: is_less_or_equal_than : a -> (a -> Bool)
// y <= x
// curried version of is_less_or_equal
// Provides a less-or-equal check with a fixed value
template <typename X>
auto is_less_or_equal_than(const X& x)
{
    return is_less_or_equal_by_than(identity<X>, x);
}

// API search type: is_greater : a -> a -> Bool
// fwd bind count: 1
// x > y
// Greater check.
template <typename T>
bool is_greater(const T& x, const T& y)
{
    return x > y;
}

// API search type: is_greater_by_and_by : ((a -> c), (b -> c)) -> ((a, b) -> Bool)
// f(x) > g(y)
// Provides a greater check of two values
// after applying a transformation function each.
template <typename F, typename G>
auto is_greater_by_and_by(F f, G g)
{
    return [f, g](const auto& x, const auto& y)
    {
        using FIn = decltype(x);
        using GIn = decltype(y);

        internal::check_compare_preprocessors_for_types<F, G, FIn, GIn>();
        return is_greater(internal::invoke(f, x), internal::invoke(g, y));
    };
}

// API search type: is_greater_by : (a -> b) -> ((a, a) -> Bool)
// f(x) > f(y)
// Provides a greater check of two values
// after applying the same transformation function to both.
template <typename F>
auto is_greater_by(F f)
{
    return is_greater_by_and_by(f, f);
}

// API search type: is_greater_by_than : ((a -> b), b) -> (a -> Bool)
// f(y) > x
// Provides a greater check to a fixed value
// after applying a transformation function.
template <typename F, typename X>
auto is_greater_by_than(F f, const X& x)
{
    return [f, x](const auto& y)
    {
        return is_greater(internal::invoke(f, y), x);
    };
}

// API search type: is_greater_than : a -> (a -> Bool)
// y > x
// curried version of is_greater
// Provides a greater check with a fixed value.
template <typename X>
auto is_greater_than(const X& x)
{
    return is_greater_by_than(identity<X>, x);
}

// API search type: is_greater_or_equal : (a, a) -> Bool
// fwd bind count: 1
// x >= y
// Greater-or-equal check.
template <typename T>
bool is_greater_or_equal(const T& x, const T& y)
{
    return x >= y;
}

// API search type: is_greater_or_equal_by_and_by : ((a -> c), (b -> c)) -> ((a, b) -> Bool)
// f(x) >= g(y)
// Provides a greater-or-equal check of two values
// after applying a transformation function each.
template <typename F, typename G>
auto is_greater_or_equal_by_and_by(F f, G g)
{
    return [f, g](const auto& x, const auto& y)
    {
        using FIn = decltype(x);
        using GIn = decltype(y);
        internal::check_compare_preprocessors_for_types<F, G, FIn, GIn>();
        return is_greater_or_equal(internal::invoke(f, x), internal::invoke(g, y));
    };
}

// API search type: is_greater_or_equal_by : (a -> b) -> ((a, a) -> Bool)
// f(x) >= f(y)
// Provides a greater-or-equal check of two values
// after applying the same transformation function to both.
template <typename F>
auto is_greater_or_equal_by(F f)
{
    return is_greater_or_equal_by_and_by(f, f);
}

// API search type: is_greater_or_equal_by_than : ((a -> b), b) -> (a -> Bool)
// f(y) >= x
// Provides a greater-or-equal check to a fixed value
// after applying a transformation function.
template <typename F, typename X>
auto is_greater_or_equal_by_than(F f, const X& x)
{
    return [f, x](const auto& y)
    {
        internal::trigger_static_asserts<internal::unary_function_tag, F, decltype(y)>();
        return is_greater_or_equal(internal::invoke(f, y), x);
    };
}

// API search type: is_greater_or_equal_than : a -> (a -> Bool)
// y >= x
// curried version of is_less_or_equal
// Provides a greater-or-equal check with a fixed valu
template <typename X>
auto is_greater_or_equal_than(const X& x)
{
    return is_greater_or_equal_by_than(identity<X>, x);
}

// API search type: xor_bools : (Bool, Bool) -> Bool
// fwd bind count: 1
// Exclusive or.
template <typename T>
bool xor_bools(const T& x, const T& y)
{
    static_assert(std::is_convertible<T, bool>::value,
        "Type must be convertible to bool.");
    return (x && !y) || (!x && y);
}

// API search type: ord_to_eq : ((a, a) -> Bool) -> ((a, a) -> Bool)
// ord_to_eq((<)) == (==)
// Takes a less-than function and converts it
// into an equality check function
// which considers two values as equal if none are lesser than the other one.
template <typename Compare>
auto ord_to_eq(Compare comp)
{
    return [comp](auto x, auto y)
    {
        static_assert(std::is_same<decltype(x), decltype(y)>::value,
            "Argument types must be the same");
        auto p = internal::ord_to_impl(comp)(x, y);
        return !p.first && !p.second;
    };
}

// API search type: ord_to_not_eq : ((a, a) -> Bool) -> ((a, a) -> Bool)
// ord_to_not_eq((<)) == (!=)
// Takes a less-than function and converts it
// into an inequality check function
// which considers to values as unequal if one is less than the other one.
template <typename Compare>
auto ord_to_not_eq(Compare comp)
{
    return logical_not(ord_to_eq(comp));
}

// API search type: ord_eq_to_eq : ((a, a) -> Bool) -> ((a, a) -> Bool)
// ord_eq_to_eq((<=)) == (==)
// ord_to_eq((<)) == (==)
// Takes a less-or-equal-than function and converts it
// into an equality check function
// which considers to values as equal if a <= b and b <= a.
template <typename Compare>
auto ord_eq_to_eq(Compare comp)
{
    return [comp](auto x, auto y)
    {
        static_assert(std::is_same<decltype(x), decltype(y)>::value,
            "Argument types must be the same");
        auto p = internal::ord_to_impl(comp)(x, y);
        return p.first && p.second;
    };
}

// API search type: ord_eq_to_not_eq : ((a, a) -> Bool) -> ((a, a) -> Bool)
// ord_eq_to_not_eq((<=)) == (!=)
// Takes a less-or-equal-than function and converts it
// into an inequality check function
// which considers to values as equal if not a <= b and not b <= a.
template <typename Compare>
auto ord_eq_to_not_eq(Compare comp)
{
  return logical_not(ord_eq_to_eq(comp));
}

} // namespace fplus

//
// container_common.hpp
//

// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)



//
// container_traits.hpp
//

// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)


#include <array>
#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <limits>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <vector>


namespace fplus
{

namespace internal
{

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
#endif

template<class T> struct has_order : public std::false_type {};
template<class T, std::size_t N> struct has_order<std::array<T, N>> : public std::true_type {};
template<class T, class Alloc> struct has_order<std::vector<T, Alloc>> : public std::true_type {};
template<class T, class Alloc> struct has_order<std::deque<T, Alloc>> : public std::true_type {};
template<class T, class Alloc> struct has_order<std::forward_list<T, Alloc>> : public std::true_type {};
template<class T, class Alloc> struct has_order<std::list<T, Alloc>> : public std::true_type {};
template<class T, class Alloc> struct has_order<std::set<T, Alloc>> : public std::false_type {};
template<class T, class Container> struct has_order<std::stack<T, Container>> : public std::true_type {};
template<class T, class Container> struct has_order<std::queue<T, Container>> : public std::true_type {};
template<class T, class Container, class Compare> struct has_order<std::priority_queue<T, Container, Compare>> : public std::false_type {};
template<class CharT, class Traits, class Alloc> struct has_order<std::basic_string<CharT, Traits, Alloc>> : public std::true_type {};

// http://stackoverflow.com/a/33828321/1866775
template<class Cont, class NewT, int SizeOffset = std::numeric_limits<int>::lowest()> struct same_cont_new_t : public std::false_type{};
template<class T, std::size_t N, class NewT, int SizeOffset> struct same_cont_new_t<std::array<T, N>, NewT, SizeOffset>
{
    static_assert(SizeOffset != std::numeric_limits<int>::lowest(), "Size of std::array must be known at compile-time.");
    typedef typename std::array<NewT, static_cast<std::size_t>(static_cast<int>(N) + SizeOffset)> type;
};
template<class T, class Alloc, class NewT, int SizeOffset> struct same_cont_new_t<std::vector<T, Alloc>, NewT, SizeOffset> { typedef typename std::vector<NewT> type; };
template<class T, class Alloc, class NewT, int SizeOffset> struct same_cont_new_t<std::deque<T, Alloc>, NewT, SizeOffset> { typedef typename std::deque<NewT> type; };
template<class T, class Alloc, class NewT, int SizeOffset> struct same_cont_new_t<std::forward_list<T, Alloc>, NewT, SizeOffset> { typedef typename std::forward_list<NewT> type; };
template<class T, class Alloc, class NewT, int SizeOffset> struct same_cont_new_t<std::list<T, Alloc>, NewT, SizeOffset> { typedef typename std::list<NewT> type; };
template<class T, class Alloc, class NewT, int SizeOffset> struct same_cont_new_t<std::set<T, Alloc>, NewT, SizeOffset> { typedef typename std::set<NewT> type; };
template<class T, class Container, class NewT, int SizeOffset> struct same_cont_new_t<std::stack<T, Container>, NewT, SizeOffset> { typedef typename std::stack<NewT, Container> type; };
template<class T, class Container, class NewT, int SizeOffset> struct same_cont_new_t<std::queue<T, Container>, NewT, SizeOffset> { typedef typename std::queue<NewT, Container> type; };
template<class T, class Container, class Compare, class NewT, int SizeOffset> struct same_cont_new_t<std::priority_queue<T, Container, Compare>, NewT, SizeOffset> { typedef typename std::priority_queue<NewT, Container, Compare> type; };
template<class CharT, class Traits, class Alloc, class NewT, int SizeOffset> struct same_cont_new_t<std::basic_string<CharT, Traits, Alloc>, NewT, SizeOffset> { typedef typename std::basic_string<NewT, Traits, Alloc> type; };

template<class Cont, class NewKey, class NewVal> struct SameMapTypeNewTypes : public std::false_type {};
template<class Key, class T, class Compare, class Alloc, class NewKey, class NewVal> struct SameMapTypeNewTypes<std::map<Key, T, Compare, Alloc>, NewKey, NewVal> { typedef typename std::map<NewKey, NewVal> type; };
template<class Key, class T, class Compare, class Alloc, class NewKey, class NewVal> struct SameMapTypeNewTypes<std::unordered_map<Key, T, Compare, Alloc>, NewKey, NewVal> { typedef typename std::unordered_map<NewKey, NewVal> type; };

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

template<
    typename ContIn,
    typename F,
    int SizeOffset = std::numeric_limits<int>::lowest(),
    typename T = typename ContIn::value_type,
    typename ContOut = typename same_cont_new_t<ContIn, std::decay_t<internal::invoke_result_t<F, T>>, SizeOffset>::type>
struct same_cont_new_t_from_unary_f
{
    typedef ContOut type;
};

template<
    typename ContIn,
    typename F,
    typename T1,
    typename T2,
    int SizeOffset = std::numeric_limits<int>::lowest(),
    typename ContOut = typename same_cont_new_t<ContIn, std::decay_t<internal::invoke_result_t<F, T1, T2>>, SizeOffset>::type>
struct same_cont_new_t_from_binary_f
{
    typedef ContOut type;
};



// https://stackoverflow.com/a/44549820/1866775

template<class T>
struct can_self_assign {
    using type = std::is_assignable<T&, T>;
};

template<typename T>
using can_self_assign_t = typename can_self_assign<T>::type;

template<typename T0, typename T1>
struct can_self_assign<std::pair<T0, T1>>
{
    enum { t0 = can_self_assign_t<T0>::value, t1 = can_self_assign_t<T1>::value, x = t0&&t1 };
    using type = std::integral_constant<bool, x>;
};

template<>
struct can_self_assign<std::tuple<>>
{
    using type = std::integral_constant<bool, true>;
};
template<typename T0, typename...Ts>
struct can_self_assign<std::tuple<T0, Ts...>>
{
    using type = std::integral_constant<bool, can_self_assign_t<T0>::value && can_self_assign_t<std::tuple<Ts...>>::value >;
};

template<class T, T v>
struct reuse_container_bool_t {
};
using create_new_container_t = reuse_container_bool_t<bool, false>;
using reuse_container_t = reuse_container_bool_t<bool, true>;

template <typename Container>
struct can_reuse
{
    using dContainer = typename std::decay<Container>::type;
    using can_assign = can_self_assign_t<typename dContainer::value_type>;
    using cannot_reuse = std::is_lvalue_reference<Container>;
    using value = reuse_container_bool_t<bool, can_assign::value && !cannot_reuse::value>;
};

template<typename Container>
using can_reuse_v = typename can_reuse<Container>::value;

template <typename T>
struct remove_const_and_ref
{
    using type = typename std::remove_const<typename std::remove_reference<T>::type>::type;
};

template<typename T>
using remove_const_and_ref_t = typename remove_const_and_ref<T>::type;


} // namespace internal

} // namespace fplus

//
// maybe.hpp
//

// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)



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
        return *reinterpret_cast<const T*>(&value_);
    }
    T& unsafe_get_just()
    {
        assert(is_just());
        return *reinterpret_cast<T*>(&value_);
    }
    typedef T type;
    maybe() : is_present_(false), value_() {};
    ~maybe()
    {
        destruct_content();
    }
    maybe(const T& val_just) : is_present_(true), value_()
    {
        new (&value_) T(val_just);
    }
    maybe(T&& val_just) : is_present_(true), value_() {
        new (&value_) T(std::move(val_just));
    }
    maybe(const maybe<T>& other) : is_present_(other.is_just()), value_()
    {
        if (is_present_)
        {
            new (&value_) T(other.unsafe_get_just());
        }
    }
    maybe(maybe<T>&& other) : is_present_(std::move(other.is_present_)), value_()
    {
        if (is_present_)
        {
            new (&value_) T(std::move(other.unsafe_get_just()));
        }
    }
    maybe<T>& operator = (const T& other)
    {
        destruct_content();
        is_present_ = true;
        new (&value_) T(other);
        return *this;
    }
    maybe& operator = (T&& other) {
        destruct_content();
        is_present_ = true;
        new (&value_) T(std::move(other));
        return *this;
    }
    maybe<T>& operator = (const maybe<T>& other)
    {
        destruct_content();
        if (other.is_just())
        {
            is_present_ = true;
            new (&value_) T(other.unsafe_get_just());
        }
        return *this;
    }
    maybe& operator = (maybe<T>&& other) {
        destruct_content();
        is_present_ = std::move(other.is_present_);
        if (is_present_)
        {
            new (&value_) T(std::move(other.unsafe_get_just()));
        }
        return *this;
    }
private:
    void destruct_content()
    {
        if (is_present_)
        {
            is_present_ = false;
            (*reinterpret_cast<const T*>(&value_)).~T();
        }
    }
    bool is_present_;
    typename std::aligned_storage<sizeof(T), alignof(T)>::type value_;
};

namespace internal
{
template <typename>
struct is_maybe : std::false_type
{
};

template <typename T>
struct is_maybe<maybe<T>> : std::true_type
{
};
}

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
    internal::trigger_static_asserts<internal::check_arity_tag, F, A>();

    using B = std::decay_t<internal::invoke_result_t<F, A>>;
    if (is_just(m))
        return just<B>(internal::invoke(f, unsafe_get_just(m)));
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
    internal::trigger_static_asserts<internal::check_arity_tag, F, A>();

    using B = std::decay_t<internal::invoke_result_t<F, A>>;
    static_assert(
        std::is_convertible<Default, B>::value,
        "Default value must be convertible to Function's return type");
    if (is_just(m))
        return B(internal::invoke(f, unsafe_get_just(m)));
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
    internal::trigger_static_asserts<internal::check_arity_tag, F, A, B>();

    using FOut = std::decay_t<internal::invoke_result_t<F, A, B>>;
    if (is_just(m_a) && is_just(m_b))
    {
        return just<FOut>(
            internal::invoke(f, unsafe_get_just(m_a), unsafe_get_just(m_b)));
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
template <typename F, typename A, typename B, typename Default>
auto lift_maybe_2_def(const Default& def,
                      F f,
                      const maybe<A>& m_a,
                      const maybe<B>& m_b)
{
    internal::trigger_static_asserts<internal::check_arity_tag, F, A, B>();

    using C = std::decay_t<internal::invoke_result_t<F, A, B>>;
    static_assert(
        std::is_convertible<Default, C>::value,
        "Default value must be convertible to Function's return type");
    if (is_just(m_a) && is_just(m_b))
        return C(internal::invoke(f, unsafe_get_just(m_a), unsafe_get_just(m_b)));
    return C(def);
}

// API search type: join_maybe : Maybe Maybe a -> Maybe a
// Flattens a nested maybe.
// join_maybe(Just Just x) == Just x
// join_maybe(Just Nothing) == Nothing
// join_maybe(Nothing) == Nothing
template <typename A>
maybe<A> join_maybe(const maybe<maybe<A>>& m)
{
    if (is_just(m))
        return unsafe_get_just(m);
    else
        return nothing<A>();
}

// API search type: and_then_maybe : ((a -> Maybe b), (Maybe a)) -> Maybe b
// fwd bind count: 1
// Monadic bind.
// Returns nothing if the maybe already is nothing.
// Otherwise return the result of applying
// the function to the just value of the maybe.
template <typename T, typename F>
auto and_then_maybe(F f, const maybe<T>& m)
{
    internal::trigger_static_asserts<internal::check_arity_tag, F, T>();
    using FOut = std::decay_t<internal::invoke_result_t<F, T>>;
    static_assert(internal::is_maybe<FOut>::value,
                  "Function must return a maybe<> type");
    if (is_just(m))
        return internal::invoke(f, unsafe_get_just(m));
    else
        return nothing<typename FOut::type>();
}

// API search type: compose_maybe : ((a -> Maybe b), (b -> Maybe c)) -> (a -> Maybe c)
// Left-to-right Kleisli composition of monads.
// Composes multiple callables taking a value and returning Maybe.
// If the first callable returns a just, the value from the just
// is extracted and shoved into the next callable.
// If the first callable returns a nothing, it remains a nothing.
// The first callable can take a variadic number of parameters.
template <typename... Callables>
auto compose_maybe(Callables&&... callables)
{
    auto bind_maybe = [](auto f, auto g) {
        // next step would be to perfectly forward callables, as shown here:
        // https://vittorioromeo.info/index/blog/capturing_perfectly_forwarded_objects_in_lambdas.html
        return [f = std::move(f), g = std::move(g)](auto&&... args)
        {
            using FOut = std::decay_t<
                internal::invoke_result_t<decltype(f), decltype(args)...>>;
            static_assert(internal::is_maybe<FOut>::value,
                          "Functions must return a maybe<> type");
            using GOut = std::decay_t<
                internal::invoke_result_t<decltype(g), typename FOut::type>>;
            static_assert(internal::is_maybe<GOut>::value,
                          "Functions must return a maybe<> type");

            auto maybeB =
                internal::invoke(f, std::forward<decltype(args)>(args)...);
            if (is_just(maybeB))
                return internal::invoke(g, unsafe_get_just(maybeB));
            return GOut{};
        };
    };

    return internal::compose_binary_lift(bind_maybe,
                                       std::forward<Callables>(callables)...);
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


//
// internal/container_common.hpp
//

// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)


#include <numeric>
#include <type_traits>


namespace fplus
{
namespace internal
{

template<class InputIt, class T>
T accumulate(InputIt first, InputIt last, T init)
{
    for (; first != last; ++first) {
        init = std::move(init) + *first;
    }
    return init;
}

template<class InputIt, class T, class BinaryOperation>
T accumulate(InputIt first, InputIt last, T init,
             BinaryOperation op)
{
    for (; first != last; ++first) {
        init = op(std::move(init), *first);
    }
    return init;
}

template <typename F,
          typename Acc,
          typename InputIterator,
          typename OutputIterator>
void scan_impl(F f,
               const Acc& init,
               OutputIterator itOut,
               InputIterator begin,
               InputIterator end)
{
    *itOut = init;

    auto g = [itOut, f](auto acc, auto x) mutable
    {
        acc = internal::invoke(f, acc, x);
        *itOut = acc;
        return acc;
    };

    internal::accumulate(begin, end, init, g);
}
}
}

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <iterator>
#include <numeric>

namespace fplus
{

namespace internal
{
    template <typename UnaryPredicate, typename Container>
    void check_unary_predicate_for_container()
    {
        internal::check_unary_predicate_for_type<UnaryPredicate,
            typename Container::value_type>();
    }

    template <typename F, typename Container>
    void check_index_with_type_predicate_for_container()
    {
        typedef typename Container::value_type T;
        internal::trigger_static_asserts<internal::binary_function_tag, F, std::size_t, T>();
        static_assert(std::is_convertible<
            internal::invoke_result_t<F, std::size_t, T>, bool>::value,
            "Function must return bool.");
    }

    template <typename Compare, typename Container>
    void check_compare_for_container()
    {
        typedef typename Container::value_type T;
        internal::trigger_static_asserts<internal::binary_predicate_tag, Compare, T, T>();
    }

    template <typename BinaryPredicate, typename Container>
    void check_binary_predicate_for_container()
    {
        typedef typename Container::value_type T;
        internal::trigger_static_asserts<internal::binary_predicate_tag, BinaryPredicate, T, T>();
    }

    // PrepareContainer and BackInserter are overloaded
    // to increase performance on std::vector and std::string
    // by using std::vector<T>::reserve
    // and std::back_inserter instead of std::back_inserter.
    // In VC2015, release mode, Celsius W520 Xeon
    // this leads to an increase in performance of about a factor of 3
    // for transform.
    template <typename C>
    void prepare_container(const std::basic_string<C, std::char_traits<C>,
        std::allocator<C>>& ys, std::size_t size)
    {
        ys.reserve(size);
    }

    template <typename Y>
    void prepare_container(std::vector<Y>& ys, std::size_t size)
    {
        ys.reserve(size);
    }

    template <typename T, std::size_t N>
    void prepare_container(std::array<T, N>&, std::size_t size)
    {
        assert(size == N);
        unused(size);
    }

    template <typename Y>
    void prepare_container(std::unordered_set<Y>& ys, std::size_t size)
    {
        ys.reserve(size);
    }

    template <typename Key, typename T>
    void prepare_container(std::unordered_map<Key, T>& ys, std::size_t size)
    {
        ys.reserve(size);
    }

    template <typename Y>
    void prepare_container(std::unordered_multiset<Y>& ys, std::size_t size)
    {
        ys.reserve(size);
    }

    template <typename Key, typename T>
    void prepare_container(std::unordered_multimap<Key, T>& ys, std::size_t size)
    {
        ys.reserve(size);
    }

    template <typename Container>
    void prepare_container(Container&, std::size_t)
    {
    }

    template <typename Container>
    std::back_insert_iterator<Container> get_back_inserter(std::string& ys)
    {
        return std::back_inserter(ys);
    }

    template <typename Container, typename Y>
    std::back_insert_iterator<Container> get_back_inserter(std::vector<Y>& ys)
    {
        return std::back_inserter(ys);
    }

    template <typename Container, typename Y>
    std::back_insert_iterator<Container> get_back_inserter(std::list<Y>& ys)
    {
        return std::back_inserter(ys);
    }

    template <typename Container, typename Y>
    std::back_insert_iterator<Container> get_back_inserter(std::deque<Y>& ys)
    {
        return std::back_inserter(ys);
    }

    template <typename T, std::size_t N>
    struct array_back_insert_iterator : public std::back_insert_iterator<std::array<T, N>>
    {
        typedef std::back_insert_iterator<std::array<T, N>> base_type;
        explicit array_back_insert_iterator(std::array<T, N>& arr) :
            base_type(arr), arr_ptr_(&arr), pos_(0) {}
        array_back_insert_iterator<T, N>(const array_back_insert_iterator<T, N>& other) :
            base_type(*other.arr_ptr_), arr_ptr_(other.arr_ptr_), pos_(other.pos_) {}
        array_back_insert_iterator<T, N>& operator=(const array_back_insert_iterator<T, N>& other)
        {
            arr_ptr_ = other.arr_ptr_;
            pos_ = other.pos_;
            return *this;
        }
        ~array_back_insert_iterator()
        {
            assert(pos_ == 0 || pos_ == N);
        }
        array_back_insert_iterator<T, N>& operator=(const T& x)
        {
            assert(pos_ < N);
            (*arr_ptr_)[pos_] = x;
            ++pos_;
            return *this;
        }
        array_back_insert_iterator<T, N>& operator=(T&& x)
        {
            assert(pos_ < N);
            (*arr_ptr_)[pos_] = std::move(x);
            ++pos_;
            return *this;
        }
        array_back_insert_iterator<T, N>& operator*() { return *this; }
        array_back_insert_iterator<T, N>& operator++() { return *this; }
        array_back_insert_iterator<T, N> operator++(int) { return *this; }
    private:
        std::array<T, N>* arr_ptr_;
        std::size_t pos_;
    };

#if defined(_MSC_VER) && _MSC_VER >= 1900
    template <typename T, std::size_t N>
    struct std::_Is_checked_helper<array_back_insert_iterator<T, N>>
        : public true_type
    { // mark array_back_insert_iterator as checked
    };
#endif

    template <typename Container, typename Y, std::size_t N>
    array_back_insert_iterator<Y, N> get_back_inserter(std::array<Y, N>& ys)
    {
        return array_back_insert_iterator<Y, N>(ys);
    }

    template <typename Container>
    std::insert_iterator<Container> get_back_inserter(Container& ys)
    {
        return std::inserter(ys, std::end(ys));
    }

    template <typename Iterator>
    void advance_iterator(Iterator& it, std::size_t distance)
    {
        std::advance(it,
            static_cast<typename Iterator::difference_type>(distance));
    }

    template <typename T>
    void advance_iterator(T*& it, std::size_t distance)
    {
        it += static_cast<std::ptrdiff_t>(distance);
    }

    template <typename Iterator>
    Iterator add_to_iterator(Iterator it, std::size_t distance = 1)
    {
        return std::next(it,
            static_cast<typename Iterator::difference_type>(distance));
    }

    // GCC 4.9 does not support std::rbegin, std::rend and std::make_reverse_iterator
    template <typename Iterator>
    std::reverse_iterator<Iterator> make_reverse_iterator(Iterator it)
    {
        return std::reverse_iterator<Iterator>(it);
    }
} // namespace internal

// API search type: is_empty : [a] -> Bool
// fwd bind count: 0
// Returns true if the container holds no elements.
// is_empty([1, 2]) == false
// is_empty([]) == true
template <typename Container>
bool is_empty(const Container& xs)
{
    return xs.empty();
}

// API search type: is_not_empty : [a] -> Bool
// fwd bind count: 0
// Returns true if the container holds at least one element.
// is_not_empty([1, 2]) == true
template <typename Container>
bool is_not_empty(const Container& xs)
{
    return !is_empty(xs);
}

// API search type: size_of_cont : [a] -> Int
// fwd bind count: 0
// Returns the number of elements in the given container.
// size_of_cont([3, 4]) == 2
template <typename Container>
std::size_t size_of_cont(const Container& xs)
{
    return xs.size();
}

// API search type: convert : a -> b
// fwd bind count: 0
// Converts one type of element into another.
template <typename Dest, typename Source>
Dest convert(const Source& x)
{
    return Dest(x);
}

// API search type: convert_elems : [a] -> [b]
// fwd bind count: 0
// Converts all elements in a sequence to a different type.
// convert_elems<NewT>([1, 2, 3]) == [NewT(1), NewT(2), NewT(3)]
template <typename NewT, typename ContainerIn,
    typename ContainerOut = typename internal::same_cont_new_t<ContainerIn, NewT, 0>::type>
ContainerOut convert_elems(const ContainerIn& xs)
{
    static_assert(std::is_constructible<NewT,
        typename ContainerIn::value_type>::value,
        "Elements not convertible.");
    ContainerOut ys;
    internal::prepare_container(ys, size_of_cont(xs));
    auto it = internal::get_back_inserter<ContainerOut>(ys);
    // using 'for (const auto& x ...)' is even for ints as fast as
    // using 'for (int x ...)' (GCC, O3), so there is no need to
    // check if the type is fundamental and then dispatch accordingly.
    for (const auto& x : xs)
    {
        *it = convert<NewT>(x);
    }
    return ys;
}

// API search type: convert_container : [a] -> [a]
// fwd bind count: 0
// Change the type of the container
// while keeping the elements in the sequence the same.
// convert_container([1, 2, 3]) == [1, 2, 3]
// Useful for example if you want to convert an std::list to an std::vector.
template <typename ContainerOut, typename ContainerIn>
ContainerOut convert_container(const ContainerIn& xs)
{
    typedef typename ContainerIn::value_type SourceElem;
    typedef typename ContainerOut::value_type DestElem;
    static_assert(std::is_same<DestElem, SourceElem>::value,
        "Source and dest container must have the same value_type");
    ContainerOut ys;
    internal::prepare_container(ys, size_of_cont(xs));
    auto itOut = internal::get_back_inserter<ContainerOut>(ys);
    std::copy(std::begin(xs), std::end(xs), itOut);
    return ys;
}

// API search type: convert_container_and_elems : [a] -> [b]
// fwd bind count: 0
// Converts between different containers and elements.
// Dest elements are allowed to have explicit constructors.
// convert([1, 2, 3]) == [1, 2, 3]
template <typename ContainerOut, typename ContainerIn>
ContainerOut convert_container_and_elems(const ContainerIn& xs)
{
    static_assert(std::is_convertible<typename ContainerIn::value_type,
        typename ContainerOut::value_type>::value,
        "Elements not convertible.");
    typedef typename ContainerOut::value_type DestElem;
    ContainerOut ys;
    internal::prepare_container(ys, size_of_cont(xs));
    auto it = internal::get_back_inserter<ContainerOut>(ys);
    for (const auto& x : xs)
    {
        *it = convert<DestElem>(x);
    }
    return ys;
}

namespace internal
{

template <typename Container>
Container get_segment(internal::reuse_container_t,
    std::size_t idx_begin, std::size_t idx_end, Container&& xs)
{
    idx_end = std::min(idx_end, size_of_cont(xs));
    if (idx_end <= idx_begin)
    {
        xs.clear();
        return std::forward<Container>(xs);
    }
    auto itBegin = std::begin(xs);
    internal::advance_iterator(itBegin, idx_begin);
    auto itEnd = itBegin;
    internal::advance_iterator(itEnd, idx_end - idx_begin);
    xs.erase(std::copy(itBegin, itEnd, std::begin(xs)), std::end(xs));
    return std::forward<Container>(xs);
}

template <typename Container>
Container get_segment(internal::create_new_container_t,
    std::size_t idx_begin, std::size_t idx_end, const Container& xs)
{
    idx_end = std::min(idx_end, size_of_cont(xs));
    if (idx_end <= idx_begin)
    {
        return {};
    }
    Container result;
    auto itBegin = std::begin(xs);
    internal::advance_iterator(itBegin, idx_begin);
    auto itEnd = itBegin;
    internal::advance_iterator(itEnd, idx_end - idx_begin);
    std::copy(itBegin, itEnd, internal::get_back_inserter(result));
    return result;
}

} // namespace internal

// API search type: get_segment : (Int, Int, [a]) -> [a]
// fwd bind count: 2
// Return a defined segment from the sequence.
// get_segment(2, 5, [0,1,2,3,4,5,6,7,8]) == [2,3,4]
// get_segment(2, 15, [0,1,2,3,4,5,6,7,8]) == [2,3,4,5,6,7,8]
// get_segment(5, 2, [0,1,2,3,4,5,6,7,8]) == []
// Also known as slice.
template <typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut get_segment
        (std::size_t idx_begin, std::size_t idx_end, Container&& xs)
{
    return internal::get_segment(internal::can_reuse_v<Container>{},
        idx_begin, idx_end, std::forward<Container>(xs));
}

namespace internal
{

template <typename ContainerToken, typename Container>
Container set_segment(internal::reuse_container_t,
    std::size_t idx_begin, const ContainerToken& token, Container&& xs)
{
    assert(idx_begin + size_of_cont(token) < size_of_cont(xs));
    auto itBegin = std::begin(xs);
    internal::advance_iterator(itBegin, idx_begin);
    std::copy(std::begin(token), std::end(token), itBegin);
    return std::forward<Container>(xs);
}

template <typename ContainerToken, typename Container>
Container set_segment(internal::create_new_container_t,
    std::size_t idx_begin, const ContainerToken& token, const Container& xs)
{
    Container result = xs;
    return set_segment(internal::reuse_container_t(),
        idx_begin, token, std::move(result));
}

} // namespace internal

// API search type: set_segment : (Int, [a], [a]) -> [a]
// fwd bind count: 2
// Replace part of a sequence with a token.
// set_segment(2, [9,9,9], [0,1,2,3,4,5,6,7,8]) == [0,1,9,9,9,5,6,7,8]
// Crashes on invalid indices.
// Also known as replace_segment.
template <typename ContainerToken, typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut set_segment
        (std::size_t idx_begin, const ContainerToken& token, Container&& xs)
{
    return internal::set_segment(internal::can_reuse_v<Container>{},
        idx_begin, token, std::forward<Container>(xs));
}

namespace internal
{

template <typename Container>
Container remove_segment(internal::reuse_container_t,
    std::size_t idx_begin, std::size_t idx_end, Container&& xs)
{
    assert(idx_begin <= idx_end);
    assert(idx_end <= size_of_cont(xs));

    auto firstBreakIt = std::begin(xs);
    internal::advance_iterator(firstBreakIt, idx_begin);

    auto secondBreakIt = std::begin(xs);
    internal::advance_iterator(secondBreakIt, idx_end);

    xs.erase(
        std::copy(secondBreakIt, std::end(xs), firstBreakIt), std::end(xs));
    return std::forward<Container>(xs);
}

template <typename Container>
Container remove_segment(internal::create_new_container_t,
    std::size_t idx_begin, std::size_t idx_end, const Container& xs)
{
    assert(idx_begin <= idx_end);
    assert(idx_end <= size_of_cont(xs));

    Container result;
    std::size_t length = idx_end - idx_begin;
    internal::prepare_container(result, size_of_cont(xs) - length);

    auto firstBreakIt = std::begin(xs);
    internal::advance_iterator(firstBreakIt, idx_begin);
    std::copy(std::begin(xs), firstBreakIt, internal::get_back_inserter(result));

    auto secondBreakIt = std::begin(xs);
    internal::advance_iterator(secondBreakIt, idx_end);
    std::copy(secondBreakIt, std::end(xs), internal::get_back_inserter(result));

    return result;
}

} // namespace internal

// API search type: remove_segment : (Int, Int, [a]) -> [a]
// fwd bind count: 2
// Cuts our a  defined segment from the sequence.
// remove_segment(2, 5, [0,1,2,3,4,5,6,7]) == [0,1,5,6,7]
// crashes on invalid indices
template <typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut remove_segment(
        std::size_t idx_begin, std::size_t idx_end, Container&& xs)
{
    return internal::remove_segment(internal::can_reuse_v<Container>{},
        idx_begin, idx_end, std::forward<Container>(xs));
}

// API search type: insert_at : (Int, [a], [a]) -> [a]
// fwd bind count: 2
// Inserts a token into a sequence at a specific position.
// insert_at(2, [8,9], [0,1,2,3,4]) == [0,1,8,9,2,3,4]
// Unsafe! Crashes on invalid index.
template <typename Container>
Container insert_at(std::size_t idx_begin,
        const Container& token, const Container& xs)
{
    assert(idx_begin <= size_of_cont(xs));

    Container result;
    internal::prepare_container(result, size_of_cont(xs) + size_of_cont(token));

    auto breakIt = std::begin(xs);
    internal::advance_iterator(breakIt, idx_begin);
    std::copy(std::begin(xs), breakIt, internal::get_back_inserter(result));
    std::copy(std::begin(token), std::end(token), internal::get_back_inserter(result));
    std::copy(breakIt, std::end(xs), internal::get_back_inserter(result));

    return result;
}

// API search type: elem_at_idx : (Int, [a]) -> a
// fwd bind count: 1
// Return the nth element of a sequence.
// elem_at_idx(2, [7,6,5,4,3]) == 5
// Unsafe! Crashes on invalid index.
template <typename Container>
auto elem_at_idx(std::size_t idx, const Container& xs)
{
    assert(idx < size_of_cont(xs));
    auto it = std::begin(xs);
    internal::advance_iterator(it, idx);
    return *it;
}

// API search type: elem_at_idx_maybe : (Int, [a]) -> Maybe a
// fwd bind count: 1
// Return the nth element of a sequence if existing.
// elem_at_idx_maybe(2, [7,6,5,4,3]) == Just 5
// elem_at_idx_maybe(9, [7,6,5,4,3]) == Nothing
// Use elem_at_idx_or_nothing if you want to provide a signed index type.
template <typename Container,
    typename T = typename Container::value_type>
maybe<T> elem_at_idx_maybe(std::size_t idx, const Container& xs)
{
    if (size_of_cont(xs) < idx)
    {
        return {};
    }
    auto it = std::begin(xs);
    internal::advance_iterator(it, idx);
    return *it;
}

// API search type: elems_at_idxs : ([Int], [a]) -> [a]
// fwd bind count: 1
// Construct a subsequence from the elements with the given indices.
// elem_at_idxs([1, 3], [7,6,5,4,3]) == [6, 4]
template <typename Container,
    typename ContainerIdxs,
    typename T = typename Container::value_type,
    typename ContainerOut = std::vector<T>>
std::vector<T> elems_at_idxs(const ContainerIdxs& idxs, const Container& xs)
{
    static_assert(std::is_same<typename ContainerIdxs::value_type, std::size_t>::value,
        "Indices must be std::size_t");
    ContainerOut result;
    internal::prepare_container(result, size_of_cont(idxs));
    auto itOut = internal::get_back_inserter(result);
    for (std::size_t idx : idxs)
    {
        *itOut = elem_at_idx(idx, xs);
    }
    return result;
}

namespace internal
{

template <typename Container, typename F>
Container transform(internal::reuse_container_t, F f, Container&& xs)
{
    internal::trigger_static_asserts<internal::unary_function_tag,
                                         F,
                                         decltype(*std::begin(xs))>();
    std::transform(std::begin(xs), std::end(xs), std::begin(xs), f);
    return std::forward<Container>(xs);
}

template <typename ContainerOut, typename F, typename ContainerIn>
ContainerOut transform(internal::create_new_container_t, F f,
    const ContainerIn& xs)
{
    internal::trigger_static_asserts<internal::unary_function_tag,
                                         F,
                                         decltype(*std::begin(xs))>();
    ContainerOut ys;
    internal::prepare_container(ys, size_of_cont(xs));
    auto it = internal::get_back_inserter<ContainerOut>(ys);
    std::transform(std::begin(xs), std::end(xs), it, f);
    return ys;
}

} // namespace internal

// API search type: transform : ((a -> b), [a]) -> [b]
// fwd bind count: 1
// Apply a function to every element in a sequence.
// transform((*2), [1, 3, 4]) == [2, 6, 8]
// Also known as map or fmap.
template <typename F, typename ContainerIn,
    typename ContainerOut = typename internal::same_cont_new_t_from_unary_f<
        internal::remove_const_and_ref_t<ContainerIn>, F, 0>::type>
ContainerOut transform(F f, ContainerIn&& xs)
{
    using reuse_t = typename std::conditional<
        std::is_same<
            internal::can_reuse_v<ContainerIn>,
            internal::reuse_container_t>::value &&
        std::is_base_of<
            std::true_type,
            internal::has_order<ContainerIn>>::value &&
        std::is_same<
            internal::remove_const_and_ref_t<ContainerIn>,
            ContainerOut>::value,
        internal::reuse_container_t,
        internal::create_new_container_t>::type;
    return internal::transform<ContainerOut>(
        reuse_t{}, f, std::forward<ContainerIn>(xs));
}

// API search type: transform_convert : ((a -> b), [a]) -> [b]
// fwd bind count: 1
// transform_convert((*2), [1, 3, 4]) == [2, 6, 8]
// Same as transform, but makes it easy to
// use an output container type different from the input container type.
template <typename ContainerOut, typename F, typename ContainerIn>
ContainerOut transform_convert(F f, const ContainerIn& xs)
{
    internal::trigger_static_asserts<internal::unary_function_tag, F, typename ContainerIn::value_type>();
    ContainerOut ys;
    internal::prepare_container(ys, size_of_cont(xs));
    auto it = internal::get_back_inserter<ContainerOut>(ys);
    std::transform(std::begin(xs), std::end(xs), it, f);
    return ys;
}

// API search type: transform_inner : ((a -> b), [[a]]) -> [[b]]
// fwd bind count: 1
// Applies a function to the elements of the inner containers
// of a nested sequence.
// transform_inner((*2), [[1, 3, 4], [1, 2]]) == [[2, 6, 8], [2, 4]]
// Also known as transform_nested, map_nested or map_inner.
template <typename F, typename ContainerIn,
    typename ContainerOut =
        typename internal::same_cont_new_t<
            ContainerIn,
            typename internal::same_cont_new_t_from_unary_f<
                typename ContainerIn::value_type, F, 0
            >::type, 0
        >::type>
ContainerOut transform_inner(F f, const ContainerIn& xs)
{
    internal::trigger_static_asserts<internal::unary_function_tag, F, typename ContainerIn::value_type::value_type>();
    return fplus::transform(
        fplus::bind_1st_of_2(
            fplus::transform<F, const typename ContainerIn::value_type&>, f),
        xs);
}

namespace internal
{

template <typename Container>
Container reverse(internal::reuse_container_t, Container&& xs)
{
    static_assert(internal::has_order<Container>::value,
        "Reverse: Container has no order.");
    std::reverse(std::begin(xs), std::end(xs));
    return std::forward<Container>(xs);
}

template <typename Container>
Container reverse(internal::create_new_container_t, const Container& xs)
{
    static_assert(internal::has_order<Container>::value,
        "Reverse: Container has no order.");
    Container ys = xs;
    std::reverse(std::begin(ys), std::end(ys));
    return ys;
}

} // namespace internal

// API search type: reverse : [a] -> [a]
// fwd bind count: 0
// Reverse a sequence.
// reverse([0,4,2,6]) == [6,2,4,0]
template <typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut reverse(Container&& xs)
{
    return internal::reverse(internal::can_reuse_v<Container>{},
        std::forward<Container>(xs));
}

// API search type: take : (Int, [a]) -> [a]
// fwd bind count: 1
// Return the first n elements of a sequence xs.
// In case n >= length(xs), xs is returned.
// take(3, [0,1,2,3,4,5,6,7]) == [0,1,2]
// take(10, [0,1,2]) == [0,1,2]
template <typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut take(std::size_t amount, Container&& xs)
{
    if (amount >= size_of_cont(xs))
        return xs;
    return get_segment(0, amount, std::forward<Container>(xs));
}

// API search type: take_exact : (Int, [a]) -> [a]
// fwd bind count: 1
// Return exactly the first n elements of a sequence xs.
// Unsafe! Crashes then sequence is too short.
// take_exact(3, [0,1,2,3,4,5,6,7]) == [0,1,2]
// take_exact(10, [0,1,2]) == crash
template <typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut take_exact(std::size_t amount, Container&& xs)
{
    return get_segment(0, amount, std::forward<Container>(xs));
}

// API search type: take_cyclic : (Int, [a]) -> [a]
// fwd bind count: 1
// Takes n elements from a sequence considering it as cyclic.
// take_cyclic(5, [0,1,2,3]) == [0,1,2,3,0]
// take_cyclic(7, [0,1,2,3]) == [0,1,2,3,0,1,2]
// take_cyclic(7, [0,1]) == [0,1,0,1,0,1,0]
// take_cyclic(2, [0,1,2,3]) == [0,1]
// take_cyclic(3, [0]) == [0,0,0]
// take_cyclic(3, []) == crash!
// Also known as take_wrap.
// xs must be non-empty.
template <typename Container>
Container take_cyclic(std::size_t amount, const Container& xs)
{
    assert(!xs.empty());

    Container ys;
    internal::prepare_container(ys, size_of_cont(xs));
    auto it_out = internal::get_back_inserter(ys);
    auto it_in = std::begin(xs);

    while (amount != 0)
    {
        *it_out = *it_in;
        --amount;
        ++it_in;
        if (it_in == std::end(xs))
        {
            it_in = std::begin(xs);
        }
    }
    return ys;
}

// API search type: drop : (Int, [a]) -> [a]
// fwd bind count: 1
// Skip the first n elements of a sequence xs.
// If n > length(xs) an empty sequence is returned.
// drop(3, [0,1,2,3,4,5,6,7]) == [3,4,5,6,7]
// Also known as skip.
template <typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut drop(std::size_t amount, Container&& xs)
{
    if (amount >= size_of_cont(xs))
        return ContainerOut();
    return get_segment(amount, size_of_cont(xs), std::forward<Container>(xs));
}

// API search type: take_last : (Int, [a]) -> [a]
// fwd bind count: 1
// Return the last n elements of a sequence xs.
// In case n >= length(xs), xs is returned.
// take_last(3, [0,1,2,3,4,5,6,7]) == [5,6,7]
// take_last(10, [0,1,2]) == [0,1,2]
template <typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut take_last(std::size_t amount, Container&& xs)
{
    if (amount >= size_of_cont(xs))
        return xs;
    return drop(size_of_cont(xs) - amount, std::forward<Container>(xs));
}

// API search type: drop_last : (Int, [a]) -> [a]
// fwd bind count: 1
// Skip the last n elements of a sequence xs.
// If n > length(xs) an empty sequence is returned.
// drop_last(3, [0,1,2,3,4,5,6,7]) == [0,1,2,3,4]
template <typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut drop_last(std::size_t amount, Container&& xs)
{
    if (amount >= size_of_cont(xs))
        return ContainerOut();
    return take(size_of_cont(xs) - amount, std::forward<Container>(xs));
}

// API search type: drop_exact : (Int, [a]) -> [a]
// fwd bind count: 1
// Skip exactly the first n elements of a sequence xs.
// Unsafe! Crashes when xs is too short.
// drop_exact(3, [0,1,2,3,4,5,6,7]) == [3,4,5,6,7]
// drop_exact(10, [0,1,2,3,4,5,6,7]) == crash
template <typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut drop_exact(std::size_t amount, Container&& xs)
{
    return get_segment(amount, size_of_cont(xs), std::forward<Container>(xs));
}

// API search type: take_while : ((a -> Bool), [a]) -> [a]
// fwd bind count: 1
// Take elements from the beginning of a sequence
// as long as they are fulfilling a predicate.
// take_while(is_even, [0,2,4,5,6,7,8]) == [0,2,4]
template <typename Container, typename UnaryPredicate>
Container take_while(UnaryPredicate pred, const Container& xs)
{
    internal::check_unary_predicate_for_container<UnaryPredicate, Container>();
    auto itFirst = std::find_if(
        std::begin(xs), std::end(xs), logical_not(pred));
    if (itFirst == std::end(xs))
        return xs;
    return Container(std::begin(xs), itFirst);
}

// API search type: take_last_while : ((a -> Bool), [a]) -> [a]
// fwd bind count: 1
// Take elements from the beginning of a sequence
// as long as they are fulfilling a predicate.
// take_last_while(is_even, [0,2,7,5,6,4,8]) == [6,4,8]
template <typename Container, typename UnaryPredicate>
Container take_last_while(UnaryPredicate pred, const Container& xs)
{
    internal::check_unary_predicate_for_container<UnaryPredicate, Container>();
    const auto r_begin = internal::make_reverse_iterator(std::end(xs));
    const auto r_end = internal::make_reverse_iterator(std::begin(xs));
    const auto itFirstReverse = std::find_if(r_begin, r_end, logical_not(pred));
    if (itFirstReverse == r_begin)
        return Container();
    if (itFirstReverse == r_end)
        return xs;
    return Container(itFirstReverse.base(), std::end(xs));
}

// API search type: drop_while : ((a -> Bool), [a]) -> [a]
// fwd bind count: 1
// Remove elements from the beginning of a sequence
// as long as they are fulfilling a predicate.
// drop_while(is_even, [0,2,4,5,6,7,8]) == [5,6,7,8]
// Also known as trim_left_by.
template <typename Container, typename UnaryPredicate>
Container drop_while(UnaryPredicate pred, const Container& xs)
{
    internal::check_unary_predicate_for_container<UnaryPredicate, Container>();
    auto itFirstNot = std::find_if_not(std::begin(xs), std::end(xs), pred);
    if (itFirstNot == std::end(xs))
        return Container();
    return Container(itFirstNot, std::end(xs));
}

// API search type: drop_last_while : ((a -> Bool), [a]) -> [a]
// fwd bind count: 1
// Remove elements from the beginning of a sequence
// as long as they are fulfilling a predicate.
// drop_last_while(is_even, [0,2,7,5,6,4,8]) == [0,2,7,5]
template <typename Container, typename UnaryPredicate>
Container drop_last_while(UnaryPredicate pred, const Container& xs)
{
    internal::check_unary_predicate_for_container<UnaryPredicate, Container>();
    const auto r_begin = internal::make_reverse_iterator(std::end(xs));
    const auto r_end = internal::make_reverse_iterator(std::begin(xs));
    const auto itFirstNotReverse = std::find_if_not(r_begin, r_end, pred);
    if (itFirstNotReverse == r_begin)
        return xs;
    if (itFirstNotReverse == r_end)
        return Container();
    return Container(std::begin(xs), itFirstNotReverse.base());
}

// API search type: fold_left : (((a, b) -> a), a, [b]) -> a
// fwd bind count: 2
// fold_left((+), 0, [1, 2, 3]) == ((0+1)+2)+3 == 6
// Takes the second argument and the first item of the list
// and applies the function to them,
// then feeds the function with this result and the second argument and so on.
template <typename F, typename Container, typename Acc>
Acc fold_left(F f, const Acc& init, const Container& xs)
{
    using std::begin;
    using std::end;

    return internal::accumulate(begin(xs), end(xs), init, f);
}

// API search type: reduce : (((a, a) -> a), a, [a]) -> a
// fwd bind count: 2
// reduce((+), 0, [1, 2, 3]) == (0+1+2+3) == 6
// Combines the initial value and all elements of the sequence
// using the given function.
// The set of f, init and value_type should form a monoid.
template <typename F, typename Container>
typename Container::value_type reduce(
    F f, const typename Container::value_type& init, const Container& xs)
{
    return fold_left(f, init, xs);
}

// API search type: fold_left_1 : (((a, a) -> a), [a]) -> a
// fwd bind count: 1
// fold_left_1((+), [1, 2, 3]) == (1+2)+3 == 6
// Takes the first 2 items of the list and applies the function to them,
// then feeds the function with this result and the third argument and so on.
// xs must be non-empty.
template <typename F, typename Container>
auto fold_left_1(F f, const Container& xs)
{
    assert(!xs.empty());

    using std::begin;
    using std::end;

    const auto it = begin(xs);
    return internal::accumulate(std::next(it), end(xs), *it, f);
}

// API search type: reduce_1 : (((a, a) -> a), [a]) -> a
// fwd bind count: 1
// reduce_1((+), [1, 2, 3]) == (1+2+3) == 6
// Joins all elements of the sequence using the given function.
// The set of f and value_type should form a semigroup.
template <typename F, typename Container>
typename Container::value_type reduce_1(F f, const Container& xs)
{
    assert(!xs.empty());
    return fold_left_1(f, xs);
}

// API search type: fold_right : (((a, b) -> b), b) -> [a] -> b
// fwd bind count: 2
// fold_right((+), 0, [1, 2, 3]) == 1+(2+(3+0)) == 6
// Takes the second argument and the last item of the list
// and applies the function,
// then it takes the penultimate item from the end and the result, and so on.
template <typename F, typename Container, typename Acc>
Acc fold_right(F f, const Acc& init, const Container& xs)
{
    return internal::accumulate(xs.rbegin(), xs.rend(), init, flip(f));
}

// API search type: fold_right_1 : (((a, a) -> a), [a]) -> a
// fwd bind count: 1
// fold_right_1((+), [1, 2, 3]) == 1+(2+3)) == 6
// Takes the last two items of the list and applies the function,
// then it takes the third item from the end and the result, and so on.
template <typename F, typename Container>
auto fold_right_1(F f, const Container& xs)
{
    assert(!xs.empty());
    const auto it = xs.rbegin();
    return internal::accumulate(std::next(it), xs.rend(), *it, flip(f));
}

// API search type: scan_left : (((a, b) -> a), a, [b]) -> [a]
// fwd bind count: 2
// scan_left((+), 0, [1, 2, 3]) == [0, 1, 3, 6]
// Takes the second argument and the first item of the list
// and applies the function to them,
// then feeds the function with this result and the second argument and so on.
// It returns the list of intermediate and final results.
template <typename F, typename ContainerIn, typename Acc>
auto scan_left(F f, const Acc& init, const ContainerIn& xs)
{
    using ContainerOut =
        typename internal::same_cont_new_t<ContainerIn, Acc, 1>::type;

    ContainerOut result;
    internal::prepare_container(result, size_of_cont(xs) + 1);

    using std::begin;
    using std::end;

    internal::scan_impl(
        f, init, internal::get_back_inserter(result), begin(xs), end(xs));
    return result;
}

// API search type: scan_left_1 : (((a, a) -> a), [a]) -> [a]
// fwd bind count: 1
// scan_left_1((+), [1, 2, 3]) == [1, 3, 6]
// Takes the first 2 items of the list and applies the function to them,
// then feeds the function with this result and the third argument and so on.
// It returns the list of intermediate and final results.
// xs must be non-empty.
template <typename F, typename ContainerIn>
auto scan_left_1(F f, const ContainerIn& xs)
{
    assert(!xs.empty());

    using std::begin;
    using std::end;

    const auto beginIt = begin(xs);

    using ContainerOut = typename internal::same_cont_new_t<
        ContainerIn,
        internal::uncvref_t<decltype(*beginIt)>,
        0>::type;

    ContainerOut result;
    internal::prepare_container(result, size_of_cont(xs));
    internal::scan_impl(f,
                      *beginIt,
                      internal::get_back_inserter(result),
                      std::next(beginIt),
                      end(xs));
    return result;
}

// API search type: scan_right : (((a, b) -> b), b, [a]) -> [b]
// fwd bind count: 2
// scan_right((+), 0, [1, 2, 3]) == [6, 5, 3, 0]
// Takes the second argument and the last item of the list
// and applies the function,
// then it takes the penultimate item from the end and the result, and so on.
// It returns the list of intermediate and final results.
template <typename F, typename ContainerIn,
    typename Acc = typename utils::function_traits<F>::template arg<1>::type,
    typename ContainerOut = typename internal::same_cont_new_t<ContainerIn, Acc, 1>::type>
ContainerOut scan_right(F f, const Acc& init, const ContainerIn& xs)
{
    return reverse(scan_left(flip(f), init, reverse(xs)));
}

// API search type: scan_right_1 : (((a, a) -> a), [a]) -> [a]
// fwd bind count: 1
// scan_right_1((+), [1, 2, 3]) == [6, 5, 3]
// Takes the last two items of the list and applies the function,
// then it takes the third item from the end and the result, and so on.
// It returns the list of inntermediate and final results.
template <typename F, typename ContainerIn,
    typename Acc = typename ContainerIn::value_type,
    typename ContainerOut = typename internal::same_cont_new_t<ContainerIn, Acc, 0>::type>
ContainerOut scan_right_1(F f, const ContainerIn& xs)
{
    return reverse(scan_left_1(flip(f), reverse(xs)));
}

// API search type: sum : [a] -> a
// fwd bind count: 0
// Adds up all values in a sequence.
// sum([0,3,1]) == 4
// sum([]) == 0
template <typename Container,
    typename T = typename Container::value_type>
T sum(const Container& xs)
{
    T result = T();
    for (const auto& x : xs)
    {
        result = result + x;
    }
    return result;
}

// API search type: product : [a] -> a
// fwd bind count: 0
// Returns the product of all values in a sequence.
// product([3,1,2]) == 6
// product([]) == 1
template <typename Container,
    typename T = typename Container::value_type>
T product(const Container& xs)
{
    T result{1};
    for (const auto& x : xs)
    {
        result = result * x;
    }
    return result;
}

namespace internal
{

template <typename T, typename Container>
Container append_elem(internal::reuse_container_t, const T& y, Container&& xs)
{
    *internal::get_back_inserter(xs) = y;
    return std::forward<Container>(xs);
}

template <typename T, typename Container>
Container append_elem(internal::create_new_container_t, const T& y,
    const Container& xs)
{
    Container result;
    internal::prepare_container(result, size_of_cont(xs) + 1);
    std::copy(std::begin(xs), std::end(xs),
        internal::get_back_inserter(result));
    *internal::get_back_inserter(result) = y;
    return result;
}

} // namespace internal

// API search type: append_elem : (a, [a]) -> [a]
// fwd bind count: 1
// Extends a sequence with one element at the back.
// append_elem([1, 2], 3) == [1, 2, 3]
template <typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>,
    typename T = typename ContainerOut::value_type>
ContainerOut append_elem(const T& y, Container&& xs)
{
    return internal::append_elem(internal::can_reuse_v<Container>{},
        y, std::forward<Container>(xs));
}

namespace internal
{

template <typename T>
std::list<T> prepend_elem(internal::reuse_container_t,
    const T& y, std::list<T>&& xs)
{
    xs.push_front(y);
    return std::forward<std::list<T>>(xs);
}

template <typename T, typename Container>
Container prepend_elem(internal::reuse_container_t,
    const T& y, Container&& xs)
{
    xs.resize(size_of_cont(xs) + 1);
    std::copy(++xs.rbegin(), xs.rend(), xs.rbegin());
    *std::begin(xs) = y;
    return std::forward<Container>(xs);
}

template <typename T, typename Container>
Container prepend_elem(internal::create_new_container_t, const T& y,
    const Container& xs)
{
    Container result;
    internal::prepare_container(result, size_of_cont(xs) + 1);
    *internal::get_back_inserter(result) = y;
    std::copy(std::begin(xs), std::end(xs),
        internal::get_back_inserter(result));
    return result;
}

} // namespace internal

// API search type: prepend_elem : (a, [a]) -> [a]
// fwd bind count: 1
// Extends a sequence with one element in the front.
// prepend_elem([2, 3], 1) == [1, 2, 3]
template <typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>,
    typename T = typename ContainerOut::value_type>
ContainerOut prepend_elem(const T& y, Container&& xs)
{
    return internal::prepend_elem(internal::can_reuse_v<Container>{},
        y, std::forward<Container>(xs));
}

// API search type: append : ([a], [a]) -> [a]
// fwd bind count: 1
// Concatenates two sequences.
// append([1, 2], [3, 4, 5]) == [1, 2, 3, 4, 5]
template <typename ContainerIn1, typename ContainerIn2 = ContainerIn1, typename ContainerOut = ContainerIn1>
ContainerOut append(const ContainerIn1& xs, const ContainerIn2& ys)
{
    ContainerOut result;
    internal::prepare_container(result, size_of_cont(xs) + size_of_cont(ys));
    std::copy(std::begin(xs), std::end(xs),
        internal::get_back_inserter(result));
    std::copy(std::begin(ys), std::end(ys),
        internal::get_back_inserter(result));
    return result;
}


// API search type: append_convert : ([a], [a]) -> [a]
// fwd bind count: 1
// Same as append, but makes it easier to
// use an output container type different from the input container type.
template <typename ContainerOut, typename ContainerIn1, typename ContainerIn2 = ContainerIn1>
ContainerOut append_convert(const ContainerIn1& xs, const ContainerIn2& ys)
{
    return append<ContainerIn1, ContainerIn2, ContainerOut>(xs, ys);
}

// API search type: concat : [[a]] -> [a]
// fwd bind count: 0
// Concatenates multiple sequences.
// concat([[1, 2], [], [3]]) == [1, 2, 3]
// Also known as flatten.
template <typename ContainerIn,
    typename ContainerOut = typename ContainerIn::value_type>
ContainerOut concat(const ContainerIn& xss)
{
    std::size_t length = sum(
        transform(size_of_cont<typename ContainerIn::value_type>, xss));
    ContainerOut result;
    internal::prepare_container(result, length);
    using std::begin;
    using std::end;
    for(const auto& xs : xss)
    {
        result.insert(end(result), begin(xs), end(xs));
    }
    return result;
}

// API search type: interweave : ([a], [a]) -> [a]
// fwd bind count: 1
// Return a sequence that contains elements from the two provided sequences
// in alternating order. If one list runs out of items,
// appends the items from the remaining list.
// interweave([1,3], [2,4]) == [1,2,3,4]
// interweave([1,3,5,7], [2,4]) == [1,2,3,4,5,7]
// See interleave for interweaving more than two sequences.
template <typename Container>
Container interweave(const Container& xs, const Container& ys)
{
    Container result;
    internal::prepare_container(result, size_of_cont(xs) + size_of_cont(ys));
    auto it = internal::get_back_inserter<Container>(result);
    auto it_xs = std::begin(xs);
    auto it_ys = std::begin(ys);
    while (it_xs != std::end(xs) || it_ys != std::end(ys))
    {
        if (it_xs != std::end(xs))
            *it = *(it_xs++);
        if (it_ys != std::end(ys))
            *it = *(it_ys++);
    }
    return result;
}

// API search type: unweave : [a] -> ([a], [a])
// fwd bind count: 0
// Puts the elements with an even index into the first list,
// and the elements with an odd index into the second list.
// Inverse of interweave.
// unweave([0,1,2,3]) == ([0,2], [1,3])
// unweave([0,1,2,3,4]) == ([0,2,4], [1,3])
template <typename Container>
std::pair<Container, Container> unweave(const Container& xs)
{
    std::pair<Container, Container> result;
    if (size_of_cont(xs) % 2 == 0)
        internal::prepare_container(result.first, size_of_cont(xs) / 2);
    else
        internal::prepare_container(result.first, size_of_cont(xs) / 2 + 1);
    internal::prepare_container(result.second, size_of_cont(xs) / 2);
    auto it_even = internal::get_back_inserter<Container>(result.first);
    auto it_odd = internal::get_back_inserter<Container>(result.second);
    std::size_t counter = 0;
    for (const auto& x : xs)
    {
        if (counter++ % 2 == 0)
            *it_even = x;
        else
            *it_odd = x;
    }
    return result;
}

namespace internal
{

template <typename Compare, typename T>
std::list<T> sort_by(internal::reuse_container_t, Compare comp,
    std::list<T>&& xs)
{
    xs.sort(comp);
    return std::forward<std::list<T>>(xs);
}

template <typename Compare, typename T>
std::list<T> sort_by(internal::create_new_container_t, Compare comp,
    const std::list<T>& xs)
{
    auto result = xs;
    result.sort(comp);
    return result;
}

template <typename Compare, typename Container>
Container sort_by(internal::reuse_container_t, Compare comp, Container&& xs)
{
    std::sort(std::begin(xs), std::end(xs), comp);
    return std::forward<Container>(xs);
}

template <typename Compare, typename Container>
Container sort_by(internal::create_new_container_t, Compare comp,
    const Container& xs)
{
    auto result = xs;
    std::sort(std::begin(result), std::end(result), comp);
    return result;
}

} // namespace internal

// API search type: sort_by : (((a, a) -> Bool), [a]) -> [a]
// fwd bind count: 1
// Sort a sequence by given less comparator.
template <typename Compare, typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut sort_by(Compare comp, Container&& xs)
{
    return internal::sort_by(internal::can_reuse_v<Container>{},
        comp, std::forward<Container>(xs));
}

namespace internal
{
    // workarounds for clang bug 24115
    // (std::sort and std::unique with std::function as comp)
    // https://llvm.org/bugs/show_bug.cgi?id=24115
    template <typename F>
    struct is_less_by_struct
    {
        is_less_by_struct(F f) : f_(f) {};
        template <typename T>
        bool operator()(const T& x, const T& y)
        {
            return f_(x) < f_(y);
        }
    private:
        F f_;
    };
    template <typename F>
    struct is_equal_by_struct
    {
        is_equal_by_struct(F f) : f_(f) {};
        template <typename T>
        bool operator()(const T& x, const T& y)
        {
            return f_(x) == f_(y);
        }
    private:
        F f_;
    };
}

// API search type: sort_on : ((a -> b), [a]) -> [a]
// fwd bind count: 1
// Sort a sequence by a given transformer.
template <typename F, typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut sort_on(F f, Container&& xs)
{
    return sort_by(internal::is_less_by_struct<F>(f),
        std::forward<Container>(xs));
}

// API search type: sort : [a] -> [a]
// fwd bind count: 0
// Sort a sequence to ascending order using std::less.
template <typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut sort(Container&& xs)
{
    typedef typename std::remove_reference<Container>::type::value_type T;
    return sort_by(std::less<T>(), std::forward<Container>(xs));
}

namespace internal
{

template <typename Compare, typename T>
std::list<T> stable_sort_by(internal::reuse_container_t, Compare comp,
    std::list<T>&& xs)
{
    xs.sort(comp); // std::list<T>::sort ist already stable.
    return std::forward<std::list<T>>(xs);
}

template <typename Compare, typename T>
std::list<T> stable_sort_by(internal::create_new_container_t, Compare comp,
    const std::list<T>& xs)
{
    auto result = xs;
    result.sort(comp); // std::list<T>::sort ist already stable.
    return result;
}

template <typename Compare, typename Container>
Container stable_sort_by(internal::reuse_container_t, Compare comp,
    Container&& xs)
{
    std::sort(std::begin(xs), std::end(xs), comp);
    return std::forward<Container>(xs);
}

template <typename Compare, typename Container>
Container stable_sort_by(internal::create_new_container_t, Compare comp,
    const Container& xs)
{
    auto result = xs;
    std::sort(std::begin(result), std::end(result), comp);
    return result;
}

} // namespace internal


// API search type: stable_sort_by : (((a, a) -> Bool), [a]) -> [a]
// fwd bind count: 1
// Sort a sequence stably by given less comparator.
template <typename Compare, typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut stable_sort_by(Compare comp, Container&& xs)
{
    return internal::stable_sort_by(internal::can_reuse_v<Container>{},
        comp, std::forward<Container>(xs));
}

// API search type: stable_sort_on : ((a -> b), [a]) -> [a]
// fwd bind count: 1
// Sort a sequence stably by given transformer.
template <typename F, typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut stable_sort_on(F f, Container&& xs)
{
    return stable_sort_by(internal::is_less_by_struct<F>(f),
        std::forward<Container>(xs));
}

// API search type: stable_sort : [a] -> [a]
// fwd bind count: 0
// Sort a sequence stably to ascending order using std::less.
template <typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut stable_sort(Container&& xs)
{
    typedef typename std::remove_reference<Container>::type::value_type T;
    return stable_sort_by(std::less<T>(), std::forward<Container>(xs));
}

namespace internal
{

template <typename Compare, typename Container>
Container partial_sort_by(internal::reuse_container_t, Compare comp,
    std::size_t count, Container&& xs)
{
    if (count > xs.size())
    {
        count = xs.size();
    }
    auto middle = std::begin(xs);
    internal::advance_iterator(middle, count);
    std::partial_sort(std::begin(xs), middle, std::end(xs), comp);
    return std::forward<Container>(get_segment(internal::reuse_container_t(),
        0, count, xs));
}

template <typename Compare, typename Container>
Container partial_sort_by(internal::create_new_container_t, Compare comp,
    std::size_t count, const Container& xs)
{
    auto result = xs;
    return partial_sort_by(
        internal::reuse_container_t(), comp, count, std::move(result));
}

} // namespace internal

// API search type: partial_sort_by : (((a, a) -> Bool), Int, [a]) -> [a]
// fwd bind count: 2
// Partially sort a sequence by a given less comparator.
// Returns only the sorted segment.
template <typename Compare, typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut partial_sort_by(Compare comp, std::size_t count, Container&& xs)
{
    return internal::partial_sort_by(internal::can_reuse_v<Container>{},
        comp, count, std::forward<Container>(xs));
}

// API search type: partial_sort_on : ((a -> b), Int, [a]) -> [a]
// fwd bind count: 2
// Partially sort a sequence by a given transformer.
// Returns only the sorted segment.
template <typename F, typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut partial_sort_on(F f, std::size_t count, Container&& xs)
{
    return partial_sort_by(internal::is_less_by_struct<F>(f), count,
        std::forward<Container>(xs));
}

// API search type: partial_sort : (Int, [a]) -> [a]
// fwd bind count: 1
// Partially sort a sequence in ascending order using std::less.
// Returns only the sorted segment.
template <typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut partial_sort(std::size_t count, Container&& xs)
{
    typedef typename std::remove_reference<Container>::type::value_type T;
    return partial_sort_by(std::less<T>(), count,
        std::forward<Container>(xs));
}

// API search type: nth_element_by : (((a, a) -> Bool), Int, [a]) -> a
// fwd bind count: 2
// Return the nth largest element of a sequence by a given less comparator.
template <typename Compare, typename Container,
    typename T = typename Container::value_type>
T nth_element_by(Compare comp, std::size_t n, const Container& xs)
{
    assert(n < xs.size());
    auto result = xs;
    auto middle = std::begin(result);
    internal::advance_iterator(middle, n);
    std::nth_element(std::begin(result), middle, std::end(result), comp);
    return *middle;
}

// API search type: nth_element_on : ((a -> b), Int, [a]) -> a
// fwd bind count: 2
// Return the nth largest element of a sequence by a given transformer.
template <typename F, typename Container,
    typename T = typename Container::value_type>
T nth_element_on(F f, std::size_t n, const Container& xs)
{
    return nth_element_by(internal::is_less_by_struct<F>(f), n, xs);
}

// API search type: nth_element : (Int, [a]) -> a
// fwd bind count: 1
// Return the nth largest element of a sequence using std::less.
template <typename Container,
    typename T = typename Container::value_type>
T nth_element(std::size_t n, const Container& xs)
{
    return nth_element_by(std::less<T>(), n, xs);
}

namespace internal
{

template <typename BinaryPredicate, typename Container>
Container unique_by(internal::reuse_container_t,
    BinaryPredicate pred, Container&& xs)
{
    internal::check_binary_predicate_for_container<BinaryPredicate, Container>();
    const auto it_end = std::unique(std::begin(xs), std::end(xs), pred);
    xs.erase(it_end, std::end(xs));
    return std::forward<Container>(xs);
}

template <typename BinaryPredicate, typename Container>
Container unique_by(internal::create_new_container_t,
    BinaryPredicate pred, const Container& xs)
{
    auto result = xs;
    return unique_by(internal::reuse_container_t(), pred, std::move(result));
}

} // namespace internal

// API search type: unique_by : (((a, a) -> Bool), [a]) -> [a]
// fwd bind count: 1
// Like unique, eliminates all but the first element
// from every consecutive group of equivalent elements from the sequence;
// but with a user supplied equality predicate.
// See nub_by for making elements globally unique in a sequence.
// O(n)
template <typename BinaryPredicate, typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut unique_by(BinaryPredicate pred, Container&& xs)
{
    return internal::unique_by(internal::can_reuse_v<Container>{},
        pred, std::forward<Container>(xs));
}

// API search type: unique_on : ((a -> b), [a]) -> [a]
// fwd bind count: 1
// Like unique, eliminates all but the first element
// from every consecutive group of equivalent elements from the sequence;
// but with a user supplied transformation (e.g. getter).
// See nub_on for making elements globally unique in a sequence.
// O(n)
// Also known as drop_repeats.
template <typename F, typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut unique_on(F f, Container&& xs)
{
    return unique_by(internal::is_equal_by_struct<F>(f),
        std::forward<Container>(xs));
}

// API search type: unique : [a] -> [a]
// fwd bind count: 0
// Eliminates all but the first element
// from every consecutive group of equivalent elements from the sequence.
// unique([1,2,2,3,2]) == [1,2,3,2]
// See nub for making elements globally unique in a sequence.
// O(n)
template <typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut unique(Container&& xs)
{
    typedef typename std::remove_reference<Container>::type::value_type T;
    return unique_on(identity<T>, std::forward<Container>(xs));
}

// API search type: intersperse : (a, [a]) -> [a]
// fwd bind count: 1
// Insert a value between all adjacent values in a sequence.
// intersperse(0, [1, 2, 3]) == [1, 0, 2, 0, 3]
// Also known as interpose.
template <typename Container,
    typename X = typename Container::value_type>
Container intersperse(const X& value, const Container& xs)
{
    if (xs.empty())
        return Container();
    if (size_of_cont(xs) == 1)
        return xs;
    Container result;
    internal::prepare_container(result, std::max<std::size_t>(0, size_of_cont(xs)*2-1));
    auto it = internal::get_back_inserter(result);
    for_each(std::begin(xs), --std::end(xs), [&value, &it](const X& x)
    {
        *it = x;
        *it = value;
    });
    *it = xs.back();
    return result;
}

// API search type: join : ([a], [[a]]) -> [a]
// fwd bind count: 1
// Inserts a separator sequence in between the elements
// of a sequence of sequences and concatenates the result.
// Also known as intercalate or implode.
// join(", ", ["a", "bee", "cee"]) == "a, bee, cee"
// join([0, 0], [[1], [2], [3, 4]]) == [1, 0, 0, 2, 0, 0, 3, 4]
template <typename Container,
    typename X = typename Container::value_type>
X join(const X& separator, const Container& xs)
{
    return concat(intersperse(separator, xs));
}

// API search type: join_elem : (a, [[a]]) -> [a]
// fwd bind count: 1
// Inserts a separator in between the elements
// of a sequence of sequences and concatenates the result.
// Also known as intercalate_elem.
// join_elem(';', ["a", "bee", "cee"]) == "a;bee;cee"
// join_elem(0, [[1], [2], [3, 4]]) == [1, 0, 2, 0, 3, 4]]
template <typename Container,
    typename Inner = typename Container::value_type,
    typename X = typename Inner::value_type>
Inner join_elem(const X& separator, const Container& xs)
{
    return join(Inner(1, separator), xs);
}

// API search type: is_elem_of_by : ((a -> Bool), [a]) -> Bool
// fwd bind count: 1
// Checks if at least one element of the container fulfils a predicate.
// is_elem_of_by((==), [1,2,3]) == true
template <typename UnaryPredicate, typename Container>
bool is_elem_of_by(UnaryPredicate pred, const Container& xs)
{
    return std::find_if(std::begin(xs), std::end(xs), pred) != std::end(xs);
}

// API search type: is_elem_of : (a, [a]) -> Bool
// fwd bind count: 1
// Checks if an element is a member of a container.
// is_elem_of(2, [1,2,3]) == true
// Also known as flip(contains).
template <typename Container>
bool is_elem_of(const typename Container::value_type& x, const Container& xs)
{
    return is_elem_of_by(is_equal_to(x), xs);
}

// API search type: nub_by : (((a, a) -> Bool), [a]) -> [a]
// fwd bind count: 1
// Makes the elements in a container unique with respect to a predicate
// nub_by((==), [1,2,2,3,2]) == [1,2,3]
// O(n^2)
template <typename Container, typename BinaryPredicate>
Container nub_by(BinaryPredicate p, const Container& xs)
{
    Container result;
    auto itOut = internal::get_back_inserter(result);
    for (const auto &x : xs)
    {
        auto eqToX = bind_1st_of_2(p, x);
        if (!is_elem_of_by(eqToX, result))
        {
            *itOut = x;
        }
    }
    return result;
}

// API search type: nub_on : ((a -> b), [a]) -> [a]
// fwd bind count: 1
// Makes the elements in a container unique
// with respect to their function value.
// nub_on((mod 10), [12,32,15]) == [12,15]
// O(n^2)
template <typename Container, typename F>
Container nub_on(F f, const Container& xs)
{
    return nub_by(is_equal_by(f), xs);
}

// API search type: nub : [a] -> [a]
// fwd bind count: 0
// Makes the elements in a container unique.
// nub([1,2,2,3,2]) == [1,2,3]
// O(n^2)
// Also known as distinct.
template <typename Container>
Container nub(const Container& xs)
{
    typedef typename Container::value_type T;
    return nub_by(std::equal_to<T>(), xs);
}

// API search type: all_unique_by_eq : (((a, a) -> Bool), [a]) -> Bool
// fwd bind count: 1
// Checks if all elements in a container are unique
// with respect to a predicate.
// Returns true for empty containers.
// O(n^2)
template <typename Container, typename BinaryPredicate>
bool all_unique_by_eq(BinaryPredicate p, const Container& xs)
{
    internal::check_binary_predicate_for_container<BinaryPredicate, Container>();
    return size_of_cont(nub_by(p, xs)) == size_of_cont(xs);
}

// API search type: all_unique_on : ((a -> b), [a]) -> Bool
// fwd bind count: 1
// Checks if all elements in a container are unique
// with respect to their function values.
// Returns true for empty containers.
// O(n^2)
template <typename Container, typename F>
bool all_unique_on(F f, const Container& xs)
{
    return all_unique_by_eq(is_equal_by(f), xs);
}

// API search type: all_unique : [a] -> Bool
// fwd bind count: 0
// Checks if all elements in a container are unique.
// Returns true for empty containers.
// O(n^2)
template <typename Container>
bool all_unique(const Container& xs)
{
    typedef typename Container::value_type T;
    auto comp = std::equal_to<T>();
    return all_unique_by_eq(comp, xs);
}

// API search type: is_strictly_sorted_by : (((a, a) -> Bool), [a]) -> Bool
// fwd bind count: 1
// Checks if a container already is strictly sorted using a predicate.
// comp(a, b) must return true only if a < b.
// O(n)
template <typename Container, typename Compare>
bool is_strictly_sorted_by(Compare comp, const Container& xs)
{
    internal::check_compare_for_container<Compare, Container>();
    if (size_of_cont(xs) < 2)
        return true;
    auto it1 = std::begin(xs);
    for (auto it2 = it1 + 1; it2 < std::end(xs); ++it1, ++it2)
        if (!internal::invoke(comp, *it1, *it2))
            return false;
    return true;
}

// API search type: is_strictly_sorted_on : ((a -> b), [a]) -> Bool
// fwd bind count: 1
// Checks if a container already is strictly sorted using a transformer.
// O(n)
template <typename Container, typename F>
bool is_strictly_sorted_on(F f, const Container& xs)
{
    return is_strictly_sorted_by(is_less_by(f), xs);
}

// API search type: is_strictly_sorted : [a] -> Bool
// fwd bind count: 0
// Checks if a container already is strictly sorted
// in ascending order using std::less.
// O(n)
template <typename Container>
bool is_strictly_sorted(const Container& xs)
{
    typedef typename Container::value_type T;
    auto comp = std::less<T>();
    return is_strictly_sorted_by(comp, xs);
}

// API search type: is_sorted_by : (((a, a) -> Bool), [a]) -> Bool
// fwd bind count: 1
// Checks if a container already is sorted using a predicate.
// comp(a, b) must return true only if a < b.
// O(n)
template <typename Container, typename Compare>
bool is_sorted_by(Compare comp, const Container& xs)
{
    internal::check_compare_for_container<Compare, Container>();
    if (size_of_cont(xs) < 2)
        return true;
    auto it1 = std::begin(xs);
    for (auto it2 = it1 + 1; it2 < std::end(xs); ++it1, ++it2)
        if (internal::invoke(comp, *it2, *it1))
            return false;
    return true;
}

// API search type: is_sorted_on : ((a -> b), [a]) -> Bool
// fwd bind count: 1
// Checks if a container already is strictly sorted using a transformer.
// O(n)
template <typename Container, typename F>
bool is_sorted_on(F f, const Container& xs)
{
    return is_sorted_by(is_less_by(f), xs);
}

// API search type: is_sorted : [a] -> Bool
// fwd bind count: 0
// Checks if a container already is sorted
// in ascending order using std::less.
// O(n)
template <typename Container>
bool is_sorted(const Container& xs)
{
    typedef typename Container::value_type T;
    auto comp = std::less<T>();
    return is_sorted_by(comp, xs);
}

// API search type: is_prefix_of : ([a], [a]) -> Bool
// fwd bind count: 1
// Checks if a containers starts with a token.
// is_prefix_of("Fun", "FunctionalPlus") == true
template <typename Container>
bool is_prefix_of(const Container& token, const Container& xs)
{
    if (size_of_cont(token) > size_of_cont(xs))
        return false;
    return get_segment(0, size_of_cont(token), xs) == token;
}

// API search type: is_suffix_of : ([a], [a]) -> Bool
// fwd bind count: 1
// Checks if a containers contains a token as a segment.
// is_suffix_of("us", "FunctionalPlus") == true
template <typename Container>
bool is_suffix_of(const Container& token, const Container& xs)
{
    if (size_of_cont(token) > size_of_cont(xs))
        return false;
    return get_segment(size_of_cont(xs) - size_of_cont(token),
        size_of_cont(xs), xs) == token;
}

// API search type: all_by : ((a -> Bool), [a]) -> Bool
// fwd bind count: 1
// Checks if a containers contains a token as a segment.
// all_by(is_even, [2, 4, 6]) == true
// Returns true for empty containers.
template <typename UnaryPredicate, typename Container>
bool all_by(UnaryPredicate p, const Container& xs)
{
    internal::check_unary_predicate_for_container<UnaryPredicate, Container>();
    return std::all_of(std::begin(xs), std::end(xs), p);
}

// API search type: all : [Bool] -> Bool
// fwd bind count: 0
// Checks if all values in a container evaluate to true.
// all([true, false, true]) == false
// Returns true for empty containers.
template <typename Container>
bool all(const Container& xs)
{
    typedef typename Container::value_type T;
    return all_by(identity<T>, xs);
}

// API search type: all_the_same_by : (((a, a) -> Bool), [a]) -> Bool
// fwd bind count: 1
// Checks if all values in a container are equal using a binary predicate.
// Returns true for empty containers.
template <typename Container, typename BinaryPredicate>
bool all_the_same_by(BinaryPredicate p, const Container& xs)
{
    internal::check_binary_predicate_for_container<BinaryPredicate, Container>();
    if (size_of_cont(xs) < 2)
        return true;
    auto unaryPredicate = bind_1st_of_2(p, xs.front());
    return all_by(unaryPredicate, xs);
}

// API search type: all_the_same_on : ((a -> b), [a]) -> Bool
// fwd bind count: 1
// Checks if all values in a container are equal using a transformer.
// Returns true for empty containers.
template <typename Container, typename F>
bool all_the_same_on(F f, const Container& xs)
{
    if (size_of_cont(xs) < 2)
        return true;
    auto unaryPredicate = is_equal_by_to(f, f(xs.front()));
    return all_by(unaryPredicate, xs);
}

// API search type: all_the_same : [a] -> Bool
// fwd bind count: 0
// Checks if all values in a container are equal.
// Returns true for empty containers.
template <typename Container>
bool all_the_same(const Container& xs)
{
    typedef typename Container::value_type T;
    auto binaryPredicate = std::equal_to<T>();
    return all_the_same_by(binaryPredicate, xs);
}

// API search type: numbers_step : (a, a, a) -> [a]
// fwd bind count: 2
// Return a sequence of numbers using a specific step.
// numbers_step(2, 9, 2) == [2, 4, 6, 8]
template <typename T,
        typename ContainerOut = std::vector<T>>
ContainerOut numbers_step
        (const T start, const T end, const T step)
{
    ContainerOut result;
    if ((step > 0 && start >= end) ||
        (step < 0 && start <= end) ||
        step == 0)
    {
        return result;
    }
    std::size_t size = static_cast<std::size_t>((end - start) / step);
    internal::prepare_container(result, size);
    auto it = internal::get_back_inserter<ContainerOut>(result);
    for (T x = start; x < end; x += step)
        *it = x;
    return result;
}

// API search type: numbers : (a, a) -> [a]
// fwd bind count: 1
// Return an ascending sequence of numbers..
// Also known as range.
// numbers(2, 9) == [2, 3, 4, 5, 6, 7, 8]
template <typename T,
        typename ContainerOut = std::vector<T>>
ContainerOut numbers(const T start, const T end)
{
    return numbers_step<T, ContainerOut>(start, end, 1);
}

// API search type: singleton_seq : a -> [a]
// fwd bind count: 0
// Construct a sequence containing a single value.
// singleton_seq(3) == [3].
template <typename T, typename ContainerOut = std::vector<T>>
ContainerOut singleton_seq(const T& x)
{
    return ContainerOut(1, x);
}

// API search type: all_idxs : [a] -> [Int]
// fwd bind count: 0
// Returns a vector containing all valid indices of sequence xs.
// all_idxs([6,4,7,6]) == [0,1,2,3]
template <typename Container>
std::vector<std::size_t> all_idxs(const Container& xs)
{
    return numbers<std::size_t>(0, size_of_cont(xs));
}

// API search type: init : [a] -> [a]
// fwd bind count: 0
// init([0,1,2,3]) == [0,1,2]
// Unsafe! xs must be non-empty.
template <typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut init(Container&& xs)
{
    assert(!is_empty(xs));
    return get_segment(0, size_of_cont(std::forward<Container>(xs)) - 1, xs);
}

// API search type: tail : [a] -> [a]
// fwd bind count: 0
// Drops the first element of a container, keeps the rest. Unsafe!
// tail([0,1,2,3]) == [1,2,3]
// Unsafe! xs must be non-empty.
template <typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut tail(Container&& xs)
{
    assert(!is_empty(xs));
    return get_segment(1, size_of_cont(std::forward<Container>(xs)), xs);
}

// API search type: head : [a] -> a
// fwd bind count: 0
// Return the first element of a container.
// head([0,1,2,3]) == 0
// Unsafe! xs must be non-empty.
template <typename Container>
typename Container::value_type head(const Container& xs)
{
    assert(!is_empty(xs));
    return xs.front();
}

// API search type: last : [a] -> a
// fwd bind count: 0
// Return the last element of a container.
// last([0,1,2,3]) == 3
// Unsafe! xs must be non-empty.
template <typename Container>
typename Container::value_type last(const Container& xs)
{
    assert(!is_empty(xs));
    return xs.back();
}

// API search type: mean_stddev : [a] -> (a, a)
// fwd bind count: 0
// Calculates the mean and the population standard deviation.
// mean_stddev([4, 8]) == (6, 2)
// mean_stddev([1, 3, 7, 4]) == (3.75, 2.5)
// xs must be non-empty.
template <typename Result, typename Container>
std::pair<Result, Result> mean_stddev(const Container& xs)
{
    assert(size_of_cont(xs) != 0);

    // http://stackoverflow.com/a/7616783/1866775
    Result sum = static_cast<Result>(
        internal::accumulate(xs.begin(), xs.end(),
            static_cast<typename Container::value_type>(0)));
    Result mean = sum / static_cast<Result>(xs.size());

    std::vector<Result> diff(xs.size());
    std::transform(xs.begin(), xs.end(), diff.begin(),
        [mean](Result x)
        {
            return x - mean;
        });
    Result sq_sum = std::inner_product(
        diff.begin(), diff.end(), diff.begin(), static_cast<Result>(0));
    Result stddev = std::sqrt(sq_sum / static_cast<Result>(xs.size()));
    return std::make_pair(mean, stddev);
}

// API search type: count_occurrences_by : ((a -> b), [a]) -> Map b Int
// fwd bind count: 1
// Returns a discrete frequency distribution of the elements in a container
// applying a specific transformer.
// count_occurrences_by(floor, [1.1, 2.3, 2.7, 3.6, 2.4]) == [(1, 1), (2, 3), (3, 1)]
// O(n)
template <typename F, typename ContainerIn>
auto count_occurrences_by(F f, const ContainerIn& xs)
{
    using In = typename ContainerIn::value_type;
    using MapOut =
        std::map<std::decay_t<internal::invoke_result_t<F, In>>, std::size_t>;

    internal::trigger_static_asserts<internal::unary_function_tag, F, typename ContainerIn::value_type>();
    MapOut result;
    for (const auto& x : xs)
    {
        ++result[internal::invoke(f, x)];
    }
    return result;
}

// API search type: count_occurrences : [a] -> Map a Int
// fwd bind count: 0
// Returns a discrete frequency distribution of the elements in a container
// applying a specific transformer.
// Can be used to create a histogram.
// count_occurrences([1,2,2,3,2]) == [(1, 1), (2, 3), (3, 1)]
// O(n)
template <typename ContainerIn,
        typename MapOut = typename std::map<
            typename ContainerIn::value_type, std::size_t>>
MapOut count_occurrences(const ContainerIn& xs)
{
    return count_occurrences_by(identity<typename ContainerIn::value_type>, xs);
}

// API search type: lexicographical_less_by : (((a, a) -> Bool), [a], [a]) -> Bool
// fwd bind count: 2
// Lexicographical less-than comparison using a specific predicate.
// lexicographical_less_by((<), [0,1,2,2,4,5], [0,1,2,3,4,5]) == true
// lexicographical_less_by((<), "012245", "012345") == true
// lexicographical_less_by((<), "01234", "012345") == true
// lexicographical_less_by((<), "012345", "01234") == false
// lexicographical_less_by((<), "012345", "012345") == false
template <typename Container, typename BinaryPredicate>
bool lexicographical_less_by(BinaryPredicate p,
        const Container& xs, const Container& ys)
{
    internal::check_binary_predicate_for_container<BinaryPredicate, Container>();
    auto itXs = std::begin(xs);
    auto itYs = std::begin(ys);
    while (itXs != std::end(xs) && itYs != std::end(ys))
    {
        if (internal::invoke(p, *itXs, *itYs))
        {
            return true;
        }
        if (internal::invoke(p, *itYs, *itXs))
        {
            return false;
        }
        ++itXs;
        ++itYs;
    }
    if (size_of_cont(xs) < size_of_cont(ys))
    {
        return true;
    }
    return false;
}

// API search type: lexicographical_less : ([a], [a]) -> Bool
// fwd bind count: 1
// Lexicographical less-than comparison.
// lexicographical_less([0,1,2,2,4,5], [0,1,2,3,4,5]) == true
// lexicographical_less("012245", "012345") == true
// lexicographical_less("01234", "012345") == true
// lexicographical_less("012345", "01234") == false
// lexicographical_less("012345", "012345") == false
template <typename Container>
bool lexicographical_less(const Container& xs, const Container& ys)
{
    return lexicographical_less_by(
        is_less<typename Container::value_type>, xs, ys);
}

// API search type: lexicographical_sort : [[a]] -> [[a]]
// fwd bind count: 0
// sort by lexicographical_less
template <typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut lexicographical_sort(Container&& xs)
{
    typedef typename std::remove_reference<Container>::type::value_type T;
    return sort_by(lexicographical_less<T>, std::forward<Container>(xs));
}

// API search type: replicate : (Int, a) -> [a]
// fwd bind count: 1
// Create a sequence containing x n times.
// replicate(3, 1) == [1, 1, 1]
template <typename T,
        typename ContainerOut = std::vector<T>>
ContainerOut replicate(std::size_t n, const T& x)
{
    return ContainerOut(n, x);
}

namespace internal
{

template <typename UnaryPredicate, typename T>
T instead_of_if(internal::reuse_container_t, UnaryPredicate pred,
    const T& alt, T&& x)
{
    if (internal::invoke(pred, x))
        return alt;
    else
        return std::forward<T>(x);
}

template <typename UnaryPredicate, typename T>
T instead_of_if(internal::create_new_container_t, UnaryPredicate pred,
    const T& alt, const T& x)
{
    if (internal::invoke(pred, x))
        return alt;
    else
        return x;
}

} // namespace internal

// API search type: instead_of_if : ((a -> Bool), a, a) -> a
// fwd bind count: 2
// Return alt if pred(x), otherwise x itself.
template <typename UnaryPredicate, typename T, typename TAlt>
auto instead_of_if(UnaryPredicate pred, const TAlt& alt, T&& x)
{
    return internal::instead_of_if(internal::can_reuse_v<T>{},
        pred, alt, std::forward<T>(x));
}

// API search type: instead_of_if_empty : ((a -> Bool), [a], [a]) -> [a]
// fwd bind count: 2
// Return alt if xs is empty, otherwise xs itself.
template <typename Container, typename ContainerAlt,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut instead_of_if_empty(const ContainerAlt& alt, Container&& xs)
{
    return instead_of_if(
        is_empty<internal::remove_const_and_ref_t<Container>>,
        alt, std::forward<Container>(xs));
}

} // namespace fplus

//
// container_properties.hpp
//

// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)



//
// generate.hpp
//

// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)



//
// filter.hpp
//

// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)



//
// result.hpp
//

// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)




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
#if defined(_MSC_VER) && _MSC_VER >= 1920 // in VS2019, compilation with /permissive- breaks with 'using' syntax below
            struct FOut : std::decay_t<
                internal::invoke_result_t<decltype(f), decltype(args)...>> {};
#else
            using FOut = std::decay_t<
                internal::invoke_result_t<decltype(f), decltype(args)...>>;
#endif

            internal::trigger_static_asserts<internal::unary_function_tag,
                                                 decltype(g),
                                                 typename FOut::ok_t>();
#if defined(_MSC_VER) && _MSC_VER >= 1920 // in VS2019, compilation with /permissive- breaks with 'using' syntax below
            struct GOut : std::decay_t<
                internal::invoke_result_t<decltype(g), typename FOut::ok_t>> {};
#else
            using GOut = std::decay_t<
                internal::invoke_result_t<decltype(g), typename FOut::ok_t>>;
#endif
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

#include <algorithm>

namespace fplus
{

namespace internal
{

template <typename Pred, typename Container>
Container keep_if(internal::reuse_container_t, Pred pred, Container&& xs)
{
    internal::check_unary_predicate_for_container<Pred, Container>();
    xs.erase(std::remove_if(
        std::begin(xs), std::end(xs), logical_not(pred)), std::end(xs));
    return std::forward<Container>(xs);
}

template <typename Pred, typename Container>
Container keep_if(internal::create_new_container_t, Pred pred,
    const Container& xs)
{
    internal::check_unary_predicate_for_container<Pred, Container>();
    Container result;
    auto it = internal::get_back_inserter<Container>(result);
    std::copy_if(std::begin(xs), std::end(xs), it, pred);
    return result;
}

} // namespace internal

// API search type: keep_if : ((a -> Bool), [a]) -> [a]
// fwd bind count: 1
// Keep the elements of a sequence fulfilling a predicate.
// keep_if(is_even, [1, 2, 3, 2, 4, 5]) == [2, 2, 4]
// Also known as filter.
template <typename Pred, typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut keep_if(Pred pred, Container&& xs)
{
    return internal::keep_if(internal::can_reuse_v<Container>{},
        pred, std::forward<Container>(xs));
}

// API search type: drop_if : ((a -> Bool), [a]) -> [a]
// fwd bind count: 1
// Drop the elements of a sequence fulfilling a predicate.
// drop_if(is_even, [1, 2, 3, 2, 4, 5]) == [1, 3, 5]
// Also known as reject.
template <typename Pred, typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut drop_if(Pred pred, Container&& xs)
{
    return keep_if(logical_not(pred), std::forward<Container>(xs));
}

// API search type: without : (a, [a]) -> [a]
// fwd bind count: 1
// Keep all elements a sequence not equal to elem.
// without(0, [1, 0, 0, 5, 3, 0, 1]) == [1, 5, 3, 1]
template <typename Container,
    typename T = typename Container::value_type,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut without(T elem, Container&& xs)
{
    return drop_if(is_equal_to(elem), std::forward<Container>(xs));
}

// API search type: without_any : (a, [a]) -> [a]
// fwd bind count: 1
// Keep all elements a sequence not present in elems.
// without([0, 1], [1, 0, 0, 5, 3, 0, 1]) == [5, 3]
template <typename Container, typename ContainerElems,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut without_any(const ContainerElems& elems, Container&& xs)
{
    static_assert(std::is_same<
        typename ContainerElems::value_type,
        typename std::remove_reference<Container>::type::value_type>::value,
        "Container values must be of the same type.");
    const auto pred = bind_2nd_of_2(is_elem_of<ContainerElems>, elems);
    return drop_if(pred, std::forward<Container>(xs));
}

// API search type: keep_if_with_idx : (((Int, a) -> Bool), [a]) -> [a]
// fwd bind count: 1
// Keep the elements of a sequence fulfilling a predicate.
// Predicate takes index and value.
// All elements fulfilling the predicate are kept.
template <typename Pred, typename Container>
Container keep_if_with_idx(Pred pred, const Container& xs)
{
    internal::check_index_with_type_predicate_for_container<Pred, Container>();
    Container ys;
    auto it = internal::get_back_inserter<Container>(ys);
    std::size_t idx = 0;
    for (const auto& x : xs)
    {
        if (internal::invoke(pred, idx++, x))
            *it = x;
    }
    return ys;
}

// API search type: drop_if_with_idx : (((Int, a) -> Bool), [a]) -> [a]
// fwd bind count: 1
// Drop the elements of a sequence fulfilling a predicate.
// Predicate takes index and value.
// All elements fulfilling the predicate are skipped.
template <typename Pred, typename Container>
Container drop_if_with_idx(Pred pred, const Container& xs)
{
    internal::check_index_with_type_predicate_for_container<Pred, Container>();
    const auto inverse_pred = [pred](auto idx, const auto& x)
    {
        return !internal::invoke(pred, idx, x);
    };
    return keep_if_with_idx(inverse_pred, xs);
}

namespace internal
{

template <typename UnaryPredicate, typename Container>
Container keep_by_idx(internal::reuse_container_t,
    UnaryPredicate pred, Container&& xs)
{
    auto itOut = std::begin(xs);
    std::size_t i = 0;
    for (auto it = std::begin(xs); it != std::end(xs); ++it)
    {
        if (internal::invoke(pred, i++))
            *itOut++ = std::move(*it);
    }
    xs.erase(itOut, std::end(xs));
    return std::forward<Container>(xs);
}

template <typename UnaryPredicate, typename Container>
Container keep_by_idx(internal::create_new_container_t,
    UnaryPredicate pred, const Container& xs)
{
    Container ys = xs;
    return internal::keep_by_idx(internal::reuse_container_t(),
        pred, std::move(ys));
}

} // namespace internal

// API search type: keep_by_idx : ((Int -> Bool), [a]) -> [a]
// fwd bind count: 1
// Keep the elements of a sequence with an index fulfilling a predicate.
// Predicate takes an index and decides if an element is kept.
template <typename UnaryPredicate, typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut keep_by_idx(UnaryPredicate pred, Container&& xs)
{
    internal::check_unary_predicate_for_type<UnaryPredicate, std::size_t>();
    return internal::keep_by_idx(internal::can_reuse_v<Container>{},
        pred, std::forward<Container>(xs));
}

// API search type: drop_by_idx : ((Int -> Bool), [a]) -> [a]
// fwd bind count: 1
// Drop the elements of a sequence with an index fulfilling a predicate.
// Predicate takes an index and decides if an element is dropped.
template <typename UnaryPredicate, typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut drop_by_idx(UnaryPredicate pred, Container&& xs)
{
    internal::check_unary_predicate_for_type<UnaryPredicate, std::size_t>();
    return keep_by_idx(logical_not(pred), std::forward<Container>(xs));
}

// API search type: keep_idxs : ([Int], [a]) -> [a]
// fwd bind count: 1
// Keep the elements of a sequence with an index present in idxs_to_keep.
// keep_idxs([2,5], [1,2,3,4,5,6,7]) == [3,6]
template <typename ContainerIdxs, typename Container>
Container keep_idxs(const ContainerIdxs& idxs_to_keep, const Container& xs)
{
    static_assert(std::is_same<typename ContainerIdxs::value_type, std::size_t>::value,
        "Indices must be std::size_t");
    auto idxs_left = convert_container<std::list<std::size_t>>(
        unique(sort(idxs_to_keep)));
    Container ys;
    auto it = internal::get_back_inserter<Container>(ys);
    std::size_t idx = 0;
    for (const auto& x : xs)
    {
        if (!idxs_left.empty() && idxs_left.front() == idx)
        {
            idxs_left.pop_front();
            *it = x;
        }
        ++idx;
    }
    return ys;
}

// API search type: drop_idxs : ([Int], [a]) -> [a]
// fwd bind count: 1
// Drop the elements of a sequence with an index present in idxs_to_keep.
// drop_idxs([2,5], [1,2,3,4,5,6,7]) == [1,2,4,5,7]
template <typename ContainerIdxs, typename Container>
Container drop_idxs(const ContainerIdxs& idxs_to_drop, const Container& xs)
{
    static_assert(std::is_same<typename ContainerIdxs::value_type, std::size_t>::value,
        "Indices must be std::size_t");
    auto idxs_left = convert_container<std::list<std::size_t>>(
        unique(sort(idxs_to_drop)));
    Container ys;
    auto it = internal::get_back_inserter<Container>(ys);
    std::size_t idx = 0;
    for (const auto& x : xs)
    {
        if (idxs_left.empty() || idxs_left.front() != idx)
        {
            *it = x;
        }
        else
        {
            if (!idxs_left.empty())
            {
                idxs_left.pop_front();
            }
        }
        ++idx;
    }
    return ys;
}

// API search type: drop_idx : (Int, [a]) -> [a]
// fwd bind count: 1
// Remove the element at a specific index from a sequence.
// drop_idx(2, [1,2,3,4,5,6,7]) == [1,2,4,5,6,7]
template <typename Container>
Container drop_idx(std::size_t idx, const Container& xs)
{
    return drop_by_idx(is_equal_to(idx), xs);
}

// API search type: justs : [Maybe a] -> [a]
// fwd bind count: 0
// From a Container filled with Maybe<T> the nothings are dropped
// and the values inside the justs are returned in a new container.
template <typename ContainerIn,
    typename ContainerOut =
        typename internal::same_cont_new_t<ContainerIn,
            typename ContainerIn::value_type::type>::type>
ContainerOut justs(const ContainerIn& xs)
{
    typedef typename ContainerIn::value_type::type T;
    auto justsInMaybes = keep_if(is_just<T>, xs);
    ContainerOut ys;
    internal::prepare_container(ys, fplus::size_of_cont(justsInMaybes));
    auto itOut = internal::get_back_inserter<ContainerOut>(ys);
    std::transform(std::begin(justsInMaybes), std::end(justsInMaybes),
        itOut, unsafe_get_just<T>);
    return ys;
}

// API search type: oks : [Result a b] -> [a]
// fwd bind count: 0
// From a Container filled with Result<Ok, Error> the errors are dropped
// and the values inside the ok are returned in a new container.
template <typename ContainerIn,
    typename ContainerOut =
        typename internal::same_cont_new_t<ContainerIn,
            typename ContainerIn::value_type::ok_t>::type>
ContainerOut oks(const ContainerIn& xs)
{
    typedef typename ContainerIn::value_type::ok_t Ok;
    typedef typename ContainerIn::value_type::error_t Error;
    auto oksInResults = keep_if(is_ok<Ok, Error>, xs);
    ContainerOut ys;
    internal::prepare_container(ys, fplus::size_of_cont(oksInResults));
    auto itOut = internal::get_back_inserter<ContainerOut>(ys);
    std::transform(std::begin(oksInResults), std::end(oksInResults),
        itOut, unsafe_get_ok<Ok, Error>);
    return ys;
}

// API search type: errors : [Result a b] -> [b]
// fwd bind count: 0
// From a Container filled with Result<Ok, Error> the oks are dropped
// and the values inside the errors are returned in a new container.
template <typename ContainerIn,
    typename ContainerOut =
        typename internal::same_cont_new_t<ContainerIn,
            typename ContainerIn::value_type::error_t>::type>
ContainerOut errors(const ContainerIn& xs)
{
    typedef typename ContainerIn::value_type::ok_t Ok;
    typedef typename ContainerIn::value_type::error_t Error;
    auto errorsInResults = keep_if(is_error<Ok, Error>, xs);
    ContainerOut ys;
    internal::prepare_container(ys, fplus::size_of_cont(errorsInResults));
    auto itOut = internal::get_back_inserter<ContainerOut>(ys);
    std::transform(std::begin(errorsInResults), std::end(errorsInResults),
        itOut, unsafe_get_error<Ok, Error>);
    return ys;
}

// API search type: trim_left : (a, [a]) -> [a]
// fwd bind count: 1
// Remove elements from the left as long as they equal x.
// trim_left('_', "___abc__") == "abc__"
// trim_left(0, [0,0,0,5,6,7,8,6,4]) == [5,6,7,8,6,4]
template <typename Container,
        typename T = typename Container::value_type>
Container trim_left(const T& x, const Container& xs)
{
    return drop_while(is_equal_to(x), xs);
}

// API search type: trim_token_left : ([a], [a]) -> [a]
// fwd bind count: 1
// Remove elements from the left as long as they match token.
// trim_token_left([0,1,2], [0,1,2,0,1,2,7,5,9]) == [7,5,9]
template <typename Container>
Container trim_token_left(const Container& token, const Container& xs)
{
    auto result = xs;
    while (is_prefix_of(token, result))
    {
        result = get_segment(size_of_cont(token), size_of_cont(result), result);
    }
    return result;
}

// API search type: trim_right_by : ((a -> Bool), [a]) -> [a]
// fwd bind count: 1
// Remove elements from the left as long as p is fulfilled.
// trim_right_by(is_even, [0,2,4,5,6,7,8,6,4]) == [0,2,4,5,6,7]
template <typename Container, typename UnaryPredicate>
Container trim_right_by(UnaryPredicate p, const Container& xs)
{
    internal::check_unary_predicate_for_container<UnaryPredicate, Container>();
    return reverse(drop_while(p, reverse(xs)));
}

// API search type: trim_right : (a, [a]) -> [a]
// fwd bind count: 1
// Remove elements from the left as long as they equal x.
// trim_right('_', "___abc__") == "___abc"
// trim_right(4, [0,2,4,5,6,7,8,4,4]) == [0,2,4,5,6,7,8]
template <typename Container,
        typename T = typename Container::value_type>
Container trim_right(const T& x, const Container& xs)
{
    return trim_right_by(is_equal_to(x), xs);
}

// API search type: trim_token_right : ([a], [a]) -> [a]
// fwd bind count: 1
// Remove elements from the right as long as they match token.
// trim_token_right([0,1,2], [7,5,9,0,1,2,0,1,2]) == [7,5,9]
template <typename Container>
Container trim_token_right(const Container& token, const Container& xs)
{
    return reverse(trim_token_left(reverse(token), reverse(xs)));
}

// API search type: trim_by : ((a -> Bool), [a]) -> [a]
// fwd bind count: 1
// Remove elements from the left and right as long as p is fulfilled.
// trim_by(is_even, [0,2,4,5,6,7,8,6,4]) == [5,6,7]
template <typename Container, typename UnaryPredicate>
Container trim_by(UnaryPredicate p, const Container& xs)
{
    internal::check_unary_predicate_for_container<UnaryPredicate, Container>();
    return trim_right_by(p, drop_while(p, xs));
}

// API search type: trim : (a, [a]) -> [a]
// fwd bind count: 1
// Remove elements from the left and right as long as they equal x.
// trim('_', "___abc__") == "abc"
// trim(0, [0,2,4,5,6,7,8,0,0]) == [2,4,5,6,7,8]
template <typename Container,
        typename T = typename Container::value_type>
Container trim(const T& x, const Container& xs)
{
    return trim_right(x, trim_left(x, xs));
}

// API search type: trim_token : ([a], [a]) -> [a]
// fwd bind count: 1
// Remove elements from the left and right as long as they match token.
// trim_token([0,1], [0,1,7,8,9,0,1]) == [7,8,9]
template <typename Container>
Container trim_token(const Container& token, const Container& xs)
{
    return trim_token_right(token, trim_token_left(token, xs));
}

// API search type: adjacent_keep_snd_if : (((a, a) -> Bool), [a]) -> [a]
// fwd bind count: 1
// For each pair of adjacent elements in a source sequence,
// evaluate the specified binary predicate.
// If the predicate evaluates to false,
// the second element of the pair is removed from the result sequence;
// otherwise, it is included.
// The first element in the source sequence is always included.
// Also known as adjacent_filter.
template <typename BinaryPredicate, typename Container>
Container adjacent_keep_snd_if(BinaryPredicate p, const Container& xs)
{
    if (is_empty(xs))
    {
        return {};
    }
    internal::check_binary_predicate_for_container<BinaryPredicate, Container>();
    Container result;
    auto it = internal::get_back_inserter<Container>(result);
    auto it_in = std::begin(xs);
    *it = *it_in;
    while (internal::add_to_iterator(it_in) != std::end(xs))
    {
        if (p(*it_in, *internal::add_to_iterator(it_in)))
        {
            *it = *internal::add_to_iterator(it_in);
        }
        internal::advance_iterator(it_in, 1);
    }
    return result;
}

// API search type: adjacent_drop_fst_if : (((a, a) -> Bool), [a]) -> [a]
// fwd bind count: 1
// For each pair of adjacent elements in a source sequence,
// evaluate the specified binary predicate.
// If the predicate evaluates to true,
// the first element of the pair is removed from the result sequence;
// otherwise, it is included.
// The last element in the source sequence is always included.
// Also known as adjacent_remove_if.
template <typename BinaryPredicate, typename Container>
Container adjacent_drop_fst_if(BinaryPredicate p, const Container& xs)
{
    if (is_empty(xs))
    {
        return {};
    }
    internal::check_binary_predicate_for_container<BinaryPredicate, Container>();
    Container result;
    auto it = internal::get_back_inserter<Container>(result);
    auto it_in = std::begin(xs);
    while (internal::add_to_iterator(it_in) != std::end(xs))
    {
        if (!internal::invoke(p, *it_in, *internal::add_to_iterator(it_in)))
        {
            *it = *it_in;
        }
        internal::advance_iterator(it_in, 1);
    }
    *it = *it_in;
    return result;
}

// API search type: adjacent_drop_snd_if : (((a, a) -> Bool), [a]) -> [a]
// fwd bind count: 1
// For each pair of adjacent elements in a source sequence,
// evaluate the specified binary predicate.
// If the predicate evaluates to true,
// the second element of the pair is removed from the result sequence;
// otherwise, it is included.
// The first element in the source sequence is always included.
template <typename BinaryPredicate, typename Container>
Container adjacent_drop_snd_if(BinaryPredicate p, const Container& xs)
{
    typedef typename Container::value_type T;
    const auto not_p = [&p](const T& x, const T& y) -> bool
    {
        return !internal::invoke(p, x, y);
    };
    return adjacent_keep_snd_if(not_p, xs);
}

// API search type: adjacent_keep_fst_if : (((a, a) -> Bool), [a]) -> [a]
// fwd bind count: 1
// For each pair of adjacent elements in a source sequence,
// evaluate the specified binary predicate.
// If the predicate evaluates to false,
// the first element of the pair is removed from the result sequence;
// otherwise, it is included.
// The last element in the source sequence is always included.
template <typename BinaryPredicate, typename Container>
Container adjacent_keep_fst_if(BinaryPredicate p, const Container& xs)
{
    typedef typename Container::value_type T;
    const auto not_p = [&p](const T& x, const T& y) -> bool
    {
        return !internal::invoke(p, x, y);
    };
    return adjacent_drop_fst_if(not_p, xs);
}

} // namespace fplus


namespace fplus
{

// API search type: generate : ((() -> a), Int) -> [a]
// Grab values from executing a nullary function
// to generate a sequence of amount values.
// generate(f, 3) == [f(), f(), f()]
// Can for example be used to generate a list of random numbers.
template <typename ContainerOut, typename F>
ContainerOut generate(F f, std::size_t amount)
{
    internal::trigger_static_asserts<internal::nullary_function_tag, F>();
    ContainerOut ys;
    internal::prepare_container(ys, amount);
    auto it = internal::get_back_inserter<ContainerOut>(ys);
    for (std::size_t i = 0; i < amount; ++i)
    {
        *it = internal::invoke(f);
    }
    return ys;
}

// API search type: generate_by_idx : ((Int -> a), Int) -> [a]
// fwd bind count: 1
// Grab values from executing a unary function with an index
// to generate a sequence of amount values.
// generate_by_idx(f, 3) == [f(0), f(1), f(2)]
template <typename ContainerOut, typename F>
ContainerOut generate_by_idx(F f, std::size_t amount)
{
    internal::
        trigger_static_asserts<internal::unary_function_tag, F, std::size_t>();

    ContainerOut ys;
    internal::prepare_container(ys, amount);
    auto it = internal::get_back_inserter<ContainerOut>(ys);
    for (std::size_t i = 0; i < amount; ++i)
    {
        *it = internal::invoke(f, i);
    }
    return ys;
}

// API search type: repeat : (Int, [a]) -> [a]
// fwd bind count: 1
// Create a sequence containing xs concatenated n times.
// repeat(3, [1, 2]) == [1, 2, 1, 2, 1, 2]
template <typename Container>
Container repeat(std::size_t n, const Container& xs)
{
    std::vector<Container> xss(n, xs);
    return concat(xss);
}

// API search type: infixes : (Int, [a]) -> [[a]]
// fwd bind count: 1
// Return als possible infixed of xs with a given length.
// infixes(3, [1,2,3,4,5,6]) == [[1,2,3], [2,3,4], [3,4,5], [4,5,6]]
// length must be > 0
template <typename ContainerIn,
    typename ContainerOut = std::vector<ContainerIn>>
ContainerOut infixes(std::size_t length, const ContainerIn& xs)
{
    assert(length > 0);
    static_assert(std::is_convertible<ContainerIn,
        typename ContainerOut::value_type>::value,
        "ContainerOut can not take values of type ContainerIn as elements.");
    ContainerOut result;
    if (size_of_cont(xs) < length)
        return result;
    internal::prepare_container(result, size_of_cont(xs) - length);
    auto itOut = internal::get_back_inserter(result);
    for (std::size_t idx = 0; idx <= size_of_cont(xs) - length; ++idx)
    {
        *itOut = get_segment(idx, idx + length, xs);
    }
    return result;
}

// API search type: carthesian_product_with_where : (((a, b) -> c), ((a -> b), Bool), [a], [b]) -> [c]
// fwd bind count: 3
// carthesian_product_with_where(make_pair, always(true), "ABC", "XY")
//   == [(A,X),(A,Y),(B,X),(B,Y),(C,X),(C,Y)]
// same as (in Haskell):
//   [ f x y | x <- xs, y <- ys, pred x y ]
// same as (in pseudo SQL):
//   SELECT f(xs.x, ys.y)
//   FROM xs, ys
//   WHERE pred(xs.x, ys.y);
template <typename F, typename Pred, typename Container1, typename Container2>
auto carthesian_product_with_where(F f,
                                   Pred pred,
                                   const Container1& xs,
                                   const Container2& ys)
{
    using X = typename Container1::value_type;
    using Y = typename Container2::value_type;
    using FOut = internal::invoke_result_t<F, X, Y>;
    using ContainerOut = std::vector<std::decay_t<FOut>>;

    ContainerOut result;
    auto itOut = internal::get_back_inserter(result);
    for (const auto& x : xs)
    {
        for (const auto& y : ys)
        {
            if (internal::invoke(pred, x, y))
            {
                itOut = f(x, y);
            }
        }
    }
    return result;
}

// API search type: carthesian_product_with : (((a, b) -> c), [a], [b]) -> [c]
// fwd bind count: 2
// carthesian_product_with(make_pair, "ABC", "XY")
//   == [(A,X),(A,Y),(B,X),(B,Y),(C,X),(C,Y)]
// same as (in Haskell):
//   [ f x y | x <- xs, y <- ys ]
// same as (in pseudo SQL):
//   SELECT f(xs.x, ys.y)
//   FROM xs, ys;
template <typename F, typename Container1, typename Container2>
auto carthesian_product_with(F f, const Container1& xs, const Container2& ys)
{
    auto always_true_x_y = [](const auto&, const auto&) { return true; };
    return carthesian_product_with_where(f, always_true_x_y, xs, ys);
}

// API search type: carthesian_product_where : (((a, b) -> Bool), [a], [b]) -> [(a, b)]
// fwd bind count: 2
// carthesian_product_where(always(true), "ABC", "XY")
//   == [(A,X),(A,Y),(B,X),(B,Y),(C,X),(C,Y)]
// same as (in Haskell):
//   [ (x, y) | x <- xs, y <- ys, pred x y ]
// same as (in pseudo SQL):
//   SELECT (xs.x, ys.y)
//   FROM xs, ys
//   WHERE pred(xs.x, ys.y);
template <typename Pred, typename Container1, typename Container2>
auto carthesian_product_where(Pred pred,
    const Container1& xs, const Container2& ys)
{
    auto make_res_pair = [](const auto& x, const auto& y)
    {
        return std::make_pair(x, y);
    };
    return carthesian_product_with_where(make_res_pair, pred, xs, ys);
}

// API search type: carthesian_product : ([a], [b]) -> [(a, b)]
// fwd bind count: 1
// carthesian_product("ABC", "XY")
//   == [(A,X),(A,Y),(B,X),(B,Y),(C,X),(C,Y)]
// same as (in Haskell):
//   [ (x, y) | x <- xs, y <- ys ]
// same as (in pseudo SQL):
//   SELECT (xs.x, ys.y)
//   FROM xs, ys;
template <typename Container1, typename Container2>
auto carthesian_product(const Container1& xs, const Container2& ys)
{
    auto make_res_pair = [](const auto& x, const auto& y)
    {
        return std::make_pair(x, y);
    };
    auto always_true_x_y = [](const auto&, const auto&) { return true; };
    return carthesian_product_with_where(
        make_res_pair, always_true_x_y, xs, ys);
}


namespace internal
{
    // productN :: Int -> [a] -> [[a]]
    // productN n = foldr go [[]] . replicate n
    //     where go elems acc = [x:xs | x <- elems, xs <- acc]
    template <typename T>
    std::vector<std::vector<T>> helper_carthesian_product_n_idxs
            (std::size_t power, const std::vector<T>& xs)
    {
        static_assert(std::is_same<T, std::size_t>::value,
            "T must be std::size_t");
        typedef std::vector<T> Vec;
        typedef std::vector<Vec> VecVec;
        if (power == 0)
            return VecVec();
        auto go = [](const Vec& elems, const VecVec& acc)
        {
            VecVec result;
            for (const T& x : elems)
            {
                for (const Vec& tail : acc)
                {
                    result.push_back(append(Vec(1, x), tail));
                }
            }
            return result;
        };
        return fold_right(go, VecVec(1), replicate(power, xs));
    }
}

// API search type: carthesian_product_n : (Int, [a]) -> [[a]]
// fwd bind count: 1
// Returns the product set with a given power.
// carthesian_product_n(2, "ABCD")
//   == AA AB AC AD BA BB BC BD CA CB CC CD DA DB DC DD
template <typename ContainerIn,
    typename T = typename ContainerIn::value_type,
    typename ContainerOut = std::vector<ContainerIn>>
ContainerOut carthesian_product_n(std::size_t power, const ContainerIn& xs_in)
{
    if (power == 0)
        return ContainerOut(1);
    std::vector<T> xs = convert_container<std::vector<T>>(xs_in);
    auto idxs = all_idxs(xs);
    auto result_idxss = internal::helper_carthesian_product_n_idxs(power, idxs);
    typedef typename ContainerOut::value_type ContainerOutInner;
    auto to_result_cont = [&](const std::vector<std::size_t>& indices)
    {
        return convert_container_and_elems<ContainerOutInner>(
            elems_at_idxs(indices, xs));
    };
    return transform(to_result_cont, result_idxss);
}

// API search type: permutations : (Int, [a]) -> [[a]]
// fwd bind count: 1
// Generate all possible permutations with a given power.
// permutations(2, "ABCD") == AB AC AD BA BC BD CA CB CD DA DB DC
template <typename ContainerIn,
    typename T = typename ContainerIn::value_type,
    typename ContainerOut = std::vector<ContainerIn>>
ContainerOut permutations(std::size_t power, const ContainerIn& xs_in)
{
    if (power == 0)
        return ContainerOut(1);
    std::vector<T> xs = convert_container<std::vector<T>>(xs_in);
    auto idxs = all_idxs(xs);
    typedef std::vector<std::size_t> idx_vec;
    auto result_idxss = keep_if(all_unique<idx_vec>,
        internal::helper_carthesian_product_n_idxs(power, idxs));
    typedef typename ContainerOut::value_type ContainerOutInner;
    auto to_result_cont = [&](const std::vector<std::size_t>& indices)
    {
        return convert_container_and_elems<ContainerOutInner>(
            elems_at_idxs(indices, xs));
    };
    return transform(to_result_cont, result_idxss);
}

// API search type: combinations : (Int, [a]) -> [[a]]
// fwd bind count: 1
// Generate all possible combinations with a given power.
// combinations(2, "ABCD") == AB AC AD BC BD CD
template <typename ContainerIn,
    typename T = typename ContainerIn::value_type,
    typename ContainerOut = std::vector<ContainerIn>>
ContainerOut combinations(std::size_t power, const ContainerIn& xs_in)
{
    if (power == 0)
        return ContainerOut(1);
    std::vector<T> xs = convert_container<std::vector<T>>(xs_in);
    auto idxs = all_idxs(xs);
    typedef std::vector<std::size_t> idx_vec;
    auto result_idxss = keep_if(is_strictly_sorted<idx_vec>,
        internal::helper_carthesian_product_n_idxs(power, idxs));
    typedef typename ContainerOut::value_type ContainerOutInner;
    auto to_result_cont = [&](const std::vector<std::size_t>& indices)
    {
        return convert_container_and_elems<ContainerOutInner>(
            elems_at_idxs(indices, xs));
    };
    return transform(to_result_cont, result_idxss);
}

// API search type: combinations_with_replacement : (Int, [a]) -> [[a]]
// fwd bind count: 1
// Generate all possible combinations using replacement with a given power.
// combinations_with_replacement(2, "ABCD") == AA AB AC AD BB BC BD CC CD DD
template <typename ContainerIn,
    typename T = typename ContainerIn::value_type,
    typename ContainerOut = std::vector<ContainerIn>>
ContainerOut combinations_with_replacement(std::size_t power,
        const ContainerIn& xs_in)
{
    if (power == 0)
        return ContainerOut(1);
    std::vector<T> xs = convert_container<std::vector<T>>(xs_in);
    auto idxs = all_idxs(xs);
    typedef std::vector<std::size_t> idx_vec;
    auto result_idxss = keep_if(is_sorted<idx_vec>,
        internal::helper_carthesian_product_n_idxs(power, idxs));
    typedef typename ContainerOut::value_type ContainerOutInner;
    auto to_result_cont = [&](const std::vector<std::size_t>& indices)
    {
        return convert_container_and_elems<ContainerOutInner>(
            elems_at_idxs(indices, xs));
    };
    return transform(to_result_cont, result_idxss);
}

// API search type: power_set : [a] -> [[a]]
// fwd bind count: 0
// Return the set of all subsets of xs_in,
// including the empty set and xs_in itself.
// power_set("xyz") == ["", "x", "y", "z", "xy", "xz", "yz", "xyz"]
// Also known as subsequences.
template <typename ContainerIn,
    typename T = typename ContainerIn::value_type,
    typename ContainerOut = std::vector<ContainerIn>>
ContainerOut power_set(const ContainerIn& xs_in)
{
    return concat(
        generate_by_idx<std::vector<ContainerOut>>(
            bind_1st_of_2(
                flip(combinations<ContainerIn, T, ContainerOut>),
                xs_in),
            size_of_cont(xs_in) + 1));
}

// API search type: iterate : ((a -> a), Int, a) -> [a]
// fwd bind count: 2
// Repeatedly apply a function (n times) to a value (starting with x)
// and recording the outputs on its way.
// iterate((*2), 5, 3) = [3, 6, 12, 24, 48]
// = [3, f(3), f(f(3)), f(f(f(3))), f(f(f(f(3))))]
template <typename F,
    typename T,
    typename ContainerOut = std::vector<T>>
ContainerOut iterate(F f, std::size_t size, const T& x)
{
    ContainerOut result;
    if (size == 0)
        return result;
    internal::prepare_container(result, size + 1);
    auto it_out = internal::get_back_inserter(result);
    T current = x;
    *it_out = current;
    for (std::size_t i = 1; i < size; ++i)
    {
        current = internal::invoke(f, current);
        *it_out = current;
    }
    return result;
}

// API search type: iterate_maybe : ((a -> Maybe a), a) -> [a]
// fwd bind count: 1
// Repeatedly apply a function to a value (starting with x)
// and recording the outputs on its way.
// Stops when the function returns nothing.
// iterate_maybe(next_collats_val, 5) = [5, 16, 8, 4, 2, 1]
template <typename F,
    typename T,
    typename ContainerOut = std::vector<T>>
ContainerOut iterate_maybe(F f, const T& x)
{
    ContainerOut result;
    auto it_out = internal::get_back_inserter(result);
    maybe<T> current(x);
    while (current.is_just())
    {
        *it_out = current.unsafe_get_just();
        current = internal::invoke(f, current.unsafe_get_just());
    }
    return result;
}

// API search type: adjacent_difference_by : [a] -> [a]
// fwd bind count: 1
// Computes the differences between the second
// and the first of each adjacent pair of elements of the sequence
// using a binary function.
// adjacent_difference_by([0,4,1,2,5]) == [0,4,-3,1,3]
template <typename ContainerIn, typename F>
auto adjacent_difference_by(F f, const ContainerIn& xs)
{
    using X = typename ContainerIn::value_type;
    using TOut = internal::invoke_result_t<F, X, X>;
    using ContainerOut = std::vector<std::decay_t<TOut>>;

    ContainerOut result;

    using std::begin;
    using std::end;
    internal::prepare_container(result, size_of_cont(xs));
    std::adjacent_difference(begin(xs), end(xs), back_inserter(result), f);
    return result;
}

// API search type: adjacent_difference : [a] -> [a]
// fwd bind count: 0
// Computes the differences between the second
// and the first of each adjacent pair of elements of the sequence.
// adjacent_difference([0,4,1,2,5]) == [0,4,-3,1,3]
template <typename Container>
Container adjacent_difference(const Container& xs)
{
    return adjacent_difference_by(
        std::minus<typename Container::value_type>(), xs);
}

// API search type: rotate_left : [a] -> [a]
// fwd bind count: 0
// Removes the first element and appends it to the back.
// rotate_left("xyz") == "yzx"
template <typename Container>
Container rotate_left(const Container& xs)
{
    if (is_empty(xs))
        return xs;
    Container ys;
    auto size = size_of_cont(xs);
    internal::prepare_container(ys, size);
    auto it = std::begin(xs);
    auto it_out = internal::get_back_inserter(ys);
    ++it;
    while (it != std::end(xs))
    {
        *it_out = *it;
        ++it;
    }
    *it_out = xs.front();
    return ys;
}

// API search type: rotate_right : [a] -> [a]
// fwd bind count: 0
// Removes the last element and prepends it to the front.
// rotate_right("xyz") == "zxy"
template <typename Container>
Container rotate_right(const Container& xs)
{
    return reverse(rotate_left(reverse(xs)));
}

// API search type: rotations_left : [a] -> [[a]]
// fwd bind count: 0
// Returns all possible rotations using rotate_left.
// rotations_left("abcd") == ["abcd", "bcda", "cdab", "dabc"]
template <typename ContainerIn,
    typename T = typename ContainerIn::value_type,
    typename ContainerOut = std::vector<ContainerIn>>
ContainerOut rotations_left(const ContainerIn& xs_in)
{
    return iterate(rotate_left<ContainerIn>, size_of_cont(xs_in), xs_in);
}

// API search type: rotations_right : [a] -> [[a]]
// fwd bind count: 0
// Returns all possible rotations using rotate_right.
// rotations_right("abcd") == ["abcd", "dabc", "cdab", "bcda"]
template <typename ContainerIn,
    typename T = typename ContainerIn::value_type,
    typename ContainerOut = std::vector<ContainerIn>>
ContainerOut rotations_right(const ContainerIn& xs_in)
{
    return iterate(rotate_right<ContainerIn>, size_of_cont(xs_in), xs_in);
}

// API search type: fill_left : (a, Int, [a]) -> [a]
// fwd bind count: 2
// Right-align a sequence.
// fill_left(0, 6, [1,2,3,4]) == [0,0,1,2,3,4]
// Also known as pad_left.
template <typename Container,
        typename T = typename Container::value_type>
Container fill_left(const T& x, std::size_t min_size, const Container& xs)
{
    if (min_size <= size_of_cont(xs))
        return xs;
    return append(replicate<T, Container>(min_size - size_of_cont(xs), x), xs);
}

// API search type: fill_right : (a, Int, [a]) -> [a]
// fwd bind count: 2
// Left-align a sequence.
// fill_right(0, 6, [1,2,3,4]) == [1,2,3,4,0,0]
template <typename Container,
        typename T = typename Container::value_type>
Container fill_right(const T& x, std::size_t min_size, const Container& xs)
{
    if (min_size <= size_of_cont(xs))
        return xs;
    return append(xs, replicate<T, Container>(min_size - size_of_cont(xs), x));
}

// API search type: inits : [a] -> [[a]]
// fwd bind count: 0
// Generate all possible segments of xs that include the first element.
// inits([0,1,2,3]) == [[],[0],[0,1],[0,1,2],[0,1,2,3]]
template <typename ContainerIn,
    typename T = typename ContainerIn::value_type,
    typename ContainerOut = std::vector<ContainerIn>>
ContainerOut inits(const ContainerIn& xs)
{
    ContainerOut result;
    std::size_t xs_size = size_of_cont(xs);
    internal::prepare_container(result, xs_size + 1);
    auto it_out = internal::get_back_inserter(result);
    for (std::size_t i = 0; i <= xs_size; ++i)
        *it_out = get_segment(0, i, xs);
    return result;
}

// API search type: tails : [a] -> [[a]]
// fwd bind count: 0
// Generate all possible segments of xs that include the last element.
// tails([0,1,2,3]) == [[0,1,2,3],[1,2,3],[2,3],[3],[]]
template <typename ContainerIn,
    typename T = typename ContainerIn::value_type,
    typename ContainerOut = std::vector<ContainerIn>>
ContainerOut tails(const ContainerIn& xs)
{
    ContainerOut result;
    std::size_t xs_size = size_of_cont(xs);
    internal::prepare_container(result, xs_size + 1);
    auto it_out = internal::get_back_inserter(result);
    for (std::size_t i = 0; i <= xs_size; ++i)
        *it_out = get_segment(i, xs_size, xs);
    return result;
}

} // namespace fplus

//
// numeric.hpp
//

// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)



//
// pairs.hpp
//

// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)



//
// internal/asserts/pairs.hpp
//

// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)



namespace fplus
{
namespace internal
{
struct apply_to_pair_tag
{
};

struct zip_with_tag
{
};

struct zip_with_3_tag
{
};

struct transform_fst_tag
{
};

struct transform_snd_tag
{
};

struct inner_product_with_tag
{
};

template <typename F, typename X, typename Y>
struct function_traits_asserts<apply_to_pair_tag, F, X, Y>
{
    static_assert(utils::function_traits<F>::arity == 2,
        "Function must take two parameters.");
    typedef typename utils::function_traits<F>::template arg<0>::type FIn0;
    typedef typename utils::function_traits<F>::template arg<1>::type FIn1;
    static_assert(std::is_convertible<X, FIn0>::value,
        "Function does not take pair.first type as first Parameter.");
    static_assert(std::is_convertible<Y, FIn1>::value,
        "Function does not take pair.second type as second Parameter.");
};

template <typename F, typename X, typename Y>
struct function_traits_asserts<zip_with_tag, F, X, Y>
{
    static_assert(utils::function_traits<F>::arity == 2,
        "Function must take two parameters.");
    typedef typename utils::function_traits<F>::template arg<0>::type FIn0;
    typedef typename utils::function_traits<F>::template arg<1>::type FIn1;
    static_assert(std::is_convertible<X, FIn0>::value,
        "Function does not take elements from first Container as first Parameter.");
    static_assert(std::is_convertible<Y, FIn1>::value,
        "Function does not take elements from second Container as second Parameter.");
};

template <typename F, typename X, typename Y, typename Z>
struct function_traits_asserts<zip_with_3_tag, F, X, Y, Z>
{
    static_assert(utils::function_traits<F>::arity == 3,
        "Function must take two parameters.");
    typedef typename utils::function_traits<F>::template arg<0>::type FIn0;
    typedef typename utils::function_traits<F>::template arg<1>::type FIn1;
    typedef typename utils::function_traits<F>::template arg<2>::type FIn2;
    static_assert(std::is_convertible<X, FIn0>::value,
        "Function does not take elements from first Container as first Parameter.");
    static_assert(std::is_convertible<Y, FIn1>::value,
        "Function does not take elements from second Container as second Parameter.");
    static_assert(std::is_convertible<Z, FIn2>::value,
        "Function does not take elements from third Container as third Parameter.");
};

template <typename F, typename X>
struct function_traits_asserts<transform_fst_tag, F, X>
{
    static_assert(utils::function_traits<F>::arity == 1,
        "Function must take one parameter.");
    typedef typename utils::function_traits<F>::template arg<0>::type FIn0;
    static_assert(std::is_convertible<X, FIn0>::value,
        "Function does not take pair.first type as first Parameter.");
};

template <typename F, typename X>
struct function_traits_asserts<transform_snd_tag, F, X>
{
    static_assert(utils::function_traits<F>::arity == 1,
        "Function must take one parameter.");
    typedef typename utils::function_traits<F>::template arg<0>::type FIn0;
    static_assert(std::is_convertible<X, FIn0>::value,
        "Function does not take pair.second type as first Parameter.");
};

template <typename F, typename X, typename Y>
struct function_traits_asserts<inner_product_with_tag, F, X, Y>
{
    static_assert(utils::function_traits<F>::arity == 2,
        "Function must take two parameters.");
    typedef typename utils::function_traits<F>::template arg<0>::type FIn0;
    typedef typename utils::function_traits<F>::template arg<1>::type FIn1;
    static_assert(std::is_convertible<X, FIn0>::value,
        "Function does not take elements from first Container as first Parameter.");
    static_assert(std::is_convertible<Y, FIn1>::value,
        "Function does not take elements from second Container as second Parameter.");
};
}
}

#include <utility>

namespace fplus
{
// API search type: apply_to_pair : (((a, b) -> c), (a, b)) -> c
// fwd bind count: 1
// Apply binary function to parts of a pair.
template <typename F, typename FIn0, typename FIn1>
auto apply_to_pair(F f, const std::pair<FIn0, FIn1>& p)
{
    internal::trigger_static_asserts<internal::apply_to_pair_tag, F, FIn0, FIn1>();
    return internal::invoke(f, p.first, p.second);
}

// API search type: zip_with : (((a, b) -> c), [a], [b]) -> [c]
// fwd bind count: 2
// Zip two sequences using a binary function.
// zip_with((+), [1, 2, 3], [5, 6]) == [1+5, 2+6] == [6, 8]
template <typename ContainerIn1,
          typename ContainerIn2,
          typename F,
          typename X = typename ContainerIn1::value_type,
          typename Y = typename ContainerIn2::value_type,
          typename TOut = std::decay_t<internal::invoke_result_t<F, X, Y>>,
          typename ContainerOut = std::vector<TOut>>
ContainerOut zip_with(F f, const ContainerIn1& xs, const ContainerIn2& ys)
{
    internal::trigger_static_asserts<internal::zip_with_tag, F, X, Y>();
    ContainerOut result;
    std::size_t resultSize = std::min(size_of_cont(xs), size_of_cont(ys));
    internal::prepare_container(result, resultSize);
    auto itResult = internal::get_back_inserter(result);
    auto itXs = std::begin(xs);
    auto itYs = std::begin(ys);
    for (std::size_t i = 0; i < resultSize; ++i)
    {
        *itResult = internal::invoke(f, *itXs, *itYs);
        ++itXs;
        ++itYs;
    }
  return result;
}

// API search type: zip_with_3 : (((a, b, c) -> d), [a], [b], [c]) -> [c]
// fwd bind count: 3
// Zip three sequences using a ternary function.
// zip_with_3((+), [1, 2, 3], [5, 6], [1, 1]) == [7, 9]
template <
    typename ContainerIn1,
    typename ContainerIn2,
    typename ContainerIn3,
    typename F,
    typename X = typename ContainerIn1::value_type,
    typename Y = typename ContainerIn2::value_type,
    typename Z = typename ContainerIn3::value_type,
    typename TOut = std::decay_t<internal::invoke_result_t<F, X, Y, Z>>,
    typename ContainerOut = typename std::vector<TOut>>
ContainerOut zip_with_3(F f,
                        const ContainerIn1& xs,
                        const ContainerIn2& ys,
                        const ContainerIn3& zs)
{
    internal::trigger_static_asserts<internal::zip_with_3_tag, F, X, Y, Z>();
    static_assert(std::is_same<
        typename internal::same_cont_new_t<ContainerIn1, void>::type,
        typename internal::same_cont_new_t<ContainerIn2, void>::type>::value,
        "All three Containers must be of same outer type.");
    static_assert(std::is_same<
        typename internal::same_cont_new_t<ContainerIn2, void>::type,
        typename internal::same_cont_new_t<ContainerIn3, void>::type>::value,
        "All three Containers must be of same outer type.");
    ContainerOut result;
    std::size_t resultSize = std::min(size_of_cont(xs), size_of_cont(ys));
    internal::prepare_container(result, resultSize);
    auto itResult = internal::get_back_inserter(result);
    auto itXs = std::begin(xs);
    auto itYs = std::begin(ys);
    auto itZs = std::begin(zs);
    for (std::size_t i = 0; i < resultSize; ++i)
    {
        *itResult = internal::invoke(f, *itXs, *itYs, *itZs);
        ++itXs;
        ++itYs;
        ++itZs;
    }
    return result;
}

// API search type: zip_with_defaults : (((a, b) -> c), a, b, [a], [b]) -> [c]
// fwd bind count: 4
// Zip two sequences and using a binary function
// and extrapolate the shorter sequence with a default value.
// zip_with_defaults((+), 6, 7, [1,2,3], [1,2]) == [2,4,10]
// zip_with_defaults((+), 6, 7, [1,2], [1,2,3]) == [2,4,9]
template <
    typename ContainerIn1,
    typename ContainerIn2,
    typename F,
    typename X = typename ContainerIn1::value_type,
    typename Y = typename ContainerIn2::value_type>
auto zip_with_defaults(F f,
    const X& default_x,
    const Y& default_y,
    const ContainerIn1& xs,
    const ContainerIn2& ys)
{
    internal::trigger_static_asserts<internal::zip_with_tag, F, X, Y>();
    const auto size_xs = size_of_cont(xs);
    const auto size_ys = size_of_cont(ys);
    if (size_xs < size_ys)
    {
        const auto extended_xs = append(
            xs,
            replicate<X, ContainerIn1>(size_ys - size_xs, default_x));
        return zip_with(f, extended_xs, ys);
    }
    else if (size_xs > size_ys)
    {
        const auto extended_ys = append(
            ys,
            replicate<Y, ContainerIn2>(size_xs - size_ys, default_y));
        return zip_with(f, xs, extended_ys);
    }
    return zip_with(f, xs, ys);
}

// API search type: zip : ([a], [b]) -> [(a, b)]
// fwd bind count: 1
// Combine two sequences to one sequence of pairs.
// zip([1, 2, 3], [5, 6]) == [(1, 5), (2, 6)]
template <typename ContainerIn1, typename ContainerIn2,
    typename X = typename ContainerIn1::value_type,
    typename Y = typename ContainerIn2::value_type>
auto zip(const ContainerIn1& xs, const ContainerIn2& ys)
{
    auto MakePair = [](const X& x, const Y& y)
        { return std::make_pair(x, y); };
    return zip_with(MakePair, xs, ys);
}

// API search type: zip_repeat : ([a], [b]) -> [(a, b)]
// fwd bind count: 1
// Similar to zip but repeats the shorter sequence
// to align with the longer sequence.
// zip([1, 2, 3, 4], [5, 6]) == [(1, 5), (2, 6), (3, 5), (4, 6)]
template <typename ContainerIn1, typename ContainerIn2>
auto zip_repeat(const ContainerIn1& xs, const ContainerIn2& ys)
{
    auto nx = xs.size();
    auto ny = ys.size();
    auto qx = ny/nx + (ny % nx ?  1 : 0);
    auto qy = nx/ny + (nx % ny ?  1 : 0);
    return zip(qx > 1 ? repeat(qx, xs) : xs,
               qy > 1 ? repeat(qy, ys) : ys);
}

// API search type: unzip : [(a, b)] -> ([a], [b])
// fwd bind count: 0
// Split a sequence of pairs into two sequences.
// unzip([(1, 5), (2, 6)]) == ([1, 2], [5, 6])
template <typename ContainerIn,
    typename TIn = typename ContainerIn::value_type,
    typename X = typename TIn::first_type,
    typename Y = typename TIn::second_type,
    typename ContainerOutX = typename internal::same_cont_new_t<ContainerIn, X>::type,
    typename ContainerOutY = typename internal::same_cont_new_t<ContainerIn, Y>::type>
std::pair<ContainerOutX, ContainerOutY> unzip(const ContainerIn& pairs)
{
    ContainerOutX firsts;
    ContainerOutY seconds;
    internal::prepare_container(firsts, size_of_cont(pairs));
    internal::prepare_container(seconds, size_of_cont(pairs));
    auto itFirsts = internal::get_back_inserter(firsts);
    auto itSeconds = internal::get_back_inserter(seconds);
    for (const auto& pair : pairs)
    {
        *itFirsts = pair.first;
        *itSeconds = pair.second;
    }
    return std::make_pair(firsts, seconds);
}

// API search type: fst : ((a, b)) -> a
// fwd bind count: 0
// Return the first element of a pair.
// fst((0, 1)) == 0
template <typename X, typename Y>
X fst(const std::pair<X, Y>& pair)
{
    return pair.first;
}

// API search type: snd : ((a, b)) -> b
// fwd bind count: 0
// Return the second element of a pair.
// snd((0, 1)) == 1
template <typename X, typename Y>
Y snd(const std::pair<X, Y>& pair)
{
    return pair.second;
}

// API search type: transform_fst : ((a -> c), (a, b)) -> (c, b)
// fwd bind count: 1
// Apply a function to the first element of a pair.
// transform_fst(square, (4, 5)) == (16, 5)
template <typename X, typename Y, typename F,
    typename ResultFirst = std::decay_t<internal::invoke_result_t<F, X>>>
std::pair<ResultFirst, Y> transform_fst(F f, const std::pair<X, Y>& pair)
{
    internal::trigger_static_asserts<internal::transform_fst_tag, F, X>();
    return std::make_pair(internal::invoke(f, pair.first), pair.second);
}

// API search type: transform_snd : ((b -> c), (a, b)) -> (a, c)
// fwd bind count: 1
// Apply a function to the second element of a pair.
// transform_snd(square, (4, 5)) == (4, 25)
template <typename X, typename Y, typename F,
    typename ResultSecond = std::decay_t<internal::invoke_result_t<F, Y>>>
std::pair<X, ResultSecond> transform_snd(F f, const std::pair<X, Y>& pair)
{
    internal::trigger_static_asserts<internal::transform_snd_tag, F, Y>();
    return std::make_pair(pair.first, internal::invoke(f, pair.second));
}

// API search type: transform_pair : ((a -> c), (b -> d), (a, b)) -> (c, d)
// fwd bind count: 2
// Apply functions the both parts of a pair.
// transform_pair(square, square, (4, 5)) == (16, 25)
template <
    typename X,
    typename Y,
    typename F,
    typename G,
    typename ResultFirst = std::decay_t<internal::invoke_result_t<F, X>>,
    typename ResultSecond = std::decay_t<internal::invoke_result_t<G, Y>>>
std::pair<ResultFirst, ResultSecond> transform_pair(F f,
                                                    G g,
                                                    const std::pair<X, Y>& pair)
{
    internal::trigger_static_asserts<internal::transform_fst_tag, F, X>();
    internal::trigger_static_asserts<internal::transform_snd_tag, G, Y>();
    return std::make_pair(internal::invoke(f, pair.first),
                          internal::invoke(g, pair.second));
}

// API search type: swap_pair_elems : (a, b) -> (b, a)
// fwd bind count: 0
// Swap the first and the second element of a pair.
// swap_pair_elems((3,4)) == (4,3)
template <typename X, typename Y>
std::pair<Y, X> swap_pair_elems(const std::pair<X, Y>& pair)
{
    return std::make_pair(pair.second, pair.first);
}

// API search type: swap_pairs_elems : [(a, b)] -> [(b, a)]
// fwd bind count: 0
// Swap the first and the second element of every pair in a sequence.
// swap_pairs_elems([(1,2), (3,4)]) == [(2,1), (4,3)]
template <typename ContainerIn,
    typename X = typename ContainerIn::value_type::first_type,
    typename Y = typename ContainerIn::value_type::second_type>
auto swap_pairs_elems(const ContainerIn& xs)
{
    return fplus::transform(swap_pair_elems<X, Y>, xs);
}

// API search type: adjacent_pairs : [a] -> [(a, a)]
// fwd bind count: 0
// Split a sequence into pairs of elements.
// adjacent_pairs([0,1,2,3,4]) == [(0,1), (2,3)]
// Also known as zip_with_next.
template <typename Container,
    typename ContainerOut =
        typename internal::same_cont_new_t<Container,
            std::pair<
                typename Container::value_type,
                    typename Container::value_type>>::type>
ContainerOut adjacent_pairs(const Container& xs)
{
    typedef typename Container::value_type T;
    static_assert(std::is_convertible<
            std::pair<T, T>,
            typename ContainerOut::value_type>::value,
        "ContainerOut can not store pairs of elements from ContainerIn.");
    ContainerOut result;
    if (size_of_cont(xs) < 2)
        return result;
    const std::size_t out_size = size_of_cont(xs) / 2;
    internal::prepare_container(result, out_size);
    auto itOut = internal::get_back_inserter(result);
    auto it1 = std::begin(xs);
    auto it2 = it1;
    internal::advance_iterator(it2, 1);
    const auto it_source_end =
        internal::add_to_iterator(std::begin(xs), out_size + out_size);
    for (;;)
    {
        *itOut = std::make_pair(*it1, *it2);
        internal::advance_iterator(it1, 2);
        if (it1 == it_source_end)
            break;
        internal::advance_iterator(it2, 2);
    }
    return result;
}

// API search type: overlapping_pairs : [a] -> [(a, a)]
// fwd bind count: 0
// Zip a sequence with itself shifted one element.
// overlapping_pairs([0,1,2,3]) == [(0,1),(1,2),(2,3)]
template <typename Container,
    typename ContainerOut =
        typename internal::same_cont_new_t<Container,
            std::pair<
                typename Container::value_type,
                    typename Container::value_type>, -1>::type>
ContainerOut overlapping_pairs(const Container& xs)
{
    typedef typename Container::value_type T;
    static_assert(std::is_convertible<
            std::pair<T, T>,
            typename ContainerOut::value_type>::value,
        "ContainerOut can not store pairs of elements from ContainerIn.");
    ContainerOut result;
    if (size_of_cont(xs) < 2)
        return result;
    internal::prepare_container(result, size_of_cont(xs) - 1);
    auto itOut = internal::get_back_inserter(result);
    auto it1 = std::begin(xs);
    auto it2 = it1;
    internal::advance_iterator(it2, 1);
    for (; it2 != std::end(xs); ++it1, ++it2)
    {
        *itOut = std::make_pair(*it1, *it2);
    }
    return result;
}

// API search type: overlapping_pairs_cyclic : [a] -> [(a, a)]
// fwd bind count: 0
// Zip a sequence with itself shifted one element,
// finally zipping the last element with the first one.
// overlapping_pairs_cyclic([0,1,2,3]) == [(0,1),(1,2),(2,3),(3,0)]
template <typename Container,
    typename ContainerOut =
        typename internal::same_cont_new_t<Container,
            std::pair<
                typename Container::value_type,
                    typename Container::value_type>, 0>::type>
ContainerOut overlapping_pairs_cyclic(const Container& xs)
{
    typedef typename Container::value_type T;
    static_assert(std::is_convertible<
            std::pair<T, T>,
            typename ContainerOut::value_type>::value,
        "ContainerOut can not store pairs of elements from ContainerIn.");
    ContainerOut result;
    if (size_of_cont(xs) < 2)
        return result;
    internal::prepare_container(result, size_of_cont(xs));
    auto itOut = internal::get_back_inserter(result);
    auto it1 = std::begin(xs);
    auto it2 = it1;
    internal::advance_iterator(it2, 1);
    for (; it2 != std::end(xs); ++it1, ++it2)
    {
        *itOut = std::make_pair(*it1, *it2);
    }
    *itOut = std::make_pair(*it1, xs.front());
    return result;
}

// API search type: enumerate : [a] -> [(Int, a)]
// fwd bind count: 0
// Attach its index to every element of a sequence.
// enumerate([6,4,7,6]) == [(0, 6), (1, 4), (2, 7), (3, 6)]
template <typename Container>
auto enumerate(const Container& xs)
{
    return zip(all_idxs(xs), xs);
}

// API search type: inner_product_with : (((a, a) -> b), ((b, b) -> b), b, [a], [a]) -> b
// fwd bind count: 4
// Calculate the inner product of two sequences using custom operations.
// inner_product_with((+), (*), [1, 2, 3], [4, 5, 6]) == [32]
template <
    typename ContainerIn1,
    typename ContainerIn2,
    typename OP1,
    typename OP2,
    typename Acc,
    typename X = typename ContainerIn1::value_type,
    typename Y = typename ContainerIn2::value_type,
    typename OP2Out = internal::invoke_result_t<OP2, X, Y>>
auto inner_product_with(OP1 op1,
                        OP2 op2,
                        const Acc& value,
                        const ContainerIn1& xs,
                        const ContainerIn2& ys)
{
    internal::trigger_static_asserts<internal::inner_product_with_tag, OP2, X, Y>();
    internal::trigger_static_asserts<internal::inner_product_with_tag, OP1, Acc, OP2Out>();
    assert(size_of_cont(xs) == size_of_cont(ys));
    return std::inner_product(
        std::begin(xs), std::end(xs), std::begin(ys), value, op1, op2);
}

// API search type: inner_product : (a, [a], [a]) -> a
// fwd bind count: 2
// Calculate the inner product of two sequences.
// inner_product([1, 2, 3], [4, 5, 6]) == [32]
template <typename ContainerIn1, typename ContainerIn2,
    typename Z>
Z inner_product(const Z& value,
        const ContainerIn1& xs, const ContainerIn2& ys)
{
    assert(size_of_cont(xs) == size_of_cont(ys));

    return std::inner_product(
        std::begin(xs), std::end(xs), std::begin(ys), value);
}

// API search type: first_mismatch_idx_by : (((a, b) -> Bool), [a], [b]) -> Maybe Int
// fwd bind count: 2
// Find the first index at which the two sequences differ
// using a binary predicate.
// first_mismatch_idx_by((==), [1, 2, 3], [1, 4, 3]) == Just 1
// first_mismatch_idx_by((==), [1, 2, 3], [1, 4]) == Just 1
// first_mismatch_idx_by((==), [1, 2, 3], [1, 2]) == Nothing
// first_mismatch_idx_by((==), [], [1, 2]) == Nothing
template <typename ContainerIn1, typename ContainerIn2,
    typename BinaryPredicate>
maybe<std::size_t> first_mismatch_idx_by(BinaryPredicate p,
    const ContainerIn1& xs, const ContainerIn2& ys)
{
    auto itXs = std::begin(xs);
    auto itYs = std::begin(ys);
    std::size_t minSize = std::min(size_of_cont(xs), size_of_cont(ys));
    for (std::size_t i = 0; i < minSize; ++i)
    {
        if (!internal::invoke(p, *itXs, *itYs))
        {
            return just(i);
        }
        ++itXs;
        ++itYs;
    }
    return nothing<std::size_t>();
}

// API search type: first_mismatch_by : (((a, b) -> Bool), [a], [b]) -> Maybe (a, b)
// fwd bind count: 2
// Find the first pair of elements differing in the two sequences
// using a binary predicate.
// first_mismatch_by((==), [1, 2, 3], [1, 4, 3]) == Just (2, 4)
// first_mismatch_by((==), [1, 2, 3], [1, 4]) == Just (2, 4)
// first_mismatch_by((==), [1, 2, 3], [1, 2]) == Nothing
// first_mismatch_by((==), [], [1, 2]) == Nothing
template <typename ContainerIn1, typename ContainerIn2,
    typename BinaryPredicate,
    typename X = typename ContainerIn1::value_type,
    typename Y = typename ContainerIn2::value_type,
    typename TOut = std::pair<X, Y>>
maybe<TOut> first_mismatch_by(BinaryPredicate p,
    const ContainerIn1& xs, const ContainerIn2& ys)
{
    const auto maybe_idx = first_mismatch_idx_by(p, xs, ys);
    if (is_nothing(maybe_idx))
    {
        return nothing<TOut>();
    }
    else
    {
        const auto idx = maybe_idx.unsafe_get_just();
        return just(std::make_pair(
            elem_at_idx(idx, xs),
            elem_at_idx(idx, ys)));
    }
}

// API search type: first_mismatch_idx_on : ((a -> b), [a], [a]) -> Maybe Int
// fwd bind count: 2
// Find the first index of elements differing in the two sequences
// using a transformer.
// first_mismatch_idx_on((mod 2), [1, 2, 3], [3, 5, 3]) == 1
// first_mismatch_idx_on((mod 2), [1, 2, 3], [1, 5]) == 1
// first_mismatch_idx_on((mod 2), [1, 2, 3], [1, 6]) == Nothing
// first_mismatch_idx_on((mod 2), [], [1, 2]) == Nothing
template <typename ContainerIn1, typename ContainerIn2,
    typename F,
    typename X = typename ContainerIn1::value_type,
    typename Y = typename ContainerIn2::value_type,
    typename TOut = std::pair<X, Y>>
maybe<std::size_t> first_mismatch_idx_on(F f,
    const ContainerIn1& xs, const ContainerIn2& ys)
{
    static_assert(std::is_same<X, Y>::value,
        "Both containers must have the same element type.");
    return first_mismatch_idx_by(is_equal_by(f), xs, ys);
}

// API search type: first_mismatch_on : ((a -> b), [a], [a]) -> Maybe (a, a)
// fwd bind count: 2
// Find the first pair of elements differing in the two sequences
// using a transformer.
// first_mismatch_on((mod 2), [1, 2, 3], [3, 5, 3]) == Just (2, 5)
// first_mismatch_on((mod 2), [1, 2, 3], [1, 5]) == Just (2, 5)
// first_mismatch_on((mod 2), [1, 2, 3], [1, 6]) == Nothing
// first_mismatch_on((mod 2), [], [1, 2]) == Nothing
template <typename ContainerIn1, typename ContainerIn2,
    typename F,
    typename X = typename ContainerIn1::value_type,
    typename Y = typename ContainerIn2::value_type,
    typename TOut = std::pair<X, Y>>
maybe<TOut> first_mismatch_on(F f,
    const ContainerIn1& xs, const ContainerIn2& ys)
{
    static_assert(std::is_same<X, Y>::value,
        "Both containers must have the same element type.");
    return first_mismatch_by(is_equal_by(f), xs, ys);
}

// API search type: first_mismatch_idx : ([a], [a]) -> Maybe Int
// fwd bind count: 2
// Find the first index of elements differing in the two sequences.
// first_mismatch_idx((==), [1, 2, 3], [1, 4, 3]) == 1
// first_mismatch_idx((==), [1, 2, 3], [1, 4]) == 1
// first_mismatch_idx((==), [1, 2, 3], [1, 2]) == Nothing
// first_mismatch_idx((==), [], [1, 2]) == Nothing
template <typename ContainerIn1, typename ContainerIn2,
    typename X = typename ContainerIn1::value_type,
    typename Y = typename ContainerIn2::value_type>
maybe<std::size_t> first_mismatch_idx(
    const ContainerIn1& xs, const ContainerIn2& ys)
{
    static_assert(std::is_same<X, Y>::value,
        "Both containers must have the same element type.");
    return first_mismatch_idx_by(std::equal_to<X>(), xs, ys);
}

// API search type: first_mismatch : ([a], [a]) -> Maybe (a, a)
// fwd bind count: 2
// Find the first pair of elements differing in the two sequences
// first_mismatch((==), [1, 2, 3], [1, 4, 3]) == Just (2, 4)
// first_mismatch((==), [1, 2, 3], [1, 4]) == Just (2, 4)
// first_mismatch((==), [1, 2, 3], [1, 2]) == Nothing
// first_mismatch((==), [], [1, 2]) == Nothing
template <typename ContainerIn1, typename ContainerIn2,
    typename X = typename ContainerIn1::value_type,
    typename Y = typename ContainerIn2::value_type,
    typename TOut = std::pair<X, Y>>
maybe<TOut> first_mismatch(const ContainerIn1& xs, const ContainerIn2& ys)
{
    static_assert(std::is_same<X, Y>::value,
        "Both containers must have the same element type.");
    return first_mismatch_by(std::equal_to<X>(), xs, ys);
}

// API search type: first_match_idx_by : (((a, b) -> Bool), [a], [b]) -> Maybe Int
// fwd bind count: 2
// Find the first index at which the two sequences equal
// using a binary predicate.
// first_match_idx_by((==), [1, 2, 3], [3, 2, 3]) == Just 1
// first_match_idx_by((==), [], [1, 2]) == Nothing
template <typename ContainerIn1, typename ContainerIn2,
    typename F,
    typename X = typename ContainerIn1::value_type,
    typename Y = typename ContainerIn2::value_type>
maybe<std::size_t> first_match_idx_by(F f,
    const ContainerIn1& xs, const ContainerIn2& ys)
{
    auto itXs = std::begin(xs);
    auto itYs = std::begin(ys);
    std::size_t minSize = std::min(size_of_cont(xs), size_of_cont(ys));
    for (std::size_t i = 0; i < minSize; ++i)
    {
        if (internal::invoke(f, *itXs, *itYs))
        {
            return just(i);
        }
        ++itXs;
        ++itYs;
    }
    return nothing<std::size_t>();
}

// API search type: first_match_by : (((a, b) -> Bool), [a], [b]) -> Maybe (a, b)
// fwd bind count: 2
// Find the first pair of equal elements in the two sequences
// using a binary predicate.
// first_match_by((==), [1, 2, 3], [3, 2, 3]) == Just (2, 2)
// first_match_by((==), [], [1, 2]) == Nothing
template <typename ContainerIn1, typename ContainerIn2,
    typename F,
    typename X = typename ContainerIn1::value_type,
    typename Y = typename ContainerIn2::value_type,
    typename TOut = std::pair<X, Y>>
maybe<TOut> first_match_by(F f, const ContainerIn1& xs, const ContainerIn2& ys)
{
    const auto maybe_idx = first_match_idx_by(f, xs, ys);
    if (is_nothing(maybe_idx))
    {
        return nothing<TOut>();
    }
    else
    {
        const auto idx = maybe_idx.unsafe_get_just();
        return just(std::make_pair(
            elem_at_idx(idx, xs),
            elem_at_idx(idx, ys)));
    }
}

// API search type: first_match_idx_on : ((a -> b), [a], [a]) -> Maybe Int
// fwd bind count: 2
// Find the first index of equal elements in the two sequences
// using a transformer.
// first_match_idx_on((mod 2), [1, 2, 3], [2, 4, 3]) == 1
// first_match_idx_on((mod 2), [], [1, 2]) == Nothing
template <typename ContainerIn1, typename ContainerIn2,
    typename F,
    typename X = typename ContainerIn1::value_type,
    typename Y = typename ContainerIn2::value_type>
maybe<std::size_t> first_match_idx_on(F f,
    const ContainerIn1& xs, const ContainerIn2& ys)
{
    static_assert(std::is_same<X, Y>::value,
        "Both containers must have the same element type.");
    return first_match_idx_by(is_equal_by(f), xs, ys);
}

// API search type: first_match_on : ((a -> b), [a], [a]) -> Maybe (a, a)
// fwd bind count: 2
// Find the first pair of equal elements in the two sequences
// using a transformer.
// first_match_on((mod 2), [1, 2, 3], [2, 4, 3]) == Just (2, 4)
// first_match_on((mod 2), [], [1, 2]) == Nothing
template <typename ContainerIn1, typename ContainerIn2,
    typename F,
    typename X = typename ContainerIn1::value_type,
    typename Y = typename ContainerIn2::value_type,
    typename TOut = std::pair<X, Y>>
maybe<TOut> first_match_on(F f, const ContainerIn1& xs, const ContainerIn2& ys)
{
    static_assert(std::is_same<X, Y>::value,
        "Both containers must have the same element type.");
    return first_match_by(is_equal_by(f), xs, ys);
}

// API search type: first_match_idx : ([a], [a]) -> Maybe Int
// fwd bind count: 2
// Find the first index of equal elements in the two sequences.
// first_match_idx((==), [1, 2, 3], [5, 2, 3]) == 1
// first_match_idx((==), [], [1, 2]) == Nothing
template <typename ContainerIn1, typename ContainerIn2,
    typename X = typename ContainerIn1::value_type,
    typename Y = typename ContainerIn2::value_type>
maybe<std::size_t> first_match_idx(
    const ContainerIn1& xs, const ContainerIn2& ys)
{
    static_assert(std::is_same<X, Y>::value,
        "Both containers must have the same element type.");
    return first_match_idx_by(std::equal_to<X>(), xs, ys);
}

// API search type: first_match : ([a], [a]) -> Maybe (a, a)
// fwd bind count: 2
// Find the first pair of equal elements in the two sequences.
// first_match((==), [1, 2, 3], [5, 2, 3]) == Just (2, 2)
// first_match((==), [], [1, 2]) == Nothing
template <typename ContainerIn1, typename ContainerIn2,
    typename X = typename ContainerIn1::value_type,
    typename Y = typename ContainerIn2::value_type,
    typename TOut = std::pair<X, Y>>
maybe<TOut> first_match(const ContainerIn1& xs, const ContainerIn2& ys)
{
    static_assert(std::is_same<X, Y>::value,
        "Both containers must have the same element type.");
    return first_match_by(std::equal_to<X>(), xs, ys);
}

} // namespace fplus

#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>
#include <stdexcept>
#include <type_traits>

namespace fplus
{

// API search type: is_in_interval : (a, a, a) -> Bool
// fwd bind count: 2
// Checks if x is in [low, high), i.e. left-closed and right-open.
template <typename T>
bool is_in_interval(const T& low, const T& high, const T& x)
{
    return (low <= x) && (x < high);
}

// API search type: is_in_interval_around : (a, a, a) -> Bool
// fwd bind count: 2
// Checks if x is in [center - radius, center + radius),
// i.e. left-closed and right-open.
template <typename T>
bool is_in_interval_around(const T& radius, const T& center, const T& x)
{
    return is_in_interval(center - radius, center + radius, x);
}

// API search type: is_in_open_interval : (a, a, a) -> Bool
// fwd bind count: 2
// Checks if x is in (low, high), i.e. left-open and right-open.
template <typename T>
bool is_in_open_interval(const T& low, const T& high, const T& x)
{
    return (low < x) && (x < high);
}

// API search type: is_in_open_interval_around : (a, a, a) -> Bool
// fwd bind count: 2
// Checks if x is in (center - radius, center + radius),
// i.e. left-open and right-open.
template <typename T>
bool is_in_open_interval_around(const T& radius, const T& center, const T& x)
{
    return is_in_open_interval(center - radius, center + radius, x);
}

// API search type: is_in_closed_interval : (a, a, a) -> Bool
// fwd bind count: 2
// Checks if x is in [low, high], i.e. left-closed and right-closed.
template <typename T>
bool is_in_closed_interval(const T& low, const T& high, const T& x)
{
    return (low <= x) && (x <= high);
}

// API search type: is_in_closed_interval_around : (a, a, a) -> Bool
// Checks if x is in [center - radius, center + radius],
// i.e. left-closed and right-closed.
template <typename T>
bool is_in_closed_interval_around(const T& radius, const T& center, const T& x)
{
    return is_in_closed_interval(center - radius, center + radius, x);
}

// API search type: reference_interval : (Float, Float, Float, Float, Float) -> Float
// fwd bind count: 4
// Linearly projects a value
// from [old_low, old_high] into [new_low, new_high].
// Does not clamp.
// reference_interval(2, 6, 0, 4, 3) == 5
// reference_interval(2, 10, 0, 4, 3) == 8
// reference_interval(2, 6, 0, 4, -1) == 1
// reference_interval(2, 10, 0, 4, -1) == 0
template <typename T>
T reference_interval(const T& new_low, const T& new_high,
    const T& old_low, const T& old_high, const T& x)
{
    const T scale = (new_high - new_low) / (old_high - old_low);
    return scale * (x - old_low) + new_low;
}

// API search type: clamp : (a, a, a) -> a
// fwd bind count: 2
// Puts value into [low, high], i.e. left-closed and right-closed.
template <typename T>
T clamp(const T& low, const T& high, const T& x)
{
    return std::max(low, std::min(high, x));
}

// API search type: is_negative : a -> Bool
// fwd bind count: 0
// Checks if x < 0.
template <typename X>
bool is_negative(X x)
{
    return x < 0;
}

// API search type: is_positive : a -> Bool
// fwd bind count: 0
// Checks if x is not negative.
template <typename X>
bool is_positive(X x)
{
    return !is_negative(x);
}

// API search type: is_even : Int -> Bool
// fwd bind count: 0
// Checks if x is even.
template <typename X>
bool is_even(X x)
{
    static_assert(std::is_integral<X>::value, "type must be integral");
    return x % 2 == 0;
}

// API search type: is_odd : Int -> Bool
// fwd bind count: 0
// Checks if x is odd.
template <typename X>
bool is_odd(X x)
{
    static_assert(std::is_integral<X>::value, "type must be integral");
    return x % 1 == 0;
}

namespace internal
{
    template <typename X>
    typename std::enable_if<std::is_unsigned<X>::value, X>::type
    abs_helper(X x)
    {
        return x;
    }

    template <typename X>
    typename std::enable_if<!std::is_unsigned<X>::value, X>::type
    abs_helper(X x)
    {
        return std::abs(x);
    }
}

// API search type: abs : a -> a
// fwd bind count: 0
// Returns the absolute (always non-negative) value of x.
template <typename X>
X abs(X x)
{
    return internal::abs_helper(x);
}

// API search type: abs_diff : (a, a) -> a
// fwd bind count: 1
// Returns the absolute difference of two values.
template <typename X>
X abs_diff(X a, X b)
{
    return a > b ? a - b : b - a;
}

// API search type: square : a -> a
// fwd bind count: 0
// Returns the square (x*x) of a value x.
template <typename X>
X square(X x)
{
    return x * x;
}

// API search type: cube : a -> a
// fwd bind count: 0
// Returns the cube (x*x*x) of a value x.
template <typename X>
X cube(X x)
{
    return x * x * x;
}

// API search type: sign : a -> Int
// fwd bind count: 0
// Returns -1 for negative values, 1 otherwise.
// sign(-3) == -1
// sign(0) == 1
// sign(16) == 1
template <typename X>
int sign(X x)
{
    return is_negative(x) ? -1 : 1;
}

// API search type: sign_with_zero : a -> Int
// fwd bind count: 0
// Returns -1 for negative values, 0 for zero, 1 for positive values.
// sign_with_zero(-3) == -1
// sign_with_zero(0) == 0
// sign_with_zero(16) == 1
template <typename X>
int sign_with_zero(X x)
{
    return x == 0 ? 0 : sign(x);
}

// API search type: integral_cast_throw : Int -> Int
// fwd bind count: 0
// Converts one integer type into another.
// Throws an std::underflow_error or std::overflow_error
// if the value does not fit into the destination type.
template <typename Out, typename X>
Out integral_cast_throw(X x)
{
#ifdef _MSC_VER
__pragma(warning(push))
__pragma(warning(disable:4127))
#endif
    static_assert(std::is_integral<X>::value, "type must be integral");
    static_assert(std::is_integral<Out>::value, "type must be integral");
    if (std::is_signed<X>::value && std::is_signed<Out>::value)
    {
        if (static_cast<std::int64_t>(x) <
            static_cast<std::int64_t>(std::numeric_limits<Out>::lowest()))
        {
            throw std::underflow_error("");
        }
        if (static_cast<std::int64_t>(x) >
            static_cast<std::int64_t>(std::numeric_limits<Out>::max()))
        {
            throw std::overflow_error("");
        }
        return static_cast<Out>(x);
    }
    else if (!std::is_signed<X>::value && !std::is_signed<Out>::value)
    {
        if (static_cast<std::uint64_t>(x) <
            static_cast<std::uint64_t>(std::numeric_limits<Out>::lowest()))
        {
            throw std::underflow_error("");
        }
        if (static_cast<std::uint64_t>(x) >
            static_cast<std::uint64_t>(std::numeric_limits<Out>::max()))
        {
            throw std::overflow_error("");
        }
        return static_cast<Out>(x);
    }
    else if (std::is_signed<X>::value && !std::is_signed<Out>::value)
    {
        if (x < 0)
            return 0;
        if (static_cast<std::uint64_t>(x) >
            static_cast<std::uint64_t>(std::numeric_limits<Out>::max()))
        {
            throw std::overflow_error("");
        }
        return static_cast<Out>(x);
    }
    else if (!std::is_signed<X>::value && std::is_signed<Out>::value)
    {
        if (static_cast<std::uint64_t>(x) >
            static_cast<std::uint64_t>(std::numeric_limits<Out>::max()))
        {
            throw std::overflow_error("");
        }
        return static_cast<Out>(x);
    }
    else
    {
        assert(false);
        return static_cast<Out>(x);
    }
#ifdef _MSC_VER
__pragma(warning(pop))
#endif
}

// API search type: integral_cast_clamp : Int -> Int
// fwd bind count: 0
// Converts one integer type into another.
// If the value does not fit into the destination type,
// the nearest possible value is used.
// Also known as saturate_cast.
template <typename Out, typename X>
Out integral_cast_clamp(X x)
{
    static_assert(std::is_integral<X>::value, "type must be integral");
    static_assert(std::is_integral<Out>::value, "type must be integral");
    if (std::is_signed<X>::value && std::is_signed<Out>::value)
    {
        if (static_cast<std::int64_t>(x) <
            static_cast<std::int64_t>(std::numeric_limits<Out>::lowest()))
        {
            return std::numeric_limits<Out>::lowest();
        }
        if (static_cast<std::int64_t>(x) >
            static_cast<std::int64_t>(std::numeric_limits<Out>::max()))
        {
            return std::numeric_limits<Out>::max();
        }
        return static_cast<Out>(x);
    }
    else if (!std::is_signed<X>::value && !std::is_signed<Out>::value)
    {
        if (static_cast<std::uint64_t>(x) <
            static_cast<std::uint64_t>(std::numeric_limits<Out>::lowest()))
        {
            return std::numeric_limits<Out>::lowest();
        }
        if (static_cast<std::uint64_t>(x) >
            static_cast<std::uint64_t>(std::numeric_limits<Out>::max()))
        {
            return std::numeric_limits<Out>::max();
        }
        return static_cast<Out>(x);
    }
    else if (std::is_signed<X>::value && !std::is_signed<Out>::value)
    {
        if (x < 0)
            return 0;
        if (static_cast<std::uint64_t>(x) >
            static_cast<std::uint64_t>(std::numeric_limits<Out>::max()))
        {
            return std::numeric_limits<Out>::max();
        }
        return static_cast<Out>(x);
    }
    else if (!std::is_signed<X>::value && std::is_signed<Out>::value)
    {
        if (static_cast<std::uint64_t>(x) >
            static_cast<std::uint64_t>(std::numeric_limits<Out>::max()))
        {
            return std::numeric_limits<Out>::max();
        }
        return static_cast<Out>(x);
    }
    else
    {
        assert(false);
        return static_cast<Out>(x);
    }
}

// API search type: round : a -> Int
// fwd bind count: 0
// Converts a value to the nearest integer.
template <typename X, typename Out = int>
Out round(X x)
{
    static_assert(!std::is_integral<X>::value, "type must be non-integral");
    static_assert(std::is_integral<Out>::value, "type must be integral");
    if (static_cast<double>(x) < static_cast<double>(std::numeric_limits<Out>::lowest()))
        return std::numeric_limits<Out>::lowest();
    if (static_cast<double>(x) > static_cast<double>(std::numeric_limits<Out>::max()))
        return std::numeric_limits<Out>::max();
    if (is_negative(x))
        x -= 1;
    return static_cast<Out>(x + 0.5);
}

// API search type: floor : a -> b
// fwd bind count: 0
// Converts a value to the nearest smaller integer.
template <typename X, typename Out = int>
Out floor(X x)
{
    static_assert(!std::is_integral<X>::value, "type must be non-integral");
    static_assert(std::is_integral<Out>::value, "type must be integral");
    if (is_negative(x))
        x -= 1;
    return static_cast<Out>(x);
}

// API search type: floor_to_int_mult : (Int, Int) -> Int
// fwd bind count: 1
// Rounds an integer down to the nearest smaller or equal multiple of n.
// n may not be zero.
template <typename X>
X floor_to_int_mult(X n, X x)
{
    static_assert(std::is_integral<X>::value, "type must be integral");
    assert(n != 0);
    if (is_negative(n))
        n = abs(n);
    if (is_negative(x) && n != 1)
        x = static_cast<X>(x - 1);
    return static_cast<X>((x / n) * n);
}

// API search type: ceil_to_int_mult : (Int, Int) -> Int
// fwd bind count: 1
// Rounds an integer up to the nearest greater or equal multiple of n.
// n may not be zero.
template <typename X>
X ceil_to_int_mult(X n, X x)
{
    return floor_to_int_mult(n, static_cast<X>(x + abs(n) - 1));
}

// API search type: ceil : a -> b
// fwd bind count: 0
// Converts a value to the nearest greater integer.
template <typename X, typename Out = int>
Out ceil(X x)
{
    static_assert(!std::is_integral<X>::value, "type must be non-integral");
    static_assert(std::is_integral<Out>::value, "type must be integral");
    return floor<X, Out>(x) + 1;
}

// API search type: int_power : (Int, Int) -> Int
// fwd bind count: 1
// integer power, only exponents >= 0
template <typename X>
X int_power(X base, X exp)
{
    static_assert(std::is_integral<X>::value,
        "type must be unsigned integral");
    assert(!is_negative(exp));
    if (exp == 0)
        return 1;
    if (exp == 1)
        return base;
    return base * int_power(base, exp - 1);
}

namespace internal
{
    // minimum of x values after transformation
    // (has an overload for non-POD types)
    // min_on(mod2, 4, 3) == 4
    // min_on(mod7, 3, 5, 7, 3) == 7
    template <typename F, typename FirstT, typename... FIn>
    auto helper_min_on(F f, const FirstT& first, const FIn&... v) ->
        typename std::common_type<FirstT, FIn...>::type
    {
      using rettype = typename std::common_type<FirstT, FIn...>::type;
      using f_rettype = std::decay_t<internal::invoke_result_t<F, decltype(first)>>;

      rettype result = first;
      f_rettype result_trans = internal::invoke(f, first);
      f_rettype v_trans;
      unused(result_trans);
      unused(v_trans);

      (void)std::initializer_list<int>{
          ((v_trans = internal::invoke(f, v), v_trans < result_trans)
               ? (result = static_cast<rettype>(v), result_trans = v_trans, 0)
               : 0)...};
      return result;
    }

    template <typename F>
    struct helper_min_on_t
    {
        helper_min_on_t(F _f) : f(_f) {}
        template <typename T, typename... Ts>
        auto operator()(T&& x, Ts&&... xs) -> typename std::common_type<T, Ts...>::type
        {
            return helper_min_on(std::forward<F>(f), std::forward<T>(x), std::forward<Ts>(xs)...);
        }
    private:
        F f;
    };
}

// API search type: min_on : ((a -> b), a, a) -> a
// minimum of x values after transformation (curried version)
// min_on(mod2)(4, 3) == 4
// min_on(mod7)(3, 5, 7, 3) == 7
template <typename F>
auto min_on(F f) -> internal::helper_min_on_t<F>
{
    return internal::helper_min_on_t<F>{f};
}

// API search type: min_2_on : ((a -> b), a, a) -> a
// fwd bind count: 2
// minimum of 2 values after transformation
// min_2_on(mod2, 4, 3) == 4
template <typename F, typename T>
T min_2_on(F f, const T& x, const T& y)
{
    return internal::invoke(f, y) < internal::invoke(f, x) ? y : x;
}

namespace internal
{
    // maximum of x values after transformation
    // (has an overload for non-POD types)
    // max_on(mod2, 4, 3) == 3
    // max_on(mod7, 3, 5, 7, 3) == 5
    template <typename F, typename FirstT, typename... FIn>
    auto helper_max_on(F f, const FirstT& first, const FIn&... v) ->
        typename std::common_type<FirstT, FIn...>::type
    {
      using rettype = typename std::common_type<FirstT, FIn...>::type;
      using f_rettype = decltype(f(first));

      rettype result = first;
      f_rettype result_trans = internal::invoke(f, first);
      f_rettype v_trans;
      unused(result_trans);
      unused(v_trans);

      (void)std::initializer_list<int>{
          ((v_trans = internal::invoke(f, v), v_trans > result_trans)
               ? (result = static_cast<rettype>(v), result_trans = v_trans, 0)
               : 0)...};
      return result;
    }

    template <typename F>
    struct helper_max_on_t
    {
        helper_max_on_t(F _f) : f(_f) {}
        template <typename T, typename... Ts>
        auto operator()(T&& x, Ts&&... xs) -> typename std::common_type<T, Ts...>::type
        {
            return helper_max_on(std::forward<F>(f), std::forward<T>(x), std::forward<Ts>(xs)...);
        }
    private:
        F f;
    };
}

// API search type: max_on : (a -> b) -> ((a, a) -> a)
// maximum of x values after transformation (curried version)
// (has an overload for non POD types)
// max_on(mod2)(4, 3) == 3
// max_on(mod7)(3, 5, 7, 3) == 5
template <typename F>
auto max_on(F f) -> internal::helper_max_on_t<F>
{
    return internal::helper_max_on_t<F>{f};
}

// API search type: max_2_on : ((a -> b), a, a) -> a
// fwd bind count: 2
// maximum of 2 values after transformation
// max_2_on(mod2, 4, 3) == 3
template <typename F, typename T>
T max_2_on(F f, const T& x, const T& y)
{
    return internal::invoke(f, y) > internal::invoke(f, x) ? y : x;
}

// API search type: min : (a, a) -> a
// Minimum of x number of values
// min(4, 3) == 3
// min(4, 3, 6, 2, 3) == 2
template <typename U, typename... V>
auto min(const U& u, const V&... v) -> typename std::common_type<U, V...>::type
{
  using rettype = typename std::common_type<U, V...>::type;
  rettype result = static_cast<rettype>(u);
  (void)std::initializer_list<int>{((v < result) ? (result = static_cast<rettype>(v), 0) : 0)...};
  return result;
}

// API search type: min_2 : (a, a) -> a
// fwd bind count: 1
// minimum of 2 values
// min_2(4, 3) == 3
template <typename T>
T min_2(const T& x, const T& y)
{
    return y < x ? y : x;
}

// API search type: max : (a, a) -> a
// Maximum of x number of values.
// max(4, 3) == 4
// max(4, 3, 6, 2, 3) == 6
template <typename U, typename... V>
auto max(const U& u, const V&... v) -> typename std::common_type<U, V...>::type
{
  using rettype = typename std::common_type<U, V...>::type;
  rettype result = static_cast<rettype>(u);
  (void)std::initializer_list<int>{((v > result) ? (result = static_cast<rettype>(v), 0) : 0)...};
  return result;
}

// API search type: max_2 : (a, a) -> a
// fwd bind count: 1
// maximum of 2 values
// max_2(4, 3) == 4
template <typename T>
T max_2(const T& x, const T& y)
{
    return y > x ? y : x;
}

namespace internal
{
    template <typename X>
    typename std::enable_if<std::is_floating_point<X>::value, X>::type
    cyclic_value_helper_mod(X x, X y)
    {
        return std::fmod(x, y);
    }

    template <typename X>
    typename std::enable_if<std::is_integral<X>::value, X>::type
    cyclic_value_helper_mod(X x, X y)
    {
        return x % y;
    }
}

// API search type: cyclic_value : a -> (a -> a)
// Modulo for floating point values.
// circumfence must be > 0
// cyclic_value(8)(3) == 3
// cyclic_value(8)(11) == 3
// cyclic_value(8)(19) == 3
// cyclic_value(8)(-2) == 6
// cyclic_value(8)(-5) == 3
// cyclic_value(8)(-13) == 3
// Can be useful to normalize an angle into [0, 360]
// For positive values it behaves like std::fmod with flipped arguments.
template <typename X>
std::function<X(X)> cyclic_value(X circumfence)
{
    assert(circumfence > 0);
    return [circumfence](X x) -> X
    {
        if (sign(x) < 0)
            return circumfence - internal::cyclic_value_helper_mod(
                abs(x), abs(circumfence));
        else
            return internal::cyclic_value_helper_mod(
                abs(x), abs(circumfence));
    };
}

// API search type: cyclic_difference : a -> ((a, a) -> a)
// Returns the distance the first value has to advance forward on a circle
// to reach the second value.
// circumfence must be > 0
// cyclic_difference(100)(5, 2) == 3
// cyclic_difference(100)(2, 5) == 97
// cyclic_difference(100)(3, -2) == 5
// cyclic_difference(100)(-2, 3) == 95
// cyclic_difference(100)(90, 10) == 80
// cyclic_difference(100)(10, 90) == 20
template <typename X>
std::function<X(X, X)> cyclic_difference(X circumfence)
{
    assert(circumfence > 0);
    return [circumfence](X a, X b) -> X
    {
        auto cyclic_value_f = cyclic_value(circumfence);
        const auto c_v_a = cyclic_value_f(a);
        const auto c_v_b = cyclic_value_f(b);
        return c_v_a > c_v_b ?
            c_v_a - c_v_b :
            circumfence + c_v_a - c_v_b;
    };
}

// API search type: cyclic_shortest_difference : a -> ((a, a) -> a)
// Returns displacement (shortest way) the first value has to move on a circle
// to reach the second value.
// circumfence must be > 0
// cyclic_shortest_difference(100)(5, 2) == 3
// cyclic_shortest_difference(100)(2, 5) == -3
// cyclic_shortest_difference(100)(3, -2) == 5
// cyclic_shortest_difference(100)(-2, 3) == -5
// cyclic_shortest_difference(100)(90, 10) == -20
// cyclic_shortest_difference(100)(10, 90) == 20
template <typename X>
std::function<X(X, X)> cyclic_shortest_difference(X circumfence)
{
    assert(circumfence > 0);
    return [circumfence](X a, X b) -> X
    {
        auto diff_func = cyclic_difference(circumfence);
        auto a_minus_b = diff_func(a, b);
        auto b_minus_a = diff_func(b, a);
        return a_minus_b <= b_minus_a ? a_minus_b : -b_minus_a;
    };
}

// API search type: cyclic_distance : a -> ((a, a) -> a)
// Returns distance (shortest way) the first value has to move on a circle
// to reach the second value.
// circumfence must be > 0
// cyclic_distance(100)(2, 5) == 3
// cyclic_distance(100)(5, 2) == 3
// cyclic_distance(100)(-2, 3) == 5
// cyclic_distance(100)(3, -2) == 5
// cyclic_distance(100)(10, 90) == 20
// cyclic_distance(100)(90, 10) == 20
// Can be useful to calculate the difference of two angles;
template <typename X>
std::function<X(X, X)> cyclic_distance(X circumfence)
{
    assert(circumfence > 0);
    return [circumfence](X a, X b) -> X
    {
        auto diff_func = cyclic_difference(circumfence);
        auto a_minus_b = diff_func(a, b);
        auto b_minus_a = diff_func(b, a);
        return a_minus_b <= b_minus_a ? a_minus_b : b_minus_a;
    };
}

// API search type: pi : () -> Float
// Pi.
constexpr inline double pi()
{
    return 3.14159265358979323846;
}

// API search type: deg_to_rad : Float -> Float
// fwd bind count: 0
// converts degrees to radians
template <typename T>
T deg_to_rad(T x)
{
    static_assert(std::is_floating_point<T>::value, "Please use a floating-point type.");
    return static_cast<T>(x * pi() / 180.0);
}

// API search type: rad_to_deg : Float -> Float
// fwd bind count: 0
// converts radians to degrees
template <typename T>
T rad_to_deg(T x)
{
    static_assert(std::is_floating_point<T>::value, "Please use a floating-point type.");
    return static_cast<T>(x * 180.0 / pi());
}

namespace internal
{

template <typename Container, typename T>
Container normalize_min_max(internal::reuse_container_t,
    const T& lower, const T& upper, Container&& xs)
{
    assert(size_of_cont(xs) != 0);
    assert(lower <= upper);
    const auto minmax_it_p = std::minmax_element(std::begin(xs), std::end(xs));
    const T x_min = *minmax_it_p.first;
    const T x_max = *minmax_it_p.second;
    const auto f = [&](const T& x) -> T
    {
        return lower + (upper - lower) * (x - x_min) / (x_max - x_min);
    };
    std::transform(std::begin(xs), std::end(xs), std::begin(xs), f);
    return std::forward<Container>(xs);
}

template <typename Container, typename T>
Container normalize_min_max(internal::create_new_container_t,
    const T& lower, const T& upper, const Container& xs)
{
    auto ys = xs;
    return normalize_min_max(internal::reuse_container_t(),
        lower, upper, std::move(ys));
}

} // namespace internal

// API search type: normalize_min_max : (a, a, [a]) -> [a]
// fwd bind count: 2
// Linearly scales the values into the given interval.
// normalize_min_max(0, 10, [1, 3, 6]) == [0, 4, 10]
// It is recommended to convert integers to double beforehand.
template <typename Container,
    typename T = typename internal::remove_const_and_ref_t<Container>::value_type>
auto normalize_min_max(const T& lower, const T& upper, Container&& xs)
{
    return internal::normalize_min_max(internal::can_reuse_v<Container>{},
        lower, upper, std::forward<Container>(xs));
}

namespace internal
{

template <typename Container, typename T>
Container normalize_mean_stddev(internal::reuse_container_t,
    const T& mean, const T& stddev, Container&& xs)
{
    assert(size_of_cont(xs) != 0);
    const auto mean_and_stddev = fplus::mean_stddev<T>(xs);
    const auto f = [&](const T& x) -> T
    {
        return mean +
            stddev * (x - mean_and_stddev.first) / mean_and_stddev.second;
    };
    std::transform(std::begin(xs), std::end(xs), std::begin(xs), f);
    return std::forward<Container>(xs);
}

template <typename Container, typename T>
Container normalize_mean_stddev(internal::create_new_container_t,
    const T& mean, const T& stddev, const Container& xs)
{
    auto ys = xs;
    return normalize_mean_stddev(internal::reuse_container_t(),
        mean, stddev, std::move(ys));
}

} // namespace internal

// API search type: normalize_mean_stddev : (a, a, [a]) -> [a]
// fwd bind count: 2
// Linearly scales the values
// to match the given mean and population standard deviation.
// normalize_mean_stddev(3, 2, [7, 8]) == [1, 5]
template <typename Container,
    typename T = typename internal::remove_const_and_ref_t<Container>::value_type>
auto normalize_mean_stddev(
    const T& mean, const T& stddev, Container&& xs)
{
    return internal::normalize_mean_stddev(internal::can_reuse_v<Container>{},
        mean, stddev, std::forward<Container>(xs));
}

// API search type: standardize : [a] -> [a]
// fwd bind count: 0
// Linearly scales the values to zero mean and population standard deviation 1.
// standardize([7, 8]) == [-1, 1]
template <typename Container>
auto standardize(Container&& xs)
{
    typedef typename internal::remove_const_and_ref_t<Container>::value_type T;
    T mean(0);
    T stddev(1);
    return normalize_mean_stddev(mean, stddev, std::forward<Container>(xs));
}

// API search type: add_to : a -> (a -> a)
// Provide a function adding to a given constant.
// add_to(3)(2) == 5
template <typename X>
std::function<X(X)> add_to(const X& x)
{
    return [x](X y) -> X
    {
        return x + y;
    };
}

// API search type: subtract_from : a -> (a -> a)
// Provide a function subtracting from a given constant.
// subtract_from(3)(2) == 1
template <typename X>
std::function<X(X)> subtract_from(const X& x)
{
    return [x](X y) -> X
    {
        return x - y;
    };
}

// API search type: subtract : a -> (a -> a)
// Provide a function subtracting a given constant.
// subtract(2)(3) == 1
template <typename X>
std::function<X(X)> subtract(const X& x)
{
    return [x](X y) -> X
    {
        return y - x;
    };
}

// API search type: multiply_with : a -> (a -> a)
// Provide a function multiplying with a given constant.
// multiply_with(3)(2) == 6
template <typename X>
std::function<X(X)> multiply_with(const X& x)
{
    return [x](X y) -> X
    {
        return y * x;
    };
}

// API search type: divide_by : a -> (a -> a)
// Provide a function dividing by a given constant.
// divide_by(2)(6) == 3
template <typename X>
std::function<X(X)> divide_by(const X& x)
{
    return [x](X y) -> X
    {
        return y / x;
    };
}

// API search type: histogram_using_intervals : ([(a, a)], [a]) -> [((a, a), Int)]
// fwd bind count: 1
// Generate a histogram of a sequence with given bins.
// histogram_using_intervals([(0,4), (4,5), (6,8)], [0,1,4,5,6,7,8,9]) ==
//     [((0, 4), 2), ((4, 5), 1), ((6, 8), 2)]
template <typename ContainerIn,
        typename ContainerIntervals,
        typename ContainerOut =
            std::vector<
                std::pair<
                    typename ContainerIntervals::value_type,
                    std::size_t>>,
        typename T = typename ContainerIn::value_type>
ContainerOut histogram_using_intervals(
        const ContainerIntervals& intervals, const ContainerIn& xs)
{
    ContainerOut bins;
    internal::prepare_container(bins, size_of_cont(intervals));
    auto itOut = internal::get_back_inserter(bins);
    for (const auto& interval : intervals)
    {
        *itOut = std::make_pair(interval, 0);
    }
    for (const auto& x : xs)
    {
        for (auto& bin : bins)
        {
            if (x >= bin.first.first && x < bin.first.second)
            {
                ++bin.second;
            }
        }
    }
    return bins;
}

// API search type: generate_consecutive_intervals : (a, a, a) -> [(a, a)]
// fwd bind count: 2
// Return intervals of a given size adjacent to each other
// generate_consecutive_intervals(0, 2, 4) == [(0,2), (2,4), (4,6), (6,8)]
template <typename T>
std::vector<std::pair<T, T>> generate_consecutive_intervals(
        const T& first_lower_bound, const T& step, std::size_t count)
{
    const auto count_as_T = static_cast<T>(count);
    return zip(
        numbers_step<T>(
            first_lower_bound,
            first_lower_bound + count_as_T * step,
            step),
        numbers_step<T>(
            first_lower_bound + step,
            first_lower_bound + step + count_as_T * step,
            step));
}

// API search type: histogram : (a, a, a, [a]) -> [((a, a), Int)]
// fwd bind count: 3
// Calculate the histogram of a sequence using a given bin width.
// histogram(1, 2, 4, [0,1,4,5,7,8,9]) == [(1, 2), (3, 0), (5, 2), (7, 1)]
template <typename ContainerIn,
        typename ContainerOut =
            std::vector<
                std::pair<
                    typename ContainerIn::value_type,
                    std::size_t>>,
        typename T = typename ContainerIn::value_type>
ContainerOut histogram(
        const T& first_center, const T& bin_width, std::size_t count,
        const ContainerIn& xs)
{
    const auto interval_histogram = histogram_using_intervals(
        generate_consecutive_intervals(
            first_center - bin_width / 2,
            bin_width,
            count),
        xs);

    assert(size_of_cont(interval_histogram) == count);

    ContainerOut histo;
    internal::prepare_container(histo, count);
    auto itOut = internal::get_back_inserter(histo);
    for (const auto& bin : interval_histogram)
    {
        const auto current_center = (bin.first.first + bin.first.second) / 2;
        *itOut = std::make_pair(current_center, bin.second);
    }
    return histo;
}

// API search type: modulo_chain : ([Int], Int) -> [Int]
// fwd bind count: 1
// For every factor (value % factor) is pushed into the result,
// and value is divided by this factor for the next iteration.
// Can be useful to convert a time in seconds
// into hours, minutes and seconds and similar calculations.
// modulo_chain([24, 60, 60], 7223) == [0, 2, 0, 23]
template <typename T>
std::vector<T> modulo_chain(const std::vector<T>& factors, T val)
{
    std::vector<T> result;
    result.reserve(factors.size());
    const auto factors_reversed = reverse(factors);
    for (const auto& factor : factors_reversed)
    {
        result.push_back(val % factor);
        val /= factor;
    }
    result.push_back(val);
    return reverse(result);
}

// API search type: line_equation : ((Float, Float), (Float, Float), Float) -> Float
// fwd bind count: 2
// Can be used to interpolate and to extrapolate
// based on two given two-dimensional points (x, y).
// Using slope, return NaN if x_1 == x_2.
// line_equation((0.0, 0.0), (2.0, 1.0), 3.0) == 1.5
// line_equation((-1.0, 1.0), (-2.0, 4.0), 0.0) == -2.0
template <typename T>
T line_equation(const std::pair<T, T>& a, const std::pair<T, T>& b, T x)
{
    static_assert(std::is_floating_point<T>::value, "Please use a floating-point type.");
    const double m = (b.second - a.second) / (b.first - a.first);
    return m * x + a.second - m * a.first;
}

} // namespace fplus

//
// search.hpp
//

// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)



#include <algorithm>

namespace fplus
{

// API search type: find_first_by : ((a -> Bool), [a]) -> Maybe a
// fwd bind count: 1
// Returns the first element fulfilling the predicate.
// find_first_by(is_even, [1, 3, 4, 6, 9]) == Just(4)
// find_first_by(is_even, [1, 3, 5, 7, 9]) == Nothing
template <typename Container, typename UnaryPredicate,
    typename T = typename Container::value_type>
maybe<T> find_first_by(UnaryPredicate pred, const Container& xs)
{
    internal::check_unary_predicate_for_container<UnaryPredicate, Container>();
    auto it = std::find_if(std::begin(xs), std::end(xs), pred);
    if (it == std::end(xs))
        return nothing<T>();
    return just<T>(*it);
}

// API search type: find_last_by : ((a -> Bool), [a]) -> Maybe a
// fwd bind count: 1
// Returns the last element fulfilling the predicate.
// find_last_by(is_even, [1, 3, 4, 6, 9]) == Just(6)
// find_last_by(is_even, [1, 3, 5, 7, 9]) == Nothing
template <typename Container, typename UnaryPredicate,
    typename T = typename Container::value_type>
maybe<T> find_last_by(UnaryPredicate pred, const Container& xs)
{
    internal::check_unary_predicate_for_container<UnaryPredicate, Container>();
    return find_first_by(pred, reverse(xs));
}

// API search type: find_first_idx_by : ((a -> Bool), [a]) -> Maybe Int
// fwd bind count: 1
// Returns the index of the first element fulfilling the predicate.
// find_first_idx_by(is_even, [1, 3, 4, 6, 9]) == Just(2)
// find_first_idx_by(is_even, [1, 3, 5, 7, 9]) == Nothing
template <typename Container, typename UnaryPredicate>
maybe<std::size_t> find_first_idx_by
        (UnaryPredicate pred, const Container& xs)
{
    internal::check_unary_predicate_for_container<UnaryPredicate, Container>();
    auto it = std::find_if(std::begin(xs), std::end(xs), pred);
    if (it == std::end(xs))
        return nothing<std::size_t>();
    return static_cast<std::size_t>(std::distance(std::begin(xs), it));
}

// API search type: find_last_idx_by : ((a -> Bool), [a]) -> Maybe Int
// fwd bind count: 1
// Returns the index of the last element fulfilling the predicate.
// find_last_idx_by(is_even, [1, 3, 4, 6, 9]) == Just(3)
// find_last_idx_by(is_even, [1, 3, 5, 7, 9]) == Nothing
template <typename Container, typename UnaryPredicate>
maybe<std::size_t> find_last_idx_by
        (UnaryPredicate pred, const Container& xs)
{
    internal::check_unary_predicate_for_container<UnaryPredicate, Container>();
    auto calcRevIdx = [&](std::size_t idx)
    {
        return size_of_cont(xs) - (idx + 1);
    };
    return lift_maybe(calcRevIdx, find_first_idx_by(pred, reverse(xs)));
}

// API search type: find_first_idx : (a, [a]) -> Maybe Int
// fwd bind count: 1
// Returns the index of the first element equal to x.
// find_first_idx(4, [1, 3, 4, 4, 9]) == Just(2)
// find_first_idx(4, [1, 3, 5, 7, 9]) == Nothing
template <typename Container>
maybe<std::size_t> find_first_idx
        (const typename Container::value_type& x, const Container& xs)
{
    return find_first_idx_by(is_equal_to(x), xs);
}

// API search type: find_last_idx : (a, [a]) -> Maybe Int
// fwd bind count: 1
// Returns the index of the last element equal to x.
// find_last_idx(4, [1, 3, 4, 4, 9]) == Just(3)
// find_last_idx(4, [1, 3, 5, 7, 9]) == Nothing
template <typename Container>
maybe<std::size_t> find_last_idx
        (const typename Container::value_type& x, const Container& xs)
{
    return find_last_idx_by(is_equal_to(x), xs);
}

// API search type: find_all_idxs_by : ((a -> Bool), [a]) -> [Int]
// fwd bind count: 1
// Returns the indices off all elements fulfilling the predicate.
// find_all_idxs_by(is_even, [1, 3, 4, 6, 9]) == [2, 3]
template <typename ContainerOut = std::vector<std::size_t>,
        typename UnaryPredicate, typename Container>
ContainerOut find_all_idxs_by(UnaryPredicate p, const Container& xs)
{
    internal::check_unary_predicate_for_container<UnaryPredicate, Container>();
    std::size_t idx = 0;
    ContainerOut result;
    auto itOut = internal::get_back_inserter(result);
    for (const auto& x : xs)
    {
        if (internal::invoke(p, x))
            *itOut = idx;
        ++idx;
    }
    return result;
}

// API search type: find_all_idxs_of : (a, [a]) -> [Int]
// fwd bind count: 1
// Returns the indices off all elements equal to x.
// find_all_idxs_of(4, [1, 3, 4, 4, 9]) == [2, 3]
template <typename ContainerOut = std::vector<std::size_t>,
        typename Container,
        typename T = typename Container::value_type>
ContainerOut find_all_idxs_of
        (const T& x, const Container& xs)
{
    return find_all_idxs_by(is_equal_to(x), xs);
}

// API search type: find_all_instances_of_token : ([a], [a]) -> [Int]
// fwd bind count: 1
// Returns the starting indices of all segments matching token.
// find_all_instances_of_token("haha", "oh, hahaha!") == [4, 6]
template <typename ContainerOut =
    std::vector<std::size_t>, typename Container>
ContainerOut find_all_instances_of_token(const Container& token,
        const Container& xs)
{
    if (size_of_cont(token) > size_of_cont(xs))
        return ContainerOut();

    auto itInBegin = std::begin(xs);
    auto itInEnd = itInBegin;
    internal::advance_iterator(itInEnd, size_of_cont(token));
    std::size_t idx = 0;
    ContainerOut result;
    auto outIt = internal::get_back_inserter(result);
    std::size_t last_possible_idx = size_of_cont(xs) - size_of_cont(token);
    auto check_and_push = [&]()
    {
        if (std::equal(itInBegin, itInEnd, std::begin(token)))
        {
            *outIt = idx;
        }
    };
    while (idx != last_possible_idx)
    {
        check_and_push();
        ++itInBegin;
        ++itInEnd;
        ++idx;
    }
    check_and_push();
    return result;
}

// API search type: find_all_instances_of_token_non_overlapping : ([a], [a]) -> [Int]
// fwd bind count: 1
// Returns the starting indices
// of all non-overlapping segments matching token.
// find_all_instances_of_token_non_overlapping("haha", "oh, hahaha!") == [4]
template <typename ContainerOut = std::vector<std::size_t>, typename Container>
ContainerOut find_all_instances_of_token_non_overlapping
        (const Container& token, const Container& xs)
{
    auto overlapping_instances = find_all_instances_of_token<ContainerOut>(
            token, xs);
    ContainerOut result;
    auto outIt = internal::get_back_inserter(result);
    std::size_t token_size = size_of_cont(token);
    for (const auto idx : overlapping_instances)
    {
        if (result.empty() || result.back() + token_size <= idx)
        {
            *outIt = idx;
        }
    }
    return result;
}

// API search type: find_first_instance_of_token : ([a], [a]) -> Maybe Int
// fwd bind count: 1
// Returns the index of the first segment matching token.
// find_first_instance_of_token("haha", "oh, hahaha!") == just 4
template <typename Container>
maybe<std::size_t> find_first_instance_of_token
        (const Container& token, const Container& xs)
{
    if (size_of_cont(token) > size_of_cont(xs))
        return nothing<std::size_t>();

    auto itInBegin = std::begin(xs);
    auto itInEnd = itInBegin;
    internal::advance_iterator(itInEnd, size_of_cont(token));
    std::size_t idx = 0;
    std::size_t last_possible_idx = size_of_cont(xs) - size_of_cont(token);
    while (idx != last_possible_idx)
    {
        if (std::equal(itInBegin, itInEnd, std::begin(token)))
        {
            return just(idx);
        }
        ++itInBegin;
        ++itInEnd;
        ++idx;
    }
    if (std::equal(itInBegin, itInEnd, std::begin(token)))
    {
        return just(idx);
    }
    return nothing<std::size_t>();
}

} // namespace fplus

//
// sets.hpp
//

// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)



#include <algorithm>
#include <set>
#include <unordered_set>

namespace fplus
{

// API search type: set_includes : (Set a, Set a) -> Bool
// fwd bind count: 1
// Checks if every element of the second set is also present in the first set.
// Also known as is_subset_of.
template <typename SetType>
bool set_includes(const SetType& set1, const SetType& set2)
{
    return std::includes(std::begin(set1), std::end(set1),
        std::begin(set2), std::end(set2));
}

// API search type: unordered_set_includes : (Unordered_Set a, Unordered_Set a) -> Bool
// fwd bind count: 1
// Checks if every element of the second unordered_set
// is also present in the first unordered_set.
// Also known as is_subset_of.
template <typename UnorderSetType>
bool unordered_set_includes(const UnorderSetType& set1,
                            const UnorderSetType& set2)
{
    auto first_not_included = std::find_if(set2.begin(), set2.end(),
        [&](const typename UnorderSetType::value_type& x)
        -> bool { return set1.find(x) == set1.end();});
    return first_not_included == set2.end();
}

// API search type: set_merge : (Set a, Set a) -> Set a
// fwd bind count: 1
// Returns the union of two given sets.
template <typename SetType>
SetType set_merge(const SetType& set1, const SetType& set2)
{
    SetType result;
    auto itOut = internal::get_back_inserter(result);
    std::merge(std::begin(set1), std::end(set1),
        std::begin(set2), std::end(set2),
        itOut);
    return result;
}

// API search type: unordered_set_merge : (Unordered_Set a, Unordered_Set a) -> Unordered_Set a
// fwd bind count: 1
// Returns the union of two given sets.
template <typename UnorderSetType>
UnorderSetType unordered_set_merge(const UnorderSetType& set1, const UnorderSetType& set2)
{
    UnorderSetType result;
    auto itOut = internal::get_back_inserter(result);
    std::copy(std::begin(set1), std::end(set1), itOut);
    std::copy(std::begin(set2), std::end(set2), itOut);
    return result;
}

// API search type: set_intersection : (Set a, Set a) -> Set a
// fwd bind count: 1
// Returns the intersection of both sets.
template <typename SetType>
SetType set_intersection(const SetType& set1, const SetType& set2)
{
    SetType result;
    auto itOut = internal::get_back_inserter(result);
    std::set_intersection(std::begin(set1), std::end(set1),
        std::begin(set2), std::end(set2),
        itOut);
    return result;
}

// API search type: unordered_set_intersection : (Unordered_Set a, Unordered_Set a) -> Unordered_Set a
// fwd bind count: 1
// Returns the intersection of both unordered_sets.
template <typename UnorderSetType>
UnorderSetType unordered_set_intersection(
    const UnorderSetType& set1, const UnorderSetType& set2)
{
    UnorderSetType result;
    auto itOut = internal::get_back_inserter(result);
    std::copy_if(std::begin(set2), std::end(set2),
        itOut, [&](const typename UnorderSetType::value_type &x)
        -> bool {return set1.find(x) != set1.end();});
    return result;
}

// API search type: set_is_disjoint : (Set a, Set a) -> Bool
// fwd bind count: 1
// Checks if two sets are disjoint.
template <typename SetType>
bool set_is_disjoint(const SetType& set1, const SetType& set2)
{
    return set_intersection(set1, set2).empty();
}

// API search type: unordered_set_is_disjoint : (Unordered_Set a, Unordered_Set a) -> Bool
// fwd bind count: 1
// Checks if two unordered_sets are disjoint.
template <typename UnorderSetType>
bool unordered_set_is_disjoint(
    const UnorderSetType& set1, const UnorderSetType& set2)
{
    return unordered_set_intersection(set1, set2).empty();
}

// API search type: set_difference : (Set a, Set a) -> Set a
// fwd bind count: 1
// Returns the elements in set1 that are not present in set2.
template <typename SetType>
SetType set_difference(const SetType& set1, const SetType& set2)
{
    SetType result;
    auto itOut = internal::get_back_inserter(result);
    std::set_difference(std::begin(set1), std::end(set1),
        std::begin(set2), std::end(set2),
        itOut);
    return result;
}

// API search type: unordered_set_difference : (Unordered_Set a, Unordered_Set a) -> Unordered_Set a
// fwd bind count: 1
// Returns the elements in unordered_set1
// that are not present in unordered_set2.
template <typename UnorderSetType>
UnorderSetType unordered_set_difference(const UnorderSetType& set1,
const UnorderSetType& set2)
{
    UnorderSetType result;
    auto itOut = internal::get_back_inserter(result);
    std::copy_if(std::begin(set1), std::end(set1),
        itOut, [&](const typename UnorderSetType::value_type &x)
        -> bool {return set2.find(x) == set2.end();});
    return result;
}

// API search type: set_symmetric_difference : (Set a, Set a) -> Set a
// fwd bind count: 1
// Returns the symmetric difference of both sets.
template <typename SetType>
SetType set_symmetric_difference(const SetType& set1, const SetType& set2)
{
    SetType result;
    auto itOut = internal::get_back_inserter(result);
    std::set_symmetric_difference(std::begin(set1), std::end(set1),
        std::begin(set2), std::end(set2),
        itOut);
    return result;
}

// API search type: unordered_set_symmetric_difference : (Unordered_Set a, Unordered_Set a) -> Unordered_Set a
// fwd bind count: 1
// Returns the symmetric difference of both unordered_sets.
template <typename UnorderSetType>
UnorderSetType unordered_set_symmetric_difference(
    const UnorderSetType& set1, const UnorderSetType& set2)
{
    UnorderSetType result;
    auto itOut = internal::get_back_inserter(result);
    std::copy_if(std::begin(set1), std::end(set1),
        itOut, [&](const typename UnorderSetType::value_type &x)
        -> bool {return set2.find(x) == set2.end();});
    std::copy_if(std::begin(set2), std::end(set2),
        itOut, [&](const typename UnorderSetType::value_type &x)
        -> bool {return set1.find(x) == set1.end();});
    return result;
}

// API search type: sets_intersection : [Set a] -> Set a
// fwd bind count: 0
// Returns the intersection of the given sets.
// Also known as intersect_many.
// For performance try to sort the inputs sets prior, ascendending by size.
template <typename ContainerIn,
    typename SetType = typename ContainerIn::value_type>
SetType sets_intersection(const ContainerIn& sets)
{
    return fold_left_1(set_intersection<SetType>, sets);
}

// API search type: unordered_sets_intersection : [Unordered_Set a] -> Unordered_Set a
// fwd bind count: 0
// Returns the intersection of the given unordered_sets.
// Also known as intersect_many.
template <typename ContainerIn,
    typename UnordSetType = typename ContainerIn::value_type>
UnordSetType unordered_sets_intersection(const ContainerIn& sets)
{
    return fold_left_1(unordered_set_intersection<UnordSetType>, sets);
}

} // namespace fplus


#include <algorithm>
#include <numeric>
#include <type_traits>

namespace fplus
{

// API search type: any_by : ((a -> Bool), [a]) -> Bool
// fwd bind count: 1
// Check if all elements in a container fulfill a predicate.
// any_by(is_odd, [2, 4, 6]) == false
template <typename UnaryPredicate, typename Container>
bool any_by(UnaryPredicate p, const Container& xs)
{
    internal::check_unary_predicate_for_container<UnaryPredicate, Container>();
    return std::any_of(std::begin(xs), std::end(xs), p);
}

// API search type: any : [Bool] -> Bool
// fwd bind count: 0
// Checks if all elements in a container are true.
// any([false, true, false]) == true
template <typename Container>
bool any(const Container& xs)
{
    typedef typename Container::value_type T;
    return any_by(identity<T>, xs);
}

// API search type: none_by : ((a -> Bool), [a]) -> Bool
// fwd bind count: 1
// Check no element in a container fulfills a predicate.
// none_by(is_even, [3, 4, 5]) == false
template <typename UnaryPredicate, typename Container>
bool none_by(UnaryPredicate p, const Container& xs)
{
    internal::check_unary_predicate_for_container<UnaryPredicate, Container>();
    return std::none_of(std::begin(xs), std::end(xs), p);
}

// API search type: none : [Bool] -> Bool
// fwd bind count: 0
// Checks if all elements in a container are false.
// none([false, true, false]) == false
template <typename Container>
bool none(const Container& xs)
{
    typedef typename Container::value_type T;
    return none_by(identity<T>, xs);
}


// API search type: minimum_idx_by : (((a, a) -> Bool), [a]) -> Int
// fwd bind count: 1
// Return the index of the first minimum element using a less comparator.
// minimum_idx_by(lessLength, ["123", "12", "1234", "123"]) -> "1"
// Unsafe! Crashes on an empty sequence.
template <typename Compare, typename Container>
typename std::size_t minimum_idx_by(Compare comp,
        const Container& xs)
{
    internal::check_compare_for_container<Compare, Container>();
    assert(is_not_empty(xs));
    return static_cast<std::size_t>(std::distance(std::begin(xs),
        std::min_element(std::begin(xs), std::end(xs), comp)));
}

// API search type: minimum_idx_by_maybe : (((a, a) -> Bool), [a]) -> Int
// fwd bind count: 1
// Return the index of the first minimum element using a less comparator
// if sequence is not empty.
// minimum_idx_by_maybe(lessLength, ["123", "12", "1234", "123"]) -> Just "1"
// minimum_idx_by_maybe(lessLength, []) -> Nothing
template <typename Compare, typename Container>
maybe<typename std::size_t> minimum_idx_by_maybe(Compare comp,
        const Container& xs)
{
    if (is_empty(xs))
        return {};
    else
        return minimum_idx_by(comp, xs);
}

// API search type: maximum_idx_by : (((a, a) -> Bool), [a]) -> Int
// fwd bind count: 1
// Return the index of the first maximum element using a less comparator.
// maximum_idx_by(lessLength, ["123", "12", "1234", "123"]) == "2"
// Unsafe! Crashes on an empty sequence.
template <typename Compare, typename Container>
typename std::size_t maximum_idx_by(Compare comp,
        const Container& xs)
{
    internal::check_compare_for_container<Compare, Container>();
    assert(is_not_empty(xs));
    return static_cast<std::size_t>(std::distance(std::begin(xs),
        std::max_element(std::begin(xs), std::end(xs), comp)));
}

// API search type: maximum_idx_by_maybe : (((a, a) -> Bool), [a]) -> Maybe Int
// fwd bind count: 1
// Return the index of the first maximum element using a less comparator
// if sequence is not empty.
// maximum_idx_by_maybe(lessLength, ["123", "12", "1234", "123"]) == Just "2"
// maximum_idx_by_maybe(lessLength, []) == Nothing
template <typename Compare, typename Container>
maybe<typename std::size_t> maximum_idx_by_maybe(Compare comp,
        const Container& xs)
{
    if (is_empty(xs))
        return {};
    else
        return maximum_idx_by(comp, xs);
}


// API search type: minimum_idx : [a] -> Int
// fwd bind count: 0
// Return the index of the first minimum element.
// minimum_idx([3, 1, 4, 2]) == 1
// Unsafe! Crashes on an empty sequence.
template <typename Container>
typename std::size_t minimum_idx(const Container& xs)
{
    return minimum_idx_by(is_less<typename Container::value_type>, xs);
}

// API search type: minimum_idx_maybe : [a] -> Maybe Int
// fwd bind count: 0
// Return the index of the first minimum element if sequence is not empty.
// minimum_idx_maybe([3, 1, 4, 2]) == Just 1
// minimum_idx_maybe([]) == Nothing
template <typename Container>
maybe<typename std::size_t> minimum_idx_maybe(const Container& xs)
{
    if (is_empty(xs))
        return {};
    else
        return minimum_idx(xs);
}

// API search type: maximum_idx : [a] -> Int
// fwd bind count: 0
// Return the index of the first maximum element.
// maximum_idx([3, 1, 4, 2]) == 2
// Unsafe! Crashes on an empty sequence.
template <typename Container>
typename std::size_t maximum_idx(const Container& xs)
{
    return maximum_idx_by(is_less<typename Container::value_type>, xs);
}

// API search type: maximum_idx_maybe : [a] -> Maybe Int
// fwd bind count: 0
// Return the index of the first maximum element if sequence is not empty.
// maximum_idx_maybe([3, 1, 4, 2]) == Just 2
// maximum_imaximum_idx_maybedx([]) == Nothing
template <typename Container>
maybe<typename std::size_t> maximum_idx_maybe(const Container& xs)
{
    if (is_empty(xs))
        return {};
    else
        return maximum_idx(xs);
}


// API search type: minimum_idx_on : ((a -> b), [a]) -> Int
// fwd bind count: 1
// Return the index of the first minimum element using a transformer.
// minimum_idx_on(length, ["123", "12", "1234", "123"]) -> "1"
// Unsafe! Crashes on an empty sequence.
template <typename F, typename Container>
std::size_t minimum_idx_on(F f, const Container& xs)
{
    using Result = internal::invoke_result_t<F, typename Container::value_type>;
    auto transformed = transform_convert<std::vector<std::decay_t<Result>>>(f, xs);
    return minimum_idx(transformed);
}

// API search type: minimum_idx_on_maybe : ((a -> b), [a]) -> Just Int
// fwd bind count: 1
// Return the index of the first minimum element using a transformer
// if sequence is not empty.
// minimum_idx_on_maybe(length, ["123", "12", "1234", "123"]) -> Just "1"
// minimum_idx_on_maybe(length, []) -> Nothing"
template <typename F, typename Container>
maybe<typename std::size_t> minimum_idx_on_maybe(F f, const Container& xs)
{
    if (is_empty(xs))
        return {};
    else
        return minimum_idx_on(f, xs);
}

// API search type: maximum_idx_on : ((a -> b), [a]) -> Int
// fwd bind count: 1
// Return the index of the first maximum element using a transformer.
// maximum_idx_on(length, ["123", "12", "1234", "123"]) == "2"
// Unsafe! Crashes on an empty sequence.
template <typename F, typename Container>
std::size_t maximum_idx_on(F f, const Container& xs)
{
    using Result = internal::invoke_result_t<F, typename Container::value_type>;
    auto transformed = transform_convert<std::vector<std::decay_t<Result>>>(f, xs);
    return maximum_idx(transformed);
}

// API search type: maximum_idx_on_maybe : ((a -> b), [a]) -> Maybe Int
// fwd bind count: 1
// Return the index of the first maximum element using a transformer
// if sequence is not empty.
// maximum_idx_on_maybe(length, ["123", "12", "1234", "123"]) == Just "2"
// maximum_idx_on_maybe(length, []) == Nothing
template <typename F, typename Container>
maybe<typename std::size_t> maximum_idx_on_maybe(F f, const Container& xs)
{
    if (is_empty(xs))
        return {};
    else
        return maximum_idx_on(f, xs);
}

// API search type: minimum_by : (((a, a) -> Bool), [a]) -> a
// fwd bind count: 1
// Return the first minimum element using a less comparator.
// minimum_by(lessLength, ["123", "12", "1234", "123"]) -> "12"
// Unsafe! Crashes on an empty sequence.
template <typename Compare, typename Container>
typename Container::value_type minimum_by(Compare comp,
        const Container& xs)
{
    internal::check_compare_for_container<Compare, Container>();
    assert(is_not_empty(xs));
    return *std::min_element(std::begin(xs), std::end(xs), comp);
}

// API search type: minimum_by_maybe : (((a, a) -> Bool), [a]) -> a
// fwd bind count: 1
// Return the first minimum element using a less comparator
// if sequence is not empty.
// minimum_by_maybe(lessLength, ["123", "12", "1234", "123"]) -> Just "12"
// minimum_by_maybe(lessLength, []) -> Nothing
template <typename Compare, typename Container>
maybe<typename Container::value_type> minimum_by_maybe(Compare comp,
        const Container& xs)
{
    if (is_empty(xs))
        return {};
    else
        return minimum_by(comp, xs);
}

// API search type: maximum_by : (((a, a) -> Bool), [a]) -> a
// fwd bind count: 1
// Return the first maximum element using a less comparator.
// maximum_by(lessLength, ["123", "12", "1234", "123"]) == "1234"
// Unsafe! Crashes on an empty sequence.
template <typename Compare, typename Container>
typename Container::value_type maximum_by(Compare comp,
        const Container& xs)
{
    internal::check_compare_for_container<Compare, Container>();
    assert(is_not_empty(xs));
    return *std::max_element(std::begin(xs), std::end(xs), comp);
}

// API search type: maximum_by_maybe : (((a, a) -> Bool), [a]) -> Maybe a
// fwd bind count: 1
// Return the first maximum element using a less comparator
// if sequence is not empty.
// maximum_by_maybe(lessLength, ["123", "12", "1234", "123"]) == Just "1234"
// maximum_by_maybe(lessLength, []) == Nothing
template <typename Compare, typename Container>
maybe<typename Container::value_type> maximum_by_maybe(Compare comp,
        const Container& xs)
{
    if (is_empty(xs))
        return {};
    else
        return maximum_by(comp, xs);
}


// API search type: minimum : [a] -> a
// fwd bind count: 0
// Return the first minimum element.
// minimum([3, 1, 4, 2]) == 1
// Unsafe! Crashes on an empty sequence.
template <typename Container>
typename Container::value_type minimum(const Container& xs)
{
    return minimum_by(is_less<typename Container::value_type>, xs);
}

// API search type: minimum_maybe : [a] -> Maybe a
// fwd bind count: 0
// Return the first minimum element if sequence is not empty
// if sequence is not empty.
// minimum_maybe([3, 1, 4, 2]) == Just 1
// minimum_maybe([]) == Nothing
template <typename Container>
maybe<typename Container::value_type> minimum_maybe(const Container& xs)
{
    if (is_empty(xs))
        return {};
    else
        return minimum(xs);
}

// API search type: maximum : [a] -> a
// fwd bind count: 0
// Return the first maximum element.
// maximum([3, 1, 4, 2]) == 4
// Unsafe! Crashes on an empty sequence.
template <typename Container>
typename Container::value_type maximum(const Container& xs)
{
    return maximum_by(is_less<typename Container::value_type>, xs);
}

// API search type: maximum_maybe : [a] -> Maybe a
// fwd bind count: 0
// Return the first maximum element if sequence is not empty
// if sequence is not empty.
// maximum_maybe([3, 1, 4, 2]) == Just 4
// maximum_maybe([]) == Nothing
template <typename Container>
maybe<typename Container::value_type> maximum_maybe(const Container& xs)
{
    if (is_empty(xs))
        return {};
    else
        return maximum(xs);
}


// API search type: minimum_on : ((a -> b), [a]) -> a
// fwd bind count: 1
// Return the first minimum element using a transformer.
// minimum_on(length, ["123", "12", "1234", "123"]) -> "12"
// Unsafe! Crashes on an empty sequence.
template <typename F, typename Container>
typename Container::value_type minimum_on(F f, const Container& xs)
{
    internal::trigger_static_asserts<internal::unary_function_tag, F, typename Container::value_type>();
    return elem_at_idx(minimum_idx_on(f, xs), xs);
}

// API search type: minimum_on_maybe : ((a -> b), [a]) -> Maybe a
// fwd bind count: 1
// Return the first minimum element using a transformer
// if sequence is not empty.
// minimum_on_maybe(length, ["123", "12", "1234", "123"]) -> Just "12"
// minimum_on_maybe(length, []) -> Nothing
template <typename F, typename Container>
maybe<typename Container::value_type> minimum_on_maybe(
    F f, const Container& xs)
{
    if (is_empty(xs))
        return {};
    else
        return minimum_on(f, xs);
}

// API search type: maximum_on : ((a -> b), [a]) -> a
// fwd bind count: 1
// Return the first maximum element using a transformer.
// maximum_on(length, ["123", "12", "1234", "123"]) == "1234"
// Unsafe! Crashes on an empty sequence.
template <typename F, typename Container>
typename Container::value_type maximum_on(F f, const Container& xs)
{
    internal::trigger_static_asserts<internal::unary_function_tag, F, typename Container::value_type>();
    return elem_at_idx(maximum_idx_on(f, xs), xs);
}

// API search type: maximum_on_maybe : ((a -> b), [a]) -> Maybe a
// fwd bind count: 1
// Return the first maximum element using a transformer
// if sequence is not empty.
// maximum_on_maybe(length, ["123", "12", "1234", "123"]) == Just "1234"
// maximum_on_maybe(length, ["123", "12", "1234", "123"]) == Nothing
template <typename F, typename Container>
maybe<typename Container::value_type> maximum_on_maybe(
    F f, const Container& xs)
{
    if (is_empty(xs))
        return {};
    else
        return maximum_on(f, xs);
}

// API search type: mean : [a] -> a
// fwd bind count: 0
// mean([1, 4, 4]) == 3
// Also known as average.
// xs must have at least one element.
// Use mean_using_doubles if overflow errors for sum(xs) can occur.
// Unsafe! Crashes on an empty sequence.
template <typename Result, typename Container>
Result mean(const Container& xs)
{
    assert(size_of_cont(xs) != 0);
    typedef typename Container::value_type T;
    return static_cast<Result>(sum(xs) / static_cast<T>(size_of_cont(xs)));
}

// API search type: mean_obj_div_size_t : [a] -> a
// fwd bind count: 0
// mean_obj_div_size_t([B 1, B 4, B 4]) == B 3
// The provided objects must support division by a std::size_t.
// Unsafe! Crashes on an empty sequence.
// Also Make sure sum(xs) does not overflow.
template <typename Container,
    typename T = typename Container::value_type>
T mean_obj_div_size_t(const Container& xs)
{
    assert(size_of_cont(xs) != 0);
    return sum(xs) / size_of_cont(xs);
}

// API search type: mean_obj_div_double : [a] -> a
// fwd bind count: 0
// mean_obj_div_double([B 1, B 4, B 4]) == B 3
// The provided objects must support division by a double.
// Unsafe! Crashes on an empty sequence.
// Also Make sure sum(xs) does not overflow.
template <typename Container,
    typename T = typename Container::value_type>
T mean_obj_div_double(const Container& xs)
{
    assert(size_of_cont(xs) != 0);
    return sum(xs) / static_cast<double>(size_of_cont(xs));
}

// API search type: mean_using_doubles : [a] -> a
// fwd bind count: 0
// mean_using_doubles([1, 4, 4]) == 3
// Converts elements to double before calculating the sum
// to prevent overflows.
// Unsafe! Crashes on an empty sequence.
template <typename Result, typename Container>
Result mean_using_doubles(const Container& xs)
{
    assert(size_of_cont(xs) != 0);
    auto xs_as_doubles = convert_elems<double>(xs);
    auto result_as_double = mean<double>(xs_as_doubles);
    if (!std::is_integral<Result>::value)
        return static_cast<Result>(result_as_double);
    else
        return round<double, Result>(result_as_double);
}

// API search type: median : [a] -> a
// fwd bind count: 0
// median([5, 6, 4, 3, 2, 6, 7, 9, 3]) == 5
// Unsafe! Crashes on an empty sequence.
template <typename Container,
        typename Result = typename Container::value_type>
Result median(const Container& xs)
{
    assert(is_not_empty(xs));

    if (size_of_cont(xs) == 1)
        return static_cast<Result>(xs.front());

    // std::nth_element (instead of sorting)
    // would be faster for random-access containers
    // but not work at all on other containers like std::list.
    auto xsSorted = sort(xs);
    if (size_of_cont(xsSorted) % 2 == 1)
    {
        auto it = std::begin(xsSorted);
        internal::advance_iterator(it, size_of_cont(xsSorted) / 2);
        return static_cast<Result>(*it);
    }
    else
    {
        auto it1 = std::begin(xsSorted);
        internal::advance_iterator(it1, size_of_cont(xsSorted) / 2 - 1);
        auto it2 = it1;
        ++it2;
        return static_cast<Result>(*it1 + *it2) / static_cast<Result>(2);
    }
}

// API search type: all_unique_by_less : (((a, a) -> Bool), [a]) -> Bool
// fwd bind count: 1
// Returns true for empty containers.
// O(n*log(n))
template <typename Container, typename Compare>
bool all_unique_by_less(Compare comp, const Container& xs)
{
    internal::check_compare_for_container<Compare, Container>();
    if (size_of_cont(xs) < 2)
        return true;
    return size_of_cont(unique(sort_by(comp, xs))) == size_of_cont(xs);
}

// API search type: all_unique_less : [a] -> Bool
// fwd bind count: 0
// Returns true for empty containers.
// O(n*log(n))
template <typename Container>
bool all_unique_less(const Container& xs)
{
    typedef typename Container::value_type T;
    auto comp = std::less<T>();
    return all_unique_by_less(comp, xs);
}

// API search type: is_infix_of : ([a], [a]) -> Bool
// fwd bind count: 1
// is_infix_of("ion", "FunctionalPlus") == true
template <typename Container>
bool is_infix_of(const Container& token, const Container& xs)
{
    return is_just(find_first_instance_of_token(token, xs));
}

// API search type: is_subsequence_of : ([a], [a]) -> Bool
// fwd bind count: 1
// is_subsequence_of("Final", "FunctionalPlus") == true
template <typename Container>
bool is_subsequence_of(const Container& seq, const Container& xs)
{
    if (is_empty(seq))
        return true;
    if (size_of_cont(seq) > size_of_cont(xs))
        return false;
    typedef typename Container::value_type T;
    auto remaining = convert_container_and_elems<std::list<T>>(seq);
    for (const auto& x : xs)
    {
        if (x == remaining.front())
        {
            remaining.pop_front();
            if (is_empty(remaining))
                return true;
        }
    }
    return false;
}

// API search type: count_if : ((a -> Bool), [a]) -> Int
// fwd bind count: 1
// count_if(is_even, [1, 2, 3, 5, 7, 8]) == 2
template <typename UnaryPredicate, typename Container>
std::size_t count_if(UnaryPredicate p, const Container& xs)
{
    internal::check_unary_predicate_for_container<UnaryPredicate, Container>();
    return size_of_cont(find_all_idxs_by(p, xs));
}

// API search type: count : (a, [a]) -> Int
// fwd bind count: 1
// count(2, [1, 2, 3, 5, 7, 2, 2]) == 3
template <typename Container>
std::size_t count
        (const typename Container::value_type& x, const Container& xs)
{
    return size_of_cont(find_all_idxs_of(x, xs));
}

// API search type: is_unique_in_by : ((a -> bool), [a]) -> Bool
// fwd bind count: 1
// is_unique_in_by((==2), [1, 2, 3, 5, 7, 2, 2]) == false
// is_unique_in_by((==5), [1, 2, 3, 5, 7, 2, 2]) == true
template <typename UnaryPredicate, typename Container>
bool is_unique_in_by
        (UnaryPredicate pred, const Container& xs)
{
    std::size_t count = 0;
    for (const auto& x : xs)
    {
        if (internal::invoke(pred, x))
        {
            ++count;
            if (count > 1)
            {
                return false;
            }
        }
    }
    return true;
}

// API search type: is_unique_in : (a, [a]) -> Bool
// fwd bind count: 1
// is_unique_in(2, [1, 2, 3, 5, 7, 2, 2]) == false
// is_unique_in(5, [1, 2, 3, 5, 7, 2, 2]) == true
template <typename Container>
bool is_unique_in
        (const typename Container::value_type& x, const Container& xs)
{
    return is_unique_in_by(is_equal_to(x), xs);
}

// API search type: is_permutation_of : ([a], [a]) -> Bool
// fwd bind count: 1
// Checks if one container is a permuation of the other one.
// is_permutation_of([2,3,1], [1,2,3]) == true
// O(log(n))
template <typename Container>
bool is_permutation_of(const Container& xs, const Container& ys)
{
    return size_of_cont(xs) == size_of_cont(ys) &&
        sort(xs) == sort(ys);
}

// API search type: fill_pigeonholes_to : (Int, [Int]) -> [Int]
// fwd bind count: 1
// Returns a list containing the count for every element in xs
// with the value corresponding to the index in the result list.
// fill_pigeonholes_to(5, [0,1,3,1]) == [1,2,0,1,0]
// fill_pigeonholes_to(3, [0,1,3,1]) == [1,2,0]
template <typename ContainerIn,
    typename ContainerOut = std::vector<std::size_t>,
    typename T = typename ContainerIn::value_type>
ContainerOut fill_pigeonholes_to(std::size_t idx_end, const ContainerIn& xs)
{
    static_assert(std::is_integral<T>::value,
        "Type must be integral.");

    if (is_empty(xs))
        return {};

    ContainerOut result(idx_end, 0);
    for (const auto& x : xs)
    {
        if (x >= 0)
        {
            const auto idx = static_cast<std::size_t>(x);
            if (idx < result.size())
            {
                ++result[idx];
            }
        }
    }
    return result;
}

// API search type: fill_pigeonholes : [Int] -> [Int]
// fwd bind count: 0
// Returns a list containing the count for every element in xs
// with the value corresponding to the index in the result list.
// fill_pigeonholes([0,1,3,1]) == [1,2,0,1]
template <typename ContainerIn,
    typename ContainerOut = std::vector<std::size_t>,
    typename T = typename ContainerIn::value_type>
ContainerOut fill_pigeonholes(const ContainerIn& xs)
{
    static_assert(std::is_integral<T>::value,
        "Type must be integral.");

    if (is_empty(xs))
        return {};

    return(fill_pigeonholes_to<ContainerIn, ContainerOut>(
        maximum(xs) + 1, xs));
}

// API search type: fill_pigeonholes_bool_to : (Int, [Int]) -> [Int]
// fwd bind count: 1
// Returns a list telling if the element corresponding to the index
// is present in xs.
// fill_pigeonholes_bool_to(5, [0,1,3,1]) == [1,1,0,1,0]
// fill_pigeonholes_bool_to(3, [0,1,3,1]) == [1,1,0]
template <typename ContainerIn,
    typename ContainerOut = std::vector<std::uint8_t>,
    typename T = typename ContainerIn::value_type>
ContainerOut fill_pigeonholes_bool_to(std::size_t idx_end, const ContainerIn& xs)
{
    static_assert(std::is_integral<T>::value,
        "Type must be integral.");

    if (is_empty(xs))
        return {};

    ContainerOut result(idx_end, 0);
    for (const auto& x : xs)
    {
        if (x >= 0)
        {
            const auto idx = static_cast<std::size_t>(x);
            if (idx < result.size())
            {
                result[idx] = 1;
            }
        }
    }
    return result;
}

// API search type: fill_pigeonholes_bool : [Int] -> [Int]
// fwd bind count: 0
// Returns a list telling if the element corresponding to the index
// is present in xs.
// fill_pigeonholes_bool([0,1,3,1]) == [1,2,0,1]
template <typename ContainerIn,
    typename ContainerOut = std::vector<std::uint8_t>,
    typename T = typename ContainerIn::value_type>
ContainerOut fill_pigeonholes_bool(const ContainerIn& xs)
{
    static_assert(std::is_integral<T>::value,
        "Type must be integral.");

    if (is_empty(xs))
        return {};

    return(fill_pigeonholes_bool_to<ContainerIn, ContainerOut>(
        maximum(xs) + 1, xs));
}

// API search type: present_in_all : [[a]] -> [a]
// fwd bind count: 0
// Returns a list containing only the elements present in all sublists of xs.
// Also known as gemstones.
// present_in_all([[4,1,2], [5,2,1], [2,4,1]]) == [1,2]
template <typename ContainerIn,
    typename SubContainerIn = typename ContainerIn::value_type,
    typename T = typename SubContainerIn::value_type,
    typename ContainerOut = std::vector<T>>
ContainerOut present_in_all(const ContainerIn& xs)
{
    return convert_container<ContainerOut>(
        fplus::sets_intersection(
            transform(
                convert_container<std::set<T>, SubContainerIn>,
                xs)));
}

} // namespace fplus

//
// extrapolate.hpp
//

// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)



namespace fplus
{

// API search type: elem_at_idx_or_nothing : (Int, [a]) -> Maybe a
// fwd bind count: 1
// Return nth element of a sequence.
// Returns nothing if index is outside of xs.
template <typename Container,
    typename T = typename Container::value_type>
maybe<T> elem_at_idx_or_nothing(signed int idx, const Container& xs)
{
    if (idx < 0 || idx >= static_cast<signed int>(size_of_cont(xs)))
    {
        return {};
    }
    auto it = std::begin(xs);
    internal::advance_iterator(it, static_cast<std::size_t>(idx));
    return *it;
}

// API search type: elem_at_idx_or_constant : (a, Int, [a]) -> a
// fwd bind count: 2
// Return nth element of a sequence.
// Interpolate outside of sequence with a constant value.
// iiiiii|abcdefgh|iiiiiii
template <typename Container,
    typename T = typename Container::value_type>
T elem_at_idx_or_constant(const T& c, signed int idx, const Container& xs)
{
    if (idx < 0 || idx >= static_cast<signed int>(size_of_cont(xs)))
    {
        return c;
    }
    auto it = std::begin(xs);
    internal::advance_iterator(it, static_cast<std::size_t>(idx));
    return *it;
}

// API search type: elem_at_idx_or_replicate : (Int, [a]) -> a
// fwd bind count: 1
// Return nth element of a sequence.
// Interpolate outside of sequence by replicating the nearest inside value.
// aaaaaa|abcdefgh|hhhhhhh
// xs must be non-empty.
template <typename Container,
    typename T = typename Container::value_type>
T elem_at_idx_or_replicate(signed int idx, const Container& xs)
{
    assert(is_not_empty(xs));
    if (idx < 0)
    {
        return xs.front();
    }
    if (idx >= static_cast<signed int>(size_of_cont(xs)))
    {
        return xs.back();
    }
    auto it = std::begin(xs);
    internal::advance_iterator(it, static_cast<std::size_t>(idx));
    return *it;
}

// API search type: elem_at_idx_or_wrap : (Int, [a]) -> a
// fwd bind count: 1
// Return nth element of a sequence.
// Interpolate outside of sequence by replicating the sequence.
// For cyclic element access.
// cdefgh|abcdefgh|abcdefg
// xs must be non-empty.
template <typename Container,
    typename T = typename Container::value_type>
T elem_at_idx_or_wrap(signed int idx, const Container& xs)
{
    assert(is_not_empty(xs));
    const signed int cont_size = static_cast<signed int>(size_of_cont(xs));
    if (idx < 0)
        idx = cont_size - (std::abs(idx) % cont_size);
    else
        idx = idx % cont_size;
    auto it = std::begin(xs);
    internal::advance_iterator(it, static_cast<std::size_t>(idx));
    return *it;
}

// API search type: extrapolate_replicate : (Int, Int, [a]) -> [a]
// fwd bind count: 2
// Extrapolate a sequence by replicating the border values.
// count_begin determines the number of elements to be prepended.
// count_end determines the number of elements to be appended.
// aaaaaa|abcdefgh|hhhhhhh
// xs must be non-empty.
template <typename Container,
    typename T = typename Container::value_type>
Container extrapolate_replicate(std::size_t count_begin, std::size_t count_end,
        const Container& xs)
{
    assert(is_not_empty(xs));
    Container ys;
    const auto xs_size = size_of_cont(xs);
    internal::prepare_container(ys, xs_size + count_begin + count_end);
    auto it = internal::get_back_inserter<Container>(ys);
    const signed int idx_end = static_cast<signed int>(xs_size + count_end);
    const signed int idx_start = -static_cast<signed int>(count_begin);
    for (signed int idx = idx_start; idx < idx_end; ++idx)
    {
        *it = elem_at_idx_or_replicate(idx, xs);
    }
    return ys;
}

// API search type: extrapolate_wrap : (Int, Int, [a]) -> [a]
// fwd bind count: 2
// Extrapolate a sequence by accessing the elements in cyclic fashion.
// count_begin determines the number of elements to be prepended.
// count_end determines the number of elements to be appended.
// cdefgh|abcdefgh|abcdefg
// xs must be non-empty.
template <typename Container,
    typename T = typename Container::value_type>
Container extrapolate_wrap(std::size_t count_begin, std::size_t count_end,
        const Container& xs)
{
    assert(is_not_empty(xs));
    Container ys;
    const auto xs_size = size_of_cont(xs);
    internal::prepare_container(ys, xs_size + count_begin + count_end);
    auto it = internal::get_back_inserter<Container>(ys);
    const signed int idx_end = static_cast<signed int>(xs_size + count_end);
    const signed int idx_start = -static_cast<signed int>(count_begin);
    for (signed int idx = idx_start; idx < idx_end; ++idx)
    {
        *it = elem_at_idx_or_wrap(idx, xs);
    }
    return ys;
}

} // namespace fplus

//
// interpolate.hpp
//

// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)



#include <cmath>

namespace fplus
{

// API search type: elem_at_float_idx : (Float, [a]) -> a
// fwd bind count: 1
// Interpolates linearly between elements.
// xs must be non-empty.
template <typename Container,
    typename T = typename Container::value_type>
T elem_at_float_idx(double idx, const Container& xs)
{
    assert(is_not_empty(xs));
    if (idx <= 0.0)
    {
        return xs.front();
    }
    std::size_t idx_floor = static_cast<std::size_t>(floor(idx));
    std::size_t idx_ceil = static_cast<std::size_t>(ceil(idx));
    if (idx_ceil >= size_of_cont(xs))
    {
        return xs.back();
    }
    double idx_floor_float = static_cast<double>(idx_floor);
    double idx_ceil_float = static_cast<double>(idx_ceil);
    double weight_floor = idx_ceil_float - idx;
    double weight_ceil = idx - idx_floor_float;
    return
        (weight_floor * elem_at_idx(idx_floor, xs) +
            weight_ceil * elem_at_idx(idx_ceil, xs));
}

} // namespace fplus

//
// maps.hpp
//

// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)




#include <map>
#include <unordered_map>

namespace fplus
{

// API search type: pairs_to_map : [(key, val)] -> Map key val
// fwd bind count: 0
// Converts a Container of pairs (key, value) into a dictionary.
template <typename MapOut, typename ContainerIn>
MapOut pairs_to_map(const ContainerIn& pairs)
{
    return convert_container_and_elems<MapOut>(pairs);
}

// API search type: pairs_to_map_grouped : [(key, val)] -> Map key [val]
// fwd bind count: 0
// Convert a list of key-value pairs to a dictionary
// while pushing values having the same key into a vector.
// pairs_to_map_grouped([("a", 1), ("a", 2), ("b", 6), ("a", 4)])
//     -> {"a": [1, 2, 4], "b": [6]}
template <typename ContainerIn,
    typename Key = typename ContainerIn::value_type::first_type,
    typename SingleValue = typename ContainerIn::value_type::second_type,
    typename MapOut = std::map<Key, std::vector<SingleValue>>>
MapOut pairs_to_map_grouped(const ContainerIn& pairs)
{
    MapOut result;
    for (const auto& p : pairs)
    {
        result[p.first].push_back(p.second);
    }
    return result;
}

// API search type: pairs_to_unordered_map_grouped : [(key, val)] -> Map key [val]
// fwd bind count: 0
// Convert a list of key-value pairs to a dictionary
// while pushing values having the same key into a vector.
// pairs_to_unordered_map_grouped([("a", 1), ("a", 2), ("b", 6), ("a", 4)])
//     -> {"a": [1, 2, 4], "b": [6]}
template <typename ContainerIn,
    typename Key = typename ContainerIn::value_type::first_type,
    typename SingleValue = typename ContainerIn::value_type::second_type,
    typename MapOut = std::unordered_map<Key, std::vector<SingleValue>>>
MapOut pairs_to_unordered_map_grouped(const ContainerIn& pairs)
{
    MapOut result;
    for (const auto& p : pairs)
    {
        result[p.first].push_back(p.second);
    }
    return result;
}

// API search type: map_to_pairs : Map key val -> [(key, val)]
// fwd bind count: 0
// Converts a dictionary into a Container of pairs (key, value).
template <typename MapType,
    typename MapPair = typename MapType::value_type,
    typename Key = typename std::remove_const<typename MapPair::first_type>::type,
    typename Val = typename std::remove_const<typename MapPair::second_type>::type,
    typename OutPair = std::pair<Key, Val>,
    typename ContainerOut = std::vector<OutPair>>
ContainerOut map_to_pairs(const MapType& dict)
{
    return convert_container_and_elems<ContainerOut>(dict);
}

// API search type: transform_map_values : ((old_val -> new_val), Map key old_val) -> Map key new_val
// fwd bind count: 1
// Manipulate the values in a dictionary, keeping the key-value relationship.
// transform_map_values((*2), {0: 2, 1: 3}) == {0: 4, 1: 6}
template <typename F, typename MapIn>
auto transform_map_values(F f, const MapIn& map)
{
    using MapInPair = typename MapIn::value_type;
    using Key = std::remove_const_t<typename MapInPair::first_type>;
    using InVal = std::remove_const_t<typename MapInPair::second_type>;
    using OutVal = std::decay_t<internal::invoke_result_t<F, InVal>>;
    using MapOut = typename internal::SameMapTypeNewTypes<MapIn, Key, OutVal>::type;

    return pairs_to_map<MapOut>(
        transform(
            bind_1st_of_2(transform_snd<Key, InVal, F>, f),
            map_to_pairs(map)));
}

// API search type: map_union_with : (((val, val) -> val), Map key val, Map key val) -> Map key val
// fwd bind count: 2
// Combine two dictionaries using a binary function for the values.
// map_union_with((++), {0: a, 1: b}, {0: c, 2: d}) == {0: ac, 1: b, 2: d}
template <typename F, typename MapIn>
auto map_union_with(F f, const MapIn& dict1, const MapIn& dict2)
{
    const auto both = append(map_to_pairs(dict1), map_to_pairs(dict2));
    using Key = typename decltype(both)::value_type::first_type;
    using SingleValue = typename decltype(both)::value_type::second_type;
    auto full_map = pairs_to_map_grouped<decltype(both), Key, SingleValue,
            typename internal::SameMapTypeNewTypes<MapIn, Key, std::vector<SingleValue>>::type>(both);
    const auto group_f = [f](const auto& vals)
    {
        return fold_left_1(f, vals);
    };
    return transform_map_values(group_f, full_map);
}

// API search type: map_union : (Map key val, Map key val) -> Map key val
// fwd bind count: 1
// Combine two dictionaries keeping the value of the first map
// in case of key collisions.
// map_union({0: a, 1: b}, {0: c, 2: d}) == {0: a, 1: b, 2: d}
template <typename MapType>
MapType map_union(const MapType& dict1, const MapType& dict2)
{
    using Value = typename MapType::value_type::second_type;

    const auto get_first = [](const Value& a, const Value&) -> Value
    {
        return a;
    };
    return map_union_with(get_first, dict1, dict2);
}

// API search type: map_grouped_to_pairs : Map key [val] -> [(key, val)]
// fwd bind count: 0
// Convert a dictionary with sequence values to a list of key-value pairs.
// Inverse operation of pairs_to_map_grouped.
// map_grouped_to_pairs({"a": [1, 2, 4], "b": [6]})
//     -> [("a", 1), ("a", 2), ("a", 4), ("b", 6)]
template<typename MapType>
auto map_grouped_to_pairs(const MapType &dict)
{
    using Key = typename MapType::key_type;
    using Group = typename MapType::mapped_type;

    auto fn = [](const auto &pair)
    {
        const auto f = zip_repeat<std::vector<Key>, Group>;
        return apply_to_pair(f, transform_fst(singleton_seq < Key > , pair));
    };
    return concat(transform(fn, map_to_pairs(dict)));
}

// API search type: get_map_keys : Map key val -> [key]
// fwd bind count: 0
// Returns all keys used in a map.
template <typename MapType,
    typename ContainerOut =
        std::vector<typename std::remove_const<typename MapType::key_type>::type>>
ContainerOut get_map_keys(const MapType& dict)
{
    auto pairs = map_to_pairs(dict);
    typedef typename decltype(pairs)::value_type::first_type FirstType;
    typedef typename decltype(pairs)::value_type::second_type SecondType;
    return transform_convert<ContainerOut>(
        fst<FirstType, SecondType>, map_to_pairs(dict));
}

// API search type: get_map_values : Map key val -> [val]
// fwd bind count: 0
// Returns all values present in a map.
template <typename MapType,
    typename ContainerOut =
        std::vector<typename std::remove_const<typename MapType::mapped_type>::type>>
ContainerOut get_map_values(const MapType& dict)
{
    auto pairs = map_to_pairs(dict);
    typedef typename decltype(pairs)::value_type::first_type FirstType;
    typedef typename decltype(pairs)::value_type::second_type SecondType;
    return transform_convert<ContainerOut>(
        snd<FirstType, SecondType>, map_to_pairs(dict));
}

// API search type: swap_keys_and_values : Map a b -> Map b a
// fwd bind count: 0
// Swaps keys and Values of a dict:
// swap_keys_and_values({1: "a", 2: "b"}) == {"a": 1, "b": 2}
template <typename MapIn,
    typename MapInPair = typename MapIn::value_type,
    typename InKey = typename MapInPair::first_type,
    typename InVal = typename MapInPair::second_type,
    typename OutKey = InVal,
    typename OutVal = typename std::remove_const<InKey>::type,
    typename MapOut = typename internal::SameMapTypeNewTypes<MapIn, OutKey, OutVal>::type>
MapOut swap_keys_and_values(const MapIn& dict)
{
    auto inAsPairs = map_to_pairs(dict);
    auto outAsPairs = transform(swap_pair_elems<InKey, InVal>, inAsPairs);
    return pairs_to_map<MapOut>(outAsPairs);
}

// API search type: create_map : ([key], [val]) -> Map key val
// fwd bind count: 1
// Zip a sequence of keys with a sequence of values to obtain a dictionary.
// create_map([1,2,3], ["one", "two"]) == {1: "one", 2: "two"}
template <typename ContainerIn1, typename ContainerIn2,
    typename Key = typename std::remove_const<typename ContainerIn1::value_type>::type,
    typename Val = typename std::remove_const<typename ContainerIn2::value_type>::type,
    typename MapOut = std::map<Key, Val>>
MapOut create_map(const ContainerIn1& keys, const ContainerIn2& values)
{
    auto pairs = zip(keys, values);
    return pairs_to_map<MapOut>(pairs);
}

// API search type: create_map_with : ((key -> val), [key]) -> Map key val
// fwd bind count: 1
// Take a list of keys and create a dictionary
// generating the values by applying f to each key.
// create_map_with(show, [1,2]) == {1: "1", 2: "2"}
template <typename ContainerIn, typename F>
auto create_map_with(F f, const ContainerIn& keys)
{
    return create_map(keys, transform(f, keys));
}

// API search type: create_map_grouped : ((val -> key), [val]) -> Map key val
// fwd bind count: 1
// Take a list of values and create a grouped dictionary
// generating the keys by applying f to each key.
// create_map_grouped(length, ["one", "three", "two"]) == {3: ["one", "two"], 5: ["three"]}
// Also known as group_by
template <typename ContainerIn, typename F>
auto create_map_grouped(F f, const ContainerIn& values)
{
    return pairs_to_map_grouped(zip(transform(f, values), values));
}

// API search type: create_unordered_map : ([key], [val]) -> Map key val
// fwd bind count: 1
// Zip a sequence of keys with a sequence of values to obtain a dictionary.
// create_unordered_map([1,2,3], ["one", "two"]) == {1: "one", 2: "two"}
template <typename ContainerIn1, typename ContainerIn2,
    typename Key = typename std::remove_const<typename ContainerIn1::value_type>::type,
    typename Val = typename std::remove_const<typename ContainerIn2::value_type>::type,
    typename MapOut = std::unordered_map<Key, Val >>
MapOut create_unordered_map(
    const ContainerIn1& keys,
    const ContainerIn2& values)
{
    auto pairs = zip(keys, values);
    return pairs_to_map<MapOut>(pairs);
}

// API search type: create_unordered_map_with : ((key -> val), [key]) -> Map key val
// fwd bind count: 1
// Take a list of keys and create a dictionary
// generating the values by applying f to each key.
// create_unordered_map_with(show, [1,2]) == {1: "1", 2: "2"}
template <typename ContainerIn, typename F>
auto create_unordered_map_with(F f, const ContainerIn& keys)
{
    return create_unordered_map(keys, transform(f, keys));
}

// API search type: create_unordered_map_grouped : ((val -> key), [val]) -> Map key val
// fwd bind count: 1
// Take a list of values and create a grouped dictionary
// generating the keys by applying f to each key.
// create_unordered_map_grouped(length, ["one", "three", "two"]) == {3: ["one", "two"], 5: ["three"]}
// Also known as group_by
template <typename ContainerIn, typename F>
auto create_unordered_map_grouped(F f, const ContainerIn& values)
{
    return pairs_to_unordered_map_grouped(zip(transform(f, values), values));
}

// API search type: get_from_map : (Map key val, key) -> Maybe val
// fwd bind count: 1
// Returns just the value of a key if key is present.
// Otherwise returns nothing.
template <typename MapType,
    typename Key = typename MapType::key_type,
    typename Val = typename MapType::mapped_type>
maybe<Val> get_from_map(const MapType& map, const Key& key)
{
    auto it = map.find(key);
    if (it == std::end(map))
        return nothing<Val>();
    return just(it->second);
}

// API search type: get_from_map_unsafe : (Map key val, key) -> val
// fwd bind count: 1
// Returns the value of a key if key is present.
// Crashes otherwise.
template <typename MapType,
    typename Key = typename MapType::key_type,
    typename Val = typename MapType::mapped_type>
Val get_from_map_unsafe(const MapType& map, const Key& key)
{
    return unsafe_get_just(get_from_map(map, key));
}

// API search type: get_from_map_with_def : (Map key val, val, key) -> val
// fwd bind count: 2
// Returns the value of a key if key is present.
// Otherwise returns the provided default.
// Also known as prop_or.
template <typename MapType,
    typename Key = typename MapType::key_type,
    typename Val = typename MapType::mapped_type>
Val get_from_map_with_def(const MapType& map, const Val& defVal,
    const Key& key)
{
    return just_with_default(defVal, get_from_map(map, key));
}

// API search type: get_first_from_map : (Map key val, [key]) -> Maybe val
// fwd bind count: 1
// Returns just the value of the first key present.
// Otherwise returns nothing.
template <typename MapType,
    typename KeysContainer,
    typename Key = typename MapType::key_type,
    typename Val = typename MapType::mapped_type>
maybe<Val> get_first_from_map(const MapType& map, const KeysContainer& keys)
{
    static_assert(std::is_same<typename KeysContainer::value_type, Key>::value,
        "Key type does not match.");
    for (const auto& key: keys)
    {
        auto it = map.find(key);
        if (it != std::end(map))
            return just(it->second);
    }
    return nothing<Val>();
}

// API search type: get_first_from_map_unsafe : (Map key val, [key]) -> val
// fwd bind count: 1
// Returns the value of the first key present.
// Crashes otherwise.
template <typename MapType,
    typename KeysContainer,
    typename Key = typename MapType::key_type,
    typename Val = typename MapType::mapped_type>
Val get_first_from_map_unsafe(const MapType& map, const KeysContainer& keys)
{
    return unsafe_get_just(get_first_from_map(map, keys));
}

// API search type: get_first_from_map_with_def : (Map key val, val, [key]) -> val
// fwd bind count: 2
// Returns the value of the first key present.
// Otherwise returns the provided default.
template <typename MapType,
    typename KeysContainer,
    typename Key = typename MapType::key_type,
    typename Val = typename MapType::mapped_type>
Val get_first_from_map_with_def(const MapType& map, const Val& defVal,
    const KeysContainer& keys)
{
    return just_with_default(defVal, get_first_from_map(map, keys));
}

// API search type: map_contains : (Map key val, key) -> Bool
// fwd bind count: 1
// Checks if a map contains a key.
template <typename MapType, typename Key = typename MapType::key_type>
bool map_contains(const MapType& map, const Key& key)
{
    auto it = map.find(key);
    return it != std::end(map);
}

// API search type: map_keep_if : ((key -> Bool), Map key val) -> Map key val
// fwd bind count: 1
// Filters the map by keys.
// map_keep_if(is_upper_case, {a: 1, b: 2, A: 3, C: 4}) == {A: 3, C: 4}
// Also known as pick_by.
template <typename MapType, typename Pred>
MapType map_keep_if(Pred pred, const MapType& map)
{
    MapType result;
    for (const auto& key_and_value : map)
    {
        if (internal::invoke(pred, key_and_value.first))
        {
            result.insert(key_and_value);
        }
    }
    return result;
}

// API search type: map_drop_if : ((key -> Bool), Map key val) -> Map key val
// fwd bind count: 1
// Filters the map by keys.
// map_drop_if(is_lower_case, {a: 1, b: 2, A: 3, C: 4}) == {A: 3, C: 4}
// Inverse of map_keep_if.
template <typename MapType, typename Pred>
MapType map_drop_if(Pred pred, const MapType& map)
{
    return map_keep_if(logical_not(pred), map);
}

// API search type: map_keep : ([key], Map key val) -> Map key val
// fwd bind count: 1
// Keeps only the pairs of the map found in the key list.
// map_keep([a, d], {a: 1, b: 2, c: 3, d: 4}) == {a: 1, d: 4}
// map_keep([a, e, f], {a: 1, b: 2, c: 3, d: 4}) == {a: 1}
// Also known as pick.
template <typename MapType, typename KeyContainer>
MapType map_keep(const KeyContainer& keys, const MapType& map)
{
    static_assert(std::is_same<
        typename KeyContainer::value_type,
        typename MapType::key_type>::value,
        "Key types do not match.");
    return map_keep_if(bind_2nd_of_2(is_elem_of<KeyContainer>, keys), map);
}

// API search type: map_drop : ([key], Map key val) -> Map key val
// fwd bind count: 1
// Keeps only the pairs of the map not found in the key list.
// Inverse of map_keep.
// map_drop([b, c], {a: 1, b: 2, c: 3, d: 4}); == {a: 1, d: 4}
template <typename MapType, typename KeyContainer>
MapType map_drop(const KeyContainer& keys, const MapType& map)
{
    static_assert(std::is_same<
        typename KeyContainer::value_type,
        typename MapType::key_type>::value,
        "Key types do not match.");
    return map_drop_if(bind_2nd_of_2(is_elem_of<KeyContainer>, keys), map);
}

// API search type: map_keep_if_value : ((val -> Bool), Map key val) -> Map key val
// fwd bind count: 1
// Filters the map by values.
// map_keep_if_value(is_upper_case, {1: a, 2: b, 3: A, 4: C}) == {3: A, 4: C}
// Also known as filter_values.
template <typename MapType, typename Pred>
MapType map_keep_if_value(Pred pred, const MapType& map)
{
    MapType result;
    for (const auto& key_and_value : map)
    {
        if (internal::invoke(pred, key_and_value.second))
        {
            result.insert(key_and_value);
        }
    }
    return result;
}

// API search type: map_drop_if_value : ((value -> Bool), Map key val) -> Map key val
// fwd bind count: 1
// Filters the map by values.
// map_drop_if_value(is_lower_case, {1: a, 2: b, 3: A, 4: C}) == {3: A, 4: C}
// Inverse of map_keep_if_value.
template <typename MapType, typename Pred>
MapType map_drop_if_value(Pred pred, const MapType& map)
{
    return map_keep_if_value(logical_not(pred), map);
}

// API search type: map_keep_values : ([value], Map key val) -> Map key val
// fwd bind count: 1
// Keeps only the pairs of the map found in the value list.
// map_keep_values([1, 4], {a: 1, b: 2, c: 3, d: 4}) == {a: 1, d: 4}
// map_keep_values([1, 5, 6], {a: 1, b: 2, c: 3, d: 4}) == {a: 1}
template <typename MapType, typename ValueContainer>
MapType map_keep_values(const ValueContainer& values, const MapType& map)
{
    static_assert(std::is_same<
        typename ValueContainer::value_type,
        typename MapType::mapped_type>::value,
        "Value types do not match.");
    return map_keep_if_value(
        bind_2nd_of_2(is_elem_of<ValueContainer>, values), map);
}

// API search type: map_drop_values : ([value], Map key val) -> Map key val
// fwd bind count: 1
// Keeps only the pairs of the map not found in the value list.
// Inverse of map_keep_values.
// map_drop_values([2, 3], {a: 1, b: 2, c: 3, d: 4}) == {a: 1, d: 4}
template <typename MapType, typename ValueContainer>
MapType map_drop_values(const ValueContainer& values, const MapType& map)
{
    static_assert(std::is_same<
        typename ValueContainer::value_type,
        typename MapType::mapped_type>::value,
        "Value types do not match.");
    return map_drop_if_value(
        bind_2nd_of_2(is_elem_of<ValueContainer>, values), map);
}

// API search type: map_pluck : (key, [Map key val]) -> [val]
// fwd bind count: 1
// Extracts values to a specific key from a list of maps.
// map_pluck('a', [{a: 1, b: 2}, {a: 3}, {c: 4}]) == [Just 1, Just 3, Nothing]
template <typename MapContainer,
    typename ContainerOut =
        std::vector<maybe<typename MapContainer::value_type::mapped_type>>,
    typename MapType = typename MapContainer::value_type,
    typename Key = typename MapType::key_type,
    typename Val = typename MapType::mapped_type>
ContainerOut map_pluck(const Key& key, const MapContainer& maps)
{
    return transform_convert<ContainerOut>(
        bind_2nd_of_2(get_from_map<MapType>, key), maps);
}

// API search type: choose : ([(a, b)], a) -> Maybe b
// fwd bind count: 1
// Selects a value assigned to a key iff the key exists exactly once.
// choose([(1,a), (2,b)], 2) == Just b;
// choose([(1,a), (1,b)], 2) == Nothing;
// choose([(1,a), (2,b)], 3) == Nothing;
template<typename Key, typename Val>
maybe<Val> choose(const std::vector<std::pair<Key, Val>>& pairs, const Key& x)
{
    if (count(x, transform(fst<Key, Val>, pairs)) != 1)
        return {};
    return get_from_map(pairs_to_map<std::unordered_map<Key, Val>>(pairs), x);
}

// API search type: choose_by : ([((a -> Bool), b)], a) -> Maybe b
// fwd bind count: 2
// Iff exactly one predicate is fulfilled
// the value assigned to this predicate is selected.
// choose_by([(is_even,a), (is_bigger_than_3,b)], 2) == Just a;
// choose_by([(is_even,a), (is_bigger_than_3,b)], 5) == Just b;
// choose_by([(is_even,a), (is_bigger_than_3,b)], 1) == Nothing;
// choose_by([(is_even,a), (is_bigger_than_3,b)], 4) == Nothing;
template<typename Key, typename Val>
maybe<Val> choose_by(
    const std::vector<std::pair<std::function<bool(const Key&)>, Val>>& pairs,
    const Key& x)
{
    maybe<Val> result;
    for (const auto& p : pairs)
    {
        if (internal::invoke(p.first, x))
        {
            if (is_just(result))
            {
                return nothing<Val>();
            }
            result = p.second;
        }
    }
    return result;
}

// API search type: choose_lazy : ([(a, (() -> b))], a) -> Maybe b
// fwd bind count: 1
// Evaluates a lazy value assigned to a key iff the key exists exactly once.
// choose_lazy([(1,a), (2,b)], 2) == Just b();
// choose_lazy([(1,a), (1,b)], 2) == Nothing;
// choose_lazy([(1,a), (2,b)], 3) == Nothing;
template <typename Key, typename ValStub>
auto choose_lazy(const std::vector<std::pair<Key, ValStub>>& pairs,
                 const Key& x)
{
    using Ret = maybe<std::decay_t<internal::invoke_result_t<ValStub>>>;
    const auto res = choose(pairs, x);
    if (res.is_nothing())
        return Ret{};
    else
        return Ret{res.unsafe_get_just()()};
}

// API search type: choose_by_lazy : ([((a -> Bool), (() -> b))], a) -> Maybe b
// fwd bind count: 2
// Iff exactly one predicate is fulfilled
// the lazy value assigned to this predicate is evaluated.
// choose_by_lazy([(is_even,a), (is_bigger_than_3,b)], 2) == Just a();
// choose_by_lazy([(is_even,a), (is_bigger_than_3,b)], 5) == Just b();
// choose_by_lazy([(is_even,a), (is_bigger_than_3,b)], 1) == Nothing;
// choose_by_lazy([(is_even,a), (is_bigger_than_3,b)], 4) == Nothing;
template <typename Key, typename ValStub>
auto choose_by_lazy(
    const std::vector<std::pair<std::function<bool(const Key&)>, ValStub>>& pairs,
    const Key& x)
{
    using Ret = maybe<std::decay_t<internal::invoke_result_t<ValStub>>>;

    const auto res = choose_by(pairs, x);
    if (res.is_nothing())
        return Ret{};
    else
        return Ret{res.unsafe_get_just()()};
}

// API search type: choose_def : (b, [(a, b)], a) -> b
// fwd bind count: 1
// Selects a value assigned to a key iff the key exists exactly once,
// otherwise returns the given default value.
// choose_def(c, [(1,a), (2,b)], 2) == b;
// choose_def(c, [(1,a), (1,b)], 2) == c;
// choose_def(c, [(1,a), (2,b)], 3) == c;
template<typename Key, typename Val>
Val choose_def(const Val& def,
    const std::vector<std::pair<Key, Val>>& pairs, const Key& x)
{
    if (count(x, transform(fst<Key, Val>, pairs)) != 1)
        return def;
    return get_from_map_with_def(
        pairs_to_map<std::unordered_map<Key, Val>>(pairs), def, x);
}

// API search type: choose_by_def : (b, [((a -> Bool), b)], a) -> b
// fwd bind count: 2
// Iff exactly one predicate is fulfilled
// the value assigned to this predicate is selected.
// Otherwise the given default value is returned.
// choose_by_def(c, [(is_even,a), (is_bigger_than_3,b)], 2) == Just a;
// choose_by_def(c, [(is_even,a), (is_bigger_than_3,b)], 5) == Just b;
// choose_by_def(c, [(is_even,a), (is_bigger_than_3,b)], 1) == c;
// choose_by_def(c, [(is_even,a), (is_bigger_than_3,b)], 4) == c;
template<typename Key, typename Val>
Val choose_by_def(const Val& def,
    const std::vector<std::pair<std::function<bool(const Key&)>, Val>>& pairs,
    const Key& x)
{
    return just_with_default(def, choose_by(pairs, x));
}

// API search type: choose_def_lazy : ((() -> b), [(a, (() -> b))], a) -> b
// fwd bind count: 1
// Evaluates a lazy value assigned to a key iff the key exists exactly once,
// otherwise evaluates the given default lazy value.
// choose_def_lazy(c, [(1,a), (2,b)], 2) == b();
// choose_def_lazy(c, [(1,a), (1,b)], 2) == c();
// choose_def_lazy(c, [(1,a), (2,b)], 3) == c();
template <typename Key, typename ValStub>
auto choose_def_lazy(const ValStub& def,
                     const std::vector<std::pair<Key, ValStub>>& pairs,
                     const Key& x)
{
    return choose_def(def, pairs, x)();
}

// API search type: choose_by_def_lazy : ((() -> b), [((a -> Bool), (() -> b))], a) -> b
// fwd bind count: 2
// Iff exactly one predicate is fulfilled
// the value assigned to this predicate is evaluated.
// Otherwise the given default value is evaluated and returned.
// choose_by_def_lazy(c, [(is_even,a), (is_bigger_than_3,b)], 2) == Just a();
// choose_by_def_lazy(c, [(is_even,a), (is_bigger_than_3,b)], 5) == Just b();
// choose_by_def_lazy(c, [(is_even,a), (is_bigger_than_3,b)], 1) == c();
// choose_by_def_lazy(c, [(is_even,a), (is_bigger_than_3,b)], 4) == c();
template <typename Key, typename ValStub>
auto choose_by_def_lazy(
    const ValStub& def,
    const std::vector<std::pair<std::function<bool(const Key&)>, ValStub>>& pairs,
    const Key& x)
{
    return choose_by_def(def, pairs, x)();
}

} // namespace fplus

//
// optimize.hpp
//

// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)



//
// transform.hpp
//

// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)



//
// split.hpp
//

// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)




//
// internal/split.hpp
//

// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)


#include <iterator>
#include <utility>



namespace fplus
{
namespace internal
{
template <typename GroupByCallable, typename F, typename ContainerIn>
auto group_on_labeled_impl(GroupByCallable group, F f, const ContainerIn& xs)
{
    const auto grouped = group(is_equal_by(f), xs);
    const auto attach_label = [f](const auto& g)
    {
        using std::begin;
        return std::make_pair(internal::invoke(f, *begin(g)), g);
    };
    return fplus::transform(attach_label, grouped);
}
}
}

namespace fplus
{

// API search type: group_by : (((a, a) -> Bool), [a]) -> [[a]]
// fwd bind count: 1
// Arrange the elements into groups using a given predicate.
// Only groups of consecutive elements are formed.
// For a version scanning the whole container see group_globally_by.
// group_by((==), [1,2,2,2,3,2,2,4,5,5]) == [[1],[2,2,2],[3],[2,2],[4],[5,5]]
// BinaryPredicate p is a (not neccessarily transitive) connectivity check.
// O(n)
template <typename BinaryPredicate, typename ContainerIn,
        typename ContainerOut = typename std::vector<ContainerIn>>
ContainerOut group_by(BinaryPredicate p, const ContainerIn& xs)
{
    // ContainerOut is not deduced to
    // SameContNewType(ContainerIn, ContainerIn)
    // here, since ContainerIn could be a std::string.
    internal::check_binary_predicate_for_container<BinaryPredicate, ContainerIn>();
    static_assert(std::is_same<ContainerIn,
        typename ContainerOut::value_type>::value,
        "Containers do not match.");
    ContainerOut result;
    if (is_empty(xs))
        return result;
    typedef typename ContainerOut::value_type InnerContainerOut;
    *internal::get_back_inserter(result) = InnerContainerOut(1, xs.front());
    for (auto it = ++std::begin(xs); it != std::end(xs); ++it)
    {
        if (internal::invoke(p, result.back().back(), *it))
            *internal::get_back_inserter(result.back()) = *it;
        else
            *internal::get_back_inserter(result) = InnerContainerOut(1, *it);
    }
    return result;
}

// API search type: group_on : ((a -> b), [a]) -> [[a]]
// fwd bind count: 1
// Arrange elements equal after applying a transformer into groups.
// Only groups of consecutive elements are formed.
// For a version scanning the whole container see group_globally_on.
// group_on((mod 10), [12,22,34]) == [[12,22],[34]]
// O(n)
template <typename F, typename ContainerIn>
auto group_on(F f, const ContainerIn& xs)
{
    return group_by(is_equal_by(f), xs);
}

// API search type: group_on_labeled : ((a -> b), [a]) -> [(b, [a])]
// fwd bind count: 1
// Arrange elements equal after applying a transformer into groups,
// adding the transformation result as a label to the group.
// Only groups of consecutive elements are formed.
// For a version scanning the whole container see group_globally_on_labeled.
// group_on_labeled((mod 10), [12,22,34]) == [(2,[12,22]), (4,[34])]
// O(n)
template <typename F, typename ContainerIn>
auto group_on_labeled(F f, const ContainerIn& xs)
{
    const auto group = [](auto f1, const auto& xs1)
    {
        return group_by(f1, xs1);
    };

    return internal::group_on_labeled_impl(group, f, xs);
}

// API search type: group : [a] -> [[a]]
// fwd bind count: 0
// Arrange equal elements into groups.
// Only groups of consecutive elements are formed.
// For a version scanning the whole container see group_globally.
// group([1,2,2,2,3,2,2,4,5,5]) == [[1],[2,2,2],[3],[2,2],[4],[5,5]]
// O(n)
template <typename ContainerIn,
        typename ContainerOut = typename std::vector<ContainerIn>>
ContainerOut group(const ContainerIn& xs)
{
    static_assert(std::is_same<ContainerIn,
        typename ContainerOut::value_type>::value,
        "Containers do not match.");
    typedef typename ContainerIn::value_type T;
    auto pred = [](const T& x, const T& y) { return x == y; };
    return group_by<decltype(pred), ContainerIn, ContainerOut>(pred, xs);
}

// API search type: group_globally_by : (((a, a) -> Bool), [a]) -> [[a]]
// fwd bind count: 1
// Arrange equal elements into groups.
// group_globally_by((==), [1,2,2,2,3,2,2,4,5,5])
// == [[1],[2,2,2,2,2],[3],[4],[5,5]]
// BinaryPredicate p is a
// transitive (whenever p(x,y) and p(y,z), then also p(x,z)) equality check.
// O(n^2)
// If you need O(n*log(n)), sort and then use group_by
template <typename BinaryPredicate, typename ContainerIn,
        typename ContainerOut = typename std::vector<ContainerIn>>
ContainerOut group_globally_by(BinaryPredicate p, const ContainerIn& xs)
{
    internal::check_binary_predicate_for_container<BinaryPredicate, ContainerIn>();
    static_assert(std::is_same<ContainerIn,
        typename ContainerOut::value_type>::value,
        "Containers do not match.");
    typedef typename ContainerOut::value_type InnerContainerOut;
    ContainerOut result;
    for (const auto& x : xs)
    {
        bool found = false;
        for (auto& ys : result)
        {
            if (internal::invoke(p, x, ys.back()))
            {
                *internal::get_back_inserter(ys) = x;
                found = true;
                break;
            }
        }
        if (!found)
        {
            *internal::get_back_inserter(result) = InnerContainerOut(1, x);
        }
    }
    return result;
}

// API search type: group_globally_on : ((a -> b), [a]) -> [[a]]
// fwd bind count: 1
// Arrange elements equal after applying a transformer into groups.
// group_globally_on((mod 10), [12,34,22]) == [[12,22],[34]]
// O(n^2)
// If you need O(n*log(n)), sort and then use group_on
template <typename F, typename ContainerIn>
auto group_globally_on(F f, const ContainerIn& xs)
{
    return group_globally_by(is_equal_by(f), xs);
}

// API search type: group_globally_on_labeled : ((a -> b), [a]) -> [(b, [a])]
// fwd bind count: 1
// Arrange elements equal after applying a transformer into groups,
// adding the transformation result as a label to the group.
// group_globally_on_labeled((mod 10), [12,34,22]) == [(2,[12,22]),(4, [34])]
// O(n^2)
// If you need O(n*log(n)), sort and then use group_on_labeled
template <typename F, typename ContainerIn>
auto group_globally_on_labeled(F f, const ContainerIn& xs)
{
    const auto group = [](auto f1, const auto& xs1)
    {
        return group_globally_by(f1, xs1);
    };

    return internal::group_on_labeled_impl(group, f, xs);
}

// API search type: group_globally : [a] -> [[a]]
// fwd bind count: 0
// Arrange equal elements into groups.
// group_globally([1,2,2,2,3,2,2,4,5,5]) == [[1],[2,2,2,2,2],[3],[4],[5,5]]
// O(n^2)
// If you need O(n*log(n)), sort and then use group
template <typename ContainerIn,
        typename ContainerOut = typename std::vector<ContainerIn>>
ContainerOut group_globally(const ContainerIn& xs)
{
    static_assert(std::is_same<ContainerIn,
        typename ContainerOut::value_type>::value,
        "Containers do not match.");
    typedef typename ContainerIn::value_type T;
    auto pred = [](const T& x, const T& y) { return x == y; };
    return group_globally_by(pred, xs);
}

// API search type: cluster_by : (((a, a) -> Bool), [a]) -> [[a]]
// fwd bind count: 1
// Groups connected components, stable regarding initial order.
// cluster_by(\x y -> abs (y - x) <= 3), [2,3,6,4,12,11,20,23,8,4])
// == [[2,3,6,4,12,11,8,4],[20,23]]
// BinaryPredicate p is a connectivity check, being
//  a) commutative (p(x,y) = p(y,x))
//  b) reflexive (p(x,x) = true)
//  c) not neccessarily transitive, but can be
// O(n^2), memory complexity also O(n^2)
template <typename BinaryPredicate, typename ContainerIn,
        typename ContainerOut = typename std::vector<ContainerIn>>
ContainerOut cluster_by(BinaryPredicate p, const ContainerIn& xs)
{
    internal::check_binary_predicate_for_container<BinaryPredicate, ContainerIn>();
    static_assert(std::is_same<ContainerIn,
        typename ContainerOut::value_type>::value,
        "Containers do not match.");

    typedef std::vector<unsigned char> bools;
    bools zero_filled_row(size_of_cont(xs), 0);

    // adjecency matrix
    typedef std::vector<bools> boolss;
    boolss adj_mat(size_of_cont(xs), zero_filled_row);

    for (const auto& idx_and_val_y : enumerate(xs))
    {
        auto idx_y = idx_and_val_y.first;
        auto val_y = idx_and_val_y.second;
        for (const auto& idx_and_val_x : enumerate(xs))
        {
            auto idx_x = idx_and_val_x.first;
            auto val_x = idx_and_val_x.second;
            if (internal::invoke(p, val_y, val_x))
            {
                adj_mat[idx_y][idx_x] = 1;
            }
        }
    }

    bools already_used = zero_filled_row;
    auto is_already_used = [&](std::size_t i) -> bool
    {
        return already_used[i] != 0;
    };

    typedef std::vector<std::size_t> idxs;
    typedef std::vector<idxs> idxss;

    auto bools_to_idxs = [](const bools& activations) -> idxs
    {
        auto unsigned_char_to_bool = [](unsigned char x)
        {
            return x != 0;
        };
        return find_all_idxs_by(unsigned_char_to_bool, activations);
    };

    idxss idx_clusters;
    std::function<void(std::size_t)> process_idx = [&](std::size_t idx) -> void
    {
        auto connected_idxs = bools_to_idxs(adj_mat[idx]);
        auto new_connected_idxs = drop_if(is_already_used, connected_idxs);
        if (is_empty(new_connected_idxs))
        {
            return;
        }
        idx_clusters.back() = append(idx_clusters.back(), new_connected_idxs);
        for (const auto& new_idx : new_connected_idxs)
        {
            already_used[new_idx] = 1;
        }
        for (const auto& new_idx : new_connected_idxs)
        {
            process_idx(new_idx);
        }
    };

    typedef typename ContainerOut::value_type InnerContainerOut;

    for (const auto& idx : all_idxs(xs))
    {
        if (is_already_used(idx))
        {
            continue;
        }
        *internal::get_back_inserter(idx_clusters) = idxs();
        *internal::get_back_inserter(idx_clusters.back()) = idx;
        already_used[idx] = 1;
        process_idx(idx);
    }

    typedef typename ContainerIn::value_type T;

    auto idx_to_val = [&](std::size_t idx) -> T
    {
        return elem_at_idx(idx, xs);
    };

    auto idxs_to_vals = [&](const idxs& val_idxs) -> InnerContainerOut
    {
        return transform_convert<InnerContainerOut>(idx_to_val, sort(val_idxs));
    };

    return transform_convert<ContainerOut>(idxs_to_vals, idx_clusters);
}

// API search type: split_by : ((a -> Bool), Bool, [a]) -> [[a]]
// fwd bind count: 2
// Split a sequence at every element fulfilling a predicate.
// The splitting elements are discarded.
// split_by(is_even, true, [1,3,2,2,5,5,3,6,7,9]) == [[1,3],[],[5,5,3],[7,9]]
// also known as split_when
// O(n)
template <typename UnaryPredicate, typename ContainerIn,
        typename ContainerOut = typename std::vector<ContainerIn>>
ContainerOut split_by
        (UnaryPredicate pred, bool allow_empty, const ContainerIn& xs)
{
    internal::check_unary_predicate_for_container<UnaryPredicate, ContainerIn>();
    static_assert(std::is_same<ContainerIn,
        typename ContainerOut::value_type>::value,
        "Containers do not match.");

    if (allow_empty && is_empty(xs))
    {
        return {{}};
    }

    ContainerOut result;
    auto itOut = internal::get_back_inserter(result);
    auto start = std::begin(xs);

    while (start != std::end(xs))
    {
        const auto stop = std::find_if(start, std::end(xs), pred);
        if (start != stop || allow_empty)
        {
            *itOut = { start, stop };
        }
        if (stop == std::end(xs))
        {
            break;
        }
        start = internal::add_to_iterator(stop);
        if (allow_empty && start == std::end(xs))
        {
            *itOut = typename ContainerOut::value_type();
        }
    }
    return result;
}

// API search type: split_by_keep_separators : ((a -> Bool), [a]) -> [[a]]
// fwd bind count: 1
// Split a sequence at every element fulfilling a predicate.
// The splitting elements are kept.
// split_by_keep_separators(is_even, true, [1,3,2,2,5,5,3,6,7,9])
// == [[1,3],[2],[2,5,5,3],[6,7,9]]
// O(n)
template <typename UnaryPredicate, typename ContainerIn,
        typename ContainerOut = typename std::vector<ContainerIn>>
ContainerOut split_by_keep_separators
        (UnaryPredicate pred, const ContainerIn& xs)
{
    internal::check_unary_predicate_for_container<UnaryPredicate, ContainerIn>();
    static_assert(std::is_same<ContainerIn,
        typename ContainerOut::value_type>::value,
        "Containers do not match.");
    ContainerOut result;
    if (is_empty(xs))
        return result;
    auto itOut = internal::get_back_inserter(result);
    auto start = std::begin(xs);
    while (start != std::end(xs))
    {
        const auto stop = std::find_if(
            internal::add_to_iterator(start), std::end(xs), pred);
        *itOut = { start, stop };
        if (stop == std::end(xs))
        {
            break;
        }
        start = stop;
    }
    return result;
}

// API search type: split : (a, Bool, [a]) -> [[a]]
// fwd bind count: 2
// Split a sequence at every element equal to x.
// The splitting elements are discarded.
// split(0, true, [1,3,2,0,0,6,0,7,5]) == [[1,3,2],[],[6],[7,5]]
// O(n)
template <typename ContainerIn,
        typename T = typename ContainerIn::value_type>
auto split(const T& x, bool allow_empty, const ContainerIn& xs)
{
    return split_by(is_equal_to(x), allow_empty, xs);
}

// API search type: split_one_of : ([a], Bool, [a]) -> [[a]]
// fwd bind count: 2
// Split a sequence at every element present in delimiters.
// The splitting elements are discarded.
// Also known as split_words_by_many.
// split_one_of([0,3], true [1,3,2,0,0,6,0,7,5]) == [[1],[2],[],[6],[7,5]]
// split_one_of(" o", false, "How are u?") == ["H","w","are","u?"]
// O(n)
template <typename ContainerIn,
        typename ContainerDelims>
auto split_one_of(
    const ContainerDelims delimiters, bool allow_empty, const ContainerIn& xs)
{
    const auto pred = [&](const typename ContainerIn::value_type& x) -> bool
    {
        return is_elem_of(x, delimiters);
    };
    return split_by(pred, allow_empty, xs);
}

// API search type: split_keep_separators : ((a -> Bool), [a]) -> [[a]]
// fwd bind count: 1
// Split a sequence at every element equal to x.
// The splitting elements are kept.
// split_keep_separators(2, true, [1,3,2,2,5,5,3,2,7,9])
// == [[1,3],[2],[2,5,5,3],[6,7,9]]
// O(n)
template <typename ContainerIn,
        typename T = typename ContainerIn::value_type>
auto split_keep_separators(const T& x, const ContainerIn& xs)
{
    return split_by_keep_separators(is_equal_to(x), xs);
}

// API search type: split_at_idx : (Int, [a]) -> ([a], [a])
// fwd bind count: 1
// Split a sequence at a specific position.
// split_at_idx(2, [0,1,2,3,4]) == ([0,1],[2,3,4])
template <typename Container>
std::pair<Container, Container> split_at_idx
        (std::size_t idx, const Container& xs)
{
    assert(idx <= size_of_cont(xs));
    return make_pair(get_segment(0, idx, xs),
        get_segment(idx, size_of_cont(xs), xs));
}

// API search type: insert_at_idx : (Int, a, [a]) -> [a]
// fwd bind count: 2
// Insert an element into a sequence at a specific position.
// insert_at_idx(2, 0, [1,2,3,4]) == [1,2,0,3,4].
template <typename Container,
        typename T = typename Container::value_type>
Container insert_at_idx(std::size_t idx, const T& x, const Container& xs)
{
    const auto splitted = split_at_idx(idx, xs);
    return concat(std::vector<Container>(
        {
            splitted.first,
            singleton_seq<T, Container>(x),
            splitted.second
        }));
}

// API search type: partition : ((a -> Bool), [a]) -> ([a], [a])
// fwd bind count: 1
// Split a sequence into two groups.
// The first group contains all elements fulfilling the predicate.
// The second group contains the remaining elements.
// partition(is_even, [0,1,1,3,7,2,3,4]) == ([0,2,4],[1,1,3,7,3])
template <typename UnaryPredicate, typename Container>
std::pair<Container, Container> partition
        (UnaryPredicate pred, const Container& xs)
{
    internal::check_unary_predicate_for_container<UnaryPredicate, Container>();
    Container matching;
    Container notMatching;
    auto itOutMatching = internal::get_back_inserter(matching);
    auto itOutNotMatching = internal::get_back_inserter(notMatching);
    for (const auto& x : xs)
    {
        if (internal::invoke(pred, x))
            *itOutMatching = x;
        else
            *itOutNotMatching = x;
    }
    return make_pair(matching, notMatching);
}

// API search type: split_at_idxs : ([Int], [a]) -> [[a]]
// fwd bind count: 1
// Split a sequence at specific indices.
// split_at_idxs([2,5], [0,1,2,3,4,5,6,7]) == [[0,1],[2,3,4],[5,6,7]]
// split_at_idxs([2,5,5], [0,1,2,3,4,5,6,7]) == [[0,1],[2,3,4],[],[5,6,7]]
template <typename ContainerIdxs, typename ContainerIn,
        typename ContainerOut = std::vector<ContainerIn>>
ContainerOut split_at_idxs(const ContainerIdxs& idxsIn, const ContainerIn& xs)
{
    static_assert(std::is_same<typename ContainerIdxs::value_type, std::size_t>::value,
        "Indices must be std::size_t");
    static_assert(std::is_same<ContainerIn,
        typename ContainerOut::value_type>::value,
        "Containers do not match.");
    ContainerIdxs idxStartC = {0};
    ContainerIdxs idxEndC = {size_of_cont(xs)};
    std::vector<ContainerIdxs> containerIdxss = {idxStartC, idxsIn, idxEndC};
    auto idxs = concat(containerIdxss);
    auto idxsClean = sort(idxs);
    ContainerOut result;
    internal::prepare_container(result, size_of_cont(idxsClean) - 1);
    auto itOut = internal::get_back_inserter(result);
    auto idxPairs = overlapping_pairs(idxsClean);
    for (const auto& idxPair : idxPairs)
    {
        *itOut = get_segment(idxPair.first, idxPair.second, xs);
    }
    return result;
}

// API search type: split_every : (Int, [a]) -> [[a]]
// fwd bind count: 1
// Split a sequence every n elements.
// split_every(3, [0,1,2,3,4,5,6,7]) == [[0,1,2],[3,4,5],[6,7]]
// Also known as chunk or chunks.
template <typename ContainerIn,
        typename ContainerOut = std::vector<ContainerIn>>
ContainerOut split_every(std::size_t n, const ContainerIn& xs)
{
    return split_at_idxs<
        std::vector<std::size_t>,
        ContainerIn,
        ContainerOut>(
            numbers_step<std::size_t>(
                n, size_of_cont(xs), n),
            xs);
}

// API search type: split_by_token : ([a], Bool, [a]) -> [[a]]
// fwd bind count: 2
// Split a sequence at every segment matching a token.
// split_by_token(", ", true, "foo, bar, baz") == ["foo", "bar", "baz"]
template <typename ContainerIn,
        typename ContainerOut = typename std::vector<ContainerIn>>
ContainerOut split_by_token(const ContainerIn& token,
        bool allow_empty, const ContainerIn& xs)
{
    static_assert(std::is_same<ContainerIn,
        typename ContainerOut::value_type>::value,
        "Containers do not match.");
    const auto token_begins =
        find_all_instances_of_token_non_overlapping(token, xs);
    const auto token_ends =
        transform(add_to<std::size_t>(size_of_cont(token)), token_begins);
    assert(is_sorted(interweave(token_begins, token_ends)));

    typedef std::vector<std::size_t> idx_vec;
    const auto segments = zip(
        fplus::append(idx_vec(1, 0), token_ends),
        fplus::append(token_begins, idx_vec(1, size_of_cont(xs))));

    ContainerOut result;
    auto itOut = internal::get_back_inserter(result);
    for (const auto& segment : segments)
    {
        if (segment.first != segment.second || allow_empty)
        *itOut = get_segment(segment.first, segment.second, xs);
    }
    return result;
}

// API search type: run_length_encode_by : (((a, a) -> Bool), [a]) -> [(Int, a)]
// fwd bind count: 1
// RLE using a specific binary predicate as equality check.
// run_length_encode_by((==),[1,2,2,2,2,3,3,2)) == [(1,1),(4,2),(2,3),(1,2)]
template <typename BinaryPredicate,
        typename ContainerIn,
        typename T = typename ContainerIn::value_type,
        typename ContainerOut =
            typename std::vector<std::pair<std::size_t, T>>>
ContainerOut run_length_encode_by(BinaryPredicate pred, const ContainerIn& xs)
{
    internal::check_binary_predicate_for_container<BinaryPredicate, ContainerIn>();
    ContainerOut result;
    auto groups = group_by(pred, xs);
    auto group_to_pair = [](const ContainerIn& group) -> std::pair<std::size_t, T>
    {
        return std::make_pair(size_of_cont(group), group.front());
    };
    return transform(group_to_pair, groups);
}

// API search type: run_length_encode : [a] -> [(Int, a)]
// fwd bind count: 0
// RLE.
// run_length_encode([1,2,2,2,2,3,3,2)) == [(1,1),(4,2),(2,3),(1,2)]
template <typename ContainerIn,
        typename T = typename ContainerIn::value_type>
auto run_length_encode(const ContainerIn& xs)
{
    return run_length_encode_by(is_equal<T>, xs);
}

// API search type: run_length_decode : [(Int, a)] -> [a]
// fwd bind count: 0
// Inverse operation to run_length_encode.
// run_length_decode([(1,1),(4,2),(2,3),(1,2)]) == [1,2,2,2,2,3,3,2)
template <typename ContainerIn,
        typename Pair = typename ContainerIn::value_type,
        typename Cnt = typename Pair::first_type>
auto run_length_decode(const ContainerIn& pairs)
{
    static_assert(std::is_convertible<Cnt, std::size_t>::value,
        "Count type must be convertible to std::size_t.");
    const auto pair_to_vec =
        [](const Pair& p)
    {
        return replicate(p.first, p.second);
    };
    return concat(transform(pair_to_vec, pairs));
}

// API search type: span : ((a -> Bool), [a]) -> ([a], [a])
// fwd bind count: 1
// span, applied to a predicate p and a list xs,
// returns a tuple where first element is longest prefix (possibly empty)
// of xs of elements that satisfy p
// and second element is the remainder of the list.
// span(is_even, [0,2,4,5,6,7,8]) == ([0,2,4], [5,6,7,8])
template <typename Container, typename UnaryPredicate>
std::pair<Container, Container> span(UnaryPredicate pred, const Container& xs)
{
    auto maybeIdx = find_first_idx_by(logical_not(pred), xs);
    return {
        take(just_with_default<std::size_t>(size_of_cont(xs), maybeIdx), xs),
        drop(just_with_default<std::size_t>(size_of_cont(xs), maybeIdx), xs)
    };
}

// API search type: divvy : (Int, Int, [a]) -> [[a]]
// fwd bind count: 2
// Generates subsequences overlapping with a specific step.
// divvy(5, 2, [0,1,2,3,4,5,6,7,8,9]) == [[0,1,2,3,4],[2,3,4,5,6],[4,5,6,7,8]]
// divvy(length, 1, xs) is also known as aperture
// divvy(1, step, xs) is also known as stride
//     (but withouts the nested lists in the result)
template <typename ContainerIn,
        typename ContainerOut = std::vector<ContainerIn>>
ContainerOut divvy(std::size_t length, std::size_t step, const ContainerIn& xs)
{
    assert(length > 0);
    assert(step > 0);
    const auto start_idxs =
        numbers_step<std::size_t>(
            0, size_of_cont(xs) - (length - 1), step);

    ContainerOut result;
    internal::prepare_container(result, size_of_cont(start_idxs));
    auto itOut = internal::get_back_inserter(result);

    for (const auto start_idx : start_idxs)
    {
        *itOut = get_segment(start_idx, start_idx + length, xs);
    }
    return result;
}

// API search type: aperture : (Int, [a]) -> [[a]]
// fwd bind count: 1
// Generates overlapping subsequences.
// aperture(5, [0,1,2,3,4,5,6]) == [[0,1,2,3,4],[1,2,3,4,5],[2,3,4,5,6]]
template <typename ContainerIn,
        typename ContainerOut = std::vector<ContainerIn>>
ContainerOut aperture(std::size_t length, const ContainerIn& xs)
{
    assert(length > 0);
    const auto start_idxs =
        numbers<std::size_t>(
            0, size_of_cont(xs) - (length - 1));

    ContainerOut result;
    internal::prepare_container(result, size_of_cont(start_idxs));
    auto itOut = internal::get_back_inserter(result);

    for (const auto start_idx : start_idxs)
    {
        *itOut = get_segment(start_idx, start_idx + length, xs);
    }
    return result;
}

// API search type: stride : (Int, [a]) -> [a]
// fwd bind count: 1
// Keeps every nth element.
// stride(3, [0,1,2,3,4,5,6,7]) == [0,3,6]
template <typename Container>
Container stride(std::size_t step, const Container& xs)
{
    assert(step > 0);
    Container ys;
    auto it = internal::get_back_inserter<Container>(ys);
    auto it_in = std::begin(xs);
    std::size_t i = 0;
    const auto xs_size = size_of_cont(xs);
    while(it_in != std::end(xs))
    {
        *it = *it_in;
        std::size_t increment = std::min(step, xs_size - i);
        internal::advance_iterator(it_in, increment);
        i += increment;
    }
    return ys;
}

// API search type: winsorize : (Float, [Float]) -> [Float]
// fwd bind count: 1
// Winsorizing
// winsorize(0.1, [1,3,4,4,4,4,4,4,6,8]) == [3,3,4,4,4,4,4,4,6,6]
template <typename Container>
Container winsorize(double trim_ratio, const Container& xs)
{
    if (size_of_cont(xs) < 2)
    {
        return xs;
    }
    trim_ratio = std::max(trim_ratio, 0.0);
    const auto xs_sorted = sort(xs);
    std::size_t amount =
        floor<double, std::size_t>(
            trim_ratio * static_cast<double>(size_of_cont(xs_sorted)));
    amount = std::min(size_of_cont(xs_sorted) / 2, amount);
    const auto parts = split_at_idxs(
        std::vector<std::size_t>({amount, size_of_cont(xs_sorted) - amount}),
        xs_sorted);
    assert(size_of_cont(parts) == 3);
    typedef typename Container::value_type T;
    if (is_empty(parts[1]))
    {
        return Container(size_of_cont(xs_sorted), median(xs_sorted));
    }
    else
    {
        const T lower = parts[1].front();
        const T upper = parts[1].back();
        const auto result = concat(std::vector<Container>({
            Container(amount, lower),
            parts[1],
            Container(amount, upper)}));
        assert(size_of_cont(result) == size_of_cont(xs_sorted));
        return result;
    }
}

// API search type: separate_on : ((a -> b), [a]) -> [[a]]
// fwd bind count: 1
// Separate elements equal after applying a transformer into groups.
// separate_on((mod 10), [12,22,34]) == [[12,34],[22]]
template <typename F, typename ContainerIn,
        typename ContainerOut = typename std::vector<ContainerIn>>
ContainerOut separate_on(F f, const ContainerIn& xs)
{
    static_assert(std::is_same<ContainerIn,
        typename ContainerOut::value_type>::value,
        "Containers do not match.");

    ContainerOut result;
    if (is_empty(xs)) {
        return result;
    }

    const auto groups = group_globally_on(f, xs);
    bool found = true;
    auto itOut = internal::get_back_inserter(result);
    std::size_t index = 0;
    while (found) {
        typename ContainerOut::value_type sub_result;
        found = false;
        auto itOutInner = internal::get_back_inserter(sub_result);
        for (auto& group: groups) {
            if (size_of_cont(group) > index)
            {
                *itOutInner = group[index];
                found = true;
            }
        }
        if (found) {
            *itOut = sub_result;
            ++index;
        }
    }
    return result;
}

// API search type: separate : [a] -> [[a]]
// fwd bind count: 0
// Separate equal elements into groups.
// separate([1, 2, 2, 3, 3, 4, 4, 4]) == [[1, 2, 3, 4], [2, 3, 4], [4]]
template <typename ContainerIn,
        typename ContainerOut = typename std::vector<ContainerIn>>
ContainerOut separate(const ContainerIn& xs)
{
    static_assert(std::is_same<ContainerIn,
        typename ContainerOut::value_type>::value,
        "Containers do not match.");
    typedef typename ContainerIn::value_type T;
    return separate_on(identity<T>, xs);
}

} // namespace fplus


#include <algorithm>
#include <future>
#include <iterator>
#include <mutex>
#include <random>

namespace fplus
{

// API search type: transform_with_idx : (((Int, a) -> b), [a]) -> [b]
// fwd bind count: 1
// Apply a function to every index and corresponding element of a sequence.
// transform_with_idx(f, [6, 4, 7]) == [f(0, 6), f(1, 4), f(2, 7)]
template <typename F, typename ContainerIn,
    typename ContainerOut = typename internal::same_cont_new_t_from_binary_f<
        ContainerIn, F, std::size_t, typename ContainerIn::value_type, 0>::type>
ContainerOut transform_with_idx(F f, const ContainerIn& xs)
{
    internal::trigger_static_asserts<internal::binary_function_tag, F>();
    ContainerOut ys;
    internal::prepare_container(ys, size_of_cont(xs));
    auto it = internal::get_back_inserter<ContainerOut>(ys);
    std::size_t idx = 0;
    for (const auto& x : xs)
    {
        *it = internal::invoke(f, idx++, x);
    }
    return ys;
}

// API search type: transform_and_keep_justs : ((a -> Maybe b), [a]) -> [b]
// fwd bind count: 1
// Map function over values and drop resulting nothings.
// Also known as filter_map.
template <typename F, typename ContainerIn>
auto transform_and_keep_justs(F f, const ContainerIn& xs)
{
    using X = typename ContainerIn::value_type;
    internal::
        trigger_static_asserts<internal::unary_function_tag, F, X>();

    using ContainerOut = typename internal::same_cont_new_t<
        ContainerIn,
        typename std::decay_t<internal::invoke_result_t<F, X>>::type>::type;

    auto transformed = transform(f, xs);
    return justs<decltype(transformed), ContainerOut>(transformed);
}

// API search type: transform_and_keep_oks : ((a -> Result b), [a]) -> [b]
// fwd bind count: 1
// Map function over values and drop resulting errors.
template <typename F, typename ContainerIn>
auto transform_and_keep_oks(F f, const ContainerIn& xs)
{
    using X = typename ContainerIn::value_type;
    internal::
        trigger_static_asserts<internal::unary_function_tag, F, X>();

    using ContainerOut = typename internal::same_cont_new_t<
        ContainerIn,
        typename std::decay_t<internal::invoke_result_t<F, X>>::ok_t>::type;
    auto transformed = transform(f, xs);
    return oks<decltype(transformed), ContainerOut>(transformed);
}

// API search type: transform_and_concat : ((a -> [b]), [a]) -> [b]
// fwd bind count: 1
// Map function over values and concat results.
// Also known as flat_map or concat_map.
template <typename F, typename ContainerIn>
auto transform_and_concat(F f, const ContainerIn& xs)
{
    internal::trigger_static_asserts<internal::unary_function_tag, F, typename ContainerIn::value_type>();
    return concat(transform(f, xs));
}

// API search type: replicate_elems : (Int, [a]) -> [a]
// fwd bind count: 1
// Replicate every element n times, concatenate the result.
// replicate_elems(3, [1,2]) == [1, 1, 1, 2, 2, 2]
template <typename Container>
Container replicate_elems(std::size_t n, const Container& xs)
{
    typedef typename Container::value_type T;
    return transform_and_concat(bind_1st_of_2(replicate<T, Container>, n), xs);
}

// API search type: interleave : [[a]] -> [a]
// fwd bind count: 0
// Return a sequence that contains elements from the provided sequences
// in alternating order. If one list runs out of items,
// appends the items from the remaining list.
// interleave([[1,2,3],[4,5],[6,7,8]]) == [1,4,6,2,5,7,3,8]
template <typename ContainerIn,
    typename ContainerOut = typename ContainerIn::value_type>
ContainerOut interleave(const ContainerIn& xss)
{
    typedef typename ContainerIn::value_type inner_t;
    typedef std::vector<typename inner_t::const_iterator> its_t;
    const auto inner_cbegin = [](const inner_t& xs) { return xs.cbegin(); };
    const auto inner_cend = [](const inner_t& xs) { return xs.cend(); };
    auto it_pairs = zip(
        transform_convert<its_t>(inner_cbegin, xss),
        transform_convert<its_t>(inner_cend, xss));

    ContainerOut result;
    const std::size_t length = sum(transform(size_of_cont<inner_t>, xss));
    internal::prepare_container(result, length);
    auto it_out = internal::get_back_inserter<ContainerOut>(result);
    bool still_appending = true;
    while (still_appending)
    {
        still_appending = false;
        for (auto& it_pair : it_pairs)
        {
            if (it_pair.first != it_pair.second)
            {
                *it_out = *it_pair.first;
                still_appending = true;
                ++it_pair.first;
            }
        }
    }
    return result;
}

// API search type: transpose : [[a]] -> [[a]]
// fwd bind count: 0
// Transpose a nested sequence aka. table aka. two-dimensional matrix.
// transpose([[1,2,3],[4,5,6],[7,8,9]]) == [[1,4,7],[2,5,8],[3,6,9]]
// transpose([[1,2,3],[4,5],[7,8,9]]) == [[1,4,7],[2,5,8],[3,9]]
template <typename Container>
Container transpose(const Container& rows)
{
    if (is_empty(rows))
    {
        return {};
    }
    return split_every<typename Container::value_type, Container>(
        size_of_cont(rows), interleave(rows));
}

namespace internal
{

template <typename Container>
Container shuffle(internal::reuse_container_t,
    std::uint_fast32_t seed, Container&& xs)
{
    std::mt19937 g(seed);
    std::shuffle(std::begin(xs), std::end(xs), g);
    return std::forward<Container>(xs);
}

template <typename Container>
Container shuffle(internal::create_new_container_t,
    std::uint_fast32_t seed, const Container& xs)
{
    Container ys = xs;
    return internal::shuffle(internal::reuse_container_t(), seed, std::move(ys));
}

} // namespace internal

// API search type: shuffle : (Int, [a]) -> [a]
// fwd bind count: 1
// Returns a randomly shuffled version of xs.
// Example call: shuffle(std::mt19937::default_seed, xs);
// Example call: shuffle(std::random_device()(), xs);
template <typename Container>
auto shuffle(std::uint_fast32_t seed, Container&& xs)
{
    return(internal::shuffle(internal::can_reuse_v<Container>{},
        seed, std::forward<Container>(xs)));
}

// API search type: sample : (Int, Int, [a]) -> [a]
// fwd bind count: 2
// Returns n random elements from xs without replacement.
// n has to be smaller than or equal to the number of elements in xs.
// Also known as rnd_select.
// Example call: sample(std::mt19937::default_seed, 3, xs);
// Example call: sample(std::random_device()(), 3, xs);
template <typename Container>
Container sample(std::uint_fast32_t seed, std::size_t n, const Container& xs)
{
    assert(n <= size_of_cont(xs));
    return get_segment(0, n, shuffle(seed, xs));
}

// API search type: random_element : (Int, [a]) -> a
// fwd bind count: 1
// Returns one random element from xs.
// xs must be non-empty.
// Example call: random_element(std::mt19937::default_seed, xs)
// Example call: random_element(std::random_device()(), xs)
// Also known as choice.
template <typename Container>
typename Container::value_type random_element(
    std::uint_fast32_t seed, const Container& xs)
{
    assert(is_not_empty(xs));
    std::mt19937 gen(seed);
    std::uniform_int_distribution<std::size_t> dis(0, size_of_cont(xs) - 1);
    return elem_at_idx(dis(gen), xs);
}

// API search type: random_elements : (Int, Int, [a]) -> a
// fwd bind count: 2
// Returns random elements from xs with replacement.
// xs must be non-empty.
// Example call: random_elements(std::mt19937::default_seed, 10, xs)
// Example call: random_elements(std::random_device()(), 10, xs)
template <typename Container>
Container random_elements(
    std::uint_fast32_t seed, std::size_t n, const Container& xs)
{
    assert(is_not_empty(xs));
    std::mt19937 gen(seed);
    std::uniform_int_distribution<std::size_t> dis(0, size_of_cont(xs) - 1);
    const auto draw = [&]() -> typename Container::value_type
    {
        return elem_at_idx(dis(gen), xs);
    };
    return generate<Container>(draw, n);
}

// API search type: apply_functions : ([(a -> b)], a) -> [b]
// fwd bind count: 1
// Applies a list of functions to a value.
template <typename FunctionContainer,
          typename F = typename FunctionContainer::value_type,
          typename FIn>
auto apply_functions(const FunctionContainer& functions, const FIn& x)
{
    internal::trigger_static_asserts<internal::unary_function_tag, F, FIn>();

    using FOut = std::decay_t<internal::invoke_result_t<F, FIn>>;
    using ContainerOut =
        typename internal::same_cont_new_t<FunctionContainer, FOut, 0>::type;

    ContainerOut ys;
    internal::prepare_container(ys, size_of_cont(functions));
    auto it = internal::get_back_inserter<ContainerOut>(ys);
    for (const auto& f : functions)
    {
        *it = internal::invoke(f, x);
    }
    return ys;
}

// API search type: apply_function_n_times : ((a -> a), Int, a) -> a
// fwd bind count: 2
// Applies a functional n times in a row.
template <typename F, typename FIn>
auto apply_function_n_times(F f, std::size_t n, const FIn& x)
{
    internal::trigger_static_asserts<internal::unary_function_tag, F, FIn>();
    using FOut = std::decay_t<internal::invoke_result_t<F, FIn>>;
    static_assert(std::is_same<FOut, FIn>::value,
                  "Input and output of F must be the same type.");
    if (n == 0)
    {
        return x;
    }
    FOut y = internal::invoke(f, x);
    for (std::size_t i = 1; i < n; ++i)
    {
        y = internal::invoke(f, y);
    }
    return y;
}

// API search type: transform_parallelly : ((a -> b), [a]) -> [b]
// fwd bind count: 1
// transform_parallelly((*2), [1, 3, 4]) == [2, 6, 8]
// Same as transform, but can utilize multiple CPUs by using std::launch::async.
// Only makes sense if one run of the provided function
// takes enough time to justify the synchronization overhead.
// One thread per container element is spawned.
// Check out transform_parallelly_n_threads to limit the number of threads.
template <typename F, typename ContainerIn>
auto transform_parallelly(F f, const ContainerIn& xs)
{
    using ContainerOut = typename internal::
        same_cont_new_t_from_unary_f<ContainerIn, F, 0>::type;
    using X = typename ContainerIn::value_type;
    internal::trigger_static_asserts<internal::unary_function_tag, F, X>();
    auto handles = transform([&f](const X& x)
    {
        return std::async(std::launch::async, [&x, &f]()
        {
            return internal::invoke(f, x);
        });
    }, xs);

    ContainerOut ys;
    internal::prepare_container(ys, size_of_cont(xs));
    auto it = internal::get_back_inserter<ContainerOut>(ys);
    for (auto& handle : handles)
    {
        *it = handle.get();
    }
    return ys;
}

// API search type: transform_parallelly_n_threads : (Int, (a -> b), [a]) -> [b]
// fwd bind count: 2
// transform_parallelly_n_threads(4, (*2), [1, 3, 4]) == [2, 6, 8]
// Same as transform, but uses n threads in parallel.
// Only makes sense if one run of the provided function
// takes enough time to justify the synchronization overhead.
// Can be used for applying the MapReduce pattern.
template <typename F, typename ContainerIn>
auto transform_parallelly_n_threads(std::size_t n, F f, const ContainerIn& xs)
{
    using ContainerOut = typename internal::
        same_cont_new_t_from_unary_f<ContainerIn, F, 0>::type;
    using X = typename ContainerIn::value_type;
    using Y = internal::invoke_result_t<F, X>;
    using x_ptr_t =  const X*;
    auto queue = transform_convert<std::vector<x_ptr_t>>(
        [](const X& x) -> x_ptr_t
        {
            return &x;
        }, xs);

    std::mutex queue_mutex;
    std::mutex thread_results_mutex;
    std::map<std::size_t, std::decay_t<Y>> thread_results;
    std::size_t queue_idx = 0;

    const auto worker_func = [&]()
    {
        for (;;)
        {
            std::size_t idx = std::numeric_limits<std::size_t>::max();
            x_ptr_t x_ptr = nullptr;
            {
                std::lock_guard<std::mutex> queue_lock(queue_mutex);
                if (queue_idx == queue.size())
                {
                    return;
                }
                idx = queue_idx;
                x_ptr = queue[idx];
                ++queue_idx;
            }

            const auto y = internal::invoke(f, *x_ptr);

            {
                std::lock_guard<std::mutex> thread_results_lock(
                    thread_results_mutex);
                thread_results.insert(std::make_pair(idx, y));
            }
        }
    };

    const auto create_thread = [&]() -> std::thread
    {
        return std::thread(worker_func);
    };
    auto threads = generate<std::vector<std::thread>>(create_thread, n);

    for (auto& thread : threads)
    {
        thread.join();
    }

    return get_map_values<decltype(thread_results), ContainerOut>(
        thread_results);
}

// API search type: reduce_parallelly : (((a, a) -> a), a, [a]) -> a
// fwd bind count: 2
// reduce_parallelly((+), 0, [1, 2, 3]) == (0+1+2+3) == 6
// Same as reduce, but can utilize multiple CPUs by using std::launch::async.
// Combines the initial value and all elements of the sequence
// using the given function in unspecified order.
// The set of f, init and value_type should form a commutative monoid.
// One thread per container element is spawned.
// Check out reduce_parallelly_n_threads to limit the number of threads.
template <typename F, typename Container>
typename Container::value_type reduce_parallelly(
    F f, const typename Container::value_type& init, const Container& xs)
{
    if (is_empty(xs))
    {
        return init;
    }
    else if (size_of_cont(xs) == 1)
    {
        return internal::invoke(f, init, xs.front());
    }
    else
    {
        typedef typename Container::value_type T;
        const auto f_on_pair = [f](const std::pair<T, T>& p) -> T
        {
            return internal::invoke(f, p.first, p.second);
        };
        auto transform_result =
            transform_parallelly(f_on_pair, adjacent_pairs(xs));
        if (size_of_cont(xs) % 2 == 1)
        {
            transform_result.push_back(last(xs));
        }
        return reduce_parallelly(f, init, transform_result);
    }
}

// API search type: reduce_parallelly_n_threads : (Int, ((a, a) -> a), a, [a]) -> a
// fwd bind count: 3
// reduce_parallelly_n_threads(2, (+), 0, [1, 2, 3]) == (0+1+2+3) == 6
// Same as reduce, but can utilize multiple CPUs by using std::launch::async.
// Combines the initial value and all elements of the sequence
// using the given function in unspecified order.
// The set of f, init and value_type should form a commutative monoid.
template <typename F, typename Container>
typename Container::value_type reduce_parallelly_n_threads(
    std::size_t n,
    F f, const typename Container::value_type& init, const Container& xs)
{
    if (is_empty(xs))
    {
        return init;
    }
    else if (size_of_cont(xs) == 1)
    {
        return internal::invoke(f, init, xs.front());
    }
    else
    {
        typedef typename Container::value_type T;
        const auto f_on_pair = [f](const std::pair<T, T>& p) -> T
        {
            return internal::invoke(f, p.first, p.second);
        };
        auto transform_result =
            transform_parallelly_n_threads(n, f_on_pair, adjacent_pairs(xs));
        if (size_of_cont(xs) % 2 == 1)
        {
            transform_result.push_back(last(xs));
        }
        return reduce_parallelly_n_threads(n, f, init, transform_result);
    }
}

// API search type: reduce_1_parallelly : (((a, a) -> a), [a]) -> a
// fwd bind count: 1
// reduce_1_parallelly((+), [1, 2, 3]) == (1+2+3) == 6
// Same as reduce_1, but can utilize multiple CPUs by using std::launch::async.
// Joins all elements of the sequence using the given function
// in unspecified order.
// The set of f and value_type should form a commutative semigroup.
// One thread per container element is spawned.
// Check out reduce_1_parallelly_n_threads to limit the number of threads.
template <typename F, typename Container>
typename Container::value_type reduce_1_parallelly(F f, const Container& xs)
{
    assert(is_not_empty(xs));
    if (size_of_cont(xs) == 1)
    {
        return xs.front();
    }
    else
    {
        typedef typename Container::value_type T;
        const auto f_on_pair = [f](const std::pair<T, T>& p) -> T
        {
            return internal::invoke(f, p.first, p.second);
        };
        auto transform_result =
            transform_parallelly(f_on_pair, adjacent_pairs(xs));
        if (size_of_cont(xs) % 2 == 1)
        {
            transform_result.push_back(last(xs));
        }
        return reduce_1_parallelly(f, transform_result);
    }
}

// API search type: reduce_1_parallelly_n_threads : (Int, ((a, a) -> a), [a]) -> a
// fwd bind count: 2
// reduce_1_parallelly_n_threads(2, (+), [1, 2, 3]) == (1+2+3) == 6
// Same as reduce_1, but can utilize multiple CPUs by using std::launch::async.
// Joins all elements of the sequence using the given function
// in unspecified order.
// The set of f and value_type should form a commutative semigroup.
template <typename F, typename Container>
typename Container::value_type reduce_1_parallelly_n_threads(
    std::size_t n, F f, const Container& xs)
{
    assert(is_not_empty(xs));
    if (size_of_cont(xs) == 1)
    {
        return xs.front();
    }
    else
    {
        typedef typename Container::value_type T;
        const auto f_on_pair = [f](const std::pair<T, T>& p) -> T
        {
            return internal::invoke(f, p.first, p.second);
        };
        auto transform_result =
            transform_parallelly_n_threads(n, f_on_pair, adjacent_pairs(xs));
        if (size_of_cont(xs) % 2 == 1)
        {
            transform_result.push_back(last(xs));
        }
        return reduce_1_parallelly_n_threads(n, f, transform_result);
    }
}

// API search type: keep_if_parallelly : ((a -> Bool), [a]) -> [a]
// fwd bind count: 1
// Same as keep_if but using multiple threads.
// Can be useful if calling the predicate takes some time.
// keep_if_parallelly(is_even, [1, 2, 3, 2, 4, 5]) == [2, 2, 4]
// One thread per container element is spawned.
// Check out keep_if_parallelly_n_threads to limit the number of threads.
template <typename Pred, typename Container>
Container keep_if_parallelly(Pred pred, const Container& xs)
{
    // Avoid a temporary std::vector<bool>.
    const auto idxs = find_all_idxs_by(
        is_equal_to<std::uint8_t>(1),
        transform_parallelly([pred](const auto & x) -> std::uint8_t {
            return pred(x) ? 1 : 0;
        }, xs));
    return elems_at_idxs(idxs, xs);
}

// API search type: keep_if_parallelly_n_threads : (Int, (a -> Bool), [a]) -> [a]
// fwd bind count: 2
// Same as keep_if but using multiple threads.
// Can be useful if calling the predicate takes some time.
// keep_if_parallelly_n_threads(3, is_even, [1, 2, 3, 2, 4, 5]) == [2, 2, 4]
template <typename Pred, typename Container>
Container keep_if_parallelly_n_threads(
    std::size_t n, Pred pred, const Container& xs)
{
    // Avoid a temporary std::vector<bool>.
    const auto idxs = find_all_idxs_by(
        is_equal_to<std::uint8_t>(1),
        transform_parallelly_n_threads(n, [pred](const auto & x) -> std::uint8_t {
            return pred(x) ? 1 : 0;
        }, xs));
    return elems_at_idxs(idxs, xs);
}

// API search type: transform_reduce : ((a -> b), ((b, b) -> b), b, [a]) -> b
// fwd bind count: 3
// transform_reduce(square, add, 0, [1,2,3]) == 0+1+4+9 = 14
// The set of binary_f, init and unary_f::output should form a
// commutative monoid.
template <typename UnaryF, typename BinaryF, typename Container, typename Acc>
auto transform_reduce(UnaryF unary_f,
                      BinaryF binary_f,
                      const Acc& init,
                      const Container& xs)
{
    return reduce(binary_f, init, transform(unary_f, xs));
}

// API search type: transform_reduce_1 : ((a -> b), ((b, b) -> b), [a]) -> b
// fwd bind count: 2
// transform_reduce_1(square, add, [1,2,3]) == 0+1+4+9 = 14
// The set of binary_f, and unary_f::output should form
// a commutative semigroup.
template <typename UnaryF, typename BinaryF, typename Container>
auto transform_reduce_1(UnaryF unary_f, BinaryF binary_f, const Container& xs)
{
    return reduce_1(binary_f, transform(unary_f, xs));
}

// API search type: transform_reduce_parallelly : ((a -> b), ((b, b) -> b), b, [a]) -> b
// fwd bind count: 3
// transform_reduce_parallelly(square, add, 0, [1,2,3]) == 0+1+4+9 = 14
// Also known as map_reduce.
// The set of binary_f, init and unary_f::output
// should form a commutative monoid.
// One thread per container element is spawned.
// Check out transform_reduce_parallelly_n_threads to limit the number of threads.
template <typename UnaryF, typename BinaryF, typename Container, typename Acc>
auto transform_reduce_parallelly(UnaryF unary_f,
                                 BinaryF binary_f,
                                 const Acc& init,
                                 const Container& xs)
{
    return reduce_parallelly(binary_f, init, transform_parallelly(unary_f, xs));
}

// API search type: transform_reduce_parallelly_n_threads : (Int, (a -> b), ((b, b) -> b), b, [a]) -> b
// fwd bind count: 4
// transform_reduce_parallelly_n_threads(2, square, add, 0, [1,2,3]) == 0+1+4+9 = 14
// Also known as map_reduce.
// The set of binary_f, init and unary_f::output
// should form a commutative monoid.
template <typename UnaryF, typename BinaryF, typename Container, typename Acc>
auto transform_reduce_parallelly_n_threads(std::size_t n,
                                           UnaryF unary_f,
                                           BinaryF binary_f,
                                           const Acc& init,
                                           const Container& xs)
{
    return reduce_parallelly_n_threads(
        n, binary_f, init, transform_parallelly_n_threads(n, unary_f, xs));
}

// API search type: transform_reduce_1_parallelly : ((a -> b), ((b, b) -> b), [a]) -> b
// fwd bind count: 2
// transform_reduce_1_parallelly(square, add, [1,2,3]) == 0+1+4+9 = 14
// Also Known as map_reduce.
// The set of binary_f, and unary_f::output
// should form a commutative semigroup.
// One thread per container element is spawned.
// Check out transform_reduce_1_parallelly_n_threads to limit the number of threads.
template <typename UnaryF, typename BinaryF, typename Container>
auto transform_reduce_1_parallelly(UnaryF unary_f,
                                   BinaryF binary_f,
                                   const Container& xs)
{
    return reduce_1_parallelly(binary_f, transform_parallelly(unary_f, xs));
}

// API search type: transform_reduce_1_parallelly_n_threads : (Int, (a -> b), ((b, b) -> b), [a]) -> b
// fwd bind count: 3
// transform_reduce_1_parallelly_n_threads(2, square, add, [1,2,3]) == 0+1+4+9 = 14
// Also Known as map_reduce.
// The set of binary_f, and unary_f::output
// should form a commutative semigroup.
template <typename UnaryF, typename BinaryF, typename Container>
auto transform_reduce_1_parallelly_n_threads(std::size_t n,
                                             UnaryF unary_f,
                                             BinaryF binary_f,
                                             const Container& xs)
{
    return reduce_1_parallelly_n_threads(
        n, binary_f, transform_parallelly_n_threads(n, unary_f, xs));
}

} // namespace fplus
#include <array>
#include <chrono>
#include <functional>

namespace fplus
{

// Optimizes the initial position to the nearest local minimum
// in regards to the objective_function
// using numerical gradient descent based on the epsilon neighborhood.
// momentum_conservation should be in [0, 1). A low value means much decay.
// If no fixed step size is provided, each step advances by the length
// of the gradient.
// In both cases the step is scaled with a step factor, starting at 1.0.
// If one iteration results in no further improvement,
// the step factor is reduced by a factor of 0.5.
// The callback is executed with
// iteration, step factor, momentum and current position
// after every iteration.
// A initial step factor other than 1.0 in all dimensions
// can be emulated by scaling ones objective function accordingly.
// Optimization stops if one of the provided criteria is met.
// minimize_downhill<1>(\x -> square(x[0] + 2), 0.0001, 0.01, {123})[0] == -2;
template <std::size_t N, typename F, typename pos_t = std::array<double, N>>
pos_t minimize_downhill(
        F objective_function,
        double epsilon,
        const pos_t& init_pos,
        maybe<double> fixed_step_size = nothing<double>(),
        double momentum_conservation = 0.5,
        double sufficing_value = std::numeric_limits<double>::lowest(),
        double min_step_factor = std::numeric_limits<double>::min(),
        std::size_t max_iterations = std::numeric_limits<std::size_t>::max(),
        long int max_milliseconds = std::numeric_limits<long int>::max(),
        const std::function<
                void (std::size_t, double, const pos_t&, const pos_t&)>&
            callback =
            std::function<
                void (std::size_t, double, const pos_t&, const pos_t&)>())
{
    std::size_t iteration = 0;
    double step_factor = 1.0;
    pos_t position = init_pos;
    double value = internal::invoke(objective_function, position);

    const auto start_time = std::chrono::steady_clock::now();
    const auto is_done = [&]() -> bool
    {
        if (max_milliseconds != std::numeric_limits<long int>::max())
        {
            const auto current_time = std::chrono::steady_clock::now();
            const auto elapsed = current_time - start_time;
            const auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
            if (elapsed_ms >= max_milliseconds)
            {
                return true;
            }
        }
        return
            iteration >= max_iterations ||
            step_factor <= min_step_factor ||
            value <= sufficing_value;
    };

    const auto calc_gradient =
        [&](const pos_t& pos) -> pos_t
    {
        pos_t result;
        for (std::size_t dim = 0; dim < N; ++dim)
        {
            auto test_pos_1 = pos;
            auto test_pos_2 = pos;
            test_pos_1[dim] -= epsilon / 2.0;
            test_pos_2[dim] += epsilon / 2.0;
            const auto val_1 = internal::invoke(objective_function, test_pos_1);
            const auto val_2 = internal::invoke(objective_function, test_pos_2);
            result[dim] = (val_2 - val_1) / epsilon;
        }
        return result;
    };

    const auto add = [](const pos_t& p1, const pos_t& p2) -> pos_t
    {
        pos_t result;
        for (std::size_t dim = 0; dim < N; ++dim)
        {
            result[dim] = p1[dim] + p2[dim];
        }
        return result;
    };

    const auto multiply = [](const pos_t& p, double f) -> pos_t
    {
        pos_t result;
        for (std::size_t dim = 0; dim < N; ++dim)
        {
            result[dim] = p[dim] * f;
        }
        return result;
    };

    const auto dist_to_origin = [](const pos_t& p) -> double
    {
        double acc = 0;
        for (std::size_t dim = 0; dim < N; ++dim)
        {
            acc += square(p[dim]);
        }
        return sqrt(acc);
    };

    const auto normalize = [&](const pos_t& p) -> pos_t
    {
        return multiply(p, 1.0 / dist_to_origin(p));
    };

    const auto null_vector = []() -> pos_t
    {
        pos_t result;
        for (std::size_t dim = 0; dim < N; ++dim)
        {
            result[dim] = 0;
        }
        return result;
    };

    pos_t momentum = null_vector();
    while (!is_done())
    {
        auto new_momentum = multiply(momentum, momentum_conservation);
        pos_t gradient = calc_gradient(add(position, new_momentum));
        const auto inverse_gradient = multiply(gradient, -1.0);

        auto new_momentum_add =
            is_nothing(fixed_step_size) ?
                inverse_gradient :
                multiply(
                    normalize(inverse_gradient),
                    fixed_step_size.unsafe_get_just());

        new_momentum =
            multiply(
                add(new_momentum, new_momentum_add),
                step_factor);
        if (dist_to_origin(momentum) <= std::numeric_limits<double>::min() &&
            dist_to_origin(new_momentum) <= std::numeric_limits<double>::min())
        {
            break;
        }
        const auto new_position = add(position, new_momentum);
        const auto new_value = internal::invoke(objective_function, new_position);
        if (new_value >= value)
        {
            step_factor /= 2.0;
        }
        else
        {
            value = new_value;
            position = new_position;
            momentum = new_momentum;
        }
        ++iteration;
        if (callback)
        {
            callback(iteration, step_factor, momentum, position);
        }
    }
    return position;
}

} // namespace fplus

//
// queue.hpp
//

// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)



#include <condition_variable>
#include <cstdint>
#include <deque>
#include <mutex>

namespace fplus
{

// A thread-safe queue.
template <typename T>
class queue
{
public:
    queue() :
        queue_(),
        mutex_(),
        cond_()
        {}
    fplus::maybe<T> pop()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if (queue_.empty())
        {
            return {};
        }
        auto item = queue_.front();
        queue_.pop_front();
        return item;
    }

    void push(const T& item)
    {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            queue_.push_back(item);
        }
        cond_.notify_one();
    }

    std::vector<T> pop_all()
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        const auto result = fplus::convert_container<std::vector<T>>(queue_);
        queue_.clear();
        return result;
    }

    std::vector<T> wait_and_pop_all()
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        cond_.wait(mlock, [&]() -> bool { return !queue_.empty(); });
        const auto result = fplus::convert_container<std::vector<T>>(queue_);
        queue_.clear();
        return result;
    }

    std::vector<T> wait_for_and_pop_all(std::int64_t max_wait_time_us)
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        const auto t = std::chrono::microseconds{ max_wait_time_us };
        cond_.wait_for(mlock, t, [&]() -> bool { return !queue_.empty(); });
        const auto result = fplus::convert_container<std::vector<T>>(queue_);
        queue_.clear();
        return result;
    }

private:
    std::deque<T> queue_;
    std::mutex mutex_;
    std::condition_variable cond_;
};

} // namespace fplus

//
// raii.hpp
//

// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)



//
// shared_ref.hpp
//

// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)


#include <memory>

namespace fplus
{

// A std::shared_ptr expresses
// optionality of the contained value (can be nullptr)
// and shared ownership that can be transferred.
// A std::optional expresses optionality only.
// The standard does not provide a class to
// express only shared ownership without optionality.
// shared_ref fills this gap.
// It is recommended to use make_shared_ref for constructing an instance.
template <typename T>
class shared_ref
{
public:
    shared_ref(const shared_ref&) = default;
    shared_ref(shared_ref&&) = default;
    shared_ref& operator=(const shared_ref&) = default;
    shared_ref& operator=(shared_ref&&) = default;
    ~shared_ref() = default;

    T* operator->() { return m_ptr.get(); }
    const T* operator->() const { return m_ptr.get(); }

    T& operator*() { return *m_ptr.get(); }
    const T& operator*() const { return *m_ptr.get(); }

    template <typename XT, typename...XTypes>
    friend shared_ref<XT> make_shared_ref(XTypes&&...args);

private:
    std::shared_ptr<T> m_ptr;
    shared_ref(T* value) :m_ptr(value) { assert(value != nullptr);  }
};

// http://stackoverflow.com/a/41976419/1866775
template <typename T, typename...Types>
shared_ref<T> make_shared_ref(Types&&...args)
{
    return shared_ref<T>(new T(std::forward<Types>(args)...));
}

} // namespace fplus

namespace fplus
{

// A generic RAII class.
// It is recommended to use make_raii for constructing an instance.
template <typename INIT, typename QUIT>
class raii
{
public:
    raii(INIT init, QUIT quit) :
        quit_(quit)
    {
        init();
    }
    ~raii()
    {
        quit_();
    }
    raii(const raii&) = delete;
    raii(raii&&) = default;
    raii& operator=(const raii&) = delete;
    raii& operator=(raii&&) = default;
private:
    QUIT quit_;
};

template <typename INIT, typename QUIT>
shared_ref<raii<INIT, QUIT>> make_raii(INIT init, QUIT quit)
{
    return make_shared_ref<raii<INIT, QUIT>>(init, quit);
}


} // namespace fplus

//
// read.hpp
//

// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)



#include <string>
#include <type_traits>

namespace fplus
{

namespace internal
{
    template <typename T>
    struct helper_read_value_struct {};

    template <>
    struct helper_read_value_struct <int>
    {
        static void read(const std::string& str,
            int& result, std::size_t& num_chars_used)
        {
            result = std::stoi(str, &num_chars_used);
        }
    };

    template <>
    struct helper_read_value_struct <long>
    {
        static void read(const std::string& str,
            long& result, std::size_t& num_chars_used)
        {
            result = std::stol(str, &num_chars_used);
        }
    };

    template <>
    struct helper_read_value_struct <long long>
    {
        static void read(const std::string& str,
            long long& result, std::size_t& num_chars_used)
        {
            result = std::stoll(str, &num_chars_used);
        }
    };

    template <>
    struct helper_read_value_struct <unsigned int>
    {
        static void read(const std::string& str,
            unsigned int& result, std::size_t& num_chars_used)
        {
            unsigned long result_u_l = std::stoul(str, &num_chars_used);
            result = static_cast<unsigned int>(result_u_l);
        }
    };

    template <>
    struct helper_read_value_struct <unsigned long>
    {
        static void read(const std::string& str,
            unsigned long& result, std::size_t& num_chars_used)
        {
            result = std::stoul(str, &num_chars_used);
        }
    };

    template <>
    struct helper_read_value_struct <unsigned long long>
    {
        static void read(const std::string& str,
            unsigned long long& result, std::size_t& num_chars_used)
        {
            result = std::stoull(str, &num_chars_used);
        }
    };

    template <>
    struct helper_read_value_struct <float>
    {
        static void read(const std::string& str,
            float& result, std::size_t& num_chars_used)
        {
            result = std::stof(str, &num_chars_used);
        }
    };

    template <>
    struct helper_read_value_struct <double>
    {
        static void read(const std::string& str,
            double& result, std::size_t& num_chars_used)
        {
            result = std::stod(str, &num_chars_used);
        }
    };

    template <>
    struct helper_read_value_struct <long double>
    {
        static void read(const std::string& str,
            long double& result, std::size_t& num_chars_used)
        {
            result = std::stold(str, &num_chars_used);
        }
    };

    template <>
    struct helper_read_value_struct <std::string>
    {
        static void read(const std::string& str,
            std::string& result, std::size_t& num_chars_used)
        {
            num_chars_used = str.size();
            result = str;
        }
    };
}

// API search type: read_value_result : String -> Result a
// Try to deserialize a value.
template <typename T>
result<T, std::string> read_value_result(const std::string& str)
{
    try
    {
        T result;
        std::size_t num_chars_used = 0;
        internal::helper_read_value_struct<T>::read(str,
            result, num_chars_used);
        if (num_chars_used != str.size())
        {
            return error<T>(std::string("String not fully parsable."));
        }
        return ok<T, std::string>(result);
    } catch(const std::invalid_argument& e) {
        return error<T, std::string>(e.what());
    } catch(const std::out_of_range& e) {
        return error<T, std::string>(e.what());
    }
}

// API search type: read_value : String -> Maybe a
// Try to deserialize/parse a value, e.g.:
// String to Int
// String to Float
// String to Double
// read_value<unsigned int>("42") == 42
// etc.
template <typename T>
maybe<T> read_value(const std::string& str)
{
    return to_maybe(read_value_result<T>(str));
}

// API search type: read_value_with_default : (a, String) -> a
// fwd bind count: 1
// Try to deserialize a value, return given default on failure, e.g.:
// String to Int
// String to Float
// String to Double
// read_value_with_default<unsigned int>(3, "42") == 42
// read_value_with_default<unsigned int>(3, "") == 3
// read_value_with_default<unsigned int>(3, "foo") == 3
// etc.
template <typename T>
T read_value_with_default(const T& def, const std::string& str)
{
    return just_with_default(def, to_maybe(read_value_result<T>(str)));
}

// API search type: read_value_unsafe : String -> a
// Try to deserialize a value, crash on failure, e.g.:
// String to Int
// String to Float
// String to Double
// read_value_unsafe<unsigned int>("42") == 42
// read_value_unsafe<unsigned int>("") == crash
// read_value_unsafe<unsigned int>("foo") == crash
// See read_value and read_value_with_default for safe versions.
// etc.
template <typename T>
T read_value_unsafe(const std::string& str)
{
    return unsafe_get_just(to_maybe(read_value_result<T>(str)));
}

} // namespace fplus

//
// replace.hpp
//

// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)



namespace fplus
{

namespace internal
{

template <typename UnaryPredicate, typename T, typename Container>
Container replace_if(internal::reuse_container_t,
    UnaryPredicate p, const T& dest, Container&& xs)
{
    std::replace_if(std::begin(xs), std::end(xs), p, dest);
    return std::forward<Container>(xs);
}

template <typename UnaryPredicate, typename T, typename Container>
Container replace_if(internal::create_new_container_t,
    UnaryPredicate p, const T& dest, const Container& xs)
{
    Container ys = xs;
    return replace_if(internal::reuse_container_t(),
        p, dest, std::move(ys));
}

} // namespace internal

// API search type: replace_if : ((a -> Bool), a, [a]) -> [a]
// fwd bind count: 2
// Replace every element fulfilling a predicate with a specific value.
// replace_if(is_even, 0, [1, 3, 4, 6, 7]) == [1, 3, 0, 0, 7]
template <typename UnaryPredicate, typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut replace_if(UnaryPredicate p,
    const typename ContainerOut::value_type& dest, Container&& xs)
{
    return internal::replace_if(internal::can_reuse_v<Container>{},
        p, dest, std::forward<Container>(xs));
}

namespace internal
{

template <typename Container,
        typename T = typename Container::value_type>
Container replace_elem_at_idx(internal::reuse_container_t,
    std::size_t idx, const T& dest, Container&& xs)
{
    assert(idx < xs.size());
    auto it = std::begin(xs);
    advance_iterator(it, idx);
    *it = dest;
    return std::forward<Container>(xs);
}

template <typename Container,
        typename T = typename Container::value_type>
Container replace_elem_at_idx(internal::create_new_container_t,
    std::size_t idx, const T& dest, const Container& xs)
{
    Container ys = xs;
    return replace_elem_at_idx(internal::reuse_container_t(),
        idx, dest, std::move(ys));
}

} // namespace internal

// API search type: replace_elem_at_idx : (Int, a, [a]) -> [a]
// fwd bind count: 2
// Replace the element at a specific index.
// replace_elem_at_idx(2, 0, [1, 3, 4, 4, 7]) == [1, 3, 0, 4, 7]
template <typename Container,
        typename ContainerOut = internal::remove_const_and_ref_t<Container>,
        typename T = typename ContainerOut::value_type>
ContainerOut replace_elem_at_idx(std::size_t idx, const T& dest,
    Container&& xs)
{
    return internal::replace_elem_at_idx(internal::can_reuse_v<Container>{},
        idx, dest, std::forward<Container>(xs));
}

// API search type: replace_elems : (a, a, [a]) -> [a]
// fwd bind count: 2
// Replace all elements matching source with dest.
// replace_elems(4, 0, [1, 3, 4, 4, 7]) == [1, 3, 0, 0, 7]
template <typename Container,
        typename ContainerOut = internal::remove_const_and_ref_t<Container>,
        typename T = typename ContainerOut::value_type>
ContainerOut replace_elems(const T& source, const T& dest, Container&& xs)
{
    return replace_if(bind_1st_of_2(is_equal<T>, source), dest, xs);
}

// API search type: replace_tokens : ([a], [a], [a]) -> [a]
// fwd bind count: 2
// Replace all segments matching source with dest.
// replace_tokens("haha", "hihi", "oh, hahaha!") == "oh, hihiha!"
// replace_tokens("haha", "o", "oh, hahaha!") == "oh, oha!"
template <typename Container>
Container replace_tokens
        (const Container& source, const Container& dest, const Container& xs)
{
    auto splitted = split_by_token(source, true, xs);
    return join(dest, splitted);
}

} // namespace fplus

//
// show.hpp
//

// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)



#include <iomanip>
#include <ios>
#include <sstream>
#include <string>

namespace fplus
{

// API search type: show : a -> String
// fwd bind count: 0
// 42 -> "42"
// Useful to simply show values, e.g.:
// Int to String
// Float to String
// Double to String
// Pair to String
// std::vector<std::list<T>> to String
// std::vector<T> to String
template <typename T>
std::string show(const T& x)
{
    std::ostringstream ss;
    ss << x;
    return ss.str();
}

// string identity
// "foo" -> "foo"
inline
std::string show(const std::string& str)
{
    return str;
}

template <typename T, typename A>
std::string show(const std::vector<T, A>& xs);

template <typename T, typename A>
std::string show(const std::list<T, A>& xs);

// {1, "one"} -> "(1, one)"
template <typename X, typename Y>
std::string show(const std::pair<X, Y>& p)
{
    return std::string("(") + show(p.first) + ", " + show(p.second) + ")";
}

template <typename Container> std::string show_cont(const Container& xs);

template <typename T, typename A>
std::string show(const std::vector<T, A>& xs)
{
    return show_cont(xs);
}

template <typename T, typename A>
std::string show(const std::list<T, A>& xs)
{
    return show_cont(xs);
}

template <typename T, typename A>
std::string show(const std::set<T, A>& xs)
{
    return show_cont(xs);
}

template <typename T, typename A>
std::string show(const std::deque<T, A>& xs)
{
    return show_cont(xs);
}

// API search type: show_cont_with_frame_and_newlines : (String, String, String, [a], Int) -> String
// fwd bind count: 3
// show_cont_with_frame_and_newlines (",", "(", ")", [1, 2, 3, 4, 5], 2)
// == "(1,2)
//      3,4)
//      5)"
template <typename Container>
std::string show_cont_with_frame_and_newlines(
    const std::string& separator,
    const std::string& prefix, const std::string& suffix,
    const Container& xs,
    std::size_t new_line_every_nth_elem )
{
    std::vector<std::string> elemStrs;
    elemStrs.reserve(xs.size());
    if (new_line_every_nth_elem == 0)
    {
        for (const auto& x : xs)
        {
            elemStrs.push_back(show(x));
        }
    }
    else
    {
        std::size_t i = 0;
        std::string newline =
            std::string("\n") + std::string(prefix.size(), ' ');
        for (const auto& x : xs)
        {
            if ( i && i % new_line_every_nth_elem == 0)
                elemStrs.push_back(newline + show(x));
            else
                elemStrs.push_back(show(x));
            ++i;
        }
    }
    return prefix + join(separator, elemStrs) + suffix;
}

// API search type: show_cont_with_frame : (String, String, String, [a]) -> String
// fwd bind count: 3
// show_cont_with_frame (" => ", "{", "}", [1, 2, 3]) == "{1 => 2 => 3}"
template <typename Container>
std::string show_cont_with_frame(
    const std::string& separator,
    const std::string& prefix, const std::string& suffix,
    const Container& xs)
{
    return
        show_cont_with_frame_and_newlines( separator, prefix, suffix, xs, 0);
}

// API search type: show_cont_with : (String, [a]) -> String
// fwd bind count: 1
// show_cont_with( " - ", [1, 2, 3]) == "[1 - 2 - 3]"
template <typename Container>
std::string show_cont_with(const std::string& separator, const Container& xs)
{
    return show_cont_with_frame(separator, "[", "]", xs);
}

// API search type: show_cont : [a] -> String
// fwd bind count: 0
// show_cont [1, 2, 3] -> "[1, 2, 3]"
// Can i.a show std::vector and std::map.
template <typename Container>
std::string show_cont(const Container& xs)
{
    return show_cont_with(", ", xs);
}

// API search type: show_maybe : Maybe a -> String
// fwd bind count: 0
// show_maybe(Just 42) -> "Just 42"
template <typename T>
std::string show_maybe(const maybe<T>& maybe)
{
    if (is_nothing(maybe))
        return "Nothing";
    else
        return std::string("Just " + show(unsafe_get_just(maybe)));
}

// API search type: show_result : Result a b -> String
// fwd bind count: 0
// show_result(Ok 42) -> "Ok 42"
// show_result(Error "fail") -> "Error fail"
template <typename Ok, typename Error>
std::string show_result(const result<Ok, Error>& result)
{
    if (is_error(result))
        return std::string("Error " + show(unsafe_get_error(result)));
    else
        return std::string("Ok " + show(unsafe_get_ok(result)));
}

// API search type: show_float : (Int, Int, Float) -> String
// fwd bind count: 2
// Can be used to show floating point values in a specific format
// (Float to String, Double to String etc.)
// Examples:
// const double pi = 3.14159
// show_float<double>(0, 3, pi) == "3.142"
// show_float<double>(1, 3, pi) == "3.142"
// show_float<double>(2, 3, pi) == "03.142"
// show_float<double>(3, 3, pi) == "003.142"
// show_float<double>(1, 2, pi) == "3.14"
// show_float<double>(1, 4, pi) == "3.1416"
// show_float<double>(1, 7, pi) == "3.1415900"
// show_float<double>(0, 3, -pi) == "-3.142"
// show_float<double>(1, 3, -pi) == "-3.142"
// show_float<double>(2, 3, -pi) == "-3.142"
// show_float<double>(3, 3, -pi) == "-03.142"
// show_float<double>(4, 3, -pi) == "-003.142"
// show_float<double>(0, 3, 0.142) == "0.142";
// show_float<double>(1, 3, 0.142) == "0.142";
// show_float<double>(2, 3, 0.142) == "00.142";
// fill_left(8, ' ', show_float<double>(0, 3, -pi)) == "  -3.142"
template <typename T>
std::string show_float(
    std::size_t min_left_chars, std::size_t right_char_count, const T& x)
{
    bool is_negative = x < 0;
    std::size_t min_left_chars_final =
        is_negative && min_left_chars > 0
        ? min_left_chars - 1
        : min_left_chars;
    std::stringstream stream;
    stream
        << std::fixed
        << std::setprecision(static_cast<int>(right_char_count))
        << std::abs(x);
    std::string s = stream.str();
    std::size_t min_dest_length = min_left_chars_final + 1 + right_char_count;
    std::string result = fill_left('0', min_dest_length, s);
    if (is_negative)
    {
        result = std::string("-") + result;
    }
    return result;
}

// API search type: show_float_fill_left : (Char, Int, Int, Float) -> String
// fwd bind count: 3
// Can be used to show floating point values in a specific precision
// left-padded with some character.
// (Float to String, Double to String etc.)
// Examples:
// const double pi = 3.14159
// show_float_fill_left<double>(' ', 8, 3, pi) == "   3.142"
// show_float_fill_left<double>(' ', 8, 6, pi) == "3.141590"
// show_float_fill_left<double>(' ', 8, 3, -pi) == "  -3.142"
// show_float_fill_left<double>(' ', 2, 3, -pi) == "-3.142"
template <typename T>
std::string show_float_fill_left(const std::string::value_type& filler,
        std::size_t min_size, std::size_t right_char_count, const T& x)
{
    return fill_left(filler, min_size, show_float<T>(0, right_char_count, x));
}

// API search type: show_fill_left : (Char, Int, a) -> String
// fwd bind count: 2
// Convert some value to a string with left-padded with some character.
// (Int to String etc.)
// Examples:
// show_fill_left<int>(' ', 4, 3) == "   3"
// show_fill_left<int>('0', 4, 3) == "0003"
// show_fill_left<int>(' ', 4, 12345) == "12345"
template <typename T>
std::string show_fill_left(
    const std::string::value_type& filler, std::size_t min_size, const T& x)
{
    return fill_left(filler, min_size, show<T>(x));
}

// API search type: show_fill_right : (Char, Int, a) -> String
// fwd bind count: 2
// Convert some value to a string with left-padded with some character.
// (Int to String etc.)
// Examples:
// show_fill_right<int>(' ', 4, 3) == "3   "
// show_fill_right<int>(' ', 4, 12345) == "12345"
template <typename T>
std::string show_fill_right(const std::string::value_type& filler,
    std::size_t min_size, const T& x)
{
    return fill_right(filler, min_size, show<T>(x));
}

} // namespace fplus

//
// string_tools.hpp
//

// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)



#include <cctype>
#include <string>
#include <locale>

namespace fplus
{

// API search type: is_letter_or_digit : Char -> Bool
// fwd bind count: 0
// Is character alphanumerical?
template <typename String>
bool is_letter_or_digit(const typename String::value_type& c)
{
    return
        std::isdigit(static_cast<unsigned char>(c)) ||
        std::isalpha(static_cast<unsigned char>(c));
}

// API search type: is_whitespace : Char -> Bool
// fwd bind count: 0
// Is character a whitespace.
template <typename String>
bool is_whitespace(const typename String::value_type& c)
{
    return (c == 32 || is_in_interval(9, 14, static_cast<int>(c)));
}

// API search type: is_line_break : Char -> Bool
// fwd bind count: 0
// Newline character ('\n')?
template <typename String>
bool is_line_break(const typename String::value_type& c)
{
    return c == '\n';
}

// API search type: clean_newlines : String -> String
// fwd bind count: 0
// Replaces windows and mac newlines with linux newlines.
template <typename String>
String clean_newlines(const String& str)
{
    return replace_elems('\r', '\n',
        replace_tokens(String("\r\n"), String("\n"), str));
}

// API search type: split_words : (Bool, String) -> [String]
// fwd bind count: 1
// Splits a string by non-letter and non-digit characters.
// split_words(false, "How are you?") == ["How", "are", "you"]
template <typename String, typename ContainerOut = std::vector<String>>
ContainerOut split_words(const bool allowEmpty, const String& str)
{
    return split_by(logical_not(is_letter_or_digit<String>), allowEmpty, str);
}

// API search type: split_lines : (Bool, String) -> [String]
// fwd bind count: 1
// Splits a string by the found newlines.
// split_lines(false, "Hi,\nhow are you?") == ["Hi,", "How are you"]
template <typename String, typename ContainerOut = std::vector<String>>
ContainerOut split_lines(bool allowEmpty, const String& str)
{
    return split_by(is_line_break<String>, allowEmpty, clean_newlines(str));
}

// API search type: trim_whitespace_left : String -> String
// fwd bind count: 0
// trim_whitespace_left("    text  ") == "text  "
template <typename String>
String trim_whitespace_left(const String& str)
{
    return drop_while(is_whitespace<String>, str);
}

// API search type: trim_whitespace_right : String -> String
// fwd bind count: 0
// Remove whitespace characters from the end of a string.
// trim_whitespace_right("    text  ") == "    text"
template <typename String>
String trim_whitespace_right(const String& str)
{
    return trim_right_by(is_whitespace<String>, str);
}

// API search type: trim_whitespace : String -> String
// fwd bind count: 0
// Remove whitespace characters from the beginning and the end of a string.
// trim_whitespace("    text  ") == "text"
template <typename String>
String trim_whitespace(const String& str)
{
    return trim_by(is_whitespace<String>, str);
}

// API search type: to_lower_case : String -> String
// fwd bind count: 0
// Convert a string to lowercase characters.
// to_lower_case("ChaRacTer&WorDs23") == "character&words23"
template <typename String>
String to_lower_case(const String& str)
{
    typedef typename String::value_type Char;
    return transform([](Char c) -> Char
        {
            return static_cast<Char>(
                std::tolower(static_cast<unsigned char>(c)));
        }, str);
}

// API search type: to_lower_case_loc : (Locale, String) -> String
// fwd bind count: 1
// Convert a string to lowercase characters using specified locale.
// to_upper_case_loc(locale("ru_RU.utf8"), "cYrIlLiC КиРиЛлИцА") == "cyrillic кириллица"
template <typename String>
String to_lower_case_loc(const std::locale &lcl, const String &str)
{
  typedef typename String::value_type Char;
  return transform([&lcl](Char c) -> Char
      {
        return static_cast<Char>(
            std::tolower(c, lcl));
      }, str);
}

// API search type: to_upper_case : String -> String
// fwd bind count: 0
// Convert a string to uppercase characters.
// to_upper_case("ChaRacTer&WorDs34") == "CHARACTER&WORDS34"
template <typename String>
String to_upper_case(const String& str)
{
    typedef typename String::value_type Char;
    return transform([](Char c) -> Char
        {
            return static_cast<Char>(
                std::toupper(static_cast<unsigned char>(c)));
        }, str);
}

// API search type: to_upper_case_loc : (Locale, String) -> String
// fwd bind count: 1
// Convert a string to uppercase characters using specified locale.
// to_upper_case_loc(locale("ru_RU.utf8"), "cYrIlLiC КиРиЛлИцА") == "CYRILLIC КИРИЛЛИЦА"
template <typename String>
String to_upper_case_loc(const std::locale &lcl, const String &str)
{
  typedef typename String::value_type Char;
  return transform([&lcl](Char c) -> Char
      {
        return static_cast<Char>(
            std::toupper(c, lcl));
      }, str);
}

// API search type: to_string_fill_left : (Char, Int, a) -> String
// fwd bind count: 2
// Convert a type right-aligned string using a fill character.
// to_string_fill_left('0', 5, 42) == "00042"
// to_string_fill_left(' ', 5, 42) == "   42"
template <typename T>
std::string to_string_fill_left(const std::string::value_type& filler,
        std::size_t min_size, const T& x)
{
    return fill_left(filler, min_size, std::to_string(x));
}

// API search type: to_string_fill_right : (Char, Int, a) -> String
// fwd bind count: 2
// Convert a type left-aligned string using a fill character.
// to_string_fill_right(' ', 5, 42) == "42   "
template <typename T>
std::string to_string_fill_right(const std::string::value_type& filler,
        std::size_t min_size, const T& x)
{
    return fill_right(filler, min_size, std::to_string(x));
}

} // namespace fplus

//
// tree.hpp
//

// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)



#include <vector>
#include <queue>

namespace fplus
{

template <typename T>
struct tree
{
    tree (const T& value, const std::vector<tree<T>>& children) :
        value_(value), children_(children) {}
    T value_;
    std::vector<tree<T>> children_;
};

namespace internal
{
template <typename T>
tree<T> make_singleton_tree(const T& x)
{
    return {x, {}};
}
} // namespace internal

namespace internal
{

template <typename BinaryPredicate, typename T>
std::vector<tree<T>> presort_trees(BinaryPredicate tree_is_child_of,
    std::vector<tree<T>> xs_orig)
{
    auto xs = fplus::convert_container<std::list<tree<T>>>(xs_orig);
    std::vector<tree<T>> result;
    while (!xs.empty())
    {
        for (auto it = std::begin(xs); it != std::end(xs);)
        {
            bool has_children = false;
            for (auto it_rest = std::begin(xs); it_rest != std::end(xs); ++it_rest)
            {
                if (it_rest != it && tree_is_child_of(*it_rest, *it))
                {
                    has_children = true;
                }
            }
            if (!has_children)
            {
                result.push_back(*it);
                it = xs.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
    return result;
}

template <typename BinaryPredicate, typename TreeCont> // todo: name?
TreeCont trees_from_sequence_helper(
    BinaryPredicate tree_is_child_of, TreeCont xs_unsorted)
{
    TreeCont result;
    auto xs = presort_trees(tree_is_child_of, xs_unsorted);
    for (auto it = std::begin(xs); it != std::end(xs); ++it)
    {
        const auto find_pred = bind_1st_of_2(tree_is_child_of, *it);
        auto it_find_begin = it;
        internal::advance_iterator(it_find_begin, 1);
        auto parent_it = std::find_if(it_find_begin, std::end(xs), find_pred);
        if (parent_it != std::end(xs))
        {
            parent_it->children_.push_back(*it);
        }
        else
        {
            result.push_back(*it);
        }
    }
    return result;
}

} // namespace internal

// API search type: trees_from_sequence : (((a, a) -> Bool), [a]) -> [Tree a]
// fwd bind count: 1
// Converts the sequence into a tree considering the given binary predicate.
template <typename BinaryPredicate, typename Container> // todo: name?
std::vector<tree<typename Container::value_type>> trees_from_sequence(
    BinaryPredicate is_child_of, const Container& xs)
{
    internal::check_binary_predicate_for_container<BinaryPredicate, Container>();
    typedef typename Container::value_type T;
    typedef tree<T> Tree;
    const auto singletons = transform_convert<std::vector<Tree>>(
        internal::make_singleton_tree<T>, xs);
    const auto tree_is_child_of =
        [is_child_of](const tree<T>& a, const tree<T>& b) -> bool
    {
        return is_child_of(a.value_, b.value_);
    };
    return internal::trees_from_sequence_helper(
        tree_is_child_of, std::move(singletons));
}

namespace internal
{

// -1 = a < b
//  0 = a == b
//  1 = b < a
template <typename T>
int tree_cmp(const tree<T>& a, const tree<T>& b)
{
    if(a.value_ < b.value_)
    {
        return -1;
    }
    else if(b.value_ < a.value_)
    {
        return 1;
    }
    else
    {
        const auto results = zip_with(tree_cmp<T>,
            sort_by(tree_cmp<T>, a.children_),
            sort_by(tree_cmp<T>, b.children_));
        return just_with_default(0, find_first_by(
                bind_1st_of_2(is_not_equal<int>, 0),
                results));
    }
}

template <typename T>
bool tree_less(const tree<T>& a, const tree<T>& b)
{
    return tree_cmp(a, b) < 0;
}

} // namespace internal

namespace internal
{

template <typename T>
bool are_normalized_trees_equal(const tree<T>& a, const tree<T>& b)
{
    if (a.value_ != b.value_ || a.children_.size() != b.children_.size())
    {
        return false;
    }
    else
    {
        return all(zip_with(are_normalized_trees_equal<T>,
            a.children_, b.children_));
    }
}

template <typename T>
tree<T> normalize_tree(tree<T> x)
{
    x.children_ = sort_by(
        internal::tree_less<T>,
        transform(normalize_tree<T>, x.children_));
    return x;
}

} // namespace internal

// API search type: are_trees_equal : (Tree a, Tree a) -> Bool
// fwd bind count: 1
template <typename T>
bool are_trees_equal(const tree<T>& a, const tree<T>& b)
{
    return internal::are_normalized_trees_equal(
        internal::normalize_tree(a), internal::normalize_tree(b));
}

// API search type: tree_size : Tree a -> Int
// fwd bind count: 0
// A tree with only one element (root) has size 1.
template <typename T>
std::size_t tree_size(const tree<T>& x)
{
    return 1 + sum(transform(tree_size<T>, x.children_));
}

// API search type: tree_depth : Tree a -> Int
// fwd bind count: 0
// A tree with only one element (root) has depth 1.
template <typename T>
std::size_t tree_depth(const tree<T>& x)
{
    return 1 + just_with_default<std::size_t>(0,
        maximum_maybe(transform(tree_depth<T>, x.children_)));
}

// API search type: flatten_tree_depth_first : Tree a -> [a]
// fwd bind count: 0
template <typename T>
std::vector<T> flatten_tree_depth_first(const tree<T>& x)
{
    return prepend_elem(x.value_,
        transform_and_concat(flatten_tree_depth_first<T>, x.children_));
}

// API search type: flatten_tree_breadth_first : Tree a -> [a]
// fwd bind count: 0
template <typename T>
std::vector<T> flatten_tree_breadth_first(const tree<T>& x)
{
    std::vector<T> result;
    result.reserve(tree_size(x));
    std::queue<const tree<T>*> q;
    q.push(&x);
    while (!q.empty())
    {
        const auto current = q.front();
        q.pop();
        result.push_back(current->value_);
        for (const auto& c : current->children_)
        {
            q.push(&c);
        }
    }
    return result;
}

} // namespace fplus

//
// side_effects.hpp
//

// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)



#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <fstream>
#include <functional>
#include <future>
#include <iostream>
#include <iterator>
#include <streambuf>
#include <string>
#include <thread>
#include <vector>

namespace fplus
{

// Executes a function f in a fixed interval,
// i.e. an average timespan between two consecutive calls of f,
// given in microseconds.
// f is a unary function, taking the time delta (in microseconds)
// between the last and the current call as its argument.
// In case of a delay outdated calls are be executed immediately.
// So the average executation time of f should be way shorter
// than the requested interval.
// Call ticker::start() to run.
// The ticker stops when ticker::stop() is called
// or the instance runs out of scope.
//
// Example usage:
//
// void say_hi(std::int64_t)
// {
//     std::cout << "hi " << std::endl;
// }
// int main()
// {
//     ticker hi_ticker(say_hi, 2 * 1000 * 1000);
//     hi_ticker.start();
//     std::this_thread::sleep_for(std::chrono::milliseconds(4500));
// }
class ticker
{
public:
    typedef std::function<void(std::int64_t)> function;
    ticker(const function& f, std::int64_t interval_us) :
        f_(f),
        interval_us_(interval_us),
        control_mutex_(),
        is_running_(false),
        thread_(),
        stop_mutex_()
    {
    }
    bool is_running()
    {
        std::lock_guard<std::mutex> lock(control_mutex_);
        return is_running_;
    }
    bool start()
    {
        std::lock_guard<std::mutex> lock(control_mutex_);
        if (is_running_)
            return false;
        stop_mutex_.lock();
        thread_ = std::thread([this]() { thread_function(); });
        is_running_ = true;
        return true;
    }
    bool stop()
    {
        std::lock_guard<std::mutex> lock(control_mutex_);
        if (!is_running_)
            return false;
        stop_mutex_.unlock();
        if (thread_.joinable())
        {
            thread_.join();
            thread_ = std::thread();
        }
        is_running_ = false;
        return true;
    }
    ~ticker()
    {
        stop();
    }
private:
    void thread_function()
    {
        auto last_wake_up_time = std::chrono::steady_clock::now();
        auto last_time = last_wake_up_time;
        bool quit = false;
        while (!quit)
        {
            const auto wake_up_time =
                last_wake_up_time + std::chrono::microseconds{ interval_us_ };
            const auto sleep_time =
                wake_up_time - std::chrono::steady_clock::now();
            if (stop_mutex_.try_lock_for(sleep_time))
            {
                stop_mutex_.unlock();
                quit = true;
            }
            const auto current_time = std::chrono::steady_clock::now();
            const auto elapsed = current_time - last_time;
            last_wake_up_time = wake_up_time;
            last_time = current_time;
            const auto elapsed_us =
                std::chrono::duration_cast<std::chrono::microseconds>(
                    elapsed).count();
            try
            {
                f_(elapsed_us);
            }
            catch (...)
            {
            }
        }
    }
    const function f_;
    const std::int64_t interval_us_;
    std::mutex control_mutex_;
    bool is_running_;
    std::thread thread_;
    std::timed_mutex stop_mutex_;
};


// API search type: sleep_for_n_seconds : Int -> Io ()
// Returns a function that suspends
// the calling thread for n seconds when executed.
inline
std::function<void()> sleep_for_n_seconds(std::size_t seconds)
{
    return [seconds]()
    {
        std::this_thread::sleep_for(std::chrono::seconds(seconds));
    };
}

// API search type: sleep_for_n_milliseconds : Int -> Io ()
// Returns a function that suspends
// the calling thread for n milliseconds when executed.
inline
std::function<void()> sleep_for_n_milliseconds(std::size_t milliseconds)
{
    return [milliseconds]()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    };
}

// API search type: sleep_for_n_microseconds : Int -> Io ()
// Returns a function that suspends
// the calling thread for n microseconds when executed.
inline
std::function<void()> sleep_for_n_microseconds(std::size_t microseconds)
{
    return [microseconds]()
    {
        std::this_thread::sleep_for(std::chrono::microseconds(microseconds));
    };
}

// API search type: execute_serially : [Io ()] -> Io ()
// Returns a function that executes
// the given side effects one after another when called.
template <typename Container>
auto execute_serially(const Container& effs)
{
    using Effect = typename Container::value_type;
    using Result = internal::invoke_result_t<Effect>;

    return [effs]
    {
        std::vector<std::decay_t<Result>> results;
        for (const Effect& e : effs)
        {
            results.push_back(internal::invoke(e));
        }
        return results;
    };
}

// API search type: execute_serially_until_success : [Io Bool] -> Io Bool
// Returns a function that (when called) executes
// the given side effects one after another until one of it returns true.
template <typename Container>
auto execute_serially_until_success(const Container& effs)
{
    using Effect = typename Container::value_type;
    using Result = internal::invoke_result_t<Effect>;
    static_assert(std::is_convertible<Result, bool>::value,
                  "Effects must return a boolish type.");
    return [effs]() -> bool
    {
        for (const Effect& e : effs)
        {
            if (internal::invoke(e))
            {
                return true;
            }
        }
        return false;
    };
}

// API search type: execute_and_return_fixed_value : (a, [Io b]) -> Io a
// Returns a function that executes the given side effect
// and returns a fixed value when called.
template <typename Result, typename Effect>
std::function<Result()> execute_and_return_fixed_value(
        Result result,
        Effect eff)
{
    return [eff, result]() -> Result
    {
        eff();
        return result;
    };
}

// Converts an arbitrary callable effect to an std::function.
template <typename Effect>
std::function<internal::invoke_result_t<Effect> ()> effect_to_std_function(Effect eff)
{
    return [eff]
    {
        return internal::invoke(eff);
    };
}

// API search type: execute_max_n_times_until_success : (Int, Io (), Int) -> Io Bool
// Returns a function that (when called) executes a side effect
// until it succeds once or the maximum number
// of attempts with an optional pause in between.
template <typename Effect>
auto execute_max_n_times_until_success(std::size_t n,
                                       const Effect& eff,
                                       std::size_t pause_in_milliseconds = 0)
{
    if (pause_in_milliseconds > 0)
    {
        auto sleep_and_return_false =
            execute_and_return_fixed_value(
                false,
                sleep_for_n_milliseconds(pause_in_milliseconds));
        return execute_serially_until_success(
            intersperse(
                sleep_and_return_false,
                replicate(n, effect_to_std_function(eff))));
    }
    return execute_serially_until_success(
        replicate(n, effect_to_std_function(eff)));
}

// API search type: execute_n_times : (Int, Io a) -> Io ()
// Returns a function that (when called) executes n times
// the provided side effect function. 
// The return values (if present) are dropped.
template<typename Effect>
auto execute_n_times(std::size_t n, const Effect& eff)
{
    for (auto _ : fplus::numbers(static_cast<size_t>(0), n))
    {
        (void) _; // suppress warning / unused variable
        eff();
    }
}

// API search type: execute_serially_until_failure : [Io Bool] -> Io Bool
// Returns a function that (when called) executes the given side effects
// one after another until one of them returns false.
template <typename Container>
std::function<bool()> execute_serially_until_failure(const Container& effs)
{
    using Effect = typename Container::value_type;
    using Result = internal::invoke_result_t<Effect>;
    static_assert(std::is_convertible<Result, bool>::value,
        "Effects must return a boolish type.");
    return [effs]() -> bool
    {
        for (const Effect& e : effs)
        {
            if (!internal::invoke(e))
            {
                return false;
            }
        }
        return true;
    };
}

// API search type: execute_parallelly : [Io a] -> Io [a]
// Returns a function that (when called) executes the given side effects
// in parallel (one thread each) and returns the collected results.
template <typename Container>
auto execute_parallelly(const Container& effs)
{
    return [effs] {
        // Bluntly re-using the transform implementation to execute side effects.
        return transform_parallelly([](const auto& eff) {
            return internal::invoke(eff);
        }, effs);
    };
}

// API search type: execute_parallelly_n_threads : (Int, [Io a]) -> Io [a]
// Returns a function that (when called) executes the given side effects
// in parallel (one thread each) and returns the collected results.
template <typename Container>
auto execute_parallelly_n_threads(std::size_t n, const Container& effs)
{
    return [n, effs] {
        // Bluntly re-using the transform implementation to execute side effects.
        return transform_parallelly_n_threads(n, [](const auto& eff) {
            return internal::invoke(eff);
        }, effs);
    };
}

// API search type: execute_fire_and_forget : Io a -> Io a
// Returns a function that (when called) executes the given side effect
// in a new thread and returns immediately.
template <typename Effect>
std::function<void()> execute_fire_and_forget(Effect eff)
{
    return [eff]()
    {
        std::thread t(eff);
        t.detach();
    };
}

// API search type: read_text_file_maybe : String -> Io (Maybe String)
// Returns a function that reads the content of a text file when called.
inline
std::function<maybe<std::string>()> read_text_file_maybe(
    const std::string& filename)
{
    return [filename]() -> maybe<std::string>
    {
        std::ifstream input(filename);
        if (!input.good())
            return {};
        return just(std::string(
                std::istreambuf_iterator<std::string::value_type>(input),
                std::istreambuf_iterator<std::string::value_type>()));
    };
}

// API search type: read_text_file : String -> Io String
// Returns a function that reads the content of a text file when called.
// This function then returns an empty string if the file could not be read.
inline
std::function<std::string()> read_text_file(const std::string& filename)
{
    return [filename]() -> std::string
    {
        return just_with_default(
            std::string(),

            read_text_file_maybe(filename)());
    };
}

// API search type: read_binary_file_maybe : String -> Io (Maybe [Int])
// Returns a function that reads the content of a binary file when executed.
inline
std::function<maybe<std::vector<std::uint8_t>>()> read_binary_file_maybe(
    const std::string& filename)
{
    return [filename]() -> maybe<std::vector<std::uint8_t>>
    {
        std::ifstream file(filename, std::ios::binary);
        if (!file.good())
            return {};
        file.unsetf(std::ios::skipws);
        std::streampos fileSize;
        file.seekg(0, std::ios::end);
        fileSize = file.tellg();
        if (fileSize == static_cast<std::streamsize>(0))
            return {};
        file.seekg(0, std::ios::beg);
        std::vector<std::uint8_t> vec(static_cast<std::size_t>(fileSize), 0);
        file.read(reinterpret_cast<char*>(&vec[0]), fileSize);
        return vec;
    };
}

// API search type: read_binary_file : String -> Io [Int]
// Returns a function that reads the content of a binary file when executed.
// This function then returns an empty vector if the file could not be read.
inline
std::function<std::vector<std::uint8_t>()> read_binary_file(
    const std::string& filename)
{
    return [filename]() -> std::vector<std::uint8_t>
    {
        return just_with_default(
            std::vector<std::uint8_t>(),
            read_binary_file_maybe(filename)());
    };
}

// API search type: read_text_file_lines_maybe : (String, Bool) -> Io (Maybe [String])
// Returns a function that (when called) reads the content of a text file
// and returns it line by line.
inline
std::function<maybe<std::vector<std::string>>()> read_text_file_lines_maybe(
        bool allow_empty, const std::string& filename)
{
    return [filename, allow_empty]() -> maybe<std::vector<std::string>>
    {
        const auto maybe_content = read_text_file_maybe(filename)();
        if (maybe_content.is_nothing())
            return {};
        else
            return split_lines(allow_empty, maybe_content.unsafe_get_just());
    };
}

// API search type: read_text_file_lines : (String, Bool) -> Io [String]
// Returns a function that (when called) reads the content of a text file
// and returns it line by line.
// This function then returns an empty vector if the file could not be read.
inline
std::function<std::vector<std::string>()> read_text_file_lines(
        bool allow_empty, const std::string& filename)
{
    return [filename, allow_empty]() -> std::vector<std::string>
    {
        return just_with_default(
            std::vector<std::string>(),
            read_text_file_lines_maybe(allow_empty, filename)());
    };
}

// API search type: write_text_file : (String, String) -> Io Bool
// Returns a function that (when called) writes content into a text file,
// replacing it if it already exists.
inline
std::function<bool()> write_text_file(const std::string& filename,
        const std::string& content)
{
    return [filename, content]() -> bool
    {
        std::ofstream output(filename);
        output << content;
        return output.good();
    };
}

// API search type: write_binary_file : (String, [Int]) -> Io Bool
// Returns a function that (when called) writes content into a binary file,
// replacing it if it already exists.
inline
std::function<bool()> write_binary_file(const std::string& filename,
        const std::vector<uint8_t>& content)
{
    return [filename, content]() -> bool
    {
        std::ofstream file(filename, std::ios::binary);
        file.write(reinterpret_cast<const char*>(&content[0]),
            static_cast<std::streamsize>(content.size()));
        return file.good();
    };
}

// API search type: write_text_file_lines : (String, [String], Bool) -> Io Bool
// Returns a function that (when called) writes lines into a text file,
// replacing it if it already exists.
inline
std::function<bool()> write_text_file_lines(bool trailing_newline,
        const std::string& filename,
        const std::vector<std::string>& lines)
{
    std::string content = join(std::string("\n"), lines);
    if (trailing_newline)
    {
        content += "\n";
    }
    return write_text_file(filename, content);
}

// API search type: execute_effect : Io a -> a
// Simply run a side effect (call a function without parameters)
// and returns the result.
// Can be useful for chaining.
template <typename F>
auto execute_effect(const F f)
{
    return internal::invoke(f);
}

// API search type: interact : (String -> String) -> Io ()
// Takes a function F of type (String -> String)
// and returns a function that
// reads the entire input from standard input,
// passes it through the given function,
// and writes the result to standard output.
template <typename F>
std::function<void()> interact(F f)
{
    return [f]() -> void
    {
        std::cout << f(std::string(
            std::istreambuf_iterator<char>(std::cin.rdbuf()),
            std::istreambuf_iterator<char>()));
    };
}

// API search type: execute_with_maybe : ((a -> void), Maybe a) -> Io Bool
// Returns a function that
// akes a unary side-effect function with
// a maybe holding a matching type
// and runs the sideeffect if the Maybe holds a just.
// The returned function returns false if the maybe was a nothing.
template <typename Effect, typename X>
std::function<bool()> execute_with_maybe(Effect eff, const maybe<X>& m)
{
    return [eff, m]() -> bool
    {
        if (m.is_nothing())
        {
            return false;
        }
        eff(m.unsafe_get_just());
        return true;
    };
}

} // namespace fplus

//
// stopwatch.hpp
//

// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)


#include <chrono>

namespace fplus
{

class stopwatch
{
public:
    stopwatch() : beg_(clock::now()) {}
    void reset() { beg_ = clock::now(); }

    // time since creation or last reset in seconds
    double elapsed() const {
        return std::chrono::duration_cast<second>
            (clock::now() - beg_).count(); }
private:
    typedef std::chrono::high_resolution_clock clock;
    typedef std::chrono::duration<double, std::ratio<1>> second;
    std::chrono::time_point<clock> beg_;
};

} // namespace fplus

//
// variant.hpp
//

// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)



#include <iostream>
#include <memory>
#include <tuple>

namespace fplus
{

namespace internal
{

    // http://stackoverflow.com/a/18987405/1866775

    template <typename...>
    struct is_one_of;

    template <typename F>
    struct is_one_of<F>
    {
        static constexpr bool value = false;
    };

    template <typename F, typename S, typename... T>
    struct is_one_of<F, S, T...>
    {
        static constexpr bool value = std::is_same<F, S>::value
            || is_one_of<F, T...>::value;
    };

    template <typename F, typename... T>
    struct is_one_of<F, std::tuple<T...>>
    {
        static constexpr bool value = is_one_of<F, T...>::value;
    };

    template <typename...>
    struct is_unique;

    template <>
    struct is_unique<> {
        static constexpr bool value = true;
    };

    template<typename F, typename... T>
    struct is_unique<F, T...>
    {
        static constexpr bool value = is_unique<T...>::value
            && !is_one_of<F, T...>::value;
    };

    template<typename... T>
    struct is_unique<std::tuple<T...>>
    {
        static constexpr bool value = is_unique<T...>::value;
    };

    template <typename...>
    struct are_same;

    template <>
    struct are_same<> {
        static constexpr bool value = true;
    };

    template<typename F1, typename F2>
    struct are_same<F1, F2>
    {
        static constexpr bool value = std::is_same<F1, F2>::value;
    };

    template<typename F1, typename F2, typename... T>
    struct are_same<F1, F2, T...>
    {
        static constexpr bool value = are_same<F2, T...>::value
            && std::is_same<F1, F2>::value;
    };

    template<typename... T>
    struct are_same<std::tuple<T...>>
    {
        static constexpr bool value = are_same<T...>::value;
    };

    // http://stackoverflow.com/a/3273571/1866775
    template<template<typename...> class List,
        template<typename> class Mod,
        typename ...Args>
    struct transform_parameter_pack {
        typedef List<typename Mod<Args>::type...> type;
    };

    template<typename T>
    struct as_shared_pointer
    {
        typedef std::shared_ptr<T> type;
    };



    // http://stackoverflow.com/a/27588263/1866775

    template <typename T, typename... Ts> struct get_index;

    template <typename T, typename... Ts>
    struct get_index<T, T, Ts...> : std::integral_constant<std::size_t, 0> {};

    template <typename T, typename Tail, typename... Ts>
    struct get_index<T, Tail, Ts...> :
        std::integral_constant<std::size_t, 1 + get_index<T, Ts...>::value> {};

    template <typename T>
    struct get_index<T>
    {
        // condition is always false, but should be dependant of T
        static_assert(sizeof(T) == 0, "element not found");
    };


    template <typename T, typename ... Ts>
    struct parameter_pack_head
    {
        typedef T type;
    };


    template <typename F>
    struct function_first_input_type
    {
        typedef typename std::remove_const<
            typename std::remove_reference<
                typename utils::function_traits<
                    F>::template arg<0>::type>::type>::type
            type;
    };

    template <typename F>
    struct unary_function_result_type
    {
        static_assert(utils::function_traits<F>::arity == 1,
                "Wrong arity.");
        typedef typename function_first_input_type<F>::type T;
        typedef std::decay_t<internal::invoke_result_t<F, T>> type;
    };


    // http://stackoverflow.com/a/42493805/1866775

    template <typename T>
    struct tag { };

    template <typename... Ts>
    struct type_set_eq_helper: tag<Ts>... { };

    template <typename, typename, typename = void>
    struct type_set_eq: std::false_type { };

    template <bool...>
    struct bool_pack { };

    template <bool... Bs>
    using my_and = std::is_same<bool_pack<Bs..., true>, bool_pack<true, Bs...>>;

    template <typename... Ts1, typename... Ts2>
    struct type_set_eq<std::tuple<Ts1...>, std::tuple<Ts2...>, typename std::enable_if< (sizeof...(Ts1) == sizeof...(Ts2)) && my_and< std::is_base_of<tag<Ts2>, type_set_eq_helper<Ts1...>>::value...  >::value  >::type  >:
       std::true_type { };


    // http://stackoverflow.com/a/42581257/1866775

    template <typename Lhs, typename Rhs>
    struct is_superset_of;

    template <typename Tuple, typename T, typename... Ts>
    struct is_superset_of<Tuple, std::tuple<T, Ts...>>
    {
        static const bool value = is_one_of<T, Tuple>::value && is_superset_of<Tuple, std::tuple<Ts...>>::value;
    };

    template <typename Tuple>
    struct is_superset_of<Tuple, std::tuple<>>
    {
        static const bool value = true;
    };


    // http://stackoverflow.com/a/36934374/1866775
    template<bool... bs>
    using all_true = std::is_same<bool_pack<bs..., true>, bool_pack<true, bs...>>;

} // namespace internal


template<typename ... Types>
struct variant
{
    static_assert(internal::is_unique<Types...>::value, "Types must be unique.");
    static_assert(internal::all_true<(!std::is_reference<Types>::value)...>::value, "No reference types allowed.");
    static_assert(internal::all_true<(!std::is_const<Types>::value)...>::value, "No const types allowed.");
    static_assert(sizeof...(Types) >= 1, "Please provide at least one type.");

    template <typename T>
    variant(const T& val) : shared_ptrs_({})
    {
        std::get<internal::get_index<T, Types...>::value>(shared_ptrs_) =
            std::make_shared<T>(val);
    }

    template <typename T>
    bool is() const
    {
        static_assert(
            internal::is_one_of<T, Types...>::value
            , "Type must match one possible variant type.");

        const auto ptr =
            std::get<internal::get_index<T, Types...>::value>(shared_ptrs_);

        return static_cast<bool>(ptr);
    }

    friend bool operator== (
        const variant<Types...>& a, const variant<Types...>& b)
    {
        return a.shared_ptrs_ == b.shared_ptrs_;
    }

    friend bool operator!= (
        const variant<Types...>& a, const variant<Types...>& b)
    {
        return a.shared_ptrs_ != b.shared_ptrs_;
    }

    template <typename F>
    auto visit_one(F f) const
    {
        using T = typename internal::function_first_input_type<F>::type;
        using Ret = internal::invoke_result_t<F, T>;
        internal::trigger_static_asserts<internal::unary_function_tag, F, T>();

        static_assert(
            internal::is_one_of<
                typename internal::function_first_input_type<F>::type,
                Types...>::value
            , "Function input must match one variant type.");

        static_assert(!std::is_same<std::decay_t<Ret>, void>::value,
                      "Function must return non-void type.");

        const auto ptr =
            std::get<internal::get_index<T, Types...>::value>(shared_ptrs_);

        if (ptr)
        {
            return just(internal::invoke(f, *ptr));
        }

        return nothing<std::decay_t<Ret>>();
    }

    template <typename ...Fs>
    auto visit(Fs ... fs) const ->
        typename internal::unary_function_result_type<
            typename internal::parameter_pack_head<Fs...>::type>::type
    {
        typedef typename internal::unary_function_result_type<
            typename internal::parameter_pack_head<Fs...>::type>::type
            Res;

        static_assert(
            sizeof...(Fs) >= std::tuple_size<shared_ptr_pack>::value,
            "Too few functions provided.");

        static_assert(
            sizeof...(Fs) <= std::tuple_size<shared_ptr_pack>::value,
            "Too many functions provided.");

        typedef typename internal::transform_parameter_pack<
            std::tuple,
            internal::unary_function_result_type,
            Fs...
            >::type return_types_tuple;

        typedef typename internal::transform_parameter_pack<
            std::tuple,
            internal::function_first_input_type,
            Fs...
            >::type function_first_input_types_tuple;

        static_assert(
            internal::is_unique<function_first_input_types_tuple>::value,
            "Only one function per input type allowed.");

        static_assert(
            internal::are_same<return_types_tuple>::value,
            "All Functions must return the same type.");

        static_assert(
            internal::type_set_eq<function_first_input_types_tuple, std::tuple<Types...>>::value,
            "Functions do not cover all possible types.");

        const auto results = justs(visit_helper<Res>(fs...));
        assert(size_of_cont(results) == 1);
        return head(results);
    }

    template <typename ...Fs>
    variant<Types...> transform(Fs ... fs) const
    {
        static_assert(
            sizeof...(Fs) >= std::tuple_size<shared_ptr_pack>::value,
            "Too few functions provided.");

        static_assert(
            sizeof...(Fs) <= std::tuple_size<shared_ptr_pack>::value,
            "Too many functions provided.");

        typedef typename internal::transform_parameter_pack<
            std::tuple,
            internal::unary_function_result_type,
            Fs...
            >::type return_types_tuple;

        typedef typename internal::transform_parameter_pack<
            std::tuple,
            internal::function_first_input_type,
            Fs...
            >::type function_first_input_types_tuple;

        static_assert(
            internal::type_set_eq<function_first_input_types_tuple, std::tuple<Types...>>::value,
            "Functions do not cover all possible types.");

        static_assert(
            internal::is_superset_of<std::tuple<Types...>, return_types_tuple>::value,
            "All Functions must return a possible variant type.");

        return visit(fs...);
    }

private:
    template <typename Res, typename F>
    std::vector<fplus::maybe<Res>> visit_helper(F f) const
    {
        return {visit_one(f)};
    }

    template <typename Res, typename F, typename ...Fs>
    std::vector<fplus::maybe<Res>> visit_helper(F f, Fs ... fs) const
    {
        return fplus::append(visit_helper<Res>(f), visit_helper<Res>(fs...));
    }

    typedef typename internal::transform_parameter_pack<
        std::tuple,
        internal::as_shared_pointer,
        Types...
    >::type shared_ptr_pack;
    shared_ptr_pack shared_ptrs_;
};

} // namespace fplus

//
// timed.hpp
//


#include <chrono>
#include <type_traits>

#include <cassert>
#include <exception>
#include <functional>
#include <memory>

namespace fplus
{
using ExecutionTime = double; // in seconds

// Holds a value of type T plus an execution time
template <typename T>
class timed : public std::pair<T, ExecutionTime>
{
    using base_pair = std::pair<T, ExecutionTime>;
public:
    timed() : base_pair() {}
    timed(const T& val, ExecutionTime t = 0.) : base_pair(val, t) {}

    // Execution time in seconds (returns a double)
    ExecutionTime time_in_s() const { return base_pair::second; }

    // Execution time as a std::chrono::duration<double>
    std::chrono::duration<double, std::ratio<1>> duration_in_s() const
    {
        return std::chrono::duration<double, std::ratio<1>>(time_in_s());
    }

    // Inner value
    const T& get() const            { return base_pair::first; }
    T& get()                        { return base_pair::first; }
};

// API search type: show_timed : Timed a -> String
// fwd bind count: 0
// show_timed((42,1)) -> "42 (1000ms)"
template <typename T>
std::string show_timed(const fplus::timed<T>& v)
{
    std::string result =
        fplus::show(v.get()) + " (" + fplus::show(v.time_in_s() * 1000.) + "ms)";
    return result;
}

namespace internal
{
    template<typename Fn>
    class timed_function_impl
    {
    public:
        explicit timed_function_impl(Fn fn) : _fn(fn) {};
        template<typename ...Args> auto operator()(Args&&... args) 
        { 
            return _timed_result(std::forward<Args>(args)...); 
        }

    private:
        template<typename ...Args>
        auto _timed_result(Args&&... args)
        {
            fplus::stopwatch timer;
            auto r = _fn(std::forward<Args>(args)...);
            auto r_t = fplus::timed<decltype(r)>(r, timer.elapsed());
            return r_t;
        }

        Fn _fn;
    };
}

// API search type: make_timed_function : ((a -> b)) -> (a -> Timed b)
// fwd bind count: 0
// Transforms a function into a timed / benchmarked version of the same function.
// -
// Example:
// -
// using Ints = std::vector<int>;
// Ints ascending_numbers = fplus::numbers(0, 1000);
// Ints shuffled_numbers = fplus::shuffle(std::mt19937::default_seed, ascending_numbers);
// auto sort_func = [](const Ints& values) { return fplus::sort(values); };
// auto sort_bench = fplus::make_timed_function(sort_func);
// auto sorted_numbers = sort_bench(shuffled_numbers);
// assert(sorted_numbers.get() == ascending_numbers); // sorted_numbers.get() <=> actual output
// assert(sorted_numbers.time_in_s() < 0.1); // // sorted_numbers.time_in_s() <=> execution time
template<class Fn>
auto make_timed_function(Fn f)
{
    return internal::timed_function_impl<decltype(f)>(f);
}

namespace internal
{
    template<typename Fn>
    class timed_void_function_impl
    {
    public:
        explicit timed_void_function_impl(Fn fn) : _fn(fn) {};
        template<typename ...Args> auto operator()(Args&&... args)
        {
            return _timed_result(std::forward<Args>(args)...);
        }

    private:
        template<typename ...Args>
        auto _timed_result(Args&&... args)
        {
            fplus::stopwatch timer;
            _fn(std::forward<Args>(args)...);
            return timer.elapsed();
        }

        Fn _fn;
    };

}

// API search type: make_timed_void_function : ((a -> Void)) -> (a -> Double)
// fwd bind count: 0
// Transforms a void function into a timed / benchmarked version of the same function.
// -
// Example:
// -
// void foo() { std::this_thread::sleep_for(std::chrono::milliseconds(1000)); }
// ...
// auto foo_bench = make_timed_void_function(foo);
// auto r = foo_bench();
// double run_time = foo_bench(); // in seconds
template<class Fn>
auto make_timed_void_function(Fn f)
{
    return internal::timed_void_function_impl<decltype(f)>(f);
}

}

//
// benchmark_session.hpp
//

// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <vector>
#include <mutex>


namespace fplus
{
using FunctionName = std::string;
struct benchmark_function_report
{
    std::size_t nb_calls;
    ExecutionTime total_time;
    ExecutionTime average_time;
    ExecutionTime deviation;
};


namespace internal
{
    std::string show_benchmark_function_report(
        const std::map<FunctionName, benchmark_function_report> & reports);
}


// benchmark_session stores timings during a benchmark session
// and is able to emit a report at the end
class benchmark_session
{
public:
    benchmark_session() : functions_times_mutex_(), functions_times_() {};

    // report() shall return a string with a summary of the session
    // Example below:
    // Function              |Nb calls|Total time|Av. time|Deviation|
    // ----------------------+--------+----------+--------+---------+
    // convert_charset_string|    4000|   4.942ms| 1.236us|  1.390us|
    // split_lines           |    1000|   4.528ms| 4.528us|  1.896us|
    inline std::string report() const
    {
        const auto reports = report_list();
        return fplus::internal::show_benchmark_function_report(reports);
    }

    std::map<FunctionName, benchmark_function_report> report_list() const
    {
        std::lock_guard<std::mutex> lock(functions_times_mutex_);
        std::map<FunctionName, benchmark_function_report> report;
        for (const auto & one_function_time : functions_times_)
        {
            report[one_function_time.first] = make_bench_report(one_function_time.second);
        }
        return report;
    }

    inline void store_one_time(const FunctionName & function_name, ExecutionTime time)
    {
        std::lock_guard<std::mutex> lock(functions_times_mutex_);
        functions_times_[function_name].push_back(time);
    }

private:
    benchmark_function_report make_bench_report(
        const std::vector<ExecutionTime> & times) const
    {
        benchmark_function_report result;
        result.nb_calls = times.size();
        auto mean_and_dev = fplus::mean_stddev<double>(times);
        result.average_time = mean_and_dev.first;
        result.deviation = mean_and_dev.second;
        result.total_time = fplus::sum(times);
        return result;
    }

    mutable std::mutex functions_times_mutex_;
    std::map<FunctionName, std::vector<ExecutionTime>> functions_times_;
};

namespace internal
{
    template<typename Fn>
    class bench_function_impl
    {
    public:
        explicit bench_function_impl(
            benchmark_session & benchmark_sess,
            FunctionName function_name,
            Fn fn)
            : benchmark_session_(benchmark_sess)
            , function_name_(function_name)
            , fn_(fn)
        {};

        template<typename ...Args> auto operator()(Args&&... args)
        {
            return _bench_result(std::forward<Args>(args)...);
        }

    private:
        template<typename ...Args>
        auto _bench_result(Args&&... args)
        {
            fplus::stopwatch timer;
            auto r = fn_(std::forward<Args>(args)...);
            benchmark_session_.store_one_time(function_name_, timer.elapsed());
            return r;
        }

        benchmark_session & benchmark_session_;
        FunctionName function_name_;
        Fn fn_;
    };

    template<typename Fn>
    class bench_void_function_impl
    {
    public:
        explicit bench_void_function_impl(
            benchmark_session & benchmark_sess,
            FunctionName function_name,
            Fn fn)
            : benchmark_session_(benchmark_sess)
            , function_name_(function_name)
            , fn_(fn)
        {};

        template<typename ...Args> auto operator()(Args&&... args)
        {
            _bench_result(std::forward<Args>(args)...);
        }

    private:
        template<typename ...Args>
        auto _bench_result(Args&&... args)
        {
            fplus::stopwatch timer;
            fn_(std::forward<Args>(args)...);
            benchmark_session_.store_one_time(function_name_, timer.elapsed());
        }

        benchmark_session & benchmark_session_;
        FunctionName function_name_;
        Fn fn_;
    };

} // namespace internal


// API search type: make_benchmark_function : (benchmark_session, string, (a... -> b)) -> (a... -> b)
// Transforms a function into a function with the *same* signature
// and behavior, except that it also stores stats into the benchmark session (first parameter),
// under the name given by the second parameter.
// -
// Notes:
// Side effects: make_benchmark_function *will add side effects* to the function, since it stores data
// into the benchmark session at each call.
// If you intend to benchmark only one function, prefer to use the simpler "make_timed_function"
// Use "make_benchmark_void_function" if your function returns void
// -
// Example of a minimal benchmark session (read benchmark_session_test.cpp for a full example)
//     fplus::benchmark_session benchmark_sess;
//     void foo() {
//         auto add_bench = fplus::make_benchmark_function(benchmark_sess, "add", add);
//         auto printf_bench = fplus::make_benchmark_void_function(benchmark_sess, "printf", printf);
//         int forty_five = add_bench(20, add_bench(19, 6));
//         int forty_two = benchmark_expression(benchmark_sess, "sub", forty_five - 3);
//         printf_bench("forty_two is %i\n", forty_two);
//     }
//     int main() {
//         foo();
//         std::cout << benchmark_sess.report();
//     }
// This will output a report like this
// Function|Nb calls|Total time|Av. time|Deviation|
// --------+--------+----------+--------+---------+
// printf  |       1|   0.010ms| 9.952us|  0.000us|
// add     |       2|   0.000ms| 0.050us|  0.009us|
// sub     |       1|   0.000ms| 0.039us|  0.000us|
// -
// As an alternative to make_benchmark_function, you can also benchmark an expression.
// For example, in order to benchmark the following line:
//     auto sorted = fplus::sort(my_vector);
// Just copy/paste this expression into "bench_expression" like shown below: this expression
// will then be benchmarked with the name "sort_my_vector"
//     auto sorted = benchmark_expression(
//         my_benchmark_session,
//         "sort_my_vector",
//         fplus::sort(my_vector);
//     );
// Notes :
// benchmark_expression is a preprocessor macro that uses an immediately invoked lambda (IIL).
// The expression can be copy-pasted with no modification, and it is possible to not remove the ";"
// (although it also works if it is not present)
// You can also benchmark an expression that returns void using benchmark_void_expression
template<class Fn>
auto make_benchmark_function(benchmark_session & session, const FunctionName & name, Fn f)
{
    // transforms f into a function with the same
    // signature, that will store timings into the benchmark session
    return internal::bench_function_impl<Fn>(session, name, f);
}


// API search type: make_benchmark_void_function : (benchmark_session, string, (a... -> Void)) -> (a... -> Void)
// Transforms a function that returns a void into a function with the *same* signature
// and behavior, except that it also stores stats into the benchmark session (first parameter),
// under the name given by the second parameter
// Note that make_benchmark_void_function *will add side effects* to the function
// (since it stores data into the benchmark session at each call)
// -
// Example:
//     benchmark_session bench_session;
//     ...
//     void foo() {
//         std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//     }
//     ...
//     auto foo_bench = make_benchmark_void_function(bench_session, "foo", foo);
//     foo_bench();
//     ...
//     std::cout << benchmark_session.report();
template<class Fn>
auto make_benchmark_void_function(benchmark_session & session, const FunctionName & name, Fn f)
{
    // transforms a void returning function into a function with the same
    // signature, that will store timings into the benchmark session
    return internal::bench_void_function_impl<Fn>(session, name, f);
}

#define benchmark_expression(bench_session, name, expression)      \
make_benchmark_function(                                           \
    bench_session,                                                 \
    name,                                                          \
    [&]() { return expression; }                                   \
)();

#define benchmark_void_expression(bench_session, name, expression) \
make_benchmark_void_function(                                      \
    bench_session,                                                 \
    name,                                                          \
    [&]() { expression; }                                          \
)();


namespace internal
{
    inline std::string show_table(const std::vector<std::vector<std::string>>& rows)
    {
        if (rows.empty() || rows[0].empty())
            return "";

        const std::vector<std::size_t> columns_width = [&]() {
            auto string_size = [](const std::string & s) -> std::size_t { return s.size(); };
            auto largest_string_size = [&](const std::vector<std::string> & strings) -> std::size_t {
                return string_size(fplus::maximum_on(string_size, strings));
            };
            return fplus::transform(largest_string_size, fplus::transpose(rows));
        }();

        auto show_one_element = [](const std::pair<std::string, std::size_t> & elem_and_width) {
            const std::string & element = elem_and_width.first;
            const auto col_width = elem_and_width.second;
            bool is_number = element.size() > 0 && isdigit(element[0]);
            if (is_number)
                return fplus::show_fill_left(' ', col_width, element) + "|";
            else
                return fplus::show_fill_right(' ', col_width, element) + "|";
        };

        auto show_one_separator = [](std::size_t col_width) {
            return fplus::show_fill_left('-', col_width, "") + "+";
        };

        auto show_one_row = [&](const std::vector<std::string> & row) {
            return fplus::sum(fplus::transform(
                show_one_element,
                fplus::zip(row, columns_width)));
        };

        auto firstrow_separator = fplus::sum(fplus::transform(show_one_separator, columns_width));
        auto rows_formatted = fplus::transform(show_one_row, rows);
        auto rows_separated = fplus::insert_at_idx(1, firstrow_separator, rows_formatted);
        return fplus::join( std::string("\n"), rows_separated) + "\n";
    }

    inline std::vector< std::pair<FunctionName, benchmark_function_report> > make_ordered_reports(
        const std::map<FunctionName, benchmark_function_report> & report_map)
    {
        auto report_pairs = fplus::map_to_pairs(report_map);
        auto report_pairs_sorted = fplus::sort_by([](const auto &a, const auto &b) {
            return a.second.total_time > b.second.total_time;
        }, report_pairs);
        return report_pairs_sorted;
    }

    inline std::string show_benchmark_function_report(const std::map<FunctionName, benchmark_function_report> & reports)
    {
        auto ordered_reports = make_ordered_reports(reports);
        auto my_show_time_ms = [](double time) -> std::string {
            std::stringstream ss;
            ss << std::fixed << std::setprecision(3);
            ss << (time * 1000.);
            return ss.str() + "ms";
        };
        auto my_show_time_us = [](double time) -> std::string {
            std::stringstream ss;
            ss << std::fixed << std::setprecision(3);
            ss << (time * 1000000.);
            return ss.str() + "us";
        };

        std::vector<std::string> header_row{ {
                "Function", "Nb calls", "Total time", "Av. time", "Deviation"
            } };
        auto value_rows = fplus::transform([&](const auto & kv) {
                const auto & report = kv.second;
                const auto & function_name = kv.first;
                std::vector<std::string> row;
                row.push_back(function_name);
                row.push_back(fplus::show(report.nb_calls));
                row.push_back(my_show_time_ms(report.total_time));
                row.push_back(my_show_time_us(report.average_time));
                row.push_back(my_show_time_us(report.deviation));
                return row;
            },
            ordered_reports);

        return fplus::internal::show_table(fplus::insert_at_idx(0, header_row, value_rows));
    }
} // namespace internal

}


//
// curry.hpp
//

// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)


namespace fplus
{
namespace curry
{

// Currying.
// Allow to generically bind parameters one by one.

#define fplus_curry_define_fn_0(fplus_curry_define_fn_0_name) \
inline auto fplus_curry_define_fn_0_name() \
{ \
    return [](auto&& fplus_curry_p1) \
    { \
        return fplus::fplus_curry_define_fn_0_name(std::forward<decltype(fplus_curry_p1)>(fplus_curry_p1)); \
    }; \
}

#define fplus_curry_define_fn_1(fplus_curry_define_fn_1_name) \
template <typename P1> \
auto fplus_curry_define_fn_1_name(P1 p1) \
{ \
    return [p1](auto&& fplus_curry_p2) \
    { \
        return fplus::fplus_curry_define_fn_1_name(p1, std::forward<decltype(fplus_curry_p2)>(fplus_curry_p2)); \
    }; \
}

#define fplus_curry_define_fn_2(fplus_curry_define_fn_2_name) \
template <typename P1> \
auto fplus_curry_define_fn_2_name(P1 p1) \
{ \
    return [p1](const auto& fplus_curry_p2) \
    { \
        return [p1, fplus_curry_p2](auto&& fplus_curry_p3) \
        { \
            return fplus::fplus_curry_define_fn_2_name(p1, fplus_curry_p2, std::forward<decltype(fplus_curry_p3)>(fplus_curry_p3)); \
        }; \
    }; \
}

#define fplus_curry_define_fn_3(fplus_curry_define_fn_3_name) \
template <typename P1> \
auto fplus_curry_define_fn_3_name(P1 p1) \
{ \
    return [p1](const auto& fplus_curry_p2) \
    { \
        return [p1, fplus_curry_p2](const auto& fplus_curry_p3) \
        { \
            return [p1, fplus_curry_p2, fplus_curry_p3](auto&& fplus_curry_p4) \
            { \
                return fplus::fplus_curry_define_fn_3_name(p1, fplus_curry_p2, fplus_curry_p3, std::forward<decltype(fplus_curry_p4)>(fplus_curry_p4)); \
            }; \
        }; \
    }; \
}

#define fplus_curry_define_fn_4(fplus_curry_define_fn_4_name) \
template <typename P1> \
auto fplus_curry_define_fn_4_name(P1 p1) \
{ \
return [p1](const auto& fplus_curry_p2) \
    { \
        return [p1, fplus_curry_p2](const auto& fplus_curry_p3) \
        { \
            return [p1, fplus_curry_p2, fplus_curry_p3](const auto& fplus_curry_p4) \
            { \
                return [p1, fplus_curry_p2, fplus_curry_p3, fplus_curry_p4](auto&& fplus_curry_p5) \
                { \
                    return fplus::fplus_curry_define_fn_4_name(p1, fplus_curry_p2, fplus_curry_p3, fplus_curry_p4, std::forward<decltype(fplus_curry_p5)>(fplus_curry_p5)); \
                }; \
            }; \
        }; \
    }; \
}



//
// curry_instances.autogenerated_defines
//

// THIS FILE WAS GENERATED AUTOMATICALLY. DO NOT EDIT.
fplus_curry_define_fn_0(identity)
fplus_curry_define_fn_1(is_equal)
fplus_curry_define_fn_1(is_not_equal)
fplus_curry_define_fn_1(is_less)
fplus_curry_define_fn_1(is_less_or_equal)
fplus_curry_define_fn_1(is_greater)
fplus_curry_define_fn_1(is_greater_or_equal)
fplus_curry_define_fn_1(xor_bools)
fplus_curry_define_fn_0(is_just)
fplus_curry_define_fn_0(is_nothing)
fplus_curry_define_fn_0(unsafe_get_just)
fplus_curry_define_fn_0(just_with_default)
fplus_curry_define_fn_1(throw_on_nothing)
fplus_curry_define_fn_0(just)
fplus_curry_define_fn_1(as_just_if)
fplus_curry_define_fn_0(maybe_to_seq)
fplus_curry_define_fn_0(singleton_seq_as_maybe)
fplus_curry_define_fn_1(lift_maybe)
fplus_curry_define_fn_2(lift_maybe_def)
fplus_curry_define_fn_2(lift_maybe_2)
fplus_curry_define_fn_3(lift_maybe_2_def)
fplus_curry_define_fn_1(and_then_maybe)
fplus_curry_define_fn_0(flatten_maybe)
fplus_curry_define_fn_0(is_empty)
fplus_curry_define_fn_0(is_not_empty)
fplus_curry_define_fn_0(size_of_cont)
fplus_curry_define_fn_0(convert)
fplus_curry_define_fn_0(convert_elems)
fplus_curry_define_fn_0(convert_container)
fplus_curry_define_fn_0(convert_container_and_elems)
fplus_curry_define_fn_2(get_segment)
fplus_curry_define_fn_2(set_segment)
fplus_curry_define_fn_2(remove_segment)
fplus_curry_define_fn_2(insert_at)
fplus_curry_define_fn_1(elem_at_idx)
fplus_curry_define_fn_1(elem_at_idx_maybe)
fplus_curry_define_fn_1(elems_at_idxs)
fplus_curry_define_fn_1(transform)
fplus_curry_define_fn_1(transform_convert)
fplus_curry_define_fn_1(transform_inner)
fplus_curry_define_fn_0(reverse)
fplus_curry_define_fn_1(take)
fplus_curry_define_fn_1(take_exact)
fplus_curry_define_fn_1(take_cyclic)
fplus_curry_define_fn_1(drop)
fplus_curry_define_fn_1(take_last)
fplus_curry_define_fn_1(drop_last)
fplus_curry_define_fn_1(drop_exact)
fplus_curry_define_fn_1(take_while)
fplus_curry_define_fn_1(take_last_while)
fplus_curry_define_fn_1(drop_while)
fplus_curry_define_fn_1(drop_last_while)
fplus_curry_define_fn_2(fold_left)
fplus_curry_define_fn_2(reduce)
fplus_curry_define_fn_1(fold_left_1)
fplus_curry_define_fn_1(reduce_1)
fplus_curry_define_fn_2(fold_right)
fplus_curry_define_fn_1(fold_right_1)
fplus_curry_define_fn_2(scan_left)
fplus_curry_define_fn_1(scan_left_1)
fplus_curry_define_fn_2(scan_right)
fplus_curry_define_fn_1(scan_right_1)
fplus_curry_define_fn_0(sum)
fplus_curry_define_fn_0(product)
fplus_curry_define_fn_1(append_elem)
fplus_curry_define_fn_1(prepend_elem)
fplus_curry_define_fn_1(append)
fplus_curry_define_fn_1(append_convert)
fplus_curry_define_fn_0(concat)
fplus_curry_define_fn_1(interweave)
fplus_curry_define_fn_0(unweave)
fplus_curry_define_fn_1(sort_by)
fplus_curry_define_fn_1(sort_on)
fplus_curry_define_fn_0(sort)
fplus_curry_define_fn_1(stable_sort_by)
fplus_curry_define_fn_1(stable_sort_on)
fplus_curry_define_fn_0(stable_sort)
fplus_curry_define_fn_2(partial_sort_by)
fplus_curry_define_fn_2(partial_sort_on)
fplus_curry_define_fn_1(partial_sort)
fplus_curry_define_fn_2(nth_element_by)
fplus_curry_define_fn_2(nth_element_on)
fplus_curry_define_fn_1(nth_element)
fplus_curry_define_fn_1(unique_by)
fplus_curry_define_fn_1(unique_on)
fplus_curry_define_fn_0(unique)
fplus_curry_define_fn_1(intersperse)
fplus_curry_define_fn_1(join)
fplus_curry_define_fn_1(join_elem)
fplus_curry_define_fn_1(is_elem_of_by)
fplus_curry_define_fn_1(is_elem_of)
fplus_curry_define_fn_1(nub_by)
fplus_curry_define_fn_1(nub_on)
fplus_curry_define_fn_0(nub)
fplus_curry_define_fn_1(all_unique_by_eq)
fplus_curry_define_fn_1(all_unique_on)
fplus_curry_define_fn_0(all_unique)
fplus_curry_define_fn_1(is_strictly_sorted_by)
fplus_curry_define_fn_1(is_strictly_sorted_on)
fplus_curry_define_fn_0(is_strictly_sorted)
fplus_curry_define_fn_1(is_sorted_by)
fplus_curry_define_fn_1(is_sorted_on)
fplus_curry_define_fn_0(is_sorted)
fplus_curry_define_fn_1(is_prefix_of)
fplus_curry_define_fn_1(is_suffix_of)
fplus_curry_define_fn_1(all_by)
fplus_curry_define_fn_0(all)
fplus_curry_define_fn_1(all_the_same_by)
fplus_curry_define_fn_1(all_the_same_on)
fplus_curry_define_fn_0(all_the_same)
fplus_curry_define_fn_2(numbers_step)
fplus_curry_define_fn_1(numbers)
fplus_curry_define_fn_0(singleton_seq)
fplus_curry_define_fn_0(all_idxs)
fplus_curry_define_fn_0(init)
fplus_curry_define_fn_0(tail)
fplus_curry_define_fn_0(head)
fplus_curry_define_fn_0(last)
fplus_curry_define_fn_0(mean_stddev)
fplus_curry_define_fn_1(count_occurrences_by)
fplus_curry_define_fn_0(count_occurrences)
fplus_curry_define_fn_2(lexicographical_less_by)
fplus_curry_define_fn_1(lexicographical_less)
fplus_curry_define_fn_0(lexicographical_sort)
fplus_curry_define_fn_1(replicate)
fplus_curry_define_fn_2(instead_of_if)
fplus_curry_define_fn_2(instead_of_if_empty)
fplus_curry_define_fn_0(is_ok)
fplus_curry_define_fn_0(is_error)
fplus_curry_define_fn_0(unsafe_get_ok)
fplus_curry_define_fn_0(unsafe_get_error)
fplus_curry_define_fn_1(ok_with_default)
fplus_curry_define_fn_0(ok)
fplus_curry_define_fn_0(error)
fplus_curry_define_fn_0(to_maybe)
fplus_curry_define_fn_1(from_maybe)
fplus_curry_define_fn_1(throw_on_error)
fplus_curry_define_fn_1(lift_result)
fplus_curry_define_fn_2(lift_result_both)
fplus_curry_define_fn_2(unify_result)
fplus_curry_define_fn_1(and_then_result)
fplus_curry_define_fn_1(keep_if)
fplus_curry_define_fn_1(drop_if)
fplus_curry_define_fn_1(without)
fplus_curry_define_fn_1(without_any)
fplus_curry_define_fn_1(keep_if_with_idx)
fplus_curry_define_fn_1(drop_if_with_idx)
fplus_curry_define_fn_1(keep_by_idx)
fplus_curry_define_fn_1(drop_by_idx)
fplus_curry_define_fn_1(keep_idxs)
fplus_curry_define_fn_1(drop_idxs)
fplus_curry_define_fn_1(drop_idx)
fplus_curry_define_fn_0(justs)
fplus_curry_define_fn_0(oks)
fplus_curry_define_fn_0(errors)
fplus_curry_define_fn_1(trim_left)
fplus_curry_define_fn_1(trim_token_left)
fplus_curry_define_fn_1(trim_right_by)
fplus_curry_define_fn_1(trim_right)
fplus_curry_define_fn_1(trim_token_right)
fplus_curry_define_fn_1(trim_by)
fplus_curry_define_fn_1(trim)
fplus_curry_define_fn_1(trim_token)
fplus_curry_define_fn_1(adjacent_keep_snd_if)
fplus_curry_define_fn_1(adjacent_drop_fst_if)
fplus_curry_define_fn_1(adjacent_drop_snd_if)
fplus_curry_define_fn_1(adjacent_keep_fst_if)
fplus_curry_define_fn_1(generate_by_idx)
fplus_curry_define_fn_1(repeat)
fplus_curry_define_fn_1(infixes)
fplus_curry_define_fn_3(carthesian_product_with_where)
fplus_curry_define_fn_2(carthesian_product_with)
fplus_curry_define_fn_2(carthesian_product_where)
fplus_curry_define_fn_1(carthesian_product)
fplus_curry_define_fn_1(carthesian_product_n)
fplus_curry_define_fn_1(permutations)
fplus_curry_define_fn_1(combinations)
fplus_curry_define_fn_1(combinations_with_replacement)
fplus_curry_define_fn_0(power_set)
fplus_curry_define_fn_2(iterate)
fplus_curry_define_fn_1(iterate_maybe)
fplus_curry_define_fn_1(adjacent_difference_by)
fplus_curry_define_fn_0(adjacent_difference)
fplus_curry_define_fn_0(rotate_left)
fplus_curry_define_fn_0(rotate_right)
fplus_curry_define_fn_0(rotations_left)
fplus_curry_define_fn_0(rotations_right)
fplus_curry_define_fn_2(fill_left)
fplus_curry_define_fn_2(fill_right)
fplus_curry_define_fn_0(inits)
fplus_curry_define_fn_0(tails)
fplus_curry_define_fn_1(apply_to_pair)
fplus_curry_define_fn_2(zip_with)
fplus_curry_define_fn_3(zip_with_3)
fplus_curry_define_fn_4(zip_with_defaults)
fplus_curry_define_fn_1(zip)
fplus_curry_define_fn_1(zip_repeat)
fplus_curry_define_fn_0(unzip)
fplus_curry_define_fn_0(fst)
fplus_curry_define_fn_0(snd)
fplus_curry_define_fn_1(transform_fst)
fplus_curry_define_fn_1(transform_snd)
fplus_curry_define_fn_2(transform_pair)
fplus_curry_define_fn_0(swap_pair_elems)
fplus_curry_define_fn_0(swap_pairs_elems)
fplus_curry_define_fn_0(adjacent_pairs)
fplus_curry_define_fn_0(overlapping_pairs)
fplus_curry_define_fn_0(overlapping_pairs_cyclic)
fplus_curry_define_fn_0(enumerate)
fplus_curry_define_fn_4(inner_product_with)
fplus_curry_define_fn_2(inner_product)
fplus_curry_define_fn_2(first_mismatch_idx_by)
fplus_curry_define_fn_2(first_mismatch_by)
fplus_curry_define_fn_2(first_mismatch_idx_on)
fplus_curry_define_fn_2(first_mismatch_on)
fplus_curry_define_fn_2(first_mismatch_idx)
fplus_curry_define_fn_2(first_mismatch)
fplus_curry_define_fn_2(first_match_idx_by)
fplus_curry_define_fn_2(first_match_by)
fplus_curry_define_fn_2(first_match_idx_on)
fplus_curry_define_fn_2(first_match_on)
fplus_curry_define_fn_2(first_match_idx)
fplus_curry_define_fn_2(first_match)
fplus_curry_define_fn_2(is_in_interval)
fplus_curry_define_fn_2(is_in_interval_around)
fplus_curry_define_fn_2(is_in_open_interval)
fplus_curry_define_fn_2(is_in_open_interval_around)
fplus_curry_define_fn_2(is_in_closed_interval)
fplus_curry_define_fn_4(reference_interval)
fplus_curry_define_fn_2(clamp)
fplus_curry_define_fn_0(is_negative)
fplus_curry_define_fn_0(is_positive)
fplus_curry_define_fn_0(is_even)
fplus_curry_define_fn_0(is_odd)
fplus_curry_define_fn_0(abs)
fplus_curry_define_fn_1(abs_diff)
fplus_curry_define_fn_0(square)
fplus_curry_define_fn_0(cube)
fplus_curry_define_fn_0(sign)
fplus_curry_define_fn_0(sign_with_zero)
fplus_curry_define_fn_0(integral_cast_throw)
fplus_curry_define_fn_0(integral_cast_clamp)
fplus_curry_define_fn_0(round)
fplus_curry_define_fn_0(floor)
fplus_curry_define_fn_1(floor_to_int_mult)
fplus_curry_define_fn_1(ceil_to_int_mult)
fplus_curry_define_fn_0(ceil)
fplus_curry_define_fn_1(int_power)
fplus_curry_define_fn_2(min_2_on)
fplus_curry_define_fn_2(max_2_on)
fplus_curry_define_fn_1(min_2)
fplus_curry_define_fn_1(max_2)
fplus_curry_define_fn_0(deg_to_rad)
fplus_curry_define_fn_0(rad_to_deg)
fplus_curry_define_fn_2(normalize_min_max)
fplus_curry_define_fn_2(normalize_mean_stddev)
fplus_curry_define_fn_0(standardize)
fplus_curry_define_fn_1(histogram_using_intervals)
fplus_curry_define_fn_2(generate_consecutive_intervals)
fplus_curry_define_fn_3(histogram)
fplus_curry_define_fn_1(modulo_chain)
fplus_curry_define_fn_2(line_equation)
fplus_curry_define_fn_1(find_first_by)
fplus_curry_define_fn_1(find_last_by)
fplus_curry_define_fn_1(find_first_idx_by)
fplus_curry_define_fn_1(find_last_idx_by)
fplus_curry_define_fn_1(find_first_idx)
fplus_curry_define_fn_1(find_last_idx)
fplus_curry_define_fn_1(find_all_idxs_by)
fplus_curry_define_fn_1(find_all_idxs_of)
fplus_curry_define_fn_1(find_all_instances_of_token)
fplus_curry_define_fn_1(find_all_instances_of_token_non_overlapping)
fplus_curry_define_fn_1(find_first_instance_of_token)
fplus_curry_define_fn_1(set_includes)
fplus_curry_define_fn_1(unordered_set_includes)
fplus_curry_define_fn_1(set_merge)
fplus_curry_define_fn_1(unordered_set_merge)
fplus_curry_define_fn_1(set_intersection)
fplus_curry_define_fn_1(unordered_set_intersection)
fplus_curry_define_fn_1(set_is_disjoint)
fplus_curry_define_fn_1(unordered_set_is_disjoint)
fplus_curry_define_fn_1(set_difference)
fplus_curry_define_fn_1(unordered_set_difference)
fplus_curry_define_fn_1(set_symmetric_difference)
fplus_curry_define_fn_1(unordered_set_symmetric_difference)
fplus_curry_define_fn_0(sets_intersection)
fplus_curry_define_fn_0(unordered_sets_intersection)
fplus_curry_define_fn_1(any_by)
fplus_curry_define_fn_0(any)
fplus_curry_define_fn_1(none_by)
fplus_curry_define_fn_0(none)
fplus_curry_define_fn_1(minimum_idx_by)
fplus_curry_define_fn_1(minimum_idx_by_maybe)
fplus_curry_define_fn_1(maximum_idx_by)
fplus_curry_define_fn_1(maximum_idx_by_maybe)
fplus_curry_define_fn_0(minimum_idx)
fplus_curry_define_fn_0(minimum_idx_maybe)
fplus_curry_define_fn_0(maximum_idx)
fplus_curry_define_fn_0(maximum_idx_maybe)
fplus_curry_define_fn_1(minimum_idx_on)
fplus_curry_define_fn_1(minimum_idx_on_maybe)
fplus_curry_define_fn_1(maximum_idx_on)
fplus_curry_define_fn_1(maximum_idx_on_maybe)
fplus_curry_define_fn_1(minimum_by)
fplus_curry_define_fn_1(minimum_by_maybe)
fplus_curry_define_fn_1(maximum_by)
fplus_curry_define_fn_1(maximum_by_maybe)
fplus_curry_define_fn_0(minimum)
fplus_curry_define_fn_0(minimum_maybe)
fplus_curry_define_fn_0(maximum)
fplus_curry_define_fn_0(maximum_maybe)
fplus_curry_define_fn_1(minimum_on)
fplus_curry_define_fn_1(minimum_on_maybe)
fplus_curry_define_fn_1(maximum_on)
fplus_curry_define_fn_1(maximum_on_maybe)
fplus_curry_define_fn_0(mean)
fplus_curry_define_fn_0(mean_obj_div_size_t)
fplus_curry_define_fn_0(mean_obj_div_double)
fplus_curry_define_fn_0(mean_using_doubles)
fplus_curry_define_fn_0(median)
fplus_curry_define_fn_1(all_unique_by_less)
fplus_curry_define_fn_0(all_unique_less)
fplus_curry_define_fn_1(is_infix_of)
fplus_curry_define_fn_1(is_subsequence_of)
fplus_curry_define_fn_1(count_if)
fplus_curry_define_fn_1(count)
fplus_curry_define_fn_1(is_unique_in_by)
fplus_curry_define_fn_1(is_unique_in)
fplus_curry_define_fn_1(is_permutation_of)
fplus_curry_define_fn_1(fill_pigeonholes_to)
fplus_curry_define_fn_0(fill_pigeonholes)
fplus_curry_define_fn_1(fill_pigeonholes_bool_to)
fplus_curry_define_fn_0(fill_pigeonholes_bool)
fplus_curry_define_fn_0(present_in_all)
fplus_curry_define_fn_1(elem_at_idx_or_nothing)
fplus_curry_define_fn_2(elem_at_idx_or_constant)
fplus_curry_define_fn_1(elem_at_idx_or_replicate)
fplus_curry_define_fn_1(elem_at_idx_or_wrap)
fplus_curry_define_fn_2(extrapolate_replicate)
fplus_curry_define_fn_2(extrapolate_wrap)
fplus_curry_define_fn_1(elem_at_float_idx)
fplus_curry_define_fn_0(pairs_to_map)
fplus_curry_define_fn_0(pairs_to_map_grouped)
fplus_curry_define_fn_0(pairs_to_unordered_map_grouped)
fplus_curry_define_fn_0(map_to_pairs)
fplus_curry_define_fn_1(transform_map_values)
fplus_curry_define_fn_2(map_union_with)
fplus_curry_define_fn_1(map_union)
fplus_curry_define_fn_0(map_grouped_to_pairs)
fplus_curry_define_fn_0(get_map_keys)
fplus_curry_define_fn_0(get_map_values)
fplus_curry_define_fn_0(swap_keys_and_values)
fplus_curry_define_fn_1(create_map)
fplus_curry_define_fn_1(create_map_with)
fplus_curry_define_fn_1(create_map_grouped)
fplus_curry_define_fn_1(create_unordered_map)
fplus_curry_define_fn_1(create_unordered_map_with)
fplus_curry_define_fn_1(create_unordered_map_grouped)
fplus_curry_define_fn_1(get_from_map)
fplus_curry_define_fn_1(get_from_map_unsafe)
fplus_curry_define_fn_2(get_from_map_with_def)
fplus_curry_define_fn_1(get_first_from_map)
fplus_curry_define_fn_1(get_first_from_map_unsafe)
fplus_curry_define_fn_2(get_first_from_map_with_def)
fplus_curry_define_fn_1(map_contains)
fplus_curry_define_fn_1(map_keep_if)
fplus_curry_define_fn_1(map_drop_if)
fplus_curry_define_fn_1(map_keep)
fplus_curry_define_fn_1(map_drop)
fplus_curry_define_fn_1(map_keep_if_value)
fplus_curry_define_fn_1(map_drop_if_value)
fplus_curry_define_fn_1(map_keep_values)
fplus_curry_define_fn_1(map_drop_values)
fplus_curry_define_fn_1(map_pluck)
fplus_curry_define_fn_1(choose)
fplus_curry_define_fn_2(choose_by)
fplus_curry_define_fn_1(choose_lazy)
fplus_curry_define_fn_2(choose_by_lazy)
fplus_curry_define_fn_1(choose_def)
fplus_curry_define_fn_2(choose_by_def)
fplus_curry_define_fn_1(choose_def_lazy)
fplus_curry_define_fn_2(choose_by_def_lazy)
fplus_curry_define_fn_1(group_by)
fplus_curry_define_fn_1(group_on)
fplus_curry_define_fn_1(group_on_labeled)
fplus_curry_define_fn_0(group)
fplus_curry_define_fn_1(group_globally_by)
fplus_curry_define_fn_1(group_globally_on)
fplus_curry_define_fn_1(group_globally_on_labeled)
fplus_curry_define_fn_0(group_globally)
fplus_curry_define_fn_1(cluster_by)
fplus_curry_define_fn_2(split_by)
fplus_curry_define_fn_1(split_by_keep_separators)
fplus_curry_define_fn_2(split)
fplus_curry_define_fn_2(split_one_of)
fplus_curry_define_fn_1(split_keep_separators)
fplus_curry_define_fn_1(split_at_idx)
fplus_curry_define_fn_2(insert_at_idx)
fplus_curry_define_fn_1(partition)
fplus_curry_define_fn_1(split_at_idxs)
fplus_curry_define_fn_1(split_every)
fplus_curry_define_fn_2(split_by_token)
fplus_curry_define_fn_1(run_length_encode_by)
fplus_curry_define_fn_0(run_length_encode)
fplus_curry_define_fn_0(run_length_decode)
fplus_curry_define_fn_1(span)
fplus_curry_define_fn_2(divvy)
fplus_curry_define_fn_1(aperture)
fplus_curry_define_fn_1(stride)
fplus_curry_define_fn_1(winsorize)
fplus_curry_define_fn_1(separate_on)
fplus_curry_define_fn_0(separate)
fplus_curry_define_fn_1(transform_with_idx)
fplus_curry_define_fn_1(transform_and_keep_justs)
fplus_curry_define_fn_1(transform_and_keep_oks)
fplus_curry_define_fn_1(transform_and_concat)
fplus_curry_define_fn_1(replicate_elems)
fplus_curry_define_fn_0(interleave)
fplus_curry_define_fn_0(transpose)
fplus_curry_define_fn_1(shuffle)
fplus_curry_define_fn_2(sample)
fplus_curry_define_fn_1(random_element)
fplus_curry_define_fn_2(random_elements)
fplus_curry_define_fn_1(apply_functions)
fplus_curry_define_fn_2(apply_function_n_times)
fplus_curry_define_fn_1(transform_parallelly)
fplus_curry_define_fn_2(transform_parallelly_n_threads)
fplus_curry_define_fn_2(reduce_parallelly)
fplus_curry_define_fn_3(reduce_parallelly_n_threads)
fplus_curry_define_fn_1(reduce_1_parallelly)
fplus_curry_define_fn_2(reduce_1_parallelly_n_threads)
fplus_curry_define_fn_1(keep_if_parallelly)
fplus_curry_define_fn_2(keep_if_parallelly_n_threads)
fplus_curry_define_fn_3(transform_reduce)
fplus_curry_define_fn_2(transform_reduce_1)
fplus_curry_define_fn_3(transform_reduce_parallelly)
fplus_curry_define_fn_4(transform_reduce_parallelly_n_threads)
fplus_curry_define_fn_2(transform_reduce_1_parallelly)
fplus_curry_define_fn_3(transform_reduce_1_parallelly_n_threads)
fplus_curry_define_fn_1(read_value_with_default)
fplus_curry_define_fn_2(replace_if)
fplus_curry_define_fn_2(replace_elem_at_idx)
fplus_curry_define_fn_2(replace_elems)
fplus_curry_define_fn_2(replace_tokens)
fplus_curry_define_fn_0(show)
fplus_curry_define_fn_3(show_cont_with_frame_and_newlines)
fplus_curry_define_fn_3(show_cont_with_frame)
fplus_curry_define_fn_1(show_cont_with)
fplus_curry_define_fn_0(show_cont)
fplus_curry_define_fn_0(show_maybe)
fplus_curry_define_fn_0(show_result)
fplus_curry_define_fn_2(show_float)
fplus_curry_define_fn_3(show_float_fill_left)
fplus_curry_define_fn_2(show_fill_left)
fplus_curry_define_fn_2(show_fill_right)
fplus_curry_define_fn_0(is_letter_or_digit)
fplus_curry_define_fn_0(is_whitespace)
fplus_curry_define_fn_0(is_line_break)
fplus_curry_define_fn_0(clean_newlines)
fplus_curry_define_fn_1(split_words)
fplus_curry_define_fn_1(split_lines)
fplus_curry_define_fn_0(trim_whitespace_left)
fplus_curry_define_fn_0(trim_whitespace_right)
fplus_curry_define_fn_0(trim_whitespace)
fplus_curry_define_fn_0(to_lower_case)
fplus_curry_define_fn_1(to_lower_case_loc)
fplus_curry_define_fn_0(to_upper_case)
fplus_curry_define_fn_1(to_upper_case_loc)
fplus_curry_define_fn_2(to_string_fill_left)
fplus_curry_define_fn_2(to_string_fill_right)
fplus_curry_define_fn_1(trees_from_sequence)
fplus_curry_define_fn_1(are_trees_equal)
fplus_curry_define_fn_0(tree_size)
fplus_curry_define_fn_0(tree_depth)
fplus_curry_define_fn_0(flatten_tree_depth_first)
fplus_curry_define_fn_0(flatten_tree_breadth_first)
fplus_curry_define_fn_0(show_timed)
fplus_curry_define_fn_0(make_timed_function)
fplus_curry_define_fn_0(make_timed_void_function)

} // namespace curry
} // namespace fplus

//
// fwd.hpp
//

// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)


namespace fplus
{
namespace fwd
{

// Partial currying.
// Allow to generically bind all but parameters except the last one.
// The lambda paramter ist named fplus_fwd_x instead of x
// because gcc can produce unjustified shadow warnings. see:
// http://stackoverflow.com/questions/41208811/parameter-of-returned-generic-lambda-allegedly-shadows-parameter-of-free-functio
#define fplus_fwd_define_fn_0(fplus_fwd_define_fn_0_name) \
inline auto fplus_fwd_define_fn_0_name() \
{ \
    return [](auto&& fplus_fwd_x) \
    { \
        return fplus::fplus_fwd_define_fn_0_name(std::forward<decltype(fplus_fwd_x)>(fplus_fwd_x)); \
    }; \
}

#define fplus_fwd_define_fn_1(fplus_fwd_define_fn_1_name) \
template <typename P1> \
auto fplus_fwd_define_fn_1_name(P1 p1) \
{ \
    return [p1](auto&& fplus_fwd_x) \
    { \
        return fplus::fplus_fwd_define_fn_1_name(p1, std::forward<decltype(fplus_fwd_x)>(fplus_fwd_x)); \
    }; \
}

#define fplus_fwd_define_fn_2(fplus_fwd_define_fn_2_name) \
template <typename P1, typename P2> \
auto fplus_fwd_define_fn_2_name(P1 p1, P2 p2) \
{ \
    return [p1, p2](auto&& fplus_fwd_x) \
    { \
        return fplus::fplus_fwd_define_fn_2_name(p1, p2, std::forward<decltype(fplus_fwd_x)>(fplus_fwd_x)); \
    }; \
}

#define fplus_fwd_define_fn_3(fplus_fwd_define_fn_3_name) \
template <typename P1, typename P2, typename P3> \
auto fplus_fwd_define_fn_3_name(P1 p1, P2 p2, P3 p3) \
{ \
    return [p1, p2, p3](auto&& fplus_fwd_x) \
    { \
        return fplus::fplus_fwd_define_fn_3_name(p1, p2, p3, std::forward<decltype(fplus_fwd_x)>(fplus_fwd_x)); \
    }; \
}

#define fplus_fwd_define_fn_4(fplus_fwd_define_fn_4_name) \
template <typename P1, typename P2, typename P3, typename P4> \
auto fplus_fwd_define_fn_4_name(P1 p1, P2 p2, P3 p3, P4 p4) \
{ \
    return [p1, p2, p3, p4](auto&& fplus_fwd_x) \
    { \
        return fplus::fplus_fwd_define_fn_4_name(p1, p2, p3, p4, std::forward<decltype(fplus_fwd_x)>(fplus_fwd_x)); \
    }; \
}


#define fplus_fwd_flip_define_fn_1(fplus_fwd_flip_define_fn_1_name) \
namespace flip \
{ \
    template <typename P2> \
    auto fplus_fwd_flip_define_fn_1_name(P2 p2) \
    { \
        return [p2](auto&& fplus_fwd_flip_x) \
        { \
            return fplus::fplus_fwd_flip_define_fn_1_name(std::forward<decltype(fplus_fwd_flip_x)>(fplus_fwd_flip_x), p2); \
        }; \
    } \
} // namespace flip


namespace internal
{
    template<typename F, typename G>
    struct compose_helper{
        compose_helper(F f, G g) : f_(f), g_(g) {}
        template<typename X>
        decltype(auto) operator()(X&& x) const
        {
            return g_(f_(std::forward<X>(x)));
        }
    private:
        F f_;
        G g_;
    };
} // namespace internal
template<typename F, typename G>
auto compose(F f, G g) {
    return internal::compose_helper<F, G> {f, g};
}
template<typename F1, typename... Fs>
auto compose(F1 f, Fs ... args)
{
    return compose(f, compose(args...));
}

template<typename X, typename... Fs>
auto apply(X&& x, Fs ... args)
{
    return compose(args...)(std::forward<X>(x));
}
template<typename X, typename F>
auto apply(X&& x, F f)
{
    return f(std::forward<X>(x));
}


//
// fwd_instances.autogenerated_defines
//

// THIS FILE WAS GENERATED AUTOMATICALLY. DO NOT EDIT.
fplus_fwd_define_fn_0(identity)
fplus_fwd_define_fn_1(is_equal)
fplus_fwd_define_fn_1(is_not_equal)
fplus_fwd_define_fn_1(is_less)
fplus_fwd_define_fn_1(is_less_or_equal)
fplus_fwd_define_fn_1(is_greater)
fplus_fwd_define_fn_1(is_greater_or_equal)
fplus_fwd_define_fn_1(xor_bools)
fplus_fwd_define_fn_0(is_just)
fplus_fwd_define_fn_0(is_nothing)
fplus_fwd_define_fn_0(unsafe_get_just)
fplus_fwd_define_fn_0(just_with_default)
fplus_fwd_define_fn_1(throw_on_nothing)
fplus_fwd_define_fn_0(just)
fplus_fwd_define_fn_1(as_just_if)
fplus_fwd_define_fn_0(maybe_to_seq)
fplus_fwd_define_fn_0(singleton_seq_as_maybe)
fplus_fwd_define_fn_1(lift_maybe)
fplus_fwd_define_fn_2(lift_maybe_def)
fplus_fwd_define_fn_2(lift_maybe_2)
fplus_fwd_define_fn_3(lift_maybe_2_def)
fplus_fwd_define_fn_1(and_then_maybe)
fplus_fwd_define_fn_0(flatten_maybe)
fplus_fwd_define_fn_0(is_empty)
fplus_fwd_define_fn_0(is_not_empty)
fplus_fwd_define_fn_0(size_of_cont)
fplus_fwd_define_fn_0(convert)
fplus_fwd_define_fn_0(convert_elems)
fplus_fwd_define_fn_0(convert_container)
fplus_fwd_define_fn_0(convert_container_and_elems)
fplus_fwd_define_fn_2(get_segment)
fplus_fwd_define_fn_2(set_segment)
fplus_fwd_define_fn_2(remove_segment)
fplus_fwd_define_fn_2(insert_at)
fplus_fwd_define_fn_1(elem_at_idx)
fplus_fwd_define_fn_1(elem_at_idx_maybe)
fplus_fwd_define_fn_1(elems_at_idxs)
fplus_fwd_define_fn_1(transform)
fplus_fwd_define_fn_1(transform_convert)
fplus_fwd_define_fn_1(transform_inner)
fplus_fwd_define_fn_0(reverse)
fplus_fwd_define_fn_1(take)
fplus_fwd_define_fn_1(take_exact)
fplus_fwd_define_fn_1(take_cyclic)
fplus_fwd_define_fn_1(drop)
fplus_fwd_define_fn_1(take_last)
fplus_fwd_define_fn_1(drop_last)
fplus_fwd_define_fn_1(drop_exact)
fplus_fwd_define_fn_1(take_while)
fplus_fwd_define_fn_1(take_last_while)
fplus_fwd_define_fn_1(drop_while)
fplus_fwd_define_fn_1(drop_last_while)
fplus_fwd_define_fn_2(fold_left)
fplus_fwd_define_fn_2(reduce)
fplus_fwd_define_fn_1(fold_left_1)
fplus_fwd_define_fn_1(reduce_1)
fplus_fwd_define_fn_2(fold_right)
fplus_fwd_define_fn_1(fold_right_1)
fplus_fwd_define_fn_2(scan_left)
fplus_fwd_define_fn_1(scan_left_1)
fplus_fwd_define_fn_2(scan_right)
fplus_fwd_define_fn_1(scan_right_1)
fplus_fwd_define_fn_0(sum)
fplus_fwd_define_fn_0(product)
fplus_fwd_define_fn_1(append_elem)
fplus_fwd_define_fn_1(prepend_elem)
fplus_fwd_define_fn_1(append)
fplus_fwd_define_fn_1(append_convert)
fplus_fwd_define_fn_0(concat)
fplus_fwd_define_fn_1(interweave)
fplus_fwd_define_fn_0(unweave)
fplus_fwd_define_fn_1(sort_by)
fplus_fwd_define_fn_1(sort_on)
fplus_fwd_define_fn_0(sort)
fplus_fwd_define_fn_1(stable_sort_by)
fplus_fwd_define_fn_1(stable_sort_on)
fplus_fwd_define_fn_0(stable_sort)
fplus_fwd_define_fn_2(partial_sort_by)
fplus_fwd_define_fn_2(partial_sort_on)
fplus_fwd_define_fn_1(partial_sort)
fplus_fwd_define_fn_2(nth_element_by)
fplus_fwd_define_fn_2(nth_element_on)
fplus_fwd_define_fn_1(nth_element)
fplus_fwd_define_fn_1(unique_by)
fplus_fwd_define_fn_1(unique_on)
fplus_fwd_define_fn_0(unique)
fplus_fwd_define_fn_1(intersperse)
fplus_fwd_define_fn_1(join)
fplus_fwd_define_fn_1(join_elem)
fplus_fwd_define_fn_1(is_elem_of_by)
fplus_fwd_define_fn_1(is_elem_of)
fplus_fwd_define_fn_1(nub_by)
fplus_fwd_define_fn_1(nub_on)
fplus_fwd_define_fn_0(nub)
fplus_fwd_define_fn_1(all_unique_by_eq)
fplus_fwd_define_fn_1(all_unique_on)
fplus_fwd_define_fn_0(all_unique)
fplus_fwd_define_fn_1(is_strictly_sorted_by)
fplus_fwd_define_fn_1(is_strictly_sorted_on)
fplus_fwd_define_fn_0(is_strictly_sorted)
fplus_fwd_define_fn_1(is_sorted_by)
fplus_fwd_define_fn_1(is_sorted_on)
fplus_fwd_define_fn_0(is_sorted)
fplus_fwd_define_fn_1(is_prefix_of)
fplus_fwd_define_fn_1(is_suffix_of)
fplus_fwd_define_fn_1(all_by)
fplus_fwd_define_fn_0(all)
fplus_fwd_define_fn_1(all_the_same_by)
fplus_fwd_define_fn_1(all_the_same_on)
fplus_fwd_define_fn_0(all_the_same)
fplus_fwd_define_fn_2(numbers_step)
fplus_fwd_define_fn_1(numbers)
fplus_fwd_define_fn_0(singleton_seq)
fplus_fwd_define_fn_0(all_idxs)
fplus_fwd_define_fn_0(init)
fplus_fwd_define_fn_0(tail)
fplus_fwd_define_fn_0(head)
fplus_fwd_define_fn_0(last)
fplus_fwd_define_fn_0(mean_stddev)
fplus_fwd_define_fn_1(count_occurrences_by)
fplus_fwd_define_fn_0(count_occurrences)
fplus_fwd_define_fn_2(lexicographical_less_by)
fplus_fwd_define_fn_1(lexicographical_less)
fplus_fwd_define_fn_0(lexicographical_sort)
fplus_fwd_define_fn_1(replicate)
fplus_fwd_define_fn_2(instead_of_if)
fplus_fwd_define_fn_2(instead_of_if_empty)
fplus_fwd_define_fn_0(is_ok)
fplus_fwd_define_fn_0(is_error)
fplus_fwd_define_fn_0(unsafe_get_ok)
fplus_fwd_define_fn_0(unsafe_get_error)
fplus_fwd_define_fn_1(ok_with_default)
fplus_fwd_define_fn_0(ok)
fplus_fwd_define_fn_0(error)
fplus_fwd_define_fn_0(to_maybe)
fplus_fwd_define_fn_1(from_maybe)
fplus_fwd_define_fn_1(throw_on_error)
fplus_fwd_define_fn_1(lift_result)
fplus_fwd_define_fn_2(lift_result_both)
fplus_fwd_define_fn_2(unify_result)
fplus_fwd_define_fn_1(and_then_result)
fplus_fwd_define_fn_1(keep_if)
fplus_fwd_define_fn_1(drop_if)
fplus_fwd_define_fn_1(without)
fplus_fwd_define_fn_1(without_any)
fplus_fwd_define_fn_1(keep_if_with_idx)
fplus_fwd_define_fn_1(drop_if_with_idx)
fplus_fwd_define_fn_1(keep_by_idx)
fplus_fwd_define_fn_1(drop_by_idx)
fplus_fwd_define_fn_1(keep_idxs)
fplus_fwd_define_fn_1(drop_idxs)
fplus_fwd_define_fn_1(drop_idx)
fplus_fwd_define_fn_0(justs)
fplus_fwd_define_fn_0(oks)
fplus_fwd_define_fn_0(errors)
fplus_fwd_define_fn_1(trim_left)
fplus_fwd_define_fn_1(trim_token_left)
fplus_fwd_define_fn_1(trim_right_by)
fplus_fwd_define_fn_1(trim_right)
fplus_fwd_define_fn_1(trim_token_right)
fplus_fwd_define_fn_1(trim_by)
fplus_fwd_define_fn_1(trim)
fplus_fwd_define_fn_1(trim_token)
fplus_fwd_define_fn_1(adjacent_keep_snd_if)
fplus_fwd_define_fn_1(adjacent_drop_fst_if)
fplus_fwd_define_fn_1(adjacent_drop_snd_if)
fplus_fwd_define_fn_1(adjacent_keep_fst_if)
fplus_fwd_define_fn_1(generate_by_idx)
fplus_fwd_define_fn_1(repeat)
fplus_fwd_define_fn_1(infixes)
fplus_fwd_define_fn_3(carthesian_product_with_where)
fplus_fwd_define_fn_2(carthesian_product_with)
fplus_fwd_define_fn_2(carthesian_product_where)
fplus_fwd_define_fn_1(carthesian_product)
fplus_fwd_define_fn_1(carthesian_product_n)
fplus_fwd_define_fn_1(permutations)
fplus_fwd_define_fn_1(combinations)
fplus_fwd_define_fn_1(combinations_with_replacement)
fplus_fwd_define_fn_0(power_set)
fplus_fwd_define_fn_2(iterate)
fplus_fwd_define_fn_1(iterate_maybe)
fplus_fwd_define_fn_1(adjacent_difference_by)
fplus_fwd_define_fn_0(adjacent_difference)
fplus_fwd_define_fn_0(rotate_left)
fplus_fwd_define_fn_0(rotate_right)
fplus_fwd_define_fn_0(rotations_left)
fplus_fwd_define_fn_0(rotations_right)
fplus_fwd_define_fn_2(fill_left)
fplus_fwd_define_fn_2(fill_right)
fplus_fwd_define_fn_0(inits)
fplus_fwd_define_fn_0(tails)
fplus_fwd_define_fn_1(apply_to_pair)
fplus_fwd_define_fn_2(zip_with)
fplus_fwd_define_fn_3(zip_with_3)
fplus_fwd_define_fn_4(zip_with_defaults)
fplus_fwd_define_fn_1(zip)
fplus_fwd_define_fn_1(zip_repeat)
fplus_fwd_define_fn_0(unzip)
fplus_fwd_define_fn_0(fst)
fplus_fwd_define_fn_0(snd)
fplus_fwd_define_fn_1(transform_fst)
fplus_fwd_define_fn_1(transform_snd)
fplus_fwd_define_fn_2(transform_pair)
fplus_fwd_define_fn_0(swap_pair_elems)
fplus_fwd_define_fn_0(swap_pairs_elems)
fplus_fwd_define_fn_0(adjacent_pairs)
fplus_fwd_define_fn_0(overlapping_pairs)
fplus_fwd_define_fn_0(overlapping_pairs_cyclic)
fplus_fwd_define_fn_0(enumerate)
fplus_fwd_define_fn_4(inner_product_with)
fplus_fwd_define_fn_2(inner_product)
fplus_fwd_define_fn_2(first_mismatch_idx_by)
fplus_fwd_define_fn_2(first_mismatch_by)
fplus_fwd_define_fn_2(first_mismatch_idx_on)
fplus_fwd_define_fn_2(first_mismatch_on)
fplus_fwd_define_fn_2(first_mismatch_idx)
fplus_fwd_define_fn_2(first_mismatch)
fplus_fwd_define_fn_2(first_match_idx_by)
fplus_fwd_define_fn_2(first_match_by)
fplus_fwd_define_fn_2(first_match_idx_on)
fplus_fwd_define_fn_2(first_match_on)
fplus_fwd_define_fn_2(first_match_idx)
fplus_fwd_define_fn_2(first_match)
fplus_fwd_define_fn_2(is_in_interval)
fplus_fwd_define_fn_2(is_in_interval_around)
fplus_fwd_define_fn_2(is_in_open_interval)
fplus_fwd_define_fn_2(is_in_open_interval_around)
fplus_fwd_define_fn_2(is_in_closed_interval)
fplus_fwd_define_fn_4(reference_interval)
fplus_fwd_define_fn_2(clamp)
fplus_fwd_define_fn_0(is_negative)
fplus_fwd_define_fn_0(is_positive)
fplus_fwd_define_fn_0(is_even)
fplus_fwd_define_fn_0(is_odd)
fplus_fwd_define_fn_0(abs)
fplus_fwd_define_fn_1(abs_diff)
fplus_fwd_define_fn_0(square)
fplus_fwd_define_fn_0(cube)
fplus_fwd_define_fn_0(sign)
fplus_fwd_define_fn_0(sign_with_zero)
fplus_fwd_define_fn_0(integral_cast_throw)
fplus_fwd_define_fn_0(integral_cast_clamp)
fplus_fwd_define_fn_0(round)
fplus_fwd_define_fn_0(floor)
fplus_fwd_define_fn_1(floor_to_int_mult)
fplus_fwd_define_fn_1(ceil_to_int_mult)
fplus_fwd_define_fn_0(ceil)
fplus_fwd_define_fn_1(int_power)
fplus_fwd_define_fn_2(min_2_on)
fplus_fwd_define_fn_2(max_2_on)
fplus_fwd_define_fn_1(min_2)
fplus_fwd_define_fn_1(max_2)
fplus_fwd_define_fn_0(deg_to_rad)
fplus_fwd_define_fn_0(rad_to_deg)
fplus_fwd_define_fn_2(normalize_min_max)
fplus_fwd_define_fn_2(normalize_mean_stddev)
fplus_fwd_define_fn_0(standardize)
fplus_fwd_define_fn_1(histogram_using_intervals)
fplus_fwd_define_fn_2(generate_consecutive_intervals)
fplus_fwd_define_fn_3(histogram)
fplus_fwd_define_fn_1(modulo_chain)
fplus_fwd_define_fn_2(line_equation)
fplus_fwd_define_fn_1(find_first_by)
fplus_fwd_define_fn_1(find_last_by)
fplus_fwd_define_fn_1(find_first_idx_by)
fplus_fwd_define_fn_1(find_last_idx_by)
fplus_fwd_define_fn_1(find_first_idx)
fplus_fwd_define_fn_1(find_last_idx)
fplus_fwd_define_fn_1(find_all_idxs_by)
fplus_fwd_define_fn_1(find_all_idxs_of)
fplus_fwd_define_fn_1(find_all_instances_of_token)
fplus_fwd_define_fn_1(find_all_instances_of_token_non_overlapping)
fplus_fwd_define_fn_1(find_first_instance_of_token)
fplus_fwd_define_fn_1(set_includes)
fplus_fwd_define_fn_1(unordered_set_includes)
fplus_fwd_define_fn_1(set_merge)
fplus_fwd_define_fn_1(unordered_set_merge)
fplus_fwd_define_fn_1(set_intersection)
fplus_fwd_define_fn_1(unordered_set_intersection)
fplus_fwd_define_fn_1(set_is_disjoint)
fplus_fwd_define_fn_1(unordered_set_is_disjoint)
fplus_fwd_define_fn_1(set_difference)
fplus_fwd_define_fn_1(unordered_set_difference)
fplus_fwd_define_fn_1(set_symmetric_difference)
fplus_fwd_define_fn_1(unordered_set_symmetric_difference)
fplus_fwd_define_fn_0(sets_intersection)
fplus_fwd_define_fn_0(unordered_sets_intersection)
fplus_fwd_define_fn_1(any_by)
fplus_fwd_define_fn_0(any)
fplus_fwd_define_fn_1(none_by)
fplus_fwd_define_fn_0(none)
fplus_fwd_define_fn_1(minimum_idx_by)
fplus_fwd_define_fn_1(minimum_idx_by_maybe)
fplus_fwd_define_fn_1(maximum_idx_by)
fplus_fwd_define_fn_1(maximum_idx_by_maybe)
fplus_fwd_define_fn_0(minimum_idx)
fplus_fwd_define_fn_0(minimum_idx_maybe)
fplus_fwd_define_fn_0(maximum_idx)
fplus_fwd_define_fn_0(maximum_idx_maybe)
fplus_fwd_define_fn_1(minimum_idx_on)
fplus_fwd_define_fn_1(minimum_idx_on_maybe)
fplus_fwd_define_fn_1(maximum_idx_on)
fplus_fwd_define_fn_1(maximum_idx_on_maybe)
fplus_fwd_define_fn_1(minimum_by)
fplus_fwd_define_fn_1(minimum_by_maybe)
fplus_fwd_define_fn_1(maximum_by)
fplus_fwd_define_fn_1(maximum_by_maybe)
fplus_fwd_define_fn_0(minimum)
fplus_fwd_define_fn_0(minimum_maybe)
fplus_fwd_define_fn_0(maximum)
fplus_fwd_define_fn_0(maximum_maybe)
fplus_fwd_define_fn_1(minimum_on)
fplus_fwd_define_fn_1(minimum_on_maybe)
fplus_fwd_define_fn_1(maximum_on)
fplus_fwd_define_fn_1(maximum_on_maybe)
fplus_fwd_define_fn_0(mean)
fplus_fwd_define_fn_0(mean_obj_div_size_t)
fplus_fwd_define_fn_0(mean_obj_div_double)
fplus_fwd_define_fn_0(mean_using_doubles)
fplus_fwd_define_fn_0(median)
fplus_fwd_define_fn_1(all_unique_by_less)
fplus_fwd_define_fn_0(all_unique_less)
fplus_fwd_define_fn_1(is_infix_of)
fplus_fwd_define_fn_1(is_subsequence_of)
fplus_fwd_define_fn_1(count_if)
fplus_fwd_define_fn_1(count)
fplus_fwd_define_fn_1(is_unique_in_by)
fplus_fwd_define_fn_1(is_unique_in)
fplus_fwd_define_fn_1(is_permutation_of)
fplus_fwd_define_fn_1(fill_pigeonholes_to)
fplus_fwd_define_fn_0(fill_pigeonholes)
fplus_fwd_define_fn_1(fill_pigeonholes_bool_to)
fplus_fwd_define_fn_0(fill_pigeonholes_bool)
fplus_fwd_define_fn_0(present_in_all)
fplus_fwd_define_fn_1(elem_at_idx_or_nothing)
fplus_fwd_define_fn_2(elem_at_idx_or_constant)
fplus_fwd_define_fn_1(elem_at_idx_or_replicate)
fplus_fwd_define_fn_1(elem_at_idx_or_wrap)
fplus_fwd_define_fn_2(extrapolate_replicate)
fplus_fwd_define_fn_2(extrapolate_wrap)
fplus_fwd_define_fn_1(elem_at_float_idx)
fplus_fwd_define_fn_0(pairs_to_map)
fplus_fwd_define_fn_0(pairs_to_map_grouped)
fplus_fwd_define_fn_0(pairs_to_unordered_map_grouped)
fplus_fwd_define_fn_0(map_to_pairs)
fplus_fwd_define_fn_1(transform_map_values)
fplus_fwd_define_fn_2(map_union_with)
fplus_fwd_define_fn_1(map_union)
fplus_fwd_define_fn_0(map_grouped_to_pairs)
fplus_fwd_define_fn_0(get_map_keys)
fplus_fwd_define_fn_0(get_map_values)
fplus_fwd_define_fn_0(swap_keys_and_values)
fplus_fwd_define_fn_1(create_map)
fplus_fwd_define_fn_1(create_map_with)
fplus_fwd_define_fn_1(create_map_grouped)
fplus_fwd_define_fn_1(create_unordered_map)
fplus_fwd_define_fn_1(create_unordered_map_with)
fplus_fwd_define_fn_1(create_unordered_map_grouped)
fplus_fwd_define_fn_1(get_from_map)
fplus_fwd_define_fn_1(get_from_map_unsafe)
fplus_fwd_define_fn_2(get_from_map_with_def)
fplus_fwd_define_fn_1(get_first_from_map)
fplus_fwd_define_fn_1(get_first_from_map_unsafe)
fplus_fwd_define_fn_2(get_first_from_map_with_def)
fplus_fwd_define_fn_1(map_contains)
fplus_fwd_define_fn_1(map_keep_if)
fplus_fwd_define_fn_1(map_drop_if)
fplus_fwd_define_fn_1(map_keep)
fplus_fwd_define_fn_1(map_drop)
fplus_fwd_define_fn_1(map_keep_if_value)
fplus_fwd_define_fn_1(map_drop_if_value)
fplus_fwd_define_fn_1(map_keep_values)
fplus_fwd_define_fn_1(map_drop_values)
fplus_fwd_define_fn_1(map_pluck)
fplus_fwd_define_fn_1(choose)
fplus_fwd_define_fn_2(choose_by)
fplus_fwd_define_fn_1(choose_lazy)
fplus_fwd_define_fn_2(choose_by_lazy)
fplus_fwd_define_fn_1(choose_def)
fplus_fwd_define_fn_2(choose_by_def)
fplus_fwd_define_fn_1(choose_def_lazy)
fplus_fwd_define_fn_2(choose_by_def_lazy)
fplus_fwd_define_fn_1(group_by)
fplus_fwd_define_fn_1(group_on)
fplus_fwd_define_fn_1(group_on_labeled)
fplus_fwd_define_fn_0(group)
fplus_fwd_define_fn_1(group_globally_by)
fplus_fwd_define_fn_1(group_globally_on)
fplus_fwd_define_fn_1(group_globally_on_labeled)
fplus_fwd_define_fn_0(group_globally)
fplus_fwd_define_fn_1(cluster_by)
fplus_fwd_define_fn_2(split_by)
fplus_fwd_define_fn_1(split_by_keep_separators)
fplus_fwd_define_fn_2(split)
fplus_fwd_define_fn_2(split_one_of)
fplus_fwd_define_fn_1(split_keep_separators)
fplus_fwd_define_fn_1(split_at_idx)
fplus_fwd_define_fn_2(insert_at_idx)
fplus_fwd_define_fn_1(partition)
fplus_fwd_define_fn_1(split_at_idxs)
fplus_fwd_define_fn_1(split_every)
fplus_fwd_define_fn_2(split_by_token)
fplus_fwd_define_fn_1(run_length_encode_by)
fplus_fwd_define_fn_0(run_length_encode)
fplus_fwd_define_fn_0(run_length_decode)
fplus_fwd_define_fn_1(span)
fplus_fwd_define_fn_2(divvy)
fplus_fwd_define_fn_1(aperture)
fplus_fwd_define_fn_1(stride)
fplus_fwd_define_fn_1(winsorize)
fplus_fwd_define_fn_1(separate_on)
fplus_fwd_define_fn_0(separate)
fplus_fwd_define_fn_1(transform_with_idx)
fplus_fwd_define_fn_1(transform_and_keep_justs)
fplus_fwd_define_fn_1(transform_and_keep_oks)
fplus_fwd_define_fn_1(transform_and_concat)
fplus_fwd_define_fn_1(replicate_elems)
fplus_fwd_define_fn_0(interleave)
fplus_fwd_define_fn_0(transpose)
fplus_fwd_define_fn_1(shuffle)
fplus_fwd_define_fn_2(sample)
fplus_fwd_define_fn_1(random_element)
fplus_fwd_define_fn_2(random_elements)
fplus_fwd_define_fn_1(apply_functions)
fplus_fwd_define_fn_2(apply_function_n_times)
fplus_fwd_define_fn_1(transform_parallelly)
fplus_fwd_define_fn_2(transform_parallelly_n_threads)
fplus_fwd_define_fn_2(reduce_parallelly)
fplus_fwd_define_fn_3(reduce_parallelly_n_threads)
fplus_fwd_define_fn_1(reduce_1_parallelly)
fplus_fwd_define_fn_2(reduce_1_parallelly_n_threads)
fplus_fwd_define_fn_1(keep_if_parallelly)
fplus_fwd_define_fn_2(keep_if_parallelly_n_threads)
fplus_fwd_define_fn_3(transform_reduce)
fplus_fwd_define_fn_2(transform_reduce_1)
fplus_fwd_define_fn_3(transform_reduce_parallelly)
fplus_fwd_define_fn_4(transform_reduce_parallelly_n_threads)
fplus_fwd_define_fn_2(transform_reduce_1_parallelly)
fplus_fwd_define_fn_3(transform_reduce_1_parallelly_n_threads)
fplus_fwd_define_fn_1(read_value_with_default)
fplus_fwd_define_fn_2(replace_if)
fplus_fwd_define_fn_2(replace_elem_at_idx)
fplus_fwd_define_fn_2(replace_elems)
fplus_fwd_define_fn_2(replace_tokens)
fplus_fwd_define_fn_0(show)
fplus_fwd_define_fn_3(show_cont_with_frame_and_newlines)
fplus_fwd_define_fn_3(show_cont_with_frame)
fplus_fwd_define_fn_1(show_cont_with)
fplus_fwd_define_fn_0(show_cont)
fplus_fwd_define_fn_0(show_maybe)
fplus_fwd_define_fn_0(show_result)
fplus_fwd_define_fn_2(show_float)
fplus_fwd_define_fn_3(show_float_fill_left)
fplus_fwd_define_fn_2(show_fill_left)
fplus_fwd_define_fn_2(show_fill_right)
fplus_fwd_define_fn_0(is_letter_or_digit)
fplus_fwd_define_fn_0(is_whitespace)
fplus_fwd_define_fn_0(is_line_break)
fplus_fwd_define_fn_0(clean_newlines)
fplus_fwd_define_fn_1(split_words)
fplus_fwd_define_fn_1(split_lines)
fplus_fwd_define_fn_0(trim_whitespace_left)
fplus_fwd_define_fn_0(trim_whitespace_right)
fplus_fwd_define_fn_0(trim_whitespace)
fplus_fwd_define_fn_0(to_lower_case)
fplus_fwd_define_fn_1(to_lower_case_loc)
fplus_fwd_define_fn_0(to_upper_case)
fplus_fwd_define_fn_1(to_upper_case_loc)
fplus_fwd_define_fn_2(to_string_fill_left)
fplus_fwd_define_fn_2(to_string_fill_right)
fplus_fwd_define_fn_1(trees_from_sequence)
fplus_fwd_define_fn_1(are_trees_equal)
fplus_fwd_define_fn_0(tree_size)
fplus_fwd_define_fn_0(tree_depth)
fplus_fwd_define_fn_0(flatten_tree_depth_first)
fplus_fwd_define_fn_0(flatten_tree_breadth_first)
fplus_fwd_define_fn_0(show_timed)
fplus_fwd_define_fn_0(make_timed_function)
fplus_fwd_define_fn_0(make_timed_void_function)
fplus_fwd_flip_define_fn_1(is_equal)
fplus_fwd_flip_define_fn_1(is_not_equal)
fplus_fwd_flip_define_fn_1(is_less)
fplus_fwd_flip_define_fn_1(is_less_or_equal)
fplus_fwd_flip_define_fn_1(is_greater)
fplus_fwd_flip_define_fn_1(is_greater_or_equal)
fplus_fwd_flip_define_fn_1(xor_bools)
fplus_fwd_flip_define_fn_1(throw_on_nothing)
fplus_fwd_flip_define_fn_1(as_just_if)
fplus_fwd_flip_define_fn_1(lift_maybe)
fplus_fwd_flip_define_fn_1(and_then_maybe)
fplus_fwd_flip_define_fn_1(elem_at_idx)
fplus_fwd_flip_define_fn_1(elem_at_idx_maybe)
fplus_fwd_flip_define_fn_1(elems_at_idxs)
fplus_fwd_flip_define_fn_1(transform)
fplus_fwd_flip_define_fn_1(transform_convert)
fplus_fwd_flip_define_fn_1(transform_inner)
fplus_fwd_flip_define_fn_1(take)
fplus_fwd_flip_define_fn_1(take_exact)
fplus_fwd_flip_define_fn_1(take_cyclic)
fplus_fwd_flip_define_fn_1(drop)
fplus_fwd_flip_define_fn_1(take_last)
fplus_fwd_flip_define_fn_1(drop_last)
fplus_fwd_flip_define_fn_1(drop_exact)
fplus_fwd_flip_define_fn_1(take_while)
fplus_fwd_flip_define_fn_1(take_last_while)
fplus_fwd_flip_define_fn_1(drop_while)
fplus_fwd_flip_define_fn_1(drop_last_while)
fplus_fwd_flip_define_fn_1(fold_left_1)
fplus_fwd_flip_define_fn_1(reduce_1)
fplus_fwd_flip_define_fn_1(fold_right_1)
fplus_fwd_flip_define_fn_1(scan_left_1)
fplus_fwd_flip_define_fn_1(scan_right_1)
fplus_fwd_flip_define_fn_1(append_elem)
fplus_fwd_flip_define_fn_1(prepend_elem)
fplus_fwd_flip_define_fn_1(append)
fplus_fwd_flip_define_fn_1(append_convert)
fplus_fwd_flip_define_fn_1(interweave)
fplus_fwd_flip_define_fn_1(sort_by)
fplus_fwd_flip_define_fn_1(sort_on)
fplus_fwd_flip_define_fn_1(stable_sort_by)
fplus_fwd_flip_define_fn_1(stable_sort_on)
fplus_fwd_flip_define_fn_1(partial_sort)
fplus_fwd_flip_define_fn_1(nth_element)
fplus_fwd_flip_define_fn_1(unique_by)
fplus_fwd_flip_define_fn_1(unique_on)
fplus_fwd_flip_define_fn_1(intersperse)
fplus_fwd_flip_define_fn_1(join)
fplus_fwd_flip_define_fn_1(join_elem)
fplus_fwd_flip_define_fn_1(is_elem_of_by)
fplus_fwd_flip_define_fn_1(is_elem_of)
fplus_fwd_flip_define_fn_1(nub_by)
fplus_fwd_flip_define_fn_1(nub_on)
fplus_fwd_flip_define_fn_1(all_unique_by_eq)
fplus_fwd_flip_define_fn_1(all_unique_on)
fplus_fwd_flip_define_fn_1(is_strictly_sorted_by)
fplus_fwd_flip_define_fn_1(is_strictly_sorted_on)
fplus_fwd_flip_define_fn_1(is_sorted_by)
fplus_fwd_flip_define_fn_1(is_sorted_on)
fplus_fwd_flip_define_fn_1(is_prefix_of)
fplus_fwd_flip_define_fn_1(is_suffix_of)
fplus_fwd_flip_define_fn_1(all_by)
fplus_fwd_flip_define_fn_1(all_the_same_by)
fplus_fwd_flip_define_fn_1(all_the_same_on)
fplus_fwd_flip_define_fn_1(numbers)
fplus_fwd_flip_define_fn_1(count_occurrences_by)
fplus_fwd_flip_define_fn_1(lexicographical_less)
fplus_fwd_flip_define_fn_1(replicate)
fplus_fwd_flip_define_fn_1(ok_with_default)
fplus_fwd_flip_define_fn_1(from_maybe)
fplus_fwd_flip_define_fn_1(throw_on_error)
fplus_fwd_flip_define_fn_1(lift_result)
fplus_fwd_flip_define_fn_1(and_then_result)
fplus_fwd_flip_define_fn_1(keep_if)
fplus_fwd_flip_define_fn_1(drop_if)
fplus_fwd_flip_define_fn_1(without)
fplus_fwd_flip_define_fn_1(without_any)
fplus_fwd_flip_define_fn_1(keep_if_with_idx)
fplus_fwd_flip_define_fn_1(drop_if_with_idx)
fplus_fwd_flip_define_fn_1(keep_by_idx)
fplus_fwd_flip_define_fn_1(drop_by_idx)
fplus_fwd_flip_define_fn_1(keep_idxs)
fplus_fwd_flip_define_fn_1(drop_idxs)
fplus_fwd_flip_define_fn_1(drop_idx)
fplus_fwd_flip_define_fn_1(trim_left)
fplus_fwd_flip_define_fn_1(trim_token_left)
fplus_fwd_flip_define_fn_1(trim_right_by)
fplus_fwd_flip_define_fn_1(trim_right)
fplus_fwd_flip_define_fn_1(trim_token_right)
fplus_fwd_flip_define_fn_1(trim_by)
fplus_fwd_flip_define_fn_1(trim)
fplus_fwd_flip_define_fn_1(trim_token)
fplus_fwd_flip_define_fn_1(adjacent_keep_snd_if)
fplus_fwd_flip_define_fn_1(adjacent_drop_fst_if)
fplus_fwd_flip_define_fn_1(adjacent_drop_snd_if)
fplus_fwd_flip_define_fn_1(adjacent_keep_fst_if)
fplus_fwd_flip_define_fn_1(generate_by_idx)
fplus_fwd_flip_define_fn_1(repeat)
fplus_fwd_flip_define_fn_1(infixes)
fplus_fwd_flip_define_fn_1(carthesian_product)
fplus_fwd_flip_define_fn_1(carthesian_product_n)
fplus_fwd_flip_define_fn_1(permutations)
fplus_fwd_flip_define_fn_1(combinations)
fplus_fwd_flip_define_fn_1(combinations_with_replacement)
fplus_fwd_flip_define_fn_1(iterate_maybe)
fplus_fwd_flip_define_fn_1(adjacent_difference_by)
fplus_fwd_flip_define_fn_1(apply_to_pair)
fplus_fwd_flip_define_fn_1(zip)
fplus_fwd_flip_define_fn_1(zip_repeat)
fplus_fwd_flip_define_fn_1(transform_fst)
fplus_fwd_flip_define_fn_1(transform_snd)
fplus_fwd_flip_define_fn_1(abs_diff)
fplus_fwd_flip_define_fn_1(floor_to_int_mult)
fplus_fwd_flip_define_fn_1(ceil_to_int_mult)
fplus_fwd_flip_define_fn_1(int_power)
fplus_fwd_flip_define_fn_1(min_2)
fplus_fwd_flip_define_fn_1(max_2)
fplus_fwd_flip_define_fn_1(histogram_using_intervals)
fplus_fwd_flip_define_fn_1(modulo_chain)
fplus_fwd_flip_define_fn_1(find_first_by)
fplus_fwd_flip_define_fn_1(find_last_by)
fplus_fwd_flip_define_fn_1(find_first_idx_by)
fplus_fwd_flip_define_fn_1(find_last_idx_by)
fplus_fwd_flip_define_fn_1(find_first_idx)
fplus_fwd_flip_define_fn_1(find_last_idx)
fplus_fwd_flip_define_fn_1(find_all_idxs_by)
fplus_fwd_flip_define_fn_1(find_all_idxs_of)
fplus_fwd_flip_define_fn_1(find_all_instances_of_token)
fplus_fwd_flip_define_fn_1(find_all_instances_of_token_non_overlapping)
fplus_fwd_flip_define_fn_1(find_first_instance_of_token)
fplus_fwd_flip_define_fn_1(set_includes)
fplus_fwd_flip_define_fn_1(unordered_set_includes)
fplus_fwd_flip_define_fn_1(set_merge)
fplus_fwd_flip_define_fn_1(unordered_set_merge)
fplus_fwd_flip_define_fn_1(set_intersection)
fplus_fwd_flip_define_fn_1(unordered_set_intersection)
fplus_fwd_flip_define_fn_1(set_is_disjoint)
fplus_fwd_flip_define_fn_1(unordered_set_is_disjoint)
fplus_fwd_flip_define_fn_1(set_difference)
fplus_fwd_flip_define_fn_1(unordered_set_difference)
fplus_fwd_flip_define_fn_1(set_symmetric_difference)
fplus_fwd_flip_define_fn_1(unordered_set_symmetric_difference)
fplus_fwd_flip_define_fn_1(any_by)
fplus_fwd_flip_define_fn_1(none_by)
fplus_fwd_flip_define_fn_1(minimum_idx_by)
fplus_fwd_flip_define_fn_1(minimum_idx_by_maybe)
fplus_fwd_flip_define_fn_1(maximum_idx_by)
fplus_fwd_flip_define_fn_1(maximum_idx_by_maybe)
fplus_fwd_flip_define_fn_1(minimum_idx_on)
fplus_fwd_flip_define_fn_1(minimum_idx_on_maybe)
fplus_fwd_flip_define_fn_1(maximum_idx_on)
fplus_fwd_flip_define_fn_1(maximum_idx_on_maybe)
fplus_fwd_flip_define_fn_1(minimum_by)
fplus_fwd_flip_define_fn_1(minimum_by_maybe)
fplus_fwd_flip_define_fn_1(maximum_by)
fplus_fwd_flip_define_fn_1(maximum_by_maybe)
fplus_fwd_flip_define_fn_1(minimum_on)
fplus_fwd_flip_define_fn_1(minimum_on_maybe)
fplus_fwd_flip_define_fn_1(maximum_on)
fplus_fwd_flip_define_fn_1(maximum_on_maybe)
fplus_fwd_flip_define_fn_1(all_unique_by_less)
fplus_fwd_flip_define_fn_1(is_infix_of)
fplus_fwd_flip_define_fn_1(is_subsequence_of)
fplus_fwd_flip_define_fn_1(count_if)
fplus_fwd_flip_define_fn_1(count)
fplus_fwd_flip_define_fn_1(is_unique_in_by)
fplus_fwd_flip_define_fn_1(is_unique_in)
fplus_fwd_flip_define_fn_1(is_permutation_of)
fplus_fwd_flip_define_fn_1(fill_pigeonholes_to)
fplus_fwd_flip_define_fn_1(fill_pigeonholes_bool_to)
fplus_fwd_flip_define_fn_1(elem_at_idx_or_nothing)
fplus_fwd_flip_define_fn_1(elem_at_idx_or_replicate)
fplus_fwd_flip_define_fn_1(elem_at_idx_or_wrap)
fplus_fwd_flip_define_fn_1(elem_at_float_idx)
fplus_fwd_flip_define_fn_1(transform_map_values)
fplus_fwd_flip_define_fn_1(map_union)
fplus_fwd_flip_define_fn_1(create_map)
fplus_fwd_flip_define_fn_1(create_map_with)
fplus_fwd_flip_define_fn_1(create_map_grouped)
fplus_fwd_flip_define_fn_1(create_unordered_map)
fplus_fwd_flip_define_fn_1(create_unordered_map_with)
fplus_fwd_flip_define_fn_1(create_unordered_map_grouped)
fplus_fwd_flip_define_fn_1(get_from_map)
fplus_fwd_flip_define_fn_1(get_from_map_unsafe)
fplus_fwd_flip_define_fn_1(get_first_from_map)
fplus_fwd_flip_define_fn_1(get_first_from_map_unsafe)
fplus_fwd_flip_define_fn_1(map_contains)
fplus_fwd_flip_define_fn_1(map_keep_if)
fplus_fwd_flip_define_fn_1(map_drop_if)
fplus_fwd_flip_define_fn_1(map_keep)
fplus_fwd_flip_define_fn_1(map_drop)
fplus_fwd_flip_define_fn_1(map_keep_if_value)
fplus_fwd_flip_define_fn_1(map_drop_if_value)
fplus_fwd_flip_define_fn_1(map_keep_values)
fplus_fwd_flip_define_fn_1(map_drop_values)
fplus_fwd_flip_define_fn_1(map_pluck)
fplus_fwd_flip_define_fn_1(choose)
fplus_fwd_flip_define_fn_1(choose_lazy)
fplus_fwd_flip_define_fn_1(choose_def)
fplus_fwd_flip_define_fn_1(choose_def_lazy)
fplus_fwd_flip_define_fn_1(group_by)
fplus_fwd_flip_define_fn_1(group_on)
fplus_fwd_flip_define_fn_1(group_on_labeled)
fplus_fwd_flip_define_fn_1(group_globally_by)
fplus_fwd_flip_define_fn_1(group_globally_on)
fplus_fwd_flip_define_fn_1(group_globally_on_labeled)
fplus_fwd_flip_define_fn_1(cluster_by)
fplus_fwd_flip_define_fn_1(split_by_keep_separators)
fplus_fwd_flip_define_fn_1(split_keep_separators)
fplus_fwd_flip_define_fn_1(split_at_idx)
fplus_fwd_flip_define_fn_1(partition)
fplus_fwd_flip_define_fn_1(split_at_idxs)
fplus_fwd_flip_define_fn_1(split_every)
fplus_fwd_flip_define_fn_1(run_length_encode_by)
fplus_fwd_flip_define_fn_1(span)
fplus_fwd_flip_define_fn_1(aperture)
fplus_fwd_flip_define_fn_1(stride)
fplus_fwd_flip_define_fn_1(winsorize)
fplus_fwd_flip_define_fn_1(separate_on)
fplus_fwd_flip_define_fn_1(transform_with_idx)
fplus_fwd_flip_define_fn_1(transform_and_keep_justs)
fplus_fwd_flip_define_fn_1(transform_and_keep_oks)
fplus_fwd_flip_define_fn_1(transform_and_concat)
fplus_fwd_flip_define_fn_1(replicate_elems)
fplus_fwd_flip_define_fn_1(shuffle)
fplus_fwd_flip_define_fn_1(random_element)
fplus_fwd_flip_define_fn_1(apply_functions)
fplus_fwd_flip_define_fn_1(transform_parallelly)
fplus_fwd_flip_define_fn_1(reduce_1_parallelly)
fplus_fwd_flip_define_fn_1(keep_if_parallelly)
fplus_fwd_flip_define_fn_1(read_value_with_default)
fplus_fwd_flip_define_fn_1(show_cont_with)
fplus_fwd_flip_define_fn_1(split_words)
fplus_fwd_flip_define_fn_1(split_lines)
fplus_fwd_flip_define_fn_1(to_lower_case_loc)
fplus_fwd_flip_define_fn_1(to_upper_case_loc)
fplus_fwd_flip_define_fn_1(trees_from_sequence)
fplus_fwd_flip_define_fn_1(are_trees_equal)

} // namespace fwd
} // namespace fplus
