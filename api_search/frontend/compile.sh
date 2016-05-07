#!/usr/bin/env bash

rm -r build

mkdir build
mkdir build/js

elm-make src/Main.elm --output build/js/fplus_api_search.js

if [ $? -eq 0 ]
then
  cp ./src/index.html ./build/index.html
  cp ./src/style.css ./build/style.css
  cp ./src/htmlmain.js ./build/js/htmlmain.js
fi