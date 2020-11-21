cmake_minimum_required(VERSION 3.2)

project(FunctionalPlus VERSION 0.2.10)

add_library(fplus INTERFACE)
add_library(FunctionalPlus::fplus ALIAS fplus)
target_include_directories(fplus INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
  )

find_package(Threads REQUIRED)
target_link_libraries(fplus INTERFACE Threads::Threads)

add_subdirectory(include_all_in_one)

# Run pkgconfig installation:
include(cmake/pkgconfig.cmake)
