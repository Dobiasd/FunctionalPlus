// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <fplus/fplus.hpp>

#if !defined(_WIN32) || !defined(WIN32) //TODO: google says there is no UTF8 locale on Windows, is it so?
TEST_CASE("stringtools_test, to_lower/upper_case, utf8")
{
    using namespace fplus;
    const std::locale loc_utf8("ru_RU.utf8");
    REQUIRE_EQ(to_lower_case(std::wstring(L"cYrIlLiC 123&? КиРиЛлИцА"), loc_utf8), std::wstring(L"cyrillic 123&? кириллица"));
    REQUIRE_EQ(to_lower_case(std::wstring(L"АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ"), loc_utf8), std::wstring(L"абвгдеёжзийклмнопрстуфхцчшщъыьэюя"));

    REQUIRE_EQ(to_upper_case(std::wstring(L"cYrIlLiC 123&? КиРиЛлИцА"), loc_utf8), std::wstring(L"CYRILLIC 123&? КИРИЛЛИЦА"));
    REQUIRE_EQ(to_upper_case(std::wstring(L"абвгдеёжзийклмнопрстуфхцчшщъыьэюя"), loc_utf8), std::wstring(L"АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ"));
}
#endif
