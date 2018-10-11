// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <vector>
#include <mutex>
#include <fplus/fplus.hpp>

namespace fplus
{
using FunctionName = std::string;
struct benchmark_function_report
{
    size_t nb_calls;
    ExecutionTime total_time;
    ExecutionTime average_time;
    ExecutionTime deviation;
};


namespace internal
{
    std::string show_benchmark_function_report(const std::map<FunctionName, benchmark_function_report> & reports);
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
            report[one_function_time.first] = make_bench_report(one_function_time.second);
        return report;
    }

    inline void store_one_time(const FunctionName & function_name, ExecutionTime time) 
    {
        std::lock_guard<std::mutex> lock(functions_times_mutex_);
        functions_times_[function_name].push_back(time);
    }

private:
    benchmark_function_report make_bench_report(const std::vector<ExecutionTime> & times) const
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

        template<typename ...Args> auto operator()(Args... args) { return _bench_result(args...); }

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

        template<typename ...Args> auto operator()(Args... args) { _bench_result(args...); }

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
// fwd bind count: 0
// Transforms a function into a function with the *same* signature
// and behavior, except that it also stores stats into the benchmark session (first parameter),
// under the name given by the second parameter.
// (use make_benchmark_void_function if your function returns void).
//
// Note that make_benchmark_function *will add side effects* to the function
// (since it stores data into the benchmark session at each call).
//
// If you intend to benchmark only one function, prefer to use the simpler `make_timed_function`
//
// Example of a minimal benchmark session:
//
// fplus::benchmark_session benchmark_sess;
// void foo() {
//     auto add_bench = fplus::make_benchmark_function(benchmark_sess, "add", add);
//     auto printf_bench = fplus::make_benchmark_void_function(benchmark_sess, "printf", printf);
//     int forty_five = add_bench(20, add_bench(19, 6));
//     int forty_two = benchmark_expression(benchmark_sess, "sub", forty_five - 3);
//     printf_bench("forty_two is %i\n", forty_two);
// }
// void main() {
//     foo();
//     std::cout << benchmark_sess.report();
// }
//
// This will output a report like this:
// Function|Nb calls|Total time|Av. time|Deviation|
// --------+--------+----------+--------+---------+
// printf  |       1|   0.010ms| 9.952ns|  0.000ns|
// add     |       2|   0.000ms| 0.050ns|  0.009ns|
// sub     |       1|   0.000ms| 0.039ns|  0.000ns|
//
// (Read benchmark_session_test.cpp for a full example)
//
// ----------------------------------------------------------
//
// As an alternative to make_benchmark_function, you can also benchmark an expression.
// For example, if you want to benchmark the following line:
//
//     auto sorted = fplus::sort(my_vector);
//
// In order to do so, ye just copy/paste this expression into "bench_expression" like shown below.
// This expression will then be benchmarked with the name "sort_my_vector"
//
// auto sorted = benchmark_expression(
//     my_benchmark_session,
//     "sort_my_vector",
//     fplus::sort(my_vector);
// );
//
// Notes :
//  - benchmark_expression is a preprocessor macro that uses an immediately invoked lambda (IIL)
// - the expression can be copy-pasted with no modification, and it is possible to not remove the ";"
//   (although it also works if it is not present)
//
template<class Fn>
auto make_benchmark_function(benchmark_session & session, const FunctionName & name, Fn f)
{
    // transforms f into a function with the same 
    // signature, that will store timings into the benchmark session
    return internal::bench_function_impl<Fn>(session, name, f);
}


// API search type: make_benchmark_void_function : (benchmark_session, string, (a... -> Void)) -> (a... -> Void)
// fwd bind count: 0
// Transforms a function that returns a void into a function with the *same* signature
// and behavior, except that it also stores stats into the benchmark session (first parameter),
// under the name given by the second parameter
//
// Note that make_benchmark_void_function *will add side effects* to the function
// (since it stores data into the benchmark session at each call)
//
// Example:
//
// benchmark_session bench_session;
// ...
// void foo() { 
//     std::this_thread::sleep_for(std::chrono::milliseconds(1000));
// }
// ...
// auto foo_bench = make_benchmark_void_function(bench_session, "foo", foo);
// foo_bench();
// ...
// std::cout << benchmark_session.report();
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


template<class Fn>
auto run_n_times(int nb_runs, Fn f)
{
    for (auto _ : fplus::numbers(0, nb_runs)) {
        (void) _; // suppress warning / unused variable
        f();
    }
}


namespace internal
{
    inline std::string show_tableau(std::vector<std::vector<std::string>> & rows, bool add_firstrow_separator)
    {
        if (rows.empty())
            return "";
        if (rows[0].empty())
            return "";
        auto string_size = [](const std::string & s) -> size_t { return s.size(); };
        auto get_item = [&](size_t row, size_t col) {
            return rows[row][col];
        };
        auto largest_string_size = [&](const std::vector<std::string> & strings) -> size_t {
            return string_size(fplus::maximum_on(string_size, strings));
        };

        size_t nb_rows = rows.size();
        size_t nb_cols = rows[0].size();

        std::vector<size_t> columns_width;
        {
            auto columns = fplus::transpose(rows);
            columns_width = fplus::transform(largest_string_size, columns);
        }

        auto show_one_row = [&](size_t row) {
            std::string result;
            for (size_t col = 0; col < nb_cols; col++)
            {
                const std::string & element = get_item(row, col);
                bool is_number = element.size() > 0 && isdigit(element[0]);
                auto col_width = columns_width[col];
                if (is_number)
                    result = result + fplus::show_fill_left(' ', col_width, element) + "|";
                else
                    result = result + fplus::show_fill_right(' ', col_width, element) + "|";
            }
            return result;
        };

        auto show_firstrow_separator = [&]() {
            std::string result;
            for (size_t col = 0; col < nb_cols; col++)
                result = result + fplus::show_fill_left('-', columns_width[col], "") + "+";
            return result;
        };

        std::string result = show_one_row(0) + "\n";
        if (add_firstrow_separator)
            result += show_firstrow_separator() + "\n";
        for (size_t row = 1; row < nb_rows; row++)
            result += show_one_row(row) + "\n";

        return result;
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
        std::vector<std::vector<std::string>> tableau_rows;
        tableau_rows.push_back(header_row);
        for (const auto & kv : ordered_reports)
        {
            const auto & report = kv.second;
            const auto & function_name = kv.first;
            std::vector<std::string> row;
            row.push_back(function_name);
            row.push_back(fplus::show(report.nb_calls));
            row.push_back(my_show_time_ms(report.total_time));
            row.push_back(my_show_time_ns(report.average_time));
            row.push_back(my_show_time_ns(report.deviation));
            tableau_rows.push_back(row);
        }

        return fplus::internal::show_tableau(tableau_rows, true);
    }
} // namespace internal 

}
