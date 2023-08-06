#
# Copyright 2021-2023 Software Radio Systems Limited
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

# Find DPDK
find_path(DPDK_INCLUDE_DIRS
        NAMES         rte_config.h
        PATH_SUFFIXES dpdk
        HINTS         /usr/local/include
        )

# Set required DPDK components
set(DPDK_COMPONENTS
        bbdev cmdline eal efd ethdev eventdev jobstats latencystats mbuf member mempool meter metrics net
        node pci pdump pipeline port rawdev rcu ring sched security stack table telemetry timer vhost)

# Find DPDK libraries
foreach(c ${DPDK_COMPONENTS})
    find_library(DPDK_rte_${c}_LIBRARY rte_${c}
            HINTS /usr/local/lib/*/dpdk)
endforeach()

foreach(c ${DPDK_COMPONENTS})
    list(APPEND CHECK_LIBRARIES "${DPDK_rte_${c}_LIBRARY}")
endforeach()

mark_as_advanced(DPDK_INCLUDE_DIRS ${CHECK_LIBRARIES})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(DPDK DEFAULT_MSG DPDK_INCLUDE_DIRS CHECK_LIBRARIES)

if(DPDK_FOUND)
    set(DPDK_LIBRARIES
            -Wl,--whole-archive ${CHECK_LIBRARIES} -lpthread -lnuma -ldl -Wl,--no-whole-archive)
endif(DPDK_FOUND)

message(STATUS "DPDK LIBRARIES: " ${DPDK_LIBRARIES})
message(STATUS "DPDK INCLUDE DIRS: " ${DPDK_INCLUDE_DIRS})
