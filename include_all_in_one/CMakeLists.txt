find_package (Python COMPONENTS Interpreter QUIET)
if (Python_FOUND)
    add_custom_target(
        include_all_in_one
        ALL
        COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/make_all_in_one.py
    )
else()
    message(STATUS "Not generating include_all_in_one, python was not found.")
endif()
