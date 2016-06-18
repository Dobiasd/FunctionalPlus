
find_package(PkgConfig)
pkg_check_modules(PKG_GMock QUIET libGMock)
set(GMock_DEFINITIONS ${PKG_GMock_CFLAGS_OTHER})

find_path(GMock_INCLUDE_DIR "gmock/gmock.h"
                                HINTS ${PKG_GMock_INCLUDE_DIRS}
                                        "${GMock_DIR}/include"
                                )

find_library(GMock_LIBRARY NAMES gmock
                                HINTS ${PKG_GMock_LIBDIR}
                                        ${PKG_GMock_LIBRARY_DIRS}
                                )


set(GMock_LIBRARIES ${GMock_LIBRARY})
set(GMock_INCLUDE_DIRS ${GMock_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GMock DEFAULT_MSG
                                        GMock_LIBRARY
                                        GMock_INCLUDE_DIR
                                        )

mark_as_advanced(GMock_INCLUDE_DIR GMock_LIBRARY)

