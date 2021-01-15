# Installation {

# Include module for standard, cross-platform install paths
include(GNUInstallDirs)

# Layout:

# * CMake package config location
set(config_install_dir "${CMAKE_INSTALL_LIBDIR}/cmake/FunctionalPlus")

# * Installed header files' location
set(include_install_dir "${CMAKE_INSTALL_INCLUDEDIR}/fplus")

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
    "${version_config}" COMPATIBILITY SameMajorVersion ARCH_INDEPENDENT
)

# Configure '<PROJECT-NAME>Config.cmake'
# Use variables:
#   * TARGETS_EXPORT_NAME
configure_file("cmake/Config.cmake.in" "${project_config}" @ONLY)

# Targets:
#   * header location after install: ${CMAKE_INSTALL_INCLUDEDIR}/fplus/fplus.hpp
#   * headers can be included by C++ code `#include <fplus/fplus.hpp>`
install(
    TARGETS fplus
    EXPORT "${TARGETS_EXPORT_NAME}"
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
    FILES "${project_config}" "${version_config}"
    DESTINATION "${config_install_dir}"
    COMPONENT "${fplus_component}"
)

# Config
#   * ${CMAKE_INSTALL_LIBDIR}/cmake/FunctionalPlus/FunctionalPlusTargets.cmake
install(
    EXPORT "${TARGETS_EXPORT_NAME}"
    NAMESPACE "${namespace}"
    DESTINATION "${config_install_dir}"
    COMPONENT "${fplus_component}"
)

# Conditionally include the CPack module only if this is the top project, so
# appropriate targets and files are generated for packaging the library
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  include(CPack)
endif()

# }
