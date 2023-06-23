#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "cu_cp")

add_executable(cell_meas_manager_test cell_meas_manager_test.cpp)
target_link_libraries(cell_meas_manager_test srsran_cu_cp_cell_meas_manager srsran_support srslog gtest gtest_main)
gtest_discover_tests(cell_meas_manager_test)
