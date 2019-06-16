// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <fplus/composition.hpp>
#include <fplus/container_traits.hpp>
#include <fplus/maybe.hpp>
#include <fplus/compare.hpp>

#include <fplus/internal/meta.hpp>
#include <fplus/internal/invoke.hpp>
#include <fplus/internal/container_common.hpp>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <iterator>
#include <numeric>

namespace fplus
{

namespace internal
{
    template <typename UnaryPredicate, typename Container>
    void check_unary_predicate_for_container()
    {
        internal::check_unary_predicate_for_type<UnaryPredicate,
            typename Container::value_type>();
    }

    template <typename F, typename Container>
    void check_index_with_type_predicate_for_container()
    {
        typedef typename Container::value_type T;
        internal::trigger_static_asserts<internal::binary_function_tag, F, std::size_t, T>();
        static_assert(std::is_convertible<
            internal::invoke_result_t<F, std::size_t, T>, bool>::value,
            "Function must return bool.");
    }

    template <typename Compare, typename Container>
    void check_compare_for_container()
    {
        typedef typename Container::value_type T;
        internal::trigger_static_asserts<internal::binary_predicate_tag, Compare, T, T>();
    }

    template <typename BinaryPredicate, typename Container>
    void check_binary_predicate_for_container()
    {
        typedef typename Container::value_type T;
        internal::trigger_static_asserts<internal::binary_predicate_tag, BinaryPredicate, T, T>();
    }

    // PrepareContainer and BackInserter are overloaded
    // to increase performance on std::vector and std::string
    // by using std::vector<T>::reserve
    // and std::back_inserter instead of std::back_inserter.
    // In VC2015, release mode, Celsius W520 Xeon
    // this leads to an increase in performance of about a factor of 3
    // for transform.
    template <typename C>
    void prepare_container(const std::basic_string<C, std::char_traits<C>,
        std::allocator<C>>& ys, std::size_t size)
    {
        ys.reserve(size);
    }

    template <typename Y>
    void prepare_container(std::vector<Y>& ys, std::size_t size)
    {
        ys.reserve(size);
    }

    template <typename T, std::size_t N>
    void prepare_container(std::array<T, N>&, std::size_t size)
    {
        assert(size == N);
    }

    template <typename Y>
    void prepare_container(std::unordered_set<Y>& ys, std::size_t size)
    {
        ys.reserve(size);
    }

    template <typename Key, typename T>
    void prepare_container(std::unordered_map<Key, T>& ys, std::size_t size)
    {
        ys.reserve(size);
    }

    template <typename Y>
    void prepare_container(std::unordered_multiset<Y>& ys, std::size_t size)
    {
        ys.reserve(size);
    }

    template <typename Key, typename T>
    void prepare_container(std::unordered_multimap<Key, T>& ys, std::size_t size)
    {
        ys.reserve(size);
    }

    template <typename Container>
    void prepare_container(Container&, std::size_t)
    {
    }

    template <typename Container>
    std::back_insert_iterator<Container> get_back_inserter(std::string& ys)
    {
        return std::back_inserter(ys);
    }

    template <typename Container, typename Y>
    std::back_insert_iterator<Container> get_back_inserter(std::vector<Y>& ys)
    {
        return std::back_inserter(ys);
    }

    template <typename Container, typename Y>
    std::back_insert_iterator<Container> get_back_inserter(std::list<Y>& ys)
    {
        return std::back_inserter(ys);
    }

    template <typename Container, typename Y>
    std::back_insert_iterator<Container> get_back_inserter(std::deque<Y>& ys)
    {
        return std::back_inserter(ys);
    }

    template <typename T, std::size_t N>
    struct array_back_insert_iterator : public std::back_insert_iterator<std::array<T, N>>
    {
        typedef std::back_insert_iterator<std::array<T, N>> base_type;
        explicit array_back_insert_iterator(std::array<T, N>& arr) :
            base_type(arr), arr_ptr_(&arr), pos_(0) {}
        array_back_insert_iterator<T, N>(const array_back_insert_iterator<T, N>& other) :
            base_type(*other.arr_ptr_), arr_ptr_(other.arr_ptr_), pos_(other.pos_) {}
        array_back_insert_iterator<T, N>& operator=(const array_back_insert_iterator<T, N>& other)
        {
            arr_ptr_ = other.arr_ptr_;
            pos_ = other.pos_;
            return *this;
        }
        ~array_back_insert_iterator()
        {
            assert(pos_ == 0 || pos_ == N);
        }
        array_back_insert_iterator<T, N>& operator=(const T& x)
        {
            assert(pos_ < N);
            (*arr_ptr_)[pos_] = x;
            ++pos_;
            return *this;
        }
        array_back_insert_iterator<T, N>& operator=(T&& x)
        {
            assert(pos_ < N);
            (*arr_ptr_)[pos_] = std::move(x);
            ++pos_;
            return *this;
        }
        array_back_insert_iterator<T, N>& operator*() { return *this; }
        array_back_insert_iterator<T, N>& operator++() { return *this; }
        array_back_insert_iterator<T, N> operator++(int) { return *this; }
    private:
        std::array<T, N>* arr_ptr_;
        std::size_t pos_;
    };

#if _MSC_VER >= 1900
    template <typename T, std::size_t N>
    struct std::_Is_checked_helper<array_back_insert_iterator<T, N>>
        : public true_type
    { // mark array_back_insert_iterator as checked
    };
#endif

    template <typename Container, typename Y, std::size_t N>
    array_back_insert_iterator<Y, N> get_back_inserter(std::array<Y, N>& ys)
    {
        return array_back_insert_iterator<Y, N>(ys);
    }

    template <typename Container>
    std::insert_iterator<Container> get_back_inserter(Container& ys)
    {
        return std::inserter(ys, std::end(ys));
    }

    template <typename Iterator>
    void advance_iterator(Iterator& it, std::size_t distance)
    {
        std::advance(it,
            static_cast<typename Iterator::difference_type>(distance));
    }

    template <typename T>
    void advance_iterator(T*& it, std::size_t distance)
    {
        it += static_cast<std::ptrdiff_t>(distance);
    }

    template <typename Iterator>
    Iterator add_to_iterator(Iterator it, std::size_t distance = 1)
    {
        return std::next(it,
            static_cast<typename Iterator::difference_type>(distance));
    }
} // namespace internal

// API search type: is_empty : [a] -> Bool
// fwd bind count: 0
// Returns true if the container holds no elements.
// is_empty([1, 2]) == false
// is_empty([]) == true
template <typename Container>
bool is_empty(const Container& xs)
{
    return xs.empty();
}

// API search type: is_not_empty : [a] -> Bool
// fwd bind count: 0
// Returns true if the container holds at least one element.
// is_not_empty([1, 2]) == true
template <typename Container>
bool is_not_empty(const Container& xs)
{
    return !is_empty(xs);
}

// API search type: size_of_cont : [a] -> Int
// fwd bind count: 0
// Returns the number of elements in the given container.
// size_of_cont([3, 4]) == 2
template <typename Container>
std::size_t size_of_cont(const Container& xs)
{
    return xs.size();
}

// API search type: convert : a -> b
// fwd bind count: 0
// Converts one type of element into another.
template <typename Dest, typename Source>
Dest convert(const Source& x)
{
    return Dest(x);
}

// API search type: convert_elems : [a] -> [b]
// fwd bind count: 0
// Converts all elements in a sequence to a different type.
// convert_elems<NewT>([1, 2, 3]) == [NewT(1), NewT(2), NewT(3)]
template <typename NewT, typename ContainerIn,
    typename ContainerOut = typename internal::same_cont_new_t<ContainerIn, NewT, 0>::type>
ContainerOut convert_elems(const ContainerIn& xs)
{
    static_assert(std::is_constructible<NewT,
        typename ContainerIn::value_type>::value,
        "Elements not convertible.");
    ContainerOut ys;
    internal::prepare_container(ys, size_of_cont(xs));
    auto it = internal::get_back_inserter<ContainerOut>(ys);
    // using 'for (const auto& x ...)' is even for ints as fast as
    // using 'for (int x ...)' (GCC, O3), so there is no need to
    // check if the type is fundamental and then dispatch accordingly.
    for (const auto& x : xs)
    {
        *it = convert<NewT>(x);
    }
    return ys;
}

// API search type: convert_container : [a] -> [a]
// fwd bind count: 0
// Change the type of the container
// while keeping the elements in the sequence the same.
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
    internal::prepare_container(ys, size_of_cont(xs));
    auto itOut = internal::get_back_inserter<ContainerOut>(ys);
    std::copy(std::begin(xs), std::end(xs), itOut);
    return ys;
}

