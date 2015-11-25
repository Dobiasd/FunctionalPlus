FunctionalPlus
==============
**helps you write concise and readable C++ code.**


Introduction
------------
Great code should mostly be self-documenting, but while using C++ in reality you can find yourself dealing with low-level stuff like iterators or hand-written loops that distract from the actual essence of the your code.

FunctionalPlus is a small header-only library supporting you in reducing code noise and in dealing with only one single level of abstraction at a time. By increasing brevity and maintainability of your code it can improve productivity (and fun!) in the long run. It tries to achieve these goals by providing easy-to-use functions that free you from implementing commonly used flows of control.

Say you have a list of numbers and are interested in the odd ones only.
```c++
bool IsOdd(int x) { return x % 2 == 1; }
int main()
{
    typedef list<int> Ints;
    Ints numbers = { 24, 11, 65, 44, 80, 18, 73, 90, 69, 18 };
    // todo: get odd values from numbers ...
}
```

There are different possiblities to obtain your goal. Some of them are:

1. write a (range based) for loop
```c++
    Ints odds;
    for (int x : numbers)
        if (IsOdd(x))
            odds.push_back(x);
```

2. use `std::copy_if` from the STL
```c++
    Ints odds;
    copy_if(begin(numbers), end(numbers), back_inserter(odds), IsOdd);
```

3. use `KeepIf` from `FunctionalPlus`
```c++
    auto odds = KeepIf(IsOdd, numbers);
```

If you think version 3 could be the one most pleasant to work with, you might like FunctionalPlus.
And if you still think the hand-written for loop is easier to understand, also consider what would happen if the loop body (i.e. a corresponding lambda function in the call to `FunctionalPlus::KeepIf`) would be much longer. When reading `KeepIf` you would still immediately know that `odds` can only contain elements that came from `numbers` and were selected by some, possibly complicated, predicate. In the for loop case you have no idea what is happening until you read the whole loop body. The loop version probably would need a comment at the top stating what the use of `KeepIf` would tell at first glance.


More examples
-------------
Below you find some short examples showing nice things you can do with functions and containers using FunctionalPlus.

### The same old song
You can test the content of a container for various properties, e.g.
```c++
#include "FunctionalPlus/FunctionalPlus.h"
#include <iostream>
int main()
{
    using namespace FunctionalPlus;
    std::list<std::string> things = {"same old", "same old"};
    if (AllTheSame(things))
        std::cout << "All things being equal." << std::endl;
}
```

### The I in our team
There also are some convenience functions for strings.
```c++
#include "FunctionalPlus/FunctionalPlus.h"
#include <iostream>
int main()
{
    using namespace FunctionalPlus;
    std::string team = "Our team is great. I love everybody.";
    if (Contains("I", SplitWords(team)))
        std::cout << "There actually is an I in team." << std::endl;
}
```

### Silent Night
Predicates can be combined logically in a functional way.
```c++
#include "FunctionalPlus/FunctionalPlus.h"
#include <iostream>
struct Entity
{
    Entity() : calm_(true), bright_(true) {}
    bool calm_;
    bool bright_;
};
bool IsCalm(const Entity& entity) { return entity.calm_; }
bool IsBright(const Entity& entity) { return entity.bright_; }

int main()
{
    using namespace FunctionalPlus;
    std::vector<Entity> entities(4);
    if (AllBy(And(IsCalm, IsBright), entities))
        std::cout << "Silent night." << std::endl;
}
```
`AllBy` is a function that takes a unary predicate and checks if all elements in the container fulfill it. `And` simply combines two unary predicates to one.

### Transformations, function composition and binding
Let's say you have the following function [given](https://gist.github.com/Dobiasd/3ca1e75a6f77172fa748).
```c++
std::list<std::uint64_t> CollatzSeq(std::uint64_t x);
```

