#!/bin/bash

[ ! -d build ] && mkdir build
(
    pushd build
    cmake -DUNITTEST=ON ..
    make
    make unittest
)