// API search type: convert_container_and_elems : [a] -> [b]
// fwd bind count: 0
// Converts between different containers and elements.
// Dest elements are allowed to have explicit constructors.
// convert([1, 2, 3]) == [1, 2, 3]
template <typename ContainerOut, typename ContainerIn>
ContainerOut convert_container_and_elems(const ContainerIn& xs)
{
    static_assert(std::is_convertible<typename ContainerIn::value_type,
        typename ContainerOut::value_type>::value,
        "Elements not convertible.");
    typedef typename ContainerOut::value_type DestElem;
    ContainerOut ys;
    internal::prepare_container(ys, size_of_cont(xs));
    auto it = internal::get_back_inserter<ContainerOut>(ys);
    for (const auto& x : xs)
    {
        *it = convert<DestElem>(x);
    }
    return ys;
}

namespace internal
{

template <typename Container>
Container get_segment(internal::reuse_container_t,
    std::size_t idx_begin, std::size_t idx_end, Container&& xs)
{
    idx_end = std::min(idx_end, size_of_cont(xs));
    if (idx_end <= idx_begin)
    {
        xs.clear();
        return std::forward<Container>(xs);
    }
    auto itBegin = std::begin(xs);
    internal::advance_iterator(itBegin, idx_begin);
    auto itEnd = itBegin;
    internal::advance_iterator(itEnd, idx_end - idx_begin);
    xs.erase(std::copy(itBegin, itEnd, std::begin(xs)), std::end(xs));
    return std::forward<Container>(xs);
}

template <typename Container>
Container get_segment(internal::create_new_container_t,
    std::size_t idx_begin, std::size_t idx_end, const Container& xs)
{
    idx_end = std::min(idx_end, size_of_cont(xs));
    if (idx_end <= idx_begin)
    {
        return {};
    }
    Container result;
    auto itBegin = std::begin(xs);
    internal::advance_iterator(itBegin, idx_begin);
    auto itEnd = itBegin;
    internal::advance_iterator(itEnd, idx_end - idx_begin);
    std::copy(itBegin, itEnd, internal::get_back_inserter(result));
    return result;
}

} // namespace internal

// API search type: get_segment : (Int, Int, [a]) -> [a]
// fwd bind count: 2
// Return a defined segment from the sequence.
// get_segment(2, 5, [0,1,2,3,4,5,6,7,8]) == [2,3,4]
// get_segment(2, 15, [0,1,2,3,4,5,6,7,8]) == [2,3,4,5,6,7,8]
// get_segment(5, 2, [0,1,2,3,4,5,6,7,8]) == []
// Also known as slice.
template <typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut get_segment
        (std::size_t idx_begin, std::size_t idx_end, Container&& xs)
{
    return internal::get_segment(internal::can_reuse_v<Container>{},
        idx_begin, idx_end, std::forward<Container>(xs));
}

namespace internal
{

template <typename ContainerToken, typename Container>
Container set_segment(internal::reuse_container_t,
    std::size_t idx_begin, const ContainerToken& token, Container&& xs)
{
    assert(idx_begin + size_of_cont(token) < size_of_cont(xs));
    auto itBegin = std::begin(xs);
    internal::advance_iterator(itBegin, idx_begin);
    std::copy(std::begin(token), std::end(token), itBegin);
    return std::forward<Container>(xs);
}

template <typename ContainerToken, typename Container>
Container set_segment(internal::create_new_container_t,
    std::size_t idx_begin, const ContainerToken& token, const Container& xs)
{
    Container result = xs;
    return set_segment(internal::reuse_container_t(),
        idx_begin, token, std::move(result));
}

} // namespace internal

// API search type: set_segment : (Int, [a], [a]) -> [a]
// fwd bind count: 2
// Replace part of a sequence with a token.
// set_segment(2, [9,9,9], [0,1,2,3,4,5,6,7,8]) == [0,1,9,9,9,5,6,7,8]
// Crashes on invalid indices.
// Also known as replace_segment.
template <typename ContainerToken, typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut set_segment
        (std::size_t idx_begin, const ContainerToken& token, Container&& xs)
{
    return internal::set_segment(internal::can_reuse_v<Container>{},
        idx_begin, token, std::forward<Container>(xs));
}

namespace internal
{

template <typename Container>
Container remove_segment(internal::reuse_container_t,
    std::size_t idx_begin, std::size_t idx_end, Container&& xs)
{
    assert(idx_begin <= idx_end);
    assert(idx_end <= size_of_cont(xs));

    auto firstBreakIt = std::begin(xs);
    internal::advance_iterator(firstBreakIt, idx_begin);

    auto secondBreakIt = std::begin(xs);
    internal::advance_iterator(secondBreakIt, idx_end);

    xs.erase(
        std::copy(secondBreakIt, std::end(xs), firstBreakIt), std::end(xs));
    return std::forward<Container>(xs);
}

template <typename Container>
Container remove_segment(internal::create_new_container_t,
    std::size_t idx_begin, std::size_t idx_end, const Container& xs)
{
    assert(idx_begin <= idx_end);
    assert(idx_end <= size_of_cont(xs));

    Container result;
    std::size_t length = idx_end - idx_begin;
    internal::prepare_container(result, size_of_cont(xs) - length);

    auto firstBreakIt = std::begin(xs);
    internal::advance_iterator(firstBreakIt, idx_begin);
    std::copy(std::begin(xs), firstBreakIt, internal::get_back_inserter(result));

    auto secondBreakIt = std::begin(xs);
    internal::advance_iterator(secondBreakIt, idx_end);
    std::copy(secondBreakIt, std::end(xs), internal::get_back_inserter(result));

    return result;
}

} // namespace internal

// API search type: remove_segment : (Int, Int, [a]) -> [a]
// fwd bind count: 2
// Cuts our a  defined segment from the sequence.
// remove_segment(2, 5, [0,1,2,3,4,5,6,7]) == [0,1,5,6,7]
// crashes on invalid indices
template <typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut remove_segment(
        std::size_t idx_begin, std::size_t idx_end, Container&& xs)
{
    return internal::remove_segment(internal::can_reuse_v<Container>{},
        idx_begin, idx_end, std::forward<Container>(xs));
}

// API search type: insert_at : (Int, [a], [a]) -> [a]
// fwd bind count: 2
// Inserts a token into a sequence at a specific position.
// insert_at(2, [8,9], [0,1,2,3,4]) == [0,1,8,9,2,3,4]
// Unsafe! Crashes on invalid index.
template <typename Container>
Container insert_at(std::size_t idx_begin,
        const Container& token, const Container& xs)
{
    assert(idx_begin <= size_of_cont(xs));

    Container result;
    internal::prepare_container(result, size_of_cont(xs) + size_of_cont(token));

    auto breakIt = std::begin(xs);
    internal::advance_iterator(breakIt, idx_begin);
    std::copy(std::begin(xs), breakIt, internal::get_back_inserter(result));
    std::copy(std::begin(token), std::end(token), internal::get_back_inserter(result));
    std::copy(breakIt, std::end(xs), internal::get_back_inserter(result));

    return result;
}

// API search type: elem_at_idx : (Int, [a]) -> a
// fwd bind count: 1
// Return the nth element of a sequence.
// elem_at_idx(2, [7,6,5,4,3]) == 5
// Unsafe! Crashes on invalid index.
template <typename Container>
auto elem_at_idx(std::size_t idx, const Container& xs)
{
    assert(idx < size_of_cont(xs));
    auto it = std::begin(xs);
    internal::advance_iterator(it, idx);
    return *it;
}

// API search type: elem_at_idx_maybe : (Int, [a]) -> Maybe a
// fwd bind count: 1
// Return the nth element of a sequence if existing.
// elem_at_idx_maybe(2, [7,6,5,4,3]) == Just 5
// elem_at_idx_maybe(9, [7,6,5,4,3]) == Nothing
// Use elem_at_idx_or_nothing if you want to provide a signed index type.
template <typename Container,
    typename T = typename Container::value_type>
maybe<T> elem_at_idx_maybe(std::size_t idx, const Container& xs)
{
    if (size_of_cont(xs) < idx)
    {
        return {};
    }
    auto it = std::begin(xs);
    internal::advance_iterator(it, idx);
    return *it;
}

// API search type: elems_at_idxs : ([Int], [a]) -> [a]
// fwd bind count: 1
// Construct a subsequence from the elements with the given indices.
// elem_at_idxs([1, 3], [7,6,5,4,3]) == [6, 4]
template <typename Container,
    typename ContainerIdxs,
    typename T = typename Container::value_type,
    typename ContainerOut = std::vector<T>>
std::vector<T> elems_at_idxs(const ContainerIdxs& idxs, const Container& xs)
{
    static_assert(std::is_same<typename ContainerIdxs::value_type, std::size_t>::value,
        "Indices must be std::size_t");
    ContainerOut result;
    internal::prepare_container(result, size_of_cont(idxs));
    auto itOut = internal::get_back_inserter(result);
    for (std::size_t idx : idxs)
    {
        *itOut = elem_at_idx(idx, xs);
    }
    return result;
}

