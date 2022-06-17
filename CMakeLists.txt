#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

########################################################################
# Prevent in-tree builds
########################################################################
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "Prevented in-tree build. This is bad practice.")
endif(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})

########################################################################
# Project setup
########################################################################
cmake_minimum_required(VERSION 3.10)
project(srsgnb)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/modules")

if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Release)
   message(STATUS "Build type not specified: defaulting to Release.")
endif(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "")

########################################################################
# Options
########################################################################

option(ASSERTS_ENABLED       "Enable srsRAN asserts"                    ON)
option(STOP_ON_WARNING       "Interrupt application on warning"         OFF)
option(ENABLE_TSAN           "Enable clang thread sanitizer"            OFF)
option(ENABLE_ASAN           "Enable clang address sanitizer"           OFF)
option(ENABLE_GCOV           "Enable code coverage"                     OFF)
option(ENABLE_UHD            "Enable UHD"                               ON)
option(ENABLE_ZEROMQ         "Enable ZeroMQ"                            ON)
option(ENABLE_FFTW           "Enable FFTW"                              ON)

if (ASSERTS_ENABLED)
    add_definitions(-DASSERTS_ENABLED)
endif()

if (ENABLE_ASAN AND ENABLE_TSAN)
    message(FATAL_ERROR "ASAN and TSAN cannot be enabled at the same time.")
endif ()

########################################################################
# Compiler specific setup
########################################################################
macro(ADD_CXX_COMPILER_FLAG_IF_AVAILABLE flag have)
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag(${flag} ${have})
    if (${have})
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}")
    endif (${have})
endmacro(ADD_CXX_COMPILER_FLAG_IF_AVAILABLE)

# Make sure no instance of abstract class is left without a destructor
ADD_CXX_COMPILER_FLAG_IF_AVAILABLE(-Wnon-virtual-dtor HAVE_NON_VIRTUAL_DTOR)

# Make sure all overridden methods are marked as override
ADD_CXX_COMPILER_FLAG_IF_AVAILABLE(-Wsuggest-override HAVE_SUGGEST_OVERRIDE)

## Avoid shadow variables which can be caused due to C code ported into C++
#ADD_CXX_COMPILER_FLAG_IF_AVAILABLE(-Wshadow HAVE_SHADOW)

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb -O0 -DDEBUG_MODE -DBUILD_TYPE_DEBUG")
else(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  if(${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb -DBUILD_TYPE_RELWITHDEBINFO")
  else(${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -fno-trapping-math -fno-math-errno -DBUILD_TYPE_RELEASE")
  endif(${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
endif(${CMAKE_BUILD_TYPE} STREQUAL "Debug")

if (NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  if (HAVE_SSE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Ofast -funroll-loops")
  endif (HAVE_SSE)
endif (NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug")

if (ENABLE_ASAN)
    # Note: When using ASAN, we need to ensure the use of RPATH instead of RUNPATH via "-Wl,--disable-new-dtags"
    # While RPATH is default, some systems (e.g. Ubuntu 18.04 and 20.04) use RUNPATH by default, which is non-transitive.
    # Since ASAN intercepts dlopen(), by which it replaces the dynamic string token "$ORIGIN" to its own location,
    # the RF plugins won't be found when using ASAN + RUNPATH in the top-level executable.
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer -Wl,--disable-new-dtags")
endif (ENABLE_ASAN)

if (ENABLE_TSAN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")
endif (ENABLE_TSAN)

if (ENABLE_GCOV)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
endif (ENABLE_GCOV)

########################################################################
# Find dependencies
########################################################################

# FFTW
if (ENABLE_FFTW)
    find_package(FFTW3F)
endif (ENABLE_FFTW)

# Threads
find_package(Threads REQUIRED)

# UHD
if (ENABLE_UHD)
    find_package(UHD)
    if (UHD_FOUND)
        include_directories(${UHD_INCLUDE_DIRS})
        link_directories(${UHD_LIBRARY_DIRS})
    endif (UHD_FOUND)
endif (ENABLE_UHD)

# ZeroMQ
if (ENABLE_ZEROMQ)
    find_package(ZeroMQ)
    if (ZEROMQ_FOUND)
        include_directories(${ZEROMQ_INCLUDE_DIRS})
        link_directories(${ZEROMQ_LIBRARY_DIRS})
    endif (ZEROMQ_FOUND)
endif (ENABLE_ZEROMQ)

########################################################################
# Compiler launcher setup
########################################################################

# Enable ccache if not already enabled
find_program(CCACHE_EXECUTABLE ccache)
mark_as_advanced(CCACHE_EXECUTABLE)
if (CCACHE_EXECUTABLE)
    foreach (LANG C CXX)
        if (NOT DEFINED CMAKE_${LANG}_COMPILER_LAUNCHER AND NOT CMAKE_${LANG}_COMPILER MATCHES ".*/ccache$")
            message(STATUS "Enabling ccache for ${LANG}")
            set(CMAKE_${LANG}_COMPILER_LAUNCHER ${CCACHE_EXECUTABLE} CACHE STRING "")
        endif ()
    endforeach ()
endif ()

include(CTest)

include_directories(include)

########################################################################
# Add headers to cmake project (useful for IDEs)
########################################################################

# List of directories that contain header files
set(ALL_HEADER_DIRS
  apps
  lib
  include
  unittests
)
set(ALL_HEADER_FILES "")
foreach(TMP_DIR ${ALL_HEADER_DIRS})
  file(GLOB_RECURSE TMP_HEADERS "${TMP_DIR}/*.h")
  list(APPEND ALL_HEADER_FILES ${TMP_HEADERS})
endforeach()
add_custom_target(all_srsgnb_headers SOURCES ${ALL_HEADER_FILES})

########################################################################
# Add the subdirectories
########################################################################

add_subdirectory(apps)
add_subdirectory(docs)
add_subdirectory(lib)
add_subdirectory(unittests)
add_subdirectory(integrationtests)
