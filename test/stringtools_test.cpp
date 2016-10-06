// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "fplus/fplus.hpp"
#include <vector>

using namespace testing;

TEST(stringtools_test, trim)
{
    using namespace fplus;
    std::string untrimmed = "  \n \t   foo  ";
    EXPECT_EQ(trim_whitespace_left(untrimmed), "foo  ");
    EXPECT_EQ(trim_whitespace_right(untrimmed), "  \n \t   foo");
    EXPECT_EQ(trim_whitespace(untrimmed), "foo");
}

TEST(stringtools_test, split)
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

    EXPECT_EQ(split_lines(text, true), textAsLinesWithEmty);
    EXPECT_EQ(split_lines(text, false), textAsLinesWithoutEmpty);
    EXPECT_EQ(split_words(text, false), textAsWords);
    EXPECT_EQ(split_words_by(text, ' ', false), textSplitBySpaceOnly);
    EXPECT_EQ(split_words_by_many(text, std::string{ " ,\r\n" }, false), textSplitBySpaceAndCommaAndLine);
}

TEST(stringtools_test, to_string_filled)
{
    using namespace fplus;
    EXPECT_EQ(to_string_fill_left('0', 5, 42), std::string("00042") );
    EXPECT_EQ(to_string_fill_right(' ', 5, 42), std::string("42   ") );
}

TEST(stringtools_test, to_lower_case)
{
    using namespace fplus;
    EXPECT_EQ(to_lower_case(std::string("ChaRacTer&WorDs23")), std::string("character&words23"));
}

TEST(stringtools_test, to_upper_case)
{
    using namespace fplus;
    EXPECT_EQ(to_upper_case(std::string("ChaRacTer&WorDs34")), std::string("CHARACTER&WORDS34"));
}
