#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
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
