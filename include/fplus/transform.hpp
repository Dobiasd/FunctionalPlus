// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <fplus/container_common.hpp>
#include <fplus/filter.hpp>
#include <fplus/generate.hpp>
#include <fplus/maybe.hpp>
#include <fplus/maps.hpp>
#include <fplus/result.hpp>
#include <fplus/split.hpp>
#include <fplus/composition.hpp>
#include <fplus/function_traits.hpp>

#include <algorithm>
#include <future>
#include <iterator>
#include <mutex>
#include <random>

namespace fplus
{

// API search type: transform_with_idx : (((Int, a) -> b), [a]) -> [b]
// fwd bind count: 1
// transform_with_idx(f, [6, 4, 7]) == [f(0, 6), f(1, 4), f(2, 7)]
template <typename F, typename ContainerIn,
    typename ContainerOut = typename internal::same_cont_new_t_from_binary_f<
        ContainerIn, F, std::size_t, typename ContainerIn::value_type>::type>
ContainerOut transform_with_idx(F f, const ContainerIn& xs)
{
    internal::check_arity<2, F>();
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
// fwd bind count: 1
// Map function over values and drop resulting nothings.
// Also known as filter_map.
template <typename F, typename ContainerIn,
    typename FIn = typename utils::function_traits<F>::template arg<0>::type,
    typename FOut = typename std::result_of<F(FIn)>::type,
    typename ContainerOut = typename internal::same_cont_new_t<ContainerIn,
        typename FOut::type>::type>
ContainerOut transform_and_keep_justs(F f, const ContainerIn& xs)
{
    internal::check_arity<1, F>();
    auto transformed = transform(f, xs);
    return justs<decltype(transformed), ContainerOut>(transformed);
}

// API search type: transform_and_keep_oks : ((a -> Result b), [a]) -> [b]
// fwd bind count: 1
// Map function over values and drop resulting errors.
template <typename F, typename ContainerIn,
    typename FIn = typename utils::function_traits<F>::template arg<0>::type,
    typename FOut = typename std::result_of<F(FIn)>::type,
    typename ContainerOut = typename internal::same_cont_new_t<
        ContainerIn, typename FOut::ok_t>::type>
ContainerOut transform_and_keep_oks(F f, const ContainerIn& xs)
{
    internal::check_arity<1, F>();
    auto transformed = transform(f, xs);
    return oks<decltype(transformed), ContainerOut>(transformed);
}

// API search type: transform_and_concat : ((a -> [b]), [a]) -> [b]
// fwd bind count: 1
// Map function over values and concat results.
// Also known as flat_map or concat_map.
template <typename F, typename ContainerIn,
    typename ContainerOut = typename internal::same_cont_new_t_from_unary_f<
        ContainerIn, F>::type::value_type>
ContainerOut transform_and_concat(F f, const ContainerIn& xs)
{
    internal::check_arity<1, F>();
    return concat(transform(f, xs));
}

// API search type: replicate_elems : (Int, [a]) -> [a]
// fwd bind count: 1
// replicate_elems(3, [1,2]) == [1, 1, 1, 2, 2, 2]
template <typename Container>
Container replicate_elems(std::size_t n, const Container& xs)
{
    typedef typename Container::value_type T;
    return transform_and_concat(bind_1st_of_2(replicate<T, Container>, n), xs);
}

// API search type: interleave : [[a]] -> [a]
// fwd bind count: 0
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
// fwd bind count: 0
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

// API search type: shuffle : (Int, [a]) -> [a]
// fwd bind count: 1
// Returns a randomly shuffled version of xs.
// Example call: shuffle(std::mt19937::default_seed, xs);
// Example call: shuffle(std::random_device()(), xs);
template <typename Container>
Container shuffle(std::uint_fast32_t seed, const Container& xs)
{
    std::mt19937 g(seed);
    Container ys = xs;
    std::shuffle(std::begin(ys), std::end(ys), g);
    return ys;
}

// API search type: sample : (Int, Int, [a]) -> [a]
// fwd bind count: 2
// Returns n random elements from xs without replacement.
// n has to be smaller than or equal to the number of elements in xs.
// Also known as rnd_select.
// Example call: sample(std::mt19937::default_seed, 3, xs);
// Example call: sample(std::random_device()(), 3, xs);
template <typename Container>
Container sample(std::uint_fast32_t seed, std::size_t n, const Container& xs)
{
    assert(n <= size_of_cont(xs));
    return get_segment(0, n, shuffle(seed, xs));
}

// API search type: random_element : (Int, [a]) -> a
// fwd bind count: 1
// Returns one random element from xs.
// xs must be non-empty.
// Example call: random_element(std::mt19937::default_seed, xs)
// Example call: random_element(std::random_device()(), xs)
// Also known as choice.
template <typename Container>
typename Container::value_type random_element(
    std::uint_fast32_t seed, const Container& xs)
{
    assert(is_not_empty(xs));
    std::mt19937 gen(seed);
    std::uniform_int_distribution<std::size_t> dis(0, size_of_cont(xs) - 1);
    return elem_at_idx(dis(gen), xs);
}

// API search type: random_elements : (Int, Int, [a]) -> a
// fwd bind count: 2
// Returns random elements from xs with replacement.
// xs must be non-empty.
// Example call: random_elements(std::mt19937::default_seed, 10, xs)
// Example call: random_elements(std::random_device()(), 10, xs)
template <typename Container>
Container random_elements(
    std::uint_fast32_t seed, std::size_t n, const Container& xs)
{
    assert(is_not_empty(xs));
    std::mt19937 gen(seed);
    std::uniform_int_distribution<std::size_t> dis(0, size_of_cont(xs) - 1);
    const auto draw = [&]() -> typename Container::value_type
    {
        return elem_at_idx(dis(gen), xs);
    };
    return generate<Container>(draw, n);
}

// API search type: apply_functions : ([(a -> b)], a) -> [b]
// fwd bind count: 1
// Applies a list of functions to a value.
template <typename FunctionContainer,
    typename F = typename FunctionContainer::value_type,
    typename FIn = typename utils::function_traits<F>::template arg<0>::type,
    typename FOut = typename std::result_of<F(FIn)>::type,
    typename ContainerOut = typename internal::same_cont_new_t<FunctionContainer, FOut>::type>
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

// API search type: apply_function_n_times : ((a -> a), Int, a) -> a
// fwd bind count: 2
// Applies a functional n times in a row.
template <typename F,
    typename FIn = typename utils::function_traits<F>::template arg<0>::type,
    typename FOut = typename std::result_of<F(FIn)>::type>
FOut apply_function_n_times(const F f, std::size_t n, const FIn& x)
{
    static_assert(utils::function_traits<F>::arity == 1, "Wrong arity.");
    static_assert(std::is_same<FOut, FIn>::value,
        "Input and output of F must be the same type.");
    if (n == 0)
    {
        return x;
    }
    FOut y = f(x);
    for (std::size_t i = 1; i < n; ++i)
    {
        y = f(y);
    }
    return y;
}

// API search type: transform_parallelly : ((a -> b), [a]) -> [b]
// fwd bind count: 1
// transform_parallelly((*2), [1, 3, 4]) == [2, 6, 8]
// Same as transform, but can utilize multiple CPUs by using std::async.
// Only makes sense if one run of the provided function
// takes enough time to justify the synchronization overhead.
template <typename F, typename ContainerIn,
    typename ContainerOut = typename internal::same_cont_new_t_from_unary_f<
        ContainerIn, F>::type,
    typename X = typename ContainerIn::value_type,
    typename Y = typename std::result_of<F(X)>::type>
ContainerOut transform_parallelly(F f, const ContainerIn& xs)
{
    internal::check_arity<1, F>();
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

// API search type: reduce_parallelly : (((a, a) -> a), a, [a]) -> a
// fwd bind count: 2
// reduce_parallelly((+), 0, [1, 2, 3]) == (0+1+2+3) == 6
// Same as reduce, but can utilize multiple CPUs by using std::async.
// Combines the initial value and all elements of the sequence
// using the given function in unspecified order.
// The set of f, init and value_type should form a commutative monoid.
template <typename F, typename Container>
typename Container::value_type reduce_parallelly(
    F f, const typename Container::value_type& init, const Container& xs)
{
    if (is_empty(xs))
    {
        return init;
    }
    else if (size_of_cont(xs) == 1)
    {
        return f(init, xs.front());
    }
    else
    {
        typedef typename Container::value_type T;
        const auto f_on_pair = [f](const std::pair<T, T>& p) -> T
        {
            return f(p.first, p.second);
        };
        auto transform_result =
            transform_parallelly(f_on_pair, adjacent_pairs(xs));
        if (size_of_cont(xs) % 2 == 1)
        {
            transform_result.push_back(last(xs));
        }
        return reduce_parallelly(f, init, transform_result);;
    }
}

// API search type: reduce_1_parallelly : (((a, a) -> a), [a]) -> a
// fwd bind count: 1
// reduce_1_parallelly((+), [1, 2, 3]) == (1+2+3) == 6
// Same as reduce_1, but can utilize multiple CPUs by using std::async.
// Joins all elements of the sequence using the given function
// in unspecified order.
// The set of f and value_type should form a commutative semigroup.
template <typename F, typename Container>
typename Container::value_type reduce_1_parallelly(F f, const Container& xs)
{
    assert(is_not_empty(xs));
    if (size_of_cont(xs) == 1)
    {
        return xs.front();
    }
    else
    {
        typedef typename Container::value_type T;
        const auto f_on_pair = [f](const std::pair<T, T>& p) -> T
        {
            return f(p.first, p.second);
        };
        auto transform_result =
            transform_parallelly(f_on_pair, adjacent_pairs(xs));
        if (size_of_cont(xs) % 2 == 1)
        {
            transform_result.push_back(last(xs));
        }
        return reduce_1_parallelly(f, transform_result);
    }
}

// API search type: keep_if_parellelly : ((a -> Bool), [a]) -> [a]
// fwd bind count: 1
// keep_if_parellelly(is_even, [1, 2, 3, 2, 4, 5]) == [2, 2, 4]
template <typename Pred, typename Container>
Container keep_if_parellelly(Pred pred, const Container& xs)
{
    const auto idxs = find_all_idxs_by(
        identity<bool>, transform_parallelly(pred, xs));
    return elems_at_idxs(idxs, xs);
}

// API search type: transform_reduce : ((a -> b), ((b, b) -> b), b, [b]) -> b
// fwd bind count: 3
// transform_reduce(square, add, 0, [1,2,3]) == 0+1+4+9 = 14
// The set of binary_f, init and unary_f::output should form a
// commutative monoid.
template <typename UnaryF, typename BinaryF, typename Container,
    typename Acc = typename std::result_of<
        UnaryF(typename Container::value_type)>::type,
    typename Out = typename std::result_of<
        BinaryF(Acc, Acc)>::type>
Out transform_reduce(
    UnaryF unary_f, BinaryF binary_f, const Acc& init, const Container& xs)
{
    return reduce(binary_f, init,
        transform(unary_f, xs));
}

// API search type: transform_reduce_1 : ((a -> b), ((b, b) -> b), [b]) -> b
// fwd bind count: 2
// transform_reduce_1(square, add, [1,2,3]) == 0+1+4+9 = 14
// The set of binary_f, and unary_f::output should form
// a commutative semigroup.
template <typename UnaryF, typename BinaryF, typename Container,
    typename Acc = typename std::result_of<
        UnaryF(typename Container::value_type)>::type,
    typename Out = typename std::result_of<
        BinaryF(Acc, Acc)>::type>
Out transform_reduce_1(
    UnaryF unary_f, BinaryF binary_f, const Container& xs)
{
    return reduce_1(binary_f,
        transform(unary_f, xs));
}

// API search type: transform_reduce_parallelly : ((a -> b), ((b, b) -> b), b, [b]) -> b
// fwd bind count: 3
// transform_reduce_parallelly(square, add, 0, [1,2,3]) == 0+1+4+9 = 14
// Also Known as map_reduce.
// The set of binary_f, init and unary_f::output
// should form a commutative monoid.
template <typename UnaryF, typename BinaryF, typename Container,
    typename Acc = typename std::result_of<
        UnaryF(typename Container::value_type)>::type,
    typename Out = typename std::result_of<
        BinaryF(Acc, Acc)>::type>
Out transform_reduce_parallelly(
    UnaryF unary_f, BinaryF binary_f, const Acc& init, const Container& xs)
{
    return reduce_parallelly(binary_f, init,
        transform_parallelly(unary_f, xs));
}

// API search type: transform_reduce_1_parallelly : ((a -> b), ((b, b) -> b), [b]) -> b
// fwd bind count: 2
// transform_reduce_1_parallelly(square, add, [1,2,3]) == 0+1+4+9 = 14
// Also Known as map_reduce.
// The set of binary_f, and unary_f::output
// should form a commutative semigroup.
template <typename UnaryF, typename BinaryF, typename Container,
    typename Acc = typename std::result_of<
        UnaryF(typename Container::value_type)>::type,
    typename Out = typename std::result_of<
        BinaryF(Acc, Acc)>::type>
Out transform_reduce_1_parallelly(
    UnaryF unary_f, BinaryF binary_f, const Container& xs)
{
    return reduce_1_parallelly(binary_f,
        transform_parallelly(unary_f, xs));
}

// API search type: transform_parallelly_n_threads : (Int, (a -> b), [a]) -> [b]
// fwd bind count: 2
// transform_parallelly_n_threads(4, (*2), [1, 3, 4]) == [2, 6, 8]
// Same as transform, but uses n threads in parallel.
// Only makes sense if one run of the provided function
// takes enough time to justify the synchronization overhead.
// Can be used for applying the MapReduce pattern.
template <typename F, typename ContainerIn,
    typename ContainerOut = typename internal::same_cont_new_t_from_unary_f<
        ContainerIn, F>::type,
    typename X = typename ContainerIn::value_type,
    typename Y = typename std::result_of<F(X)>::type>
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
