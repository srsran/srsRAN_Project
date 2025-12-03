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
