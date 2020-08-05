cmake_minimum_required(VERSION 3.5)
project(Neyson VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(NEYSON_MASTER OFF CACHE INTERNAL "")
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(NEYSON_MASTER ON CACHE INTERNAL "")
endif()

set(NEYSON_VERSION ${PROJECT_VERSION} CACHE INTERNAL "")
set(NEYSON_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}" CACHE INTERNAL "")
set(NEYSON_LIBRARIES "neyson" CACHE INTERNAL "")

option(NEYSON_BUILD_LIB "Build Neyson Library" ON)
option(NEYSON_BUILD_TESTS "Build Neyson Tests" ${NEYSON_MASTER})
option(NEYSON_INSTALL_LIB "Install Neyson Library" ${NEYSON_MASTER})

file(GLOB HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/neyson.h")
file(GLOB SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/neyson.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/neyson.cpp")

add_library(neyson ${SOURCES})

if(NEYSON_INSTALL_LIB)
    install(TARGETS neyson
        EXPORT neysonConfig
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)
    install(FILES ${HEADERS}
        DESTINATION include/neyson)
    install(EXPORT neysonConfig
        NAMESPACE neyson::
        DESTINATION lib/cmake/neyson)
    export(TARGETS neyson
        NAMESPACE neyson::
        FILE ${PROJECT_BINARY_DIR}/neysonConfig.cmake)
endif()

if(NEYSON_BUILD_TESTS)
    set(BUILD_GMOCK OFF CACHE BOOL "")
    set(INSTALL_GTEST OFF CACHE BOOL "")
    add_subdirectory(./googletest/)
    add_executable(Tests tests.cpp)
    target_link_libraries(Tests neyson gtest gtest_main)
endif()
