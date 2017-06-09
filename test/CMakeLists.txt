
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
_add_test(container_properties_test)
_add_test(container_traits_test)
_add_test(curry_test)
_add_test(extrapolate_test)
_add_test(filter_test)
_add_test(function_traits_test)
_add_test(fwd_test)
_add_test(generate_test)
_add_test(interpolate_test)
_add_test(maps_test)
_add_test(maybe_test)
_add_test(numeric_test)
_add_test(optimize_test)
_add_test(pairs_test)
_add_test(queue_test)
_add_test(raii_test)
_add_test(read_test)
_add_test(result_test)
_add_test(replace_test)
_add_test(search_test)
_add_test(sets_test)
_add_test(shared_ref_test)
_add_test(show_test)
_add_test(side_effects_test)
_add_test(split_test)
_add_test(stringtools_test)
_add_test(transform_test)
_add_test(variant_test)


add_custom_target(unittest compare_test
                        COMMAND composition_test
                        COMMAND container_common_test
                        COMMAND container_properties_test
                        COMMAND container_traits_test
                        COMMAND curry_test
                        COMMAND extrapolate_test
                        COMMAND filter_test
                        COMMAND function_traits_test
                        COMMAND fwd_test
                        COMMAND generate_test
                        COMMAND interpolate_test
                        COMMAND maps_test
                        COMMAND maybe_test
                        COMMAND numeric_test
                        COMMAND optimize_test
                        COMMAND pairs_test
                        COMMAND queue_test
                        COMMAND raii_test
                        COMMAND read_test
                        COMMAND result_test
                        COMMAND replace_test
                        COMMAND search_test
                        COMMAND sets_test
                        COMMAND shared_ref_test
                        COMMAND show_test
                        COMMAND side_effects_test
                        COMMAND split_test
                        COMMAND stringtools_test
                        COMMAND transform_test
                        COMMAND variant_test
                        COMMENT "Running unittests\n\n"
                        VERBATIM
                        )

