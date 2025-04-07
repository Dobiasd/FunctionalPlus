#!/bin/bash

# Central script called by the CI
# Usage: 
#    ci.sh {run_build|run_tests}

#
#  Private Impl
#

# Get the directory of the current script (this is bash's notion of poetry)
THIS_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
REPO_DIR="$THIS_DIR"/..

# Determine the install prefix based on the OS
if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" ]]; then
    # Windows path handling is a never ending source of ...
    INSTALL_PREFIX="$USERPROFILE\\.local"
    INSTALL_PREFIX=$(cygpath -u "$INSTALL_PREFIX")  # Make CMake happy when using git bash under windows
    export CMAKE_PREFIX_PATH=$INSTALL_PREFIX;$CMAKE_PREFIX_PATH  # Notice the ";" instead of ":"
else
    INSTALL_PREFIX="$HOME/.local"
    export CMAKE_PREFIX_PATH=$INSTALL_PREFIX:$CMAKE_PREFIX_PATH
fi

# Function to install doctest
_install_doctest() {
    cd "$REPO_DIR"
    git clone --depth=1 --branch=v2.4.11 https://github.com/doctest/doctest
    cd doctest && mkdir -p build && cd build
    cmake .. -DDOCTEST_WITH_TESTS=OFF -DDOCTEST_WITH_MAIN_IN_STATIC_LIB=OFF -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX
    cmake --build . --config Release --target install
}

# Function to build the project
_build_impl() {
    cd "$REPO_DIR"
    JOBS=4
    BUILD_TYPE=Release
    cmake -S test -B build -D CMAKE_BUILD_TYPE=${BUILD_TYPE}
    cmake --build build --config ${BUILD_TYPE} -j ${JOBS}
}

# Function to run tests
_tests_impl() {
    cd "$REPO_DIR/build"
    JOBS=4
    BUILD_TYPE=Release
    ctest -C ${BUILD_TYPE} -j ${JOBS} --output-on-failure
}

#
# API
#

# API function to run tests
run_tests() {
    _install_doctest
    _build_impl
    _tests_impl
}

# API function to build the project
run_build() {
    _install_doctest
    _build_impl
}

# Main script logic
case "$1" in
    run_build)
        run_build
        ;;
    run_tests)
        run_tests
        ;;
    *)
        echo "Usage: $0 {run_build|run_tests}"
        exit 1
        ;;
esac
