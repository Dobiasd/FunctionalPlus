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

TEST_CASE("stringtools_test, to_lower/upper_case, cp1253")
{
    using namespace fplus;
    const std::locale loc = get_locale(
#if defined(WIN32)
        "el-GR"
#elif defined(__APPLE__)
        "el_GR.ISO8859-7"
#else
        "el_GR.cp1253"
#endif
    );
    auto lower = fwd::to_lower_case_loc(loc);
    auto upper = fwd::to_upper_case_loc(loc);
    REQUIRE_EQ(lower(std::string("GrEeCe 123&? ÅëËáÄá")), std::string("greece 123&? åëëáäá"));
    REQUIRE_EQ(lower(std::string("ÁÂÃÄÅÆÇÈÉÊËÌÍÎÏĞÑÓÔÕÖ×ØÙ")), std::string("áâãäåæçèéêëìíîïğñóôõö÷øù"));

    REQUIRE_EQ(upper(std::string("GrEeCe 123&? ÅëËáÄá")), std::string("GREECE 123&? ÅËËÁÄÁ"));
    REQUIRE_EQ(upper(std::string("áâãäåæçèéêëìíîïğñóôõö÷øù")), std::string("ÁÂÃÄÅÆÇÈÉÊËÌÍÎÏĞÑÓÔÕÖ×ØÙ"));
}