And you want to create an `std::map<std::uint64_t, std::string>` containing string representations of the [Collatz sequences](https://en.wikipedia.org/wiki/Collatz_conjecture) for all numbers below 30. You can implement this nicely in a functional way too.

```c++
#include "FunctionalPlus/FunctionalPlus.h"
#include <iostream>

// std::list<std::uint64_t> CollatzSeq(std::uint64_t x) { ... }

int main()
{
    using namespace FunctionalPlus;
    using namespace std;

    typedef list<uint64_t> Ints;

    // [1, 2, 3 ... 29]
    auto numbers = GenerateIntegralRange<Ints>(1, 30);

    // A function that does [1, 2, 3, 4, 5] -> "[1 => 2 => 3 => 4 => 5]"
    auto ShowInts = Bind1of2(ShowContWith<Ints>, " => ");

    // A composed function that calculates a Collatz sequence and shows it.
    auto ShowCollatsSeq = Compose(CollatzSeq, ShowInts);

    // Apply it to all our numbers.
    auto seqStrs = Transform(ShowCollatsSeq, numbers);

    // Combine the numbers and their sequence representations into a map.
    auto collatzDict = CreateMap(numbers, seqStrs);

    // Print some of the sequences.
    cout << collatzDict[13] << endl;
    cout << collatzDict[17] << endl;
}
```

Output:

```
[13 => 40 => 20 => 10 => 5 => 16 => 8 => 4 => 2 => 1]
[17 => 52 => 26 => 13 => 40 => 20 => 10 => 5 => 16 => 8 => 4 => 2 => 1]
```

The functions shown not only work with default STL containers like `std::vector`, `std::list`, `std::deque`, `std::string` etc., but also with custom containers providing a similar interface.


Type deduction and useful errors messages
-----------------------------------------
FunctionalPlus deduces types for you where possible. Let's take one line of code from the Collatz example
```c++
    auto showCollatsSeq = Compose(CollatzSeq, showInts);
```

`CollatzSeq` is a Function taking an `uint64_t` and returning a `list<uint64_t>`. `ShowInts` takes a `list<uint64_t>` and returns a `string`. Thanks to making use of `function_traits` [written by kennyim](https://github.com/kennytm/utils/blob/master/traits.hpp) it is possible to automatically deduce the expression `Compose(CollatzSeq, ShowInts)` being a function taking an `uint64_t` and returning a `string`, so you do not have to manually provide type hints to the compiler.

In case you would accidentally pass two functions whose "connecting type" does not match, you will get a nice error message telling you exactly that, because FunctionalPlus uses compile time assertions where feasible to guard you from the sometimes confusingly long error messages compilers like to generate when faced with type errors in function templates.

By changing the way you think about programming from "writing your own loops and nested ifs" to "using and composing small functions" you will first perhaps get more errors at compile time, but this will pay out in having fewer errors at runtime and in spending less time debugging.


Finding the right functions
---------------------------
Library is splitted into several header files. If you for example need a function dividing a sequence at elements with a specific property, just look into `split.h` and you will soon find `FunctionalPlus::SplitBy` doing exactly this: `SplitBy(isEven, true, [1,3,2,2,5,5,3,6,7,9]) == [[1,3],[],[5,5,3],[7,9]]`

If interest in a documentation or something similar arises, I would be happy if you would [tell me what you think could a good approach](https://github.com/Dobiasd/FunctionalPlus/issues).


Performance
-----------
The basic functions are fast, thanks to C++'s concept of abstraction without overhead. Here are some measurements from the first example, taken on a standard desktop PC, compiled with GCC and the `O3` flag.
```
10000 random numbers, keep odd ones, 1000 consecutive runs accumulated
----------------------------------------------------------------------

| Hand-written for loop | std::copy_if | FunctionalPlus::KeepIf |
|-----------------------|--------------|------------------------|
|               0.468 s |      0.475 s |                0.463 s |
```

So the compiler seems to do a very good job in optimizing and inlining everthing to basically equal machine code performance-wise.

The more complex functions though can probably be written in a more optimized/optimizable way. Sometimes they are not even in the best possible time complexity class. If you use FunctionalPlus in a performance-critical scenario and profiling shows you need a faster version of a function [please let me know](https://github.com/Dobiasd/FunctionalPlus/issues) or [even help improving FunctionalPlus](https://github.com/Dobiasd/FunctionalPlus/pulls).


Installation/Requirements
-------------------------
Just download FunctionalPlus and tell your compile to use the `include` directory.

A ***C++14***-compatible compiler is needed. The tests run successfully on GCC 4.9, Clang 3.6 and Visual C++ 2015.


Naming convention or `keep_if` vs. `KeepIf`
-----------------
[`snake_case` is considered more modern by some people](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#Rl-camel), while the [Google style guide votes for `CamelCase`](https://google.github.io/styleguide/cppguide.html#Function_Names).
I do not think that it is very important as long as you are consistent throughout a project. 


Disclaimer
----------
The functionality in this library initially grew due to my personal need for it while using C++ on a regular basis. I try my best to make it error free and as comfortable to use as I can. It is still in an experimental state, and the API is likely to change in the future. If you have any suggestions, find errors, miss some functions or want to give general feedback/criticism, I'd [love to hear from you](https://github.com/Dobiasd/FunctionalPlus/issues). Of course, [contributions](https://github.com/Dobiasd/FunctionalPlus/pulls) are also very welcome.


License
-------
Distributed under the Boost Software License, Version 1.0.
(See accompanying file `LICENSE_1_0.txt` or copy at
[http://www.boost.org/LICENSE_1_0.txt](http://www.boost.org/LICENSE_1_0.txt))