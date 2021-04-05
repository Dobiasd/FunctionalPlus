// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <doctest/doctest.h>
#include <fplus/fplus.hpp>
#include <vector>

namespace {

    typedef std::deque<int> IntDeq;
    typedef std::deque<IntDeq> IntContCont;
    typedef IntDeq IntCont;
    typedef IntCont Row;
}

std::string CcI2SFree(const std::string& str, int x)
{
    return str + std::to_string(x);
}

auto CcI2SLambda = [](const std::string& str, int x)
{ return CcI2SFree(str, x); };

std::function<std::string(const std::string&, int)>
CcI2SStdFunction = CcI2SLambda;

std::string (*CcI2SFunctionPointer)(const std::string&, int) =
&CcI2SFree;

struct CcI2SStrct {
    std::string operator() (const std::string& str, int x)
    { return CcI2SFree(str, x); }
    std::string nonCMemF (const std::string& str, int x)
    { return CcI2SFree(str, x); }
    std::string cnstMemF (const std::string& str, int x) const
    { return CcI2SFree(str, x); }
    static std::string sttcMemF (const std::string& str, int x)
    { return CcI2SFree(str, x); }
};

TEST_CASE("function_traits_test - static_asserts")
{
    using namespace fplus;
    static_assert(std::is_same<
        utils::function_traits<decltype(CcI2SFree)>::arg<0>::type,
        const std::string&>::value, "No.");
    static_assert(std::is_same<
        utils::function_traits<decltype(CcI2SFree)>::arg<1>::type,
        int>::value, "No.");
    static_assert(std::is_same<
        utils::function_traits<decltype(CcI2SFree)>::result_type,
        std::string>::value, "No.");

    static_assert(std::is_same<
        utils::function_traits<decltype(CcI2SLambda)>::arg<0>::type,
        const std::string&>::value, "No.");
    static_assert(std::is_same<
        utils::function_traits<decltype(CcI2SLambda)>::arg<1>::type,
        int>::value, "No.");
    static_assert(std::is_same<
        utils::function_traits<decltype(CcI2SLambda)>::result_type,
        std::string>::value, "No.");

    static_assert(std::is_same<
        utils::function_traits<decltype(CcI2SStdFunction)>::arg<0>::type,
        const std::string&>::value, "No.");
    static_assert(std::is_same<
        utils::function_traits<decltype(CcI2SStdFunction)>::arg<1>::type,
        int>::value, "No.");
    static_assert(std::is_same<
        utils::function_traits<decltype(CcI2SStdFunction)>::result_type,
        std::string>::value, "No.");

    static_assert(std::is_same<
        utils::function_traits<decltype(CcI2SFunctionPointer)>::arg<0>::type,
        const std::string&>::value, "No.");
    static_assert(std::is_same<
        utils::function_traits<decltype(CcI2SFunctionPointer)>::arg<1>::type,
        int>::value, "No.");
    static_assert(std::is_same<
        utils::function_traits<decltype(CcI2SFunctionPointer)>::result_type,
        std::string>::value, "No.");

    CcI2SStrct ccI2SStrct;
    ccI2SStrct("dummy call to avoid unused variable warnings", 0);
    static_assert(std::is_same<
        utils::function_traits<decltype(ccI2SStrct)>::arg<0>::type,
        const std::string&>::value, "No.");
    static_assert(std::is_same<
        utils::function_traits<decltype(ccI2SStrct)>::arg<1>::type,
        int>::value, "No.");
    static_assert(std::is_same<
        utils::function_traits<decltype(ccI2SStrct)>::result_type,
        std::string>::value, "No.");

    static_assert(std::is_same<
        utils::function_traits<decltype(&CcI2SStrct::nonCMemF)>::arg<0>::type,
        const std::string&>::value, "No.");
    static_assert(std::is_same<
        utils::function_traits<decltype(&CcI2SStrct::nonCMemF)>::arg<1>::type,
        int>::value, "No.");
    static_assert(std::is_same<
        utils::function_traits<decltype(&CcI2SStrct::nonCMemF)>::result_type,
        std::string>::value, "No.");

    static_assert(std::is_same<
        utils::function_traits<decltype(&CcI2SStrct::cnstMemF)>::arg<0>::type,
        const std::string&>::value, "No.");
    static_assert(std::is_same<
        utils::function_traits<decltype(&CcI2SStrct::cnstMemF)>::arg<1>::type,
        int>::value, "No.");
    static_assert(std::is_same<
        utils::function_traits<decltype(&CcI2SStrct::cnstMemF)>::result_type,
        std::string>::value, "No.");

    static_assert(std::is_same<
        utils::function_traits<decltype(&CcI2SStrct::sttcMemF)>::arg<0>::type,
        const std::string&>::value, "No.");
    static_assert(std::is_same<
        utils::function_traits<decltype(&CcI2SStrct::sttcMemF)>::arg<1>::type,
        int>::value, "No.");
    static_assert(std::is_same<
        utils::function_traits<decltype(&CcI2SStrct::sttcMemF)>::result_type,
        std::string>::value, "No.");
}
