// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <fplus/fplus.hpp>

#include <atomic>
#include <cassert>
#include <chrono>
#include <ctime>
#include <deque>
#include <functional>
#include <iostream>
#include <list>
#include <stdexcept>
#include <string>
#include <vector>

bool is_odd_int(int x) { return x % 2 == 1; }

void Test_example_KeepIf()
{
    typedef std::vector<int> Ints;
    Ints values = { 24, 11, 65, 44, 80, 18, 73, 90, 69, 18 };

    { // Version 1: hand written range based for loop
        Ints odds;
        for (int x : values)
            if (is_odd_int(x))
                odds.push_back(x);
    }

    { // Version 2: STL
        Ints odds;
        std::copy_if(std::begin(values), std::end(values),
                std::back_inserter(odds), is_odd_int);
    }

    { // Version : FunctionalPlus
        auto odds = fplus::keep_if(is_odd_int, values);
    }
}

void run_n_times(std::function<std::vector<int>(std::vector<int>)> f,
    std::size_t n, const std::string& name, const std::vector<int>& inList)
{
    typedef std::chrono::time_point<std::chrono::system_clock> Time;
    Time startTime = std::chrono::system_clock::now();
    std::size_t lengthSum = 0;
    for (std::size_t i = 0; i < n; ++i)
    {
        lengthSum += f(inList).size();
    }
    Time endTime = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = endTime - startTime;
    std::cout << name << "(check: " << lengthSum << "), elapsed time: " << elapsed_seconds.count() << "s\n";
}

void Test_example_KeepIf_performance()
{
    using namespace fplus;

    typedef std::vector<int> Ints;
    auto run_loop = [&](const Ints values)
    {
        Ints odds;
        for (int x : values)
            if (is_odd_int(x))
                odds.push_back(x);
        return odds;
    };
    auto run_stl = [&](const Ints values)
    {
        Ints odds;
        std::copy_if(std::begin(values), std::end(values),
                std::back_inserter(odds), is_odd_int);
        return odds;
    };
    auto run_FunctionalPlus = [&](const Ints values)
        { return keep_if(is_odd_int, values); };

    // make debug runs faster
#if defined NDEBUG || defined _DEBUG
    std::size_t numRuns = 10;
#else
    std::size_t numRuns = 20000;
#endif

    Ints values = generate<Ints>(rand, 5000);
    run_n_times(run_loop, numRuns, "Hand-written for loop", values);
    run_n_times(run_stl, numRuns, "std::copy_if", values);
    run_n_times(run_FunctionalPlus, numRuns, "FunctionalPlus::keep_if", values);
}


void Test_example_SameOldSameOld()
{
    std::list<std::string> things = {"same old", "same old"};
    if (fplus::all_the_same(things))
        std::cout << "All things being equal." << std::endl;
}

void Test_example_IInTeam()
{
    std::string team = "Our team is great. I love everybody I work with.";
    std::cout << "There actually are this many 'I's in team: " <<
        fplus::count("I", fplus::split_words(false, team)) << std::endl;
}

struct Entity
{
    Entity() : calm_(true), bright_(true) {}
    bool calm_;
    bool bright_;
};

void Test_example_AllIsCalmAndBright()
{
    auto isCalm = [](const Entity& e) { return e.calm_; };
    auto isBright = [](const Entity& e) { return e.bright_; };
    std::vector<Entity> entities(4);
    if (fplus::all_by(fplus::logical_and(isCalm, isBright), entities))
        std::cout << "Silent night." << std::endl;
}

struct cat
{
    double cuteness() const
    {
        return softness_ * temperature_ * roundness_ * fur_amount_ - size_;
    }
    std::string name_;
    double softness_;
    double temperature_;
    double size_;
    double roundness_;
    double fur_amount_;
};

void Test_example_TheCutestCat()
{
    std::vector<cat> cats = {
        {"Tigger",   5, 5, 5, 5, 5},
        {"Simba",    2, 9, 9, 2, 7},
        {"Muffin",   9, 4, 2, 8, 6},
        {"Garfield", 6, 5, 7, 9, 5}};

    auto cutest_cat = fplus::maximum_on(std::mem_fn(&cat::cuteness), cats);

    std::cout << cutest_cat.name_ <<
        " is happy and sleepy. *purr* *purr* *purr*" << std::endl;
}

std::list<int> collatz_seq(int x)
{
    std::list<int> result;
    while (x > 1)
    {
        result.push_back(x);
        if (x % 2 == 0)
            x = x / 2;
        else
            x = 3 * x + 1;
    }
    result.push_back(x);
    return result;
}

