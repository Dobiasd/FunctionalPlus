#!/bin/bash

[ ! -d build ] && mkdir build
(
    pushd build
    cmake -DUNITTEST=ON -DCPP14TEST=ON ..
    make
    make unittest
)
