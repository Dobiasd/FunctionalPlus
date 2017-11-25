// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <fplus/maybe.hpp>
#include <fplus/result.hpp>
#include <fplus/container_common.hpp>

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
