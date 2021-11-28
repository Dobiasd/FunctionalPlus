FunctionalPlus
==============

Requirements and Installation
-----------------------------

You can install FunctionalPlus in **one of the following ways**:


### way 1: using [CMake](https://cmake.org/)

```bash
git clone https://github.com/Dobiasd/FunctionalPlus
cmake -S FunctionalPlus -B FunctionalPlus/build
cmake --build FunctionalPlus/build
sudo cmake --install FunctionalPlus/build
```

<a id="cmake-dependency"></a>
And then, you can add **FunctionalPlus** as a dependency in your CMake project as in the following.

```cmake
find_package(FunctionalPlus REQUIRED)

add_executable(HelloWorld main.cpp)
target_link_libraries(HelloWorld FunctionalPlus::fplus)
```

If you want CMake to download and install the package automatically,
see [FetchContent](#way-3-using-cmake-314-fetchcontent) or
[ExternalProject](#way-2-using-cmakes-externalproject) below.

#### Building the unit tests

Unit Tests are available from a separate CMakeLists.txt file. Building the
tests requires [doctest](https://github.com/onqtam/doctest).

Install doctest:
```bash
git clone --depth=1 --branch=2.4.3 https://github.com/onqtam/doctest
cmake -S doctest -B doctest/build -DDOCTEST_WITH_TESTS=OFF -DDOCTEST_WITH_MAIN_IN_STATIC_LIB=OFF
cmake --build doctest/build -j 4
sudo cmake --install doctest/build
```

Then, compile & run the tests
````bash
git clone https://github.com/Dobiasd/FunctionalPlus
cmake -S FunctionalPlus/test -B FunctionalPlus/build
cmake --build FunctionalPlus/build -j 4
(cd FunctionalPlus/build; ctest)
````

As an alternative, doctest global installation can be skipped by installing to
a local prefix:

````bash
git clone --depth=1 --branch=2.4.3 https://github.com/onqtam/doctest
cmake -S doctest -B doctest/build -DDOCTEST_WITH_TESTS=OFF -DDOCTEST_WITH_MAIN_IN_STATIC_LIB=OFF
cmake --build doctest/build
cmake --install doctest/build --prefix doctest

git clone https://github.com/Dobiasd/FunctionalPlus
cmake -S FunctionalPlus/test -B FunctionalPlus/build -D "CMAKE_PREFIX_PATH=${PWD}/doctest"
cmake --build FunctionalPlus/build
(cd FunctionalPlus/build; ctest)
````


### way 2: using [CMake's ExternalProject](https://cmake.org/cmake/help/latest/module/ExternalProject.html)

You can also add `FunctionalPlus` as an `ExternalProject` to your CMakeLists.

The benefits of this:

- No installation
- Better version control with the `GIT_TAG`
  - Always get the latest version when `GIT_TAG master`
  - When you build your project, it will automatically update the headers if there is a change
  - Or get the specific version by setting it to a specific commit point

```cmake
cmake_minimum_required(VERSION 3.8)
project(FplusMinimalExternalExample)

include(ExternalProject)
ExternalProject_Add(
    functional_plus
    GIT_REPOSITORY https://github.com/Dobiasd/FunctionalPlus.git
    GIT_TAG master

    SOURCE_DIR "${CMAKE_BINARY_DIR}/thirdparty/fplus"

    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""

    LOG_DOWNLOAD ON
    LOG_BUILD ON
)

ExternalProject_Get_Property(functional_plus SOURCE_DIR)

add_executable(main src/main.cpp)
add_dependencies(main functional_plus)
target_compile_features(main PRIVATE cxx_std_14)
target_include_directories(main PRIVATE "${SOURCE_DIR}")
```


### way 3: using [CMake 3.14 FetchContent](https://cmake.org/cmake/help/latest/module/FetchContent.html)

This should be preferred over the
[ExternalProject](#way-2-using-cmakes-externalproject) method.

This has similar benefits, but it's easier to set up in your CMakeLists.txt.

```cmake
cmake_minimum_required(VERSION 3.14)
project(FplusMinimalExternalExample)

include(FetchContent)
FetchContent_Declare(
    functional_plus
    GIT_REPOSITORY https://github.com/Dobiasd/FunctionalPlus.git
    GIT_TAG master
)
FetchContent_MakeAvailable(functional_plus)

add_executable(main src/main.cpp)
target_compile_features(main PRIVATE cxx_std_14)
target_link_libraries(main PRIVATE FunctionalPlus::fplus)
```


### way 4: using [cget](https://github.com/pfultz2/cget/)

```bash
# Setup up toolchain to use c++14
cget init --std=c++14
# Test and install
cget install Dobiasd/FunctionalPlus
```


### way 5: download manually

Just [download](https://github.com/Dobiasd/FunctionalPlus/archive/master.zip)/extract FunctionalPlus and tell your compiler to use the `include` directory.


### way 6: using [Conan C/C++ package manager](https://conan.io)

Just add a *conanfile.txt* with FunctionalPlus as a requirement and chose the generator for your project.

```
[requires]
functionalplus/0.2.18-p0

[generators]
cmake
```

Then install it:

```bash
conan install conanfile.txt
```


### way 7: using [conda-forge](https://conda-forge.org/)

```bash
conda config --add channels conda-forge
conda install FunctionalPlus
```

Visit [`conda-forge/FunctionalPlus-feedstock`](https://github.com/conda-forge/FunctionalPlus-feedstock) for more details.


### way 8: using [Homebrew](https://brew.sh/)

```bash
brew install functionalplus
```

And then, you can add **FunctionalPlus** as a dependency in your CMake project [as in way 1](#cmake-dependency).

If you're not using CMake, you might need to add `$(brew --prefix functionalplus)/include` to the additional include paths for your compiler.


### way 9: using the "all in one" include file

[include_all_in_one/include/fplus/fplus.hpp](include_all_in_one/include/fplus/fplus.hpp) is a standalone header that groups all FunctionalPlus code.

For example, download this file into external/fplus/include/fplus, like this:

````bash
mkdir -p external/fplus/include/fplus
wget -O external/fplus/include/fplus/fplus.hpp https://raw.githubusercontent.com/Dobiasd/FunctionalPlus/master/include_all_in_one/include/fplus/fplus.hpp
````

Then, compile like this:
````bash
g++ --std=c++14 src/main.cpp -Iexternal/fplus/include
````

or, with CMake:
````cmake
cmake_minimum_required(VERSION 3.8)
project(YourProjectName)
add_executable(main src/main.cpp)
target_compile_features(main PRIVATE cxx_std_14)
target_include_directories(main PRIVATE external/fplus/include)
````

You can also use this standalone header on [Compiler Explorer](https://godbolt.org): See [this example](https://godbolt.org/z/arnEx7). Just add a line like this:
````cpp
#include "https://raw.githubusercontent.com/Dobiasd/FunctionalPlus/master/include_all_in_one/include/fplus/fplus.hpp"
````
