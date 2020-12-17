#!/bin/bash

git clone --depth=1 https://github.com/onqtam/doctest
cd doctest && mkdir -p build && cd build
cmake .. -DDOCTEST_WITH_TESTS=OFF -DDOCTEST_WITH_MAIN_IN_STATIC_LIB=OFF
make -j
make install
