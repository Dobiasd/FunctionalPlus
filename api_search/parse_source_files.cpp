// Copyright Tobias Hermann 2015.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <fplus/fplus.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

typedef std::vector<std::string> string_vec;

const std::string api_search_type_key = "API search type: ";
const std::string comment_token = "// ";

const auto is_comment = fplus::bind_1st_of_2(
        fplus::is_prefix_of<std::string>, comment_token);

struct function_help
{
    std::string name;
    std::string signature;
    std::string documentation;
    std::string declaration;
};

std::string get_function_help_name(const function_help& f)
{
    return f.name;
}

std::string get_function_help_signature(const function_help& f)
{
    return f.signature;
}

std::string get_function_help_documentation(const function_help& f)
{
    return f.documentation;
}

std::string get_function_help_declaration(const function_help& f)
{
    return f.declaration;
}

std::ostream & operator<<(std::ostream &os, const function_help& f)
{
    return os << "Name: " << f.name << "\n" <<
        "Type: " << f.signature << "\n" <<
        "Doc: " << f.documentation << "\n" <<
        "Decl: " << f.declaration;
}

function_help lines_to_function_help(const string_vec& lines)
{
    const auto trim_line = fplus::bind_1st_of_2(
            fplus::trim_token_left<std::string>, comment_token);
    const auto trim_type_line = fplus::bind_1st_of_2(
            fplus::trim_token_left<std::string>, api_search_type_key);
    const auto trim_lines = fplus::bind_1st_of_2(
            fplus::transform<decltype(trim_line), const string_vec&>,
            trim_line);
    const auto search_type = trim_type_line(trim_line(lines[0]));
    const auto doc_and_decl = fplus::span(is_comment, fplus::tail(lines));
    const auto doc_and_decl_trimmed = fplus::transform_pair(
            trim_lines, trim_lines, doc_and_decl);
    auto name_and_type = fplus::split_by_token(std::string(" : "),
            false, search_type);
    assert(name_and_type.size() == 2);
    return {
        name_and_type[0],
        name_and_type[1],
        fplus::join(std::string("\n"), doc_and_decl_trimmed.first),
        fplus::join(std::string("\n"), doc_and_decl_trimmed.second)
    };
}

bool begins_with_curly_open(const std::string& line)
{
    return (!line.empty() && line.front() == '{');
}
const auto no_curly_open = fplus::logical_not(begins_with_curly_open);

std::vector<function_help> parse_code_file(const std::string& code_file)
{
    using namespace std;
    typedef vector<string> strings;
    const auto lines = fplus::read_text_file_lines(true, code_file)();
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
            !help.name.empty() &&
            !help.signature.empty() &&
            !help.declaration.empty() &&
            !help.documentation.empty();
    };
    return fplus::drop_if(help_is_ok, helps);
}

std::string functions_to_elm_code(const std::vector<function_help> functions)
{
    auto escape_backslashes = [](const std::string& str) -> std::string
    {
        return fplus::replace_tokens(
                std::string("\\"), std::string("\\\\"), str);
    };
    auto escape_quotation_marks = [](const std::string& str) -> std::string
    {
        return fplus::replace_tokens(
                std::string("\""), std::string("\\\""), str);
    };
    auto escape_newlines = [](const std::string& str)
    {
        return fplus::replace_tokens(
                std::string("\n"), std::string("\\n"), str);
    };
    auto escape_special_characters =
            fplus::compose(
                    escape_backslashes,
                    escape_quotation_marks,
                    escape_newlines);
    auto show_function = [&](const function_help& f) -> std::string
    {
        std::string str;
        str += std::string("{ name = \"") + f.name + "\"";
        str += std::string(", signature = \"") + f.signature + "\"";
        str += std::string(", documentation = \"") +
                escape_special_characters(f.documentation) + "\"";
        str += std::string(", declaration = \"") +
                escape_special_characters(f.declaration) + "\" }";
        return str;
    };

    auto function_strings = fplus::transform(show_function, functions);

    const auto chunks = fplus::split_every(64, function_strings);

    std::string result;
    result += "module Database exposing (..)\n\n\n";
    result += "type alias Function =\n";
    result += "    { name : String\n";
    result += "    , signature : String\n";
    result += "    , documentation : String\n";
    result += "    , declaration : String\n";
    result += "    }\n\n\n";
    result += "functions : List Function\n";
    result += "functions =\n";

    const auto show_chunk = [](const string_vec& strs) -> std::string
    {
        std::string res;
        res += "    [ ";
        res += fplus::join(std::string("\n    , "), strs);
        res += "\n    ]";
        return res;
    };

    result += fplus::join(
        std::string("\n    ++\n"),
        fplus::transform(show_chunk, chunks));
    return result;
}

void print_duplicates(const string_vec& strs)
{
    const auto occurences = fplus::count_occurrences(strs);
    typedef decltype(occurences)::value_type pair;
    const string_vec allowed_dups =
            {"and_then_maybe", "and_then_result", "compose", "show"};
    const auto dups = fplus::keep_if([&](const pair& p) -> bool
        {
            return p.second > 1 && !fplus::is_elem_of(p.first, allowed_dups);
        }, fplus::map_to_pairs(occurences));
    if (!dups.empty())
    {
        std::cerr << "Duplicates!\n";
        std::cerr << fplus::show_cont(dups) << std::endl;
    }
    std::cout << "---" << std::endl;
}

int main (int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Provide code file via command line." << std::endl;
        return 1;
    }

    const auto functions = fplus::sort_on(get_function_help_name,
            parse_code_file(std::string(argv[1])));
    const auto broken = get_broken_function_helps(functions);
    std::cout << "broken:" << std::endl;
    if (fplus::is_not_empty(broken))
    {
        std::cout << fplus::show_cont_with_frame_and_newlines(
            "\n-----\n", "[", "]",
            get_broken_function_helps(functions),
            0) << std::endl;
        return 1;
    }
    std::cout << "---" << std::endl;

    using fplus::transform;
    std::cout << "duplicate help names:" << std::endl;
    print_duplicates(transform(get_function_help_name, functions));
    //print_duplicates(transform(get_function_help_signature, functions));
    std::cout << "duplicate help documentations:" << std::endl;
    print_duplicates(transform(get_function_help_documentation, functions));
    std::cout << "duplicate help declarations:" << std::endl;
    print_duplicates(transform(get_function_help_declaration, functions));

    auto output = functions_to_elm_code(functions);
    std::string out_file = "frontend/src/Database.elm";
    if (fplus::write_text_file(out_file, output)())
    {
        std::cout << out_file << " written." << std::endl;
    }
    else
    {
        std::cerr << "Error: Unable to write " << out_file << std::endl;
    }
}
