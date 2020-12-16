#!/bin/bash

# Install doctest
git clone --depth=1 https://github.com/onqtam/doctest
cd doctest && mkdir -p build && cd build
cmake .. -DDOCTEST_WITH_TESTS=OFF -DDOCTEST_WITH_MAIN_IN_STATIC_LIB=OFF
make -j
make install


# Setup locales
locale-gen ru_RU
locale-gen ru_RU.UTF-8
locale-gen el_GR
locale-gen el_GR.UTF-8
update-locale
localedef -c -i ru_RU -f CP1251 ru_RU.CP1251
localedef -c -i el_GR -f CP1253 el_GR.CP1253
