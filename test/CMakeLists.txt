if (FPLUS_UNITTEST_USE_CONAN)
  # if using conan for the build, first install doctest via conan:
  # > cd build/
  # > conan install .. -obuild_unittest=True
  # > cmake .. -DFPLUS_BUILD_UNITTEST=ON -DFPLUS_UNITTEST_USE_CONAN=ON
  include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  conan_basic_setup()
else()
    find_package(doctest CONFIG REQUIRED)
endif()

set(link_libraries fplus ${CMAKE_THREAD_LIBS_INIT})
if (NOT FPLUS_UNITTEST_USE_CONAN)
    list(APPEND link_libraries doctest::doctest)
endif()

function(add_test_suite name)
    add_executable(${name} ${name}.cpp)
    add_test(NAME ${name} COMMAND ${name})
    target_compile_options(${name} PRIVATE ${COMPILE_OPTIONS})
    target_link_libraries(${name} PRIVATE ${link_libraries})
endfunction()

add_test_suite(show_versions)
add_test_suite(benchmark_session_test)
add_test_suite(compare_test)
add_test_suite(composition_test)
add_test_suite(container_common_test)
add_test_suite(container_properties_test)
add_test_suite(container_traits_test)
add_test_suite(curry_test)
add_test_suite(extrapolate_test)
add_test_suite(filter_test)
add_test_suite(function_traits_test)
add_test_suite(fwd_test)
add_test_suite(generate_test)
add_test_suite(interpolate_test)
add_test_suite(invoke_test)
add_test_suite(maps_test)
add_test_suite(maybe_test)
add_test_suite(numeric_test)
add_test_suite(optimize_test)
add_test_suite(pairs_test)
add_test_suite(queue_test)
add_test_suite(raii_test)
add_test_suite(read_test)
add_test_suite(readme_examples_test)
add_test_suite(result_test)
add_test_suite(replace_test)
add_test_suite(search_test)
add_test_suite(sets_test)
add_test_suite(shared_ref_test)
add_test_suite(show_test)
add_test_suite(side_effects_test)
add_test_suite(split_test)
add_test_suite(stopwatch_test)
add_test_suite(stringtools_test)
add_test_suite(stringtools_utf8_test)
add_test_suite(stringtools_cp1251_test)
add_test_suite(stringtools_cp1253_test)
add_test_suite(transform_test)
add_test_suite(timed_test)
add_test_suite(tree_test)
add_test_suite(udemy_course_test)
add_test_suite(variant_test)

if(NOT MSVC)
    target_compile_options(stringtools_cp1251_test
        PRIVATE -Wno-invalid-source-encoding
    )
    target_compile_options(stringtools_cp1253_test
        PRIVATE -Wno-invalid-source-encoding
    )
    target_compile_options(stringtools_utf8_test
        PRIVATE -Wno-invalid-source-encoding
    )
endif()


add_custom_target(unittest show_versions
                        COMMAND benchmark_session_test
                        COMMAND compare_test
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
                        COMMAND invoke_test
                        COMMAND maps_test
                        COMMAND maybe_test
                        COMMAND numeric_test
                        COMMAND optimize_test
                        COMMAND pairs_test
                        COMMAND queue_test
                        COMMAND raii_test
                        COMMAND read_test
                        COMMAND readme_examples_test
                        COMMAND result_test
                        COMMAND replace_test
                        COMMAND search_test
                        COMMAND sets_test
                        COMMAND shared_ref_test
                        COMMAND show_test
                        COMMAND side_effects_test
                        COMMAND split_test
                        COMMAND stopwatch_test
                        COMMAND stringtools_test
                        COMMAND stringtools_cp1251_test
                        COMMAND stringtools_cp1253_test
                        COMMAND stringtools_utf8_test
                        COMMAND timed_test
                        COMMAND transform_test
                        COMMAND tree_test
                        COMMAND udemy_course_test
                        COMMAND variant_test
                        COMMENT "Running unittests\n\n"
                        VERBATIM
                        )
