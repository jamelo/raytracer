cmake_minimum_required(VERSION 2.6)
project(raytracer)

add_executable(raytracer main.cpp)

install(TARGETS raytracer RUNTIME DESTINATION bin)
