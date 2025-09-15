#
# Copyright 2021-2025 Software Radio Systems Limited
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
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "Prevented in-tree build. This is bad practice.")
endif (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})

########################################################################
# Project setup
########################################################################
cmake_minimum_required(VERSION 3.14)
project(srsran)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")


list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/modules")
include(version) # sets version information

# Configure supported build types and default build type
set(supported_build_types "Release" "RelWithDebInfo" "Debug")
set(default_build_type "Release")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Build type not specified, using default: ${default_build_type}")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Select build type" FORCE)
else()
  # Check if build type is supported
  if(NOT "${CMAKE_BUILD_TYPE}" IN_LIST supported_build_types)
    message(SEND_ERROR "Unsupported build type '${CMAKE_BUILD_TYPE}'. Supported build types: ${supported_build_types}")
  else()
    message(STATUS "The build type is ${CMAKE_BUILD_TYPE}")
  endif()
endif()

# Add possible values for build type in cmake-gui (drop-down widget, for convenience only)
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${supported_build_types})

# Generate CMake to include build information
configure_file(
  ${PROJECT_SOURCE_DIR}/cmake/modules/build_info.cmake.in
  ${CMAKE_BINARY_DIR}/build_info.cmake
)

########################################################################
# Options
########################################################################

option(STOP_ON_WARNING       "Interrupt application on warning"           OFF)
option(ENABLE_WERROR         "Stop compilation on errors"                 ON)
option(ENABLE_TSAN           "Enable clang thread sanitizer"              OFF)
option(ENABLE_ASAN           "Enable clang address sanitizer"             OFF)
option(ENABLE_UBSAN          "Enable clang undefined behaviour sanitizer" OFF)
option(ENABLE_UBSAN_MIN      "Enable clang sanitizer minimal runtime"     OFF)
option(ENABLE_RTSAN          "Enable clang real-time sanitizer"           OFF)
option(ENABLE_GCOV           "Enable code coverage"                       OFF)
option(ENABLE_BACKWARD       "Enable backward"                            ON)
option(ENABLE_UHD            "Enable UHD"                                 ON)
option(ENABLE_ZEROMQ         "Enable ZeroMQ"                              OFF)
option(ENABLE_FFTW           "Enable FFTW"                                ON)
option(ENABLE_MKL            "Enable Intel MKL"                           ON)
option(ENABLE_ARMPL          "Enable ARM performance library"             ON)
option(ENABLE_DPDK           "Enable DPDK"                                OFF)
option(ENABLE_LIBNUMA        "Enable LibNUMA"                             OFF)
option(ENABLE_EXPORT         "Enable PIC and export libraries"            OFF)
option(ENABLE_TRX_DRIVER     "Enable Amarisoft TRX driver library"        OFF)
option(ENABLE_PLUGINS        "Compile plugins in the plugin folder"       ON)
option(BUILD_TESTING         "Compile tests"                              OFF)
option(ENABLE_GPROF          "Enable gprof"                               OFF)
option(FORCE_DEBUG_INFO      "Add debug information to Release build"     OFF)
if (BUILD_TESTING)
option(USE_PHY_TESTVECTORS   "Enable testvector PHY tests"                OFF)
endif(BUILD_TESTING)

# Set linker options and default value.
set(LINKER "default" CACHE STRING "Which linker to use")
set_property(CACHE LINKER PROPERTY STRINGS default bfd lld)

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
        check_cxx_compiler_flag("-Wstringop-overflow" HAS_STRINGOP_OVERFLOW)
        if (HAS_STRINGOP_OVERFLOW)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=stringop-overflow")
        endif()
        if (ASSERT_LEVEL STREQUAL "MINIMAL")
            check_cxx_compiler_flag("-Warray-bounds" HAS_ARRAY_BOUNDS)
            if (HAS_ARRAY_BOUNDS)
                set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=array-bounds")
            endif()
        endif()
    endif()
endif ()

if (ENABLE_ASAN AND ENABLE_TSAN)
    message(FATAL_ERROR "ASAN and TSAN cannot be enabled at the same time.")
endif ()

# Hardware acceleration for both PUSCH and PDSCH is enabled by default when using DPDK.
if (ENABLE_DPDK)
  SET(ENABLE_PDSCH_HWACC ON CACHE BOOL "Enable PDSCH hardware-acceleration")
  SET(ENABLE_PUSCH_HWACC ON CACHE BOOL "Enable PUSCH hardware-acceleration")
