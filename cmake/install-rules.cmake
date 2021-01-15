include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# Create an export set for the CMake package
install(
    TARGETS fplus
    EXPORT FunctionalPlusTargets
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

# Dev component name, useful if the client embeds the library into their build
# tree and want to install only their own files
set(fplus_component FunctionalPlus_Development)

# Install the include/fplus directory to a location that was added to the
# export set and the include directories of its target above
install(
    DIRECTORY "${PROJECT_SOURCE_DIR}/include/fplus" # no trailing slash
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT "${fplus_component}"
)

# Use the PROJECT_VERSION variable to generate a basic config version file in
# the PROJECT_BINARY_DIR directory
write_basic_package_version_file(
    FunctionalPlusConfigVersion.cmake
    COMPATIBILITY SameMajorVersion
    ARCH_INDEPENDENT
)

# Location of the CMake package that find_package() can find
set(config_install_dir "${CMAKE_INSTALL_LIBDIR}/cmake/FunctionalPlus")

install(
    FILES
    "${PROJECT_SOURCE_DIR}/cmake/FunctionalPlusConfig.cmake"
    "${PROJECT_BINARY_DIR}/FunctionalPlusConfigVersion.cmake"
    DESTINATION "${config_install_dir}"
    COMPONENT "${fplus_component}"
)

# Generate the FunctionalPlusTargets.cmake file in the config_install_dir that
# is included by FunctionalPlusConfig.cmake after the dependencies have been
# found
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
