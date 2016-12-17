// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "fplus/composition.hpp"
#include "fplus/container_traits.hpp"
#include "fplus/maybe.hpp"
#include "fplus/compare.hpp"

#include <algorithm>
#include <cassert>
#include <numeric>
#include <iterator>
#include <cmath>

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

    template <typename UnaryPredicate, typename Container>
    void check_index_with_type_predicate_for_container()
    {
        internal::check_index_with_type_predicate_for_type<UnaryPredicate,
            typename Container::value_type>();
    }

    template <typename Compare, typename Container>
    void check_compare_for_container()
    {
        internal::check_compare_for_type<Compare, typename Container::value_type>();
    }

    template <typename BinaryPredicate, typename Container>
    void check_binary_predicate_for_container()
    {
        internal::check_binary_predicate_for_type<BinaryPredicate,
            typename Container::value_type>();
    }

    // PrepareContainer and BackInserter are overloaded
    // to increase performance on std::vector and std::string
    // by using std::vector<T>::reserve
    // and std::back_inserter instead of std::back_inserter.
    // In VC2015, release mode, Celsius W520 Xeon
    // this leads to an increase in performance of about a factor of 3
    // for Transform.
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

    template <typename Container>
    void prepare_container(Container&, std::size_t)
    {
    }

    template <typename Container, typename Y>
    std::back_insert_iterator<Container> get_back_inserter(std::string& ys)
    {
        return std::back_inserter(ys);
    }

    template <typename Container, typename Y>
    std::back_insert_iterator<Container> get_back_inserter(std::vector<Y>& ys)
    {
        return std::back_inserter(ys);
    }

    template <typename Container>
    std::insert_iterator<Container> get_back_inserter(Container& ys)
    {
        return std::inserter(ys, std::end(ys));
    }
} // namespace internal

// API search type: is_empty : [a] -> Bool
// is_empty([1, 2]) == false
template <typename Container>
bool is_empty(const Container& xs)
{
    return xs.empty();
}

// API search type: is_not_empty : [a] -> Bool
// is_not_empty([1, 2]) == true
template <typename Container>
bool is_not_empty(const Container& xs)
{
    return !is_empty(xs);
}

// API search type: size_of_cont : [a] -> Int
// size_of_cont([3, 4]) == 2
template <typename Container>
std::size_t size_of_cont(const Container& xs)
{
    return xs.size();
}

// API search type: convert : a -> b
// Converts one type of element to another.
template <typename Dest, typename Source>
Dest convert(const Source& x)
{
    return Dest(x);
}

// API search type: convert_elems : [a] -> [b]
// convert_elems<NewT>([1, 2, 3]) == [NewT(1), NewT(2), NewT(3)]
template <typename NewT, typename ContainerIn,
    typename ContainerOut = typename same_cont_new_t<ContainerIn, NewT>::type>
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

// API search type: get_range : (Int, Int, [a]) -> [a]
// get_range(2, 5, [0,1,2,3,4,5,6,7,8]) == [2,3,4]
// Also known as slice.
// crashes on invalid indices
template <typename Container>
Container get_range
        (std::size_t idx_begin, std::size_t idx_end, const Container& xs)
{
    assert(idx_begin <= idx_end);
    assert(idx_end <= size_of_cont(xs));
    Container result;
    auto itBegin = std::begin(xs);
    std::advance(itBegin, idx_begin);
    auto itEnd = itBegin;
    std::advance(itEnd, idx_end - idx_begin);
    std::copy(itBegin, itEnd, internal::get_back_inserter(result));
    return result;
}

// API search type: set_range : (Int, [a], [a]) -> [a]
// set_range(2, [9,9,9], [0,1,2,3,4,5,6,7,8]) == [0,1,9,9,9,5,6,7,8]
// crashes on invalid indices
template <typename Container>
Container set_range
        (std::size_t idx_begin, const Container& token, const Container& xs)
{
    assert(idx_begin + size_of_cont(token) < size_of_cont(xs));
    Container result = xs;
    auto itBegin = std::begin(result);
    std::advance(itBegin, idx_begin);
    std::copy(std::begin(token), std::end(token), itBegin);
    return result;
}

