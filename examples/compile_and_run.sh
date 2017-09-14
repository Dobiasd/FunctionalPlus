#!/usr/bin/env bash

g++ -std=c++14 -O3 -Wall -Wextra -pedantic -Wshadow -Werror -Weffc++ -Wconversion -Wsign-conversion -Wctor-dtor-privacy -Wreorder -Wold-style-cast -Wparentheses -pthread -o ./temp_99_problems__gcc -I./../include 99_problems.cpp
if [ -f ./temp_99_problems__gcc ];
then
    ./temp_99_problems__gcc
    rm ./temp_99_problems__gcc
fi

clang++ -std=c++14 -O3 -Wall -Wextra -pedantic -Wshadow -Werror -Weffc++ -Wconversion -Wsign-conversion -Wctor-dtor-privacy -Wreorder -Wold-style-cast -Wparentheses -pthread -o ./temp_99_problems__clang -I./../include 99_problems.cpp
if [ -f ./temp_99_problems__clang ];
then
    ./temp_99_problems__clang
    rm ./temp_99_problems__clang
fi


g++ -std=c++14 -O3 -Wall -Wextra -pedantic -Wshadow -Werror -Weffc++ -Wconversion -Wsign-conversion -Wctor-dtor-privacy -Wreorder -Wold-style-cast -Wparentheses -pthread -o ./temp_readme_perf_examples__gcc -I./../include readme_perf_examples.cpp
if [ -f ./temp_readme_perf_examples__gcc ];
then
    ./temp_readme_perf_examples__gcc
    rm ./temp_readme_perf_examples__gcc
fi

clang++ -std=c++14 -O3 -Wall -Wextra -pedantic -Wshadow -Werror -Weffc++ -Wconversion -Wsign-conversion -Wctor-dtor-privacy -Wreorder -Wold-style-cast -Wparentheses -pthread -o ./temp_readme_perf_examples__clang -I./../include readme_perf_examples.cpp
if [ -f ./temp_readme_perf_examples__clang ];
then
    ./temp_readme_perf_examples__clang
    rm ./temp_readme_perf_examples__clang
fi
