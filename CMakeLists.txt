cmake_minimum_required(VERSION 3.2)

project(FunctionalPlus VERSION 0.2.10)

message(STATUS "===( ${PROJECT_NAME} ${PROJECT_VERSION} )===")
option(FPLUS_USE_TOOLCHAIN "Use compiler flags from an external toolchain" OFF)

if(NOT FPLUS_USE_TOOLCHAIN)
  if (MSVC)
    include(cmake/toolchain_msvc.cmake)
  else()
    include(cmake/toolchain.cmake)
  endif()
endif()

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
