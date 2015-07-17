cmake_minimum_required(VERSION 2.6)
project(raytracer)

add_subdirectory(gtest-1.7.0)
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

set(EXECUTABLE_NAME "raytracer")
add_executable(${EXECUTABLE_NAME} main.cpp)

set_target_properties(${EXECUTABLE_NAME} PROPERTIES COMPILE_FLAGS "")

# Use Clang
#set(CMAKE_C_COMPILER                "/usr/bin/clang")
set(CMAKE_C_FLAGS                   "-Wall -pendantic -Wextra -std=c99")
set(CMAKE_C_FLAGS_DEBUG             "-g -O0")
set(CMAKE_C_FLAGS_MINSIZEREL        "-Os -DNDEBUG")
set(CMAKE_C_FLAGS_RELEASE           "-O4 -DNDEBUG")
set(CMAKE_C_FLAGS_RELWITHDEBINFO    "-O2 -g")

#set(CMAKE_CXX_COMPILER              "clang++")
set(CMAKE_CXX_FLAGS                 "-Wall -pedantic -Wextra -std=c++14 -Wno-missing-braces")
set(CMAKE_CXX_FLAGS_DEBUG           "-g -O0")
set(CMAKE_CXX_FLAGS_MINSIZEREL      "-Os -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE         "-O4 -DNDEBUG -mfpmath=sse -mmmx -msse -msse2 -msse3 -save-temps")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO  "-O2 -g -pg -save-temps")

#set(CMAKE_AR      "/usr/bin/llvm-ar")
#set(CMAKE_LINKER  "/usr/bin/llvm-ld")
#set(CMAKE_NM      "/usr/bin/llvm-nm")
#set(CMAKE_OBJDUMP "/usr/bin/llvm-objdump")
#set(CMAKE_RANLIB  "/usr/bin/llvm-ranlib")

# Require SFML
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
find_package(SFML 2.2 REQUIRED system window graphics)
target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES})

find_package( Boost 1.55 COMPONENTS REQUIRED )
include_directories( ${Boost_INCLUDE_DIR} )
target_link_libraries(${EXECUTABLE_NAME} ${Boost_LIBRARIES} -lboost_system -lboost_timer)

find_package(Threads)
target_link_libraries(${EXECUTABLE_NAME} ${CMAKE_THREAD_LIBS_INIT})

install(TARGETS ${EXECUTABLE_NAME} RUNTIME DESTINATION bin)

# CPack packaging
include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
include(CPack)
