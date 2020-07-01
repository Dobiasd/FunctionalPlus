FunctionalPlus
==============

Requirements and Installation
-----------------------------

You can install FunctionalPlus in **one of the following 5 ways**:


### way 1: using [cmake](https://cmake.org/)

```bash
git clone https://github.com/Dobiasd/FunctionalPlus
cd FunctionalPlus
mkdir build
cd build
cmake ..
make
sudo make install
```

And then, you can add **FunctionalPlus** as a dependency in your cmake project as in the following.

```cmake
find_package(FunctionalPlus REQUIRED)

add_executable(HelloWorld main.cpp)
target_link_libraries(HelloWorld FunctionalPlus::fplus)
```

If you want cmake to download and install the package automatically,
see [ExternalProject](#way-2-using-cmakes-externalproject) below.

#### Building the unit tests

Unit Tests are disabled by default. Building the tests (optional) requires [doctest](https://github.com/onqtam/doctest).

First, install the required locales
````bash
sudo locale-gen ru_RU
sudo locale-gen ru_RU.UTF-8
sudo locale-gen el_GR
sudo locale-gen el_GR.UTF-8
sudo localedef -c -i ru_RU -f CP1251 ru_RU.CP1251
sudo localedef -c -i el_GR -f CP1253 el_GR.CP1253
````

Then, install doctest:
```bash
git clone https://github.com/onqtam/doctest
cd doctest
mkdir -p build && cd build
cmake .. -DDOCTEST_WITH_TESTS=off -DDOCTEST_WITH_MAIN_IN_STATIC_LIB=OFF
make
sudo make install
```

Then, compile & run the tests
````bash
git clone https://github.com/Dobiasd/FunctionalPlus
cd FunctionalPlus
mkdir build
cd build
cmake .. -DFPLUS_BUILD_UNITTEST=ON
make
make test
````

As an alternative, doctest global installation can be skipped by using [conan](https://conan.io):

````bash
# pip install conan # (if conan is not installed)
git clone https://github.com/Dobiasd/FunctionalPlus
cd FunctionalPlus
mkdir build
cd build
conan install .. -obuild_unittest=True --build=missing
cmake .. -DFPLUS_BUILD_UNITTEST=ON -DFPLUS_UNITTEST_USE_CONAN=ON
make
make test
````


### way 2: using [cmake's ExternalProject](https://cmake.org/cmake/help/v3.0/module/ExternalProject.html)

You can also add `FunctionalPlus` as an `ExternalProject` to your CMakeLists.

The benefits of this:

- No installation
- Better version control with the `GIT_TAG`
  - Always get the latest version when `GIT_TAG master`
  - When you build your project, it will automatically update the headers if there is a change
  - Or get the specific version by setting it to a specific commit point


```cmake
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(FplusMinimalExternalExample)
set(CMAKE_CXX_STANDARD 14)

include(ExternalProject)
ExternalProject_Add(functional_plus
  GIT_REPOSITORY https://github.com/Dobiasd/FunctionalPlus.git
  GIT_TAG master

  SOURCE_DIR "${CMAKE_BINARY_DIR}/thirdparty/fplus"

  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""

  LOG_DOWNLOAD ON
  LOG_BUILD ON
)
set(FPLUS_INCLUDE_DIR ${CMAKE_BINARY_DIR}/thirdparty/fplus/include)
include_directories(${FPLUS_INCLUDE_DIR})

add_executable(main src/main.cpp)
add_dependencies(main functional_plus)
```


### way 3: using [cget](https://github.com/pfultz2/cget/)

```bash
# Setup up toolchain to use c++14
cget init --std=c++14
# Test and install
cget install Dobiasd/FunctionalPlus
```


### way 4: download manually

Just [download](https://github.com/Dobiasd/FunctionalPlus/archive/master.zip)/extract FunctionalPlus and tell your compiler to use the `include` directory.


### way 5: using [Conan C/C++ package manager](https://conan.io)

Just add a *conanfile.txt* with FunctionalPlus as a requirement and chose the generator for your project.

```
[requires]
functionalplus/v0.2.8-p0@Dobiasd/stable

[generators]
cmake
```

Then install it:

```bash
conan install conanfile.txt
```


### way 6: using [conda-forge](https://conda-forge.org/)

```bash
conda config --add channels conda-forge
conda install FunctionalPlus
```

Visit [`conda-forge/FunctionalPlus-feedstock`](https://github.com/conda-forge/FunctionalPlus-feedstock) for more details.
