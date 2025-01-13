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

# - Try to find armpl - the ARM performance library
# Once done this will define
#  ARMPL_FOUND - System has ARMPL
#  ARMPL_INCLUDE_DIRS - The ARMPL include directories
#  ARMPL_LIBRARIES - The libraries needed to use ARMPL
#  ARMPL_DEFINITIONS - Compiler switches required for using ARMPL

find_path(ARMPL_INCLUDE_DIR
            NAMES armpl.h
            HINTS $ENV{ARMPL_DIR}/include
                  /opt/arm/armpl_*/include/
                  /opt/arm/armpl/include/
                  /usr/inlude/armpl/
            PATHS)

find_path(ARMPL_FFTW_INCLUDE_DIR
            NAMES fftw3.h
            HINTS $ENV{ARMPL_DIR}/include/fftw3
                  /opt/arm/armpl_*/include/fftw3
                  /opt/arm/armpl/include/fftw3
                  /usr/inlude/armpl/fftw3
            PATHS)
find_library(ARMPL_LIBRARIES
            NAMES libarmpl_lp64.a
            HINTS $ENV{ARMPL_DIR}/lib
                  /opt/arm/armpl_*/lib/
                  /opt/arm/armpl/lib/
            PATHS)

set(ARMPL_STATIC_LIBRARIES ${ARMPL_LIBRARIES})
set(ARMPL_INCLUDE_DIRS ${ARMPL_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set ARMPL_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(ARMPL  DEFAULT_MSG
                                  ARMPL_LIBRARIES ARMPL_INCLUDE_DIRS)

if(ARMPL_FOUND)
  MESSAGE(STATUS "Found ARMPL_INCLUDE_DIRS: ${ARMPL_INCLUDE_DIRS}" )
  MESSAGE(STATUS "Found ARMPL_LIBRARIES: ${ARMPL_LIBRARIES}" )
  MESSAGE(STATUS "Found ARMPL_STATIC_LIBRARIES: ${ARMPL_STATIC_LIBRARIES}" )
endif(ARMPL_FOUND)

mark_as_advanced(ARMPL_INCLUDE_DIR ARMPL_FFTW_INCLUDE_DIR ARMPL_LIBRARIES)
