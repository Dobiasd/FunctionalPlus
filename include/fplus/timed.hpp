#pragma once

#include <chrono>
#include <type_traits>
#include <fplus/function_traits.hpp>
#include <fplus/container_common.hpp>
#include <fplus/internal/composition.hpp>
#include <fplus/internal/asserts/composition.hpp>

#include <cassert>
#include <exception>
#include <functional>
#include <memory>

namespace fplus
{
using ExecutionTime = double; // in seconds

// Holds a value of type T plus an execution time
template <typename T>
class timed : public std::pair<T, ExecutionTime>
{
    using base_pair = std::pair<T, ExecutionTime>;
public:
    timed() : base_pair() {}
    timed(const T& val, ExecutionTime t = 0.) : base_pair(val, t) {}

    // Execution time in seconds (returns a double)
    ExecutionTime time_in_s() const { return base_pair::second; }

    // Execution time as a std::chrono::duration<double>
    std::chrono::duration<double, std::ratio<1>> duration_in_s() const
    {
        return std::chrono::duration<double, std::ratio<1>>(time_in_s());
    }

    // Inner value
    const T& get() const            { return base_pair::first; }
    T& get()                        { return base_pair::first; }
};

// API search type: show_timed : Timed a -> String
// fwd bind count: 0
// show_timed((42,1)) -> "42 (1000ms)"
template <typename T>
std::string show_timed(const fplus::timed<T>& v)
{
    std::string result =
        fplus::show(v.get()) + " (" + fplus::show(v.time_in_s() * 1000.) + "ms)";
    return result;
}

namespace internal
{
    template<typename Fn>
    class timed_function_impl
    {
    public:
        explicit timed_function_impl(Fn fn) : _fn(fn) {};
        template<typename ...Args> auto operator()(Args... args) { return _timed_result(args...); }

    private:
        template<typename ...Args>
        auto _timed_result(Args... args)
        {
            fplus::stopwatch timer;
            auto r = _fn(args...);
            auto r_t = fplus::timed<decltype(r)>(r, timer.elapsed());
            return r_t;
        }

        Fn _fn;
    };
}

// API search type: make_timed_function : ((a -> b)) -> (a -> Timed b)
// fwd bind count: 0
// Transforms a function into a timed / benchmarked version of the same function.
//
// Example:
//
// using Ints = std::vector<int>;
// Ints ascending_numbers = fplus::numbers(0, 1000);
// Ints shuffled_numbers = fplus::shuffle(std::mt19937::default_seed, ascending_numbers);
// auto sort_func = [](const Ints& values) { return fplus::sort(values); };
// auto sort_bench = fplus::make_timed_function(sort_func);
// auto sorted_numbers = sort_bench(shuffled_numbers);
// // sorted_numbers.get() will return the actual computation output
// assert(sorted_numbers.get() == ascending_numbers);
// // sorted_numbers.time() will return the execution time in seconds
// assert(sorted_numbers.time() < 0.1);
template<class Fn>
auto make_timed_function(Fn f)
{
    return internal::timed_function_impl<decltype(f)>(f);
}

namespace internal
{
    template<typename Fn>
    class timed_void_function_impl
    {
    public:
        explicit timed_void_function_impl(Fn fn) : _fn(fn) {};
        template<typename ...Args> auto operator()(Args... args)
        {
            return _timed_result(args...);
        }

    private:
        template<typename ...Args>
        auto _timed_result(Args... args)
        {
            fplus::stopwatch timer;
            _fn(args...);
            return timer.elapsed();
        }

        Fn _fn;
    };

}

// API search type: make_timed_void_function : ((a -> Void)) -> (a -> Double)
// fwd bind count: 0
// Transforms a void function into a timed / benchmarked version of the same function.
//
// Example:
//
// void foo() {
//     std::this_thread::sleep_for(std::chrono::milliseconds(1000));
// }
// ...
// auto foo_bench = make_timed_void_function(foo);
// auto r = foo_bench();
// double run_time = foo_bench(); // in seconds
template<class Fn>
auto make_timed_void_function(Fn f)
{
    return internal::timed_void_function_impl<decltype(f)>(f);
}

}
