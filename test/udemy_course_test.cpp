// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)


// tests for exercise solutions of the Udemy course
// "Functional Programming using C++"
// https://www.udemy.com/functional-programming-using-cpp/

#include <doctest/doctest.h>
#include <fplus/fplus.hpp>
#include <vector>

namespace Correctness_follows_from_expressiveness
{
    bool is_even(int x)
    {
        return x % 2 == 0;
    }

    template <typename Pred, typename Cont>
    Cont keep_if(Pred pred, const Cont& xs)
    {
        Cont ys;
        for (const auto x : xs)
        {
            if (pred(x))
            {
                ys.push_back(x);
            }
        }
        return ys;
    }
}

TEST_CASE("udemy_course_test - Correctness_follows_from_expressiveness")
{
    using namespace Correctness_follows_from_expressiveness;
    std::vector<int> xs = {0,1,2,3,4};
    const auto ys = keep_if(is_even, xs);
    REQUIRE_EQ(ys, std::vector<int>({0,2,4}));
}

namespace Programming_challenge_parse_and_product
{
    double str_to_double(const std::string& str)
    {
        double result;
        std::istringstream(str) >> result;
        return result;
    }
}

TEST_CASE("udemy_course_test - Programming_challenge_parse_and_product")
{
    using namespace Programming_challenge_parse_and_product;
    const std::string input = "1,5,4,7,2,2,3.34";
    const auto parts = fplus::split(',', false, input);

    const auto nums =
        fplus::transform(str_to_double, parts);

    const auto result =
        fplus::reduce(std::plus<double>(), 1, nums);

    REQUIRE(fplus::is_in_interval_around(0.001, 25.34, result));
}

namespace Programming_challenge_longest_edge_of_polygon
{
    typedef std::pair<float, float> point;

    float point_distance(const point& p1, const point& p2)
    {
        const float dx = p2.first - p1.first;
        const float dy = p2.second - p1.second;
        return std::sqrt(dx * dx + dy * dy);
    }
}

TEST_CASE("udemy_course_test - Programming_challenge_longest_edge_of_polygon")
{
    using namespace std;
    using namespace Programming_challenge_longest_edge_of_polygon;

    vector<point> polygon =
        { {1.f,2.f}, {7.f,3.f}, {6.f,5.f}, {4.f,4.f}, {2.f,9.f} };

    const auto edges =
        fplus::overlapping_pairs_cyclic(polygon);

    const auto result = fplus::maximum_on(
        [](const std::pair<point, point>& edge) -> float
        {
            return point_distance(edge.first, edge.second);
        }, edges);

    REQUIRE_EQ(fplus::show(result), std::string("((2, 9), (1, 2))"));
}

namespace The_problem_with_comments
{
    int str_to_int(const std::string& str)
    {
        int result;
        std::istringstream(str) >> result;
        return result;
    }

    template <typename Cont>
    typename Cont::value_type product(const Cont& xs)
    {
        return fplus::reduce(std::multiplies<int>(), 1, xs);
    }

    template <typename Cont>
    typename Cont::value_type sum(const Cont& xs)
    {
        return fplus::reduce(std::plus<int>(), 1, xs);
    }
}

TEST_CASE("udemy_course_test - The_problem_with_comments")
{
    using namespace The_problem_with_comments;
    const std::string input = "1,5,4,7,2,2,3";
    const auto parts = fplus::split(',', false, input);
    const auto nums = fplus::transform(str_to_int, parts);
    const auto result = product(nums); // sum(nums)
    REQUIRE_EQ(result, 1680);
}

namespace High_level_expressiveness_and_concise_code
{
    typedef std::pair<float, float> point;
    typedef std::pair<point, point> edge;
    typedef std::vector<point> points;

    float point_distance(const point& p1, const point& p2)
    {
        const float dx = p2.first - p1.first;
        const float dy = p2.second - p1.second;
        return std::sqrt(dx * dx + dy * dy);
    }

    float edge_length(const edge& e)
    {
        return point_distance(e.first, e.second);
    }

    std::vector<edge> get_edges(const points& polygon)
    {
        return fplus::overlapping_pairs_cyclic(polygon);
    }
}

TEST_CASE("udemy_course_test - High_level_expressiveness_and_concise_code")
{
    using namespace std;
    using namespace High_level_expressiveness_and_concise_code;

    vector<point> polygon =
        { {1.f,2.f}, {7.f,3.f}, {6.f,5.f}, {4.f,4.f}, {2.f,9.f} };

    const auto result = fplus::maximum_on(
        edge_length,
        get_edges(polygon));

    REQUIRE_EQ(fplus::show(result), std::string("((2, 9), (1, 2))"));
}

namespace Currying_and_partial_function_application
{
}

