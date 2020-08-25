# BSD 3-Clause License
#
# Copyright (c) 2020, Shahriar Rezghi <shahriar25.ss@gmail.com>
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its
#    contributors may be used to endorse or promote products derived from
#    this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

cmake_minimum_required(VERSION 3.5)
project(Neyson VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(NEYSON_MASTER OFF CACHE INTERNAL "")
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(NEYSON_MASTER ON CACHE INTERNAL "")
endif()

set(NEYSON_VERSION ${PROJECT_VERSION} CACHE INTERNAL "")
set(NEYSON_LIBRARIES "neyson" CACHE INTERNAL "")
set(NEYSON_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/"
    "${CMAKE_CURRENT_BINARY_DIR}/gen/" CACHE INTERNAL "")

option(NEYSON_BUILD_LIB "Build Neyson Library" ON)
option(NEYSON_BUILD_TESTS "Build Neyson Tests" ${NEYSON_MASTER})
option(NEYSON_INSTALL_LIB "Install Neyson Library" ${NEYSON_MASTER})

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/neyson/config.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/gen/neyson/config.h")
file(GLOB HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/neyson/neyson.h"
    "${CMAKE_CURRENT_BINARY_DIR}/gen/neyson/config.h")
file(GLOB SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/neyson/neyson.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/neyson/neyson.cpp")

if(NEYSON_BUILD_LIB)
    add_library(neyson ${SOURCES})
    target_include_directories(neyson PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/gen/>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include/>)
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
    if(WIN32)
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    endif(WIN32)

    set(BUILD_GMOCK OFF CACHE BOOL "")
    set(INSTALL_GTEST OFF CACHE BOOL "")

    add_subdirectory(./googletest/)
    add_executable(Tests "test/main.cpp")
    target_link_libraries(Tests neyson gtest gtest_main)
endif()
