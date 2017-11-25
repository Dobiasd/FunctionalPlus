// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <functional>
#include <type_traits>
#include <utility>

#include <fplus/internal/meta.hpp>

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
