#!/usr/bin/env bash

clang++-3.6 -O3 -std=c++11 -Wall -Wextra -pedantic -Wshadow -Werror -Weffc++ -Wconversion -Wsign-conversion -Wctor-dtor-privacy -Wreorder -Wold-style-cast -pthread -o ./temp_FunctionalPlus_api__clang -I./../include -lboost_filesystem -lboost_system parse_source_files.cpp

if [ -f ./temp_FunctionalPlus_api__clang ];
then
    ./temp_FunctionalPlus_api__clang
    rm ./temp_FunctionalPlus_api__clang
fi
