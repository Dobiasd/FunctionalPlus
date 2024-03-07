#!/bin/bash


## Install doctest
git clone --depth=1 --branch=v2.4.11 https://github.com/doctest/doctest
cd doctest && mkdir -p build && cd build
cmake .. -DDOCTEST_WITH_TESTS=OFF -DDOCTEST_WITH_MAIN_IN_STATIC_LIB=OFF
cmake --build . --config Release --target install
