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
void CheckUnaryPredicateForContainer()
{
    CheckUnaryPredicateForType<UnaryPredicate, typename Container::value_type>();
}

template <typename UnaryPredicate, typename Container>
void CheckIndexWithTypePredicateForContainer()
{
    CheckIndexWithTypePredicateForType<UnaryPredicate, typename Container::value_type>();
}

template <typename Compare, typename Container>
void CheckCompareForContainer()
{
    CheckCompareForType<Compare, typename Container::value_type>();
}

template <typename BinaryPredicate, typename Container>
void CheckBinaryPredicateForContainer()
{
    CheckBinaryPredicateForType<BinaryPredicate, typename Container::value_type>();
}

// PrepareContainer and BackInserter are overloaded
// to increase performance on std::vector and std::string
// by using std::vector<T>::reserve
// and std::back_inserter instead of std::back_inserter.
// In VC2015, release mode, Celsius W520 Xeon
// this leads to an increase in performance of about a factor of 3
// for Transform.
template <typename C>
void PrepareContainer(const std::basic_string<C, std::char_traits<C>,
    std::allocator<C> >& ys, std::size_t size)
{
    ys.reserve(size);
}

template <typename Y>
void PrepareContainer(std::vector<Y>& ys, std::size_t size)
{
    ys.reserve(size);
}

template <typename Container>
void PrepareContainer(Container&, std::size_t)
{
}

template <typename Container, typename Y>
std::back_insert_iterator<Container> BackInserter(std::string& ys)
{
    return std::back_inserter(ys);
}

template <typename Container, typename Y>
std::back_insert_iterator<Container> BackInserter(std::vector<Y>& ys)
{
    return std::back_inserter(ys);
}

template <typename Container>
std::insert_iterator<Container> BackInserter(Container& ys)
{
    return std::inserter(ys, std::end(ys));
}

// IsEmpty([1, 2]) == false
template <typename Container>
bool IsEmpty(const Container& xs)
{
    return xs.empty();
}

// IsEmpty([1, 2]) == true
template <typename Container>
bool IsNotEmpty(const Container& xs)
{
    return !IsEmpty(xs);
}

// Size([3, 4]) == 2
template <typename Container>
std::size_t Size(const Container& xs)
{
    return xs.size();
}

// ConvertElems<NewT>([1, 2, 3]) == [NewT(1), NewT(2), NewT(3)]
template <typename NewT, typename ContainerIn,
    typename ContainerOut = typename SameContNewT<ContainerIn, NewT>::type>
ContainerOut ConvertElems(const ContainerIn& xs)
{
    static_assert(std::is_constructible<NewT, typename ContainerIn::value_type>::value, "Elements not convertible.");
    ContainerOut ys;
    PrepareContainer(ys, Size(xs));
    auto it = BackInserter<ContainerOut>(ys);
    // using 'for (const auto& x ...)' is even for ints as fast as
    // using 'for (int x ...)' (GCC, O3), so there is no need to
    // check if the type is fundamental and then dispatch accordingly.
    for (const auto& x : xs)
    {
        *it = NewT(x);
    }
    return ys;
}

// ConvertContainer([1, 2, 3]) == [1, 2, 3]
// Useful for example if you want to convert an std::list to an std::vector.
template <typename ContainerOut, typename ContainerIn>
ContainerOut ConvertContainer(const ContainerIn& xs)
{
    typedef typename ContainerIn::value_type SourceElem;
    typedef typename ContainerOut::value_type DestElem;
    static_assert(std::is_same<DestElem, SourceElem>::value,
        "ConvertContainer: Source and dest container must have the same value_type");
    ContainerOut ys;
    PrepareContainer(ys, Size(xs));
    auto itOut = BackInserter<ContainerOut>(ys);
    std::copy(std::begin(xs), std::end(xs), itOut);
    return ys;
}

// Converts between different containers and elements.
// Dest elements are allowed to have explicit constructors.
// Convert([1, 2, 3]) == [1, 2, 3]
template <typename ContainerOut, typename ContainerIn>
ContainerOut Convert(const ContainerIn& xs)
{
    static_assert(std::is_convertible<typename ContainerIn::value_type, typename ContainerOut::value_type>::value, "Elements not convertible.");
    typedef typename ContainerOut::value_type DestElem;
    ContainerOut ys;
    PrepareContainer(ys, Size(xs));
    auto it = BackInserter<ContainerOut>(ys);
    for (const auto& x : xs)
    {
        *it = DestElem(x);
    }
    return ys;
}

// GetRange(2, 5, [0,1,2,3,4,5,6,7,8]) == [2,3,4]
template <typename Container>
Container GetRange
        (std::size_t idxBegin, std::size_t idxEnd, const Container& xs)
{
    assert(idxBegin <= idxEnd);
    assert(idxEnd <= Size(xs));
    Container result;
    auto itBegin = std::begin(xs);
    std::advance(itBegin, idxBegin);
    auto itEnd = itBegin;
    std::advance(itEnd, idxEnd - idxBegin);
    std::copy(itBegin, itEnd, BackInserter(result));
    return result;
}

