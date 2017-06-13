#!/usr/bin/env bash

rm -r -f build

mkdir -p build
mkdir -p build/js

elm-make src/Main.elm --output build/js/fplus_api_search.js

if [ $? -eq 0 ]
then
  cp ../../logo/fplus.png ./build/
  cp -r ./src/highlight ./build/
  cp ./src/style.css ./build/style.css
  cp ./src/favicon.png ./build/favicon.png
  cp ./src/htmlmain.js ./build/js/htmlmain.js
  cp ./src/index.html ./build/index.html
fi

./ExploreCompile.sh
./TypeSignatureTestCompile.sh
