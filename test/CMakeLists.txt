
message(STATUS "Building Unit Tests ${UNITTEST}")

find_package(Threads REQUIRED)
find_package(GTest REQUIRED)
find_package(GMock REQUIRED)

macro(_add_test _NAME)
    add_executable(${_NAME} ${_NAME}.cpp)
    add_test(NAME ${_NAME} COMMAND ${_NAME})

    target_include_directories(${_NAME} SYSTEM PUBLIC
                                ${GTest_INCLUDE_DIR}
                                ${GMock_INCLUDE_DIR}
                                )
    target_link_libraries(${_NAME} ${GTest_LIBRARIES}
                                    ${GMock_LIBRARIES}
                                    ${CMAKE_THREAD_LIBS_INIT}
                                    )
endmacro()

_add_test(compare_test)
_add_test(composition_test)
_add_test(container_common_test)
_add_test(function_traits_test)
_add_test(filter_test)
_add_test(generate_test)
_add_test(maybe_test)
_add_test(numeric_test)
_add_test(read_test)
_add_test(result_test)
_add_test(replace_test)
_add_test(search_test)
_add_test(side_effects_test)
_add_test(stringtools_test)


add_custom_target(unittest compare_test
                        COMMAND composition_test
                        COMMAND container_common_test
                        COMMAND filter_test
                        COMMAND function_traits_test
                        COMMAND generate_test
                        COMMAND maybe_test
                        COMMAND numeric_test
                        COMMAND read_test
                        COMMAND result_test
                        COMMAND replace_test
                        COMMAND search_test
                        COMMAND side_effects_test
                        COMMAND stringtools_test
                        COMMENT "Running unittests\n\n"
                        VERBATIM
                        )



function(add_test_executable TEST_NAME)
    add_executable (${TEST_NAME} EXCLUDE_FROM_ALL ${ARGN})
    if(WIN32)
        add_test(NAME ${TEST_NAME} WORKING_DIRECTORY ${LIBRARY_OUTPUT_PATH} COMMAND ${TEST_NAME}${CMAKE_EXECUTABLE_SUFFIX})
    else()
        add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
    endif()
    add_dependencies(check ${TEST_NAME})
endfunction(add_test_executable)

add_test_executable(tests tests.cpp)
target_link_libraries(tests ${CMAKE_THREAD_LIBS_INIT})
