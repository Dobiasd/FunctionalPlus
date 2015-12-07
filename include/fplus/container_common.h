// Copyright Tobias Hermann 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "container_traits.h"
#include "maybe.h"
#include "compare.h"

#include <algorithm>
#include <cassert>
#include <iterator>

namespace fplus
{

template <typename UnaryPredicate, typename Container>
void check_unary_predicate_for_container()
{
    check_unary_predicate_for_type<UnaryPredicate,
     typename Container::value_type>();
}

template <typename UnaryPredicate, typename Container>
void check_index_with_type_predicate_for_container()
{
    check_index_with_type_predicate_for_type<UnaryPredicate,
        typename Container::value_type>();
}

template <typename Compare, typename Container>
void check_compare_for_container()
{
    check_compare_for_type<Compare, typename Container::value_type>();
}

template <typename BinaryPredicate, typename Container>
void check_binary_predicate_for_container()
{
    check_binary_predicate_for_type<BinaryPredicate,
        typename Container::value_type>();
}

// PrepareContainer and BackInserter are overloaded
// to increase performance on std::vector and std::string
// by using std::vector<T>::reserve
// and std::back_inserter instead of std::back_inserter.
// In VC2015, release mode, Celsius W520 Xeon
// this leads to an increase in performance of about a factor of 3
// for Transform.
template <typename C>
void prepare_container(const std::basic_string<C, std::char_traits<C>,
    std::allocator<C> >& ys, std::size_t size)
{
    ys.reserve(size);
}

template <typename Y>
void prepare_container(std::vector<Y>& ys, std::size_t size)
{
    ys.reserve(size);
}

template <typename Container>
void prepare_container(Container&, std::size_t)
{
}

template <typename Container, typename Y>
std::back_insert_iterator<Container> get_back_inserter(std::string& ys)
{
    return std::back_inserter(ys);
}

template <typename Container, typename Y>
std::back_insert_iterator<Container> get_back_inserter(std::vector<Y>& ys)
{
    return std::back_inserter(ys);
}

template <typename Container>
std::insert_iterator<Container> get_back_inserter(Container& ys)
{
    return std::inserter(ys, std::end(ys));
}

// is_empty([1, 2]) == false
template <typename Container>
bool is_empty(const Container& xs)
{
    return xs.empty();
}

// is_not_empty([1, 2]) == true
template <typename Container>
bool is_not_empty(const Container& xs)
{
    return !is_empty(xs);
}

// size_of_cont([3, 4]) == 2
template <typename Container>
std::size_t size_of_cont(const Container& xs)
{
    return xs.size();
}

// convert_elems<NewT>([1, 2, 3]) == [NewT(1), NewT(2), NewT(3)]
template <typename NewT, typename ContainerIn,
    typename ContainerOut = typename same_cont_new_t<ContainerIn, NewT>::type>
ContainerOut convert_elems(const ContainerIn& xs)
{
    static_assert(std::is_constructible<NewT, typename ContainerIn::value_type>::value, "Elements not convertible.");
    ContainerOut ys;
    prepare_container(ys, size_of_cont(xs));
    auto it = get_back_inserter<ContainerOut>(ys);
    // using 'for (const auto& x ...)' is even for ints as fast as
    // using 'for (int x ...)' (GCC, O3), so there is no need to
    // check if the type is fundamental and then dispatch accordingly.
    for (const auto& x : xs)
    {
        *it = NewT(x);
    }
    return ys;
}

// convert_container([1, 2, 3]) == [1, 2, 3]
// Useful for example if you want to convert an std::list to an std::vector.
template <typename ContainerOut, typename ContainerIn>
ContainerOut convert_container(const ContainerIn& xs)
{
    typedef typename ContainerIn::value_type SourceElem;
    typedef typename ContainerOut::value_type DestElem;
    static_assert(std::is_same<DestElem, SourceElem>::value,
        "Source and dest container must have the same value_type");
    ContainerOut ys;
    prepare_container(ys, size_of_cont(xs));
    auto itOut = get_back_inserter<ContainerOut>(ys);
    std::copy(std::begin(xs), std::end(xs), itOut);
    return ys;
}

// Converts between different containers and elements.
// Dest elements are allowed to have explicit constructors.
// convert([1, 2, 3]) == [1, 2, 3]
template <typename ContainerOut, typename ContainerIn>
ContainerOut convert(const ContainerIn& xs)
{
    static_assert(std::is_convertible<typename ContainerIn::value_type, typename ContainerOut::value_type>::value, "Elements not convertible.");
    typedef typename ContainerOut::value_type DestElem;
    ContainerOut ys;
    prepare_container(ys, size_of_cont(xs));
    auto it = get_back_inserter<ContainerOut>(ys);
    for (const auto& x : xs)
    {
        *it = DestElem(x);
    }
    return ys;
}

// get_range(2, 5, [0,1,2,3,4,5,6,7,8]) == [2,3,4]
template <typename Container>
Container get_range
        (std::size_t idxBegin, std::size_t idxEnd, const Container& xs)
{
    assert(idxBegin <= idxEnd);
    assert(idxEnd <= size_of_cont(xs));
    Container result;
    auto itBegin = std::begin(xs);
    std::advance(itBegin, idxBegin);
    auto itEnd = itBegin;
    std::advance(itEnd, idxEnd - idxBegin);
    std::copy(itBegin, itEnd, get_back_inserter(result));
    return result;
}

// set_range(2, [9,9,9], [0,1,2,3,4,5,6,7,8]) == [0,1,9,9,9,5,6,7,8]
template <typename Container>
Container set_range
        (std::size_t idxBegin, Container& token, const Container& xs)
{
    assert(idxBegin + size_of_cont(token) < size_of_cont(xs));
    Container result = xs;
    auto itBegin = std::begin(result);
    std::advance(itBegin, idxBegin);
    std::copy(std::begin(token), std::end(token), itBegin);
    return result;
}

// remove_range(2, 5, [0,1,2,3,4,5,6,7]) == [0,1,5,6,7]
template <typename Container>
Container remove_range
        (std::size_t idxBegin, std::size_t idxEnd, const Container& xs)
{
    assert(idxBegin <= idxEnd);
    assert(idxEnd <= size_of_cont(xs));

    Container result;
    std::size_t length = idxEnd - idxBegin;
    prepare_container(result, size_of_cont(xs) - length);

    auto firstBreakIt = std::begin(xs);
    std::advance(firstBreakIt, idxBegin);
    std::copy(std::begin(xs), firstBreakIt, get_back_inserter(result));

    auto secondBreakIt = firstBreakIt;
    std::advance(secondBreakIt, length);
    std::copy(secondBreakIt, std::end(xs), get_back_inserter(result));

    return result;
}

// insert_at(2, [8,9], [0,1,2,3,4]) == [0,1,8,9,2,3,4]
template <typename Container>
Container insert_at(std::size_t idxBegin,
        const Container& token, const Container& xs)
{
    assert(idxBegin <= size_of_cont(xs));

    Container result;
    prepare_container(result, size_of_cont(xs) + size_of_cont(token));

    auto breakIt = std::begin(xs);
    std::advance(breakIt, idxBegin);
    std::copy(std::begin(xs), breakIt, get_back_inserter(result));
    std::copy(std::begin(token), std::end(token), get_back_inserter(result));
    std::copy(breakIt, std::end(xs), get_back_inserter(result));

    return result;
}

// replace_range(2, [8,9], [0,1,2,3,4]) == [0,1,8,9,4]
template <typename Container>
Container replace_range(std::size_t idxBegin,
        const Container& token, const Container& xs)
{
    std::size_t idxEnd = idxBegin + size_of_cont(token);
    return insert_at(idxBegin, token, remove_range(idxBegin, idxEnd, xs));
}

// nth_element(2, [5,6,7,8]) == 7
template <typename Container>
typename Container::value_type nth_element
        (const std::size_t n, const Container& xs)
{
    assert(size_of_cont(xs) > n);
    return xs[n];
}

// reverse([0,4,2,6]) == [6,2,4,0]
template <typename Container>
Container reverse(const Container& xs)
{
    static_assert(has_order<Container>::value,
        "Reverse: Container has no order.");
    Container ys = xs;
    std::reverse(std::begin(ys), std::end(ys));
    return ys;
}

// take(3, [0,1,2,3,4,5,6,7]) == [0,1,2]
template <typename Container>
Container take(std::size_t amount, const Container& xs)
{
    assert(amount <= size_of_cont(xs));
    return get_range(0, amount, xs);
}

// drop(3, [0,1,2,3,4,5,6,7]) == [3,4,5,6,7]
template <typename Container>
Container drop(std::size_t amount, const Container& xs)
{
    assert(amount <= size_of_cont(xs));
    return get_range(amount, size_of_cont(xs), xs);
}

// fold_left((+), 0, [1, 2, 3]) == ((0+1)+2)+3 == 6
// (a -> b -> a) -> a -> [b] -> a
// Takes the second argument and the first item of the list
// and applies the function to them,
// then feeds the function with this result and the second argument and so on.
template <typename F, typename Container,
    typename Acc = typename utils::function_traits<F>::template arg<0>::type>
Acc fold_left(F f, const Acc& init, const Container& xs)
{
    Acc acc = init;
    for (const auto& x : xs)
    {
        acc = f(acc, x);
    }
    return acc;
}

// fold_left_1((+), [1, 2, 3]) == (1+2)+3 == 6
// (a -> a -> a) -> [a] -> a
// Takes the first 2 items of the list and applies the function to them,
// then feeds the function with this result and the third argument and so on.
template <typename F, typename Container,
    typename Acc = typename Container::value_type>
Acc fold_left_1(F f, const Container& xs)
{
    assert(!xs.empty());
    return fold_left(f, xs.front(), drop(1, xs));
}

// fold_right((+), 0, [1, 2, 3]) == 1+(2+(3+0)) == 6
// (a -> b -> b) -> b -> [a] -> b
// Takes the second argument and the last item of the list
// and applies the function,
// then it takes the penultimate item from the end and the result, and so on.
template <typename F, typename Container,
    typename Acc = typename utils::function_traits<F>::template arg<1>::type>
Acc fold_right(F f, const Acc& init, const Container& xs)
{
    return fold_left(flip(f), init, reverse(xs));
}

// fold_right_1((+), [1, 2, 3]) == 1+(2+3)) == 6
// (a -> a -> a) -> [a] -> a
// Takes the last two items of the list and applies the function,
// then it takes the third item from the end and the result, and so on.
template <typename F, typename Container,
    typename Acc = typename Container::value_type>
Acc fold_right_1(F f, const Container& xs)
{
    return fold_left_1(flip(f), reverse(xs));
}

// scan_left((+), 0, [1, 2, 3]) == [0, 1, 3, 6]
// (a -> b -> a) -> a -> [b] -> [a]
// Takes the second argument and the first item of the list
// and applies the function to them,
// then feeds the function with this result and the second argument and so on.
// It returns the list of intermediate and final results.
template <typename F, typename ContainerIn,
    typename Acc = typename utils::function_traits<F>::template arg<0>::type,
    typename ContainerOut = typename same_cont_new_t<ContainerIn, Acc>::type>
ContainerOut scan_left(F f, const Acc& init, const ContainerIn& xs)
{
    ContainerOut result;
    prepare_container(result, size_of_cont(xs));
    auto itOut = get_back_inserter(result);
    Acc acc = init;
    *itOut = acc;
    for (const auto& x : xs)
    {
        acc = f(acc, x);
        *itOut = acc;
    }
    return result;
}

// scan_left_1((+), [1, 2, 3]) == [1, 3, 6]
// (a -> a -> a) -> [a] -> [a]
// Takes the first 2 items of the list and applies the function to them,
// then feeds the function with this result and the third argument and so on.
// It returns the list of intermediate and final results.
template <typename F, typename ContainerIn,
    typename Acc = typename ContainerIn::value_type,
    typename ContainerOut = typename same_cont_new_t<ContainerIn, Acc>::type>
ContainerOut scan_left_1(F f, const ContainerIn& xs)
{
    assert(!xs.empty());
    return scan_left(f, xs.front(), drop(1, xs));
}

// scan_right((+), 0, [1, 2, 3]) == [6, 5, 3, 0]
// (a -> b -> b) -> b -> [a] -> [b]
// Takes the second argument and the last item of the list
// and applies the function,
// then it takes the penultimate item from the end and the result, and so on.
// It returns the list of intermediate and final results.
template <typename F, typename ContainerIn,
    typename Acc = typename utils::function_traits<F>::template arg<1>::type,
    typename ContainerOut = typename same_cont_new_t<ContainerIn, Acc>::type>
ContainerOut scan_right(F f, const Acc& init, const ContainerIn& xs)
{
    return reverse(scan_left(flip(f), init, reverse(xs)));
}

// scan_right_1((+), [1, 2, 3]) == [1, 3, 6]
// (a -> a -> a) -> [a] -> [a]
// Takes the last two items of the list and applies the function,
// then it takes the third item from the end and the result, and so on.
// It returns the list of inntermediate and final results.
template <typename F, typename ContainerIn,
    typename Acc = typename ContainerIn::value_type,
    typename ContainerOut = typename same_cont_new_t<ContainerIn, Acc>::type>
ContainerOut scan_right_1(F f, const ContainerIn& xs)
{
    return reverse(scan_left_1(flip(f), reverse(xs)));
}

// append([1, 2], [3, 4, 5]) == [1, 2, 3, 4, 5]
template <typename Container>
Container append(const Container& xs, const Container& ys)
{
    Container result;
    prepare_container(result, size_of_cont(xs) + size_of_cont(ys));
    std::copy(std::begin(xs), std::end(xs),
        get_back_inserter(result));
    std::copy(std::begin(ys), std::end(ys),
        get_back_inserter(result));
    return result;
}

// concat([[1, 2], [], [3]]) == [1, 2, 3]
// also known as flatten
template <typename ContainerIn,
    typename ContainerOut = typename ContainerIn::value_type>
ContainerOut concat(const ContainerIn& xss)
{
    auto append_one = [](
            const ContainerOut& acc,
            const typename ContainerIn::value_type& xs)
        { return append(acc, xs); };
    return fold_left(append_one, ContainerOut(), xss);
}

// sort by std::less
template <typename Container>
Container sort(const Container& xs)
{
    auto result = xs;
    std::sort(std::begin(result), std::end(result));
    return result;
}

// sort by given less comparator
template <typename Compare, typename Container>
Container sort_by(Compare comp, const Container& xs)
{
    auto result = xs;
    std::sort(std::begin(result), std::end(result), comp);
    return result;
}

// Like unique but with user supplied equality predicate.
template <typename Container, typename BinaryPredicate>
Container unique_by(BinaryPredicate p, const Container& xs)
{
    check_binary_predicate_for_container<BinaryPredicate, Container>();
    auto result = xs;
    auto last = std::unique(std::begin(result), std::end(result), p);
    result.erase(last, std::end(result));
    return result;
}

// unique([1,2,2,3,2]) == [1,2,3,2]
template <typename Container>
Container unique(const Container& xs)
{
    auto result = xs;
    auto last = std::unique(std::begin(result), std::end(result));
    result.erase(last, std::end(result));
    return result;
}

// intersperse(0, [1, 2, 3]) == [1, 0, 2, 0, 3]
template <typename Container,
    typename X = typename Container::value_type>
Container intersperse(const X& value, const Container& xs)
{
    if (xs.empty())
        return Container();
    if (size_of_cont(xs) == 1)
        return xs;
    Container result;
    prepare_container(result, std::max<std::size_t>(0, size_of_cont(xs)*2-1));
    auto it = get_back_inserter(result);
    for_each(std::begin(xs), --std::end(xs), [&value, &it](const X& x)
    {
        *it = x;
        *it = value;
    });
    *it = xs.back();
    return result;
}

// Also known as Intercalate.
// join([0, 0], [[1], [2], [3, 4]]) == [1, 0, 0, 2, 0, 0, 3, 4]
template <typename Container,
    typename X = typename Container::value_type>
X join(const X& separator, const Container& xs)
{
    return concat(intersperse(separator, xs));
}

// contains_by((==), [1,2,3]) == true
template <typename BinaryPredicate, typename Container>
bool contains_by(BinaryPredicate pred, const Container& xs)
{
    return std::find_if(std::begin(xs), std::end(xs), pred) != std::end(xs);
}

// contains(2, [1,2,3]) == true
template <typename Container>
bool contains(const typename Container::value_type& x, const Container& xs)
{
    typedef typename Container::value_type T;
    auto pred = bind_1st_of_2(is_equal<T>, x);
    return contains_by(pred, xs);
}

// nub_by((==), [1,2,2,3,2]) == [1,2,3]
template <typename Container, typename BinaryPredicate>
Container nub_by(BinaryPredicate p, const Container& xs)
{
    Container result;
    auto itOut = get_back_inserter(result);
    for (const auto &x : xs)
    {
        auto eqToX = bind_1st_of_2(p, x);
        if (!contains_by(eqToX, result))
        {
            *itOut = x;
        }
    }
    return result;
}

// nub([1,2,2,3,2]) == [1,2,3]
template <typename Container>
Container nub(const Container& xs)
{
    auto pred = is_equal<typename Container::value_type>;
    return nub_by(pred, xs);
}

} // namespace fplus