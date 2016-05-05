// Copyright Tobias Hermann 2016.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "fplus.h"
#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

const std::string key = "API search type: ";
const std::string comment_token = "// ";
const std::string code_dir = "../include/fplus/";

auto is_comment = fplus::bind_1st_of_2(
        fplus::is_prefix_of<std::string>, comment_token);

std::vector<std::string> list_files(const std::string& dir_path)
{
    std::vector<std::string> result;
    using namespace boost::filesystem;
    path p(dir_path);
    for (auto i = directory_iterator(p); i != directory_iterator(); i++)
    {
        if (!is_directory(i->path())) //we eliminate directories
        {
            result.push_back(i->path().filename().string());
        }
    }
    return result;
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
    assert(!lines.empty());

    auto trim_line = fplus::bind_1st_of_2(
            fplus::trim_token_left<std::string>, comment_token);
    auto trim_lines = fplus::bind_1st_of_2(fplus::transform<decltype(trim_line), std::vector<std::string>>, trim_line);

    auto search_type = trim_line(lines[0]);
    auto doc_and_decl = fplus::span(is_comment, fplus::tail(lines));
    doc_and_decl = fplus::transform_pair(trim_lines, trim_lines, doc_and_decl);
    return {search_type, doc_and_decl.first, doc_and_decl.second};
}

int main()
{
    // todo typedef, using namespaces
    using namespace std;
    typedef vector<string> strings;
    auto code_files = list_files(code_dir);
    code_files.resize(1); // asdasd
    auto begins_with_curly_open = [](const std::string& line)
    {
        return (!line.empty() && line.front() == '{');
    };
    auto no_curly_open = fplus::logical_not(begins_with_curly_open);
    for (const auto& code_file : code_files)
    {
        auto lines = fplus::read_text_file_lines(code_dir + code_file);
        auto is_search_type = fplus::bind_1st_of_2(
                fplus::is_infix_of<string>, key);
        auto functions_lines = fplus::split_by_keep_separators(
                is_search_type, lines);
        if (functions_lines.size() < 2)
            continue;
        functions_lines = fplus::tail(functions_lines);
        auto get_non_impl_lines = fplus::bind_1st_of_2(
                fplus::take_while<strings, decltype(no_curly_open)>,
                    no_curly_open);
        auto functions_docs = fplus::transform(
                get_non_impl_lines, functions_lines);
        auto functions = fplus::transform(
                lines_to_function_help, functions_docs);
        cout << fplus::show_cont_with_frame_and_newlines(
                "\n-----\n", "[", "]", functions, 0) << endl;
    }
}