// API search type: remove_range : (Int, Int, [a]) -> [a]
// remove_range(2, 5, [0,1,2,3,4,5,6,7]) == [0,1,5,6,7]
// crashes on invalid indices
template <typename Container>
Container remove_range
        (std::size_t idx_begin, std::size_t idx_end, const Container& xs)
{
    assert(idx_begin <= idx_end);
    assert(idx_end <= size_of_cont(xs));

    Container result;
    std::size_t length = idx_end - idx_begin;
    internal::prepare_container(result, size_of_cont(xs) - length);

    auto firstBreakIt = std::begin(xs);
    std::advance(firstBreakIt, idx_begin);
    std::copy(std::begin(xs), firstBreakIt, internal::get_back_inserter(result));

    auto secondBreakIt = firstBreakIt;
    std::advance(secondBreakIt, length);
    std::copy(secondBreakIt, std::end(xs), internal::get_back_inserter(result));

    return result;
}

// API search type: insert_at : (Int, [a], [a]) -> [a]
// insert_at(2, [8,9], [0,1,2,3,4]) == [0,1,8,9,2,3,4]
// crashes on invalid index
template <typename Container>
Container insert_at(std::size_t idx_begin,
        const Container& token, const Container& xs)
{
    assert(idx_begin <= size_of_cont(xs));

    Container result;
    internal::prepare_container(result, size_of_cont(xs) + size_of_cont(token));

    auto breakIt = std::begin(xs);
    std::advance(breakIt, idx_begin);
    std::copy(std::begin(xs), breakIt, internal::get_back_inserter(result));
    std::copy(std::begin(token), std::end(token), internal::get_back_inserter(result));
    std::copy(breakIt, std::end(xs), internal::get_back_inserter(result));

    return result;
}

// API search type: replace_range : (Int, [a], [a]) -> [a]
// replace_range(2, [8,9], [0,1,2,3,4]) == [0,1,8,9,4]
// crashes on invalid index
template <typename Container>
Container replace_range(std::size_t idx_begin,
        const Container& token, const Container& xs)
{
    std::size_t idx_end = idx_begin + size_of_cont(token);
    return insert_at(idx_begin, token, remove_range(idx_begin, idx_end, xs));
}

// API search type: elem_at_idx : (Int, [a]) -> a
// elem_at_idx(2, [7,6,5,4,3]) == 5
// Unsafe!
// Curried version: nth_element
template <typename Container,
    typename T = typename Container::value_type>
T elem_at_idx(std::size_t idx, const Container& xs)
{
    assert(idx < size_of_cont(xs));
    auto it = std::begin(xs);
    std::advance(it, idx);
    return *it;
}

// API search type: elem_at_idx_maybe : (Int, [a]) -> Maybe a
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
    std::advance(it, idx);
    return *it;
}

// API search type: elems_at_idxs : ([Int], [a]) -> [a]
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
    auto itOut = back_inserter(result);
    for (std::size_t idx : idxs)
    {
        *itOut = elem_at_idx(idx, xs);
    }
    return result;
}

// API search type: nth_element : Int -> ([a] -> a)
// nth_element(2)([5,6,7,8]) == 7
// Uncurried version: elem_at_idx
template <typename Container,
        typename T = typename Container::value_type>
std::function<T(const Container& xs)> nth_element(std::size_t n)
{
    return [n](const Container& xs) -> T
    {
        return elem_at_idx(n, xs);
    };
}

// API search type: nth_element_flipped : [a] -> (Int -> a)
// nth_element_flipped([5,6,7,8])(2) == 7
// Can be used to erase outer container type.
template <typename Container,
        typename T = typename Container::value_type>
std::function<T(std::size_t n)> nth_element_flipped(const Container& xs)
{
    return [xs](std::size_t n) -> T
    {
        return elem_at_idx(n, xs);
    };
}

// API search type: transform : ((a -> b), [a]) -> [b]
// transform((*2), [1, 3, 4]) == [2, 6, 8]
// Also known as map.
template <typename F, typename ContainerIn,
    typename ContainerOut = typename same_cont_new_t_from_unary_f<
        ContainerIn, F>::type>
ContainerOut transform(F f, const ContainerIn& xs)
{
    internal::check_arity<1, F>();
    ContainerOut ys;
    internal::prepare_container(ys, size_of_cont(xs));
    auto it = internal::get_back_inserter<ContainerOut>(ys);
    std::transform(std::begin(xs), std::end(xs), it, f);
    return ys;
}

