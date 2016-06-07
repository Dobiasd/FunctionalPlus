// Copyright Tobias Hermann 2015.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "filter.h"
#include "numeric.h"
#include "transform.h"
#include "composition.h"

namespace fplus
{

// API search type: generate : ((() -> a), Int) -> [a]
// generate(f, 3) == [f(), f(), f()]
template <typename ContainerOut, typename F>
ContainerOut generate(F f, std::size_t amount)
{
    check_arity<0, F>();
    ContainerOut ys;
    prepare_container(ys, amount);
    auto it = get_back_inserter<ContainerOut>(ys);
    for (std::size_t i = 0; i < amount; ++i)
    {
        *it = f();
    }
    return ys;
}

// API search type: generate_by_idx : ((Int -> a), Int) -> [a]
// generate_by_idx(f, 3) == [f(0), f(1), f(2)]
template <typename ContainerOut, typename F>
ContainerOut generate_by_idx(F f, std::size_t amount)
{
    check_arity<1, F>();
    typedef typename utils::function_traits<F>::template arg<0>::type FIn;
    static_assert(std::is_convertible<std::size_t, FIn>::value,
        "Function does not take std::size_t or compatible type.");
    ContainerOut ys;
    prepare_container(ys, amount);
    auto it = get_back_inserter<ContainerOut>(ys);
    for (std::size_t i = 0; i < amount; ++i)
    {
        *it = f(i);
    }
    return ys;
}

// API search type: repeat : (Int, [a]) -> [a]
// repeat(3, [1, 2]) == [1, 2, 1, 2, 1, 2]
template <typename Container>
Container repeat(size_t n, const Container& xs)
{
    std::vector<Container> xss(n, xs);
    return concat(xss);
}

// API search type: replicate : (Int, a) -> [a]
// replicate(3, 1) == [1, 1, 1]
template <typename T,
        typename ContainerOut = std::vector<T>>
ContainerOut replicate(size_t n, const T& x)
{
    return ContainerOut(n, x);
}

// API search type: infixes : (Int, [a]) -> [[a]]
// infixes(3, [1,2,3,4,5,6]) == [[1,2,3], [2,3,4], [3,4,5], [4,5,6]]
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
    prepare_container(result, size_of_cont(xs) - length);
    auto itOut = get_back_inserter(result);
    for (std::size_t idx = 0; idx <= size_of_cont(xs) - length; ++idx)
    {
        *itOut = get_range(idx, idx + length, xs);
    }
    return result;
}

// API search type: carthesian_product_with_where : (((a, b) -> c), ((a -> b), Bool), [a], [b]) -> [c]
// carthesian_product_with_where(make_pair, always(true), "ABC", "XY")
//   == [(A,X),(A,Y),(B,X),(B,Y),(C,X),(C,Y)]
// same as (in Haskell):
//   [ f x y | x <- xs, y <- ys, pred x y ]
// same as (in pseudo SQL):
//   SELECT f(xs.x, ys.y)
//   FROM xs, ys
//   WHERE pred(xs.x, ys.y);
template <typename F,
    typename Pred,
    typename Container1,
    typename Container2,
    typename X = typename Container1::value_type,
    typename Y = typename Container2::value_type,
    typename FOut = typename utils::function_traits<F>::result_type,
    typename ContainerOut = std::vector<FOut>>
ContainerOut carthesian_product_with_where(F f, Pred pred,
    const Container1& xs, const Container2& ys)
{
    ContainerOut result;
    auto itOut = get_back_inserter(result);
    for (const auto& x : xs)
    {
        for (const auto& y : ys)
        {
            if (pred(x, y))
            {
                itOut = f(x, y);
            }
        }
    }
    return result;
}

// API search type: carthesian_product_with : (((a, b) -> c), [a], [b]) -> [c]
// carthesian_product_with(make_pair, "ABC", "XY")
// == [(A,X),(A,Y),(B,X),(B,Y),(C,X),(C,Y)]
template <typename F,
    typename Container1,
    typename Container2,
    typename X = typename Container1::value_type,
    typename Y = typename Container2::value_type,
    typename FOut = typename utils::function_traits<F>::result_type,
    typename ContainerOut = std::vector<FOut>>
