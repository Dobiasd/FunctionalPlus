#!/bin/bash

locale-gen ru_RU
locale-gen ru_RU.UTF-8
locale-gen el_GR
locale-gen el_GR.UTF-8
update-locale
localedef -c -i ru_RU -f CP1251 ru_RU.CP1251
localedef -c -i el_GR -f CP1253 el_GR.CP1253
