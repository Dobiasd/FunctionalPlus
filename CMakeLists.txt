cmake_minimum_required(VERSION 2.8.11)

project(FunctionalPlus)
set(PROJECT_VERSION 0.1)

message(STATUS "===( ${PROJECT_NAME} ${PROJECT_VERSION} )===")

option(UNITTEST "Build unit tests" OFF)


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
                )

find_package(Threads REQUIRED)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(Cpp11)

include_directories(include)

if( UNITTEST )
    enable_testing()
    subdirs(test)
endif()


add_executable(readme_examples EXCLUDE_FROM_ALL examples/readme_examples.cpp)
target_link_libraries(readme_examples ${CMAKE_THREAD_LIBS_INIT})