ContainerOut carthesian_product_with(F f,
    const Container1& xs, const Container2& ys)
{
    auto always_true_x_y = [](const X&, const Y&) { return true; };
    return carthesian_product_with_where(f, always_true_x_y, xs, ys);
}

// API search type: carthesian_product_where : (((a, b) -> Bool), [a], [b]) -> [(a, b)]
// carthesian_product_where(always(true), "ABC", "XY")
// == [(A,X),(A,Y),(B,X),(B,Y),(C,X),(C,Y)]
template <typename Pred,
    typename Container1,
    typename Container2,
    typename X = typename Container1::value_type,
    typename Y = typename Container2::value_type,
    typename Out = std::pair<X, Y>,
    typename ContainerOut = std::vector<Out>>
ContainerOut carthesian_product_where(Pred pred,
    const Container1& xs, const Container2& ys)
{
    auto make_res_pair = [](const X& x, const Y& y)
    {
        return std::make_pair(x, y);
    };
    return carthesian_product_with_where(make_res_pair, pred, xs, ys);
}

// API search type: carthesian_product : ([a], [b]) -> [(a, b)]
// carthesian_product("ABC", "XY") == [(A,X),(A,Y),(B,X),(B,Y),(C,X),(C,Y)]
template <typename Container1,
    typename Container2,
    typename X = typename Container1::value_type,
    typename Y = typename Container2::value_type,
    typename Out = std::pair<X, Y>,
    typename ContainerOut = std::vector<Out>>
ContainerOut carthesian_product(const Container1& xs, const Container2& ys)
{
    auto make_res_pair = [](const X& x, const Y& y)
    {
        return std::make_pair(x, y);
    };
    auto always_true_x_y = [](const X&, const Y&) { return true; };
    return carthesian_product_with_where(
        make_res_pair, always_true_x_y, xs, ys);
}


// productN :: Int -> [a] -> [[a]]
// productN n = foldr go [[]] . replicate n
//     where go elems acc = [x:xs | x <- elems, xs <- acc]
template <typename T>
std::vector<std::vector<T>> internal_helper_carthesian_product_n_idxs
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


// API search type: carthesian_product_n : (Int, [a]) -> [[a]]
// carthesian_product_n(2, "ABCD") == AA AB AC AD BA BB BC BD CA CB CC CD DA DB DC DD
template <typename ContainerIn,
    typename T = typename ContainerIn::value_type,
    typename ContainerOut = std::vector<ContainerIn>>
ContainerOut carthesian_product_n(std::size_t power, const ContainerIn& xs_in)
{
    if (power == 0)
        return ContainerOut(1);
    std::vector<T> xs = convert_container<std::vector<T>>(xs_in);
    auto idxs = all_idxs(xs);
    auto result_idxss = internal_helper_carthesian_product_n_idxs(power, idxs);
    typedef typename ContainerOut::value_type ContainerOutInner;
    auto to_result_cont = [&](const std::vector<std::size_t>& idxs)
    {
        return convert_container_and_elems<ContainerOutInner>(
            elems_at_idxs(idxs, xs));
    };
    return transform(to_result_cont, result_idxss);
}

// API search type: permutations : (Int, [a]) -> [[a]]
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
        internal_helper_carthesian_product_n_idxs(power, idxs));
    typedef typename ContainerOut::value_type ContainerOutInner;
    auto to_result_cont = [&](const std::vector<std::size_t>& idxs)
    {
        return convert_container_and_elems<ContainerOutInner>(
            elems_at_idxs(idxs, xs));
    };
    return transform(to_result_cont, result_idxss);
}

// API search type: combinations : (Int, [a]) -> [[a]]
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
        internal_helper_carthesian_product_n_idxs(power, idxs));
    typedef typename ContainerOut::value_type ContainerOutInner;
    auto to_result_cont = [&](const std::vector<std::size_t>& idxs)
    {
        return convert_container_and_elems<ContainerOutInner>(
            elems_at_idxs(idxs, xs));
    };
    return transform(to_result_cont, result_idxss);
}

