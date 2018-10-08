// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <vector>
#include <fplus/fplus.hpp>

namespace fplus
{
    using FunctionName = std::string;
    struct benchmark_function_report
    {
        std::string function_name;
        size_t nb_calls;
        ExecutionTime total_time;
        ExecutionTime average_time;
        ExecutionTime deviation;
    };

    namespace internal
    {
        std::string show_benchmark_function_report(const std::vector<benchmark_function_report> & reports);
    }

    // benchmark_session stores timings during a benchmark session
    // and is able to emit a report at the end
    class benchmark_session
    {
    public:
        // report() shall return a string with a summary of the session
        // Example below:
        // Function              |Nb calls|Total time|Av. time|Deviation|
        // ----------------------+--------+----------+--------+---------+
        // convert_charset_string|    4000|   4.942ms| 1.236ns|  1.390ns|
        // split_lines           |    1000|   4.528ms| 4.528ns|  1.896ns|
        inline std::string report() const {
            const auto reports = report_list();
            return fplus::internal::show_benchmark_function_report(reports);
        }

        std::vector<benchmark_function_report> report_list() const
        {
            std::vector<benchmark_function_report> report;
            for (const auto & one_function_time : functions_times_)
                report.push_back(make_bench_report(one_function_time.first, one_function_time.second));
            // sort by total_time descending
            auto report_sorted = fplus::sort_by([](const benchmark_function_report &a, const benchmark_function_report &b) {
                return a.total_time > b.total_time;
            }, report);
            return report_sorted;
        }

        inline void store_one_time(const FunctionName & function_name, ExecutionTime time) {
            functions_times_[function_name].push_back(time);
        }

    private:
        benchmark_function_report make_bench_report(
            const FunctionName & function_name, const std::vector<ExecutionTime> & times) const
        {
            benchmark_function_report result;
            result.function_name = function_name;            
            result.nb_calls = times.size();
            auto mean_and_dev = fplus::mean_stddev<double>(times);
            result.average_time = mean_and_dev.first;
            result.deviation = mean_and_dev.second;
            result.total_time = fplus::sum(times);
            return result;
        }

        std::map<FunctionName, std::vector<ExecutionTime>> functions_times_;
    };


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
        fplus::maybe<int> v;
    };


    template<class Fn>
    auto make_benchmark_function(benchmark_session & session, const FunctionName & name, Fn f)
    {
        // transforms f into a function with the same 
        // signature, that will store timings into the benchmark session
        return bench_function_impl<Fn>(session, name, f);
    }


    template<class Fn>
    auto run_n_times(int nb_runs, Fn f)
    {
        for (auto _ : fplus::numbers(0, nb_runs))
            f();
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

        std::string show_benchmark_function_report(const std::vector<benchmark_function_report> & reports)
        {
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
            for (const auto & report : reports)
            {
                std::vector<std::string> row;
                row.push_back(report.function_name);
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