namespace internal
{

template <typename Container, typename F>
Container transform(internal::reuse_container_t, F f, Container&& xs)
{
    internal::trigger_static_asserts<internal::unary_function_tag,
                                         F,
                                         decltype(*std::begin(xs))>();
    std::transform(std::begin(xs), std::end(xs), std::begin(xs), f);
    return std::forward<Container>(xs);
}

template <typename ContainerOut, typename F, typename ContainerIn>
ContainerOut transform(internal::create_new_container_t, F f,
    const ContainerIn& xs)
{
    internal::trigger_static_asserts<internal::unary_function_tag,
                                         F,
                                         decltype(*std::begin(xs))>();
    ContainerOut ys;
    internal::prepare_container(ys, size_of_cont(xs));
    auto it = internal::get_back_inserter<ContainerOut>(ys);
    std::transform(std::begin(xs), std::end(xs), it, f);
    return ys;
}

} // namespace internal

// API search type: transform : ((a -> b), [a]) -> [b]
// fwd bind count: 1
// Apply a function to every element in a sequence.
// transform((*2), [1, 3, 4]) == [2, 6, 8]
// Also known as map or fmap.
template <typename F, typename ContainerIn,
    typename ContainerOut = typename internal::same_cont_new_t_from_unary_f<
        internal::remove_const_and_ref_t<ContainerIn>, F, 0>::type>
ContainerOut transform(F f, ContainerIn&& xs)
{
    using reuse_t = typename std::conditional<
        std::is_same<
            internal::can_reuse_v<ContainerIn>,
            internal::reuse_container_t>::value &&
        std::is_base_of<
            std::true_type,
            internal::has_order<ContainerIn>>::value &&
        std::is_same<
            internal::remove_const_and_ref_t<ContainerIn>,
            ContainerOut>::value,
        internal::reuse_container_t,
        internal::create_new_container_t>::type;
    return internal::transform<ContainerOut>(
        reuse_t{}, f, std::forward<ContainerIn>(xs));
}

// API search type: transform_convert : ((a -> b), [a]) -> [b]
// fwd bind count: 1
// transform_convert((*2), [1, 3, 4]) == [2, 6, 8]
// Same as transform, but makes it easy to
// use an output container type different from the input container type.
template <typename ContainerOut, typename F, typename ContainerIn>
ContainerOut transform_convert(F f, const ContainerIn& xs)
{
    internal::trigger_static_asserts<internal::unary_function_tag, F, typename ContainerIn::value_type>();
    ContainerOut ys;
    internal::prepare_container(ys, size_of_cont(xs));
    auto it = internal::get_back_inserter<ContainerOut>(ys);
    std::transform(std::begin(xs), std::end(xs), it, f);
    return ys;
}

// API search type: transform_inner : ((a -> b), [[a]]) -> [[b]]
// fwd bind count: 1
// Applies a function to the elements of the inner containers
// of a nested sequence.
// transform_inner((*2), [[1, 3, 4], [1, 2]]) == [[2, 6, 8], [2, 4]]
// Also known as transform_nested, map_nested or map_inner.
template <typename F, typename ContainerIn,
    typename ContainerOut =
        typename internal::same_cont_new_t<
            ContainerIn,
            typename internal::same_cont_new_t_from_unary_f<
                typename ContainerIn::value_type, F, 0
            >::type, 0
        >::type>
ContainerOut transform_inner(F f, const ContainerIn& xs)
{
    internal::trigger_static_asserts<internal::unary_function_tag, F, typename ContainerIn::value_type::value_type>();
    return fplus::transform(
        fplus::bind_1st_of_2(
            fplus::transform<F, const typename ContainerIn::value_type&>, f),
        xs);
}

namespace internal
{

template <typename Container>
Container reverse(internal::reuse_container_t, Container&& xs)
{
    static_assert(internal::has_order<Container>::value,
        "Reverse: Container has no order.");
    std::reverse(std::begin(xs), std::end(xs));
    return std::forward<Container>(xs);
}

template <typename Container>
Container reverse(internal::create_new_container_t, const Container& xs)
{
    static_assert(internal::has_order<Container>::value,
        "Reverse: Container has no order.");
    Container ys = xs;
    std::reverse(std::begin(ys), std::end(ys));
    return ys;
}

} // namespace internal

// API search type: reverse : [a] -> [a]
// fwd bind count: 0
// Reverse a sequence.
// reverse([0,4,2,6]) == [6,2,4,0]
template <typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut reverse(Container&& xs)
{
    return internal::reverse(internal::can_reuse_v<Container>{},
        std::forward<Container>(xs));
}

// API search type: take : (Int, [a]) -> [a]
// fwd bind count: 1
// Return the first n elements of a sequence xs.
// In case n >= length(xs), xs is returned.
// take(3, [0,1,2,3,4,5,6,7]) == [0,1,2]
// take(10, [0,1,2]) == [0,1,2]
template <typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut take(std::size_t amount, Container&& xs)
{
    if (amount >= size_of_cont(xs))
        return xs;
    return get_segment(0, amount, std::forward<Container>(xs));
}

// API search type: take_exact : (Int, [a]) -> [a]
// fwd bind count: 1
// Return exactly the first n elements of a sequence xs.
// Unsafe! Crashes then sequence is too short.
// take_exact(3, [0,1,2,3,4,5,6,7]) == [0,1,2]
// take_exact(10, [0,1,2]) == crash
template <typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut take_exact(std::size_t amount, Container&& xs)
{
    return get_segment(0, amount, std::forward<Container>(xs));
}

// API search type: take_cyclic : (Int, [a]) -> [a]
// fwd bind count: 1
// Takes n elements from a sequence considering it as cyclic.
// take_cyclic(5, [0,1,2,3]) == [0,1,2,3,0]
// take_cyclic(7, [0,1,2,3]) == [0,1,2,3,0,1,2]
// take_cyclic(7, [0,1]) == [0,1,0,1,0,1,0]
// take_cyclic(2, [0,1,2,3]) == [0,1]
// take_cyclic(3, [0]) == [0,0,0]
// take_cyclic(3, []) == crash!
// Also known as take_wrap.
// xs must be non-empty.
template <typename Container>
Container take_cyclic(std::size_t amount, const Container& xs)
{
    assert(!xs.empty());

    Container ys;
    internal::prepare_container(ys, size_of_cont(xs));
    auto it_out = internal::get_back_inserter(ys);
    auto it_in = std::begin(xs);

    while (amount != 0)
    {
        *it_out = *it_in;
        --amount;
        ++it_in;
        if (it_in == std::end(xs))
        {
            it_in = std::begin(xs);
        }
    }
    return ys;
}

// API search type: drop : (Int, [a]) -> [a]
// fwd bind count: 1
// Skip the first n elements of a sequence xs.
// If n > length(xs) an empty sequence is returned.
// drop(3, [0,1,2,3,4,5,6,7]) == [3,4,5,6,7]
// Also known as skip.
template <typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut drop(std::size_t amount, Container&& xs)
{
    if (amount >= size_of_cont(xs))
        return ContainerOut();
    return get_segment(amount, size_of_cont(xs), std::forward<Container>(xs));
}

// API search type: take_last : (Int, [a]) -> [a]
// fwd bind count: 1
// Return the last n elements of a sequence xs.
// In case n >= length(xs), xs is returned.
// take_last(3, [0,1,2,3,4,5,6,7]) == [5,6,7]
// take_last(10, [0,1,2]) == [0,1,2]
template <typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut take_last(std::size_t amount, Container&& xs)
{
    if (amount >= size_of_cont(xs))
        return xs;
    return drop(size_of_cont(xs) - amount, std::forward<Container>(xs));
}

// API search type: drop_last : (Int, [a]) -> [a]
// fwd bind count: 1
// Skip the last n elements of a sequence xs.
// If n > length(xs) an empty sequence is returned.
// drop_last(3, [0,1,2,3,4,5,6,7]) == [0,1,2,3,4]
template <typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut drop_last(std::size_t amount, Container&& xs)
{
    if (amount >= size_of_cont(xs))
        return ContainerOut();
    return take(size_of_cont(xs) - amount, std::forward<Container>(xs));
}

// API search type: drop_exact : (Int, [a]) -> [a]
// fwd bind count: 1
// Skip exactly the first n elements of a sequence xs.
// Unsafe! Crashes when xs is too short.
// drop_exact(3, [0,1,2,3,4,5,6,7]) == [3,4,5,6,7]
// drop_exact(10, [0,1,2,3,4,5,6,7]) == crash
template <typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut drop_exact(std::size_t amount, Container&& xs)
{
    return get_segment(amount, size_of_cont(xs), std::forward<Container>(xs));
}

// API search type: take_while : ((a -> Bool), [a]) -> [a]
// fwd bind count: 1
// Take elements from the beginning of a sequence
// as long as they are fulfilling a predicate.
// take_while(is_even, [0,2,4,5,6,7,8]) == [0,2,4]
template <typename Container, typename UnaryPredicate>
Container take_while(UnaryPredicate pred, const Container& xs)
{
    internal::check_unary_predicate_for_container<UnaryPredicate, Container>();
    auto itFirst = std::find_if(
        std::begin(xs), std::end(xs), logical_not(pred));
    if (itFirst == std::end(xs))
        return xs;
    return Container(std::begin(xs), itFirst);
}

