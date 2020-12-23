#!/bin/bash

JOBS=4
BUILD_TYPE=Release

cmake -S test -B build -D CMAKE_BUILD_TYPE=${BUILD_TYPE}
cmake --build build --config ${BUILD_TYPE} -j ${JOBS}

cd build
ctest -C ${BUILD_TYPE} -j ${JOBS} --output-on-failure
