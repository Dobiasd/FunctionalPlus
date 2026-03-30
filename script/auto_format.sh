#!/usr/bin/env bash

set -e

(find api_search -name "*.cpp" && find include -name "*.hpp" && find examples -name "*.cpp" && find test -name "*.cpp") | xargs clang-format -i {}
