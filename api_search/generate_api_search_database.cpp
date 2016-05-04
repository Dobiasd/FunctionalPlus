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

std::vector<std::string> read_text_file(const std::string& filename)
{
    std::ifstream input(filename);
    std::string line;
    std::vector<std::string> result;
    while (std::getline(input, line))
    {
        result.push_back(line);
    }
    return result;
}

int main()
{
    const std::string key = "API search type: ";
    const std::string code_dir = "../include/fplus/";
    auto code_files = list_files(code_dir);
    for (const auto& code_file : code_files)
    {
        auto lines = read_text_file(code_dir + code_file);
        for (const auto& line : lines)
        {
            if (fplus::is_infix_of(key, line))
            {
                std::cout << line << std::endl;
            }
        }
    }
}
