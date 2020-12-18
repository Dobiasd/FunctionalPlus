#!/bin/bash

apt-get update
apt-get install -y wget git locales


## Install CMake
wget -nv https://github.com/Kitware/CMake/releases/download/v3.19.2/cmake-3.19.2-Linux-x86_64.sh -O setup_cmake.sh
chmod +x setup_cmake.sh
./setup_cmake.sh --prefix=/usr/local --skip-license


## Generate locales for tests
locale-gen ru_RU
locale-gen ru_RU.UTF-8
locale-gen el_GR
locale-gen el_GR.UTF-8
update-locale
localedef -c -i ru_RU -f CP1251 ru_RU.CP1251
localedef -c -i ru_RU -f UTF-8 ru_RU.UTF-8
localedef -c -i el_GR -f CP1253 el_GR.CP1253
localedef -c -i el_GR -f UTF-8 el_GR.UTF-8
