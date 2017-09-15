wget https://raw.githubusercontent.com/onqtam/doctest/master/doctest/doctest.h
sudo mv ./doctest.h /usr/local/include/doctest.h

which $CXX
which $CC
which valgrind
$CXX --version
if [ -n "$CLANG_VERSION" ]; then sudo PATH="${PATH}" CXX="$CXX" CC="$CC" ./install_libcxx.sh; fi