// API search type: drop_while : ((a -> Bool), [a]) -> [a]
// fwd bind count: 1
// Remove elements from the beginning of a sequence
// as long as they are fulfilling a predicate.
// drop_while(is_even, [0,2,4,5,6,7,8]) == [5,6,7,8]
// Also known as trim_left_by.
template <typename Container, typename UnaryPredicate>
Container drop_while(UnaryPredicate pred, const Container& xs)
{
    internal::check_unary_predicate_for_container<UnaryPredicate, Container>();
    auto itFirstNot = std::find_if_not(std::begin(xs), std::end(xs), pred);
    if (itFirstNot == std::end(xs))
        return Container();
    return Container(itFirstNot, std::end(xs));
}

// API search type: fold_left : (((a, b) -> a), a, [b]) -> a
// fwd bind count: 2
// fold_left((+), 0, [1, 2, 3]) == ((0+1)+2)+3 == 6
// Takes the second argument and the first item of the list
// and applies the function to them,
// then feeds the function with this result and the second argument and so on.
template <typename F, typename Container, typename Acc>
Acc fold_left(F f, const Acc& init, const Container& xs)
{
    using std::begin;
    using std::end;

    return internal::accumulate(begin(xs), end(xs), init, f);
}

// API search type: reduce : (((a, a) -> a), a, [a]) -> a
// fwd bind count: 2
// reduce((+), 0, [1, 2, 3]) == (0+1+2+3) == 6
// Combines the initial value and all elements of the sequence
// using the given function.
// The set of f, init and value_type should form a monoid.
template <typename F, typename Container>
typename Container::value_type reduce(
    F f, const typename Container::value_type& init, const Container& xs)
{
    return fold_left(f, init, xs);
}

// API search type: fold_left_1 : (((a, a) -> a), [a]) -> a
// fwd bind count: 1
// fold_left_1((+), [1, 2, 3]) == (1+2)+3 == 6
// Takes the first 2 items of the list and applies the function to them,
// then feeds the function with this result and the third argument and so on.
// xs must be non-empty.
template <typename F, typename Container>
auto fold_left_1(F f, const Container& xs)
{
    assert(!xs.empty());

    using std::begin;
    using std::end;

    const auto it = begin(xs);
    return internal::accumulate(std::next(it), end(xs), *it, f);
}

// API search type: reduce_1 : (((a, a) -> a), [a]) -> a
// fwd bind count: 1
// reduce_1((+), [1, 2, 3]) == (1+2+3) == 6
// Joins all elements of the sequence using the given function.
// The set of f and value_type should form a semigroup.
template <typename F, typename Container>
typename Container::value_type reduce_1(F f, const Container& xs)
{
    assert(!xs.empty());
    return fold_left_1(f, xs);
}

// API search type: fold_right : (((a, b) -> b), b) -> [a] -> b
// fwd bind count: 2
// fold_right((+), 0, [1, 2, 3]) == 1+(2+(3+0)) == 6
// Takes the second argument and the last item of the list
// and applies the function,
// then it takes the penultimate item from the end and the result, and so on.
template <typename F, typename Container, typename Acc>
Acc fold_right(F f, const Acc& init, const Container& xs)
{
    return internal::accumulate(xs.rbegin(), xs.rend(), init, flip(f));
}

// API search type: fold_right_1 : (((a, a) -> a), [a]) -> a
// fwd bind count: 1
// fold_right_1((+), [1, 2, 3]) == 1+(2+3)) == 6
// Takes the last two items of the list and applies the function,
// then it takes the third item from the end and the result, and so on.
template <typename F, typename Container>
auto fold_right_1(F f, const Container& xs)
{
    assert(!xs.empty());
    const auto it = xs.rbegin();
    return internal::accumulate(std::next(it), xs.rend(), *it, flip(f));
}

// API search type: scan_left : (((a, b) -> a), a, [b]) -> [a]
// fwd bind count: 2
// scan_left((+), 0, [1, 2, 3]) == [0, 1, 3, 6]
// Takes the second argument and the first item of the list
// and applies the function to them,
// then feeds the function with this result and the second argument and so on.
// It returns the list of intermediate and final results.
template <typename F, typename ContainerIn, typename Acc>
auto scan_left(F f, const Acc& init, const ContainerIn& xs)
{
    using ContainerOut =
        typename internal::same_cont_new_t<ContainerIn, Acc, 1>::type;

    ContainerOut result;
    internal::prepare_container(result, size_of_cont(xs) + 1);

    using std::begin;
    using std::end;

    internal::scan_impl(
        f, init, internal::get_back_inserter(result), begin(xs), end(xs));
    return result;
}

// API search type: scan_left_1 : (((a, a) -> a), [a]) -> [a]
// fwd bind count: 1
// scan_left_1((+), [1, 2, 3]) == [1, 3, 6]
// Takes the first 2 items of the list and applies the function to them,
// then feeds the function with this result and the third argument and so on.
// It returns the list of intermediate and final results.
// xs must be non-empty.
template <typename F, typename ContainerIn>
auto scan_left_1(F f, const ContainerIn& xs)
{
    assert(!xs.empty());

    using std::begin;
    using std::end;

    const auto beginIt = begin(xs);

    using ContainerOut = typename internal::same_cont_new_t<
        ContainerIn,
        internal::uncvref_t<decltype(*beginIt)>,
        0>::type;

    ContainerOut result;
    internal::prepare_container(result, size_of_cont(xs));
    internal::scan_impl(f,
                      *beginIt,
                      internal::get_back_inserter(result),
                      std::next(beginIt),
                      end(xs));
    return result;
}

// API search type: scan_right : (((a, b) -> b), b, [a]) -> [b]
// fwd bind count: 2
// scan_right((+), 0, [1, 2, 3]) == [6, 5, 3, 0]
// Takes the second argument and the last item of the list
// and applies the function,
// then it takes the penultimate item from the end and the result, and so on.
// It returns the list of intermediate and final results.
template <typename F, typename ContainerIn,
    typename Acc = typename utils::function_traits<F>::template arg<1>::type,
    typename ContainerOut = typename internal::same_cont_new_t<ContainerIn, Acc, 1>::type>
ContainerOut scan_right(F f, const Acc& init, const ContainerIn& xs)
{
    return reverse(scan_left(flip(f), init, reverse(xs)));
}

// API search type: scan_right_1 : (((a, a) -> a), [a]) -> [a]
// fwd bind count: 1
// scan_right_1((+), [1, 2, 3]) == [6, 5, 3]
// Takes the last two items of the list and applies the function,
// then it takes the third item from the end and the result, and so on.
// It returns the list of inntermediate and final results.
template <typename F, typename ContainerIn,
    typename Acc = typename ContainerIn::value_type,
    typename ContainerOut = typename internal::same_cont_new_t<ContainerIn, Acc, 0>::type>
ContainerOut scan_right_1(F f, const ContainerIn& xs)
{
    return reverse(scan_left_1(flip(f), reverse(xs)));
}

// API search type: sum : [a] -> a
// fwd bind count: 0
// Adds up all values in a sequence.
// sum([0,3,1]) == 4
// sum([]) == 0
template <typename Container,
    typename T = typename Container::value_type>
T sum(const Container& xs)
{
    T result = T();
    for (const auto& x : xs)
    {
        result = result + x;
    }
    return result;
}

// API search type: product : [a] -> a
// fwd bind count: 0
// Returns the product of all values in a sequence.
// product([3,1,2]) == 6
// product([]) == 1
template <typename Container,
    typename T = typename Container::value_type>
T product(const Container& xs)
{
    T result{1};
    for (const auto& x : xs)
    {
        result = result * x;
    }
    return result;
}

namespace internal
{

template <typename T, typename Container>
Container append_elem(internal::reuse_container_t, const T& y, Container&& xs)
{
    *internal::get_back_inserter(xs) = y;
    return std::forward<Container>(xs);
}

template <typename T, typename Container>
Container append_elem(internal::create_new_container_t, const T& y,
    const Container& xs)
{
    Container result;
    internal::prepare_container(result, size_of_cont(xs) + 1);
    std::copy(std::begin(xs), std::end(xs),
        internal::get_back_inserter(result));
    *internal::get_back_inserter(result) = y;
    return result;
}

} // namespace internal

// API search type: append_elem : (a, [a]) -> [a]
// fwd bind count: 1
// Extends a sequence with one element at the back.
// append_elem([1, 2], 3) == [1, 2, 3]
template <typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>,
    typename T = typename ContainerOut::value_type>
ContainerOut append_elem(const T& y, Container&& xs)
{
    return internal::append_elem(internal::can_reuse_v<Container>{},
        y, std::forward<Container>(xs));
}

