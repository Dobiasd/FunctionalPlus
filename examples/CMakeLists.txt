cmake_minimum_required(VERSION 3.14)

project(FunctionalPlusExamples)

include(../cmake/root-project.cmake)

function(add_example NAME)
  add_executable("${NAME}" "${NAME}.cpp")
  target_link_libraries("${NAME}" PRIVATE FunctionalPlus::fplus)
  target_compile_features("${NAME}" PRIVATE cxx_std_14)
endfunction()

add_example(readme_perf_examples)
add_example(99_problems)
