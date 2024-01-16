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

# - Try to find yaml-cpp - a YAML parser for C++
# Once done this will define
#  YAMLCPP_FOUND - System has yaml-cpp
#  YAMLCPP_INCLUDE_DIR - The yaml-cpp include directories
#  YAMLCPP_LIBRARY - The library needed to use yaml-cpp

find_package(PkgConfig REQUIRED)
pkg_check_modules(PC_YAMLCPP REQUIRED yaml-cpp)
set(YAMLCPP_DEFINITIONS ${PC_YAMLCPP_CFLAGS_OTHER})

find_path(
        YAMLCPP_INCLUDE_DIR
        NAMES yaml-cpp/yaml.h
        PATHS /usr/local/include
        /usr/include
)

find_library(
        YAMLCPP_LIBRARY
        NAMES yaml-cpp libyaml-cpp
        HINTS ${PC_YAMLCPP_LIBDIR} ${PC_YAMLCPP_LIBRARY_DIRS}
        PATHS /usr/local/lib
        /usr/local/lib64
        /usr/lib
        /usr/lib64
        /usr/lib/x86_64-linux-gnu
)

message(STATUS "YAMLCPP LIBRARIES: " ${YAMLCPP_LIBRARY})
message(STATUS "YAMLCPP INCLUDE DIRS: " ${YAMLCPP_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(YAMLCPP DEFAULT_MSG YAMLCPP_LIBRARY YAMLCPP_INCLUDE_DIR)
mark_as_advanced(YAMLCPP_LIBRARY YAMLCPP_INCLUDE_DIR)
