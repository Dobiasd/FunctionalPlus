#!/usr/bin/env bash
(find api_search -name "*.cpp" && find include -name "*.hpp" && find examples -name "*.cpp" && find test -name "*.cpp") | xargs clang-format --style=WebKit -i {}
