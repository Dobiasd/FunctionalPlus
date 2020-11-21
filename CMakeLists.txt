cmake_minimum_required(VERSION 3.2)

project(FunctionalPlus VERSION 0.2.10)

# Protect dependents from this project's warnings if the guard isn't disabled
set(FunctionalPlus_warning_guard SYSTEM)
if(FunctionalPlus_INCLUDE_WITHOUT_SYSTEM)
  set(FunctionalPlus_warning_guard "")
endif()

add_library(fplus INTERFACE)
add_library(FunctionalPlus::fplus ALIAS fplus)
target_include_directories(fplus ${FunctionalPlus_warning_guard} INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
  )

find_package(Threads REQUIRED)
target_link_libraries(fplus INTERFACE Threads::Threads)

# Install rules
include(cmake/install-rules.cmake)
