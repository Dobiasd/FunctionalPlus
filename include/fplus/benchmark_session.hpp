// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <vector>
#include <mutex>
#include <fplus/fplus.hpp>

#pragma once

namespace fplus
{
using FunctionName = std::string;
struct benchmark_function_report
{
    std::size_t nb_calls;
    ExecutionTime total_time;
    ExecutionTime average_time;
    ExecutionTime deviation;
};


namespace internal
{
    std::string show_benchmark_function_report(
        const std::map<FunctionName, benchmark_function_report> & reports);
}


// benchmark_session stores timings during a benchmark session
// and is able to emit a report at the end
class benchmark_session
{
public:
    benchmark_session() : functions_times_mutex_(), functions_times_() {};

    // report() shall return a string with a summary of the session
    // Example below:
    // Function              |Nb calls|Total time|Av. time|Deviation|
    // ----------------------+--------+----------+--------+---------+
    // convert_charset_string|    4000|   4.942ms| 1.236ns|  1.390ns|
    // split_lines           |    1000|   4.528ms| 4.528ns|  1.896ns|
    inline std::string report() const
    {
        const auto reports = report_list();
        return fplus::internal::show_benchmark_function_report(reports);
    }

    std::map<FunctionName, benchmark_function_report> report_list() const
    {
        std::lock_guard<std::mutex> lock(functions_times_mutex_);
        std::map<FunctionName, benchmark_function_report> report;
        for (const auto & one_function_time : functions_times_)
        {
            report[one_function_time.first] = make_bench_report(one_function_time.second);
        }
        return report;
    }

    inline void store_one_time(const FunctionName & function_name, ExecutionTime time)
    {
        std::lock_guard<std::mutex> lock(functions_times_mutex_);
        functions_times_[function_name].push_back(time);
    }

private:
    benchmark_function_report make_bench_report(
        const std::vector<ExecutionTime> & times) const
    {
        benchmark_function_report result;
        result.nb_calls = times.size();
        auto mean_and_dev = fplus::mean_stddev<double>(times);
        result.average_time = mean_and_dev.first;
        result.deviation = mean_and_dev.second;
        result.total_time = fplus::sum(times);
        return result;
    }

    mutable std::mutex functions_times_mutex_;
    std::map<FunctionName, std::vector<ExecutionTime>> functions_times_;
};

namespace internal
{
    template<typename Fn>
    class bench_function_impl
    {
    public:
        explicit bench_function_impl(
            benchmark_session & benchmark_sess,
            FunctionName function_name,
            Fn fn)
            : benchmark_session_(benchmark_sess)
            , function_name_(function_name)
            , fn_(fn)
        {};

        template<typename ...Args> auto operator()(Args... args)
        {
            return _bench_result(args...);
        }

    private:
        template<typename ...Args>
        auto _bench_result(Args... args)
        {
            fplus::stopwatch timer;
            auto r = fn_(args...);
            benchmark_session_.store_one_time(function_name_, timer.elapsed());
            return r;
        }

        benchmark_session & benchmark_session_;
        FunctionName function_name_;
        Fn fn_;
    };

    template<typename Fn>
    class bench_void_function_impl
    {
    public:
        explicit bench_void_function_impl(
            benchmark_session & benchmark_sess,
            FunctionName function_name,
            Fn fn)
            : benchmark_session_(benchmark_sess)
            , function_name_(function_name)
            , fn_(fn)
        {};

        template<typename ...Args> auto operator()(Args... args)
        {
            _bench_result(args...);
        }

    private:
        template<typename ...Args>
        auto _bench_result(Args... args)
        {
            fplus::stopwatch timer;
            fn_(args...);
            benchmark_session_.store_one_time(function_name_, timer.elapsed());
        }

