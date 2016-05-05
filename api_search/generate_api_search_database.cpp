// Copyright Tobias Hermann 2016.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "fplus.h"
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

const std::string api_search_type_key = "API search type: ";
const std::string comment_token = "// ";
const std::string code_dir = "../include/fplus/";

const auto is_comment = fplus::bind_1st_of_2(
        fplus::is_prefix_of<std::string>, comment_token);

std::vector<std::string> list_files(const std::string& dir_path)
{
    using namespace boost::filesystem;
    return fplus::transform([](const directory_entry& entry)
    {
        return entry.path().filename().string();
    }, std::vector<directory_entry>(directory_iterator(path(dir_path)), {}));
}

struct function_help
{
    std::string search_type;
    std::vector<std::string> documentation;
    std::vector<std::string> declaration;
};

std::ostream & operator<<(std::ostream &os, const function_help& f)
{
    return os << "Type: " << f.search_type << "\n" <<
        "Doc: " << fplus::show_cont_with_frame_and_newlines(
                "\n", "", "", f.documentation, 0) << "\n" <<
        "Decl: " << fplus::show_cont_with_frame_and_newlines(
                "\n", "", "", f.declaration, 0);
}

function_help lines_to_function_help(const std::vector<std::string>& lines)
{
    const auto trim_line = fplus::bind_1st_of_2(
            fplus::trim_token_left<std::string>, comment_token);
    const auto trim_type_line = fplus::bind_1st_of_2(
            fplus::trim_token_left<std::string>, api_search_type_key);
    const auto trim_lines = fplus::bind_1st_of_2(
            fplus::transform<decltype(trim_line), std::vector<std::string>>,
            trim_line);
    const auto search_type = trim_type_line(trim_line(lines[0]));
    const auto doc_and_decl = fplus::span(is_comment, fplus::tail(lines));
    const auto doc_and_decl_trimmed = fplus::transform_pair(
            trim_lines, trim_lines, doc_and_decl);
    return {search_type,
            doc_and_decl_trimmed.first, doc_and_decl_trimmed.second};
}

bool begins_with_curly_open(const std::string& line)
{
    return (!line.empty() && line.front() == '{');
};
const auto no_curly_open = fplus::logical_not(begins_with_curly_open);

std::vector<function_help> parse_code_file(const std::string& code_file)
{
    using namespace std;
    typedef vector<string> strings;
    const auto lines = fplus::read_text_file_lines(code_dir + code_file)();
    const auto is_search_type = fplus::bind_1st_of_2(
            fplus::is_infix_of<string>, api_search_type_key);
    const auto functions_lines = fplus::split_by_keep_separators(
            is_search_type, lines);
    if (functions_lines.size() < 2)
        return {};
    const auto get_non_impl_lines = fplus::bind_1st_of_2(
            fplus::take_while<strings, decltype(no_curly_open)>,
                no_curly_open);
    const auto functions_docs = fplus::transform(
            get_non_impl_lines, fplus::tail(functions_lines));
    return fplus::transform(lines_to_function_help, functions_docs);
}

std::vector<function_help> get_broken_function_helps(
        const std::vector<function_help>& helps)
{
    auto help_is_ok = [](const function_help& help)
    {
        return
            !help.search_type.empty() &&
            !help.declaration.empty() &&
            !help.documentation.empty();
    };
    return fplus::drop_if(help_is_ok, helps);
}

int main()
{
    const auto code_files = list_files(code_dir);
    const auto functions = fplus::transform_and_concat(
            parse_code_file, code_files);
    const auto broken = get_broken_function_helps(functions);
    if (fplus::is_not_empty(broken))
    {
        std::cout << fplus::show_cont_with_frame_and_newlines(
            "\n-----\n", "[", "]",
            get_broken_function_helps(functions),
            0) << std::endl;
        return 1;
    }
    else
    {
        std::cout << "All OK." << std::endl;
    }
}
