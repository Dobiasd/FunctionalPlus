// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <fplus/internal/function_traits_asserts.hpp>

namespace fplus
{
namespace internal
{
struct apply_to_pair_tag
{
};

struct zip_with_tag
{
};

struct zip_with_3_tag
{
};

struct transform_fst_tag
{
};

struct transform_snd_tag
{
};

struct inner_product_with_tag
{
};

template <typename F, typename X, typename Y>
struct function_traits_asserts<apply_to_pair_tag, F, X, Y>
{
    static_assert(utils::function_traits<F>::arity == 2,
        "Function must take two parameters.");
    typedef typename utils::function_traits<F>::template arg<0>::type FIn0;
    typedef typename utils::function_traits<F>::template arg<1>::type FIn1;
    static_assert(std::is_convertible<X, FIn0>::value,
        "Function does not take pair.first type as first Parameter.");
    static_assert(std::is_convertible<Y, FIn1>::value,
        "Function does not take pair.second type as second Parameter.");
};

template <typename F, typename X, typename Y>
struct function_traits_asserts<zip_with_tag, F, X, Y>
{
    static_assert(utils::function_traits<F>::arity == 2,
        "Function must take two parameters.");
    typedef typename utils::function_traits<F>::template arg<0>::type FIn0;
    typedef typename utils::function_traits<F>::template arg<1>::type FIn1;
    static_assert(std::is_convertible<X, FIn0>::value,
        "Function does not take elements from first Container as first Parameter.");
    static_assert(std::is_convertible<Y, FIn1>::value,
        "Function does not take elements from second Container as second Parameter.");
};

template <typename F, typename X, typename Y, typename Z>
struct function_traits_asserts<zip_with_3_tag, F, X, Y, Z>
{
    static_assert(utils::function_traits<F>::arity == 3,
        "Function must take two parameters.");
    typedef typename utils::function_traits<F>::template arg<0>::type FIn0;
    typedef typename utils::function_traits<F>::template arg<1>::type FIn1;
    typedef typename utils::function_traits<F>::template arg<2>::type FIn2;
    static_assert(std::is_convertible<X, FIn0>::value,
        "Function does not take elements from first Container as first Parameter.");
    static_assert(std::is_convertible<Y, FIn1>::value,
        "Function does not take elements from second Container as second Parameter.");
    static_assert(std::is_convertible<Z, FIn2>::value,
        "Function does not take elements from third Container as third Parameter.");
};

template <typename F, typename X>
struct function_traits_asserts<transform_fst_tag, F, X>
{
    static_assert(utils::function_traits<F>::arity == 1,
        "Function must take one parameter.");
    typedef typename utils::function_traits<F>::template arg<0>::type FIn0;
    static_assert(std::is_convertible<X, FIn0>::value,
        "Function does not take pair.first type as first Parameter.");
};

template <typename F, typename X>
struct function_traits_asserts<transform_snd_tag, F, X>
{
    static_assert(utils::function_traits<F>::arity == 1,
        "Function must take one parameter.");
    typedef typename utils::function_traits<F>::template arg<0>::type FIn0;
    static_assert(std::is_convertible<X, FIn0>::value,
        "Function does not take pair.second type as first Parameter.");
};

template <typename F, typename X, typename Y>
struct function_traits_asserts<inner_product_with_tag, F, X, Y>
{
    static_assert(utils::function_traits<F>::arity == 2,
        "Function must take two parameters.");
    typedef typename utils::function_traits<F>::template arg<0>::type FIn0;
    typedef typename utils::function_traits<F>::template arg<1>::type FIn1;
    static_assert(std::is_convertible<X, FIn0>::value,
        "Function does not take elements from first Container as first Parameter.");
    static_assert(std::is_convertible<Y, FIn1>::value,
        "Function does not take elements from second Container as second Parameter.");
};
}
}
