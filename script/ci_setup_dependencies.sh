#!/bin/bash


## Install doctest
git clone --depth=1 --branch=2.4.3 https://github.com/onqtam/doctest
cd doctest && mkdir -p build && cd build
cmake .. -DDOCTEST_WITH_TESTS=OFF -DDOCTEST_WITH_MAIN_IN_STATIC_LIB=OFF

BUILDUSINGCMAKE=${BUILDUSINGCMAKE:-false}
if [ $BUILDUSINGCMAKE = true ] ; then
    cmake --build . --config Release --target INSTALL
else
fi
    make -j
    make install
}
