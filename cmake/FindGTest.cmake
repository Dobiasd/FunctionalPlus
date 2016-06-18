
find_package(PkgConfig)
pkg_check_modules(PKG_GTest QUIET libGTest)
set(GTest_DEFINITIONS ${PKG_GTest_CFLAGS_OTHER})

find_path(GTest_INCLUDE_DIR "gtest/gtest.h"
                                HINTS ${PKG_GTest_INCLUDE_DIRS}
                                        "${GTest_DIR}/include"
                                )

find_library(GTest_LIBRARY NAMES gtest
                                HINTS ${PKG_GTest_LIBDIR}
                                        ${PKG_GTest_LIBRARY_DIRS}
                                )
find_library(GTest_Main_LIBRARY NAMES gtest_main
                                HINTS ${PKG_GTest_LIBDIR}
                                        ${PKG_GTest_LIBRARY_DIRS}
                                        ${GTest_LIBRARY}/..
                                )


set(GTest_LIBRARIES ${GTest_LIBRARY} ${GTest_Main_LIBRARY})
set(GTest_INCLUDE_DIRS ${GTest_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GTest DEFAULT_MSG
                                        GTest_LIBRARY
                                        GTest_Main_LIBRARY
                                        GTest_INCLUDE_DIR
                                        )

mark_as_advanced(GTest_INCLUDE_DIR GTest_LIBRARY ${GTest_Main_LIBRARY})

