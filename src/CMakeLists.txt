cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
find_package(Threads)
find_package(Qt4 REQUIRED)
find_package(Boost 1.55 COMPONENTS REQUIRED)
find_package(SFML 2.2 REQUIRED system window graphics)

set(SOURCES
    main.cpp
    Raytracer.cpp
    RaytracerWindow.cpp
    Canvas.cpp
)

add_executable(raytracer ${SOURCES})
set(CMAKE_AUTOMOC ON)
qt4_automoc(${SOURCES})

include_directories(
    "${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_BINARY_DIR}"
    ${Boost_INCLUDE_DIR}
    ${QT_INCLUDES}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/vendor/json/src
)

target_link_libraries(raytracer ${Boost_LIBRARIES} -lboost_system -lboost_timer)
target_link_libraries(raytracer ${SFML_LIBRARIES})
target_link_libraries(raytracer ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(raytracer ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})

set(CMAKE_C_FLAGS                   "-Wall -pendantic -Wextra -std=c99")
set(CMAKE_C_FLAGS_DEBUG             "-g -O0")
set(CMAKE_C_FLAGS_MINSIZEREL        "-Os -DNDEBUG")
set(CMAKE_C_FLAGS_RELEASE           "-O4 -DNDEBUG")
set(CMAKE_C_FLAGS_RELWITHDEBINFO    "-O2 -g")

#set(CMAKE_CXX_COMPILER              "clang++")
set(CMAKE_CXX_FLAGS                 "-Wall -pedantic -Wextra -std=c++14 -Wno-missing-braces")
set(CMAKE_CXX_FLAGS_DEBUG           "-g -O0")
set(CMAKE_CXX_FLAGS_MINSIZEREL      "-Os -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE         "-O4 -DNDEBUG -mfpmath=sse -mmmx -msse -msse2 -msse3 -ggdb")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO  "-O2 -g -pg")

install(TARGETS raytracer RUNTIME DESTINATION bin)

# CPack packaging
include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
include(CPack)
