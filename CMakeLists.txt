cmake_minimum_required(VERSION 3.2)

project(FunctionalPlus)
set(PROJECT_VERSION 0.2)

message(STATUS "===( ${PROJECT_NAME} ${PROJECT_VERSION} )===")

option(UNITTEST "Build unit tests" OFF)
option(CPP14TEST "Build with C++14 enabled" OFF)


install(DIRECTORY include/fplus DESTINATION include)

add_definitions(-Wall
                -Wextra
                -pedantic
                -Wshadow
                -Werror
                -Weffc++
                -Wconversion
                -Wsign-conversion
                -Wctor-dtor-privacy
                -Wreorder
                -Wold-style-cast
                -Wparentheses
                )

find_package(Threads REQUIRED)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(Cpp11)

include_directories(include)

if( UNITTEST )
    enable_testing()
    subdirs(test)
endif()

if( CPP14TEST )
    set(CMAKE_CXX_STANDARD 14)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()


add_executable(readme_examples EXCLUDE_FROM_ALL examples/readme_examples.cpp)
target_link_libraries(readme_examples ${CMAKE_THREAD_LIBS_INIT})
