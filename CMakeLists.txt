cmake_minimum_required(VERSION 3.2)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

project(FunctionalPlus VERSION 0.2)

message(STATUS "===( ${PROJECT_NAME} ${PROJECT_VERSION} )===")

option(FPLUS_USE_TOOLCHAIN "Use compiler flags from an external toolchain" OFF)
option(FPLUS_BUILD_EXAMPLES "Build examples" ON)
option(FPLUS_BUILD_UNITTEST "Build unit tests" ON)
message(STATUS "Building Unit Tests ${UNITTEST}")

if(NOT FPLUS_USE_TOOLCHAIN)
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

# Installation (https://github.com/forexample/package-example) {

# Layout. This works for all platforms:
#   * <prefix>/lib/cmake/<PROJECT-NAME>
#   * <prefix>/lib/
#   * <prefix>/include/
set(config_install_dir "lib/cmake/${PROJECT_NAME}")
set(include_install_dir "include")

set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")

# Configuration
set(version_config "${generated_dir}/${PROJECT_NAME}ConfigVersion.cmake")
set(project_config "${generated_dir}/${PROJECT_NAME}Config.cmake")
set(TARGETS_EXPORT_NAME "${PROJECT_NAME}Targets")
set(namespace "${PROJECT_NAME}::")

# Include module with fuction 'write_basic_package_version_file'
include(CMakePackageConfigHelpers)

# Configure '<PROJECT-NAME>ConfigVersion.cmake'
# Use:
#   * PROJECT_VERSION
write_basic_package_version_file(
    "${version_config}" COMPATIBILITY SameMajorVersion
)

# Configure '<PROJECT-NAME>Config.cmake'
# Use variables:
#   * TARGETS_EXPORT_NAME
#   * PROJECT_NAME
configure_package_config_file(
    "cmake/Config.cmake.in"
    "${project_config}"
    INSTALL_DESTINATION "${config_install_dir}"
)

# Targets:
#   * header location after install: <prefix>/include/fplus/fplus.hpp
#   * headers can be included by C++ code `#include <fplus/fplus.hpp>`
install(
    TARGETS fplus
    EXPORT "${TARGETS_EXPORT_NAME}"
    LIBRARY DESTINATION "lib"
    ARCHIVE DESTINATION "lib"
    RUNTIME DESTINATION "bin"
    INCLUDES DESTINATION "${include_install_dir}"
)

# Headers:
#   * include/fplus/fplus.hpp -> <prefix>/include/fplus/fplus.hpp
install(
    DIRECTORY "include/fplus" # no trailing slash
    DESTINATION "${include_install_dir}"
)

# Config
#   * <prefix>/lib/cmake/FunctionalPlus/FunctionalPlusConfig.cmake
#   * <prefix>/lib/cmake/FunctionalPlus/FunctionalPlusConfigVersion.cmake
install(
    FILES "${project_config}" "${version_config}"
    DESTINATION "${config_install_dir}"
)

# Config
#   * <prefix>/lib/cmake/FunctionalPlus/FunctionalPlusTargets.cmake
install(
    EXPORT "${TARGETS_EXPORT_NAME}"
    NAMESPACE "${namespace}"
    DESTINATION "${config_install_dir}"
)

# }
