#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(cell_meas_manager_helpers cell_meas_manager_test_helpers.cpp)
target_include_directories(cell_meas_manager_helpers PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(cell_meas_manager_helpers srsran_cu_cp_cell_meas_manager srsran_support srslog gtest gtest_main)

set(SOURCES
        cell_meas_manager_test.cpp)

add_executable(cell_meas_manager_test ${SOURCES})
target_include_directories(cell_meas_manager_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(cell_meas_manager_test cell_meas_manager_helpers cu_cp_test_helpers)
gtest_discover_tests(cell_meas_manager_test PROPERTIES "LABELS;cell_meas_manager")
