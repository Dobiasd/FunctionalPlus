#!/bin/bash

mkdir -p build && cd build
cmake .. -DFPLUS_BUILD_UNITTEST=ON
cmake --build . --target unittest --config Release -- -j4
