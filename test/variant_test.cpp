// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <doctest/doctest.h>
#include <fplus/fplus.hpp>

namespace {
    std::string print_output;

    bool print_int(int x)
    {
        print_output = "int " + std::to_string(x);
        return true;
    }

    bool print_double(double x)
    {
        print_output = "double " + std::to_string(fplus::round(x));
        return true;
    }

    bool print_string(const std::string& str)
    {
        print_output = "string " + str;
        return true;
    }

    std::string show_int(int x)
    {
        return fplus::show(x);
    }

    std::string show_string(const std::string& str)
    {
        return fplus::show(str);
    }
}

TEST_CASE("variant_test - visit_one")
{
    using namespace fplus;

    fplus::variant<int, double> int_or_double(3);

    int_or_double.visit_one(print_int);
    REQUIRE_EQ(print_output, "int 3");
    print_output.clear();

    int_or_double.visit_one(print_double);
    REQUIRE_EQ(print_output, "");
    print_output.clear();

    int_or_double = 23.0;

    int_or_double.visit_one(print_int);
    REQUIRE_EQ(print_output, "");
    print_output.clear();

    int_or_double.visit_one(print_double);
    REQUIRE_EQ(print_output, "double 23");
    print_output.clear();


    using float_or_double = fplus::variant<float, double>;
    using nested = fplus::variant<int, float_or_double>;
    nested int_or_float_double_variant(fplus::variant<float, double>(3.0));

    const auto print_nested_double = [&](const float_or_double& f_o_d) -> int
    {
        f_o_d.visit_one(print_double);
        return 0;
    };

    int_or_float_double_variant.visit_one(print_nested_double);
    REQUIRE_EQ(print_output, "double 3");
    print_output.clear();
}

TEST_CASE("variant_test - equality_test")
{
    using namespace fplus;
    fplus::variant<int, std::string> int_or_string_i(3);
    fplus::variant<int, std::string> int_or_string_s(std::string("hi"));
    REQUIRE(int_or_string_i == int_or_string_i);
    REQUIRE_FALSE(int_or_string_i == int_or_string_s);
}

TEST_CASE("variant_test - inequality_test")
{
    using namespace fplus;
    fplus::variant<int, std::string> int_or_string_i(3);
    fplus::variant<int, std::string> int_or_string_s(std::string("hi"));
    REQUIRE_FALSE(int_or_string_i != int_or_string_i);
    REQUIRE(int_or_string_i != int_or_string_s);
}

TEST_CASE("variant_test - visit")
{
    using namespace fplus;

    // should not compile
    //int_or_double.visit_one<std::string>(print_string);

    fplus::variant<int, std::string> int_or_string(3);

    REQUIRE(int_or_string.is<int>());
    REQUIRE_FALSE(int_or_string.is<std::string>());

    int_or_string.visit(print_int, print_string);
    REQUIRE_EQ(print_output, "int 3");
    print_output.clear();

    const auto transform_result =
        int_or_string.transform(show_int, show_string);
    transform_result.visit_one(print_string);
    REQUIRE_EQ(print_output, "string 3");
    print_output.clear();

    // should not compile
    //std::cout << int_or_string.visit(show_int, show_float) << std::endl;

    // should not compile
    //std::cout << int_or_string.visit(show_int, show_int) << std::endl;

    // should not compile
    //std::cout << int_or_string.visit(show_int, show_string, show_double) << std::endl;

    // should not compile
    //std::cout << int_or_string.visit(show_int) << std::endl;
}
