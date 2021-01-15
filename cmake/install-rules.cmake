include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

set(include_install_dir "${CMAKE_INSTALL_INCLUDEDIR}/fplus")

install(
    TARGETS fplus
    EXPORT FunctionalPlusTargets
    INCLUDES DESTINATION "${include_install_dir}"
)

# Dev component name, useful if the client embeds the library into their build
# tree and want to install only their own files
set(fplus_component "${PROJECT_NAME}_Development")

install(
    DIRECTORY "${PROJECT_SOURCE_DIR}/include/fplus" # no trailing slash
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT "${fplus_component}"
)

write_basic_package_version_file(
    FunctionalPlusConfigVersion.cmake
    COMPATIBILITY SameMajorVersion
    ARCH_INDEPENDENT
)

set(config_install_dir "${CMAKE_INSTALL_LIBDIR}/cmake/FunctionalPlus")

install(
    FILES
    "${PROJECT_SOURCE_DIR}/cmake/FunctionalPlusConfig.cmake"
    "${PROJECT_BINARY_DIR}/FunctionalPlusConfigVersion.cmake"
    DESTINATION "${config_install_dir}"
    COMPONENT "${fplus_component}"
)

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
