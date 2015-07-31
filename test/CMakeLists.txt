include_directories(
    SYSTEM ${gtest_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include
)

file(GLOB test_sources "*.cpp")

set(CMAKE_CXX_COMPILER "clang++")

set(CMAKE_CXX_FLAGS "-std=c++14 -Wall -Weffc++ -pedantic -Wextra")

add_executable(raytracer_test ${test_sources})
target_link_libraries(raytracer_test gtest_main)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
find_package(SFML 2.2 REQUIRED system window graphics)
target_link_libraries(raytracer_test ${SFML_LIBRARIES})
