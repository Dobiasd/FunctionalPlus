// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

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

#include <fplus/internal/invoke.hpp>

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
