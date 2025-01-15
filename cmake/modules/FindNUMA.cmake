#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

find_package(PkgConfig REQUIRED)

find_path(
    NUMA_INCLUDE_DIRS
    NAMES numa.h numaif.h
    PATHS /usr/local/include
    /usr/include
)

find_library(
        NUMA_LIBRARIES
        NAMES numa
        PATHS /usr/local/lib
        /usr/lib
        /usr/lib/x86_64-linux-gnu
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(NUMA DEFAULT_MSG NUMA_LIBRARIES NUMA_INCLUDE_DIRS)
MARK_AS_ADVANCED(NUMA_LIBRARIES NUMA_INCLUDE_DIRS)
