// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <doctest/doctest.h>
#include <functional>
#include <fplus/internal/invoke.hpp>

#include <initializer_list>
#include <stdexcept>
#include <string>
#include <tuple>

using namespace fplus;

#if defined(_MSC_VER) && (_MSC_VER < 1910)
// only one test fails to compile with MSVC 2015, all tests pass with MSVC 2017
#define FPLUS_MSVC2015_BYPASS_FAILING_TESTS
#endif

namespace
{
template <typename T>
struct identity
{
  using type = T;
};

template <typename Ret, typename Class, typename... Args>
auto cv_qualifiers(identity<Ret (Class::*)(Args...)>) {
  return std::tuple<identity<Ret (Class::*)(Args...)>,
                    identity<Ret (Class::*)(Args...) const>,
                    identity<Ret (Class::*)(Args...) volatile>,
                    identity<Ret (Class::*)(Args...) const volatile>>{};
}

template <typename Ret, typename Class, typename... Args>
auto ref_qualifiers(identity<Ret (Class::*)(Args...)>)
{
  return std::tuple<identity<Ret (Class::*)(Args...)&>,
                    identity<Ret (Class::*)(Args...) &&>>{};
}

template <typename Ret, typename Class, typename... Args>
auto ref_qualifiers(identity<Ret (Class::*)(Args...) const>)
{
  return std::tuple<identity<Ret (Class::*)(Args...) const&>,
                    identity<Ret (Class::*)(Args...) const &&>>{};
}

template <typename Ret, typename Class, typename... Args>
auto ref_qualifiers(identity<Ret (Class::*)(Args...) volatile>)
{
  return std::tuple<identity<Ret (Class::*)(Args...) volatile&>,
                    identity<Ret (Class::*)(Args...) volatile &&>>{};
}

template <typename Ret, typename Class, typename... Args>
auto ref_qualifiers(identity<Ret (Class::*)(Args...) const volatile>)
{
  return std::tuple<identity<Ret (Class::*)(Args...) const volatile&>,
                    identity<Ret (Class::*)(Args...) const volatile&&>>{};
}

template <typename T>
auto ref_qualifiers(identity<T>)
{
  return std::tuple<identity<T&>, identity<T&&>>{};
}

template <typename T>
auto all_qualifiers(identity<T> f)
{
  auto a = cv_qualifiers(f);
  // make_index_sequence would work, but no need
  auto b = ref_qualifiers(std::get<0>(a));
  auto c = ref_qualifiers(std::get<1>(a));
  auto d = ref_qualifiers(std::get<2>(a));
  auto e = ref_qualifiers(std::get<3>(a));

  return std::make_tuple(std::get<0>(b),
                         std::get<1>(b),
                         std::get<0>(c),
                         std::get<1>(c),
                         std::get<0>(d),
                         std::get<1>(d),
                         std::get<0>(e),
                         std::get<1>(e));
}

template <typename... Args>
struct all_invocable;

template <typename... TupleArgs, typename Class, typename... FuncArgs>
struct all_invocable<std::tuple<TupleArgs...>, Class, FuncArgs...>
{
  using Tuple = std::tuple<TupleArgs...>;

  // two ::type, becaus of identity wrapper
  template <std::size_t N, typename T>
  using Elem = typename std::tuple_element<N, T>::type::type;

  static_assert(sizeof...(TupleArgs) == 8,
                "all_invocable applies to each cv-ref qualified overloads");

  // Class& because `&` functions can only be invokej on lvalue references
  static constexpr bool value = internal::conjunction<
      internal::is_invocable<Elem<0, Tuple>, Class&, FuncArgs...>,
      internal::is_invocable<Elem<1, Tuple>, Class, FuncArgs...>,
      internal::is_invocable<Elem<2, Tuple>, Class&, FuncArgs...>,
      internal::is_invocable<Elem<3, Tuple>, Class, FuncArgs...>,
      internal::is_invocable<Elem<4, Tuple>, Class&, FuncArgs...>,
      internal::is_invocable<Elem<5, Tuple>, Class, FuncArgs...>,
      internal::is_invocable<Elem<6, Tuple>, Class&, FuncArgs...>,
      internal::is_invocable<Elem<7, Tuple>, Class, FuncArgs...>>::value;
};
}