TEST_CASE("udemy_course_test - Currying_and_partial_function_application")
{
    using namespace Currying_and_partial_function_application;

    std::vector<std::vector<int>> xss =
        {{0,1,2}, {3,4,5}};

    // 1
    fplus::transform(fplus::fwd::transform(fplus::square<int>), xss);

    // 2
    const auto add_four_curried = [](int a)
    {
        return [a](int b)
        {
            return [a, b](int c)
            {
                return [a, b, c](int d)
                {
                    return a + b + c + d;
                };
            };
        };
    };

    REQUIRE_EQ(add_four_curried(1)(2)(3)(4), 1+2+3+4);
}

namespace Forward_application
{
    typedef std::pair<float, float> point;
    typedef std::pair<point, point> edge;
    typedef std::vector<point> points;

    float point_distance(const point& p1, const point& p2)
    {
        const float dx = p2.first - p1.first;
        const float dy = p2.second - p1.second;
        return std::sqrt(dx * dx + dy * dy);
    }

    float edge_length(const edge& e)
    {
        return point_distance(e.first, e.second);
    }

    std::vector<edge> get_edges(const points& polygon)
    {
        return fplus::overlapping_pairs_cyclic(polygon);
    }
}

TEST_CASE("udemy_course_test - Forward_application")
{
    using namespace std;
    using namespace Forward_application;

    vector<point> polygon =
        { {1.f,2.f}, {7.f,3.f}, {6.f,5.f}, {4.f,4.f}, {2.f,9.f} };

    // 1:
    const auto result = fplus::fwd::apply(polygon
        , get_edges
        , fplus::fwd::maximum_on(edge_length));

    REQUIRE_EQ(fplus::show(result), std::string("((2, 9), (1, 2))"));

    // 2:
    int a = 3;

    // intermediate values
    int b = fplus::square(a);
    int c = fplus::min_2(2, b);
    int d = fplus::abs_diff(7, c);
    int e = fplus::clamp(1, 4, d);
    int f = fplus::max_2(6, e);
    REQUIRE_EQ(f, 6);

    // nested function calls
    int f_nested = fplus::max_2(6,
        fplus::clamp(1, 4,
            fplus::abs_diff(7,
                fplus::min_2(2,
                    fplus::square(a)))));
    REQUIRE_EQ(f_nested, 6);

    // foward-application style
    int f_fwd = fplus::fwd::apply(a
        , fplus::fwd::square()
        , fplus::fwd::min_2(2)
        , fplus::fwd::abs_diff(7)
        , fplus::fwd::clamp(1, 4)
        , fplus::fwd::max_2(6));
    REQUIRE_EQ(f_fwd, 6);
}

namespace Programming_challenge_Interacting_with_the_command_line
{
    // cmd_line_interact : (String -> String) -> ()
    template <typename F>
    void cmd_line_interact(F)
    {
        // no side effects in unit tests
    }
}

TEST_CASE("udemy_course_test - Programming_challenge_Interacting_with_the_command_line")
{
    using namespace fplus;
    using namespace Programming_challenge_Interacting_with_the_command_line;

    // 1:
    cmd_line_interact(fwd::to_upper_case());

    // 2:
    cmd_line_interact(
        fwd::compose(
            fwd::split_lines(false),
            fwd::sort(),
            fwd::join(std::string("\n"))));
}

namespace Function_composition
{
    double str_to_double(const std::string& str)
    {
        double result;
        std::istringstream(str) >> result;
        return result;
    }

    const auto parse_and_product = fplus::fwd::compose(
        fplus::fwd::split(',', false),
        fplus::fwd::transform(str_to_double),
        fplus::fwd::product());
}

TEST_CASE("udemy_course_test - Function_composition")
{
    using namespace Function_composition;
    const std::string input = "1,5,4,7,2,2,3.34";
    const auto result = parse_and_product(input);
    REQUIRE(fplus::is_in_interval_around(0.001, 1870.4, result));
}

namespace Programming_challenge_an_SQL_analogy
{
    struct user
    {
        std::string name;
        std::string country;
        std::size_t visits;
    };

    std::string get_country(const user& u)
    {
        return u.country;
    }

    std::size_t get_visits(const user& u)
    {
        return u.visits;
    }
}

TEST_CASE("udemy_course_test - Programming_challenge_an_SQL_analogy")
{
    using namespace Programming_challenge_an_SQL_analogy;

    const std::vector<user> users = {
        {"Nicole", "GER", 2},
        {"Justin", "USA", 1},
        {"Rachel", "USA", 5},
        {"Robert", "USA", 6},
        {"Stefan", "GER", 4}
    };


    const auto visit_sum = [](const std::vector<user>& xs) -> std::size_t
    {
        return fplus::fwd::apply(xs
            , fplus::fwd::transform(get_visits)
            , fplus::fwd::sum());
    };

    // n^2
    const auto result = fplus::fwd::apply(users
        , fplus::fwd::group_globally_on_labeled(get_country)
        , fplus::fwd::transform(fplus::fwd::transform_snd(visit_sum))
        );
    REQUIRE_EQ(fplus::show_cont(result),
        std::string("[(GER, 6), (USA, 12)]"));

    // n * log(n)
    const auto result_n_log_n = fplus::fwd::apply(users
        , fplus::fwd::sort_on(get_country)
        , fplus::fwd::group_on_labeled(get_country)
        , fplus::fwd::transform(fplus::fwd::transform_snd(visit_sum))
        );
    REQUIRE_EQ(fplus::show_cont(result_n_log_n),
        std::string("[(GER, 6), (USA, 12)]"));
}

