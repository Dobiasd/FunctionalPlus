#!/bin/bash

JOBS=4

cmake -S test -B build
cmake --build build --config Release -j $JOBS
ctest -C Release -j ${JOBS} --output-on-failure