namespace
{
int regular_function_sum(int a, int b)
{
  return a + b;
}

template <unsigned int N, typename ...Args>
auto return_n_arg_type(Args&&... args) -> typename std::tuple_element<N, std::tuple<Args...>>::type
{
  return std::get<N>(std::forward_as_tuple(std::forward<Args>(args)...));
}

struct function_object_t
{
  int i = 0;

  int operator()(int a, int b) const
  {
    return a + b;
  }

  void mutate_data()
  {
    i = 0;
  }
};

struct derived_function_object_t : function_object_t
{
};
}

TEST_CASE("regular function")
{
  using regular_function_t = decltype(regular_function_sum);
  using regular_function_ptr_t = std::add_pointer<decltype(regular_function_sum)>::type;

  // implicit conversions work
  static_assert(internal::is_invocable<regular_function_t, int, unsigned int>::value, "");
  static_assert(internal::is_invocable_r<bool, regular_function_t, int, unsigned int>::value, "");

  static_assert(!internal::is_invocable<regular_function_t, int, char*>::value, "");
  static_assert(!internal::is_invocable<regular_function_t, int, char, char>::value, "");
  static_assert(!internal::is_invocable_r<std::string, regular_function_t, int, unsigned int>::value, "");

  static_assert(internal::is_invocable<regular_function_ptr_t, int, unsigned int>::value, "");
  static_assert(internal::is_invocable_r<bool, regular_function_ptr_t, int, unsigned int>::value, "");

  static_assert(!internal::is_invocable<regular_function_ptr_t, int, char*>::value, "");
  static_assert(!internal::is_invocable<regular_function_ptr_t, int, char, char>::value, "");
  static_assert(!internal::is_invocable_r<std::string, regular_function_ptr_t, int, unsigned int>::value, "");

  REQUIRE_EQ(internal::invoke(regular_function_sum, 32, 10), 42);
}

TEST_CASE("regular variadic function")
{
  int i = 42;

  using variadic_function_t = decltype(return_n_arg_type<0, int&, float>);

  static_assert(internal::is_invocable<variadic_function_t, int&, float>::value, "");
  static_assert(internal::is_invocable_r<const int&, variadic_function_t, int&, float>::value, "");

  static_assert(!internal::is_invocable<variadic_function_t, int, float>::value, "");
  static_assert(!internal::is_invocable_r<short&, variadic_function_t, int&, float>::value, "");

  REQUIRE_EQ(std::addressof(internal::invoke(return_n_arg_type<0, int&, float>, i, 2.0f)),
             std::addressof(i));
}

TEST_CASE("function object")
{
  static_assert(internal::is_invocable<function_object_t, int const&, double>::value, "");
  static_assert(internal::is_invocable_r<int&&, function_object_t, int&, float>::value, "");

  static_assert(!internal::is_invocable<function_object_t, int, std::string>::value, "");
  static_assert(!internal::is_invocable_r<int&, function_object_t, int, int>::value, "");

  REQUIRE_EQ(internal::invoke(function_object_t{}, 40, 2), 42);
}

TEST_CASE("lambda")
{
  auto add = [](int a, int b) { return a + b; };

  using lambda_t = decltype(add);

  static_assert(internal::is_invocable<lambda_t, int const&, double>::value, "");
  static_assert(internal::is_invocable_r<int&&, lambda_t, int&, float>::value, "");

  static_assert(!internal::is_invocable<lambda_t, int, std::string>::value, "");
  static_assert(!internal::is_invocable_r<int&, lambda_t, int, int>::value, "");

  REQUIRE_EQ(internal::invoke(add, 40, 2), 42);
}

TEST_CASE("member function - object reference")
{
  using call_operator_t = decltype(&function_object_t::operator());
  using mutate_data_t = decltype(&function_object_t::mutate_data);

  auto qualifiers =
      all_qualifiers(identity<int (function_object_t::*)(int, int)>{});
  static_assert(all_invocable<decltype(qualifiers), function_object_t, int const&, double>::value, "");

  static_assert(internal::is_invocable<call_operator_t, function_object_t, int const&, double>::value, "");
  static_assert(internal::is_invocable<mutate_data_t, function_object_t>::value, "");
  static_assert(internal::is_invocable_r<int&&, call_operator_t, function_object_t, int&, float>::value, "");

  // non-const member function
  static_assert(internal::is_invocable<mutate_data_t, function_object_t&>::value, "");
  static_assert(!internal::is_invocable<mutate_data_t, const function_object_t&>::value, "");

  static_assert(!internal::is_invocable_r<int&, call_operator_t, function_object_t, int, int>::value, "");

  auto adder = function_object_t{};
  REQUIRE_EQ(internal::invoke(&function_object_t::operator(), adder, 40, 2), 42);
}

