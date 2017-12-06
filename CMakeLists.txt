cmake_minimum_required(VERSION 3.2)

project(FunctionalPlus VERSION 0.2)

message(STATUS "===( ${PROJECT_NAME} ${PROJECT_VERSION} )===")

option(UNITTEST "Build unit tests" OFF)
message(STATUS "Building Unit Tests ${UNITTEST}")


set(COMPILE_OPTIONS -Wall
                    -Wextra
                    -pedantic
                    -Werror
                    -Weffc++
                    -Wconversion
                    -Wsign-conversion
                    -Wctor-dtor-privacy
                    -Wreorder
                    -Wold-style-cast
                    -Wparentheses
                    )

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


find_package(Threads REQUIRED)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

add_library(fplus INTERFACE)
target_include_directories(fplus INTERFACE
                       $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                       $<INSTALL_INTERFACE:include>
                        )

if( UNITTEST )
    enable_testing()
    add_subdirectory(test)
endif()

add_executable(readme_perf_examples EXCLUDE_FROM_ALL examples/readme_perf_examples.cpp)
target_compile_options(readme_perf_examples PRIVATE ${COMPILE_OPTIONS})
target_link_libraries(readme_perf_examples PRIVATE fplus ${CMAKE_THREAD_LIBS_INIT})

add_executable(99_problems EXCLUDE_FROM_ALL examples/99_problems.cpp)
target_compile_options(99_problems PRIVATE ${COMPILE_OPTIONS})
target_link_libraries(99_problems PRIVATE fplus ${CMAKE_THREAD_LIBS_INIT})


install(TARGETS fplus EXPORT fplus-config DESTINATION include)
install(DIRECTORY "${CMAKE_SOURCE_DIR}/include/" DESTINATION include)
install(EXPORT fplus-config DESTINATION share/fplus/cmake)

