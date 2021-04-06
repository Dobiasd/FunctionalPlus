cmake_minimum_required(VERSION 3.14)

project(FunctionalPlusTests)

set(
    FunctionalPlus_INCLUDE_WITHOUT_SYSTEM
    YES
    CACHE
    INTERNAL
    "Turn the warning guard off to have errors appear in test builds"
)
include(../cmake/root-project.cmake)
include(../cmake/warnings.cmake)

find_package(doctest 2.4.3 CONFIG REQUIRED)
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
    transform_test
    timed_test
    tree_test
    udemy_course_test
    variant_test
)

foreach(name IN LISTS tests)
  add_executable("${name}" "${name}.cpp")
  target_compile_options("${name}" PRIVATE ${project_warnings})
  target_compile_definitions(
      "${name}"
      PRIVATE
      DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
  )
  target_link_libraries(
      "${name}"
      PRIVATE
      FunctionalPlus::fplus
      doctest::doctest
  )
  target_compile_features("${name}" PRIVATE cxx_std_14)
  doctest_discover_tests("${name}")
endforeach()