else (ENABLE_DPDK)
  unset(ENABLE_PDSCH_HWACC CACHE)
  unset(ENABLE_PUSCH_HWACC CACHE)
endif (ENABLE_DPDK)

########################################################################
# ENABLE_EXPORT
########################################################################

# ENABLE_EXPORT tells cmake to make some libaries available for other
# software to link to. If ON, the code must be compiled in PIC mode.
# We also add a dummy target that will depend on all the exported
# libraries to simplify their compilation.
if (ENABLE_EXPORT)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
  add_custom_target(srsran_exported_libs)
endif (ENABLE_EXPORT)

# Simple macro that tags libraries to be exported and adds them to the
# dependencies of the dummy target.
macro(ADD_TO_EXPORTED_LIBS)
  if(ENABLE_EXPORT)
    # Tag libraries.
    install(TARGETS ${ARGV} EXPORT srsran_export)
    # Make libraries dependencies of the srsran_exported_libs dummy
    # target, which can be called to compile all exported libraries
    # at once.
    add_dependencies(srsran_exported_libs ${ARGV})
  endif(ENABLE_EXPORT)
endmacro(ADD_TO_EXPORTED_LIBS)

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

# Avoid shadow variables which can be caused due to C code ported into C++
ADD_CXX_COMPILER_FLAG_IF_AVAILABLE(-Wshadow HAVE_SHADOW)

# Avoid extra (stray) semicolons
ADD_CXX_COMPILER_FLAG_IF_AVAILABLE(-Wextra-semi HAVE_EXTRA_SEMI)

# Disable RTTI
ADD_CXX_COMPILER_FLAG_IF_AVAILABLE(-fno-rtti HAVE_NO_RTTI)

# Set compiler flags for different build types.
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb -O0 -DDEBUG_MODE -DBUILD_TYPE_DEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -ggdb -DBUILD_TYPE_RELWITHDEBINFO")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fno-trapping-math -fno-math-errno -DBUILD_TYPE_RELEASE")

if(ENABLE_GPROF)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
endif(ENABLE_GPROF)

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

