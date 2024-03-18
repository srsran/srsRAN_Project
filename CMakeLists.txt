#
# Copyright 2021-2024 Software Radio Systems Limited
#
# This file is part of srsRAN
#
# srsRAN is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsRAN is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
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
cmake_minimum_required(VERSION 3.14)
project(srsran)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/modules")
include(version) # sets version information

if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Release)
   message(STATUS "Build type not specified: defaulting to Release.")
endif(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "")

# Generate CMake to include build information
configure_file(
  ${PROJECT_SOURCE_DIR}/cmake/modules/build_info.cmake.in
  ${CMAKE_BINARY_DIR}/build_info.cmake
)

########################################################################
# Options
########################################################################

option(STOP_ON_WARNING       "Interrupt application on warning"         OFF)
option(ENABLE_WERROR         "Stop compilation on errors"               ON)
option(ENABLE_TSAN           "Enable clang thread sanitizer"            OFF)
option(ENABLE_ASAN           "Enable clang address sanitizer"           OFF)
option(ENABLE_GCOV           "Enable code coverage"                     OFF)
option(ENABLE_UHD            "Enable UHD"                               ON)
option(ENABLE_ZEROMQ         "Enable ZeroMQ"                            OFF)
option(ENABLE_FFTW           "Enable FFTW"                              ON)
option(ENABLE_DPDK           "Enable DPDK"                              OFF)
option(ENABLE_LIBNUMA        "Enable LibNUMA"                           OFF)
option(ENABLE_EXPORT         "Enable PIC and export libraries"          OFF)
option(ENABLE_TRX_DRIVER     "Enable Amarisoft TRX driver library"      OFF)
option(AUTO_DETECT_ISA       "Enable automatic ISA detection"           ON)
option(BUILD_TESTS           "Compile tests"                            ON)
option(ENABLE_GPROF          "Enable gprof"                             OFF)
option(USE_PHY_TESTVECTORS   "Enable testvector PHY tests"              OFF)

# Set assertion level options and default value.
set(ASSERT_LEVEL "AUTO" CACHE STRING "Assertion paranoia level")
set_property(CACHE ASSERT_LEVEL PROPERTY STRINGS AUTO MINIMAL NORMAL PARANOID)

# Set maximum time to wait for a clean exit.
set(EXIT_TIMEOUT "AUTO" CACHE STRING "Timer to wait for a clean exit")

if (ENABLE_WERROR)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
    # Disable gcc's maybe uninitialized analysis as it raises false positives
    if (CMAKE_COMPILER_IS_GNUCXX)
        include(CheckCXXCompilerFlag)
        check_cxx_compiler_flag("-Wmaybe-uninitialized" HAS_MAYBE_UNINITIALIZED)
        if (HAS_MAYBE_UNINITIALIZED)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=maybe-uninitialized")
        endif()
    endif()
endif ()

if (ENABLE_ASAN AND ENABLE_TSAN)
    message(FATAL_ERROR "ASAN and TSAN cannot be enabled at the same time.")
endif ()

if (ENABLE_EXPORT)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif (ENABLE_EXPORT)

# Hardware acceleration for both PUSCH and PDSCH is enabled by default when using DPDK.
if (ENABLE_DPDK)
  # Make sure that library export is disabed, as it is not compatible with hardware acceleration.
  if (NOT ENABLE_EXPORT)
    SET(ENABLE_PDSCH_HWACC ON CACHE BOOL "Enable PDSCH hardware-acceleration")
    message(STATUS "Hardware-acceleration enabled for PDSCH.")
    SET(ENABLE_PUSCH_HWACC ON CACHE BOOL "Enable PUSCH hardware-acceleration")
    message(STATUS "Hardware-acceleration enabled for PUSCH.")
  endif ()
else (ENABLE_DPDK)
    unset(ENABLE_PDSCH_HWACC CACHE)
    unset(ENABLE_PUSCH_HWACC CACHE)
endif (ENABLE_DPDK)

########################################################################
# Install Dirs
########################################################################

set(DATA_DIR share/${CMAKE_PROJECT_NAME})

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
ADD_CXX_COMPILER_FLAG_IF_AVAILABLE(-Wshadow HAVE_SHADOW)

# Set compiler flags for different build types.
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb -O0 -DDEBUG_MODE -DBUILD_TYPE_DEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -ggdb -DBUILD_TYPE_RELWITHDEBINFO")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fno-trapping-math -fno-math-errno -DBUILD_TYPE_RELEASE")

