// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <doctest/doctest.h>
#include <fplus/fplus.hpp>
#include <vector>

TEST_CASE("stringtools_test -  trim")
{
    using namespace fplus;
    std::string untrimmed = "  \n \t   foo  ";
    REQUIRE_EQ(trim_whitespace_left(untrimmed), "foo  ");
    REQUIRE_EQ(trim_whitespace_right(untrimmed), "  \n \t   foo");
    REQUIRE_EQ(trim_whitespace(untrimmed), "foo");
}

TEST_CASE("stringtools_test -  split")
{
    using namespace fplus;
    std::string text = "Hi,\nI am a\r\n***strange***\n\rstring.";
    std::vector<std::string> textAsLinesWithEmty = {
        std::string("Hi,"),
        std::string("I am a"),
        std::string("***strange***"),
        std::string(""),
        std::string("string.") };
    std::vector<std::string> textAsLinesWithoutEmpty = {
        std::string("Hi,"),
        std::string("I am a"),
        std::string("***strange***"),
        std::string("string.") };
    std::vector<std::string> textAsWords = {
        std::string("Hi"),
        std::string("I"),
        std::string("am"),
        std::string("a"),
        std::string("strange"),
        std::string("string") };
    std::vector<std::string> textSplitBySpaceOnly = {
        std::string("Hi,\nI"),
        std::string("am"),
        std::string("a\r\n***strange***\n\rstring.") };
    std::vector<std::string> textSplitBySpaceAndCommaAndLine = {
        std::string("Hi"),
        std::string("I"),
        std::string("am"),
        std::string("a"),
        std::string("***strange***"),
        std::string("string.") };

    REQUIRE_EQ(split_lines(true, text), textAsLinesWithEmty);
    REQUIRE_EQ(split_lines(false, text), textAsLinesWithoutEmpty);
    REQUIRE_EQ(split_words(false, text), textAsWords);
    REQUIRE_EQ(split(' ', false, text), textSplitBySpaceOnly);
    REQUIRE_EQ(split_one_of(std::string{ " ,\r\n" }, false, text), textSplitBySpaceAndCommaAndLine);
}

TEST_CASE("stringtools_test -  to_string_filled")
{
    using namespace fplus;
    REQUIRE_EQ(to_string_fill_left('0', 5, 42), std::string("00042") );
    REQUIRE_EQ(to_string_fill_right(' ', 5, 42), std::string("42   ") );
}

TEST_CASE("stringtools_test -  to_lower_case")
{
    using namespace fplus;
    REQUIRE_EQ(to_lower_case(std::string("ChaRacTer&WorDs23")), std::string("character&words23"));
}

TEST_CASE("stringtools_test -  to_upper_case")
{
    using namespace fplus;
    REQUIRE_EQ(to_upper_case(std::string("ChaRacTer&WorDs34")), std::string("CHARACTER&WORDS34"));
}
