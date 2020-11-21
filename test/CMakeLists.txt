cmake_minimum_required(VERSION 3.2)

project(FunctionalPlusTests)

set(
    FunctionalPlus_INCLUDE_WITHOUT_SYSTEM
    YES
    CACHE
    INTERNAL
    "Turn the warning guard off to have errors appear in test builds"
)
add_subdirectory("${PROJECT_SOURCE_DIR}/.." "${PROJECT_BINARY_DIR}/root_project")

find_package(doctest 2.3.4 CONFIG REQUIRED)
# for doctest_discover_tests
include(doctest)

enable_testing()

set(
    tests
    show_versions
    benchmark_session_test
    compare_test
    composition_test
    container_common_test
    container_properties_test
    container_traits_test
    curry_test
    extrapolate_test
    filter_test
    function_traits_test
    fwd_test
    generate_test
    interpolate_test
    invoke_test
    maps_test
    maybe_test
    numeric_test
    optimize_test
    pairs_test
    queue_test
    raii_test
    read_test
    readme_examples_test
    result_test
    replace_test
    search_test
    sets_test
    shared_ref_test
    show_test
    side_effects_test
    split_test
    stopwatch_test
    stringtools_test
    stringtools_utf8_test
    stringtools_cp1251_test
    stringtools_cp1253_test
    transform_test
    timed_test
    tree_test
    udemy_course_test
    variant_test
)

foreach(name IN LISTS tests)
  add_executable("${name}" "${name}.cpp")
  target_compile_options("${name}" PRIVATE "${COMPILE_OPTIONS}")
  target_link_libraries(
      "${name}"
      PRIVATE
      FunctionalPlus::fplus
      doctest::doctest
  )
  # CMake 3.2 doesn't know cxx_std_14 so instead we use cxx_variable_templates
  target_compile_features("${name}" INTERFACE cxx_variable_templates)
  doctest_discover_tests("${name}")
endforeach()

if(NOT MSVC)
  foreach(encoding IN ITEMS cp1251 cp1253 utf8)
    target_compile_options(
        "stringtools_${encoding}_test"
        PRIVATE
        -Wno-invalid-source-encoding
    )
  endforeach()
endif()
