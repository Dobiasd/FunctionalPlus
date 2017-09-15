#!/bin/bash

if [[ "$CXX" == clang* ]]; then
    export CXXFLAGS="-stdlib=libc++"
fi

[ ! -d build ] && mkdir build
(
    pushd build
    cmake -DUNITTEST=ON ..
    make
    make unittest
)
