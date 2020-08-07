cmake_minimum_required(VERSION 3.5)
project(Neyson VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(NEYSON_MASTER OFF CACHE INTERNAL "")
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(NEYSON_MASTER ON CACHE INTERNAL "")
endif()

set(NEYSON_VERSION ${PROJECT_VERSION} CACHE INTERNAL "")
set(NEYSON_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/" CACHE INTERNAL "")
set(NEYSON_LIBRARIES "neyson" CACHE INTERNAL "")
option(NEYSON_BUILD_LIB "Build Neyson Library" ON)
option(NEYSON_BUILD_TESTS "Build Neyson Tests" ${NEYSON_MASTER})
option(NEYSON_INSTALL_LIB "Install Neyson Library" ${NEYSON_MASTER})

file(GLOB HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/neyson/neyson.h"
    "${CMAKE_CURRENT_BINARY_DIR}/config.h")
file(GLOB SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/neyson/neyson.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/neyson/neyson.cpp")

if(NEYSON_BUILD_LIB)
    add_library(neyson ${SOURCES})
    target_include_directories(neyson PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include/>)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/src/neyson/config.h.in"
        "${CMAKE_CURRENT_BINARY_DIR}/config.h")
endif()

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
    add_executable(Tests "test/main.cpp")
    target_link_libraries(Tests neyson gtest gtest_main)
endif()
