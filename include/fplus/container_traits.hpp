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
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <set>
#include <stack>
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
template<class T, class Alloc> struct has_order<std::set<T, Alloc>> : public std::true_type {};
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
    typename ContOut = typename same_cont_new_t<ContIn, typename std::result_of<F(T)>::type, SizeOffset>::type>
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
    typename ContOut = typename same_cont_new_t<ContIn, typename std::result_of<F(T1, T2)>::type, SizeOffset>::type>
struct same_cont_new_t_from_binary_f
{
    typedef ContOut type;
};

} // namespace internal

} // namespace fplus