if (ENABLE_UBSAN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined -fno-sanitize=vptr")
    add_definitions(-DENABLE_UBSAN)
endif ()

if (ENABLE_UBSAN_MIN)
    if (NOT ENABLE_UBSAN)
        message(FATAL_ERROR "Trying to use minimal runtime without UBSAN enabled.")
    endif ()
    if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	message(FATAL_ERROR "Must use Clang for enabling sanitizer minimal runtime.")
    endif()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize-minimal-runtime")
    add_definitions(-DENABLE_UBSAN_MIN)
endif ()

if (ENABLE_RTSAN)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=realtime")
    ADD_CXX_COMPILER_FLAG_IF_AVAILABLE("-fsanitize=realtime" HAVE_RPATH_FORCE)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=realtime")
endif ()

if (FORCE_DEBUG_INFO)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif (FORCE_DEBUG_INFO)

if (ENABLE_GCOV)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
endif (ENABLE_GCOV)

if(NOT LINKER STREQUAL "default")
    if (LINKER STREQUAL "bfd")
        add_link_options(-fuse-ld=bfd)
        message(STATUS "Linker set to BFD")
    elseif (LINKER STREQUAL "lld")
        add_link_options(-fuse-ld=lld)
        message(STATUS "Linker set to LLD")
    else ()
        message(FATAL_ERROR "Unsupported linker ${LINKER} for compiler ${CMAKE_CXX_COMPILER_ID}")
    endif ()
endif ()

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
# Create SBOM project
########################################################################
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/external/cmake-sbom")
include(sbom)
include(copyright)
include(GNUInstallDirs)

sbom_generate(
    OUTPUT       "${CMAKE_INSTALL_PREFIX}/share/srsran.spdx"
    PROJECT      "srsRAN 5G CU DU"
    SUPPLIER     "Software Radio Systems Ltd"
    SUPPLIER_URL "https://srs.io"
    DOWNLOAD_URL "https://github.com/srsran/srsRAN_Project.git"
    LICENSE      "LicenseRef-srsRAN"
)

########################################################################
# Find dependencies
########################################################################

# Backward-cpp
if (ENABLE_BACKWARD)
    find_package(Backward)
    # Define macro for adding backwards to a target.
    macro(add_backward TARGET)
        target_include_directories(${TARGET} PRIVATE ${BACKWARD_INCLUDE_DIRS})
        set_property(TARGET ${TARGET} APPEND PROPERTY COMPILE_DEFINITIONS ${BACKWARD_DEFINITIONS})
        target_link_libraries(${TARGET} PRIVATE ${BACKWARD_LIBRARIES})
    endmacro(add_backward TARGET)
    if(BACKWARD_FOUND)
        if(BACKWARD_HAS_EXTERNAL_LIBRARIES)
            message(STATUS "Building with backward-cpp support")
        else (BACKWARD_HAS_EXTERNAL_LIBRARIES)
            message(STATUS "Backward-cpp found, but external libraries are missing.")
        endif()
    endif()
    if (LIBDW_FOUND)
        sbom_add(
            PACKAGE            "libdw"
            DOWNLOAD_LOCATION  "https://sourceware.org/elfutils/"
            SUPPLIER           "elfutils project"
            LICENSE            "GPL-2.0-or-later"
            VERSION            ${LIBDW_VERSION}
        )
	elseif(LIBBFD_FOUND)
        sbom_add(
            PACKAGE            "libbfd"
            DOWNLOAD_LOCATION  "https://sourceware.org/elfutils/"
            SUPPLIER           "elfutils project"
            LICENSE            "GPL-2.0-or-later"
            VERSION            ${LIBBFD_VERSION}
        )
	elseif(LIBDWARF_FOUND)
        sbom_add(
            PACKAGE            "libdwarf"
            DOWNLOAD_LOCATION  "https://sourceware.org/elfutils/"
            SUPPLIER           "elfutils project"
            LICENSE            "GPL-2.0-or-later"
            VERSION            ${LIBDWARF_VERSION}
        )
    endif()
else ()
    # Empty macro to add backward when it is not enabled.
    macro(add_backward)
    endmacro()
endif ()

# Crypto
find_package(MbedTLS REQUIRED)
if (MBEDTLS_FOUND)
    set(SEC_INCLUDE_DIRS "${MBEDTLS_INCLUDE_DIRS}")
    if(BUILD_STATIC)
        set(SEC_LIBRARIES "${MBEDTLS_STATIC_LIBRARIES}")
    else(BUILD_STATIC)
        set(SEC_LIBRARIES "${MBEDTLS_LIBRARIES}")
    endif(BUILD_STATIC)
    sbom_add(
        PACKAGE            "mbedTLS"
        DOWNLOAD_LOCATION  "https://tls.mbed.org"
        SUPPLIER           "MbedTLS project"
        LICENSE            "Apache-2.0"
        VERSION            ${PC_MBEDTLS_VERSION}
    )
else(MBEDTLS_FOUND)
    message(FATAL_ERROR "mbedTLS is required to build ${CMAKE_PROJECT_NAME}")
endif (MBEDTLS_FOUND)

# FFTW
if (ENABLE_FFTW)
    find_package(FFTW3F)
    if(FFTW3F_FOUND)
        sbom_add(
            PACKAGE            "FFTW3"
            DOWNLOAD_LOCATION  "http://www.fftw.org"
            SUPPLIER           "FFTW project"
            LICENSE            "GPL-2.0-or-later OR LGPL-2.1-or-later"
            VERSION            ${PC_FFTW3F_VERSION}
        )
    endif(FFTW3F_FOUND)
endif (ENABLE_FFTW)

# MKL
if (ENABLE_MKL AND ${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
    find_package(MKL)
    if(MKL_FOUND)
        sbom_add(
            PACKAGE            "Intel MKL"
            DOWNLOAD_LOCATION  "https://www.intel.com/content/www/us/en/developer/tools/oneapi/onemkl.html"
            SUPPLIER           "Intel Corporation"
            LICENSE            "Apache-2.0"
            VERSION            ${MKL_VERSION}
        )
    endif(MKL_FOUND)
endif (ENABLE_MKL AND ${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")

# ARMPL
if (ENABLE_ARMPL AND ${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64")
    find_package(ARMPL)
    if(ARMPL_FOUND)
        sbom_add(
            PACKAGE            "ARM Performance Libraries"
            DOWNLOAD_LOCATION  "https://developer.arm.com/Tools%20and%20Software/Arm%20Performance%20Libraries#Downloads"
            SUPPLIER           "Arm Ltd."
            LICENSE            "Apache-2.0"
            VERSION            ${ARMPL_VERSION}
        )
    endif(ARMPL_FOUND)
endif (ENABLE_ARMPL AND ${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64")

# Google Tests
if (BUILD_TESTING)
    find_package(GTest REQUIRED)
    # Alias gtest_discover_tests increase discovery the timeout.
    function(gtest_discover_tests)
        _gtest_discover_tests(
            ${ARGV}
            DISCOVERY_MODE PRE_TEST
            DISCOVERY_TIMEOUT 15
        )
    endfunction()
else()
    message(STATUS "Skipping Tests. Set BUILD_TESTING to On to compile tests.")
endif (BUILD_TESTING)

# Threads
find_package(Threads REQUIRED)

# UHD
if (ENABLE_UHD)
    find_package(UHD)
    if (UHD_FOUND)
        include_directories(${UHD_INCLUDE_DIRS})
        link_directories(${UHD_LIBRARY_DIRS})
        sbom_add(
            PACKAGE            "UHD"
            DOWNLOAD_LOCATION  "https://github.com/EttusResearch/uhd"
            SUPPLIER           "Ettus Research, a National Instruments Brand"
            LICENSE            "GPL-3.0-or-later"
            VERSION            ${UHD_VERSION}
        )
    endif (UHD_FOUND)
else (ENABLE_UHD)
    unset(UHD_FOUND CACHE)
endif (ENABLE_UHD)

# Yaml-cpp
find_package(YAMLCPP)
if (YAMLCPP_FOUND)
    include_directories(${YAMLCPP_INCLUDE_DIR})
    link_directories(${YAMLCPP_LIBRARY})
    sbom_add(
        PACKAGE            "yaml-cpp"
        DOWNLOAD_LOCATION  "https://github.com/jbeder/yaml-cpp"
        SUPPLIER           "yaml-cpp project"
        LICENSE            "MIT"
        VERSION            ${PC_YAMLCPP_VERSION}
    )
else (YAMLCPP_FOUND)
    message(FATAL_ERROR "yaml-cpp is required to build ${CMAKE_PROJECT_NAME}")
endif (YAMLCPP_FOUND)

# ZeroMQ
if (ENABLE_ZEROMQ)
    find_package(ZeroMQ)
    if (ZEROMQ_FOUND)
        include_directories(${ZEROMQ_INCLUDE_DIRS})
        link_directories(${ZEROMQ_LIBRARY_DIRS})
        sbom_add(
            PACKAGE            "ZeroMQ"
            DOWNLOAD_LOCATION  "https://zeromq.org/"
            SUPPLIER           "ZeroMQ project"
            LICENSE            "LGPL-3.0-or-later"
            VERSION            ${ZEROMQ_PKG_VERSION}
        )
    endif (ZEROMQ_FOUND)
else (ENABLE_ZEROMQ)
    unset(ZEROMQ_FOUND CACHE)
endif (ENABLE_ZEROMQ)

if (ENABLE_LIBNUMA)
    find_package(NUMA)
    if (NUMA_FOUND)
        include_directories(${NUMA_INCLUDE_DIRS})
        add_definitions(-DNUMA_SUPPORT)
        sbom_add(
            PACKAGE            "LibNUMA"
            DOWNLOAD_LOCATION  "https://github.com/numactl/numactl"
            SUPPLIER           "NUMA project"
            LICENSE            "LGPL-2.1-or-later"
            VERSION            ${NUMA_VERSION}
        )
    endif (NUMA_FOUND)
endif (ENABLE_LIBNUMA)

# DPDK
if (ENABLE_DPDK)
    set(DPDK_MIN_VERSION "22.11")
    find_package(DPDK ${DPDK_MIN_VERSION})
    if (DPDK_FOUND)
        include_directories(SYSTEM ${DPDK_INCLUDE_DIRS})
        sbom_add(
            PACKAGE            "DPDK"
            DOWNLOAD_LOCATION  "https://www.dpdk.org/"
            SUPPLIER           "DPDK project"
            LICENSE            "BSD-3-Clause"
            VERSION            ${DPDK_VERSION}
        )
    else ()
        unset(DPDK_FOUND CACHE)
        message(WARNING "Building without DPDK support as DPDK dependencies could not be resolved")
    endif (DPDK_FOUND)
else (ENABLE_DPDK)
    unset(DPDK_FOUND CACHE)
endif (ENABLE_DPDK)

if(SCTP_FOUND)
    sbom_add(
        PACKAGE            "SCTP"
        DOWNLOAD_LOCATION  "https://www.sctp.org/"
        SUPPLIER           "SCTP project"
        LICENSE            "BSD-3-Clause"
        VERSION            ${PC_SCTP_VERSION}
    )
endif (SCTP_FOUND)

########################################################################
# Instruction Set Architecture setup
########################################################################
set(MTUNE "generic" CACHE STRING "Compiler -mtune flag. Default value is 'generic'")
if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64")
    set(MARCH "armv8-a" CACHE STRING "Compiler -march flag. Default value is 'armv8-a' for aarch64.")
    message(STATUS "Detected aarch64 processor")
else (${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64")
    set(MARCH "native" CACHE STRING "Compiler -march flag. Default value is 'native' for x86-64.")
endif (${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64")

add_cxx_compiler_flag_if_available("-march=${MARCH}" HAVE_MARCH)
if (NOT HAVE_MARCH)
    message(SEND_ERROR "The compiler does not support -march=${MARCH}, try setting a different value.")
endif (NOT HAVE_MARCH)

add_cxx_compiler_flag_if_available("-mtune=${MTUNE}" HAVE_MTUNE)
if (NOT HAVE_MTUNE)
    message(SEND_ERROR "The compiler does not support -mtune=${MTUNE}, try setting a different value.")
endif (NOT HAVE_MTUNE)

message(STATUS "ARCH value is ${MARCH}")
message(STATUS "TUNE value is ${MTUNE}")

# Append march and mtune to the compilation flags.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=${MARCH} -mtune=${MTUNE}")

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
  external
  lib
  include
  tests/unittests
)
set(ALL_HEADER_FILES "")
foreach(TMP_DIR ${ALL_HEADER_DIRS})
  file(GLOB_RECURSE TMP_HEADERS "${TMP_DIR}/*.h" "${TMP_DIR}/*.hpp")
  list(APPEND ALL_HEADER_FILES ${TMP_HEADERS})
endforeach()
add_custom_target(all_srsran_headers SOURCES ${ALL_HEADER_FILES})

########################################################################
# Simple function to notify binary targets + Add target to SBOM
########################################################################

function(notify_binary_target)
    message(STATUS "Adding binary target: ${ARGV}")
    sbom_add(
        TARGET       ${ARGV}
        LICENSE      "LicenseRef-srsRAN"
    )
endfunction()

########################################################################
# Add the subdirectories
########################################################################

add_subdirectory(apps)
add_subdirectory(configs)
add_subdirectory(docs)
add_subdirectory(external)
add_subdirectory(lib)
add_subdirectory(utils)

if (BUILD_TESTING)
    add_subdirectory(tests/test_doubles)
    add_subdirectory(tests/unittests)
    add_subdirectory(tests/integrationtests)
    add_subdirectory(tests/benchmarks)
endif (BUILD_TESTING)

if (ENABLE_PLUGINS)
    # Discover all subdirectories in the plugin directory (but not recursively)
    file(GLOB SUBDIRS RELATIVE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/plugins/*)

    # Loop through each plugin subdirectory
    foreach(subdir ${SUBDIRS})
        # Check if it's a directory and contains a CMakeLists.txt file
        if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/${subdir} AND EXISTS ${CMAKE_SOURCE_DIR}/${subdir}/CMakeLists.txt)
            message(STATUS "Adding plugin: ${subdir}")
            add_subdirectory(${subdir})
        endif()
    endforeach()
endif ()

########################################################################
# Export (selected) libraries
########################################################################
if(ENABLE_EXPORT)
  export(EXPORT srsran_export NAMESPACE srsran:: FILE "${CMAKE_BINARY_DIR}/srsran.cmake")
else(ENABLE_EXPORT)
  # Remove any previous export file (if it   exists), since it will become outdated.
  file(REMOVE "${CMAKE_BINARY_DIR}/srsran.cmake")
endif(ENABLE_EXPORT)

message(STATUS "Building srsRAN version ${SRSRAN_VERSION_STRING}")

########################################################################
# Generate SBOM
########################################################################
parse_copyright_file("${CMAKE_SOURCE_DIR}/COPYRIGHT")
sbom_add(
    LICENSE "LicenseRef-srsRAN"
    NAME    "AGPL-3.0-only"
    FILE    "${CMAKE_SOURCE_DIR}/LICENSE"
)
sbom_finalize(NO_VERIFY)
