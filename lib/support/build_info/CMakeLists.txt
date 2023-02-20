#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(srsran_build_info STATIC build_info.cpp)
include_directories(${CMAKE_BINARY_DIR})
add_custom_target(gen_build_info COMMAND cmake -P ${CMAKE_BINARY_DIR}/build_info.cmake)
add_dependencies(srsran_build_info gen_build_info)