namespace internal
{

template <typename T>
std::list<T> prepend_elem(internal::reuse_container_t,
    const T& y, std::list<T>&& xs)
{
    xs.push_front(y);
    return std::forward<std::list<T>>(xs);
}

template <typename T, typename Container>
Container prepend_elem(internal::reuse_container_t,
    const T& y, Container&& xs)
{
    xs.resize(size_of_cont(xs) + 1);
    std::copy(++xs.rbegin(), xs.rend(), xs.rbegin());
    *std::begin(xs) = y;
    return std::forward<Container>(xs);
}

template <typename T, typename Container>
Container prepend_elem(internal::create_new_container_t, const T& y,
    const Container& xs)
{
    Container result;
    internal::prepare_container(result, size_of_cont(xs) + 1);
    *internal::get_back_inserter(result) = y;
    std::copy(std::begin(xs), std::end(xs),
        internal::get_back_inserter(result));
    return result;
}

} // namespace internal

// API search type: prepend_elem : (a, [a]) -> [a]
// fwd bind count: 1
// Extends a sequence with one element in the front.
// prepend_elem([2, 3], 1) == [1, 2, 3]
template <typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>,
    typename T = typename ContainerOut::value_type>
ContainerOut prepend_elem(const T& y, Container&& xs)
{
    return internal::prepend_elem(internal::can_reuse_v<Container>{},
        y, std::forward<Container>(xs));
}

// API search type: append : ([a], [a]) -> [a]
// fwd bind count: 1
// Concatenates two sequences.
// append([1, 2], [3, 4, 5]) == [1, 2, 3, 4, 5]
template <typename ContainerIn1, typename ContainerIn2 = ContainerIn1, typename ContainerOut = ContainerIn1>
ContainerOut append(const ContainerIn1& xs, const ContainerIn2& ys)
{
    ContainerOut result;
    internal::prepare_container(result, size_of_cont(xs) + size_of_cont(ys));
    std::copy(std::begin(xs), std::end(xs),
        internal::get_back_inserter(result));
    std::copy(std::begin(ys), std::end(ys),
        internal::get_back_inserter(result));
    return result;
}


// API search type: append_convert : ([a], [a]) -> [a]
// fwd bind count: 1
// Same as append, but makes it easier to
// use an output container type different from the input container type.
template <typename ContainerOut, typename ContainerIn1, typename ContainerIn2 = ContainerIn1>
ContainerOut append_convert(const ContainerIn1& xs, const ContainerIn2& ys)
{
    return append<ContainerIn1, ContainerIn2, ContainerOut>(xs, ys);
}

// API search type: concat : [[a]] -> [a]
// fwd bind count: 0
// Concatenates multiple sequences.
// concat([[1, 2], [], [3]]) == [1, 2, 3]
// Also known as flatten.
template <typename ContainerIn,
    typename ContainerOut = typename ContainerIn::value_type>
ContainerOut concat(const ContainerIn& xss)
{
    std::size_t length = sum(
        transform(size_of_cont<typename ContainerIn::value_type>, xss));
    ContainerOut result;
    internal::prepare_container(result, length);
    using std::begin;
    using std::end;
    for(const auto& xs : xss)
    {
        result.insert(end(result), begin(xs), end(xs));
    }
    return result;
}

// API search type: interweave : ([a], [a]) -> [a]
// fwd bind count: 1
// Return a sequence that contains elements from the two provided sequences
// in alternating order. If one list runs out of items,
// appends the items from the remaining list.
// interweave([1,3], [2,4]) == [1,2,3,4]
// interweave([1,3,5,7], [2,4]) == [1,2,3,4,5,7]
// See interleave for interweaving more than two sequences.
template <typename Container>
Container interweave(const Container& xs, const Container& ys)
{
    Container result;
    internal::prepare_container(result, size_of_cont(xs) + size_of_cont(ys));
    auto it = internal::get_back_inserter<Container>(result);
    auto it_xs = std::begin(xs);
    auto it_ys = std::begin(ys);
    while (it_xs != std::end(xs) || it_ys != std::end(ys))
    {
        if (it_xs != std::end(xs))
            *it = *(it_xs++);
        if (it_ys != std::end(ys))
            *it = *(it_ys++);
    }
    return result;
}

// API search type: unweave : [a] -> ([a], [a])
// fwd bind count: 0
// Puts the elements with an even index into the first list,
// and the elements with an odd index into the second list.
// Inverse of interweave.
// unweave([0,1,2,3]) == ([0,2], [1,3])
// unweave([0,1,2,3,4]) == ([0,2,4], [1,3])
template <typename Container>
std::pair<Container, Container> unweave(const Container& xs)
{
    std::pair<Container, Container> result;
    if (size_of_cont(xs) % 2 == 0)
        internal::prepare_container(result.first, size_of_cont(xs) / 2);
    else
        internal::prepare_container(result.first, size_of_cont(xs) / 2 + 1);
    internal::prepare_container(result.second, size_of_cont(xs) / 2);
    auto it_even = internal::get_back_inserter<Container>(result.first);
    auto it_odd = internal::get_back_inserter<Container>(result.second);
    std::size_t counter = 0;
    for (const auto& x : xs)
    {
        if (counter++ % 2 == 0)
            *it_even = x;
        else
            *it_odd = x;
    }
    return result;
}

namespace internal
{

template <typename Compare, typename T>
std::list<T> sort_by(internal::reuse_container_t, Compare comp,
    std::list<T>&& xs)
{
    xs.sort(comp);
    return std::forward<std::list<T>>(xs);
}

template <typename Compare, typename T>
std::list<T> sort_by(internal::create_new_container_t, Compare comp,
    const std::list<T>& xs)
{
    auto result = xs;
    result.sort(comp);
    return result;
}

template <typename Compare, typename Container>
Container sort_by(internal::reuse_container_t, Compare comp, Container&& xs)
{
    std::sort(std::begin(xs), std::end(xs), comp);
    return std::forward<Container>(xs);
}

template <typename Compare, typename Container>
Container sort_by(internal::create_new_container_t, Compare comp,
    const Container& xs)
{
    auto result = xs;
    std::sort(std::begin(result), std::end(result), comp);
    return result;
}

} // namespace internal

// API search type: sort_by : (((a, a) -> Bool), [a]) -> [a]
// fwd bind count: 1
// Sort a sequence by given less comparator.
template <typename Compare, typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut sort_by(Compare comp, Container&& xs)
{
    return internal::sort_by(internal::can_reuse_v<Container>{},
        comp, std::forward<Container>(xs));
}

namespace internal
{
    // workarounds for clang bug 24115
    // (std::sort and std::unique with std::function as comp)
    // https://llvm.org/bugs/show_bug.cgi?id=24115
    template <typename F>
    struct is_less_by_struct
    {
        is_less_by_struct(F f) : f_(f) {};
        template <typename T>
        bool operator()(const T& x, const T& y)
        {
            return f_(x) < f_(y);
        }
    private:
        F f_;
    };
    template <typename F>
    struct is_equal_by_struct
    {
        is_equal_by_struct(F f) : f_(f) {};
        template <typename T>
        bool operator()(const T& x, const T& y)
        {
            return f_(x) == f_(y);
        }
    private:
        F f_;
    };
}

// API search type: sort_on : ((a -> b), [a]) -> [a]
// fwd bind count: 1
// Sort a sequence by a given transformer.
template <typename F, typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut sort_on(F f, Container&& xs)
{
    return sort_by(internal::is_less_by_struct<F>(f),
        std::forward<Container>(xs));
}

// API search type: sort : [a] -> [a]
// fwd bind count: 0
// Sort a sequence to ascending order using std::less.
template <typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut sort(Container&& xs)
{
    typedef typename std::remove_reference<Container>::type::value_type T;
    return sort_by(std::less<T>(), std::forward<Container>(xs));
}

namespace internal
{

template <typename Compare, typename T>
std::list<T> stable_sort_by(internal::reuse_container_t, Compare comp,
    std::list<T>&& xs)
{
    xs.sort(comp); // std::list<T>::sort ist already stable.
    return std::forward<std::list<T>>(xs);
}

template <typename Compare, typename T>
std::list<T> stable_sort_by(internal::create_new_container_t, Compare comp,
    const std::list<T>& xs)
{
    auto result = xs;
    result.sort(comp); // std::list<T>::sort ist already stable.
    return result;
}

template <typename Compare, typename Container>
Container stable_sort_by(internal::reuse_container_t, Compare comp,
    Container&& xs)
{
    std::sort(std::begin(xs), std::end(xs), comp);
    return std::forward<Container>(xs);
}

template <typename Compare, typename Container>
Container stable_sort_by(internal::create_new_container_t, Compare comp,
    const Container& xs)
{
    auto result = xs;
    std::sort(std::begin(result), std::end(result), comp);
    return result;
}

} // namespace internal


// API search type: stable_sort_by : (((a, a) -> Bool), [a]) -> [a]
// fwd bind count: 1
// Sort a sequence stably by given less comparator.
template <typename Compare, typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut stable_sort_by(Compare comp, Container&& xs)
{
    return internal::stable_sort_by(internal::can_reuse_v<Container>{},
        comp, std::forward<Container>(xs));
}

