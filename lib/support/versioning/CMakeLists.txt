#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

configure_file(version_info.h.in ${CMAKE_CURRENT_BINARY_DIR}/version_info.h)
include_directories(${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR})
add_library(srsran_versioning STATIC build_info.cpp version.cpp)
add_custom_target(gen_build_info COMMAND cmake -P ${CMAKE_BINARY_DIR}/build_info.cmake)
add_dependencies(srsran_versioning gen_build_info)
