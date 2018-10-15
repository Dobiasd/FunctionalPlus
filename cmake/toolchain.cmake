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


####### Specific compiler adjustments
include(CheckCXXCompilerFlag)

# -Wreturn-std-move was introduced by clang 7:
# It causes an error at container_common.hpp: 338
# (sometimes std:move is required, sometimes not, depending on the template type)
check_cxx_compiler_flag(-Wreturn-std-move compiler-has-warning-return-std-move)
if (compiler-has-warning-return-std-move)
  list(APPEND COMPILE_OPTIONS -Wno-return-std-move)
endif()