// API search type: transform_convert : ((a -> b), [a]) -> [b]
// transform_convert((*2), [1, 3, 4]) == [2, 6, 8]
// Same as transform, but makes it easy to
// use an output container type different from the input container type.
template <typename ContainerOut, typename F, typename ContainerIn>
ContainerOut transform_convert(F f, const ContainerIn& xs)
{
    internal::check_arity<1, F>();
    ContainerOut ys;
    internal::prepare_container(ys, size_of_cont(xs));
    auto it = internal::get_back_inserter<ContainerOut>(ys);
    std::transform(std::begin(xs), std::end(xs), it, f);
    return ys;
}

// API search type: transform_inner : ((a -> b), [a]) -> [b]
// transform_inner((*2), [[1, 3, 4], [1, 2]]) == [[2, 6, 8], [2, 4]]
// Also known as transform_nested, map_nested or map_inner.
template <typename F, typename ContainerIn,
    typename ContainerOut =
        typename same_cont_new_t<
            ContainerIn,
            typename same_cont_new_t_from_unary_f<
                typename ContainerIn::value_type, F
            >::type
        >::type>
ContainerOut transform_inner(F f, const ContainerIn& xs)
{
    internal::check_arity<1, F>();
    return fplus::transform(
        fplus::bind_1st_of_2(
            fplus::transform<F, typename ContainerIn::value_type>, f),
        xs);
}

// API search type: reverse : [a] -> [a]
// reverse([0,4,2,6]) == [6,2,4,0]
template <typename Container>
Container reverse(const Container& xs)
{
    static_assert(has_order<Container>::value,
        "Reverse: Container has no order.");
    Container ys = xs;
    std::reverse(std::begin(ys), std::end(ys));
    return ys;
}

// API search type: take : (Int, [a]) -> [a]
// take(3, [0,1,2,3,4,5,6,7]) == [0,1,2]
// take(10, [0,1,2]) == [0,1,2]
template <typename Container>
Container take(std::size_t amount, const Container& xs)
{
    if (amount >= size_of_cont(xs))
        return xs;
    return get_range(0, amount, xs);
}

// API search type: take_exact : (Int, [a]) -> [a]
// Unsafe!
// take_exact(3, [0,1,2,3,4,5,6,7]) == [0,1,2]
// take_exact(10, [0,1,2]) == crash
template <typename Container>
Container take_exact(std::size_t amount, const Container& xs)
{
    return get_range(0, amount, xs);
}

// API search type: take_cyclic : (Int, [a]) -> [a]
// take_cyclic(5, [0,1,2,3]) == [0,1,2,3,0]
// take_cyclic(7, [0,1,2,3]) == [0,1,2,3,0,1,2]
// take_cyclic(7, [0,1]) == [0,1,0,1,0,1,0]
// take_cyclic(2, [0,1,2,3]) == [0,1]
// take_cyclic(3, [0]) == [0,0,0]
// take_cyclic(3, []) == crash!
// Also known as take_wrap.
// xs must not be empty.
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
// drop(3, [0,1,2,3,4,5,6,7]) == [3,4,5,6,7]
// Also known as skip.
template <typename Container>
Container drop(std::size_t amount, const Container& xs)
{
    if (amount >= size_of_cont(xs))
        return Container();
    return get_range(amount, size_of_cont(xs), xs);
}

// API search type: drop_exact : (Int, [a]) -> [a]
// Unsafe!
// drop_exact(3, [0,1,2,3,4,5,6,7]) == [3,4,5,6,7]
// drop_exact(10, [0,1,2,3,4,5,6,7]) == crash
template <typename Container>
Container drop_exact(std::size_t amount, const Container& xs)
{
    return get_range(amount, size_of_cont(xs), xs);
}

// API search type: fold_left : (((a, b) -> a), a, [b]) -> a
// fold_left((+), 0, [1, 2, 3]) == ((0+1)+2)+3 == 6
// Takes the second argument and the first item of the list
// and applies the function to them,
// then feeds the function with this result and the second argument and so on.
template <typename F, typename Container,
    typename Acc = typename utils::function_traits<F>::template arg<0>::type>
