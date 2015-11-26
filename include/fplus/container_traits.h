// Copyright Tobias Hermann 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <array>
#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <unordered_map>
#include <queue>
#include <set>
#include <stack>
#include <vector>

namespace fplus
{

template<class T> struct has_order : public std::false_type {};
template<class T, std::size_t N> struct has_order<std::array<T, N>> : public std::true_type {};
template<class T, class Alloc> struct has_order<std::vector<T, Alloc>> : public std::true_type {};
template<class T, class Alloc> struct has_order<std::deque<T, Alloc>> : public std::true_type {};
template<class T, class Alloc> struct has_order<std::forward_list<T, Alloc>> : public std::true_type {};
template<class T, class Alloc> struct has_order<std::list<T, Alloc>> : public std::true_type {};
template<class T, class Container> struct has_order<std::stack<T, Container>> : public std::true_type {};
template<class T, class Container> struct has_order<std::queue<T, Container>> : public std::true_type {};
template<class T, class Container, class Compare> struct has_order<std::priority_queue<T, Container, Compare>> : public std::false_type {};
template<class CharT, class Traits, class Alloc> struct has_order<std::basic_string<CharT, Traits, Alloc>> : public std::true_type {};

//http://stackoverflow.com/a/33828321/1866775
template <class T, class NewP>
struct same_cont_new_t;
template <template <class...> class T, class... TPs, class NewP>
struct same_cont_new_t<T<TPs...>, NewP> {
    using type = T<NewP>;
};

template<class Cont, class NewKey, class NewVal> struct SameMapTypeNewTypes : public std::false_type {};
template<class Key, class T, class Compare, class Alloc, class NewKey, class NewVal> struct SameMapTypeNewTypes<std::map<Key, T, Compare, Alloc>, NewKey, NewVal> { typedef typename std::map<NewKey, NewVal> type; };
template<class Key, class T, class Compare, class Alloc, class NewKey, class NewVal> struct SameMapTypeNewTypes<std::unordered_map<Key, T, Compare, Alloc>, NewKey, NewVal> { typedef typename std::unordered_map<NewKey, NewVal> type; };

template<
    typename ContIn,
    typename F,
    typename T = typename ContIn::value_type,
    typename ContOut = typename same_cont_new_t<ContIn, typename std::result_of<F&(T)>::type>::type>
struct same_cont_new_t_from_unary_f
{
    typedef ContOut type;
};

template<
    typename ContIn,
    typename F,
    typename T1,
    typename T2,
    typename ContOut = typename same_cont_new_t<ContIn, typename std::result_of<F&(T1, T2)>::type>::type>
struct same_cont_new_t_from_binary_f
{
    typedef ContOut type;
};

} // namespace fplus