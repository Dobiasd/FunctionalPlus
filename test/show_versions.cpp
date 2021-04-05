// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <doctest/doctest.h>
#include <fplus/fplus.hpp>

TEST_CASE("show_versions - print_defines")
{
#ifdef __cplusplus
    std::cout << "__cplusplus: " << __cplusplus << std::endl;
#endif

#ifdef _LIBCPP_VERSION
    std::cout << "_LIBCPP_VERSION: " << _LIBCPP_VERSION << std::endl;
#endif

#ifdef _LIBCPP_ABI_VERSION
    std::cout << "_LIBCPP_ABI_VERSION: " << _LIBCPP_ABI_VERSION << std::endl;
#endif

#ifdef _MSC_VER
    std::cout << "_MSC_VER: " << _MSC_VER << std::endl;
#endif

#ifdef __GLIBCPP__
    std::cout << "__GLIBCPP__: " << __GLIBCPP__ << std::endl;
#endif

#ifdef __GLIBCXX__
    std::cout << "__GLIBCXX__: " << __GLIBCXX__ << std::endl;
#endif
}