![logo](logo/fplus.png)

[![Build Status](https://travis-ci.org/Dobiasd/FunctionalPlus.svg?branch=master)][travis]
[![(License Boost 1.0)](https://img.shields.io/badge/license-boost%201.0-blue.svg)][license]

[travis]: https://travis-ci.org/Dobiasd/FunctionalPlus
[license]: http://www.boost.org/LICENSE_1_0.txt


FunctionalPlus
==============
**helps you write concise and readable C++ code.**

Table of contents
-----------------
  * [Introduction](#introduction)
  * [Usage examples](#usage-examples)
  * [Type deduction and useful error messages](#type-deduction-and-useful-error-messages)
  * [Tutorial](#tutorial)
  * [Forward composition and application](#forward-composition-and-application)
  * [Finding the functions you need](#finding-the-functions-you-need)
  * [Performance](#performance)
  * [Comparison with Range-v3](#comparison-with-range-v3)
  * [Requirements and Installation](#requirements-and-installation)


Introduction
------------
Great code should mostly be self-documenting, but while using C++ in reality you can find yourself dealing with low-level stuff like iterators or hand-written loops that distract from the actual essence of your code.

**FunctionalPlus is a small header-only library** supporting you in reducing code noise and in dealing with only one single level of abstraction at a time. By increasing brevity and maintainability of your code it can improve productivity (and fun!) in the long run. It pursues these goals by providing pure and easy-to-use functions that free you from implementing commonly used flows of control over and over again.

Say you have a list of numbers and are interested in the odd ones only.

```c++
bool is_odd_int(int x) { return x % 2 == 1; }

int main()
{
    typedef vector<int> Ints;
    Ints values = {24, 11, 65, 44, 80, 18, 73, 90, 69, 18};
    // todo: get odd numbers from values ...
}
```

There are different possibilities to attain your goal. Some of them are:

1. write a (range based) for loop
 ```c++
     Ints odds;
     for (int x : values)
     {
         if (is_odd_int(x))
         {
             odds.push_back(x);
         }
     }
 ```

2. use `std::copy_if` from the STL
 ```c++
     Ints odds;
     std::copy_if(std::begin(values), std::end(values),
             std::back_inserter(odds), is_odd_int);
 ```

3. use `keep_if` from `FunctionalPlus`
 ```c++
     auto odds = fplus::keep_if(is_odd_int, values);
 ```

If you think version 3 could be the one most pleasant to work with, you might like FunctionalPlus.
And if you still think the hand-written for loop is easier to understand, also consider what would happen if the loop body (i.e. a corresponding lambda function in the call to `fplus::keep_if`) would be much longer. When reading `keep_if` you would still immediately know that `odds` can only contain elements that came from `values` and were selected by some, possibly complicated, predicate. In the for loop case you have no idea what is happening until you read the whole loop body. The loop version probably would need a comment at the top stating what the use of `keep_if` would tell at first glance.


Usage examples
--------------
Below you find some short examples showing nice things you can do with functions and containers using FunctionalPlus.

### The same old song
You can test the content of a container for various properties, e.g.
```c++
#include <fplus/fplus.hpp>
#include <iostream>

int main()
{
    std::list<std::string> things = {"same old", "same old"};
    if (fplus::all_the_same(things))
        std::cout << "All things being equal." << std::endl;
}
```

### The I in our `team`
There also are some convenience functions for retrieving properties of containers. For example you can count the occurrences of a character in a string.
```c++
#include <fplus/fplus.hpp>
#include <iostream>

int main()
{
    std::string team = "Our team is great. I love everybody I work with.";
    std::cout << "There actually are this many 'I's in team: " <<
        fplus::count("I", fplus::split_words(false, team)) << std::endl;
}
```

Output:

```
There actually are this many 'I's in team: 2
```

### The cutest kitty
Finding the highest rated element in a container is very simple compared to [a hand-written version](https://gist.github.com/Dobiasd/a4e7aa9c25a3dd4c0522d75a71e2a867#file-cuteness-cpp-L28-L38).
```c++
#include <fplus/fplus.hpp>
#include <iostream>

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
```

Output:

```
Muffin is happy and sleepy. *purr* *purr* *purr*
```

### Function composition, binding and map creation
Let's say you have the following function [given](https://gist.github.com/Dobiasd/17f5eeab2ba0ee6631394f149fc61ce2).
```c++
std::list<int> collatz_seq(int x);
```

And you want to create an `std::map<std::uint64_t, std::string>` containing string representations of the [Collatz sequences](https://en.wikipedia.org/wiki/Collatz_conjecture) for all numbers below 30. You can implement this nicely in a functional way too.

```c++
#include <fplus/fplus.hpp>
#include <iostream>

// std::list<std::uint64_t> collatz_seq(std::uint64_t x) { ... }

int main()
{
    typedef std::list<int> Ints;

    // [1, 2, 3 ... 29]
    auto xs = fplus::numbers<Ints>(1, 30);

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
```

Output:

```
[13 => 40 => 20 => 10 => 5 => 16 => 8 => 4 => 2 => 1]
[17 => 52 => 26 => 13 => 40 => 20 => 10 => 5 => 16 => 8 => 4 => 2 => 1]
```

The functions shown not only work with default STL containers like `std::vector`, `std::list`, `std::deque`, `std::string` etc., but also with custom containers providing a similar interface.


Type deduction and useful error messages
----------------------------------------

FunctionalPlus deduces types for you where possible. Let's take one line of code from the Collatz example:
```c++
    auto show_collats_seq = fplus::compose(collatz_seq, show_ints);
```

`collatz_seq` is a function taking an `uint64_t` and returning a `list<uint64_t>`. `show_ints` takes a `list<uint64_t>` and returns a `string`. Thanks to making use of `function_traits` [written by kennyim](https://github.com/kennytm/utils/blob/master/traits.hpp) it is possible to automatically deduce the expression `fplus::compose(collatz_seq, show_ints)` being a function taking an `uint64_t` and returning a `string`, so you do not have to manually provide type hints to the compiler.

In case you would accidentally pass two functions whose "connecting type" does not match, you will get a nice error message telling you exactly that, because FunctionalPlus uses compile time assertions where feasible to guard you from the sometimes confusingly long error messages compilers like to generate when faced with type errors in function templates.

By changing the way you think about programming from "writing your own loops and nested ifs" to "using and composing small functions" you will first perhaps get more errors at compile time, but this will pay out in having fewer errors at runtime and in spending less time debugging.


Tutorial
--------

The article "[Functional programming in C++ with the FunctionalPlus library; today: HackerRank challenge Gemstones](https://github.com/Dobiasd/articles/blob/master/functional_programming_in_cpp_with_the_functionalplus_library_today_hackerrank_challange_gemstones.md)" provides a smooth introduction into the library by showing how one could develop an elegant solution to a problem using the FunctionalPlus approach.


Forward composition and application
-----------------------------------

The "Gemstones" tutorial above explains how one can apply functional thinking to arrive at the solution below for the following problem:

> Find the number of characters present in every line of an input text.

```c++
std::string gemstone_count(const std::string& input)
{
    using namespace fplus;

    typedef std::set<std::string::value_type> character_set;

    const auto lines = split_lines(false, input); // false = no empty lines

    const auto sets = transform(
        convert_container<character_set, std::string>,
        lines);

    // Build the intersection of all given character sets (one per line).
    const auto gem_elements = fold_left_1(
        set_intersection<character_set>, sets);

    return show(size_of_cont(gem_elements));
}
```

By using the functionality from `namespace fwd`, you can get along without temporary variables, and make it clear that the whole process is simply pushing the input through a chain of functions, similar to the pipe concept in the Unix command line.

```c++
std::string gemstone_count_fwd_apply(const std::string& input)
{
    using namespace fplus;
    typedef std::set<std::string::value_type> character_set;
    return fwd::apply(
        input
        , fwd::split_lines(false)
        , fwd::transform(convert_container<character_set, std::string>)
        , fwd::fold_left_1(set_intersection<character_set>)
        , fwd::size_of_cont()
        , fwd::show()
    );
}
```

In `fplus::fwd::` you find many `fplus::` functions again, but in a partially [curried](http://stackoverflow.com/a/36321/1866775) version, i.e. `fplus::foo : (a, b, c) -> d` has its counterpart with `fplus::foo : (a, b) -> (c -> d)`. This makes the style above possible.

Alternatively to the forward application version, you can also write [point-free](https://en.wikipedia.org/wiki/Tacit_programming) and define your function by composition:

```c++
using namespace fplus;
typedef std::set<std::string::value_type> character_set;

const auto gemstone_count_fwd_compose = fwd::compose(
    fwd::split_lines(false),
    fwd::transform(convert_container<character_set, std::string>),
    fwd::fold_left_1(set_intersection<character_set>),
    fwd::size_of_cont(),
    fwd::show()
);
```


Finding the functions you need
------------------------------
If you are looking for a specific FunctionalPlus function you do not know the name of yet, you can of course use the auto-complete feature of your IDE to browse the content of the `namespace fplus`. But the recommended way is to simply use the **[FunctionalPlus API search website](http://www.editgym.com/fplus-api-search/)**. You can quickly search by keywords or function type signatures with it.


Performance
-----------

The basic functions are fast, thanks to C++'s concept of abstraction without overhead. Here are some measurements from the first example, taken on a standard desktop PC, compiled with GCC and the `O3` flag.
```
5000 random numbers, keep odd ones, 20000 consecutive runs accumulated
----------------------------------------------------------------------

| Hand-written for loop | std::copy_if | fplus::keep_if |
|-----------------------|--------------|----------------|
|               0.632 s |      0.641 s |        0.627 s |
```

So the compiler seems to do a very good job in optimizing and inlining everything to basically equal machine code performance-wise.

The more complex functions though sometimes could be written in a more optimized way. If you use FunctionalPlus in a performance-critical scenario and profiling shows you need a faster version of a function [please let me know](https://github.com/Dobiasd/FunctionalPlus/issues) or [even help improving FunctionalPlus](https://github.com/Dobiasd/FunctionalPlus/pulls).

Additionally keep in mind that FunctionalPlus always produces copies and never operates in place. [Even modern compilers often can not optimize these allocations away in chained calls.](http://stackoverflow.com/questions/39658768/loop-fusion-in-c-how-to-help-the-compiler). Thanks to working with a multi-paradigm language one easily can combine manually optimized imperative code with `fplus` functions. Luckily experience (aka. profiling) shows that in most cases the vast majority of code in an application is not relevant for overall performance and memory consumption. So initially focusing on developer productivity and readability of code is a good idea.


Comparison with Range-v3
------------------------

[Range-v3](https://github.com/ericniebler/range-v3) and FunctionalPlus do have things in common, as the following code snippet shows.

```c++
const auto times_3 = [](int i){return 3 * i;};
const auto is_odd = [](int i){return i % 2 == 0;};
const auto as_string_length = [](int i){return std::to_string(i).size();};

// FunctionalPlus
using namespace fplus;
const auto result_fplus = fwd::apply(
    numbers(0, 15000000)
    , fwd::transform(times_3)
    , fwd::drop_if(is_odd)
    , fwd::transform(as_string_length)
    , fwd::sum());

// Range-v3
const auto result_range_v3 =
    accumulate(
        view::ints(0)
        | view::take(15000000)
        | view::transform(times_3)
        | view::remove_if(is_odd)
        | view::transform(as_string_length)
        , 0);
```

There are some differences though. Range-v3 ranges are lazy, which means no intermediate copies of the containers are made during the single steps of a processing chain like above. Also Range-v3 will probably be [part of the C++ standard](https://ericniebler.github.io/std/wg21/D4128.html) at some point in the future.
When using FunctionalPlus on the other hand you work with normal STL-containers. Also [implementing a new function](https://github.com/Dobiasd/FunctionalPlus/blob/a17fc716d40a4370eed13f16e7d9105c4cc75e26/include/fplus/generate.hpp#L19) is simpler compared to [writing a new range adaptor](https://github.com/ericniebler/range-v3/blob/4cfcb59c3db1c279d72c64ccf15de3c724a0362d/include/range/v3/algorithm/generate.hpp#L32). Additionally FunctionalPlus provides much more functions out of the box, has the [API search website](http://www.editgym.com/fplus-api-search/) and shorter compile times (2 s vs. 12 s in the example above). So the choice between the two libraries depends on your preferences and project's needs.


Requirements and Installation
-----------------------------

A **C++11**-compatible compiler is needed (C++14 if you want to use the features from `namespace fwd`). Compilers from these versions on are fine: GCC 4.9, Clang 3.6 and Visual C++ 2015

You can install FunctionalPlus in one of the following ways:

### download manually

Just [download](https://github.com/Dobiasd/FunctionalPlus/archive/master.zip)/extract FunctionalPlus and tell your compiler to use the `include` directory.

### using [cmake](https://cmake.org/)

```
git clone https://github.com/Dobiasd/FunctionalPlus
cd FunctionalPlus
mkdir build
cd build
cmake ..
sudo make install
```

Building the tests (optional) requires [doctest](https://github.com/onqtam/doctest). Unit Tests are disabled by default – they are enabled and executed by:

```
cmake -DUNITTEST=ON -DCPP14TEST=ON ..
make unittest
```

### using [cget](https://github.com/pfultz2/cget/)

```
# Setup up toolchain to use c++11
cget init --std=c++11
# Test and install
cget install Dobiasd/FunctionalPlus
```

Disclaimer
----------
The functionality in this library initially grew due to my personal need for it while using C++ on a regular basis. I try my best to make it error free and as comfortable to use as I can. The API still might change in the future. If you have any suggestions, find errors, miss some functions or want to give general feedback/criticism, I'd [love to hear from you](https://github.com/Dobiasd/FunctionalPlus/issues). Of course, [contributions](CONTRIBUTING.md) are also very welcome.


License
-------
Distributed under the Boost Software License, Version 1.0.
(See accompanying file [`LICENSE_1_0.txt`](https://github.com/Dobiasd/FunctionalPlus/blob/master/LICENSE_1_0.txt) or copy at
[http://www.boost.org/LICENSE_1_0.txt](http://www.boost.org/LICENSE_1_0.txt))
