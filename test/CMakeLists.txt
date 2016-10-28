
message(STATUS "Building Unit Tests ${UNITTEST}")

find_package(doctest REQUIRED)


macro(_add_test _NAME)
    add_executable(${_NAME} ${_NAME}.cpp)
    add_test(NAME ${_NAME} COMMAND ${_NAME})

    target_include_directories(${_NAME} SYSTEM PUBLIC ${doctest_INCLUDE_DIR})
    target_link_libraries(${_NAME} ${CMAKE_THREAD_LIBS_INIT})
endmacro()

_add_test(compare_test)
_add_test(composition_test)
_add_test(container_common_test)
_add_test(function_traits_test)
_add_test(filter_test)
_add_test(generate_test)
_add_test(maybe_test)
_add_test(numeric_test)
_add_test(optimize_test)
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
                        COMMAND optimize_test
                        COMMAND read_test
                        COMMAND result_test
                        COMMAND replace_test
                        COMMAND search_test
                        COMMAND side_effects_test
                        COMMAND stringtools_test
                        COMMENT "Running unittests\n\n"
                        VERBATIM
                        )