void Test_example_CollatzSequence()
{
    typedef std::list<int> Ints;

    // [1, 2, 3 ... 29]
    auto xs = fplus::numbers<int>(1, 30);

    // A function that does [1, 2, 3, 4, 5] -> "[1 => 2 => 3 => 4 => 5]"
    auto show_ints = fplus::bind_1st_of_2(fplus::show_cont_with<Ints>, " => ");

    // A composed function that calculates a Collatz sequence and shows it.
    auto show_collats_seq = fplus::compose(collatz_seq, show_ints);

    // Associate the numbers with the string representation of their sequences.
    auto collatz_dict = fplus::create_map_with(show_collats_seq, xs);

    // Print some of the sequences.
    std::cout << collatz_dict[13] << std::endl;
    std::cout << collatz_dict[17] << std::endl;
}

std::string gemstone_count(const std::string& input)
{
    using namespace fplus;

    typedef std::set<std::string::value_type> characters;

    const auto lines = split_lines(false, input);

    const auto sets = transform(
        convert_container<characters, std::string>,
        lines);

    const auto gem_elements = fold_left_1(
        set_intersection<characters>, sets);

    return show(size_of_cont(gem_elements));
}

std::string gemstone_count_fwd_apply(const std::string& input)
{
    using namespace fplus;
    typedef std::set<std::string::value_type> characters;
    return fwd::apply(
        input
        , fwd::split_lines(false)
        , fwd::transform(convert_container<characters, std::string>)
        , fwd::fold_left_1(set_intersection<characters>)
        , fwd::size_of_cont()
        , fwd::show()
    );
}

typedef std::set<std::string::value_type> characters;
const auto gemstone_count_fwd_compose = fplus::fwd::compose(
    fplus::fwd::split_lines(false),
    fplus::fwd::transform(fplus::convert_container<characters, std::string>),
    fplus::fwd::fold_left_1(fplus::set_intersection<characters>),
    fplus::fwd::size_of_cont(),
    fplus::fwd::show()
);

void Test_example_fwd_style()
{
    const std::string input =
        "Lorem ipsum\ndolor sit amet,\nconsectetur,\nadipisci velit";
    const auto result = gemstone_count(input);
    const auto result_fwd_apply = gemstone_count_fwd_apply(input);
    const auto result_fwd_compose = gemstone_count_fwd_compose(input);
}

std::string square_is_even(const std::vector<int>& xs)
{
    using namespace fplus;
    auto ys = fplus::transform(fplus::square<int>, xs);
    auto zs = fplus::keep_if(fplus::is_even<int>, ys);
    return show(size_of_cont(zs));
}

std::string square_is_even_chain(const std::vector<int>& xs)
{
    using namespace fplus;
    auto zs = fplus::keep_if(fplus::is_even<int>, fplus::transform(fplus::square<int>, xs));
    return show(size_of_cont(zs));
}

std::string square_is_even_fwd_apply(const std::vector<int>& xs)
{
    using namespace fplus;
    auto zs = fwd::apply(
        xs
        , fplus::fwd::transform(fplus::square<int>)
        , fplus::fwd::keep_if(fplus::is_even<int>));
    return show(size_of_cont(zs));
}

const auto square_is_even_fwd_compose = fplus::fwd::compose(
    fplus::fwd::transform(fplus::square<int>),
    fplus::fwd::keep_if(fplus::is_even<int>),
    fplus::fwd::size_of_cont(),
    fplus::fwd::show()
);

void Test_square_is_even()
{
    const std::vector<int> xs = {0,1,2,3,4,5,6,7};
    const auto result = square_is_even(xs); // uses l-value keep_if internally
    const auto result_chain = square_is_even_chain(xs); // uses r-value keep_if internally
    const auto result_fwd_apply = square_is_even_fwd_apply(xs); // uses r-value keep_if internally
    const auto result_fwd_compose = square_is_even_fwd_compose(xs); // uses r-value keep_if internally
}

int main()
{
    std::cout << "Testing Applications." << std::endl;
    Test_example_KeepIf();
    Test_example_KeepIf_performance();
    Test_example_SameOldSameOld();
    Test_example_IInTeam();
    Test_example_AllIsCalmAndBright();
    Test_example_TheCutestCat();
    Test_example_CollatzSequence();
    Test_example_fwd_style();
    Test_square_is_even();
    std::cout << "-------" << std::endl;
    std::cout << "Applications OK." << std::endl;
    std::cout << "=======" << std::endl;
}
