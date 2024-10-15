#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
  up_resource_manager_impl.cpp
  up_resource_manager_helpers.cpp
)

add_library(srsran_up_resource_manager STATIC ${SOURCES})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
