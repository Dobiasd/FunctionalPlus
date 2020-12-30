#!/bin/bash


## Install doctest
git clone --depth=1 --branch=2.4.3 https://github.com/onqtam/doctest
cd doctest && mkdir -p build && cd build
cmake .. -DDOCTEST_WITH_TESTS=OFF -DDOCTEST_WITH_MAIN_IN_STATIC_LIB=OFF

BUILD_USING_CMAKE=${BUILD_USING_CMAKE:-false}
if [ $BUILD_USING_CMAKE = true ]
then
    cmake --build . --config Release --target INSTALL
else
    make -j
    make install
fi