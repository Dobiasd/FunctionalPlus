cmake_minimum_required(VERSION 2.8.11)

project(FunctionalPlus)
set(PROJECT_VERSION 0.1)

message(STATUS "===( ${PROJECT_NAME} ${PROJECT_VERSION} )===")


option(UNITTEST "Build unit tests" OFF)


install (DIRECTORY include/fplus DESTINATION include)
install (FILES include/fplus.h DESTINATION include)

add_definitions(-Wall -Wextra -pedantic -Wshadow -Werror -Weffc++ -Wconversion -Wsign-conversion)

include(CTest)

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} -VV -C ${CMAKE_CFG_INTDIR})

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(Cpp11)

include_directories(include)

if( UNITTEST )
    enable_testing()
    subdirs(test)
endif()