// SetRange(2, [9,9,9], [0,1,2,3,4,5,6,7,8]) == [0,1,9,9,9,5,6,7,8]
template <typename Container>
Container SetRange
        (std::size_t idxBegin, Container& token, const Container& xs)
{
    assert(idxBegin + Size(token) < Size(xs));
    Container result = xs;
    auto itBegin = std::begin(result);
    std::advance(itBegin, idxBegin);
    std::copy(std::begin(token), std::end(token), itBegin);
    return result;
}

// RemoveRange(2, 3, [0,1,2,3,4,5,6,7]) == [0,1,5,6,7]
template <typename Container>
Container RemoveRange
        (std::size_t idxBegin, std::size_t idxEnd, const Container& xs)
{
    assert(idxBegin <= idxEnd);
    assert(idxEnd <= Size(xs));

    Container result;
    std::size_t length = idxEnd - idxBegin;
    PrepareContainer(result, Size(xs) - length);

    auto firstBreakIt = std::begin(xs);
    std::advance(firstBreakIt, idxBegin);
    std::copy(std::begin(xs), firstBreakIt, BackInserter(result));

    auto secondBreakIt = firstBreakIt;
    std::advance(secondBreakIt, length);
    std::copy(secondBreakIt, std::end(xs), BackInserter(result));

    return result;
}

// InsertAt(2, [8,9], [0,1,2,3,4]) == [0,1,8,9,2,3,4]
template <typename Container>
Container InsertAt(std::size_t idxBegin,
        const Container& token, const Container& xs)
{
    assert(idxBegin <= Size(xs));

    Container result;
    PrepareContainer(result, Size(xs) + Size(token));

    auto breakIt = std::begin(xs);
    std::advance(breakIt, idxBegin);
    std::copy(std::begin(xs), breakIt, BackInserter(result));
    std::copy(std::begin(token), std::end(token), BackInserter(result));
    std::copy(breakIt, std::end(xs), BackInserter(result));

    return result;
}

// ReplaceRange(2, [8,9], [0,1,2,3,4]) == [0,1,8,9,4]
template <typename Container>
Container ReplaceRange(std::size_t idxBegin,
        const Container& token, const Container& xs)
{
    std::size_t idxEnd = idxBegin + Size(token);
    return InsertAt(idxBegin, token, RemoveRange(idxBegin, idxEnd, xs));
}

// NthElement(2, [5,6,7,8]) == 7
template <typename Container>
typename Container::value_type NthElement
        (const std::size_t n, const Container& xs)
{
    assert(Size(xs) > n);
    return xs[n];
}

// Reverse([0,4,2,6]) == [6,2,4,0]
template <typename Container>
Container Reverse(const Container& xs)
{
    static_assert(has_order<Container>::value, "Reverse: Container has no order.");
    Container ys = xs;
    std::reverse(std::begin(ys), std::end(ys));
    return ys;
}

// Foldl((+), 0, [1, 2, 3]) = ((0+1)+2)+3 = 6
// (a -> b -> a) -> a -> [b] -> a
// Takes the second argument and the first item of the list and applies the function to them, then feeds the function with this result and the second argument and so on.
template <typename F, typename Container,
    typename Acc = typename utils::function_traits<F>::template arg<0>::type>
Acc Foldl(F f, const Acc& init, const Container& xs)
{
    Acc acc = init;
    for (const auto& x : xs)
    {
        acc = f(acc, x);
    }
    return acc;
}

// Foldr((+), 0, [1, 2, 3]) = 1+(2+(3+0)) = 6
// (a -> b -> b) -> b -> [a] -> b
// Takes the second argument and the last item of the list and applies the function, then it takes the penultimate item from the end and the result, and so on.
template <typename F, typename Container,
    typename Acc = typename utils::function_traits<F>::template arg<1>::type>
Acc Foldr(F f, const Acc& init, const Container& xs)
{
    return Foldl(Flip(f), init, Reverse(xs));
}

// Scanl((+), 0, [1, 2, 3]) = [0, 1, 3, 6]
// (a -> b -> a) -> a -> [b] -> [a]
// Takes the second argument and the first item of the list and applies the function to them, then feeds the function with this result and the second argument and so on. It returns the list of intermediate and final results.
template <typename F, typename ContainerIn,
    typename Acc = typename utils::function_traits<F>::template arg<0>::type,
    typename ContainerOut = typename SameContNewT<ContainerIn, Acc>::type>
ContainerOut Scanl(F f, const Acc& init, const ContainerIn& xs)
{
    ContainerOut result;
    PrepareContainer(result, Size(xs));
    auto itOut = BackInserter(result);
    Acc acc = init;
    *itOut = acc;
    for (const auto& x : xs)
    {
        acc = f(acc, x);
        *itOut = acc;
    }
    return result;
}

