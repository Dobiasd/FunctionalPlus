#!/bin/bash

if [[ "$CXX" == clang* ]]; then
    export CXXFLAGS="-stdlib=libc++"
fi

[ ! -d build ] && mkdir build
(
    pushd build
    cmake -DUNITTEST=ON -DCPP14TEST=ON ..
    make
    make unittest
)
