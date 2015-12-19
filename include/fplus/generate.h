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

// generate(f, 3) == [f(), f(), f()]
template <typename ContainerOut, typename F>
ContainerOut generate(F f, std::size_t amount)
{
    static_assert(utils::function_traits<F>::arity == 0, "Wrong arity.");
    ContainerOut ys;
    prepare_container(ys, amount);
    auto it = get_back_inserter<ContainerOut>(ys);
    for (std::size_t i = 0; i < amount; ++i)
    {
        *it = f();
    }
    return ys;
}

// generate_by_idx(f, 3) == [f(0), f(1), f(2)]
template <typename ContainerOut, typename F>
ContainerOut generate_by_idx(F f, std::size_t amount)
{
    static_assert(utils::function_traits<F>::arity == 1, "Wrong arity.");
    typedef typename utils::function_traits<F>::template arg<0>::type FIn;
    static_assert(std::is_convertible<std::size_t, FIn>::value, "Function does not take std::size_t or compatible type.");
    ContainerOut ys;
    prepare_container(ys, amount);
    auto it = get_back_inserter<ContainerOut>(ys);
    for (std::size_t i = 0; i < amount; ++i)
    {
        *it = f(i);
    }
    return ys;
}

// repeat(3, [1, 2]) == [1, 2, 1, 2, 1, 2]
template <typename Container>
Container repeat(size_t n, const Container& xs)
{
    std::vector<Container> xss(n, xs);
    return concat(xss);
}

// replicate(3, 1) == [1, 1, 1]
template <typename T,
        typename ContainerOut = std::vector<T>>
ContainerOut replicate(size_t n, const T& x)
{
    return ContainerOut(n, x);
}

// infixes(3, [1,2,3,4,5,6]) == [[1,2,3], [2,3,4], [3,4,5], [4,5,6]]
template <typename ContainerIn,
    typename ContainerOut = std::vector<ContainerIn>>
ContainerOut infixes(std::size_t length, const ContainerIn& xs)
{
    assert(length > 0);
    static_assert(std::is_convertible<ContainerIn, typename ContainerOut::value_type>::value, "ContainerOut can not take values of type ContainerIn as elements.");
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

namespace {
    template <typename T>
    std::vector<std::vector<T>> carthesian_product_n_idxs_without_empty
            (std::size_t power, const std::vector<T>& xs)
    {
        static_assert(std::is_same<T, std::size_t>::value, "T must be std::size_t");
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
} // anonymous namespace

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
    auto result_idxss = carthesian_product_n_idxs_without_empty(power, idxs);
    typedef typename ContainerOut::value_type ContainerOutInner;
    auto to_result_cont = [&](const std::vector<std::size_t>& idxs)
    {
        return convert_container_and_elems<ContainerOutInner>(
            elems_at_idxs(idxs, xs));
    };
    return transform(to_result_cont, result_idxss);
}

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
        carthesian_product_n_idxs_without_empty(power, idxs));
    typedef typename ContainerOut::value_type ContainerOutInner;
    auto to_result_cont = [&](const std::vector<std::size_t>& idxs)
    {
        return convert_container_and_elems<ContainerOutInner>(
            elems_at_idxs(idxs, xs));
    };
    return transform(to_result_cont, result_idxss);
}

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
        carthesian_product_n_idxs_without_empty(power, idxs));
    typedef typename ContainerOut::value_type ContainerOutInner;
    auto to_result_cont = [&](const std::vector<std::size_t>& idxs)
    {
        return convert_container_and_elems<ContainerOutInner>(
            elems_at_idxs(idxs, xs));
    };
    return transform(to_result_cont, result_idxss);
}

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
        carthesian_product_n_idxs_without_empty(power, idxs));
    typedef typename ContainerOut::value_type ContainerOutInner;
    auto to_result_cont = [&](const std::vector<std::size_t>& idxs)
    {
        return convert_container_and_elems<ContainerOutInner>(
            elems_at_idxs(idxs, xs));
    };
    return transform(to_result_cont, result_idxss);
}

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

//fill_left(0, 6, [1,2,3,4]) == [0,0,1,2,3,4]
template <typename Container,
        typename T = typename Container::value_type>
Container fill_left(const T& x, std::size_t min_size, const Container& xs)
{
    if (min_size <= size_of_cont(xs))
        return xs;
    return append(replicate<T, Container>(min_size - size_of_cont(xs), x), xs);
}

//fill_right(0, 6, [1,2,3,4]) == [1,2,3,4,0,0]
template <typename Container,
        typename T = typename Container::value_type>
Container fill_right(const T& x, std::size_t min_size, const Container& xs)
{
    if (min_size <= size_of_cont(xs))
        return xs;
    return append(xs, replicate<T, Container>(min_size - size_of_cont(xs), x));
}

} // namespace fplus