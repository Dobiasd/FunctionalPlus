#!/usr/bin/env bash

BUILD_JOBS=4
VERSION="${1}0"

apt-get install -y g++-6
apt-get --purge autoremove -y libc++-dev libc++abi-dev


echo "Fetching libc++ and libc++abi '${VERSION}'..."

git clone --depth=1 --branch=release_${VERSION} https://github.com/llvm-mirror/llvm.git llvm-source
git clone --depth=1 --branch=release_${VERSION} https://github.com/llvm-mirror/libcxx.git llvm-source/projects/libcxx
git clone --depth=1 --branch=release_${VERSION} https://github.com/llvm-mirror/libcxxabi.git llvm-source/projects/libcxxabi

mkdir llvm-build
cd llvm-build

cmake -DCMAKE_C_COMPILER=${CC} \
    -DCMAKE_CXX_COMPILER=${CXX} \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    -DCMAKE_INSTALL_PREFIX=/usr \
    -DLIBCXX_ABI_UNSTABLE=ON \
    ../llvm-source
    make cxx -j${BUILD_JOBS}
    make install-cxxabi install-cxx

exit 0