Acc fold_left(F f, const Acc& init, const Container& xs)
{
    Acc acc = init;
    for (const auto& x : xs)
    {
        acc = f(acc, x);
    }
    return acc;
}

// API search type: fold_left_1 : (((a, a) -> a), [a]) -> a
// fold_left_1((+), [1, 2, 3]) == (1+2)+3 == 6
// Takes the first 2 items of the list and applies the function to them,
// then feeds the function with this result and the third argument and so on.
// xs must not be empty.
template <typename F, typename Container,
    typename Acc = typename Container::value_type>
Acc fold_left_1(F f, const Container& xs)
{
    assert(!xs.empty());
    return fold_left(f, xs.front(), drop(1, xs));
}

// API search type: fold_right : (((a, b) -> b), b) -> [a] -> b
// fold_right((+), 0, [1, 2, 3]) == 1+(2+(3+0)) == 6
// Takes the second argument and the last item of the list
// and applies the function,
// then it takes the penultimate item from the end and the result, and so on.
template <typename F, typename Container,
    typename Acc = typename utils::function_traits<F>::template arg<1>::type>
Acc fold_right(F f, const Acc& init, const Container& xs)
{
    return fold_left(flip(f), init, reverse(xs));
}

// API search type: fold_right_1 : (((a, a) -> a), [a]) -> a
// fold_right_1((+), [1, 2, 3]) == 1+(2+3)) == 6
// Takes the last two items of the list and applies the function,
// then it takes the third item from the end and the result, and so on.
template <typename F, typename Container,
    typename Acc = typename Container::value_type>
Acc fold_right_1(F f, const Container& xs)
{
    return fold_left_1(flip(f), reverse(xs));
}

// API search type: scan_left : (((a, b) -> a), a, [b]) -> [a]
// scan_left((+), 0, [1, 2, 3]) == [0, 1, 3, 6]
// Takes the second argument and the first item of the list
// and applies the function to them,
// then feeds the function with this result and the second argument and so on.
// It returns the list of intermediate and final results.
template <typename F, typename ContainerIn,
    typename Acc = typename utils::function_traits<F>::template arg<0>::type,
    typename ContainerOut = typename same_cont_new_t<ContainerIn, Acc>::type>
ContainerOut scan_left(F f, const Acc& init, const ContainerIn& xs)
{
    ContainerOut result;
    internal::prepare_container(result, size_of_cont(xs));
    auto itOut = internal::get_back_inserter(result);
    Acc acc = init;
    *itOut = acc;
    for (const auto& x : xs)
    {
        acc = f(acc, x);
        *itOut = acc;
    }
    return result;
}

// API search type: scan_left_1 : (((a, a) -> a), [a]) -> [a]
// scan_left_1((+), [1, 2, 3]) == [1, 3, 6]
// Takes the first 2 items of the list and applies the function to them,
// then feeds the function with this result and the third argument and so on.
// It returns the list of intermediate and final results.
// xs must not be empty.
template <typename F, typename ContainerIn,
    typename Acc = typename ContainerIn::value_type,
    typename ContainerOut = typename same_cont_new_t<ContainerIn, Acc>::type>
ContainerOut scan_left_1(F f, const ContainerIn& xs)
{
    assert(!xs.empty());
    return scan_left(f, xs.front(), drop(1, xs));
}

// API search type: scan_right : (((a, b) -> b), b, [a]) -> [b]
// scan_right((+), 0, [1, 2, 3]) == [6, 5, 3, 0]
// Takes the second argument and the last item of the list
// and applies the function,
// then it takes the penultimate item from the end and the result, and so on.
// It returns the list of intermediate and final results.
template <typename F, typename ContainerIn,
    typename Acc = typename utils::function_traits<F>::template arg<1>::type,
    typename ContainerOut = typename same_cont_new_t<ContainerIn, Acc>::type>
ContainerOut scan_right(F f, const Acc& init, const ContainerIn& xs)
{
    return reverse(scan_left(flip(f), init, reverse(xs)));
}

