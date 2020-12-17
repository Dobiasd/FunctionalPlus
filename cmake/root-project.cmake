# This file is to be used by subprojects that make use of the library and uses
# the variable PROJECT_SOURCE_DIR from the scope of include().

include(FetchContent)
FetchContent_Declare(fplus SOURCE_DIR "${PROJECT_SOURCE_DIR}/..")
FetchContent_MakeAvailable(fplus)