if(ENABLE_GPROF)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
endif(ENABLE_GPROF)

if (NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  if (HAVE_SSE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Ofast -funroll-loops")
  endif (HAVE_SSE)
endif (NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug")

if (ENABLE_ASAN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
    # Note: When using ASAN, we need to ensure the use of RPATH instead of RUNPATH via "-Wl,--disable-new-dtags"
    # While RPATH is default, some systems (e.g. Ubuntu 18.04 and 20.04) use RUNPATH by default, which is non-transitive.
    # Since ASAN intercepts dlopen(), by which it replaces the dynamic string token "$ORIGIN" to its own location,
    # the RF plugins won't be found when using ASAN + RUNPATH in the top-level executable.
    ADD_CXX_COMPILER_FLAG_IF_AVAILABLE("-Wl,--disable-new-dtags" HAVE_RPATH_FORCE)
endif (ENABLE_ASAN)

if (ENABLE_TSAN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")
    add_definitions(-DENABLE_TSAN)
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

if(NOT EXIT_TIMEOUT STREQUAL "AUTO")
    message(STATUS "Manually set exit timeout. timeout=${EXIT_TIMEOUT}s")
    add_compile_definitions(TERM_TIMEOUT_S=${EXIT_TIMEOUT})
endif()

########################################################################
# Find dependencies
########################################################################

# Backward-cpp
find_package(Backward)
if(Backward_FOUND)
    if(BACKWARD_HAS_EXTERNAL_LIBRARIES)
        message(STATUS "Building with backward-cpp support")
    else (BACKWARD_HAS_EXTERNAL_LIBRARIES)
        message(STATUS "Backward-cpp found, but external libraries are missing.")
    endif()
endif()

# Crypto
find_package(MbedTLS REQUIRED)
if (MBEDTLS_FOUND)
    set(SEC_INCLUDE_DIRS "${MBEDTLS_INCLUDE_DIRS}")
    if(BUILD_STATIC)
        set(SEC_LIBRARIES "${MBEDTLS_STATIC_LIBRARIES}")
    else(BUILD_STATIC)
        set(SEC_LIBRARIES "${MBEDTLS_LIBRARIES}")
    endif(BUILD_STATIC)
else(MBEDTLS_FOUND)
    message(FATAL_ERROR "mbedTLS is required to build ${CMAKE_PROJECT_NAME}")
endif (MBEDTLS_FOUND)

# FFTW
if (ENABLE_FFTW)
    find_package(FFTW3F)
endif (ENABLE_FFTW)

# Google Tests
if (BUILD_TESTS)
    find_package(GTest REQUIRED)

    # Alias gtest_discover_tests increase discovery the timeout.
    function(gtest_discover_tests)
        _gtest_discover_tests(${ARGV} DISCOVERY_TIMEOUT 15)
    endfunction()
endif (BUILD_TESTS)

# Threads
find_package(Threads REQUIRED)

# UHD
if (ENABLE_UHD)
    find_package(UHD)
    if (UHD_FOUND)
        include_directories(${UHD_INCLUDE_DIRS})
        link_directories(${UHD_LIBRARY_DIRS})
    endif (UHD_FOUND)
else (ENABLE_UHD)
    unset(UHD_FOUND CACHE)
endif (ENABLE_UHD)

# Yaml-cpp
find_package(YAMLCPP)
if (YAMLCPP_FOUND)
    include_directories(${YAMLCPP_INCLUDE_DIR})
    link_directories(${YAMLCPP_LIBRARY})
else (YAMLCPP_FOUND)
    message(FATAL_ERROR "yaml-cpp is required to build ${CMAKE_PROJECT_NAME}")
endif (YAMLCPP_FOUND)

# ZeroMQ
if (ENABLE_ZEROMQ)
    find_package(ZeroMQ)
    if (ZEROMQ_FOUND)
        include_directories(${ZEROMQ_INCLUDE_DIRS})
        link_directories(${ZEROMQ_LIBRARY_DIRS})
    endif (ZEROMQ_FOUND)
else (ENABLE_ZEROMQ)
    unset(ZEROMQ_FOUND CACHE)
endif (ENABLE_ZEROMQ)

if (ENABLE_LIBNUMA)
    find_package(NUMA)
    if (NUMA_FOUND)
        include_directories(${NUMA_INCLUDE_DIRS})
        add_definitions(-DNUMA_SUPPORT)
    endif (NUMA_FOUND)
endif (ENABLE_LIBNUMA)

# DPDK
if (ENABLE_DPDK)
    set(DPDK_MIN_VERSION "22.11")
    find_package(DPDK ${DPDK_MIN_VERSION})
    if (DPDK_FOUND)
        include_directories(SYSTEM ${DPDK_INCLUDE_DIRS})
    endif (DPDK_FOUND)
else (ENABLE_DPDK)
    unset(DPDK_FOUND CACHE)
endif (ENABLE_DPDK)

########################################################################
# Instruction Set Architecture setup
########################################################################
if (AUTO_DETECT_ISA)
    if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64")
        set(GCC_ARCH armv8-a CACHE STRING "GCC compile for specific architecture.")
        message(STATUS "Detected aarch64 processor")
        add_definitions(-DHAVE_NEON)
        set(HAVE_NEON True CACHE BOOL "NEON Instruction set is supported")
    else (${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64")
        set(GCC_ARCH native CACHE STRING "GCC compile for specific architecture.")
    endif (${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64")

    find_package(SSE)

    # Add default architecture if enabled and available.
    ADD_CXX_COMPILER_FLAG_IF_AVAILABLE("-march=${GCC_ARCH}" HAVE_MARCH)

    if (HAVE_AVX2)
        add_definitions(-DHAVE_AVX2)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2")
    endif (HAVE_AVX2)
    if (HAVE_AVX)
        add_definitions(-DHAVE_AVX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx")
    endif (HAVE_AVX)
    if (HAVE_SSE)
        add_definitions(-DHAVE_SSE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.1")
    endif (HAVE_SSE)
    if (HAVE_FMA)
        add_definitions(-DHAVE_FMA)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfma")
    endif (HAVE_FMA)
    if (HAVE_AVX512)
        add_definitions(-DHAVE_AVX512)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx512f -mavx512cd -mavx512bw -mavx512dq")
    endif (HAVE_AVX512)
else (AUTO_DETECT_ISA)
    unset(HAVE_SSE CACHE)
    unset(HAVE_AVX CACHE)
    unset(HAVE_AVX2 CACHE)
    unset(HAVE_FMA CACHE)
    unset(HAVE_AVX512 CACHE)
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
execute_process(COMMAND sed -i "s|MemoryCheckCommandOptions: |MemoryCheckCommandOptions: --verbose --trace-children=yes --time-stamp=yes --leak-check=full --show-leak-kinds=all --show-reachable=yes --exit-on-first-error=yes --error-exitcode=22 --suppressions=${CMAKE_SOURCE_DIR}/.memcheck-suppressions|" DartConfiguration.tcl
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

include_directories(include)
include_directories(external/fmt/include)
include_directories(external)

########################################################################
# Add headers to cmake project (useful for IDEs)
########################################################################

# List of directories that contain header files
set(ALL_HEADER_DIRS
  apps
  lib
  include
  tests/unittests
)
set(ALL_HEADER_FILES "")
foreach(TMP_DIR ${ALL_HEADER_DIRS})
  file(GLOB_RECURSE TMP_HEADERS "${TMP_DIR}/*.h")
  list(APPEND ALL_HEADER_FILES ${TMP_HEADERS})
endforeach()
add_custom_target(all_srsran_headers SOURCES ${ALL_HEADER_FILES})

########################################################################
# Add the subdirectories
########################################################################

add_subdirectory(apps)
add_subdirectory(configs)
add_subdirectory(docs)
add_subdirectory(external)
add_subdirectory(lib)
add_subdirectory(utils)

if (BUILD_TESTS)
    add_subdirectory(tests/test_doubles)
    add_subdirectory(tests/unittests)
    add_subdirectory(tests/integrationtests)
    add_subdirectory(tests/benchmarks)
endif (BUILD_TESTS)

########################################################################
# Export (selected) libraries
########################################################################
if(ENABLE_EXPORT)
  export(EXPORT srsran_export NAMESPACE srsran:: FILE "${CMAKE_BINARY_DIR}/srsran.cmake")
else(ENABLE_EXPORT)
  # Remove any previous export file (if it exists), since it will become outdated.
  file(REMOVE "${CMAKE_BINARY_DIR}/srsran.cmake")
endif(ENABLE_EXPORT)

message(STATUS "Building srsRAN version ${SRSRAN_VERSION_STRING}")
