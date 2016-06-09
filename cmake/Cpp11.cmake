
include(CheckCXXCompilerFlag)

CHECK_CXX_COMPILER_FLAG("-std=c++11" CPP_STD_11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" CPP_STD_0x)

if( CPP_STD_11 )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif( CPP_STD_0x )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(WARNING "C++11 not supported by ${CMAKE_CXX_COMPILER}")
endif()