// API search type: stable_sort_on : ((a -> b), [a]) -> [a]
// fwd bind count: 1
// Sort a sequence stably by given transformer.
template <typename F, typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut stable_sort_on(F f, Container&& xs)
{
    return stable_sort_by(internal::is_less_by_struct<F>(f),
        std::forward<Container>(xs));
}

// API search type: stable_sort : [a] -> [a]
// fwd bind count: 0
// Sort a sequence stably to ascending order using std::less.
template <typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut stable_sort(Container&& xs)
{
    typedef typename std::remove_reference<Container>::type::value_type T;
    return stable_sort_by(std::less<T>(), std::forward<Container>(xs));
}

namespace internal
{

template <typename Compare, typename Container>
Container partial_sort_by(internal::reuse_container_t, Compare comp,
    std::size_t count, Container&& xs)
{
    if (count > xs.size())
    {
        count = xs.size();
    }
    auto middle = std::begin(xs);
    internal::advance_iterator(middle, count);
    std::partial_sort(std::begin(xs), middle, std::end(xs), comp);
    return std::forward<Container>(get_segment(internal::reuse_container_t(),
        0, count, xs));
}

template <typename Compare, typename Container>
Container partial_sort_by(internal::create_new_container_t, Compare comp,
    std::size_t count, const Container& xs)
{
    auto result = xs;
    return partial_sort_by(
        internal::reuse_container_t(), comp, count, std::move(result));
}

} // namespace internal

// API search type: partial_sort_by : (((a, a) -> Bool), Int, [a]) -> [a]
// fwd bind count: 2
// Partially sort a sequence by a given less comparator.
// Returns only the sorted segment.
template <typename Compare, typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut partial_sort_by(Compare comp, std::size_t count, Container&& xs)
{
    return internal::partial_sort_by(internal::can_reuse_v<Container>{},
        comp, count, std::forward<Container>(xs));
}

// API search type: partial_sort_on : ((a -> b), Int, [a]) -> [a]
// fwd bind count: 2
// Partially sort a sequence by a given transformer.
// Returns only the sorted segment.
template <typename F, typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut partial_sort_on(F f, std::size_t count, Container&& xs)
{
    return partial_sort_by(internal::is_less_by_struct<F>(f), count,
        std::forward<Container>(xs));
}

// API search type: partial_sort : (Int, [a]) -> [a]
// fwd bind count: 1
// Partially sort a sequence in ascending order using std::less.
// Returns only the sorted segment.
template <typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut partial_sort(std::size_t count, Container&& xs)
{
    typedef typename std::remove_reference<Container>::type::value_type T;
    return partial_sort_by(std::less<T>(), count,
        std::forward<Container>(xs));
}

// API search type: nth_element_by : (((a, a) -> Bool), Int, [a]) -> a
// fwd bind count: 2
// Return the nth largest element of a sequence by a given less comparator.
template <typename Compare, typename Container,
    typename T = typename Container::value_type>
T nth_element_by(Compare comp, std::size_t n, const Container& xs)
{
    assert(n < xs.size());
    auto result = xs;
    auto middle = std::begin(result);
    internal::advance_iterator(middle, n);
    std::nth_element(std::begin(result), middle, std::end(result), comp);
    return *middle;
}

// API search type: nth_element_on : ((a -> b), Int, [a]) -> a
// fwd bind count: 2
// Return the nth largest element of a sequence by a given transformer.
template <typename F, typename Container,
    typename T = typename Container::value_type>
T nth_element_on(F f, std::size_t n, const Container& xs)
{
    return nth_element_by(internal::is_less_by_struct<F>(f), n, xs);
}

// API search type: nth_element : (Int, [a]) -> a
// fwd bind count: 1
// Return the nth largest element of a sequence using std::less.
template <typename Container,
    typename T = typename Container::value_type>
T nth_element(std::size_t n, const Container& xs)
{
    return nth_element_by(std::less<T>(), n, xs);
}

namespace internal
{

template <typename BinaryPredicate, typename Container>
Container unique_by(internal::reuse_container_t,
    BinaryPredicate pred, Container&& xs)
{
    internal::check_binary_predicate_for_container<BinaryPredicate, Container>();
    const auto it_end = std::unique(std::begin(xs), std::end(xs), pred);
    xs.erase(it_end, std::end(xs));
    return std::forward<Container>(xs);
}

template <typename BinaryPredicate, typename Container>
Container unique_by(internal::create_new_container_t,
    BinaryPredicate pred, const Container& xs)
{
    auto result = xs;
    return unique_by(internal::reuse_container_t(), pred, std::move(result));
}

} // namespace internal

// API search type: unique_by : (((a, a) -> Bool), [a]) -> [a]
// fwd bind count: 1
// Like unique, eliminates all but the first element
// from every consecutive group of equivalent elements from the sequence;
// but with a user supplied equality predicate.
// See nub_by for making elements globally unique in a sequence.
// O(n)
template <typename BinaryPredicate, typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut unique_by(BinaryPredicate pred, Container&& xs)
{
    return internal::unique_by(internal::can_reuse_v<Container>{},
        pred, std::forward<Container>(xs));
}

// API search type: unique_on : ((a -> b), [a]) -> [a]
// fwd bind count: 1
// Like unique, eliminates all but the first element
// from every consecutive group of equivalent elements from the sequence;
// but with a user supplied transformation (e.g. getter).
// See nub_on for making elements globally unique in a sequence.
// O(n)
// Also known as drop_repeats.
template <typename F, typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut unique_on(F f, Container&& xs)
{
    return unique_by(internal::is_equal_by_struct<F>(f),
        std::forward<Container>(xs));
}

// API search type: unique : [a] -> [a]
// fwd bind count: 0
// Eliminates all but the first element
// from every consecutive group of equivalent elements from the sequence.
// unique([1,2,2,3,2]) == [1,2,3,2]
// See nub for making elements globally unique in a sequence.
// O(n)
template <typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut unique(Container&& xs)
{
    typedef typename std::remove_reference<Container>::type::value_type T;
    return unique_on(identity<T>, std::forward<Container>(xs));
}

// API search type: intersperse : (a, [a]) -> [a]
// fwd bind count: 1
// Insert a value between all adjacent values in a sequence.
// intersperse(0, [1, 2, 3]) == [1, 0, 2, 0, 3]
// Also known as interpose.
template <typename Container,
    typename X = typename Container::value_type>
Container intersperse(const X& value, const Container& xs)
{
    if (xs.empty())
        return Container();
    if (size_of_cont(xs) == 1)
        return xs;
    Container result;
    internal::prepare_container(result, std::max<std::size_t>(0, size_of_cont(xs)*2-1));
    auto it = internal::get_back_inserter(result);
    for_each(std::begin(xs), --std::end(xs), [&value, &it](const X& x)
    {
        *it = x;
        *it = value;
    });
    *it = xs.back();
    return result;
}

// API search type: join : ([a], [[a]]) -> [a]
// fwd bind count: 1
// Inserts a separator sequence in between the elements
// of a sequence of sequences and concatenates the result.
// Also known as intercalate or implode.
// join(", ", ["a", "bee", "cee"]) == "a, bee, cee"
// join([0, 0], [[1], [2], [3, 4]]) == [1, 0, 0, 2, 0, 0, 3, 4]
template <typename Container,
    typename X = typename Container::value_type>
X join(const X& separator, const Container& xs)
{
    return concat(intersperse(separator, xs));
}

// API search type: join_elem : (a, [[a]]) -> [a]
// fwd bind count: 1
// Inserts a separator in between the elements
// of a sequence of sequences and concatenates the result.
// Also known as intercalate_elem.
// join_elem(';', ["a", "bee", "cee"]) == "a;bee;cee"
// join_elem(0, [[1], [2], [3, 4]]) == [1, 0, 2, 0, 3, 4]]
template <typename Container,
    typename Inner = typename Container::value_type,
    typename X = typename Inner::value_type>
Inner join_elem(const X& separator, const Container& xs)
{
    return join(Inner(1, separator), xs);
}

// API search type: is_elem_of_by : ((a -> Bool), [a]) -> Bool
// fwd bind count: 1
// Checks if at least one element of the container fulfils a predicate.
// is_elem_of_by((==), [1,2,3]) == true
template <typename UnaryPredicate, typename Container>
bool is_elem_of_by(UnaryPredicate pred, const Container& xs)
{
    return std::find_if(std::begin(xs), std::end(xs), pred) != std::end(xs);
}

// API search type: is_elem_of : (a, [a]) -> Bool
// fwd bind count: 1
// Checks if an element is a member of a container.
// is_elem_of(2, [1,2,3]) == true
// Also known as flip(contains).
template <typename Container>
bool is_elem_of(const typename Container::value_type& x, const Container& xs)
{
    return is_elem_of_by(is_equal_to(x), xs);
}

