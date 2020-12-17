cmake_minimum_required(VERSION 3.14)

project(FunctionalPlus VERSION 0.2.13)

# ---- Warning guard ----

# Protect dependents from this project's warnings if the guard isn't disabled
set(FunctionalPlus_warning_guard SYSTEM)
if(FunctionalPlus_INCLUDE_WITHOUT_SYSTEM)
  set(FunctionalPlus_warning_guard "")
endif()

# ---- Declare library ----

add_library(fplus INTERFACE)
add_library(FunctionalPlus::fplus ALIAS fplus)
target_include_directories(
    fplus
    ${FunctionalPlus_warning_guard}
    INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
)

target_compile_features(fplus INTERFACE cxx_std_14)

find_package(Threads REQUIRED)
target_link_libraries(fplus INTERFACE Threads::Threads)

# ---- Install ----

include(cmake/install-rules.cmake)