// API search type: scan_right_1 : (((a, a) -> a), [a]) -> [a]
// scan_right_1((+), [1, 2, 3]) == [6, 5, 3]
// Takes the last two items of the list and applies the function,
// then it takes the third item from the end and the result, and so on.
// It returns the list of inntermediate and final results.
template <typename F, typename ContainerIn,
    typename Acc = typename ContainerIn::value_type,
    typename ContainerOut = typename same_cont_new_t<ContainerIn, Acc>::type>
ContainerOut scan_right_1(F f, const ContainerIn& xs)
{
    return reverse(scan_left_1(flip(f), reverse(xs)));
}

// API search type: sum : [a] -> a
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

// API search type: append_elem : ([a], a) -> [a]
// append_elem([1, 2], 3) == [1, 2, 3]
template <typename Container,
    typename T = typename Container::value_type>
Container append_elem(const Container& xs, const T& y)
{
    Container result;
    internal::prepare_container(result, size_of_cont(xs) + 1);
    std::copy(std::begin(xs), std::end(xs),
        internal::get_back_inserter(result));
    *internal::get_back_inserter(result) = y;
    return result;
}

// API search type: prepend_elem : ([a], a) -> [a]
// prepend_elem([2, 3], 1) == [1, 2, 3]
template <typename Container,
    typename T = typename Container::value_type>
Container prepend_elem(const Container& xs, const T& y)
{
    Container result;
    *internal::get_back_inserter(result) = y;
    internal::prepare_container(result, size_of_cont(xs) + 1);
    std::copy(std::begin(xs), std::end(xs),
        internal::get_back_inserter(result));
    return result;
}

// API search type: append : ([a], [a]) -> [a]
// append([1, 2], [3, 4, 5]) == [1, 2, 3, 4, 5]
template <typename Container>
Container append(const Container& xs, const Container& ys)
{
    Container result;
    internal::prepare_container(result, size_of_cont(xs) + size_of_cont(ys));
    std::copy(std::begin(xs), std::end(xs),
        internal::get_back_inserter(result));
    std::copy(std::begin(ys), std::end(ys),
        internal::get_back_inserter(result));
    return result;
}

// API search type: concat : [[a]] -> [a]
// concat([[1, 2], [], [3]]) == [1, 2, 3]
// also known as flatten
template <typename ContainerIn,
    typename ContainerOut = typename ContainerIn::value_type>
ContainerOut concat(const ContainerIn& xss)
{
    std::size_t length = sum(
        transform(size_of_cont<typename ContainerIn::value_type>, xss));
    ContainerOut result;
    internal::prepare_container(result, length);
    for(const auto& xs : xss)
    {
        result.insert(end(result), begin(xs), end(xs));
    }
    return result;
}

// API search type: interweave : ([a], [a]) -> [a]
// Return a list that contains elements from the two provided,
// in alternate order. If one list runs out of items,
// append the items from the remaining list.
// interweave([1,3], [2,4]) == [1,2,3,4]
// interweave([1,3,5,7], [2,4]) == [1,2,3,4,5,7]
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

// sort by given less comparator
template <typename Compare, typename T>
std::list<T> sort_by(Compare comp, const std::list<T>& xs)
{
    auto result = xs;
    result.sort(comp);
    return result;
}

// API search type: sort_by : (((a, a) -> Bool), [a]) -> [a]
// sort by given less comparator
template <typename Compare, typename Container>
Container sort_by(Compare comp, const Container& xs)
{
    auto result = xs;
    std::sort(std::begin(result), std::end(result), comp);
    return result;
}

// API search type: sort_on : ((a -> b), [a]) -> [a]
// sort by given transformer
template <typename F, typename Container>
Container sort_on(F f, const Container& xs)
{
    return sort_by(is_less_by(f), xs);
}

// API search type: sort : [a] -> [a]
// sort by std::less
template <typename Container>
Container sort(const Container& xs)
{
    typedef typename Container::value_type T;
    return sort_by(std::less<T>(), xs);
}

// sort stably by given less comparator
template <typename Compare, typename T>
std::list<T> stable_sort_by(Compare comp, const std::list<T>& xs)
{
    auto result = xs;
    result.sort(comp); // std::list<T>::sort ist already stable.
    return result;
}

// API search type: stable_sort_by : (((a, a) -> Bool), [a]) -> [a]
// sort stably by given less comparator
template <typename Compare, typename Container>
Container stable_sort_by(Compare comp, const Container& xs)
{
    auto result = xs;
    std::stable_sort(std::begin(result), std::end(result), comp);
    return result;
}

