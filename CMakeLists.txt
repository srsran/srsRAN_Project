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

option(STOP_ON_WARNING       "Interrupt application on warning"         OFF)
option(ENABLE_TSAN           "Enable clang thread sanitizer"            OFF)
option(ENABLE_ASAN           "Enable clang address sanitizer"           OFF)
option(ENABLE_GCOV           "Enable code coverage"                     OFF)
option(ENABLE_UHD            "Enable UHD"                               ON)
option(ENABLE_ZEROMQ         "Enable ZeroMQ"                            ON)
option(ENABLE_FFTW           "Enable FFTW"                              ON)
option(ENABLE_EXPORT         "Enable PIC and export libraries"          OFF)
option(AUTO_DETECT_ISA       "Enable automatic ISA detection"           ON)
option(BUILD_TESTS           "Compile tests"                            ON)
# Set assertion level options and default value.
set(ASSERT_LEVEL "AUTO" CACHE STRING "Assertion paranoia level")
set_property(CACHE ASSERT_LEVEL PROPERTY STRINGS AUTO MINIMAL NORMAL PARANOID)

if (ENABLE_ASAN AND ENABLE_TSAN)
    message(FATAL_ERROR "ASAN and TSAN cannot be enabled at the same time.")
endif ()

if (ENABLE_EXPORT)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif (ENABLE_EXPORT)

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

if(NOT ASSERT_LEVEL STREQUAL "MINIMAL")
    # asserts are enabled.
    add_definitions(-DASSERTS_ENABLED)
    if((ASSERT_LEVEL STREQUAL "PARANOID") OR
       ((ASSERT_LEVEL STREQUAL "AUTO") AND (${CMAKE_BUILD_TYPE} STREQUAL "Debug")))
        add_definitions(-DPARANOID_ASSERTS_ENABLED)
        message(STATUS "Assertion level set to PARANOID")
    else()
        message(STATUS "Assertion level set to NORMAL")
    endif()
endif()

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

# Google Tests
if (BUILD_TESTS)
    find_package(GTest REQUIRED)
endif (BUILD_TESTS)

########################################################################
# Instruction Set Architecture setup
########################################################################
if (AUTO_DETECT_ISA)
    if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64")
        set(GCC_ARCH armv8-a CACHE STRING "GCC compile for specific architecture.")
        message(STATUS "Detected aarch64 processor")
    else (${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64")
        set(GCC_ARCH native CACHE STRING "GCC compile for specific architecture.")
    endif (${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64")

    find_package(SSE)

    ADD_CXX_COMPILER_FLAG_IF_AVAILABLE("-march=${GCC_ARCH}" HAVE_MARCH_${GCC_ARCH})
    if (HAVE_AVX2)
        add_definitions(-DHAVE_AVX2)
    endif (HAVE_AVX2)
    if (HAVE_AVX)
        add_definitions(-DHAVE_AVX)
    endif (HAVE_AVX)
    if (HAVE_SSE)
        add_definitions(-DHAVE_SSE)
    endif (HAVE_SSE)
    if (HAVE_FMA)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfma -DHAVE_FMA")
    endif (HAVE_FMA)
    if (HAVE_AVX512)
        add_definitions(-DHAVE_AVX512)
    endif (HAVE_AVX512)
endif (AUTO_DETECT_ISA)

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

if (BUILD_TESTS)
    add_subdirectory(unittests)
    add_subdirectory(integrationtests)
    add_subdirectory(benchmarks)
endif (BUILD_TESTS)

########################################################################
# Export (selected) libraries
########################################################################
if(ENABLE_EXPORT)
  export(EXPORT srsgnb_export NAMESPACE srsgnb:: FILE "${CMAKE_BINARY_DIR}/srsgnb.cmake")
else(ENABLE_EXPORT)
  # Remove any previous export file (if it exists), since it will become outdated.
  file(REMOVE "${CMAKE_BINARY_DIR}/srsgnb.cmake")
endif(ENABLE_EXPORT)
