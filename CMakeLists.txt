cmake_minimum_required(VERSION 3.13)

project(static_map C ASM)

add_library(static_map INTERFACE)

target_sources(static_map INTERFACE
	src/static_map.c
)

target_include_directories(static_map INTERFACE
	src
)

# Option to build standalone executable for testing
option(STATIC_MAP_TEST "Build standalone executable for static_map" OFF)

if(STATIC_MAP_TEST)
    set(CMAKE_C_COMPILER gcc)

    # Add standalone executable for testing static_map
    add_executable(test_static_map test/test_static_map.c)

    # Link the static_map library to the standalone executable
    target_link_libraries(test_static_map PRIVATE static_map)

    # Optionally, add any specific compiler options for testing
    target_compile_options(test_static_map PRIVATE -Wall -Wextra -pedantic)
endif()