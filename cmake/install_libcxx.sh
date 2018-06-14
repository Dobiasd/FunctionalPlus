#!/usr/bin/env bash

BUILD_JOBS=4
VERSION=$(echo "${CLANG_VERSION}" | sed 's/\.//g')

echo "Fetching libc++ and libc++abi '${VERSION}'..."

# Checkout LLVM sources
git clone --depth=1 --branch=release_${VERSION} https://github.com/llvm-mirror/llvm.git llvm-source
git clone --depth=1 --branch=release_${VERSION} https://github.com/llvm-mirror/libcxx.git llvm-source/projects/libcxx
git clone --depth=1 --branch=release_${VERSION} https://github.com/llvm-mirror/libcxxabi.git llvm-source/projects/libcxxabi

mkdir llvm-build
cd llvm-build

# - libc++ versions < 4.x do not have the install-cxxabi and install-cxx targets
# - only ASAN is enabled for clang/libc++ versions < 4.x
if [[ ${VERSION} == "3"* ]]; then
    echo "Clang 3.x"
    cmake -DCMAKE_C_COMPILER=${CC} \
            -DCMAKE_CXX_COMPILER=${CXX} \
            -DCMAKE_BUILD_TYPE=RelWithDebInfo \
            -DCMAKE_INSTALL_PREFIX=/usr \
            ../llvm-source
    if [[ $SANITIZER == "Address;Undefined" ]]; then
        ASAN_FLAGS="-fsanitize=address"
        cmake -DCMAKE_CXX_FLAGS="${ASAN_FLAGS}" \
                -DCMAKE_EXE_LINKER_FLAGS="${ASAN_FLAGS}" \
                ../llvm-source
    fi
    make cxx -j${BUILD_JOBS}
    sudo cp -r lib/* /usr/lib/
    sudo cp -r include/c++ /usr/include/
else
    cmake -DCMAKE_C_COMPILER=${CC} \
            -DCMAKE_CXX_COMPILER=${CXX} \
            -DCMAKE_BUILD_TYPE=RelWithDebInfo \
            -DCMAKE_INSTALL_PREFIX=/usr \
            -DLIBCXX_ABI_UNSTABLE=ON \
            -DLLVM_USE_SANITIZER=${SANITIZER} \
            ../llvm-source
    make cxx -j${BUILD_JOBS}
    sudo make install-cxxabi install-cxx
fi

exit 0
