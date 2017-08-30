cmake_minimum_required(VERSION 3.2)

project(FunctionalPlus)
set(PROJECT_VERSION 0.2)

message(STATUS "===( ${PROJECT_NAME} ${PROJECT_VERSION} )===")

option(UNITTEST "Build unit tests" OFF)


install(DIRECTORY include/fplus DESTINATION include)

add_compile_options(-Wall
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

include_directories(include)

if( UNITTEST )
    enable_testing()
    subdirs(test)
endif()


set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


add_executable(readme_examples EXCLUDE_FROM_ALL examples/readme_examples.cpp)
target_link_libraries(readme_examples ${CMAKE_THREAD_LIBS_INIT})
