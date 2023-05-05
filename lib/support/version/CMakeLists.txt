#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/version_info.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/version_info.h
)
include_directories("${CMAKE_CURRENT_BINARY_DIR}")
add_library(srsran_version STATIC version.cpp)