        benchmark_session & benchmark_session_;
        FunctionName function_name_;
        Fn fn_;
    };

} // namespace internal


// API search type: make_benchmark_function : (benchmark_session, string, (a... -> b)) -> (a... -> b)
// Transforms a function into a function with the *same* signature
// and behavior, except that it also stores stats into the benchmark session (first parameter),
// under the name given by the second parameter.
// -
// Notes:
// Side effects: make_benchmark_function *will add side effects* to the function, since it stores data
// into the benchmark session at each call.
// If you intend to benchmark only one function, prefer to use the simpler "make_timed_function"
// Use "make_benchmark_void_function" if your function returns void
// -
// Example of a minimal benchmark session (read benchmark_session_test.cpp for a full example)
//     fplus::benchmark_session benchmark_sess;
//     void foo() {
//         auto add_bench = fplus::make_benchmark_function(benchmark_sess, "add", add);
//         auto printf_bench = fplus::make_benchmark_void_function(benchmark_sess, "printf", printf);
//         int forty_five = add_bench(20, add_bench(19, 6));
//         int forty_two = benchmark_expression(benchmark_sess, "sub", forty_five - 3);
//         printf_bench("forty_two is %i\n", forty_two);
//     }
//     int main() {
//         foo();
//         std::cout << benchmark_sess.report();
//     }
// This will output a report like this
// Function|Nb calls|Total time|Av. time|Deviation|
// --------+--------+----------+--------+---------+
// printf  |       1|   0.010ms| 9.952ns|  0.000ns|
// add     |       2|   0.000ms| 0.050ns|  0.009ns|
// sub     |       1|   0.000ms| 0.039ns|  0.000ns|
// -
// As an alternative to make_benchmark_function, you can also benchmark an expression.
// For example, in order to benchmark the following line:
//     auto sorted = fplus::sort(my_vector);
// Just copy/paste this expression into "bench_expression" like shown below: this expression
// will then be benchmarked with the name "sort_my_vector"
//     auto sorted = benchmark_expression(
//         my_benchmark_session,
//         "sort_my_vector",
//         fplus::sort(my_vector);
//     );
// Notes :
// benchmark_expression is a preprocessor macro that uses an immediately invoked lambda (IIL).
// The expression can be copy-pasted with no modification, and it is possible to not remove the ";"
// (although it also works if it is not present)
// You can also benchmark an expression that returns void using benchmark_void_expression
template<class Fn>
auto make_benchmark_function(benchmark_session & session, const FunctionName & name, Fn f)
{
    // transforms f into a function with the same
    // signature, that will store timings into the benchmark session
    return internal::bench_function_impl<Fn>(session, name, f);
}


// API search type: make_benchmark_void_function : (benchmark_session, string, (a... -> Void)) -> (a... -> Void)
// Transforms a function that returns a void into a function with the *same* signature
// and behavior, except that it also stores stats into the benchmark session (first parameter),
// under the name given by the second parameter
// Note that make_benchmark_void_function *will add side effects* to the function
// (since it stores data into the benchmark session at each call)
// -
// Example:
//     benchmark_session bench_session;
//     ...
//     void foo() {
//         std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//     }
//     ...
//     auto foo_bench = make_benchmark_void_function(bench_session, "foo", foo);
//     foo_bench();
//     ...
//     std::cout << benchmark_session.report();
template<class Fn>
auto make_benchmark_void_function(benchmark_session & session, const FunctionName & name, Fn f)
{
    // transforms a void returning function into a function with the same
    // signature, that will store timings into the benchmark session
    return internal::bench_void_function_impl<Fn>(session, name, f);
}

#define benchmark_expression(bench_session, name, expression)      \
make_benchmark_function(                                           \
    bench_session,                                                 \
    name,                                                          \
    [&]() { return expression; }                                   \
)();

#define benchmark_void_expression(bench_session, name, expression) \
make_benchmark_void_function(                                      \
    bench_session,                                                 \
    name,                                                          \
    [&]() { expression; }                                          \
)();


namespace internal
{
    inline std::string show_table(const std::vector<std::vector<std::string>>& rows)
    {
        if (rows.empty() || rows[0].empty())
            return "";

        const std::vector<std::size_t> columns_width = [&]() {
            auto string_size = [](const std::string & s) -> std::size_t { return s.size(); };
            auto largest_string_size = [&](const std::vector<std::string> & strings) -> std::size_t {
                return string_size(fplus::maximum_on(string_size, strings));
            };
            return fplus::transform(largest_string_size, fplus::transpose(rows));
        }();

        auto show_one_element = [](const std::pair<std::string, std::size_t> & elem_and_width) {
            const std::string & element = elem_and_width.first;
            const auto col_width = elem_and_width.second;
            bool is_number = element.size() > 0 && isdigit(element[0]);
            if (is_number)
                return fplus::show_fill_left(' ', col_width, element) + "|";
            else
                return fplus::show_fill_right(' ', col_width, element) + "|";
        };

        auto show_one_separator = [](std::size_t col_width) {
            return fplus::show_fill_left('-', col_width, "") + "+";
        };

        auto show_one_row = [&](const std::vector<std::string> & row) {
            return fplus::sum(fplus::transform(
                show_one_element,
                fplus::zip(row, columns_width)));
        };

        auto firstrow_separator = fplus::sum(fplus::transform(show_one_separator, columns_width));
        auto rows_formatted = fplus::transform(show_one_row, rows);
        auto rows_separated = fplus::insert_at_idx(1, firstrow_separator, rows_formatted);
        return fplus::join( std::string("\n"), rows_separated) + "\n";
    }

    inline std::vector< std::pair<FunctionName, benchmark_function_report> > make_ordered_reports(
        const std::map<FunctionName, benchmark_function_report> & report_map)
    {
        auto report_pairs = fplus::map_to_pairs(report_map);
        auto report_pairs_sorted = fplus::sort_by([](const auto &a, const auto &b) {
            return a.second.total_time > b.second.total_time;
        }, report_pairs);
        return report_pairs_sorted;
    }

    inline std::string show_benchmark_function_report(const std::map<FunctionName, benchmark_function_report> & reports)
    {
        auto ordered_reports = make_ordered_reports(reports);
        auto my_show_time_ms = [](double time) -> std::string {
            std::stringstream ss;
            ss << std::fixed << std::setprecision(3);
            ss << (time * 1000.);
            return ss.str() + "ms";
        };
        auto my_show_time_ns = [](double time) -> std::string {
            std::stringstream ss;
            ss << std::fixed << std::setprecision(3);
            ss << (time * 1000000.);
            return ss.str() + "ns";
        };

        std::vector<std::string> header_row{ {
                "Function", "Nb calls", "Total time", "Av. time", "Deviation"
            } };
        auto value_rows = fplus::transform([&](const auto & kv) {
                const auto & report = kv.second;
                const auto & function_name = kv.first;
                std::vector<std::string> row;
                row.push_back(function_name);
                row.push_back(fplus::show(report.nb_calls));
                row.push_back(my_show_time_ms(report.total_time));
                row.push_back(my_show_time_ns(report.average_time));
                row.push_back(my_show_time_ns(report.deviation));
                return row;
            },
            ordered_reports);

        return fplus::internal::show_table(fplus::insert_at_idx(0, header_row, value_rows));
    }
} // namespace internal

}