// API search type: stable_sort_on : ((a -> b), [a]) -> [a]
// sort stably by given transformer
template <typename F, typename Container>
Container stable_sort_on(F f, const Container& xs)
{
    return stable_sort_by(is_less_by(f), xs);
}

// API search type: stable_sort : [a] -> [a]
// sort stably by std::less
template <typename Container>
Container stable_sort(const Container& xs)
{
    typedef typename Container::value_type T;
    return stable_sort_by(std::less<T>(), xs);
}

// API search type: partial_sort_by : (((a, a) -> Bool), Int, [a]) -> [a]
// partially sort by given less comparator
template <typename Compare, typename Container>
Container partial_sort_by(Compare comp, std::size_t count, const Container& xs)
{
    auto result = xs;
    if (count > xs.size())
    {
        count = xs.size();
    }
    auto middle = std::begin(result);
    std::advance(middle, count);
    std::partial_sort(std::begin(result), middle, std::end(result), comp);
    return get_range(0, count, result);
}

// API search type: partial_sort_on : ((a -> b), Int, [a]) -> [a]
// partially sort by given transformer
template <typename F, typename Container>
Container partial_sort_on(F f, std::size_t count, const Container& xs)
{
    return partial_sort_by(is_less_by(f), count, xs);
}

// API search type: partial_sort : (Int, [a]) -> [a]
// partially sort by std::less
template <typename Container>
Container partial_sort(std::size_t count, const Container& xs)
{
    typedef typename Container::value_type T;
    return partial_sort_by(std::less<T>(), count, xs);
}

// API search type: unique_by : (((a, a) -> Bool), [a]) -> [a]
// Like unique but with user supplied equality predicate.
// O(n)
template <typename Container, typename BinaryPredicate>
Container unique_by(BinaryPredicate p, const Container& xs)
{
    internal::check_binary_predicate_for_container<BinaryPredicate, Container>();
    auto result = xs;
    auto last = std::unique(std::begin(result), std::end(result), p);
    result.erase(last, std::end(result));
    return result;
}

// API search type: unique_on : ((a -> b), [a]) -> [a]
// Like unique but with user supplied transformation (e.g. getter).
// O(n)
// Also known as drop_repeats.
template <typename Container, typename F>
Container unique_on(F f, const Container& xs)
{
    return unique_by(is_equal_by(f), xs);
}

// API search type: unique : [a] -> [a]
// unique([1,2,2,3,2]) == [1,2,3,2]
// O(n)
template <typename Container>
Container unique(const Container& xs)
{
    auto result = xs;
    auto last = std::unique(std::begin(result), std::end(result));
    result.erase(last, std::end(result));
    return result;
}

// API search type: intersperse : (a, [a]) -> [a]
// intersperse(0, [1, 2, 3]) == [1, 0, 2, 0, 3]
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
// Also known as intercalate.
// join(";", "["a", "b", "c"]) == "a;b;c"
// join([0, 0], [[1], [2], [3, 4]]) == [1, 0, 0, 2, 0, 0, 3, 4]
template <typename Container,
    typename X = typename Container::value_type>
X join(const X& separator, const Container& xs)
{
    return concat(intersperse(separator, xs));
}

// API search type: is_elem_of_by : ((a -> Bool), [a]) -> Bool
// Checks if at least one element of the container fulfils a predicate.
// is_elem_of_by((==), [1,2,3]) == true
template <typename UnaryPredicate, typename Container>
bool is_elem_of_by(UnaryPredicate pred, const Container& xs)
{
    return std::find_if(std::begin(xs), std::end(xs), pred) != std::end(xs);
}

// API search type: is_elem_of : (a, [a]) -> Bool
// Checks if an element is a member of a container.
// is_elem_of(2, [1,2,3]) == true
// Equals flip(contains).
template <typename Container>
bool is_elem_of(const typename Container::value_type& x, const Container& xs)
{
    return is_elem_of_by(is_equal_to(x), xs);
}

// API search type: contains : ([a], a) -> Bool
// Checks if an element is a member of a container.
// contains([1,2,3], 2) == true
// Equals flip(is_elem_of).
template <typename Container>
bool contains(const Container& xs, const typename Container::value_type& x)
{
    return is_elem_of(x, xs);
}

