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

std::string CcI2SFreeNoexcept(const std::string& str, int x) noexcept
{
    return str + std::to_string(x);
}

auto CcI2SLambda = [](const std::string& str, int x) { return CcI2SFree(str, x); };

auto CcI2SLambdaNoexcept = [](const std::string& str, int x) noexcept { return CcI2SFree(str, x); };

std::function<std::string(const std::string&, int)>
    CcI2SStdFunction = CcI2SLambda;

std::function<std::string(const std::string&, int)>
    CcI2SStdFunctionNoexcept = CcI2SLambdaNoexcept;

std::string (*CcI2SFunctionPointer)(const std::string&, int) = &CcI2SFree;

std::string (*CcI2SFunctionPointerNoexcept)(const std::string&, int) noexcept = &CcI2SFreeNoexcept;

struct CcI2SStrct {
    std::string operator()(const std::string& str, int x)
    {
        return CcI2SFree(str, x);
    }
    std::string nonCMemF(const std::string& str, int x)
    {
        return CcI2SFree(str, x);
    }
    std::string cnstMemF(const std::string& str, int x) const
    {
        return CcI2SFree(str, x);
    }
    static std::string sttcMemF(const std::string& str, int x)
    {
        return CcI2SFree(str, x);
    }
};

struct CcI2SStrctNoexcept {
    std::string operator()(const std::string& str, int x) noexcept
    {
        return CcI2SFreeNoexcept(str, x);
    }
    std::string nonCMemF(const std::string& str, int x) noexcept
    {
        return CcI2SFreeNoexcept(str, x);
    }
    std::string cnstMemF(const std::string& str, int x) const noexcept
    {
        return CcI2SFreeNoexcept(str, x);
    }
    static std::string sttcMemF(const std::string& str, int x) noexcept
    {
        return CcI2SFreeNoexcept(str, x);
    }
};

