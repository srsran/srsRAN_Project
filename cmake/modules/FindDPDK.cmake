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
