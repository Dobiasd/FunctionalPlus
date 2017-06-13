#!/usr/bin/env bash

elm-make src/Explore.elm --output build/js/fplus_api_explore.js

if [ $? -eq 0 ]
then
  cp ./src/explore.html ./build/explore.html
  cp ./src/htmlmain_explore.js ./build/js/htmlmain_explore.js
  cp ./src/style_explore.css ./build/style_explore.css
fi