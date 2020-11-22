cmake_minimum_required(VERSION 3.2)

project(FunctionalPlusExamples)

add_subdirectory("${PROJECT_SOURCE_DIR}/.." "${PROJECT_BINARY_DIR}/root_project")

function(add_example NAME)
  add_executable("${NAME}" "${NAME}.cpp")
  target_link_libraries("${NAME}" PRIVATE FunctionalPlus::fplus)
  # CMake 3.2 doesn't know cxx_std_14 so instead we use cxx_variable_templates
  target_compile_features("${NAME}" PRIVATE cxx_variable_templates)
endfunction()

add_example(readme_perf_examples)
add_example(99_problems)
