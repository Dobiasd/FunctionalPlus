// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include <fplus/fplus.hpp>
#include <fplus/fwd.hpp>
#include "get_locale.hpp"

TEST_CASE("stringtools_test, to_lower/upper_case, cp1251")
{
    using namespace fplus;
    const std::locale loc = get_locale(
#ifdef WIN32
        "ru-RU"
#else
        "ru_RU.cp1251"
#endif
    );
    auto lower = fwd::to_lower_case_loc(loc);
    auto upper = fwd::to_upper_case_loc(loc);
    REQUIRE_EQ(lower(std::string("cYrIlLiC 123&? ���������")), std::string("cyrillic 123&? ���������"));
    REQUIRE_EQ(lower(std::string("�����Ũ��������������������������")), std::string("��������������������������������"));

    REQUIRE_EQ(upper(std::string("cYrIlLiC 123&? ���������")), std::string("CYRILLIC 123&? ���������"));
    REQUIRE_EQ(upper(std::string("��������������������������������")), std::string("�����Ũ��������������������������"));
}