// API search type: nub_by : (((a, a) -> Bool), [a]) -> [a]
// fwd bind count: 1
// Makes the elements in a container unique with respect to a predicate
// nub_by((==), [1,2,2,3,2]) == [1,2,3]
// O(n^2)
template <typename Container, typename BinaryPredicate>
Container nub_by(BinaryPredicate p, const Container& xs)
{
    Container result;
    auto itOut = internal::get_back_inserter(result);
    for (const auto &x : xs)
    {
        auto eqToX = bind_1st_of_2(p, x);
        if (!is_elem_of_by(eqToX, result))
        {
            *itOut = x;
        }
    }
    return result;
}

// API search type: nub_on : ((a -> b), [a]) -> [a]
// fwd bind count: 1
// Makes the elements in a container unique
// with respect to their function value.
// nub_on((mod 10), [12,32,15]) == [12,15]
// O(n^2)
template <typename Container, typename F>
Container nub_on(F f, const Container& xs)
{
    return nub_by(is_equal_by(f), xs);
}

// API search type: nub : [a] -> [a]
// fwd bind count: 0
// Makes the elements in a container unique.
// nub([1,2,2,3,2]) == [1,2,3]
// O(n^2)
// Also known as distinct.
template <typename Container>
Container nub(const Container& xs)
{
    typedef typename Container::value_type T;
    return nub_by(std::equal_to<T>(), xs);
}

// API search type: all_unique_by_eq : (((a, a) -> Bool), [a]) -> Bool
// fwd bind count: 1
// Checks if all elements in a container are unique
// with respect to a predicate.
// Returns true for empty containers.
// O(n^2)
template <typename Container, typename BinaryPredicate>
bool all_unique_by_eq(BinaryPredicate p, const Container& xs)
{
    internal::check_binary_predicate_for_container<BinaryPredicate, Container>();
    return size_of_cont(nub_by(p, xs)) == size_of_cont(xs);
}

// API search type: all_unique_on : ((a -> b), [a]) -> Bool
// fwd bind count: 1
// Checks if all elements in a container are unique
// with respect to their function values.
// Returns true for empty containers.
// O(n^2)
template <typename Container, typename F>
bool all_unique_on(F f, const Container& xs)
{
    return all_unique_by_eq(is_equal_by(f), xs);
}

// API search type: all_unique : [a] -> Bool
// fwd bind count: 0
// Checks if all elements in a container are unique.
// Returns true for empty containers.
// O(n^2)
template <typename Container>
bool all_unique(const Container& xs)
{
    typedef typename Container::value_type T;
    auto comp = std::equal_to<T>();
    return all_unique_by_eq(comp, xs);
}

// API search type: is_strictly_sorted_by : (((a, a) -> Bool), [a]) -> Bool
// fwd bind count: 1
// Checks if a container already is strictly sorted using a predicate.
// comp(a, b) must return true only if a < b.
// O(n)
template <typename Container, typename Compare>
bool is_strictly_sorted_by(Compare comp, const Container& xs)
{
    internal::check_compare_for_container<Compare, Container>();
    if (size_of_cont(xs) < 2)
        return true;
    auto it1 = std::begin(xs);
    for (auto it2 = it1 + 1; it2 < std::end(xs); ++it1, ++it2)
        if (!internal::invoke(comp, *it1, *it2))
            return false;
    return true;
}

// API search type: is_strictly_sorted_on : ((a -> b), [a]) -> Bool
// fwd bind count: 1
// Checks if a container already is strictly sorted using a transformer.
// O(n)
template <typename Container, typename F>
bool is_strictly_sorted_on(F f, const Container& xs)
{
    return is_strictly_sorted_by(is_less_by(f), xs);
}

// API search type: is_strictly_sorted : [a] -> Bool
// fwd bind count: 0
// Checks if a container already is strictly sorted
// in ascending order using std::less.
// O(n)
template <typename Container>
bool is_strictly_sorted(const Container& xs)
{
    typedef typename Container::value_type T;
    auto comp = std::less<T>();
    return is_strictly_sorted_by(comp, xs);
}

// API search type: is_sorted_by : (((a, a) -> Bool), [a]) -> Bool
// fwd bind count: 1
// Checks if a container already is sorted using a predicate.
// comp(a, b) must return true only if a < b.
// O(n)
template <typename Container, typename Compare>
bool is_sorted_by(Compare comp, const Container& xs)
{
    internal::check_compare_for_container<Compare, Container>();
    if (size_of_cont(xs) < 2)
        return true;
    auto it1 = std::begin(xs);
    for (auto it2 = it1 + 1; it2 < std::end(xs); ++it1, ++it2)
        if (internal::invoke(comp, *it2, *it1))
            return false;
    return true;
}

// API search type: is_sorted_on : ((a -> b), [a]) -> Bool
// fwd bind count: 1
// Checks if a container already is strictly sorted using a transformer.
// O(n)
template <typename Container, typename F>
bool is_sorted_on(F f, const Container& xs)
{
    return is_sorted_by(is_less_by(f), xs);
}

// API search type: is_sorted : [a] -> Bool
// fwd bind count: 0
// Checks if a container already is sorted
// in ascending order using std::less.
// O(n)
template <typename Container>
bool is_sorted(const Container& xs)
{
    typedef typename Container::value_type T;
    auto comp = std::less<T>();
    return is_sorted_by(comp, xs);
}

// API search type: is_prefix_of : ([a], [a]) -> Bool
// fwd bind count: 1
// Checks if a containers starts with a token.
// is_prefix_of("Fun", "FunctionalPlus") == true
template <typename Container>
bool is_prefix_of(const Container& token, const Container& xs)
{
    if (size_of_cont(token) > size_of_cont(xs))
        return false;
    return get_segment(0, size_of_cont(token), xs) == token;
}

// API search type: is_suffix_of : ([a], [a]) -> Bool
// fwd bind count: 1
// Checks if a containers contains a token as a segment.
// is_suffix_of("us", "FunctionalPlus") == true
template <typename Container>
bool is_suffix_of(const Container& token, const Container& xs)
{
    if (size_of_cont(token) > size_of_cont(xs))
        return false;
    return get_segment(size_of_cont(xs) - size_of_cont(token),
        size_of_cont(xs), xs) == token;
}

// API search type: all_by : ((a -> Bool), [a]) -> Bool
// fwd bind count: 1
// Checks if a containers contains a token as a segment.
// all_by(is_even, [2, 4, 6]) == true
// Returns true for empty containers.
template <typename UnaryPredicate, typename Container>
bool all_by(UnaryPredicate p, const Container& xs)
{
    internal::check_unary_predicate_for_container<UnaryPredicate, Container>();
    return std::all_of(std::begin(xs), std::end(xs), p);
}

// API search type: all : [Bool] -> Bool
// fwd bind count: 0
// Checks if all values in a container evaluate to true.
// all([true, false, true]) == false
// Returns true for empty containers.
template <typename Container>
bool all(const Container& xs)
{
    typedef typename Container::value_type T;
    return all_by(identity<T>, xs);
}

// API search type: all_the_same_by : (((a, a) -> Bool), [a]) -> Bool
// fwd bind count: 1
// Checks if all values in a container are equal using a binary predicate.
// Returns true for empty containers.
template <typename Container, typename BinaryPredicate>
bool all_the_same_by(BinaryPredicate p, const Container& xs)
{
    internal::check_binary_predicate_for_container<BinaryPredicate, Container>();
    if (size_of_cont(xs) < 2)
        return true;
    auto unaryPredicate = bind_1st_of_2(p, xs.front());
    return all_by(unaryPredicate, xs);
}

// API search type: all_the_same_on : ((a -> b), [a]) -> Bool
// fwd bind count: 1
// Checks if all values in a container are equal using a transformer.
// Returns true for empty containers.
template <typename Container, typename F>
bool all_the_same_on(F f, const Container& xs)
{
    if (size_of_cont(xs) < 2)
        return true;
    auto unaryPredicate = is_equal_by_to(f, f(xs.front()));
    return all_by(unaryPredicate, xs);
}

// API search type: all_the_same : [a] -> Bool
// fwd bind count: 0
// Checks if all values in a container are equal.
// Returns true for empty containers.
template <typename Container>
bool all_the_same(const Container& xs)
{
    typedef typename Container::value_type T;
    auto binaryPredicate = std::equal_to<T>();
    return all_the_same_by(binaryPredicate, xs);
}

// API search type: numbers_step : (a, a, a) -> [a]
// fwd bind count: 2
// Return a sequence of numbers using a specific step.
// numbers_step(2, 9, 2) == [2, 4, 6, 8]
template <typename T,
        typename ContainerOut = std::vector<T>>
ContainerOut numbers_step
        (const T start, const T end, const T step)
{
    ContainerOut result;
    if ((step > 0 && start >= end) ||
        (step < 0 && start <= end) ||
        step == 0)
    {
        return result;
    }
    std::size_t size = static_cast<std::size_t>((end - start) / step);
    internal::prepare_container(result, size);
    auto it = internal::get_back_inserter<ContainerOut>(result);
    for (T x = start; x < end; x += step)
        *it = x;
    return result;
}