TEST_CASE("function_traits_test - static_asserts")
{
    using namespace fplus;
    static_assert(std::is_same<
                      utils::function_traits<decltype(CcI2SFree)>::arg<0>::type,
                      const std::string&>::value,
        "No.");
    static_assert(std::is_same<
                      utils::function_traits<decltype(CcI2SFree)>::arg<1>::type,
                      int>::value,
        "No.");
    static_assert(std::is_same<
                      utils::function_traits<decltype(CcI2SFree)>::result_type,
                      std::string>::value,
        "No.");

    static_assert(std::is_same<
                      utils::function_traits<decltype(CcI2SLambda)>::arg<0>::type,
                      const std::string&>::value,
        "No.");
    static_assert(std::is_same<
                      utils::function_traits<decltype(CcI2SLambda)>::arg<1>::type,
                      int>::value,
        "No.");
    static_assert(std::is_same<
                      utils::function_traits<decltype(CcI2SLambda)>::result_type,
                      std::string>::value,
        "No.");

    static_assert(std::is_same<
                      utils::function_traits<decltype(CcI2SStdFunction)>::arg<0>::type,
                      const std::string&>::value,
        "No.");
    static_assert(std::is_same<
                      utils::function_traits<decltype(CcI2SStdFunction)>::arg<1>::type,
                      int>::value,
        "No.");
    static_assert(std::is_same<
                      utils::function_traits<decltype(CcI2SStdFunction)>::result_type,
                      std::string>::value,
        "No.");

    static_assert(std::is_same<
                      utils::function_traits<decltype(CcI2SFunctionPointer)>::arg<0>::type,
                      const std::string&>::value,
        "No.");
    static_assert(std::is_same<
                      utils::function_traits<decltype(CcI2SFunctionPointer)>::arg<1>::type,
                      int>::value,
        "No.");
    static_assert(std::is_same<
                      utils::function_traits<decltype(CcI2SFunctionPointer)>::result_type,
                      std::string>::value,
        "No.");

    static_assert(std::is_same<
                      utils::function_traits<decltype(CcI2SFreeNoexcept)>::arg<0>::type,
                      const std::string&>::value,
        "No.");
    static_assert(std::is_same<
                      utils::function_traits<decltype(CcI2SFreeNoexcept)>::arg<1>::type,
                      int>::value,
        "No.");
    static_assert(std::is_same<
                      utils::function_traits<decltype(CcI2SFreeNoexcept)>::result_type,
                      std::string>::value,
        "No.");

    static_assert(std::is_same<
                      utils::function_traits<decltype(CcI2SLambdaNoexcept)>::arg<0>::type,
                      const std::string&>::value,
        "No.");
    static_assert(std::is_same<
                      utils::function_traits<decltype(CcI2SLambdaNoexcept)>::arg<1>::type,
                      int>::value,
        "No.");
    static_assert(std::is_same<
                      utils::function_traits<decltype(CcI2SLambdaNoexcept)>::result_type,
                      std::string>::value,
        "No.");

    static_assert(std::is_same<
                      utils::function_traits<decltype(CcI2SStdFunctionNoexcept)>::arg<0>::type,
                      const std::string&>::value,
        "No.");
    static_assert(std::is_same<
                      utils::function_traits<decltype(CcI2SStdFunctionNoexcept)>::arg<1>::type,
                      int>::value,
        "No.");
    static_assert(std::is_same<
                      utils::function_traits<decltype(CcI2SStdFunctionNoexcept)>::result_type,
                      std::string>::value,
        "No.");

    static_assert(std::is_same<
                      utils::function_traits<decltype(CcI2SFunctionPointerNoexcept)>::arg<0>::type,
                      const std::string&>::value,
        "No.");
    static_assert(std::is_same<
                      utils::function_traits<decltype(CcI2SFunctionPointerNoexcept)>::arg<1>::type,
                      int>::value,
        "No.");
    static_assert(std::is_same<
                      utils::function_traits<decltype(CcI2SFunctionPointerNoexcept)>::result_type,
                      std::string>::value,
        "No.");

    CcI2SStrct ccI2SStrct;
    ccI2SStrct("dummy call to avoid unused variable warnings", 0);
    static_assert(std::is_same<
                      utils::function_traits<decltype(ccI2SStrct)>::arg<0>::type,
                      const std::string&>::value,
        "No.");
    static_assert(std::is_same<
                      utils::function_traits<decltype(ccI2SStrct)>::arg<1>::type,
                      int>::value,
        "No.");
    static_assert(std::is_same<
                      utils::function_traits<decltype(ccI2SStrct)>::result_type,
                      std::string>::value,
        "No.");

    static_assert(std::is_same<
                      utils::function_traits<decltype(&CcI2SStrct::nonCMemF)>::arg<0>::type,
                      const std::string&>::value,
        "No.");
    static_assert(std::is_same<
                      utils::function_traits<decltype(&CcI2SStrct::nonCMemF)>::arg<1>::type,
                      int>::value,
        "No.");
    static_assert(std::is_same<
                      utils::function_traits<decltype(&CcI2SStrct::nonCMemF)>::result_type,
                      std::string>::value,
        "No.");

    static_assert(std::is_same<
                      utils::function_traits<decltype(&CcI2SStrct::cnstMemF)>::arg<0>::type,
                      const std::string&>::value,
        "No.");
    static_assert(std::is_same<
                      utils::function_traits<decltype(&CcI2SStrct::cnstMemF)>::arg<1>::type,
                      int>::value,
        "No.");
    static_assert(std::is_same<
                      utils::function_traits<decltype(&CcI2SStrct::cnstMemF)>::result_type,
                      std::string>::value,
        "No.");

    static_assert(std::is_same<
                      utils::function_traits<decltype(&CcI2SStrct::sttcMemF)>::arg<0>::type,
                      const std::string&>::value,
        "No.");
    static_assert(std::is_same<
                      utils::function_traits<decltype(&CcI2SStrct::sttcMemF)>::arg<1>::type,
                      int>::value,
        "No.");
    static_assert(std::is_same<
                      utils::function_traits<decltype(&CcI2SStrct::sttcMemF)>::result_type,
                      std::string>::value,
        "No.");

    CcI2SStrctNoexcept ccI2SStrctNoexcept;
    ccI2SStrctNoexcept("dummy call to avoid unused variable warnings", 0);
    static_assert(std::is_same<
                      utils::function_traits<decltype(ccI2SStrctNoexcept)>::arg<0>::type,
                      const std::string&>::value,
        "No.");
    static_assert(std::is_same<
                      utils::function_traits<decltype(ccI2SStrctNoexcept)>::arg<1>::type,
                      int>::value,
        "No.");
    static_assert(std::is_same<
                      utils::function_traits<decltype(ccI2SStrctNoexcept)>::result_type,
                      std::string>::value,
        "No.");

    static_assert(std::is_same<
                      utils::function_traits<decltype(&CcI2SStrctNoexcept::nonCMemF)>::arg<0>::type,
                      const std::string&>::value,
        "No.");
    static_assert(std::is_same<
                      utils::function_traits<decltype(&CcI2SStrctNoexcept::nonCMemF)>::arg<1>::type,
                      int>::value,
        "No.");
    static_assert(std::is_same<
                      utils::function_traits<decltype(&CcI2SStrctNoexcept::nonCMemF)>::result_type,
                      std::string>::value,
        "No.");

    static_assert(std::is_same<
                      utils::function_traits<decltype(&CcI2SStrctNoexcept::cnstMemF)>::arg<0>::type,
                      const std::string&>::value,
        "No.");
    static_assert(std::is_same<
                      utils::function_traits<decltype(&CcI2SStrctNoexcept::cnstMemF)>::arg<1>::type,
                      int>::value,
        "No.");
    static_assert(std::is_same<
                      utils::function_traits<decltype(&CcI2SStrctNoexcept::cnstMemF)>::result_type,
                      std::string>::value,
        "No.");

    static_assert(std::is_same<
                      utils::function_traits<decltype(&CcI2SStrctNoexcept::sttcMemF)>::arg<0>::type,
                      const std::string&>::value,
        "No.");
    static_assert(std::is_same<
                      utils::function_traits<decltype(&CcI2SStrctNoexcept::sttcMemF)>::arg<1>::type,
                      int>::value,
        "No.");
    static_assert(std::is_same<
                      utils::function_traits<decltype(&CcI2SStrctNoexcept::sttcMemF)>::result_type,
                      std::string>::value,
        "No.");
}
