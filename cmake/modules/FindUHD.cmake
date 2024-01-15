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

FIND_PACKAGE(PkgConfig REQUIRED)

#PKG_CHECK_MODULES(UHD uhd)
IF(NOT UHD_FOUND)

FIND_PATH(
    UHD_INCLUDE_DIRS
    NAMES uhd.h
    HINTS $ENV{UHD_DIR}/include
    PATHS /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    UHD_LIBRARIES
    NAMES uhd
    HINTS $ENV{UHD_DIR}/lib
    PATHS /usr/local/lib
          /usr/lib
          /usr/lib/x86_64-linux-gnu
          /usr/local/lib64
          /usr/local/lib32
)

message(STATUS "UHD LIBRARIES " ${UHD_LIBRARIES})
message(STATUS "UHD INCLUDE DIRS " ${UHD_INCLUDE_DIRS})

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(UHD DEFAULT_MSG UHD_LIBRARIES UHD_INCLUDE_DIRS)
MARK_AS_ADVANCED(UHD_LIBRARIES UHD_INCLUDE_DIRS)

include(CheckCXXSourceCompiles)

IF(UHD_FOUND)
  # UHD library directory
  get_filename_component(UHD_LIBRARY_DIR ${UHD_LIBRARIES} DIRECTORY)

  # Save current required variables
  set(_CMAKE_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS})
  set(_CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES})
  set(_CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES})

  # Set required variables
  set(CMAKE_REQUIRED_INCLUDES ${UHD_INCLUDE_DIRS})
  set(CMAKE_REQUIRED_FLAGS "${CMAKE_CXX_FLAGS} -L${UHD_LIBRARY_DIR}")
  set(CMAKE_REQUIRED_LIBRARIES uhd boost_program_options boost_system)

  # Recover required variables
  set(CMAKE_REQUIRED_FLAGS ${_CMAKE_REQUIRED_FLAGS})
  set(CMAKE_REQUIRED_LIBRARIES ${_CMAKE_REQUIRED_LIBRARIES})
  set(CMAKE_REQUIRED_INCLUDES ${_CMAKE_REQUIRED_INCLUDES})
ENDIF(UHD_FOUND)

ENDIF(NOT UHD_FOUND)