// API search type: nub_by : (((a, a) -> Bool), [a]) -> [a]
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
// Makes the elements in a container unique with respect to their function value
// nub_on((mod 10), [12,32,15]) == [12,15]
// O(n^2)
template <typename Container, typename F>
Container nub_on(F f, const Container& xs)
{
    return nub_by(is_equal_by(f), xs);
}

// API search type: nub : [a] -> [a]
// Makes the elements in a container unique
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
// Checks if all elements in a container are unique with respect to a predicate
// Returns true for empty containers.
// O(n^2)
template <typename Container, typename BinaryPredicate>
bool all_unique_by_eq(BinaryPredicate p, const Container& xs)
{
    internal::check_binary_predicate_for_container<BinaryPredicate, Container>();
    return size_of_cont(nub_by(p, xs)) == size_of_cont(xs);
}

// API search type: all_unique_on : ((a -> b), [a]) -> Bool
// Checks if all elements in a container are unique with respect to their function value
// Returns true for empty containers.
// O(n^2)
template <typename Container, typename F>
bool all_unique_on(F f, const Container& xs)
{
    return all_unique_by_eq(is_equal_by(f), xs);
}

// API search type: all_unique : [a] -> Bool
// Checks if all elements in a container are unique
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
        if (!comp(*it1, *it2))
            return false;
    return true;
}

// API search type: is_strictly_sorted_on : ((a -> b), [a]) -> Bool
// O(n)
template <typename Container, typename F>
bool is_strictly_sorted_on(F f, const Container& xs)
{
    return is_strictly_sorted_by(is_less_by(f), xs);
}

// API search type: is_strictly_sorted : [a] -> Bool
// O(n)
template <typename Container>
bool is_strictly_sorted(const Container& xs)
{
    typedef typename Container::value_type T;
    auto comp = std::less<T>();
    return is_strictly_sorted_by(comp, xs);
}

// API search type: is_sorted_by : (((a, a) -> Bool), [a]) -> Bool
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
        if (comp(*it2, *it1))
            return false;
    return true;
}

// API search type: is_sorted_on : ((a -> b), [a]) -> Bool
// O(n)
template <typename Container, typename F>
bool is_sorted_on(F f, const Container& xs)
{
    return is_sorted_by(is_less_by(f), xs);
}

// API search type: is_sorted : [a] -> Bool
// O(n)
template <typename Container>
bool is_sorted(const Container& xs)
{
    typedef typename Container::value_type T;
    auto comp = std::less<T>();
    return is_sorted_by(comp, xs);
}

// API search type: is_prefix_of : ([a], [a]) -> Bool
// is_prefix_of("Fun", "FunctionalPlus") == true
template <typename Container>
bool is_prefix_of(const Container& token, const Container& xs)
{
    if (size_of_cont(token) > size_of_cont(xs))
        return false;
    return get_range(0, size_of_cont(token), xs) == token;
}

// API search type: is_suffix_of : ([a], [a]) -> Bool
// is_suffix_of("us", "FunctionalPlus") == true
template <typename Container>
bool is_suffix_of(const Container& token, const Container& xs)
{
    if (size_of_cont(token) > size_of_cont(xs))
        return false;
    return get_range(size_of_cont(xs) - size_of_cont(token),
        size_of_cont(xs), xs) == token;
}

// API search type: all_by : ((a -> Bool), [a]) -> Bool
// all_by(is_even, [2, 4, 6]) == true
// Returns true for empty containers.
template <typename UnaryPredicate, typename Container>
bool all_by(UnaryPredicate p, const Container& xs)
{
    internal::check_unary_predicate_for_container<UnaryPredicate, Container>();
    return std::all_of(std::begin(xs), std::end(xs), p);
}

// API search type: all : [Bool] -> Bool
// all([true, false, true]) == false
// Returns true for empty containers.
template <typename Container>
bool all(const Container& xs)
{
    typedef typename Container::value_type T;
    return all_by(identity<T>, xs);
}

// API search type: all_the_same_by : (((a, a) -> Bool), [a]) -> Bool
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

