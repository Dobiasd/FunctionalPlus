cmake_minimum_required(VERSION 2.8)

message(STATUS "Building Unit Tests ${UNITTEST}")

find_package(Threads REQUIRED)
find_package(GTest REQUIRED)
find_package(GMock REQUIRED)

macro(_add_test _NAME)
    add_executable(${_NAME} ${_NAME}.cpp)
    add_test(NAME ${_NAME} COMMAND ${_NAME})

    target_include_directories(${_NAME} PUBLIC
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
_add_test(function_traits_test)
_add_test(filter_test)
_add_test(generate_test)
_add_test(maybe_test)
_add_test(numeric_test)
_add_test(result_test)


add_custom_target(unittest compare_test
                        COMMAND composition_test
                        COMMAND filter_test
                        COMMAND function_traits_test
                        COMMAND generate_test
                        COMMAND maybe_test
                        COMMAND numeric_test
                        COMMAND result_test
                        COMMENT "Running unittests\n\n"
                        VERBATIM
                        )
