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

find_package(PkgConfig REQUIRED)
pkg_check_modules(PC_ROHC rohc>=2.3.1)

#find Mbedtls
find_path(
    ROHC_INCLUDE_DIRS
    NAMES rohc/rohc.h
    HINTS $ENV{ROHC_DIR}/include
          ${PC_ROHC_INCLUDEDIR}
          ${CMAKE_INSTALL_PREFIX}/include
    PATHS /usr/local/include
          /usr/include
)

find_library(
    ROHC_LIBRARIES
    NAMES rohc
    HINTS $ENV{ROHC_DIR}/lib
          ${PC_ROHC_LIBDIR}
          ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
    PATHS /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ROHC DEFAULT_MSG ROHC_LIBRARIES ROHC_INCLUDE_DIRS)
mark_as_advanced(ROHC_LIBRARIES ROHC_INCLUDE_DIRS)

if(ROHC_FOUND)
  message(STATUS "ROHC LIBRARIES: " ${ROHC_LIBRARIES})
  message(STATUS "ROHC INCLUDE DIRS: " ${ROHC_INCLUDE_DIRS})
endif()
