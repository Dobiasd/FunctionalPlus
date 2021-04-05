// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <doctest/doctest.h>
#include <fplus/fplus.hpp>

namespace {
    typedef std::vector<int> IntVector;
    IntVector xs = {1,2,2,3,2};
    std::string xsShown("[1, 2, 2, 3, 2]");

    typedef std::vector<std::string> StringVector;
    StringVector stringVec = {"foo", "bar"};
    std::string stringVecShown("[foo, bar]");

    typedef std::set<int> IntSet;
    IntSet xsSet = {1,2,3};
    std::string xsSetShown("[1, 2, 3]");

    typedef std::set<std::string> StringSet;
    StringSet stringSet = {"foo", "bar"};
    std::string stringSetShown("[bar, foo]");
}

TEST_CASE("show_test - show")
{
    using namespace fplus;
    std::map<int, std::string> mapToShow = {{1, "one"}, {2, "two"}};
    REQUIRE_EQ(show_cont(mapToShow), "[(1, one), (2, two)]");

    REQUIRE_EQ(show_cont(xs), xsShown);
    REQUIRE_EQ(show(xs), xsShown);

    REQUIRE_EQ(show_cont(xsSet), xsSetShown);
    REQUIRE_EQ(show(xsSet), xsSetShown);

    REQUIRE_EQ(show_cont(stringVec), stringVecShown);
    REQUIRE_EQ(show(stringVec), stringVecShown);

    REQUIRE_EQ(show_cont(stringSet), stringSetShown);
    REQUIRE_EQ(show(stringSet), stringSetShown);

    REQUIRE_EQ(show_cont_with(", ", xs), xsShown);
    std::string xsShownNLs = "(1,2,\n"
                             " 2,3,\n"
                             " 2)";
    REQUIRE_EQ(show_cont_with_frame_and_newlines(",", "(", ")", xs, 2), xsShownNLs);
    REQUIRE_EQ(show<int>(1), "1");

    REQUIRE_EQ(show(std::vector<std::vector<int>>({{1,2,3},{4,5,6}})), "[[1, 2, 3], [4, 5, 6]]");
}

TEST_CASE("show_test - show_float")
{
    using namespace fplus;
    const double pi = 3.14159;
    REQUIRE_EQ(show_float<double>(0, 3, pi), "3.142");
    REQUIRE_EQ(show_float<double>(1, 3, pi), "3.142");
    REQUIRE_EQ(show_float<double>(2, 3, pi), "03.142");
    REQUIRE_EQ(show_float<double>(3, 3, pi), "003.142");
    REQUIRE_EQ(show_float<double>(1, 2, pi), "3.14");
    REQUIRE_EQ(show_float<double>(1, 4, pi), "3.1416");
    REQUIRE_EQ(show_float<double>(1, 7, pi), "3.1415900");
    REQUIRE_EQ(show_float<double>(0, 3, -pi), "-3.142");
    REQUIRE_EQ(show_float<double>(1, 3, -pi), "-3.142");
    REQUIRE_EQ(show_float<double>(2, 3, -pi), "-3.142");
    REQUIRE_EQ(show_float<double>(3, 3, -pi), "-03.142");
    REQUIRE_EQ(show_float<double>(4, 3, -pi), "-003.142");
    REQUIRE_EQ(show_float<double>(0, 3, 0.142), "0.142");
    REQUIRE_EQ(show_float<double>(1, 3, 0.142), "0.142");
    REQUIRE_EQ(show_float<double>(2, 3, 0.142), "00.142");
    REQUIRE_EQ(fill_left(' ', 8, show_float<double>(0, 3, -pi)), "  -3.142");

    REQUIRE_EQ(show_float_fill_left<double>(' ', 8, 3, pi), "   3.142");
    REQUIRE_EQ(show_float_fill_left<double>(' ', 8, 6, pi), "3.141590");
    REQUIRE_EQ(show_float_fill_left<double>(' ', 8, 3, -pi), "  -3.142");
    REQUIRE_EQ(show_float_fill_left<double>(' ', 2, 3, -pi), "-3.142");

    REQUIRE_EQ(show_fill_left<int>(' ', 4, 3), "   3");
    REQUIRE_EQ(show_fill_left<int>('0', 4, 3), "0003");
    REQUIRE_EQ(show_fill_left<int>(' ', 4, 12345), "12345");

    REQUIRE_EQ(show_fill_right<int>(' ', 4, 3), "3   ");
    REQUIRE_EQ(show_fill_right<int>(' ', 4, 12345), "12345");
}