// API search type: numbers : (a, a) -> [a]
// fwd bind count: 1
// Return an ascending sequence of numbers..
// Also known as range.
// numbers(2, 9) == [2, 3, 4, 5, 6, 7, 8]
template <typename T,
        typename ContainerOut = std::vector<T>>
ContainerOut numbers(const T start, const T end)
{
    return numbers_step<T, ContainerOut>(start, end, 1);
}

// API search type: singleton_seq : a -> [a]
// fwd bind count: 0
// Construct a sequence containing a single value.
// singleton_seq(3) == [3].
template <typename T, typename ContainerOut = std::vector<T>>
ContainerOut singleton_seq(const T& x)
{
    return ContainerOut(1, x);
}

// API search type: all_idxs : [a] -> [Int]
// fwd bind count: 0
// Returns a vector containing all valid indices of sequence xs.
// all_idxs([6,4,7,6]) == [0,1,2,3]
template <typename Container>
std::vector<std::size_t> all_idxs(const Container& xs)
{
    return numbers<std::size_t>(0, size_of_cont(xs));
}

// API search type: init : [a] -> [a]
// fwd bind count: 0
// init([0,1,2,3]) == [0,1,2]
// Unsafe! xs must be non-empty.
template <typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut init(Container&& xs)
{
    assert(!is_empty(xs));
    return get_segment(0, size_of_cont(std::forward<Container>(xs)) - 1, xs);
}

// API search type: tail : [a] -> [a]
// fwd bind count: 0
// Drops the first element of a container, keeps the rest. Unsafe!
// tail([0,1,2,3]) == [1,2,3]
// Unsafe! xs must be non-empty.
template <typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut tail(Container&& xs)
{
    assert(!is_empty(xs));
    return get_segment(1, size_of_cont(std::forward<Container>(xs)), xs);
}

// API search type: head : [a] -> a
// fwd bind count: 0
// Return the first element of a container.
// head([0,1,2,3]) == 0
// Unsafe! xs must be non-empty.
template <typename Container>
typename Container::value_type head(const Container& xs)
{
    assert(!is_empty(xs));
    return xs.front();
}

// API search type: last : [a] -> a
// fwd bind count: 0
// Return the last element of a container.
// last([0,1,2,3]) == 3
// Unsafe! xs must be non-empty.
template <typename Container>
typename Container::value_type last(const Container& xs)
{
    assert(!is_empty(xs));
    return xs.back();
}

// API search type: mean_stddev : [a] -> (a, a)
// fwd bind count: 0
// Calculates the mean and the population standard deviation.
// mean_stddev([4, 8]) == (6, 2)
// mean_stddev([1, 3, 7, 4]) == (3.75, 2.5)
// xs must be non-empty.
template <typename Result, typename Container>
std::pair<Result, Result> mean_stddev(const Container& xs)
{
    assert(size_of_cont(xs) != 0);

    // http://stackoverflow.com/a/7616783/1866775
    Result sum = static_cast<Result>(
        internal::accumulate(xs.begin(), xs.end(),
            static_cast<typename Container::value_type>(0)));
    Result mean = sum / static_cast<Result>(xs.size());

    std::vector<Result> diff(xs.size());
    std::transform(xs.begin(), xs.end(), diff.begin(),
        [mean](Result x)
        {
            return x - mean;
        });
    Result sq_sum = std::inner_product(
        diff.begin(), diff.end(), diff.begin(), static_cast<Result>(0));
    Result stddev = std::sqrt(sq_sum / static_cast<Result>(xs.size()));
    return std::make_pair(mean, stddev);
}

// API search type: count_occurrences_by : ((a -> b), [a]) -> Map b Int
// fwd bind count: 1
// Returns a discrete frequency distribution of the elements in a container
// applying a specific transformer.
// count_occurrences_by(floor, [1.1, 2.3, 2.7, 3.6, 2.4]) == [(1, 1), (2, 3), (3, 1)]
// O(n)
template <typename F, typename ContainerIn>
auto count_occurrences_by(F f, const ContainerIn& xs)
{
    using In = typename ContainerIn::value_type;
    using MapOut =
        std::map<std::decay_t<internal::invoke_result_t<F, In>>, std::size_t>;

    internal::trigger_static_asserts<internal::unary_function_tag, F, typename ContainerIn::value_type>();
    MapOut result;
    for (const auto& x : xs)
    {
        ++result[internal::invoke(f, x)];
    }
    return result;
}

// API search type: count_occurrences : [a] -> Map a Int
// fwd bind count: 0
// Returns a discrete frequency distribution of the elements in a container
// applying a specific transformer.
// Can be used to create a histogram.
// count_occurrences([1,2,2,3,2]) == [(1, 1), (2, 3), (3, 1)]
// O(n)
template <typename ContainerIn,
        typename MapOut = typename std::map<
            typename ContainerIn::value_type, std::size_t>>
MapOut count_occurrences(const ContainerIn& xs)
{
    return count_occurrences_by(identity<typename ContainerIn::value_type>, xs);
}

// API search type: lexicographical_less_by : (((a, a) -> Bool), [a], [a]) -> Bool
// fwd bind count: 2
// Lexicographical less-than comparison using a specific predicate.
// lexicographical_less_by((<), [0,1,2,2,4,5], [0,1,2,3,4,5]) == true
// lexicographical_less_by((<), "012245", "012345") == true
// lexicographical_less_by((<), "01234", "012345") == true
// lexicographical_less_by((<), "012345", "01234") == false
// lexicographical_less_by((<), "012345", "012345") == false
template <typename Container, typename BinaryPredicate>
bool lexicographical_less_by(BinaryPredicate p,
        const Container& xs, const Container& ys)
{
    internal::check_binary_predicate_for_container<BinaryPredicate, Container>();
    auto itXs = std::begin(xs);
    auto itYs = std::begin(ys);
    while (itXs != std::end(xs) && itYs != std::end(ys))
    {
        if (internal::invoke(p, *itXs, *itYs))
        {
            return true;
        }
        if (internal::invoke(p, *itYs, *itXs))
        {
            return false;
        }
        ++itXs;
        ++itYs;
    }
    if (size_of_cont(xs) < size_of_cont(ys))
    {
        return true;
    }
    return false;
}

// API search type: lexicographical_less : ([a], [a]) -> Bool
// fwd bind count: 1
// Lexicographical less-than comparison.
// lexicographical_less([0,1,2,2,4,5], [0,1,2,3,4,5]) == true
// lexicographical_less("012245", "012345") == true
// lexicographical_less("01234", "012345") == true
// lexicographical_less("012345", "01234") == false
// lexicographical_less("012345", "012345") == false
template <typename Container>
bool lexicographical_less(const Container& xs, const Container& ys)
{
    return lexicographical_less_by(
        is_less<typename Container::value_type>, xs, ys);
}

// API search type: lexicographical_sort : [[a]] -> [[a]]
// fwd bind count: 0
// sort by lexicographical_less
template <typename Container,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut lexicographical_sort(Container&& xs)
{
    typedef typename std::remove_reference<Container>::type::value_type T;
    return sort_by(lexicographical_less<T>, std::forward<Container>(xs));
}

// API search type: replicate : (Int, a) -> [a]
// fwd bind count: 1
// Create a sequence containing x n times.
// replicate(3, 1) == [1, 1, 1]
template <typename T,
        typename ContainerOut = std::vector<T>>
ContainerOut replicate(std::size_t n, const T& x)
{
    return ContainerOut(n, x);
}

namespace internal
{

template <typename UnaryPredicate, typename T>
T instead_of_if(internal::reuse_container_t, UnaryPredicate pred,
    const T& alt, T&& x)
{
    if (internal::invoke(pred, x))
        return alt;
    else
        return std::forward<T>(x);
}

template <typename UnaryPredicate, typename T>
T instead_of_if(internal::create_new_container_t, UnaryPredicate pred,
    const T& alt, const T& x)
{
    if (internal::invoke(pred, x))
        return alt;
    else
        return x;
}

} // namespace internal

// API search type: instead_of_if : ((a -> Bool), a, a) -> a
// fwd bind count: 2
// Return alt if pred(x), otherwise x itself.
template <typename UnaryPredicate, typename T, typename TAlt>
auto instead_of_if(UnaryPredicate pred, const TAlt& alt, T&& x)
{
    return internal::instead_of_if(internal::can_reuse_v<T>{},
        pred, alt, std::forward<T>(x));
}

// API search type: instead_of_if_empty : ((a -> Bool), [a], [a]) -> [a]
// fwd bind count: 2
// Return alt if xs is empty, otherwise xs itself.
template <typename Container, typename ContainerAlt,
    typename ContainerOut = internal::remove_const_and_ref_t<Container>>
ContainerOut instead_of_if_empty(const ContainerAlt& alt, Container&& xs)
{
    return instead_of_if(
        is_empty<internal::remove_const_and_ref_t<Container>>,
        alt, std::forward<Container>(xs));
}

} // namespace fplus
