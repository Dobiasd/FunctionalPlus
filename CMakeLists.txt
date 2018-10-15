cmake_minimum_required(VERSION 3.2)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

project(FunctionalPlus VERSION 0.2.2)

message(STATUS "===( ${PROJECT_NAME} ${PROJECT_VERSION} )===")
option(FPLUS_USE_TOOLCHAIN "Use compiler flags from an external toolchain" OFF)
option(FPLUS_BUILD_EXAMPLES "Build examples" ON)
option(FPLUS_BUILD_UNITTEST "Build unit tests" OFF)
option(FPLUS_UNITTEST_USE_CONAN "Use conan to get doctest dependency (for unit tests only)" OFF)

message(STATUS "Building Unit Tests ${FPLUS_BUILD_UNITTEST}")
message(STATUS "Use conan ${FPLUS_UNITTEST_USE_CONAN}")
message(STATUS "Building examples ${FPLUS_BUILD_EXAMPLES}")

if(NOT FPLUS_USE_TOOLCHAIN)
  if (MSVC)
    include(cmake/toolchain_msvc.cmake)
  else()
    include(cmake/toolchain.cmake)
  endif()
endif()

add_library(fplus INTERFACE)
target_include_directories(fplus INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
  )

find_package(Threads REQUIRED)
target_link_libraries(fplus INTERFACE Threads::Threads)

if(FPLUS_BUILD_UNITTEST)
    enable_testing()
    add_subdirectory(test)
endif()

if(FPLUS_BUILD_EXAMPLES)
    add_executable(readme_perf_examples EXCLUDE_FROM_ALL examples/readme_perf_examples.cpp)
    target_compile_options(readme_perf_examples PRIVATE ${COMPILE_OPTIONS})
    target_link_libraries(readme_perf_examples PRIVATE fplus ${CMAKE_THREAD_LIBS_INIT})

    add_executable(99_problems EXCLUDE_FROM_ALL examples/99_problems.cpp)
    target_compile_options(99_problems PRIVATE ${COMPILE_OPTIONS})
    target_link_libraries(99_problems PRIVATE fplus ${CMAKE_THREAD_LIBS_INIT})
endif()

# Run pkgconfig installation:
include(cmake/pkgconfig.cmake)