// API search type: all_the_same_on : ((a -> Bool), [a]) -> Bool
// Returns true for empty containers.
template <typename Container, typename F>
bool all_the_same_on(F f, const Container& xs)
{
    if (size_of_cont(xs) < 2)
        return true;
    auto unaryPredicate = is_equal_by_to(f, xs.front());
    return all_by(unaryPredicate, xs);
}

// API search type: all_the_same : [a] -> Bool
// Returns true for empty containers.
template <typename Container>
bool all_the_same(const Container& xs)
{
    typedef typename Container::value_type T;
    auto binaryPredicate = std::equal_to<T>();
    return all_the_same_by(binaryPredicate, xs);
}

// API search type: generate_range_step : (a, a, a) -> [a]
// generate_range_step(2, 9, 2) == [2, 4, 6, 8]
template <typename ContainerOut, typename T>
ContainerOut generate_range_step
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
    for (T x = start; x < end; x+=step)
        *it = x;
    return result;
}

// API search type: generate_range : (a, a) -> [a]
// generate_range(2, 9) == [2, 3, 4, 5, 6, 7, 8]
template <typename ContainerOut, typename T = typename ContainerOut::value_type>
ContainerOut generate_range(const T start, const T end)
{
    return generate_range_step<ContainerOut, T>(start, end, 1);
}

// API search type: all_idxs : [a] -> [Int]
// all_idxs([6,4,7,6]) == [0,1,2,3]
template <typename Container>
std::vector<std::size_t> all_idxs(const Container& xs)
{
    return generate_range<std::vector<std::size_t>, std::size_t>
        (0, size_of_cont(xs));
}

// API search type: init : [a] -> [a]
// init([0,1,2,3]) == [0,1,2]
// Unsafe! xs must not be empty.
template <typename Container>
Container init(const Container& xs)
{
    assert(!is_empty(xs));
    return get_range(0, size_of_cont(xs) - 1, xs);
}

// API search type: tail : [a] -> [a]
// Drops the first element of a container, keeps the rest. Unsafe!
// tail([0,1,2,3]) == [1,2,3]
// Unsafe! xs must not be empty.
template <typename Container>
Container tail(const Container& xs)
{
    assert(!is_empty(xs));
    return get_range(1, size_of_cont(xs), xs);
}

// API search type: head : [a] -> a
// Return the first element of a container. Unsafe!
// head([0,1,2,3]) == 0
// Unsafe! xs must not be empty.
template <typename Container>
typename Container::value_type head(const Container& xs)
{
    assert(!is_empty(xs));
    return xs.front();
}

// API search type: mean_stddev : [a] -> (a, a)
// calculates the mean and the population standard deviation
// mean_stddev([4, 8]) == (6, 2)
// mean_stddev([1, 3, 7, 4]) == (3.75, 2.5)
// xs must not be empty.
template <typename Result, typename Container>
std::pair<Result, Result> mean_stddev(const Container& xs)
{
    assert(size_of_cont(xs) != 0);

    // http://stackoverflow.com/a/7616783/1866775
    Result sum = static_cast<Result>(
        std::accumulate(xs.begin(), xs.end(),
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
// count_occurrences_by(floor, [1.1, 2.3, 2.7, 3.6, 2.4]) == [(1, 1), (2, 3), (3, 1)]
// O(n)
template <typename F, typename ContainerIn,
        typename MapOut = typename std::map<
            typename utils::function_traits<F>::result_type, std::size_t>>
MapOut count_occurrences_by(F f, const ContainerIn& xs)
{
    internal::check_arity<1, F>();
    MapOut result;
    for (const auto& x : xs)
    {
        ++result[f(x)];
    }
    return result;
}

// API search type: count_occurrences : [a] -> Map a Int
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
        if (p(*itXs, *itYs))
        {
            return true;
        }
        if (p(*itYs, *itXs))
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
// sort by lexicographical_less
template <typename Container>
Container lexicographical_sort(const Container& xs)
{
    typedef typename Container::value_type T;
    return sort_by(lexicographical_less<T>, xs);
}

// API search type: replicate : (Int, a) -> [a]
// replicate(3, 1) == [1, 1, 1]
template <typename T,
        typename ContainerOut = std::vector<T>>
ContainerOut replicate(std::size_t n, const T& x)
{
    return ContainerOut(n, x);
}

} // namespace fplus
