// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <vector>
#include <queue>

namespace fplus
{

template <typename T>
struct tree
{
    tree (const T& value, const std::vector<tree<T>>& children) :
        value_(value), children_(children) {}
    T value_;
    std::vector<tree<T>> children_;
};

namespace internal
{
template <typename T>
tree<T> make_singleton_tree(const T& x)
{
    return {x, {}};
}
} // namespace internal

namespace internal
{

template <typename BinaryPredicate, typename T>
std::vector<tree<T>> presort_trees(BinaryPredicate tree_is_child_of,
    std::vector<tree<T>> xs_orig)
{
    auto xs = fplus::convert_container<std::list<tree<T>>>(xs_orig);
    std::vector<tree<T>> result;
    while (!xs.empty())
    {
        for (auto it = std::begin(xs); it != std::end(xs);)
        {
            bool has_children = false;
            for (auto it_rest = std::begin(xs); it_rest != std::end(xs); ++it_rest)
            {
                if (it_rest != it && tree_is_child_of(*it_rest, *it))
                {
                    has_children = true;
                }
            }
            if (!has_children)
            {
                result.push_back(*it);
                it = xs.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
    return result;
}

template <typename BinaryPredicate, typename TreeCont> // todo: name?
TreeCont trees_from_sequence_helper(
    BinaryPredicate tree_is_child_of, TreeCont xs_unsorted)
{
    TreeCont result;
    auto xs = presort_trees(tree_is_child_of, xs_unsorted);
    for (auto it = std::begin(xs); it != std::end(xs); ++it)
    {
        const auto find_pred = bind_1st_of_2(tree_is_child_of, *it);
        auto it_find_begin = it;
        internal::advance_iterator(it_find_begin, 1);
        auto parent_it = std::find_if(it_find_begin, std::end(xs), find_pred);
        if (parent_it != std::end(xs))
        {
            parent_it->children_.push_back(*it);
        }
        else
        {
            result.push_back(*it);
        }
    }
    return result;
}

} // namespace internal

// API search type: trees_from_sequence : (((a, a) -> Bool), [a]) -> [Tree a]
// fwd bind count: 1
// Converts the sequence into a tree considering the given binary predicate.
template <typename BinaryPredicate, typename Container> // todo: name?
std::vector<tree<typename Container::value_type>> trees_from_sequence(
    BinaryPredicate is_child_of, const Container& xs)
{
    internal::check_binary_predicate_for_container<BinaryPredicate, Container>();
    typedef typename Container::value_type T;
    typedef tree<T> Tree;
    const auto singletons = transform_convert<std::vector<Tree>>(
        internal::make_singleton_tree<T>, xs);
    const auto tree_is_child_of =
        [is_child_of](const tree<T>& a, const tree<T>& b) -> bool
    {
        return is_child_of(a.value_, b.value_);
    };
    return internal::trees_from_sequence_helper(
        tree_is_child_of, std::move(singletons));
}

namespace internal
{

// -1 = a < b
//  0 = a == b
//  1 = b < a
template <typename T>
int tree_cmp(const tree<T>& a, const tree<T>& b)
{
    if(a.value_ < b.value_)
    {
        return -1;
    }
    else if(b.value_ < a.value_)
    {
        return 1;
    }
    else
    {
        const auto results = zip_with(tree_cmp<T>,
            sort_by(tree_cmp<T>, a.children_),
            sort_by(tree_cmp<T>, b.children_));
        return just_with_default(0, find_first_by(
                bind_1st_of_2(is_not_equal<int>, 0),
                results));
    }
}

template <typename T>
bool tree_less(const tree<T>& a, const tree<T>& b)
{
    return tree_cmp(a, b) < 0;
}

} // namespace internal

namespace internal
{

template <typename T>
bool are_normalized_trees_equal(const tree<T>& a, const tree<T>& b)
{
    if (a.value_ != b.value_ || a.children_.size() != b.children_.size())
    {
        return false;
    }
    else
    {
        return all(zip_with(are_normalized_trees_equal<T>,
            a.children_, b.children_));
    }
}

template <typename T>
tree<T> normalize_tree(tree<T> x)
{
    x.children_ = sort_by(
        internal::tree_less<T>,
        transform(normalize_tree<T>, x.children_));
    return x;
}

} // namespace internal

// API search type: are_trees_equal : (Tree a, Tree a) -> Bool
// fwd bind count: 1
template <typename T>
bool are_trees_equal(const tree<T>& a, const tree<T>& b)
{
    return internal::are_normalized_trees_equal(
        internal::normalize_tree(a), internal::normalize_tree(b));
}

// API search type: tree_size : Tree a -> Int
// fwd bind count: 0
// A tree with only one element (root) has size 1.
template <typename T>
std::size_t tree_size(const tree<T>& x)
{
    return 1 + sum(transform(tree_size<T>, x.children_));
}

// API search type: tree_depth : Tree a -> Int
// fwd bind count: 0
// A tree with only one element (root) has depth 1.
template <typename T>
std::size_t tree_depth(const tree<T>& x)
{
    return 1 + just_with_default<std::size_t>(0,
        maximum_maybe(transform(tree_depth<T>, x.children_)));
}

// API search type: flatten_tree_depth_first : Tree a -> [a]
// fwd bind count: 0
template <typename T>
std::vector<T> flatten_tree_depth_first(const tree<T>& x)
{
    return prepend_elem(x.value_,
        transform_and_concat(flatten_tree_depth_first<T>, x.children_));
}

// API search type: flatten_tree_breadth_first : Tree a -> [a]
// fwd bind count: 0
template <typename T>
std::vector<T> flatten_tree_breadth_first(const tree<T>& x)
{
    std::vector<T> result;
    result.reserve(tree_size(x));
    std::queue<const tree<T>*> q;
    q.push(&x);
    while (!q.empty())
    {
        const auto current = q.front();
        q.pop();
        result.push_back(current->value_);
        for (const auto& c : current->children_)
        {
            q.push(&c);
        }
    }
    return result;
}

} // namespace fplus