// Scanr((+), 0, [1, 2, 3]) = [6, 5, 3, 0]
// (a -> b -> b) -> b -> [a] -> [b]
// Takes the second argument and the last item of the list and applies the function, then it takes the penultimate item from the end and the result, and so on. It returns the list of intermediate and final results.
template <typename F, typename ContainerIn,
    typename Acc = typename utils::function_traits<F>::template arg<1>::type,
    typename ContainerOut = typename SameContNewT<ContainerIn, Acc>::type>
ContainerOut Scanr(F f, const Acc& init, const ContainerIn& xs)
{
    return Reverse(Scanl(Flip(f), init, Reverse(xs)));
}

// [1, 2] -> [3, 4] -> [1, 2, 3, 4]
template <typename Container>
Container Append(const Container& xs, const Container& ys)
{
    Container result;
    PrepareContainer(result, Size(xs) + Size(ys));
    std::copy(std::begin(xs), std::end(xs),
        BackInserter(result));
    std::copy(std::begin(ys), std::end(ys),
        BackInserter(result));
    return result;
}

// [[1, 2], [], [3]] -> [1, 2, 3], also known as flatten
template <typename ContainerIn,
    typename ContainerOut = typename ContainerIn::value_type>
ContainerOut Concat(const ContainerIn& xss)
{
    auto append = [](
            const ContainerOut& acc,
            const typename ContainerIn::value_type& xs)
        { return Append<ContainerOut>(acc, xs); };
    return Foldl<decltype(append)>(append, ContainerOut(), xss);
}

// sort by std::less
template <typename Container>
Container Sort(const Container& xs)
{
    auto result = xs;
    std::sort(std::begin(result), std::end(result));
    return result;
}

// sort by given less comparator
template <typename Compare, typename Container>
Container SortBy(Compare comp, const Container& xs)
{
    auto result = xs;
    std::sort(std::begin(result), std::end(result), comp);
    return result;
}

// Unique [1,2,2,3,2] -> [1,2,3,2]
template <typename Container>
Container Unique(const Container& xs)
{
    auto result = xs;
    auto last = std::unique(std::begin(result), std::end(result));
    result.erase(last, std::end(result));
    return result;
}

// Like Unique but with user supplied equality predicate.
template <typename Container, typename BinaryPredicate>
Container UniqueBy(BinaryPredicate p, const Container& xs)
{
    auto result = xs;
    auto last = std::unique(std::begin(result), std::end(result), p);
    result.erase(last, std::end(result));
    return result;
}

// Intersperse(0, [1, 2, 3]) == [1, 0, 2, 0, 3]
template <typename Container,
    typename X = typename Container::value_type>
Container Intersperse(const X& value, const Container& xs)
{
    if (xs.empty())
        return Container();
    if (Size(xs) == 1)
        return xs;
    Container result;
    PrepareContainer(result, std::max<std::size_t>(0, Size(xs) * 2 - 1));
    auto it = BackInserter(result);
    for_each(std::begin(xs), --std::end(xs), [&value, &it](const X& x)
    {
        *it = x;
        *it = value;
    });
    *it = xs.back();
    return result;
}

// Also known as Intercalate.
// Join([0, 0] -> [[1], [2], [3, 4]] -> [1, 0, 0, 2, 0, 0, 3, 4]
template <typename Container,
    typename X = typename Container::value_type>
X Join(const X& separator, const Container& xs)
{
    return Concat(Intersperse(separator, xs));
}

// ContainsBy((==), [1,2,3]) == true
template <typename BinaryPredicate, typename Container>
bool ContainsBy(BinaryPredicate pred, const Container& xs)
{
    return std::find_if(std::begin(xs), std::end(xs), pred) != std::end(xs);
}

// Contains(2, [1,2,3]) == true
template <typename Container>
bool Contains(const typename Container::value_type& x, const Container& xs)
{
    typedef typename Container::value_type T;
    auto pred = Bind1of2(IsEqual<T>, x);
    return ContainsBy(pred, xs);
}

// NubBy((==), [1,2,2,3,2]) == [1,2,3]
template <typename Container, typename BinaryPredicate>
Container NubBy(BinaryPredicate p, const Container& xs)
{
    Container result;
    auto itOut = BackInserter(result);
    for (const auto &x : xs)
    {
        auto eqToX = Bind1of2(p, x);
        if (!ContainsBy(eqToX, result))
        {
            *itOut = x;
        }
    }
    return result;
}

// Nub([1,2,2,3,2]) == [1,2,3]
template <typename Container>
Container Nub(const Container& xs)
{
    auto pred = IsEqual<typename Container::value_type>;
    return NubBy(pred, xs);
}

} // namespace fplus