find_package(PkgConfig)
pkg_check_modules(PKG_doctest QUIET doctest)
set(doctest_DEFINITIONS ${PKG_doctest_CFLAGS_OTHER})

find_path(doctest_INCLUDE_DIR "doctest.h"
                                HINTS ${PKG_doctest_INCLUDE_DIRS}
                                        "${doctest_DIR}/include"
                                )

set(doctest_INCLUDE_DIRS ${doctest_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(doctest DEFAULT_MSG
                                        doctest_INCLUDE_DIR
                                        )

mark_as_advanced(doctest_INCLUDE_DIR)



