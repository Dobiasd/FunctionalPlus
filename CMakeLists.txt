
cmake_minimum_required(VERSION 2.8)


option(UNITTEST "Build unit tests" OFF)


install (DIRECTORY include/fplus DESTINATION include)
install (FILES include/fplus.h DESTINATION include)

add_definitions(-Wall -Wextra -pedantic -Wshadow -Werror)

include(CTest)

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} -VV -C ${CMAKE_CFG_INTDIR})

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(Cpp11)

include_directories(include)

if( UNITTEST )
    enable_testing()
    subdirs(test)
endif()

