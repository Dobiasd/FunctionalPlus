#!/bin/bash

if [[ "$CXX" == clang* ]]; then
    export CXXFLAGS="-stdlib=libc++"

    # print libstdc++ version
    ldconfig -p | grep stdc++
    strings /usr/lib/libstdc++.so.6 | grep LIBCXX
fi

[ ! -d build ] && mkdir build
(
    pushd build
    cmake -DUNITTEST=ON -DCPP14TEST=ON ..
    make
    make unittest
)
