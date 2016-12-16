// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "fplus/container_common.hpp"
#include "fplus/filter.hpp"
#include "fplus/generate.hpp"
#include "fplus/maybe.hpp"
#include "fplus/maps.hpp"
#include "fplus/result.hpp"
#include "fplus/split.hpp"
#include "fplus/composition.hpp"
#include "fplus/function_traits.hpp"

#include <algorithm>
#include <future>
#include <mutex>
#include <random>

namespace fplus
{

// API search type: transform_with_idx : (((Int, a) -> b), [a]) -> [b]
// transform_with_idx(f, [6, 4, 7]) == [f(0, 6), f(1, 4), f(2, 7)]
template <typename F, typename ContainerIn,
    typename ContainerOut = typename same_cont_new_t_from_binary_f<
        ContainerIn, F, std::size_t, typename ContainerIn::value_type>::type>
ContainerOut transform_with_idx(F f, const ContainerIn& xs)
{
    check_arity<2, F>();
    ContainerOut ys;
    internal::prepare_container(ys, size_of_cont(xs));
    auto it = internal::get_back_inserter<ContainerOut>(ys);
    std::size_t idx = 0;
    for (const auto& x : xs)
    {
        *it = f(idx++, x);
    }
    return ys;
}

// API search type: transform_and_keep_justs : ((a -> Maybe b), [a]) -> [b]
// Map function over values and drop resulting nothings.
// Also known as filter_map.
template <typename F, typename ContainerIn,
    typename FOut = typename utils::function_traits<F>::result_type,
    typename ContainerOut = typename same_cont_new_t<ContainerIn,
        typename FOut::type>::type>
ContainerOut transform_and_keep_justs(F f, const ContainerIn& xs)
{
    check_arity<1, F>();
    auto transformed = transform(f, xs);
    return justs<decltype(transformed), ContainerOut>(transformed);
}

// API search type: transform_and_keep_oks : ((a -> Result b), [a]) -> [b]
// Map function over values and drop resulting errors.
template <typename F, typename ContainerIn,
    typename FOut = typename utils::function_traits<F>::result_type,
    typename ContainerOut = typename same_cont_new_t<
        ContainerIn, typename FOut::ok_t>::type>
ContainerOut transform_and_keep_oks(F f, const ContainerIn& xs)
{
    check_arity<1, F>();
    auto transformed = transform(f, xs);
    return oks<decltype(transformed), ContainerOut>(transformed);
}

// API search type: transform_and_concat : ((a -> [b]), [a]) -> [b]
// Map function over values and concat results.
// Also known as flat_map or concat_map.
template <typename F, typename ContainerIn,
    typename ContainerOut = typename same_cont_new_t_from_unary_f<
        ContainerIn, F>::type::value_type>
ContainerOut transform_and_concat(F f, const ContainerIn& xs)
{
    check_arity<1, F>();
    return concat(transform(f, xs));
}

// API search type: interleave : [[a]] -> [a]
// interleave([[1,2,3],[4,5],[6,7,8]]) == [1,4,6,2,5,7,3,8]
template <typename ContainerIn,
    typename ContainerOut = typename ContainerIn::value_type>
ContainerOut interleave(const ContainerIn& xss)
{
    typedef typename ContainerIn::value_type inner_t;
    typedef std::vector<typename inner_t::const_iterator> its_t;
    const auto inner_cbegin = [](const inner_t& xs) { return xs.cbegin(); };
    const auto inner_cend = [](const inner_t& xs) { return xs.cend(); };
    auto it_pairs = zip(
        transform_convert<its_t>(inner_cbegin, xss),
        transform_convert<its_t>(inner_cend, xss));

    ContainerOut result;
    const std::size_t length = sum(transform(size_of_cont<inner_t>, xss));
    internal::prepare_container(result, length);
    auto it_out = internal::get_back_inserter<ContainerOut>(result);
    bool still_appending = true;
    while (still_appending)
    {
        still_appending = false;
        for (auto& it_pair : it_pairs)
        {
            if (it_pair.first != it_pair.second)
            {
                *it_out = *it_pair.first;
                still_appending = true;
                ++it_pair.first;
            }
        }
    }
    return result;
}

// API search type: transpose : [[a]] -> [[a]]
// transpose([[1,2,3],[4,5,6],[7,8,9]]) == [[1,4,7],[2,5,8],[3,6,9]]
// transpose([[1,2,3],[4,5],[7,8,9]]) == [[1,4,7],[2,5,8],[3,9]]
template <typename Container>
Container transpose(const Container& rows)
{
    if (is_empty(rows))
    {
        return {};
    }
    return split_every<typename Container::value_type, Container>(
        size_of_cont(rows), interleave(rows));
}

// API search type: shuffle : [a] -> [a]
// Returns a shuffled version of xs.
// If you want a different seed, use something like
// std::srand(std::time(nullptr));
template <typename Container>
Container shuffle( const Container& xs)
{
    Container ys = xs;
    std::random_shuffle(begin(ys), end(ys));
    return ys;
}

// API search type: sample : (Int, [a]) -> [a]
// Returns n random elements from xs.
// n has to be smaller than or equal to the number of elements in xs.
// If you want a different seed, use something like
// std::srand(std::time(nullptr));
template <typename Container>
Container sample(std::size_t n, const Container& xs)
{
    assert(n <= size_of_cont(xs));
    return get_range(0, n, shuffle(xs));
}

// API search type: apply_functions : [(a -> b)] -> a -> [b]
// Applies a list of functions to a value.
template <typename FunctionContainer,
    typename F = typename FunctionContainer::value_type,
    typename FIn = typename utils::function_traits<F>::template arg<0>::type,
    typename FOut = typename utils::function_traits<F>::result_type,
    typename ContainerOut = typename same_cont_new_t<FunctionContainer, FOut>::type>
ContainerOut apply_functions(const FunctionContainer& functions, const FIn& x)
{
    static_assert(utils::function_traits<F>::arity == 1, "Wrong arity.");
    ContainerOut ys;
    internal::prepare_container(ys, size_of_cont(functions));
    auto it = internal::get_back_inserter<ContainerOut>(ys);
    for (const auto& f : functions)
    {
        *it = f(x);
    }
    return ys;
}

// API search type: transform_parallelly : ((a -> b), [a]) -> [b]
// transform_parallelly((*2), [1, 3, 4]) == [2, 6, 8]
// Same as transform, but can utilize multiple CPUs by using std::async.
// Only makes sense if one run of the provided function
// takes enough time to justify the synchronization overhead.
// Can be used for applying the MapReduce pattern.
template <typename F, typename ContainerIn,
    typename ContainerOut = typename same_cont_new_t_from_unary_f<
        ContainerIn, F>::type,
    typename X = typename ContainerIn::value_type,
    typename Y = typename utils::function_traits<F>::result_type>
ContainerOut transform_parallelly(F f, const ContainerIn& xs)
{
    check_arity<1, F>();
    auto handles = transform([&f](const X& x) -> std::future<Y>
    {
        return std::async(std::launch::async, [&x, &f]()
            {
                return f(x);
            });
    }, xs);

    ContainerOut ys;
    internal::prepare_container(ys, size_of_cont(xs));
    for (std::future<Y>& handle : handles)
    {
        ys.push_back(handle.get());
    }
    return ys;
}

// API search type: transform_parallelly_n_threads : (Int, (a -> b), [a]) -> [b]
// transform_parallelly_n_threads(4, (*2), [1, 3, 4]) == [2, 6, 8]
// Same as transform, but uses n threads in parallel.
// Only makes sense if one run of the provided function
// takes enough time to justify the synchronization overhead.
// Can be used for applying the MapReduce pattern.
template <typename F, typename ContainerIn,
    typename ContainerOut = typename same_cont_new_t_from_unary_f<
        ContainerIn, F>::type,
    typename X = typename ContainerIn::value_type,
    typename Y = typename utils::function_traits<F>::result_type>
ContainerOut transform_parallelly_n_threads(
    std::size_t n, F f, const ContainerIn& xs)
{
    typedef const X * x_ptr_t;
    auto queue = transform_convert<std::vector<x_ptr_t>>(
        [](const X& x) -> x_ptr_t
        {
            return &x;
        }, xs);

    std::mutex queue_mutex;
    std::mutex thread_results_mutex;
    std::map<std::size_t, Y> thread_results;
    std::size_t queue_idx = 0;

    const auto worker_func = [&]()
    {
        for (;;)
        {
            std::size_t idx = std::numeric_limits<std::size_t>::max();
            x_ptr_t x_ptr = nullptr;
            {
                std::lock_guard<std::mutex> queue_lock(queue_mutex);
                if (queue_idx == queue.size())
                {
                    return;
                }
                idx = queue_idx;
                x_ptr = queue[idx];
                ++queue_idx;
            }

            const auto y = f(*x_ptr);

            {
                std::lock_guard<std::mutex> thread_results_lock(
                    thread_results_mutex);
                thread_results.insert(std::make_pair(idx, y));
            }
        }
    };

    const auto create_thread = [&]() -> std::thread
    {
        return std::thread(worker_func);
    };
    auto threads = generate<std::vector<std::thread>>(create_thread, n);

    for (auto& thread : threads)
    {
        thread.join();
    }

    return get_map_values<decltype(thread_results), ContainerOut>(
        thread_results);
}

} // namespace fplus
