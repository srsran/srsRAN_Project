#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

# - Try to find ROHC library
#
# Once done this will define
#  ROHC_FOUND        - System has ROHC library installed
#  ROHC_INCLUDE_DIRS - The ROHC include directories
#  ROHC_LIBRARIES    - The ROHC library
#  ROHC_VERSION      - The ROHC library version

find_package(PkgConfig REQUIRED)
pkg_check_modules(ROHC rohc>=2.3.1)

if(ROHC_FOUND)
  message(STATUS "ROHC LIBRARIES: " ${ROHC_LIBRARIES})
  message(STATUS "ROHC INCLUDE DIRS: " ${ROHC_INCLUDE_DIRS})
endif()
