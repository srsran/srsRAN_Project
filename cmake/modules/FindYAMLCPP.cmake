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

# - Try to find yaml-cpp - a YAML parser for C++
# Once done this will define
#  YAML_CPP_FOUND - System has yaml-cpp
#  YAML_CPP_INCLUDE_DIR - The yaml-cpp include directories
#  YAML_CPP_LIBRARY_DIR - Directory containing libraries
#  YAML_CPP_LIBRARIES - The libraries needed to use yaml-cpp
#  YAML_CPP_VERSION - Version of the yaml-cpp library

unset(YAML_CPP_FOUND)

# First try to locate yaml-cpp using the CMake config mode
find_package(yaml-cpp CONFIG QUIET)

if (yaml-cpp_FOUND)
    set(YAML_CPP_FOUND TRUE)
    set(YAML_CPP_VERSION ${yaml-cpp_VERSION})
else (yaml-cpp_FOUND)
    # Otherwise try to find yaml-cpp pkgconfig definition
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(PC_YAMLCPP QUIET yaml-cpp)

    if (PC_YAMLCPP_FOUND)
        set(YAML_CPP_FOUND TRUE)

        set(YAML_CPP_INCLUDE_DIR ${PC_YAMLCPP_INCLUDE_DIRS})
        set(YAML_CPP_LIBRARY_DIR ${PC_YAMLCPP_LIBRARY_DIRS})
        set(YAML_CPP_LIBRARIES ${PC_YAMLCPP_LIBRARIES})
        set(YAML_CPP_VERSION ${PC_YAMLCPP_VERSION})
    endif (PC_YAMLCPP_FOUND)
endif (yaml-cpp_FOUND)

if (YAML_CPP_FOUND)
    message(STATUS "Found yaml-cpp, version " ${YAML_CPP_VERSION})
    message(STATUS "YAMLCPP LIBRARIES: " ${YAML_CPP_LIBRARIES})
    message(STATUS "YAMLCPP INCLUDE DIRS: " ${YAML_CPP_INCLUDE_DIR})
    message(STATUS "YAMLCPP LIBRARY DIRS: " ${YAML_CPP_LIBRARY_DIR})
endif (YAML_CPP_FOUND)