// API search type: combinations_with_replacement : (Int, [a]) -> [[a]]
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
        internal_helper_carthesian_product_n_idxs(power, idxs));
    typedef typename ContainerOut::value_type ContainerOutInner;
    auto to_result_cont = [&](const std::vector<std::size_t>& idxs)
    {
        return convert_container_and_elems<ContainerOutInner>(
            elems_at_idxs(idxs, xs));
    };
    return transform(to_result_cont, result_idxss);
}

// API search type: power_set : [a] -> [[a]]
// power_set("xyz") == ["", "x", "y", "z", "xy", "xz", "yz", "xyz"]
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

// API search type: iterate : (((a, a) -> a), Int, a) -> [a]
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
    prepare_container(result, size + 1);
    auto it_out = get_back_inserter(result);
    T current = x;
    *it_out = current;
    for (std::size_t i = 1; i < size; ++i)
    {
        current = f(current);
        *it_out = current;
    }
    return result;
}

// API search type: rotate_left : [a] -> [a]
// rotate_left("xyz") == "yzx"
template <typename Container>
Container rotate_left(const Container& xs)
{
    if (is_empty(xs))
        return xs;
    Container ys;
    auto size = size_of_cont(xs);
    prepare_container(ys, size);
    auto it = std::begin(xs);
    auto it_out = get_back_inserter(ys);
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
// rotate_right("xyz") == "zxy"
template <typename Container>
Container rotate_right(const Container& xs)
{
    return reverse(rotate_left(reverse(xs)));
}

// API search type: rotations_left : [a] -> [[a]]
// rotations_left("abcd") == ["abcd", "bcda", "cdab", "dabc"]
template <typename ContainerIn,
    typename T = typename ContainerIn::value_type,
    typename ContainerOut = std::vector<ContainerIn>>
ContainerOut rotations_left(const ContainerIn& xs_in)
{
    return iterate(rotate_left<ContainerIn>, size_of_cont(xs_in), xs_in);
}

// API search type: rotations_right : [a] -> [[a]]
// rotations_right("abcd") == ["abcd", "dabc", "cdab", "bcda"]
template <typename ContainerIn,
    typename T = typename ContainerIn::value_type,
    typename ContainerOut = std::vector<ContainerIn>>
ContainerOut rotations_right(const ContainerIn& xs_in)
{
    return iterate(rotate_right<ContainerIn>, size_of_cont(xs_in), xs_in);
}

// API search type: fill_left : (a, Int, [a]) -> [a]
// fill_left(0, 6, [1,2,3,4]) == [0,0,1,2,3,4]
template <typename Container,
        typename T = typename Container::value_type>
Container fill_left(const T& x, std::size_t min_size, const Container& xs)
{
    if (min_size <= size_of_cont(xs))
        return xs;
    return append(replicate<T, Container>(min_size - size_of_cont(xs), x), xs);
}

// API search type: fill_right : (a, Int, [a]) -> [a]
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
// inits([0,1,2,3]) == [[],[0],[0,1],[0,1,2],[0,1,2,3]]
template <typename ContainerIn,
    typename T = typename ContainerIn::value_type,
    typename ContainerOut = std::vector<ContainerIn>>
ContainerOut inits(const ContainerIn& xs)
{
    ContainerOut result;
    std::size_t xs_size = size_of_cont(xs);
    prepare_container(result, xs_size + 1);
    auto it_out = get_back_inserter(result);
    for (std::size_t i = 0; i <= xs_size; ++i)
        *it_out = get_range(0, i, xs);
    return result;
}

// API search type: tails : [a] -> [[a]]
// tails([0,1,2,3]) == [[0,1,2,3],[1,2,3],[2,3],[3],[]]
template <typename ContainerIn,
    typename T = typename ContainerIn::value_type,
    typename ContainerOut = std::vector<ContainerIn>>
ContainerOut tails(const ContainerIn& xs)
{
    ContainerOut result;
    std::size_t xs_size = size_of_cont(xs);
    prepare_container(result, xs_size + 1);
    auto it_out = get_back_inserter(result);
    for (std::size_t i = 0; i <= xs_size; ++i)
        *it_out = get_range(i, xs_size, xs);
    return result;
}

} // namespace fplus
