#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

# Try to find DPDK
#
# Once done, this will define:
#  DPDK_FOUND        - System has DPDK
#  DPDK_INCLUDE_DIRS - The DPDK include directories
#  DPDK_LIBRARIES    - The DPDK library

# DPDK requires PkgConfig
find_package(PkgConfig REQUIRED)

# Set search path
if (DEFINED ENV{DPDK_DIR})
    file(TO_CMAKE_PATH "$ENV{DPDK_DIR}" DPDK_SEARCH_PATH)
    set(CMAKE_PREFIX_PATH "${DPDK_SEARCH_PATH}")
endif (DEFINED ENV{DPDK_DIR})

# Find DPDK
unset(DPDK_FOUND CACHE)
pkg_check_modules(DPDK libdpdk>=${DPDK_MIN_VERSION})

if (DPDK_FOUND)
    # In case a specific path was provided, check that the library was not relocated after installation.
    if (DEFINED ENV{DPDK_DIR})
        if (NOT ${DPDK_PREFIX} STREQUAL ${DPDK_SEARCH_PATH})
            message(WARNING
                    "DPDK prefix detected by pkg-config (${DPDK_PREFIX}) does not match the requested DPDK path "
                    "(${DPDK_SEARCH_PATH}), the resulting libraries and headers path might be incorrect. "
                    "Please check whether the library was relocated from its original installation directory")
        endif()
    endif(DEFINED ENV{DPDK_DIR})
endif (DPDK_FOUND)

if (DPDK_FOUND)
    set(DPDK_LIBRARIES ${DPDK_LDFLAGS})
    message(STATUS "DPDK LIBRARIES: " ${DPDK_LIBRARIES})
    message(STATUS "DPDK INCLUDE DIRS: ${DPDK_INCLUDE_DIRS}")
endif (DPDK_FOUND)
