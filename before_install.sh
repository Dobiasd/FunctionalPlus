
export GTEST_VERSION=master
export GTEST=googletest-${GTEST_VERSION}
wget https://github.com/google/googletest/archive/${GTEST_VERSION}.tar.gz
tar -xzvf *.tar.gz
cd ${GTEST}
mkdir build && cd build
cmake ..
make -j4 && sudo make install && cd ../..

