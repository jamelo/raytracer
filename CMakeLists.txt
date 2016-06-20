cmake_minimum_required(VERSION 2.6)
project(raytracer)

set(CMAKE_AUTOMOC ON)

add_subdirectory(gtest-1.7.0)
add_subdirectory(src)
add_subdirectory(test)

set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 1)

configure_file(
    "${PROJECT_SOURCE_DIR}/config.h.in"
    "${PROJECT_BINARY_DIR}/config.h"
)

include_directories(
    "${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_BINARY_DIR}"
)