TEST_CASE("member function - reference_wrapper<object>")
{
  using call_operator_t = decltype(&function_object_t::operator());
  using mutate_data_t = decltype(&function_object_t::mutate_data);
  using ref_wrapper_t = std::reference_wrapper<function_object_t>;
  using ref_wrapper_const_t = std::reference_wrapper<const function_object_t>;

  static_assert(internal::is_invocable<call_operator_t, ref_wrapper_t, int const&, double>::value, "");
  static_assert(internal::is_invocable<call_operator_t, ref_wrapper_const_t, int const&, double>::value, "");
  static_assert(internal::is_invocable_r<int&&, call_operator_t, ref_wrapper_t, int&, float>::value, "");
  static_assert(internal::is_invocable_r<int&&, call_operator_t, ref_wrapper_const_t, int&, float>::value, "");

  // non-const member function
  static_assert(internal::is_invocable<mutate_data_t, ref_wrapper_t>::value, "");
  static_assert(!internal::is_invocable<mutate_data_t, ref_wrapper_const_t>::value, "");

  static_assert(!internal::is_invocable_r<int&, call_operator_t, ref_wrapper_t, int, int>::value, "");

  auto adder = function_object_t{};
  REQUIRE_EQ(internal::invoke(&function_object_t::operator(), std::ref(adder), 40, 2), 42);
  REQUIRE_EQ(internal::invoke(&function_object_t::operator(), std::cref(adder), 40, 2), 42);
}

TEST_CASE("member function - object pointer")
{
  using call_operator_t = decltype(&function_object_t::operator());
  using mutate_data_t = decltype(&function_object_t::mutate_data);

  static_assert(internal::is_invocable<call_operator_t, function_object_t*, int const&, double>::value, "");
  static_assert(internal::is_invocable_r<int&&, call_operator_t, function_object_t*, int&, float>::value, "");

  // non-const member function
  static_assert(internal::is_invocable<mutate_data_t, function_object_t*>::value, "");
  static_assert(!internal::is_invocable<mutate_data_t, const function_object_t*>::value, "");

  static_assert(!internal::is_invocable_r<int&, call_operator_t, function_object_t*, int, int>::value, "");

  auto adder = function_object_t{};
  REQUIRE_EQ(internal::invoke(&function_object_t::operator(), &adder, 40, 2), 42);
}

TEST_CASE("member function - derived object reference")
{
  using call_operator_t = decltype(&function_object_t::operator());
  using mutate_data_t = decltype(&function_object_t::mutate_data);

  // should split all_qualifiers to get specific ones, right now it cannot
  // be used to test const objects and reference_wrapper.
  // Need to add make_index_sequence to do that properly.
  auto qualifiers =
      all_qualifiers(identity<int (function_object_t::*)(int, int)>{});
#if !defined(FPLUS_MSVC2015_BYPASS_FAILING_TESTS) // Error C2338 under MSVC (i.e static_assert fail)
  static_assert(all_invocable<decltype(qualifiers), derived_function_object_t, int const&, double>::value, "");
#endif

  static_assert(internal::is_invocable<call_operator_t, derived_function_object_t, int const&, double>::value, "");
  static_assert(internal::is_invocable_r<int&&, call_operator_t, derived_function_object_t, int&, float>::value, "");

  // non-const member function
  static_assert(internal::is_invocable<mutate_data_t, derived_function_object_t&>::value, "");
  static_assert(!internal::is_invocable<mutate_data_t, const derived_function_object_t&>::value, "");

  static_assert(!internal::is_invocable_r<int&, call_operator_t, derived_function_object_t&, int, int>::value, "");

  auto adder = derived_function_object_t{};
  REQUIRE_EQ(internal::invoke(&function_object_t::operator(), adder, 40, 2), 42);
}

