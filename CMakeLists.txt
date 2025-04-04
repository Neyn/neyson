# BSD 3-Clause License
#
# Copyright (c) 2020-2025, Shahriar Rezghi <shahriar.rezghi.sh@gmail.com>
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

cmake_minimum_required(VERSION 3.10)
project(Neyson VERSION 2.0.0 LANGUAGES CXX)

#---------------------------------------------------------------------
# Determine if this is the master project and set test option accordingly
#---------------------------------------------------------------------
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(NEYSON_MASTER ON)
else()
  set(NEYSON_MASTER OFF)
endif()
option(NEYSON_BUILD_TESTS "Build Neyson tests" ${NEYSON_MASTER})

#---------------------------------------------------------------------
# SQLite Support: Enable by default if found, else disable
#---------------------------------------------------------------------
find_package(SQLite3 QUIET)
if(SQLite3_FOUND)
  option(NEYSON_ENABLE_SQLITE "Build Neyson with SQLite support" ON)
else()
  option(NEYSON_ENABLE_SQLITE "Build Neyson with SQLite support" OFF)
endif()

if(NEYSON_ENABLE_SQLITE AND NOT SQLite3_FOUND)
  message(FATAL_ERROR "SQLite interface is enabled but couldn't find the SQLite3 library")
endif()

#---------------------------------------------------------------------
# Set C++ Standard and Position Independent Code
#---------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

#---------------------------------------------------------------------
# Configure the header file from config.h.in
#---------------------------------------------------------------------
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/src/neyson/config.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/gen/neyson/config.h"
  @ONLY
)

#---------------------------------------------------------------------
# Gather Source and Header Files
#---------------------------------------------------------------------
file(GLOB_RECURSE NEYSON_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/src/neyson/*.cpp"
)
file(GLOB_RECURSE NEYSON_PUBLIC_HEADERS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/neyson/*.h"
)

#---------------------------------------------------------------------
# Create the Library Target
#---------------------------------------------------------------------
add_library(neyson ${NEYSON_SOURCES} ${NEYSON_PUBLIC_HEADERS})
target_include_directories(neyson
  PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/gen>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/3rd/rapidxml>"
    "$<INSTALL_INTERFACE:include>"
)
target_compile_features(neyson PUBLIC cxx_std_11)
add_library(neyson::neyson ALIAS neyson)

#---------------------------------------------------------------------
# Link SQLite and add compile definition if enabled
#---------------------------------------------------------------------
if(NEYSON_ENABLE_SQLITE)
  target_link_libraries(neyson PRIVATE SQLite::SQLite3)
  target_compile_definitions(neyson PUBLIC NEYSON_USE_SQLITE)
endif()

#---------------------------------------------------------------------
# Add the tests target if enabled
#---------------------------------------------------------------------
if(NEYSON_BUILD_TESTS)
    file(GLOB TESTS_SOURCES test/*)
    add_executable(tests ${TESTS_SOURCES})
    target_link_libraries(tests PUBLIC neyson)
endif()

#---------------------------------------------------------------------
# Installation Rules
#---------------------------------------------------------------------
include(GNUInstallDirs)
install(TARGETS neyson
  EXPORT neysonTargets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
# Install public headers from the source directory
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/neyson"
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  FILES_MATCHING PATTERN "*.h"
)
# Install the generated config.h header separately
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/gen/neyson/config.h"
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/neyson
)
# Export the library target for downstream use
install(EXPORT neysonTargets
  FILE neysonTargets.cmake
  NAMESPACE neyson::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/neyson
)
