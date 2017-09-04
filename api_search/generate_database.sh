#!/usr/bin/env bash

clang++ -O3 -std=c++14 -Wall -Wextra -pedantic -Wshadow -Werror -Weffc++ -Wconversion -Wsign-conversion -Wctor-dtor-privacy -Wreorder -Wold-style-cast -Wparentheses -pthread -o ./temp_FunctionalPlus_api__clang -I../include parse_source_files.cpp

if [ -f ./temp_FunctionalPlus_api__clang ];
then
    g++ -E -CC -std=c++14 -w -I../include ../include/fplus/fplus.hpp > temp_preprocessor_output.txt
    ./temp_FunctionalPlus_api__clang temp_preprocessor_output.txt
    rm ./temp_FunctionalPlus_api__clang
    rm -f temp_preprocessor_output.txt
fi
