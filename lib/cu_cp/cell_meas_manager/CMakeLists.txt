#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
  cell_meas_manager_impl.cpp
  cell_meas_manager_helpers.cpp
)

add_library(srsran_cu_cp_cell_meas_manager STATIC ${SOURCES})
target_link_libraries(srsran_cu_cp_cell_meas_manager srsran_ran)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_to_exported_libs(srsran_cu_cp_cell_meas_manager)