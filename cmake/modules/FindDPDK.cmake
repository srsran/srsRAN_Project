#
# Copyright 2021-2023 Software Radio Systems Limited
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

# Find DPDK
find_path(DPDK_INCLUDE_DIRS
        NAMES         rte_config.h
        PATH_SUFFIXES dpdk
        HINTS         $ENV{DPDK_DIR}/include
        PATHS         /usr/local/include
        )

# Set required DPDK components
set(DPDK_COMPONENTS
        acl bbdev bpf cmdline cryptodev dmadev eal efd ethdev eventdev graph hash ipsec ip_frag 
        jobstats latencystats kvargs lpm mbuf member mempool meter metrics net node pcapng pci 
        pdump pipeline port rawdev rcu ring sched security stack table telemetry timer vhost)

# Find DPDK libraries
foreach(c ${DPDK_COMPONENTS})
    find_library(DPDK_rte_${c}_LIBRARY rte_${c}
    HINTS $ENV{DPDK_DIR}/lib/*
    PATHS /usr/local/lib/*/dpdk)
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
