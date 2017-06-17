// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <fplus/fplus.hpp>
using namespace fplus;

typedef std::vector<int> Ints;
typedef std::vector<Ints> Intss;
typedef std::vector<std::size_t> Idxs;
Ints xs = {0,2,2,2,1,3,3,4};
Intss xss = {{0,1,2},{3,4}};
typedef std::pair<std::size_t, int> size_t_int_pair;
typedef std::vector<size_t_int_pair> size_t_int_pairs;
size_t_int_pairs xs_run_length_encoded = {{1, 0}, {3, 2}, {1, 1}, {2, 3}, {1, 4}};

template <typename T>
void print_result(const T& x)
{
    std::cout << show(x) << std::endl;
}

// P01 (*) Find the last box of a list.
void problem_01()
{
    print_result(last(xs));
}

// P02 (*) Find the last but one box of a list.
void problem_02()
{
    print_result(elem_at_idx(size_of_cont(xs) - 2, xs));
}

// P03 (*) Find the K'th element of a list.
void problem_03()
{
    print_result(elem_at_idx(2, xs));
}

// P04 (*) Find the number of elements of a list.
void problem_04()
{
    print_result(size_of_cont(xs));
}

// P05 (*) Reverse a list.
void problem_05()
{
    print_result(reverse(xs));
}

// P06 (*) Find out whether a list is a palindrome.
void problem_06()
{
    print_result(xs == reverse(xs));
}

// P07 (**) Flatten a nested list structure.
void problem_07()
{
    print_result(concat(xss));
}

// P08 (**) Eliminate consecutive duplicates of list elements.
void problem_08()
{
    print_result(unique(xs));
}

// P09 (**) Pack consecutive duplicates of list elements into sublists.
void problem_09()
{
    print_result(group(xs));
}

// P10 (*) Run-length encoding of a list.
void problem_10()
{
    auto group_to_pair = [](const Ints& group) -> size_t_int_pair
    {
        return std::make_pair(size_of_cont(group), group.front());
    };
    print_result(transform(group_to_pair, group(xs)));
}

// P11 (*) Modified run-length encoding.
void problem_11()
{
    const auto modify = [](const auto& p) -> Ints
    {
        return p.first == 1 ?
            Ints({p.second})
            : Ints({static_cast<int>(p.first), p.second});
    };

    print_result(fwd::apply(xs,
        fwd::run_length_encode(),
        fwd::transform(modify)
        ));
}

// P12 (**) Decode a run-length encoded list.
void problem_12()
{
    const auto pair_to_vec = [](const size_t_int_pair& p) -> Ints
    {
        return replicate(p.first, p.second);
    };
    print_result(concat(transform(pair_to_vec, xs_run_length_encoded)));
}

// P13 (**) Run-length encoding of a list (direct solution).
void problem_13()
{
    const auto f = [](const Intss& acc, int x) -> Intss
    {
        if (is_empty(acc))
        {
            return singleton_seq(singleton_seq(x));
        }
        else if (size_of_cont(acc.back()) == 1 && acc.back().back() == x)
        {
            return replace_elem_at_idx(size_of_cont(acc) - 1, {2, x}, acc);
        }
        else
        {
            if (acc.back().back() == x)
            {
                return replace_elem_at_idx(
                    size_of_cont(acc) - 1, {acc.back().front() + 1, x}, acc);
            }
            else
            {
                return append_elem(singleton_seq(x), acc);
            }
        }
    };
    print_result(fold_left(f, Intss(), xs));
}

// P14 (*) Duplicate the elements of a list.
void problem_14()
{
    print_result(replicate_elems(2, xs));
}

// P15 (**) Replicate the elements of a list a given number of times.
void problem_15()
{
    print_result(replicate_elems(3, xs));
}

// P16 (**) Drop every N'th element from a list.
void problem_16()
{
    std::size_t n = 3;
    print_result(
        drop_idxs(numbers_step(std::size_t(0), size_of_cont(xs), n), xs));
}

// P17 (*) Split a list into two parts; the length of the first part is given.
void problem_17()
{
    print_result(split_at_idx(2, xs));
}

// P18 (**) Extract a slice from a list.
void problem_18()
{
    print_result(get_segment(2, 4+1, xs));
}

// P19 (**) Rotate a list N places to the left.
void problem_19()
{
    print_result(apply_function_n_times(rotate_left<Ints>, 3, xs));
}

// P20 (*) Remove the K'th element from a list.
void problem_20()
{
    print_result(drop_idx(0, xs));
}

// P21 (*) Insert an element at a given position into a list.
void problem_21()
{
    print_result(insert_at_idx(3, -1, xs));
}

// P22 (*) Create a list containing all integers within a given range.
void problem_22()
{
    print_result(numbers<int>(3, 8));
}

// P23 (**) Extract a given number of randomly selected elements from a list.
void problem_23()
{
    print_result(sample(std::random_device()(), 3, xs));
}

// P24 (*) Lotto: Draw N different random numbers from the set 1..M.
void problem_24()
{
    print_result(sample(std::random_device()(), 3, numbers<int>(1, 100)));
}

// P25 (*) Generate a random permutation of the elements of a list.
void problem_25()
{
    print_result(shuffle(std::random_device()(), xs));
}

// P26 (**) Generate the combinations of K distinct objects chosen from the N elements of a list
void problem_26()
{
    print_result(combinations(3, xs));
}

// P27 (**) Group the elements of a set into disjoint subsets.
void problem_27()
{
    // todo ;)
}

// P28 (**) Sorting a list of lists according to length of sublists
void problem_28()
{
    print_result(sort_on(size_of_cont<Ints>, xss));
}

// Let's follow tradition for once and at least do the list-related problems.
// L-99: Ninety-Nine Lisp Problems: http://www.ic.unicamp.br/~meidanis/courses/mc336/2006s2/funcional/L-99_Ninety-Nine_Lisp_Problems.html
// P-99: Ninety-Nine Prolog Problems: https://sites.google.com/site/prologsite/prolog-problems
// H-99: Ninety-Nine Haskell Problems: https://wiki.haskell.org/H-99:_Ninety-Nine_Haskell_Problems
int main()
{
    problem_01();
    problem_02();
    problem_03();
    problem_04();
    problem_05();
    problem_06();
    problem_07();
    problem_08();
    problem_09();
    problem_10();
    problem_11();
    problem_12();
    problem_13();
    problem_14();
    problem_15();
    problem_16();
    problem_17();
    problem_18();
    problem_19();
    problem_20();
    problem_21();
    problem_22();
    problem_23();
    problem_24();
    problem_25();
    problem_26();
    problem_27();
    problem_28();
}
