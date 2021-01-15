# Installation {

# Include module for standard, cross-platform install paths
include(GNUInstallDirs)

# Layout:

# * CMake package config location
set(config_install_dir "${CMAKE_INSTALL_LIBDIR}/cmake/FunctionalPlus")

# * Installed header files' location
set(include_install_dir "${CMAKE_INSTALL_INCLUDEDIR}/fplus")

# Include module with fuction 'write_basic_package_version_file'
include(CMakePackageConfigHelpers)

# Configure '<PROJECT-NAME>ConfigVersion.cmake'
# Use:
#   * PROJECT_VERSION
write_basic_package_version_file(
    FunctionalPlusConfigVersion.cmake
    COMPATIBILITY SameMajorVersion
    ARCH_INDEPENDENT
)

# Targets:
#   * header location after install: ${CMAKE_INSTALL_INCLUDEDIR}/fplus/fplus.hpp
#   * headers can be included by C++ code `#include <fplus/fplus.hpp>`
install(
    TARGETS fplus
    EXPORT FunctionalPlusTargets
    INCLUDES DESTINATION "${include_install_dir}"
)

# Dev component name, useful if the client embeds the library into their build
# tree and want to install only their own files
set(fplus_component "${PROJECT_NAME}_Development")

# Headers:
#   * include/fplus/fplus.hpp -> ${CMAKE_INSTALL_INCLUDEDIR}/fplus/fplus.hpp
install(
    DIRECTORY "include/fplus" # no trailing slash
    DESTINATION "${include_install_dir}"
    COMPONENT "${fplus_component}"
)

# Config
#   * ${CMAKE_INSTALL_LIBDIR}/cmake/FunctionalPlus/FunctionalPlusConfig.cmake
#   * ${CMAKE_INSTALL_LIBDIR}/cmake/FunctionalPlus/FunctionalPlusConfigVersion.cmake
install(
    FILES
    "${PROJECT_SOURCE_DIR}/cmake/FunctionalPlusConfig.cmake"
    "${PROJECT_BINARY_DIR}/FunctionalPlusConfigVersion.cmake"
    DESTINATION "${config_install_dir}"
    COMPONENT "${fplus_component}"
)

# Config
#   * ${CMAKE_INSTALL_LIBDIR}/cmake/FunctionalPlus/FunctionalPlusTargets.cmake
install(
    EXPORT FunctionalPlusTargets
    NAMESPACE FunctionalPlus::
    DESTINATION "${config_install_dir}"
    COMPONENT "${fplus_component}"
)

# Conditionally include the CPack module only if this is the top project, so
# appropriate targets and files are generated for packaging the library
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  include(CPack)
endif()

# }
