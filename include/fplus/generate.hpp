// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <fplus/filter.hpp>
#include <fplus/numeric.hpp>
#include <fplus/composition.hpp>

#include <fplus/internal/asserts/functions.hpp>

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
