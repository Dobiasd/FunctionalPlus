#!/bin/bash

apt-get update
apt-get install -y git locales


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