namespace Functors
{
    template <typename ValOut, typename F, typename Key, typename ValIn>
    std::map<Key, ValOut> lift_dict(F f, const std::map<Key, ValIn>& dict)
    {
        std::map<Key, ValOut> result;
        for (const auto& key_and_val : dict)
        {
            result[key_and_val.first] = f(key_and_val.second);
        }
        return result;
    }
}

TEST_CASE("udemy_course_test - Functors")
{
    using namespace Functors;
    using namespace fplus;
    std::map<int, double> dict =
        {{2, 1.41}, {3, 1.73}, {4, 2.0}};
    auto dict_squared = lift_dict<double>(square<double>, dict);
    auto dict_shown = lift_dict<std::string>(show<double>, dict);

    REQUIRE_EQ(show_cont(dict_squared), "[(2, 1.9881), (3, 2.9929), (4, 4)]");
    REQUIRE_EQ(show_cont(dict_shown), "[(2, 1.41), (3, 1.73), (4, 2)]");
}

namespace Monads
{
    using namespace std;
    using namespace fplus;
    typedef vector<int> Ints;
    typedef vector<string> Strings;

    result<string, string> get_input_filepath(const Strings& args)
    {
        assert(args.size() > 0);
        if (args.size() != 2)
            return error<string, string>(
                "Usage: " + args[0] + " FILEPATH");
        else
            return ok<string, string>(args[1]);
    }

    result<string, string> read_file(const string&)
    {
        // dummy, no side effects in tests
        return ok<string, string>("1,1,1,4");
    }

    result<Ints, string> parse_content(const string& content)
    {
        const auto maybe_values = fwd::apply(content
            , fwd::split(',', false)
            , fwd::transform(read_value<int>));
        if (all_by(is_just<int>, maybe_values))
            return ok<Ints, string>(justs(maybe_values));
        else
            return error<Ints, string>("Can not parse file.");
    }

    result<int, string> calc_median(const vector<int>& xs)
    {
        if (is_not_empty(xs))
            return ok<int, string>(median(xs));
        return error<int, string>("Need at least one value.");
    }

    string show_median(int value)
    {
        return "The median is " + show(value);
    }

    string show_error(const string& error)
    {
        return "ERROR: " + error;
    }

    template <typename Res, typename Error,
        typename F, typename A>
    result<Res, Error> my_and_then_result(
        F f, const result<A, Error>& r)
    {
        if (is_ok(r))
            return f(unsafe_get_ok(r));
        else
            return error<Res, Error>(r.unsafe_get_error());
    }
}

TEST_CASE("udemy_course_test - Monads")
{
    using namespace fplus;
    using namespace std;
    using namespace Monads;
    const Strings arguments = {"executable", "input.txt"};

    const string error_msg = "An error occured.";

    const auto input_filepath = get_input_filepath(arguments);

    const auto file_content =
        my_and_then_result<string>(read_file, input_filepath);

    const auto values =
        my_and_then_result<Ints>(parse_content, file_content);

    const auto res =
        my_and_then_result<int>(calc_median, values);

    const auto output = unify_result(show_median, show_error, res);

    REQUIRE_EQ(output, std::string("The median is 1"));
}

namespace Multithreading
{
    struct Image {}; // dummy
    struct FaceImage {}; // dummy
    std::vector<Image> images;
    FaceImage extract_face(Image) { return {}; } // dummy
    FaceImage empty_face_image; // dummy
    FaceImage add_face_images(FaceImage, FaceImage) { return {}; } // dummy
    FaceImage divide_values(FaceImage, std::size_t) { return {}; } // dummy
}

TEST_CASE("udemy_course_test - Multithreading")
{
    using namespace Multithreading;
    fplus::transform_reduce_parallelly(
        extract_face, add_face_images, empty_face_image, images);
}

namespace OOP_Design_patterns_vanishing
{
    int square(int x)
    {
        return x * x;
    }

    // decorate_with_logging : (String, (Int -> Int)) -> (Int -> Int)
    template <typename F>
    std::function<int(int)> decorate_with_logging(const std::string& str, F f)
    {
        return [str, f](int x) -> int
        {
            int result = f(x);
            // no side effects in tests
            //std::cout << str << ": " << x << " => " << result << std::endl;
            return result;
        };
    }
}

TEST_CASE("udemy_course_test - OOP_Design_patterns_vanishing")
{
    using namespace OOP_Design_patterns_vanishing;
    const auto logging_square =
        decorate_with_logging("Square", square);

    int a = logging_square(4);
    int b = logging_square(5);

    REQUIRE_EQ(a, 16);
    REQUIRE_EQ(b, 25);
}