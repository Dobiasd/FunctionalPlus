// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <fplus/fplus.hpp>


TEST_CASE("stringtools_test, to_lower/upper_case, cp1251")
{
    using namespace fplus;
    const std::locale loc_cp1251 {
#ifdef WIN32
        "ru-RU"
#else
        "ru_RU.cp1251"
#endif
    };
    REQUIRE_EQ(to_lower_case(std::string("cYrIlLiC 123&? ÊèĞèËëÈöÀ"), loc_cp1251), std::string("cyrillic 123&? êèğèëëèöà"));
    REQUIRE_EQ(to_lower_case(std::string("ÀÁÂÃÄÅ¨ÆÇÈÉÊËÌÍÎÏĞÑÒÓÔÕÖ×ØÙÚÛÜİŞß"), loc_cp1251), std::string("àáâãäå¸æçèéêëìíîïğñòóôõö÷øùúûüışÿ"));

    REQUIRE_EQ(to_upper_case(std::string("cYrIlLiC 123&? ÊèĞèËëÈöÀ"), loc_cp1251), std::string("CYRILLIC 123&? ÊÈĞÈËËÈÖÀ"));
    REQUIRE_EQ(to_upper_case(std::string("àáâãäå¸æçèéêëìíîïğñòóôõö÷øùúûüışÿ"), loc_cp1251), std::string("ÀÁÂÃÄÅ¨ÆÇÈÉÊËÌÍÎÏĞÑÒÓÔÕÖ×ØÙÚÛÜİŞß"));
}