TEST_CASE("member function - reference_wrapper<derived object>")
{
  using call_operator_t = decltype(&function_object_t::operator());
  using mutate_data_t = decltype(&function_object_t::mutate_data);

  using ref_wrapper_t = std::reference_wrapper<derived_function_object_t>;
  using ref_wrapper_const_t = std::reference_wrapper<const derived_function_object_t>;

  static_assert(internal::is_invocable<call_operator_t, ref_wrapper_t, int const&, double>::value, "");
  static_assert(internal::is_invocable<call_operator_t, ref_wrapper_const_t, int const&, double>::value, "");
  static_assert(internal::is_invocable_r<int&&, call_operator_t, ref_wrapper_t, int&, float>::value, "");

  // non-const member function
  static_assert(internal::is_invocable<mutate_data_t, ref_wrapper_t>::value, "");
  static_assert(!internal::is_invocable<mutate_data_t, ref_wrapper_const_t>::value, "");

  static_assert(!internal::is_invocable_r<int&, call_operator_t, ref_wrapper_t&, int, int>::value, "");

  auto adder = derived_function_object_t{};
  REQUIRE_EQ(internal::invoke(&function_object_t::operator(), adder, 40, 2), 42);
}

TEST_CASE("member function - derived object pointer")
{
  using call_operator_t = decltype(&function_object_t::operator());
  using mutate_data_t = decltype(&function_object_t::mutate_data);

  static_assert(internal::is_invocable<call_operator_t, derived_function_object_t*, int const&, double>::value, "");
  static_assert(internal::is_invocable_r<int&&, call_operator_t, derived_function_object_t*, int&, float>::value, "");

  // non-const non-volatile member function
  static_assert(internal::is_invocable<mutate_data_t, derived_function_object_t*>::value, "");
  static_assert(!internal::is_invocable<mutate_data_t, const derived_function_object_t*>::value, "");
  static_assert(!internal::is_invocable<mutate_data_t, volatile derived_function_object_t*>::value, "");

  static_assert(!internal::is_invocable_r<int&, call_operator_t, derived_function_object_t*, int, int>::value, "");

  auto adder = derived_function_object_t{};
  REQUIRE_EQ(internal::invoke(&function_object_t::operator(), &adder, 40, 2), 42);
}

TEST_CASE("member data - object reference")
{
  using member_data_t = decltype(&function_object_t::i);

  static_assert(internal::is_invocable<member_data_t, function_object_t>::value, "");
  static_assert(internal::is_invocable_r<int&&, member_data_t, function_object_t>::value, "");

  // cannot convert lvalue ref to rvalue-reference
  static_assert(!internal::is_invocable_r<int&&, member_data_t, function_object_t&>::value, "");

  static_assert(!internal::is_invocable<member_data_t, function_object_t, int>::value, "");

  auto obj = function_object_t{};
  obj.i = 42;
  REQUIRE_EQ(internal::invoke(&function_object_t::i, obj), 42);
}

TEST_CASE("member data - reference_wrapper<object>")
{
  using member_data_t = decltype(&function_object_t::i);
  using ref_wrapper_t = std::reference_wrapper<function_object_t>;
  using ref_wrapper_const_t = std::reference_wrapper<const function_object_t>;

  static_assert(internal::is_invocable<member_data_t, ref_wrapper_t>::value, "");
  static_assert(internal::is_invocable_r<int const&, member_data_t, ref_wrapper_const_t>::value, "");

  // cannot convert lvalue ref to rvalue-reference
  static_assert(!internal::is_invocable_r<int&&, member_data_t, ref_wrapper_t>::value, "");
  // nor from const lvalue reference to non-const lvalue reference
  static_assert(!internal::is_invocable_r<int&, member_data_t, ref_wrapper_const_t>::value, "");

  auto obj = function_object_t{};
  obj.i = 42;

  REQUIRE_EQ(internal::invoke(&function_object_t::i, std::ref(obj)), 42);
  REQUIRE_EQ(internal::invoke(&function_object_t::i, std::cref(obj)), 42);
}

TEST_CASE("member data - object pointer")
{
  using member_data_t = decltype(&function_object_t::i);

  static_assert(internal::is_invocable<member_data_t, function_object_t*>::value, "");
  static_assert(internal::is_invocable_r<int&, member_data_t, function_object_t*>::value, "");

  // cannot convert lvalue ref to rvalue-reference
  static_assert(!internal::is_invocable_r<int&&, member_data_t, function_object_t*>::value, "");
  static_assert(!internal::is_invocable_r<int&, member_data_t, const function_object_t*>::value, "");

  static_assert(!internal::is_invocable<member_data_t, function_object_t*, int>::value, "");

  auto obj = function_object_t{};
  obj.i = 42;
  REQUIRE_EQ(internal::invoke(&function_object_t::i, &obj), 42);
}

