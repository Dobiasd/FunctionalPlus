#!/bin/bash

VERSION=$1

if [[ "${VERSION}" -ge "9" ]]; then
    wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add -
    add-apt-repository "deb http://apt.llvm.org/focal/ llvm-toolchain-focal-${VERSION} main"
    apt update
fi

if [[ "${VERSION}" -le "6" ]]; then
    VERSION_FULL="${VERSION}.0"
    apt-get install -y clang-${VERSION_FULL}
    ln -s $(which clang-${VERSION_FULL}) /usr/bin/clang-${VERSION}
    ln -s $(which clang++-${VERSION_FULL}) /usr/bin/clang++-${VERSION}
else
    apt-get install -y clang-${VERSION} libc++-${VERSION}-dev libc++abi-${VERSION}-dev
fi
