
cmake_minimum_required(VERSION 2.8)

install (DIRECTORY include/fplus DESTINATION include)
install (FILES include/fplus.h DESTINATION include)

add_definitions(-Wall -Wextra -pedantic -Wshadow -Werror)

include(CTest)

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} -VV -C ${CMAKE_CFG_INTDIR})

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(Cpp11)

function(add_test_executable TEST_NAME)
    add_executable (${TEST_NAME} EXCLUDE_FROM_ALL ${ARGN})
    if(WIN32)
        add_test(NAME ${TEST_NAME} WORKING_DIRECTORY ${LIBRARY_OUTPUT_PATH} COMMAND ${TEST_NAME}${CMAKE_EXECUTABLE_SUFFIX})
    else()
        add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
    endif()
    add_dependencies(check ${TEST_NAME})
endfunction(add_test_executable)

find_package(Threads)

include_directories(include)
add_test_executable(tests test/tests.cpp)
target_link_libraries(tests ${CMAKE_THREAD_LIBS_INIT})