TEST_CASE("member data - derived object reference")
{
  using member_data_t = decltype(&function_object_t::i);

  static_assert(internal::is_invocable<member_data_t, derived_function_object_t>::value, "");
  static_assert(internal::is_invocable_r<int&&, member_data_t, derived_function_object_t>::value, "");

  // cannot convert lvalue ref to rvalue-reference
  static_assert(!internal::is_invocable_r<int&&, member_data_t, derived_function_object_t&>::value, "");

  static_assert(!internal::is_invocable<member_data_t, derived_function_object_t, int>::value, "");

  auto obj = derived_function_object_t{};
  obj.i = 42;
  REQUIRE_EQ(internal::invoke(&function_object_t::i, obj), 42);
}

TEST_CASE("member data - reference_wrapper<derived object>")
{
  using member_data_t = decltype(&function_object_t::i);

  using ref_wrapper_t = std::reference_wrapper<derived_function_object_t>;
  using ref_wrapper_const_t = std::reference_wrapper<const derived_function_object_t>;

  static_assert(internal::is_invocable<member_data_t, ref_wrapper_t>::value, "");
  static_assert(internal::is_invocable_r<int const&, member_data_t, ref_wrapper_const_t>::value, "");

  // cannot convert lvalue ref to rvalue-reference
  static_assert(!internal::is_invocable_r<int&&, member_data_t, ref_wrapper_t>::value, "");
  // nor from const lvalue reference to non-const lvalue reference
  static_assert(!internal::is_invocable_r<int&, member_data_t, ref_wrapper_const_t>::value, "");

  auto obj = derived_function_object_t{};
  obj.i = 42;

  REQUIRE_EQ(internal::invoke(&function_object_t::i, std::ref(obj)), 42);
  REQUIRE_EQ(internal::invoke(&function_object_t::i, std::cref(obj)), 42);
}

TEST_CASE("member data - derived object pointer")
{
  using member_data_t = decltype(&function_object_t::i);

  static_assert(internal::is_invocable<member_data_t, derived_function_object_t*>::value, "");
  static_assert(internal::is_invocable_r<int&, member_data_t, derived_function_object_t*>::value, "");

  // cannot convert lvalue ref to rvalue-reference
  static_assert(!internal::is_invocable_r<int&&, member_data_t, derived_function_object_t*>::value, "");
  static_assert(!internal::is_invocable_r<int&, member_data_t, const derived_function_object_t*>::value, "");

  static_assert(!internal::is_invocable<member_data_t, derived_function_object_t*, int>::value, "");

  auto obj = derived_function_object_t{};
  obj.i = 42;
  REQUIRE_EQ(internal::invoke(&derived_function_object_t::i, &obj), 42);
}

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#endif // __GNUC__
TEST_CASE("generic lambda")
{
  auto add = [](auto a, auto b) { return a + b; };

  using lambda_t = decltype(add);

  static_assert(internal::is_invocable<lambda_t, int const&, double>::value, "");
  static_assert(internal::is_invocable_r<int&&, lambda_t, int&, float>::value, "");

  // compile error, static_assert doesn't trigger though
  // from cppreference:
  //
  // Formally, determines whether INVOKE(declval<Fn>(),
  // declval<ArgTypes>()...) is well formed when treated as an unevaluated
  // operand, where INVOKE is the operation defined in Callable.
  //
  // This is indeed well-formed in the unevaluated context...
  // static_assert(!internal::is_invocable<lambda_t, int, std::string>::value, "");

  static_assert(!internal::is_invocable_r<int&, lambda_t, int, int>::value, "");

  REQUIRE_EQ(internal::invoke(add, 40, 2), 42);
}

TEST_CASE("transparent function objects")
{
  static_assert(internal::is_invocable<std::plus<>, int, int>::value, "");
  static_assert(internal::is_invocable<std::plus<>, int, float>::value, "");

  REQUIRE_EQ(internal::invoke(std::plus<>{}, 40, 2), 42);
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif // __GNUC__
